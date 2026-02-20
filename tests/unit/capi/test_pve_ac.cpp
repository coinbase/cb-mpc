#include <array>
#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <unordered_map>

#include <cbmpc/capi/access_structure.h>
#include <cbmpc/capi/pve.h>
#include <cbmpc/capi/pve_ac.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/macros.h>
#include <cbmpc/internal/crypto/base_ecc.h>

namespace {

using coinbase::buf_t;
using coinbase::mem_t;

struct fake_hsm_ecies_p256_t {
  std::unordered_map<std::string, coinbase::crypto::ecc_prv_key_t> keys;
};

static cbmpc_error_t hsm_ecies_p256_ecdh_cb(void* ctx, cmem_t dk_handle, cmem_t kem_ct, cmem_t* out_dh_x32) {
  if (!out_dh_x32) return E_BADARG;
  *out_dh_x32 = cmem_t{nullptr, 0};
  if (!ctx) return E_BADARG;
  if (dk_handle.size <= 0 || !dk_handle.data) return E_BADARG;
  if (kem_ct.size < 0) return E_BADARG;
  if (kem_ct.size > 0 && !kem_ct.data) return E_BADARG;

  const auto* hsm = static_cast<const fake_hsm_ecies_p256_t*>(ctx);
  const std::string handle(reinterpret_cast<const char*>(dk_handle.data), static_cast<size_t>(dk_handle.size));
  const auto it = hsm->keys.find(handle);
  if (it == hsm->keys.end()) return E_BADARG;

  coinbase::crypto::ecc_point_t E;
  coinbase::error_t rv = E.from_oct(coinbase::crypto::curve_p256, mem_t(kem_ct.data, kem_ct.size));
  if (rv) return rv;
  if (rv = coinbase::crypto::curve_p256.check(E)) return rv;

  const buf_t dh = it->second.ecdh(E);
  if (dh.size() != 32) return CBMPC_E_CRYPTO;

  out_dh_x32->data = static_cast<uint8_t*>(cbmpc_malloc(32));
  if (!out_dh_x32->data) return E_INSUFFICIENT;
  out_dh_x32->size = 32;
  std::memmove(out_dh_x32->data, dh.data(), 32);
  return CBMPC_SUCCESS;
}

static buf_t expected_Q(cbmpc_curve_id_t curve_id, mem_t x) {
  const coinbase::crypto::ecurve_t curve = (curve_id == CBMPC_CURVE_P256)        ? coinbase::crypto::curve_p256
                                           : (curve_id == CBMPC_CURVE_SECP256K1) ? coinbase::crypto::curve_secp256k1
                                           : (curve_id == CBMPC_CURVE_ED25519)   ? coinbase::crypto::curve_ed25519
                                                                                 : coinbase::crypto::ecurve_t();
  cb_assert(curve.valid());

  const coinbase::crypto::bn_t bn_x = coinbase::crypto::bn_t::from_bin(x) % curve.order();
  const coinbase::crypto::ecc_point_t Q = bn_x * curve.generator();
  return Q.to_compressed_bin();
}

static cbmpc_error_t toy_encrypt(void* /*ctx*/, cmem_t /*ek*/, cmem_t /*label*/, cmem_t plain, cmem_t /*rho*/,
                                 cmem_t* out_ct) {
  if (!out_ct) return E_BADARG;
  *out_ct = cmem_t{nullptr, 0};
  if (plain.size < 0) return E_BADARG;
  if (plain.size > 0 && !plain.data) return E_BADARG;

  const int n = plain.size;
  if (n == 0) return CBMPC_SUCCESS;

  out_ct->data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(n)));
  if (!out_ct->data) return E_INSUFFICIENT;
  out_ct->size = n;
  std::memmove(out_ct->data, plain.data, static_cast<size_t>(n));
  return CBMPC_SUCCESS;
}

static cbmpc_error_t toy_decrypt(void* /*ctx*/, cmem_t /*dk*/, cmem_t /*label*/, cmem_t ct, cmem_t* out_plain) {
  if (!out_plain) return E_BADARG;
  *out_plain = cmem_t{nullptr, 0};
  if (ct.size < 0) return E_BADARG;
  if (ct.size > 0 && !ct.data) return E_BADARG;

  const int n = ct.size;
  if (n == 0) return CBMPC_SUCCESS;

  out_plain->data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(n)));
  if (!out_plain->data) return E_INSUFFICIENT;
  out_plain->size = n;
  std::memmove(out_plain->data, ct.data, static_cast<size_t>(n));
  return CBMPC_SUCCESS;
}

