#include <array>
#include <gtest/gtest.h>

#include <cbmpc/api/pve.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/crypto/base_ecc.h>
#include <cbmpc/internal/crypto/base_rsa.h>

namespace {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;

using coinbase::api::curve_id;

class toy_base_pke_t final : public coinbase::api::pve::base_pke_i {
 public:
  explicit toy_base_pke_t(bool mutate) : mutate_(mutate) {}

  error_t encrypt(mem_t /*ek*/, mem_t /*label*/, mem_t plain, mem_t /*rho*/, buf_t& out_ct) const override {
    out_ct = buf_t(plain);
    if (mutate_) {
      const uint8_t b = 0x42;
      out_ct += mem_t(&b, 1);
    }
    return SUCCESS;
  }

  error_t decrypt(mem_t /*dk*/, mem_t /*label*/, mem_t ct, buf_t& out_plain) const override {
    out_plain = buf_t(ct);
    if (mutate_) {
      if (out_plain.size() < 1) return E_FORMAT;
      out_plain.resize(out_plain.size() - 1);
    }
    return SUCCESS;
  }

 private:
  bool mutate_ = false;
};

static buf_t expected_Q(curve_id cid, mem_t x) {
  const coinbase::crypto::ecurve_t curve = (cid == curve_id::p256)        ? coinbase::crypto::curve_p256
                                           : (cid == curve_id::secp256k1) ? coinbase::crypto::curve_secp256k1
                                           : (cid == curve_id::ed25519)   ? coinbase::crypto::curve_ed25519
                                                                          : coinbase::crypto::ecurve_t();
  cb_assert(curve.valid());

  const coinbase::crypto::bn_t bn_x = coinbase::crypto::bn_t::from_bin(x) % curve.order();
  const coinbase::crypto::ecc_point_t Q = bn_x * curve.generator();
  return Q.to_compressed_bin();
}

// Mirror of the cbmpc base-PKE key blob format used by `coinbase::api::pve`.
// This is test-only plumbing so we can build HSM stubs using software keys.
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

static error_t parse_rsa_prv_from_dk_blob(mem_t dk_blob, coinbase::crypto::rsa_prv_key_t& out_sk) {
  base_pke_dk_blob_v1_t blob;
  error_t rv = coinbase::convert(blob, dk_blob);
  if (rv) return rv;
  if (blob.version != base_pke_key_blob_version_v1) return E_FORMAT;
  if (static_cast<base_pke_key_type_v1>(blob.key_type) != base_pke_key_type_v1::rsa_oaep_2048) return E_BADARG;
  out_sk = blob.rsa_dk;
  return SUCCESS;
}

static error_t parse_ecies_prv_from_dk_blob(mem_t dk_blob, coinbase::crypto::ecc_prv_key_t& out_sk) {
  base_pke_dk_blob_v1_t blob;
  error_t rv = coinbase::convert(blob, dk_blob);
  if (rv) return rv;
  if (blob.version != base_pke_key_blob_version_v1) return E_FORMAT;
  if (static_cast<base_pke_key_type_v1>(blob.key_type) != base_pke_key_type_v1::ecies_p256) return E_BADARG;
  out_sk = blob.ecies_dk;
  return SUCCESS;
}

}  // namespace

TEST(ApiPve, EncryptVerifyDecrypt_CustomBasePke) {
  const toy_base_pke_t base_pke(/*mutate=*/false);

  const curve_id curve = curve_id::secp256k1;
  const buf_t ek = buf_t("ek");
  const buf_t dk = buf_t("dk");
  const buf_t label = buf_t("label");

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i);
  const mem_t x_mem(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt(base_pke, curve, ek, label, x_mem, ct), SUCCESS);
  ASSERT_GT(ct.size(), 0);

  buf_t Q_ct;
  ASSERT_EQ(coinbase::api::pve::get_Q(ct, Q_ct), SUCCESS);

  buf_t L_ct;
  ASSERT_EQ(coinbase::api::pve::get_Label(ct, L_ct), SUCCESS);
  EXPECT_EQ(L_ct, label);

  const buf_t Q_expected = expected_Q(curve, x_mem);
  EXPECT_EQ(Q_ct, Q_expected);

  ASSERT_EQ(coinbase::api::pve::verify(base_pke, curve, ek, ct, Q_expected, label), SUCCESS);

  buf_t x_out;
  ASSERT_EQ(coinbase::api::pve::decrypt(base_pke, curve, dk, ek, ct, label, x_out), SUCCESS);
  EXPECT_EQ(x_out.size(), 32);
  EXPECT_EQ(x_out, buf_t(x_mem));
}

