#include <array>
#include <cstring>
#include <gtest/gtest.h>

#include <cbmpc/c_api/pve_base_pke.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/crypto/base_ecc.h>
#include <cbmpc/internal/crypto/base_rsa.h>

namespace {

using coinbase::buf_t;
using coinbase::mem_t;

static cbmpc_error_t toy_encrypt(void* ctx, cmem_t /*ek*/, cmem_t /*label*/, cmem_t plain, cmem_t /*rho*/,
                                 cmem_t* out_ct) {
  if (!out_ct) return E_BADARG;
  *out_ct = cmem_t{nullptr, 0};
  if (plain.size < 0) return E_BADARG;
  if (plain.size > 0 && !plain.data) return E_BADARG;

  const int mode = ctx ? *static_cast<const int*>(ctx) : 0;
  const int extra = (mode == 0) ? 0 : 1;
  const int n = plain.size + extra;
  if (n == 0) return CBMPC_SUCCESS;

  out_ct->data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(n)));
  if (!out_ct->data) return E_INSUFFICIENT;
  out_ct->size = n;
  if (plain.size) std::memmove(out_ct->data, plain.data, static_cast<size_t>(plain.size));
  if (extra) out_ct->data[n - 1] = 0x42;
  return CBMPC_SUCCESS;
}

static cbmpc_error_t toy_decrypt(void* ctx, cmem_t /*dk*/, cmem_t /*label*/, cmem_t ct, cmem_t* out_plain) {
  if (!out_plain) return E_BADARG;
  *out_plain = cmem_t{nullptr, 0};
  if (ct.size < 0) return E_BADARG;
  if (ct.size > 0 && !ct.data) return E_BADARG;

  const int mode = ctx ? *static_cast<const int*>(ctx) : 0;
  const int extra = (mode == 0) ? 0 : 1;
  if (ct.size < extra) return E_FORMAT;

  const int n = ct.size - extra;
  if (n == 0) return CBMPC_SUCCESS;

  out_plain->data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(n)));
  if (!out_plain->data) return E_INSUFFICIENT;
  out_plain->size = n;
  std::memmove(out_plain->data, ct.data, static_cast<size_t>(n));
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

static void expect_eq(cmem_t a, cmem_t b) {
  ASSERT_EQ(a.size, b.size);
  if (a.size > 0) {
    ASSERT_NE(a.data, nullptr);
    ASSERT_NE(b.data, nullptr);
    ASSERT_EQ(std::memcmp(a.data, b.data, static_cast<size_t>(a.size)), 0);
  }
}

// Mirror of the cbmpc base-PKE key blob format used by `cbmpc_pve_generate_base_pke_*`.
// Test-only plumbing to build HSM stubs using software keys.
constexpr uint32_t base_pke_key_blob_version_v1 = 1;
enum class base_pke_key_type_v1 : uint32_t {
  rsa_oaep_2048 = 1,
  ecies_p256 = 2,
};

struct base_pke_dk_blob_v1_t {
  uint32_t version = base_pke_key_blob_version_v1;
  uint32_t key_type = static_cast<uint32_t>(base_pke_key_type_v1::rsa_oaep_2048);

  coinbase::crypto::rsa_prv_key_t rsa_dk;
  coinbase::crypto::ecc_prv_key_t ecies_dk;

  void convert(coinbase::converter_t& c) {
    c.convert(version, key_type);
    switch (static_cast<base_pke_key_type_v1>(key_type)) {
      case base_pke_key_type_v1::rsa_oaep_2048:
        c.convert(rsa_dk);
        return;
      case base_pke_key_type_v1::ecies_p256:
        c.convert(ecies_dk);
        return;
      default:
        c.set_error();
        return;
    }
  }
};

static cbmpc_error_t parse_rsa_prv_from_dk_blob(cmem_t dk_blob, coinbase::crypto::rsa_prv_key_t& out_sk) {
  base_pke_dk_blob_v1_t blob;
  const coinbase::error_t rv = coinbase::convert(blob, mem_t(dk_blob.data, dk_blob.size));
  if (rv) return rv;
  if (blob.version != base_pke_key_blob_version_v1) return E_FORMAT;
  if (static_cast<base_pke_key_type_v1>(blob.key_type) != base_pke_key_type_v1::rsa_oaep_2048) return E_BADARG;
  out_sk = blob.rsa_dk;
  return CBMPC_SUCCESS;
}