static void expect_eq(cmem_t a, cmem_t b) {
  ASSERT_EQ(a.size, b.size);
  if (a.size > 0) {
    ASSERT_NE(a.data, nullptr);
    ASSERT_NE(b.data, nullptr);
    ASSERT_EQ(std::memcmp(a.data, b.data, static_cast<size_t>(a.size)), 0);
  }
}

}  // namespace

TEST(CApiPveAc, EncVer_PDec_Agg_DefPke_Rsa) {
  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  // Access structure: THRESHOLD(2-of-3) over leaves {p1,p2,p3}.
  const char* p1 = "p1";
  const char* p2 = "p2";
  const char* p3 = "p3";

  const std::array<int32_t, 3> child_indices = {1, 2, 3};
  const std::array<cbmpc_access_structure_node_t, 4> nodes = {
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, /*leaf_name=*/nullptr, /*k=*/2,
                                    /*off=*/0, /*cnt=*/3},
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_LEAF, p1, /*k=*/0, /*off=*/0, /*cnt=*/0},
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_LEAF, p2, /*k=*/0, /*off=*/0, /*cnt=*/0},
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_LEAF, p3, /*k=*/0, /*off=*/0, /*cnt=*/0},
  };

  const cbmpc_access_structure_t ac = {
      /*nodes=*/nodes.data(),
      /*nodes_count=*/static_cast<int32_t>(nodes.size()),
      /*child_indices=*/child_indices.data(),
      /*child_indices_count=*/static_cast<int32_t>(child_indices.size()),
      /*root_index=*/0,
  };

  constexpr int n = 4;
  std::array<uint8_t, static_cast<size_t>(n) * 32> xs_flat{};
  std::array<int, n> xs_sizes{};
  for (int i = 0; i < n; i++) {
    xs_sizes[static_cast<size_t>(i)] = 32;
    for (int j = 0; j < 32; j++) xs_flat[static_cast<size_t>(i * 32 + j)] = static_cast<uint8_t>(i + j);
  }
  const cmems_t xs_in = {n, xs_flat.data(), xs_sizes.data()};

  // Per-leaf base-PKE key blobs.
  std::array<cmem_t, 3> eks = {cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}};
  std::array<cmem_t, 3> dks = {cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_rsa_keypair(&eks[0], &dks[0]), CBMPC_SUCCESS);
  ASSERT_EQ(cbmpc_pve_generate_base_pke_rsa_keypair(&eks[1], &dks[1]), CBMPC_SUCCESS);
  ASSERT_EQ(cbmpc_pve_generate_base_pke_rsa_keypair(&eks[2], &dks[2]), CBMPC_SUCCESS);

  const std::array<const char*, 3> leaf_names = {p1, p2, p3};

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_ac_encrypt(/*base_pke=*/nullptr, curve, &ac, leaf_names.data(), eks.data(),
                                 static_cast<int>(eks.size()), label, xs_in, &ct),
            CBMPC_SUCCESS);

  int batch_count = 0;
  ASSERT_EQ(cbmpc_pve_ac_get_count(ct, &batch_count), CBMPC_SUCCESS);
  ASSERT_EQ(batch_count, n);

  // Expected Qs.
  std::array<uint8_t, static_cast<size_t>(n) * 33> Qs_flat{};
  std::array<int, n> Qs_sizes{};
  for (int i = 0; i < n; i++) {
    const mem_t xi(xs_flat.data() + i * 32, 32);
    const buf_t qi = expected_Q(curve, xi);
    ASSERT_EQ(qi.size(), 33);
    Qs_sizes[static_cast<size_t>(i)] = qi.size();
    std::memmove(Qs_flat.data() + i * 33, qi.data(), static_cast<size_t>(qi.size()));
  }
  const cmems_t Qs_expected = {n, Qs_flat.data(), Qs_sizes.data()};

  ASSERT_EQ(cbmpc_pve_ac_verify(/*base_pke=*/nullptr, curve, &ac, leaf_names.data(), eks.data(),
                                static_cast<int>(eks.size()), ct, Qs_expected, label),
            CBMPC_SUCCESS);

  const int row_index = 0;
  cmem_t share_p1{nullptr, 0};
  cmem_t share_p2{nullptr, 0};
  ASSERT_EQ(
      cbmpc_pve_ac_party_decrypt_row(/*base_pke=*/nullptr, curve, &ac, ct, row_index, p1, dks[0], label, &share_p1),
      CBMPC_SUCCESS);
  ASSERT_EQ(
      cbmpc_pve_ac_party_decrypt_row(/*base_pke=*/nullptr, curve, &ac, ct, row_index, p2, dks[1], label, &share_p2),
      CBMPC_SUCCESS);

  const std::array<const char*, 2> quorum_names = {p1, p2};
  const std::array<cmem_t, 2> quorum_shares = {share_p1, share_p2};

  cmems_t xs_out{0, nullptr, nullptr};
  ASSERT_EQ(
      cbmpc_pve_ac_aggregate_to_restore_row(/*base_pke=*/nullptr, curve, &ac, quorum_names.data(), quorum_shares.data(),
                                            static_cast<int>(quorum_shares.size()), ct, row_index, label, &xs_out),
      CBMPC_SUCCESS);
  ASSERT_EQ(xs_out.count, n);

  int off = 0;
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(xs_out.sizes[i], 32);
    ASSERT_EQ(std::memcmp(xs_out.data + off, xs_flat.data() + i * 32, 32), 0);
    off += xs_out.sizes[i];
  }

  // Insufficient quorum should fail.
  const std::array<const char*, 1> q1_names = {p1};
  const std::array<cmem_t, 1> q1_shares = {share_p1};
  cmems_t xs_out2{0, nullptr, nullptr};
  EXPECT_NE(cbmpc_pve_ac_aggregate_to_restore_row(/*base_pke=*/nullptr, curve, &ac, q1_names.data(), q1_shares.data(),
                                                  static_cast<int>(q1_shares.size()), ct, row_index, label, &xs_out2),
            CBMPC_SUCCESS);

  cbmpc_cmem_free(share_p2);
  cbmpc_cmem_free(share_p1);
  cbmpc_cmems_free(xs_out);
  cbmpc_cmem_free(ct);
  for (int i = 0; i < 3; i++) {
    cbmpc_cmem_free(dks[static_cast<size_t>(i)]);
    cbmpc_cmem_free(eks[static_cast<size_t>(i)]);
  }
}

