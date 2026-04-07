#include <array>
#include <cstring>
#include <gtest/gtest.h>

#include <cbmpc/c_api/pve_base_pke.h>
#include <cbmpc/c_api/pve_batch_single_recipient.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/macros.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/crypto/base_ecc.h>

namespace {

using coinbase::buf_t;
using coinbase::mem_t;

struct pve_batch_ciphertext_blob_v1_t {
  uint32_t version = 1;
  uint32_t batch_count = 0;
  buf_t ct;

  void convert(coinbase::converter_t& c) { c.convert(version, batch_count, ct); }
};

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

static void expect_eq(cmem_t a, cmem_t b) {
  ASSERT_EQ(a.size, b.size);
  if (a.size > 0) {
    ASSERT_NE(a.data, nullptr);
    ASSERT_NE(b.data, nullptr);
    ASSERT_EQ(std::memcmp(a.data, b.data, static_cast<size_t>(a.size)), 0);
  }
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

}  // namespace

TEST(CApiPveBatch, EncVerDec_DefBasePke_RsaBlob) {
  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  constexpr int n = 4;
  std::array<uint8_t, static_cast<size_t>(n) * 32> xs_flat{};
  std::array<int, n> xs_sizes{};
  for (int i = 0; i < n; i++) {
    xs_sizes[static_cast<size_t>(i)] = 32;
    for (int j = 0; j < 32; j++) xs_flat[static_cast<size_t>(i * 32 + j)] = static_cast<uint8_t>(i + j);
  }
  const cmems_t xs_in = {n, xs_flat.data(), xs_sizes.data()};

  cmem_t ek_blob{nullptr, 0};
  cmem_t dk_blob{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_rsa_keypair(&ek_blob, &dk_blob), CBMPC_SUCCESS);

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_batch_encrypt(/*base_pke=*/nullptr, curve, ek_blob, label, xs_in, &ct), CBMPC_SUCCESS);

  int batch_count = 0;
  ASSERT_EQ(cbmpc_pve_batch_get_count(ct, &batch_count), CBMPC_SUCCESS);
  ASSERT_EQ(batch_count, n);

  cmem_t L_ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_batch_get_Label(ct, &L_ct), CBMPC_SUCCESS);
  expect_eq(L_ct, label);
  cbmpc_cmem_free(L_ct);

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

  ASSERT_EQ(cbmpc_pve_batch_verify(/*base_pke=*/nullptr, curve, ek_blob, ct, Qs_expected, label), CBMPC_SUCCESS);

  cmems_t xs_out{0, nullptr, nullptr};
  ASSERT_EQ(cbmpc_pve_batch_decrypt(/*base_pke=*/nullptr, curve, dk_blob, ek_blob, ct, label, &xs_out), CBMPC_SUCCESS);
  ASSERT_EQ(xs_out.count, n);

  int off = 0;
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(xs_out.sizes[i], 32);
    ASSERT_EQ(std::memcmp(xs_out.data + off, xs_flat.data() + i * 32, 32), 0);
    off += xs_out.sizes[i];
  }

  cbmpc_cmems_free(xs_out);
  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk_blob);
  cbmpc_cmem_free(ek_blob);
}

TEST(CApiPveBatch, EncryptVerifyDecrypt_CustomBasePke) {
  const cbmpc_pve_base_pke_t base_pke = {
      /*ctx=*/nullptr,
      /*encrypt=*/toy_encrypt,
      /*decrypt=*/toy_decrypt,
  };

  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t ek = {reinterpret_cast<uint8_t*>(const_cast<char*>("ek")), 2};
  const cmem_t dk = {reinterpret_cast<uint8_t*>(const_cast<char*>("dk")), 2};
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  constexpr int n = 3;
  std::array<uint8_t, static_cast<size_t>(n) * 32> xs_flat{};
  std::array<int, n> xs_sizes{};
  for (int i = 0; i < n; i++) {
    xs_sizes[static_cast<size_t>(i)] = 32;
    for (int j = 0; j < 32; j++) xs_flat[static_cast<size_t>(i * 32 + j)] = static_cast<uint8_t>(0x77 + i + j);
  }
  const cmems_t xs_in = {n, xs_flat.data(), xs_sizes.data()};

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_batch_encrypt(&base_pke, curve, ek, label, xs_in, &ct), CBMPC_SUCCESS);

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

  ASSERT_EQ(cbmpc_pve_batch_verify(&base_pke, curve, ek, ct, Qs_expected, label), CBMPC_SUCCESS);

  cmems_t xs_out{0, nullptr, nullptr};
  ASSERT_EQ(cbmpc_pve_batch_decrypt(&base_pke, curve, dk, ek, ct, label, &xs_out), CBMPC_SUCCESS);
  ASSERT_EQ(xs_out.count, n);

  int off = 0;
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(xs_out.sizes[i], 32);
    ASSERT_EQ(std::memcmp(xs_out.data + off, xs_flat.data() + i * 32, 32), 0);
    off += xs_out.sizes[i];
  }

  cbmpc_cmems_free(xs_out);
  cbmpc_cmem_free(ct);
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

#include <cbmpc/internal/core/log.h>