static cbmpc_error_t parse_ecies_prv_from_dk_blob(cmem_t dk_blob, coinbase::crypto::ecc_prv_key_t& out_sk) {
  base_pke_dk_blob_v1_t blob;
  const coinbase::error_t rv = coinbase::convert(blob, mem_t(dk_blob.data, dk_blob.size));
  if (rv) return rv;
  if (blob.version != base_pke_key_blob_version_v1) return E_FORMAT;
  if (static_cast<base_pke_key_type_v1>(blob.key_type) != base_pke_key_type_v1::ecies_p256) return E_BADARG;
  out_sk = blob.ecies_dk;
  return CBMPC_SUCCESS;
}

}  // namespace

TEST(CApiPve, EncryptVerifyDecrypt_CustomBasePke) {
  int mode0 = 0;
  const cbmpc_pve_base_pke_t base_pke0 = {
      /*ctx=*/&mode0,
      /*encrypt=*/toy_encrypt,
      /*decrypt=*/toy_decrypt,
  };

  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t ek = {reinterpret_cast<uint8_t*>(const_cast<char*>("ek")), 2};
  const cmem_t dk = {reinterpret_cast<uint8_t*>(const_cast<char*>("dk")), 2};
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i);
  const cmem_t x = {x_bytes.data(), static_cast<int>(x_bytes.size())};

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt(&base_pke0, curve, ek, label, x, &ct), CBMPC_SUCCESS);
  ASSERT_GT(ct.size, 0);

  cmem_t Q_ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_get_Q(ct, &Q_ct), CBMPC_SUCCESS);

  cmem_t L_ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_get_Label(ct, &L_ct), CBMPC_SUCCESS);
  expect_eq(L_ct, label);

  const buf_t Q_expected_buf = expected_Q(curve, mem_t(x.data, x.size));
  ASSERT_EQ(Q_ct.size, Q_expected_buf.size());
  ASSERT_EQ(std::memcmp(Q_ct.data, Q_expected_buf.data(), static_cast<size_t>(Q_ct.size)), 0);

  ASSERT_EQ(cbmpc_pve_verify(&base_pke0, curve, ek, ct, Q_ct, label), CBMPC_SUCCESS);

  cmem_t x_out{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_decrypt(&base_pke0, curve, dk, ek, ct, label, &x_out), CBMPC_SUCCESS);
  ASSERT_EQ(x_out.size, 32);
  ASSERT_EQ(std::memcmp(x_out.data, x.data, 32), 0);

  cbmpc_cmem_free(x_out);
  cbmpc_cmem_free(L_ct);
  cbmpc_cmem_free(Q_ct);
  cbmpc_cmem_free(ct);
}

TEST(CApiPve, EncVerDec_DefBasePke_EciesBlob) {
  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0xB0 + i);
  const cmem_t x = {x_bytes.data(), static_cast<int>(x_bytes.size())};

  cmem_t ek_blob{nullptr, 0};
  cmem_t dk_blob{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek_blob, &dk_blob), CBMPC_SUCCESS);

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt(/*base_pke=*/nullptr, curve, ek_blob, label, x, &ct), CBMPC_SUCCESS);

  const buf_t Q_expected_buf = expected_Q(curve, mem_t(x.data, x.size));
  const cmem_t Q_expected = {const_cast<uint8_t*>(Q_expected_buf.data()), Q_expected_buf.size()};
  ASSERT_EQ(cbmpc_pve_verify(/*base_pke=*/nullptr, curve, ek_blob, ct, Q_expected, label), CBMPC_SUCCESS);

  cmem_t x_out{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_decrypt(/*base_pke=*/nullptr, curve, dk_blob, ek_blob, ct, label, &x_out), CBMPC_SUCCESS);
  ASSERT_EQ(x_out.size, 32);
  ASSERT_EQ(std::memcmp(x_out.data, x.data, 32), 0);

  cbmpc_cmem_free(x_out);
  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk_blob);
  cbmpc_cmem_free(ek_blob);
}