TEST(CApiPveAc, EncVer_PartDec_Agg_CustomBasePke) {
  const cbmpc_pve_base_pke_t base_pke = {
      /*ctx=*/nullptr,
      /*encrypt=*/toy_encrypt,
      /*decrypt=*/toy_decrypt,
  };

  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  const char* p1 = "p1";
  const char* p2 = "p2";
  const char* p3 = "p3";
  const std::array<int32_t, 3> child_indices = {1, 2, 3};
  const std::array<cbmpc_access_structure_node_t, 4> nodes = {
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, /*leaf_name=*/nullptr, /*k=*/2,
                                    /*off=*/0, /*cnt=*/3},
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_LEAF, p1, /*k=*/0, /*off=*/0, /*cnt=*/0},
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_LEAF, p2, /*k=*/0, /*off=*/0, /*cnt=*/0},
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_LEAF, p3, /*k=*/0, /*off=*/0, /*cnt=*/0},
  };
  const cbmpc_access_structure_t ac = {
      /*nodes=*/nodes.data(),
      /*nodes_count=*/static_cast<int32_t>(nodes.size()),
      /*child_indices=*/child_indices.data(),
      /*child_indices_count=*/static_cast<int32_t>(child_indices.size()),
      /*root_index=*/0,
  };

  constexpr int n = 3;
  std::array<uint8_t, static_cast<size_t>(n) * 32> xs_flat{};
  std::array<int, n> xs_sizes{};
  for (int i = 0; i < n; i++) {
    xs_sizes[static_cast<size_t>(i)] = 32;
    for (int j = 0; j < 32; j++) xs_flat[static_cast<size_t>(i * 32 + j)] = static_cast<uint8_t>(0x77 + i + j);
  }
  const cmems_t xs_in = {n, xs_flat.data(), xs_sizes.data()};

  const cmem_t ek1 = {reinterpret_cast<uint8_t*>(const_cast<char*>("ek1")), 3};
  const cmem_t ek2 = {reinterpret_cast<uint8_t*>(const_cast<char*>("ek2")), 3};
  const cmem_t ek3 = {reinterpret_cast<uint8_t*>(const_cast<char*>("ek3")), 3};
  const std::array<cmem_t, 3> eks = {ek1, ek2, ek3};
  const std::array<const char*, 3> leaf_names = {p1, p2, p3};

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_ac_encrypt(&base_pke, curve, &ac, leaf_names.data(), eks.data(), static_cast<int>(eks.size()),
                                 label, xs_in, &ct),
            CBMPC_SUCCESS);

  // Verify expected Qs.
  std::array<uint8_t, static_cast<size_t>(n) * 33> Qs_flat{};
  std::array<int, n> Qs_sizes{};
  for (int i = 0; i < n; i++) {
    const mem_t xi(xs_flat.data() + i * 32, 32);
    const buf_t qi = expected_Q(curve, xi);
    ASSERT_EQ(qi.size(), 33);
    Qs_sizes[static_cast<size_t>(i)] = qi.size();
    std::memmove(Qs_flat.data() + i * 33, qi.data(), static_cast<size_t>(qi.size()));
  }
  const cmems_t Qs_expected = {n, Qs_flat.data(), Qs_sizes.data()};

  ASSERT_EQ(cbmpc_pve_ac_verify(&base_pke, curve, &ac, leaf_names.data(), eks.data(), static_cast<int>(eks.size()), ct,
                                Qs_expected, label),
            CBMPC_SUCCESS);

  const int row_index = 0;
  cmem_t share_p1{nullptr, 0};
  cmem_t share_p3{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_ac_party_decrypt_row(&base_pke, curve, &ac, ct, row_index, p1, ek1, label, &share_p1),
            CBMPC_SUCCESS);
  ASSERT_EQ(cbmpc_pve_ac_party_decrypt_row(&base_pke, curve, &ac, ct, row_index, p3, ek3, label, &share_p3),
            CBMPC_SUCCESS);

  const std::array<const char*, 2> quorum_names = {p1, p3};
  const std::array<cmem_t, 2> quorum_shares = {share_p1, share_p3};

  cmems_t xs_out{0, nullptr, nullptr};
  ASSERT_EQ(
      cbmpc_pve_ac_aggregate_to_restore_row(&base_pke, curve, &ac, quorum_names.data(), quorum_shares.data(),
                                            static_cast<int>(quorum_shares.size()), ct, row_index, label, &xs_out),
      CBMPC_SUCCESS);
  ASSERT_EQ(xs_out.count, n);

  int off = 0;
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(xs_out.sizes[i], 32);
    ASSERT_EQ(std::memcmp(xs_out.data + off, xs_flat.data() + i * 32, 32), 0);
    off += xs_out.sizes[i];
  }

  cbmpc_cmem_free(share_p3);
  cbmpc_cmem_free(share_p1);
  cbmpc_cmems_free(xs_out);
  cbmpc_cmem_free(ct);
}