TEST(ApiPve, EncVerDec_DefBasePke_EciesBlob) {
  const curve_id curve = curve_id::secp256k1;
  const buf_t label = buf_t("label");

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0xA0 + i);
  const mem_t x_mem(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ek_blob;
  buf_t dk_blob;
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_ecies_p256_keypair(ek_blob, dk_blob), SUCCESS);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt(curve, ek_blob, label, x_mem, ct), SUCCESS);

  const buf_t Q_expected = expected_Q(curve, x_mem);
  ASSERT_EQ(coinbase::api::pve::verify(curve, ek_blob, ct, Q_expected, label), SUCCESS);

  buf_t x_out;
  ASSERT_EQ(coinbase::api::pve::decrypt(curve, dk_blob, ek_blob, ct, label, x_out), SUCCESS);
  EXPECT_EQ(x_out.size(), 32);
  EXPECT_EQ(x_out, buf_t(x_mem));
}

TEST(ApiPve, EncVerDec_DefBasePke_RsaBlob) {
  const curve_id curve = curve_id::secp256k1;
  const buf_t label = buf_t("label");

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x11 + i);
  const mem_t x_mem(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ek_blob;
  buf_t dk_blob;
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(ek_blob, dk_blob), SUCCESS);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt(curve, ek_blob, label, x_mem, ct), SUCCESS);

  const buf_t Q_expected = expected_Q(curve, x_mem);
  ASSERT_EQ(coinbase::api::pve::verify(curve, ek_blob, ct, Q_expected, label), SUCCESS);

  buf_t x_out;
  ASSERT_EQ(coinbase::api::pve::decrypt(curve, dk_blob, ek_blob, ct, label, x_out), SUCCESS);
  EXPECT_EQ(x_out.size(), 32);
  EXPECT_EQ(x_out, buf_t(x_mem));
}

TEST(ApiPve, EncryptRejectsOversizedX) {
  const curve_id curve = curve_id::secp256k1;
  const buf_t label = buf_t("label");

  buf_t ek_blob;
  buf_t dk_blob;
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(ek_blob, dk_blob), SUCCESS);

  // secp256k1 order is 32 bytes; ensure oversize inputs are rejected.
  std::array<uint8_t, 33> x_bytes{};
  for (int i = 0; i < 33; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  const mem_t x_mem(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ct;
  EXPECT_EQ(coinbase::api::pve::encrypt(curve, ek_blob, label, x_mem, ct), E_RANGE);
}

TEST(ApiPve, DecryptRsaOaepHsm_UsesCallback) {
  struct ctx_t {
    coinbase::crypto::rsa_prv_key_t sk;
  } ctx;

  const curve_id curve = curve_id::secp256k1;
  const buf_t label = buf_t("label");

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x22 + i);
  const mem_t x_mem(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ek_blob;
  buf_t dk_blob;
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(ek_blob, dk_blob), SUCCESS);
  ASSERT_EQ(parse_rsa_prv_from_dk_blob(dk_blob, ctx.sk), SUCCESS);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt(curve, ek_blob, label, x_mem, ct), SUCCESS);

  coinbase::api::pve::rsa_oaep_hsm_decap_cb_t cb;
  cb.ctx = &ctx;
  cb.decap = +[](void* c, mem_t /*dk_handle*/, mem_t kem_ct, buf_t& out_kem_ss) -> error_t {
    auto* ctxp = static_cast<ctx_t*>(c);
    // OAEP label is empty per cbmpc's RSA KEM policy.
    return ctxp->sk.decrypt_oaep(kem_ct, coinbase::crypto::hash_e::sha256, coinbase::crypto::hash_e::sha256, mem_t(),
                                 out_kem_ss);
  };

  buf_t x_out;
  ASSERT_EQ(
      coinbase::api::pve::decrypt_rsa_oaep_hsm(curve, /*dk_handle=*/buf_t("hsm-handle"), ek_blob, ct, label, cb, x_out),
      SUCCESS);
  EXPECT_EQ(x_out, buf_t(x_mem));
}