TEST(CApiPve, EncVerDec_DefBasePke_RsaBlob) {
  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0xC0 + i);
  const cmem_t x = {x_bytes.data(), static_cast<int>(x_bytes.size())};

  cmem_t ek_blob{nullptr, 0};
  cmem_t dk_blob{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_rsa_keypair(&ek_blob, &dk_blob), CBMPC_SUCCESS);

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt(/*base_pke=*/nullptr, curve, ek_blob, label, x, &ct), CBMPC_SUCCESS);

  const buf_t Q_expected_buf = expected_Q(curve, mem_t(x.data, x.size));
  const cmem_t Q_expected = {const_cast<uint8_t*>(Q_expected_buf.data()), Q_expected_buf.size()};
  ASSERT_EQ(cbmpc_pve_verify(/*base_pke=*/nullptr, curve, ek_blob, ct, Q_expected, label), CBMPC_SUCCESS);

  cmem_t x_out{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_decrypt(/*base_pke=*/nullptr, curve, dk_blob, ek_blob, ct, label, &x_out), CBMPC_SUCCESS);
  ASSERT_EQ(x_out.size, 32);
  ASSERT_EQ(std::memcmp(x_out.data, x.data, 32), 0);

  cbmpc_cmem_free(x_out);
  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk_blob);
  cbmpc_cmem_free(ek_blob);
}

static cbmpc_error_t rsa_oaep_hsm_decap_cb(void* ctx, cmem_t /*dk_handle*/, cmem_t kem_ct, cmem_t* out_kem_ss) {
  if (!ctx || !out_kem_ss) return E_BADARG;
  *out_kem_ss = cmem_t{nullptr, 0};
  auto* sk = static_cast<coinbase::crypto::rsa_prv_key_t*>(ctx);

  coinbase::buf_t kem_ss;
  const coinbase::error_t rv = sk->decrypt_oaep(mem_t(kem_ct.data, kem_ct.size), coinbase::crypto::hash_e::sha256,
                                                coinbase::crypto::hash_e::sha256, mem_t(), kem_ss);
  if (rv) return rv;

  out_kem_ss->data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(kem_ss.size())));
  if (!out_kem_ss->data) return E_INSUFFICIENT;
  out_kem_ss->size = kem_ss.size();
  std::memmove(out_kem_ss->data, kem_ss.data(), static_cast<size_t>(kem_ss.size()));
  return CBMPC_SUCCESS;
}

TEST(CApiPve, DecryptRsaOaepHsm_UsesCallback) {
  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x44 + i);
  const cmem_t x = {x_bytes.data(), static_cast<int>(x_bytes.size())};

  cmem_t ek_blob{nullptr, 0};
  cmem_t dk_blob{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_rsa_keypair(&ek_blob, &dk_blob), CBMPC_SUCCESS);

  coinbase::crypto::rsa_prv_key_t sk;
  ASSERT_EQ(parse_rsa_prv_from_dk_blob(dk_blob, sk), CBMPC_SUCCESS);

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt(/*base_pke=*/nullptr, curve, ek_blob, label, x, &ct), CBMPC_SUCCESS);

  cbmpc_pve_rsa_oaep_hsm_decap_t cb;
  cb.ctx = &sk;
  cb.decap = rsa_oaep_hsm_decap_cb;

  cmem_t x_out{nullptr, 0};
  ASSERT_EQ(
      cbmpc_pve_decrypt_rsa_oaep_hsm(curve,
                                     /*dk_handle=*/cmem_t{reinterpret_cast<uint8_t*>(const_cast<char*>("hsm")), 3},
                                     ek_blob, ct, label, &cb, &x_out),
      CBMPC_SUCCESS);
  ASSERT_EQ(x_out.size, 32);
  ASSERT_EQ(std::memcmp(x_out.data, x.data, 32), 0);

  cbmpc_cmem_free(x_out);
  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk_blob);
  cbmpc_cmem_free(ek_blob);
}

static cbmpc_error_t ecies_p256_hsm_ecdh_cb(void* ctx, cmem_t /*dk_handle*/, cmem_t kem_ct, cmem_t* out_dh_x32) {
  if (!ctx || !out_dh_x32) return E_BADARG;
  *out_dh_x32 = cmem_t{nullptr, 0};
  auto* sk = static_cast<coinbase::crypto::ecc_prv_key_t*>(ctx);

  coinbase::crypto::ecc_point_t E;
  coinbase::error_t rv = E.from_oct(coinbase::crypto::curve_p256, mem_t(kem_ct.data, kem_ct.size));
  if (rv) return rv;
  if (rv = coinbase::crypto::curve_p256.check(E)) return rv;

  const coinbase::buf_t dh = sk->ecdh(E);
  if (dh.size() != 32) return E_FORMAT;

  out_dh_x32->data = static_cast<uint8_t*>(cbmpc_malloc(32));
  if (!out_dh_x32->data) return E_INSUFFICIENT;
  out_dh_x32->size = 32;
  std::memmove(out_dh_x32->data, dh.data(), 32);
  return CBMPC_SUCCESS;
}