TEST(CApiPveAc, EncVer_PDec_Agg_DefPke_EciesHsmRow) {
  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  // Access structure: THRESHOLD(2-of-3) over leaves {p1,p2,p3}.
  const char* p1 = "p1";
  const char* p2 = "p2";
  const char* p3 = "p3";
  const std::array<int32_t, 3> child_indices = {1, 2, 3};
  const std::array<cbmpc_access_structure_node_t, 4> nodes = {
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, /*leaf_name=*/nullptr, /*k=*/2,
                                    /*off=*/0, /*cnt=*/3},
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_LEAF, p1, /*k=*/0, /*off=*/0, /*cnt=*/0},
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_LEAF, p2, /*k=*/0, /*off=*/0, /*cnt=*/0},
      cbmpc_access_structure_node_t{CBMPC_ACCESS_STRUCTURE_NODE_LEAF, p3, /*k=*/0, /*off=*/0, /*cnt=*/0},
  };
  const cbmpc_access_structure_t ac = {
      /*nodes=*/nodes.data(),
      /*nodes_count=*/static_cast<int32_t>(nodes.size()),
      /*child_indices=*/child_indices.data(),
      /*child_indices_count=*/static_cast<int32_t>(child_indices.size()),
      /*root_index=*/0,
  };

  constexpr int n = 4;
  std::array<uint8_t, static_cast<size_t>(n) * 32> xs_flat{};
  std::array<int, n> xs_sizes{};
  for (int i = 0; i < n; i++) {
    xs_sizes[static_cast<size_t>(i)] = 32;
    for (int j = 0; j < 32; j++) xs_flat[static_cast<size_t>(i * 32 + j)] = static_cast<uint8_t>(0x55 + i + j);
  }
  const cmems_t xs_in = {n, xs_flat.data(), xs_sizes.data()};

  // Simulated HSM that stores P-256 private keys. The library only sees opaque handles.
  fake_hsm_ecies_p256_t hsm;
  std::array<std::string, 3> handles = {"hsm-ecies-p256-p1", "hsm-ecies-p256-p2", "hsm-ecies-p256-p3"};

  std::array<cmem_t, 3> eks = {cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}};
  for (int i = 0; i < 3; i++) {
    coinbase::crypto::ecc_prv_key_t prv;
    prv.generate(coinbase::crypto::curve_p256);

    const buf_t pub_oct = prv.pub().to_oct();
    ASSERT_EQ(pub_oct.size(), 65);

    const cmem_t pub_oct_mem = {pub_oct.data(), pub_oct.size()};
    ASSERT_EQ(cbmpc_pve_base_pke_ecies_p256_ek_from_oct(pub_oct_mem, &eks[static_cast<size_t>(i)]), CBMPC_SUCCESS);
    ASSERT_GT(eks[static_cast<size_t>(i)].size, 0);

    hsm.keys.emplace(handles[static_cast<size_t>(i)], std::move(prv));
  }

  const std::array<const char*, 3> leaf_names = {p1, p2, p3};

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_ac_encrypt(/*base_pke=*/nullptr, curve, &ac, leaf_names.data(), eks.data(),
                                 static_cast<int>(eks.size()), label, xs_in, &ct),
            CBMPC_SUCCESS);

  // Verify expected Qs.
  std::array<uint8_t, static_cast<size_t>(n) * 33> Qs_flat{};
  std::array<int, n> Qs_sizes{};
  for (int i = 0; i < n; i++) {
    const mem_t xi(xs_flat.data() + i * 32, 32);
    const buf_t qi = expected_Q(curve, xi);
    ASSERT_EQ(qi.size(), 33);
    Qs_sizes[static_cast<size_t>(i)] = qi.size();
    std::memmove(Qs_flat.data() + i * 33, qi.data(), static_cast<size_t>(qi.size()));
  }
  const cmems_t Qs_expected = {n, Qs_flat.data(), Qs_sizes.data()};

  ASSERT_EQ(cbmpc_pve_ac_verify(/*base_pke=*/nullptr, curve, &ac, leaf_names.data(), eks.data(),
                                static_cast<int>(eks.size()), ct, Qs_expected, label),
            CBMPC_SUCCESS);

  const cbmpc_pve_ecies_p256_hsm_ecdh_t cb = {
      /*ctx=*/&hsm,
      /*ecdh=*/hsm_ecies_p256_ecdh_cb,
  };

  const int row_index = 0;
  const cmem_t h1 = {reinterpret_cast<uint8_t*>(handles[0].data()), static_cast<int>(handles[0].size())};
  const cmem_t h3 = {reinterpret_cast<uint8_t*>(handles[2].data()), static_cast<int>(handles[2].size())};

  cmem_t share_p1{nullptr, 0};
  cmem_t share_p3{nullptr, 0};
  ASSERT_EQ(
      cbmpc_pve_ac_party_decrypt_row_ecies_p256_hsm(curve, &ac, ct, row_index, p1, h1, eks[0], label, &cb, &share_p1),
      CBMPC_SUCCESS);
  ASSERT_EQ(
      cbmpc_pve_ac_party_decrypt_row_ecies_p256_hsm(curve, &ac, ct, row_index, p3, h3, eks[2], label, &cb, &share_p3),
      CBMPC_SUCCESS);

  const std::array<const char*, 2> quorum_names = {p1, p3};
  const std::array<cmem_t, 2> quorum_shares = {share_p1, share_p3};

  cmems_t xs_out{0, nullptr, nullptr};
  ASSERT_EQ(
      cbmpc_pve_ac_aggregate_to_restore_row(/*base_pke=*/nullptr, curve, &ac, quorum_names.data(), quorum_shares.data(),
                                            static_cast<int>(quorum_shares.size()), ct, row_index, label, &xs_out),
      CBMPC_SUCCESS);
  ASSERT_EQ(xs_out.count, n);

  int off = 0;
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(xs_out.sizes[i], 32);
    ASSERT_EQ(std::memcmp(xs_out.data + off, xs_flat.data() + i * 32, 32), 0);
    off += xs_out.sizes[i];
  }

  cbmpc_cmem_free(share_p3);
  cbmpc_cmem_free(share_p1);
  cbmpc_cmems_free(xs_out);
  cbmpc_cmem_free(ct);
  for (int i = 0; i < 3; i++) cbmpc_cmem_free(eks[static_cast<size_t>(i)]);
}