TEST(ApiPve, DecryptEciesP256Hsm_UsesCallback) {
  struct ctx_t {
    coinbase::crypto::ecc_prv_key_t sk;
  } ctx;

  const curve_id curve = curve_id::secp256k1;
  const buf_t label = buf_t("label");

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x33 + i);
  const mem_t x_mem(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ek_blob;
  buf_t dk_blob;
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_ecies_p256_keypair(ek_blob, dk_blob), SUCCESS);
  ASSERT_EQ(parse_ecies_prv_from_dk_blob(dk_blob, ctx.sk), SUCCESS);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt(curve, ek_blob, label, x_mem, ct), SUCCESS);

  coinbase::api::pve::ecies_p256_hsm_ecdh_cb_t cb;
  cb.ctx = &ctx;
  cb.ecdh = +[](void* c, mem_t /*dk_handle*/, mem_t kem_ct, buf_t& out_dh_x32) -> error_t {
    auto* ctxp = static_cast<ctx_t*>(c);
    coinbase::crypto::ecc_point_t E;
    error_t rv = E.from_oct(coinbase::crypto::curve_p256, kem_ct);
    if (rv) return rv;
    if (rv = coinbase::crypto::curve_p256.check(E)) return rv;
    out_dh_x32 = ctxp->sk.ecdh(E);
    return SUCCESS;
  };

  buf_t x_out;
  ASSERT_EQ(coinbase::api::pve::decrypt_ecies_p256_hsm(curve, /*dk_handle=*/buf_t("hsm-handle"), ek_blob, ct, label, cb,
                                                       x_out),
            SUCCESS);
  EXPECT_EQ(x_out, buf_t(x_mem));
}

TEST(ApiPve, VerifyRejectsWrongLabel) {
  const toy_base_pke_t base_pke(/*mutate=*/false);
  const curve_id curve = curve_id::secp256k1;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");

  std::array<uint8_t, 32> x_bytes{};
  const mem_t x_mem(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt(base_pke, curve, ek, label, x_mem, ct), SUCCESS);

  const buf_t Q_expected = expected_Q(curve, x_mem);
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::verify(base_pke, curve, ek, ct, Q_expected, /*label=*/buf_t("wrong")), SUCCESS);
}

TEST(ApiPve, VerifyRejectsWrongQ) {
  const toy_base_pke_t base_pke(/*mutate=*/false);
  const curve_id curve = curve_id::secp256k1;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");

  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 7;
  const mem_t x_mem(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt(base_pke, curve, ek, label, x_mem, ct), SUCCESS);

  // Wrong Q: flip one byte.
  buf_t Q_wrong = expected_Q(curve, x_mem);
  Q_wrong[0] ^= 0x01;

  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::verify(base_pke, curve, ek, ct, Q_wrong, label), SUCCESS);
}

TEST(ApiPve, BasePkeMismatchRejected) {
  const toy_base_pke_t base_pke1(/*mutate=*/false);
  const toy_base_pke_t base_pke2(/*mutate=*/true);

  const curve_id curve = curve_id::secp256k1;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");

  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 9;
  const mem_t x_mem(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt(base_pke1, curve, ek, label, x_mem, ct), SUCCESS);
  const buf_t Q_expected = expected_Q(curve, x_mem);

  // Verifying with a different base PKE should fail.
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::verify(base_pke2, curve, ek, ct, Q_expected, label), SUCCESS);
}