TEST(CApiPve, DecryptEciesP256Hsm_UsesCallback) {
  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x55 + i);
  const cmem_t x = {x_bytes.data(), static_cast<int>(x_bytes.size())};

  cmem_t ek_blob{nullptr, 0};
  cmem_t dk_blob{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek_blob, &dk_blob), CBMPC_SUCCESS);

  coinbase::crypto::ecc_prv_key_t sk;
  ASSERT_EQ(parse_ecies_prv_from_dk_blob(dk_blob, sk), CBMPC_SUCCESS);

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt(/*base_pke=*/nullptr, curve, ek_blob, label, x, &ct), CBMPC_SUCCESS);

  cbmpc_pve_ecies_p256_hsm_ecdh_t cb;
  cb.ctx = &sk;
  cb.ecdh = ecies_p256_hsm_ecdh_cb;

  cmem_t x_out{nullptr, 0};
  ASSERT_EQ(
      cbmpc_pve_decrypt_ecies_p256_hsm(curve,
                                       /*dk_handle=*/cmem_t{reinterpret_cast<uint8_t*>(const_cast<char*>("hsm")), 3},
                                       ek_blob, ct, label, &cb, &x_out),
      CBMPC_SUCCESS);
  ASSERT_EQ(x_out.size, 32);
  ASSERT_EQ(std::memcmp(x_out.data, x.data, 32), 0);

  cbmpc_cmem_free(x_out);
  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk_blob);
  cbmpc_cmem_free(ek_blob);
}

struct toy_kem_ctx_t {
  uint8_t tag = 0;
};

static cbmpc_error_t toy_kem_encap(void* ctx, cmem_t /*ek*/, cmem_t rho32, cmem_t* out_kem_ct, cmem_t* out_kem_ss) {
  if (!ctx || !out_kem_ct || !out_kem_ss) return E_BADARG;
  if (rho32.size != 32) return E_BADARG;
  *out_kem_ct = cmem_t{nullptr, 0};
  *out_kem_ss = cmem_t{nullptr, 0};

  const auto* c = static_cast<const toy_kem_ctx_t*>(ctx);
  out_kem_ct->data = static_cast<uint8_t*>(cbmpc_malloc(32));
  out_kem_ss->data = static_cast<uint8_t*>(cbmpc_malloc(32));
  if (!out_kem_ct->data || !out_kem_ss->data) {
    if (out_kem_ct->data) cbmpc_free(out_kem_ct->data);
    if (out_kem_ss->data) cbmpc_free(out_kem_ss->data);
    *out_kem_ct = cmem_t{nullptr, 0};
    *out_kem_ss = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  }
  out_kem_ct->size = 32;
  out_kem_ss->size = 32;

  for (int i = 0; i < 32; i++) {
    const uint8_t b = static_cast<uint8_t>(rho32.data[i] ^ c->tag);
    out_kem_ct->data[i] = b;
    out_kem_ss->data[i] = b;
  }
  return CBMPC_SUCCESS;
}

static cbmpc_error_t toy_kem_decap(void* ctx, cmem_t /*dk*/, cmem_t kem_ct, cmem_t* out_kem_ss) {
  if (!ctx || !out_kem_ss) return E_BADARG;
  if (kem_ct.size != 32) return E_BADARG;
  *out_kem_ss = cmem_t{nullptr, 0};

  out_kem_ss->data = static_cast<uint8_t*>(cbmpc_malloc(32));
  if (!out_kem_ss->data) return E_INSUFFICIENT;
  out_kem_ss->size = 32;
  std::memmove(out_kem_ss->data, kem_ct.data, 32);
  return CBMPC_SUCCESS;
}