TEST(CApiPveBatchNeg, Encrypt) {
  dylog_disable_scope_t no_log;
  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek, &dk), CBMPC_SUCCESS);
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  int x_size = 32;
  const cmems_t xs = {1, x_bytes.data(), &x_size};
  cmem_t ct{nullptr, 0};

  EXPECT_EQ(cbmpc_pve_batch_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, xs, nullptr), E_BADARG);
  EXPECT_NE(cbmpc_pve_batch_encrypt(nullptr, static_cast<cbmpc_curve_id_t>(0), ek, label, xs, &ct), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_encrypt(nullptr, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, label, xs, &ct), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, cmem_t{nullptr, 0}, xs, &ct), CBMPC_SUCCESS);
  const cmems_t empty_xs = {0, nullptr, nullptr};
  EXPECT_NE(cbmpc_pve_batch_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, empty_xs, &ct), CBMPC_SUCCESS);

  cbmpc_cmem_free(dk);
  cbmpc_cmem_free(ek);
}

TEST(CApiPveBatchNeg, Verify) {
  dylog_disable_scope_t no_log;
  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek, &dk), CBMPC_SUCCESS);
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  int x_size = 32;
  const cmems_t xs = {1, x_bytes.data(), &x_size};
  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_batch_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, xs, &ct), CBMPC_SUCCESS);
  cmems_t Qs{0, nullptr, nullptr};
  ASSERT_EQ(cbmpc_pve_batch_get_Qs(ct, &Qs), CBMPC_SUCCESS);

  EXPECT_NE(cbmpc_pve_batch_verify(nullptr, static_cast<cbmpc_curve_id_t>(0), ek, ct, Qs, label), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_verify(nullptr, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, ct, Qs, label), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_verify(nullptr, CBMPC_CURVE_SECP256K1, ek, cmem_t{nullptr, 0}, Qs, label), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_verify(nullptr, CBMPC_CURVE_SECP256K1, ek, ct, Qs, cmem_t{nullptr, 0}), CBMPC_SUCCESS);

  cbmpc_cmems_free(Qs);
  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk);
  cbmpc_cmem_free(ek);
}

TEST(CApiPveBatchNeg, Decrypt) {
  dylog_disable_scope_t no_log;
  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek, &dk), CBMPC_SUCCESS);
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  int x_size = 32;
  const cmems_t xs = {1, x_bytes.data(), &x_size};
  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_batch_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, xs, &ct), CBMPC_SUCCESS);
  cmems_t xs_out{0, nullptr, nullptr};

  EXPECT_EQ(cbmpc_pve_batch_decrypt(nullptr, CBMPC_CURVE_SECP256K1, dk, ek, ct, label, nullptr), E_BADARG);
  EXPECT_NE(cbmpc_pve_batch_decrypt(nullptr, static_cast<cbmpc_curve_id_t>(0), dk, ek, ct, label, &xs_out),
            CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_decrypt(nullptr, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, ek, ct, label, &xs_out),
            CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_decrypt(nullptr, CBMPC_CURVE_SECP256K1, dk, cmem_t{nullptr, 0}, ct, label, &xs_out),
            CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_decrypt(nullptr, CBMPC_CURVE_SECP256K1, dk, ek, cmem_t{nullptr, 0}, label, &xs_out),
            CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_decrypt(nullptr, CBMPC_CURVE_SECP256K1, dk, ek, ct, cmem_t{nullptr, 0}, &xs_out),
            CBMPC_SUCCESS);

  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk);
  cbmpc_cmem_free(ek);
}

TEST(CApiPveBatchNeg, DecryptRejectsTamperedOuterBatchCount) {
  dylog_disable_scope_t no_log;

  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek, &dk), CBMPC_SUCCESS);

  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  int x_size = 32;
  const cmems_t xs = {1, x_bytes.data(), &x_size};

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_batch_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, xs, &ct), CBMPC_SUCCESS);

  pve_batch_ciphertext_blob_v1_t blob;
  ASSERT_EQ(coinbase::deser(mem_t(ct.data, ct.size), blob), SUCCESS);
  blob.batch_count = 2;
  const buf_t tampered = coinbase::convert(blob);

  cmems_t xs_out{0, nullptr, nullptr};
  EXPECT_NE(cbmpc_pve_batch_decrypt(nullptr, CBMPC_CURVE_SECP256K1, dk, ek,
                                    cmem_t{const_cast<uint8_t*>(tampered.data()), tampered.size()},
                                    label, &xs_out),
            CBMPC_SUCCESS);
  EXPECT_EQ(xs_out.count, 0);
  EXPECT_EQ(xs_out.data, nullptr);
  EXPECT_EQ(xs_out.sizes, nullptr);

  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk);
  cbmpc_cmem_free(ek);
}

TEST(CApiPveBatchNeg, GetCount) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  EXPECT_EQ(cbmpc_pve_batch_get_count(cmem_t{nullptr, 0}, nullptr), E_BADARG);
  int count = 0;
  EXPECT_NE(cbmpc_pve_batch_get_count(cmem_t{nullptr, 0}, &count), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_get_count(cmem_t{garbage, 4}, &count), CBMPC_SUCCESS);
}

TEST(CApiPveBatchNeg, GetQs) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  EXPECT_EQ(cbmpc_pve_batch_get_Qs(cmem_t{nullptr, 0}, nullptr), E_BADARG);
  cmems_t Qs{0, nullptr, nullptr};
  EXPECT_NE(cbmpc_pve_batch_get_Qs(cmem_t{nullptr, 0}, &Qs), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_get_Qs(cmem_t{garbage, 4}, &Qs), CBMPC_SUCCESS);
}

TEST(CApiPveBatchNeg, GetLabel) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  EXPECT_EQ(cbmpc_pve_batch_get_Label(cmem_t{nullptr, 0}, nullptr), E_BADARG);
  cmem_t label{nullptr, 0};
  EXPECT_NE(cbmpc_pve_batch_get_Label(cmem_t{nullptr, 0}, &label), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_batch_get_Label(cmem_t{garbage, 4}, &label), CBMPC_SUCCESS);
}