TEST(CApiPve, EncVerDec_CustomKem_TwoInstOneProc) {
  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  const cmem_t ek = {reinterpret_cast<uint8_t*>(const_cast<char*>("ek")), 2};
  const cmem_t dk = {reinterpret_cast<uint8_t*>(const_cast<char*>("dk")), 2};

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x66 + i);
  const cmem_t x = {x_bytes.data(), static_cast<int>(x_bytes.size())};

  toy_kem_ctx_t ctx_a{.tag = 0x00};
  toy_kem_ctx_t ctx_b{.tag = 0xFF};
  const cbmpc_pve_base_kem_t kem_a = {&ctx_a, toy_kem_encap, toy_kem_decap};
  const cbmpc_pve_base_kem_t kem_b = {&ctx_b, toy_kem_encap, toy_kem_decap};

  cmem_t ct_a{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt_with_kem(&kem_a, curve, ek, label, x, &ct_a), CBMPC_SUCCESS);

  const buf_t Q_expected_buf = expected_Q(curve, mem_t(x.data, x.size));
  const cmem_t Q_expected = {const_cast<uint8_t*>(Q_expected_buf.data()), Q_expected_buf.size()};

  ASSERT_EQ(cbmpc_pve_verify_with_kem(&kem_a, curve, ek, ct_a, Q_expected, label), CBMPC_SUCCESS);
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(cbmpc_pve_verify_with_kem(&kem_b, curve, ek, ct_a, Q_expected, label), CBMPC_SUCCESS);

  cmem_t x_out{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_decrypt_with_kem(&kem_a, curve, dk, ek, ct_a, label, &x_out), CBMPC_SUCCESS);
  ASSERT_EQ(x_out.size, 32);
  ASSERT_EQ(std::memcmp(x_out.data, x.data, 32), 0);
  cbmpc_cmem_free(x_out);
  cbmpc_cmem_free(ct_a);
}

TEST(CApiPve, BasePkeMismatchRejected) {
  int mode0 = 0;
  int mode1 = 1;
  const cbmpc_pve_base_pke_t base_pke0 = {
      /*ctx=*/&mode0,
      /*encrypt=*/toy_encrypt,
      /*decrypt=*/toy_decrypt,
  };
  const cbmpc_pve_base_pke_t base_pke1 = {
      /*ctx=*/&mode1,
      /*encrypt=*/toy_encrypt,
      /*decrypt=*/toy_decrypt,
  };

  const cbmpc_curve_id_t curve = CBMPC_CURVE_SECP256K1;
  const cmem_t ek = {reinterpret_cast<uint8_t*>(const_cast<char*>("ek")), 2};
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};

  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 9;
  const cmem_t x = {x_bytes.data(), static_cast<int>(x_bytes.size())};

  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt(&base_pke0, curve, ek, label, x, &ct), CBMPC_SUCCESS);

  const buf_t Q_expected_buf = expected_Q(curve, mem_t(x.data, x.size));
  cmem_t Q_expected{const_cast<uint8_t*>(Q_expected_buf.data()), Q_expected_buf.size()};

  dylog_disable_scope_t no_log_err;
  EXPECT_NE(cbmpc_pve_verify(&base_pke1, curve, ek, ct, Q_expected, label), CBMPC_SUCCESS);

  cbmpc_cmem_free(ct);
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

TEST(CApiPveNeg, Encrypt) {
  dylog_disable_scope_t no_log;
  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek, &dk), CBMPC_SUCCESS);
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  const cmem_t x = {x_bytes.data(), 32};
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  cmem_t ct{nullptr, 0};

  EXPECT_EQ(cbmpc_pve_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, x, nullptr), E_BADARG);
  EXPECT_NE(cbmpc_pve_encrypt(nullptr, static_cast<cbmpc_curve_id_t>(0), ek, label, x, &ct), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_encrypt(nullptr, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, label, x, &ct), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, cmem_t{nullptr, 0}, x, &ct), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, cmem_t{nullptr, 0}, &ct), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_encrypt(nullptr, CBMPC_CURVE_SECP256K1, cmem_t{garbage, 4}, label, x, &ct), CBMPC_SUCCESS);

  cbmpc_cmem_free(dk);
  cbmpc_cmem_free(ek);
}

TEST(CApiPveNeg, Verify) {
  dylog_disable_scope_t no_log;
  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek, &dk), CBMPC_SUCCESS);
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  const cmem_t x = {x_bytes.data(), 32};
  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, x, &ct), CBMPC_SUCCESS);
  const auto Q_buf = expected_Q(CBMPC_CURVE_SECP256K1, coinbase::mem_t(x.data, x.size));
  const cmem_t Q = {const_cast<uint8_t*>(Q_buf.data()), Q_buf.size()};

  EXPECT_NE(cbmpc_pve_verify(nullptr, static_cast<cbmpc_curve_id_t>(0), ek, ct, Q, label), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_verify(nullptr, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, ct, Q, label), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_verify(nullptr, CBMPC_CURVE_SECP256K1, ek, cmem_t{nullptr, 0}, Q, label), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_verify(nullptr, CBMPC_CURVE_SECP256K1, ek, ct, cmem_t{nullptr, 0}, label), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_verify(nullptr, CBMPC_CURVE_SECP256K1, ek, ct, Q, cmem_t{nullptr, 0}), CBMPC_SUCCESS);

  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk);
  cbmpc_cmem_free(ek);
}

TEST(CApiPveNeg, Decrypt) {
  dylog_disable_scope_t no_log;
  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek, &dk), CBMPC_SUCCESS);
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  const cmem_t x = {x_bytes.data(), 32};
  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, x, &ct), CBMPC_SUCCESS);
  cmem_t x_out{nullptr, 0};

  EXPECT_EQ(cbmpc_pve_decrypt(nullptr, CBMPC_CURVE_SECP256K1, dk, ek, ct, label, nullptr), E_BADARG);
  EXPECT_NE(cbmpc_pve_decrypt(nullptr, static_cast<cbmpc_curve_id_t>(0), dk, ek, ct, label, &x_out), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_decrypt(nullptr, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, ek, ct, label, &x_out),
            CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_decrypt(nullptr, CBMPC_CURVE_SECP256K1, dk, cmem_t{nullptr, 0}, ct, label, &x_out),
            CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_decrypt(nullptr, CBMPC_CURVE_SECP256K1, dk, ek, cmem_t{nullptr, 0}, label, &x_out),
            CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_decrypt(nullptr, CBMPC_CURVE_SECP256K1, dk, ek, ct, cmem_t{nullptr, 0}, &x_out), CBMPC_SUCCESS);

  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk);
  cbmpc_cmem_free(ek);
}

TEST(CApiPveNeg, GetQ) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  EXPECT_EQ(cbmpc_pve_get_Q(cmem_t{nullptr, 0}, nullptr), E_BADARG);
  cmem_t Q{nullptr, 0};
  EXPECT_NE(cbmpc_pve_get_Q(cmem_t{nullptr, 0}, &Q), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_get_Q(cmem_t{garbage, 4}, &Q), CBMPC_SUCCESS);
}

TEST(CApiPveNeg, GetLabel) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  EXPECT_EQ(cbmpc_pve_get_Label(cmem_t{nullptr, 0}, nullptr), E_BADARG);
  cmem_t label{nullptr, 0};
  EXPECT_NE(cbmpc_pve_get_Label(cmem_t{nullptr, 0}, &label), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_get_Label(cmem_t{garbage, 4}, &label), CBMPC_SUCCESS);
}

TEST(CApiPveNeg, GenerateBasePkeRsaKeypair) {
  dylog_disable_scope_t no_log;
  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  EXPECT_EQ(cbmpc_pve_generate_base_pke_rsa_keypair(nullptr, &dk), E_BADARG);
  EXPECT_EQ(cbmpc_pve_generate_base_pke_rsa_keypair(&ek, nullptr), E_BADARG);
}

TEST(CApiPveNeg, GenerateBasePkeEciesP256Keypair) {
  dylog_disable_scope_t no_log;
  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  EXPECT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(nullptr, &dk), E_BADARG);
  EXPECT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek, nullptr), E_BADARG);
}

TEST(CApiPveNeg, BasePkeEciesP256EkFromOct) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  std::array<uint8_t, 64> wrong_size{};
  cmem_t ek{nullptr, 0};
  EXPECT_EQ(cbmpc_pve_base_pke_ecies_p256_ek_from_oct(cmem_t{nullptr, 0}, nullptr), E_BADARG);
  EXPECT_NE(cbmpc_pve_base_pke_ecies_p256_ek_from_oct(cmem_t{nullptr, 0}, &ek), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_base_pke_ecies_p256_ek_from_oct(cmem_t{garbage, 4}, &ek), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_base_pke_ecies_p256_ek_from_oct(cmem_t{wrong_size.data(), 64}, &ek), CBMPC_SUCCESS);
}

TEST(CApiPveNeg, BasePkeRsaEkFromModulus) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  std::array<uint8_t, 128> wrong_size{};
  wrong_size[0] = 1;
  std::array<uint8_t, 256> all_zero{};
  cmem_t ek{nullptr, 0};
  EXPECT_EQ(cbmpc_pve_base_pke_rsa_ek_from_modulus(cmem_t{nullptr, 0}, nullptr), E_BADARG);
  EXPECT_NE(cbmpc_pve_base_pke_rsa_ek_from_modulus(cmem_t{nullptr, 0}, &ek), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_base_pke_rsa_ek_from_modulus(cmem_t{garbage, 4}, &ek), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_base_pke_rsa_ek_from_modulus(cmem_t{wrong_size.data(), 128}, &ek), CBMPC_SUCCESS);
  EXPECT_NE(cbmpc_pve_base_pke_rsa_ek_from_modulus(cmem_t{all_zero.data(), 256}, &ek), CBMPC_SUCCESS);
}

TEST(CApiPveNeg, DecryptRsaOaepHsm) {
  dylog_disable_scope_t no_log;
  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_rsa_keypair(&ek, &dk), CBMPC_SUCCESS);
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  const cmem_t x = {x_bytes.data(), 32};
  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, x, &ct), CBMPC_SUCCESS);
  coinbase::crypto::rsa_prv_key_t sk;
  ASSERT_EQ(parse_rsa_prv_from_dk_blob(dk, sk), CBMPC_SUCCESS);
  cbmpc_pve_rsa_oaep_hsm_decap_t cb;
  cb.ctx = &sk;
  cb.decap = rsa_oaep_hsm_decap_cb;
  cbmpc_pve_rsa_oaep_hsm_decap_t cb_null_decap;
  cb_null_decap.ctx = nullptr;
  cb_null_decap.decap = nullptr;
  cmem_t x_out{nullptr, 0};
  const cmem_t dk_handle = {reinterpret_cast<uint8_t*>(const_cast<char*>("hsm")), 3};

  EXPECT_EQ(cbmpc_pve_decrypt_rsa_oaep_hsm(CBMPC_CURVE_SECP256K1, dk_handle, ek, ct, label, &cb, nullptr), E_BADARG);
  EXPECT_EQ(cbmpc_pve_decrypt_rsa_oaep_hsm(CBMPC_CURVE_SECP256K1, dk_handle, ek, ct, label, nullptr, &x_out), E_BADARG);
  EXPECT_EQ(cbmpc_pve_decrypt_rsa_oaep_hsm(CBMPC_CURVE_SECP256K1, dk_handle, ek, ct, label, &cb_null_decap, &x_out),
            E_BADARG);
  EXPECT_NE(cbmpc_pve_decrypt_rsa_oaep_hsm(CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, ek, ct, label, &cb, &x_out),
            CBMPC_SUCCESS);

  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk);
  cbmpc_cmem_free(ek);
}

TEST(CApiPveNeg, DecryptEciesP256Hsm) {
  dylog_disable_scope_t no_log;
  cmem_t ek{nullptr, 0};
  cmem_t dk{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_generate_base_pke_ecies_p256_keypair(&ek, &dk), CBMPC_SUCCESS);
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  const cmem_t x = {x_bytes.data(), 32};
  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt(nullptr, CBMPC_CURVE_SECP256K1, ek, label, x, &ct), CBMPC_SUCCESS);
  coinbase::crypto::ecc_prv_key_t ecies_sk;
  ASSERT_EQ(parse_ecies_prv_from_dk_blob(dk, ecies_sk), CBMPC_SUCCESS);
  cbmpc_pve_ecies_p256_hsm_ecdh_t cb;
  cb.ctx = &ecies_sk;
  cb.ecdh = ecies_p256_hsm_ecdh_cb;
  cbmpc_pve_ecies_p256_hsm_ecdh_t cb_null_ecdh;
  cb_null_ecdh.ctx = nullptr;
  cb_null_ecdh.ecdh = nullptr;
  cmem_t x_out{nullptr, 0};
  const cmem_t dk_handle = {reinterpret_cast<uint8_t*>(const_cast<char*>("hsm")), 3};

  EXPECT_EQ(cbmpc_pve_decrypt_ecies_p256_hsm(CBMPC_CURVE_SECP256K1, dk_handle, ek, ct, label, &cb, nullptr), E_BADARG);
  EXPECT_EQ(cbmpc_pve_decrypt_ecies_p256_hsm(CBMPC_CURVE_SECP256K1, dk_handle, ek, ct, label, nullptr, &x_out),
            E_BADARG);
  EXPECT_EQ(cbmpc_pve_decrypt_ecies_p256_hsm(CBMPC_CURVE_SECP256K1, dk_handle, ek, ct, label, &cb_null_ecdh, &x_out),
            E_BADARG);
  EXPECT_NE(cbmpc_pve_decrypt_ecies_p256_hsm(CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, ek, ct, label, &cb, &x_out),
            CBMPC_SUCCESS);

  cbmpc_cmem_free(ct);
  cbmpc_cmem_free(dk);
  cbmpc_cmem_free(ek);
}

TEST(CApiPveNeg, EncryptWithKem) {
  dylog_disable_scope_t no_log;
  const cmem_t ek = {reinterpret_cast<uint8_t*>(const_cast<char*>("ek")), 2};
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  const cmem_t x = {x_bytes.data(), 32};
  toy_kem_ctx_t ctx{.tag = 0x00};
  const cbmpc_pve_base_kem_t kem = {&ctx, toy_kem_encap, toy_kem_decap};
  const cbmpc_pve_base_kem_t kem_null_encap = {&ctx, nullptr, toy_kem_decap};
  cmem_t ct{nullptr, 0};

  EXPECT_EQ(cbmpc_pve_encrypt_with_kem(&kem, CBMPC_CURVE_SECP256K1, ek, label, x, nullptr), E_BADARG);
  EXPECT_EQ(cbmpc_pve_encrypt_with_kem(nullptr, CBMPC_CURVE_SECP256K1, ek, label, x, &ct), E_BADARG);
  EXPECT_EQ(cbmpc_pve_encrypt_with_kem(&kem_null_encap, CBMPC_CURVE_SECP256K1, ek, label, x, &ct), E_BADARG);
}

TEST(CApiPveNeg, VerifyWithKem) {
  dylog_disable_scope_t no_log;
  const cmem_t ek = {reinterpret_cast<uint8_t*>(const_cast<char*>("ek")), 2};
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  const cmem_t x = {x_bytes.data(), 32};
  toy_kem_ctx_t ctx{.tag = 0x00};
  const cbmpc_pve_base_kem_t kem = {&ctx, toy_kem_encap, toy_kem_decap};
  const cbmpc_pve_base_kem_t kem_null_encap = {&ctx, nullptr, toy_kem_decap};
  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt_with_kem(&kem, CBMPC_CURVE_SECP256K1, ek, label, x, &ct), CBMPC_SUCCESS);
  const auto Q_buf = expected_Q(CBMPC_CURVE_SECP256K1, coinbase::mem_t(x.data, x.size));
  const cmem_t Q = {const_cast<uint8_t*>(Q_buf.data()), Q_buf.size()};

  EXPECT_EQ(cbmpc_pve_verify_with_kem(nullptr, CBMPC_CURVE_SECP256K1, ek, ct, Q, label), E_BADARG);
  EXPECT_EQ(cbmpc_pve_verify_with_kem(&kem_null_encap, CBMPC_CURVE_SECP256K1, ek, ct, Q, label), E_BADARG);

  cbmpc_cmem_free(ct);
}

TEST(CApiPveNeg, DecryptWithKem) {
  dylog_disable_scope_t no_log;
  const cmem_t ek = {reinterpret_cast<uint8_t*>(const_cast<char*>("ek")), 2};
  const cmem_t dk = {reinterpret_cast<uint8_t*>(const_cast<char*>("dk")), 2};
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("label")), 5};
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  const cmem_t x = {x_bytes.data(), 32};
  toy_kem_ctx_t ctx{.tag = 0x00};
  const cbmpc_pve_base_kem_t kem = {&ctx, toy_kem_encap, toy_kem_decap};
  const cbmpc_pve_base_kem_t kem_null_decap = {&ctx, toy_kem_encap, nullptr};
  cmem_t ct{nullptr, 0};
  ASSERT_EQ(cbmpc_pve_encrypt_with_kem(&kem, CBMPC_CURVE_SECP256K1, ek, label, x, &ct), CBMPC_SUCCESS);
  cmem_t x_out{nullptr, 0};

  EXPECT_EQ(cbmpc_pve_decrypt_with_kem(&kem, CBMPC_CURVE_SECP256K1, dk, ek, ct, label, nullptr), E_BADARG);
  EXPECT_EQ(cbmpc_pve_decrypt_with_kem(nullptr, CBMPC_CURVE_SECP256K1, dk, ek, ct, label, &x_out), E_BADARG);
  EXPECT_EQ(cbmpc_pve_decrypt_with_kem(&kem_null_decap, CBMPC_CURVE_SECP256K1, dk, ek, ct, label, &x_out), E_BADARG);

  cbmpc_cmem_free(ct);
}
