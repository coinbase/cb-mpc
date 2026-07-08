#include <gtest/gtest.h>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/protocol/pve.h>
#include <cbmpc/internal/protocol/pve_batch.h>
#include <cbmpc/internal/protocol/util.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;
using namespace coinbase::mpc;

namespace {

typedef ec_pve_batch_t pve_batch_t;

struct toy_kem_policy_t {
  struct ek_t {};
  struct dk_t {};

  static error_t encapsulate(const ek_t&, buf_t& kem_ct, buf_t& kem_ss, crypto::drbg_aes_ctr_t* drbg) {
    kem_ss = drbg ? drbg->gen(32) : crypto::gen_random(32);
    kem_ct = kem_ss;  // trivial, not secure, only for test
    return SUCCESS;
  }

  static error_t decapsulate(const dk_t&, mem_t kem_ct, buf_t& kem_ss) {
    kem_ss = buf_t(kem_ct);
    return SUCCESS;
  }
};

error_t runtime_encap_fail(void*, mem_t, mem_t, buf_t&, buf_t&) { return E_CRYPTO; }
error_t runtime_decap_fail(void*, const void*, mem_t, buf_t&) { return E_CRYPTO; }

error_t rsa_hsm_decap_fail(void*, mem_t, mem_t, buf_t&) { return E_CRYPTO; }
error_t rsa_hsm_decap_bad_size(void*, mem_t, mem_t, buf_t& kem_ss) {
  kem_ss = crypto::gen_random(31);
  return SUCCESS;
}

error_t ecies_hsm_ecdh_fail(void*, mem_t, mem_t, buf_t&) { return E_CRYPTO; }
error_t ecies_hsm_ecdh_bad_size(void*, mem_t, mem_t, buf_t& dh_x32) {
  dh_x32 = crypto::gen_random(31);
  return SUCCESS;
}

TEST(PVE, RSA_Completeness) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();

  rsa_prv_key_t rsa_sk;
  rsa_sk.generate(2048);
  rsa_pub_key_t rsa_pk(rsa_sk.pub());

  ec_pve_t pve;
  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;

  EXPECT_OK(pve.encrypt(pve_base_pke_rsa(), pve_keyref(rsa_pk), "test-label", curve, x));
  EXPECT_OK(pve.verify(pve_base_pke_rsa(), pve_keyref(rsa_pk), X, "test-label"));

  bn_t decrypted_x;
  EXPECT_OK(pve.decrypt(pve_base_pke_rsa(), pve_keyref(rsa_sk), pve_keyref(rsa_pk), "test-label", curve, decrypted_x));
  EXPECT_EQ(x, decrypted_x);
}

TEST(PVE, KeyRefRejectsWrongTypesAndMalformedCiphertexts) {
  const pve_base_pke_i& base_pke = kem_pve_base_pke<toy_kem_policy_t>();
  toy_kem_policy_t::ek_t ek;
  toy_kem_policy_t::dk_t dk;

  pve_keyref_t ek_ref = pve_keyref(ek);
  EXPECT_EQ(ek_ref.get<toy_kem_policy_t::ek_t>(), &ek);
  EXPECT_EQ(ek_ref.get<toy_kem_policy_t::dk_t>(), nullptr);

  const toy_kem_policy_t::ek_t* null_ek = nullptr;
  pve_keyref_t null_ref = pve_keyref(null_ek);
  EXPECT_EQ(null_ref.get<toy_kem_policy_t::ek_t>(), nullptr);

  buf_t ciphertext;
  buf_t plaintext;
  dylog_disable_scope_t no_log_err;
  EXPECT_ER(base_pke.encrypt(pve_keyref(dk), "test-label", "plain", crypto::gen_random(32), ciphertext));
  EXPECT_ER(base_pke.decrypt(pve_keyref(ek), "test-label", ciphertext, plaintext));
  EXPECT_ER(base_pke.decrypt(pve_keyref(dk), "test-label", buf_t("malformed-ciphertext"), plaintext));
}

TEST(PVE, BuiltInBasePkesRejectWrongTypesMalformedCiphertextsAndWrongLabels) {
  rsa_prv_key_t rsa_sk;
  rsa_sk.generate(2048);
  rsa_pub_key_t rsa_pk(rsa_sk.pub());

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  buf_t ciphertext;
  buf_t plaintext;
  dylog_disable_scope_t no_log_err;

  EXPECT_ER(pve_base_pke_rsa().encrypt(pve_keyref(ecc_pk), "test-label", "plain", crypto::gen_random(32), ciphertext));
  ASSERT_OK(pve_base_pke_rsa().encrypt(pve_keyref(rsa_pk), "test-label", "plain", crypto::gen_random(32), ciphertext));
  EXPECT_ER(pve_base_pke_rsa().decrypt(pve_keyref(ecc_sk), "test-label", ciphertext, plaintext));
  EXPECT_ER(pve_base_pke_rsa().decrypt(pve_keyref(rsa_sk), "test-label", buf_t("malformed-ciphertext"), plaintext));
  EXPECT_ER(pve_base_pke_rsa().decrypt(pve_keyref(rsa_sk), "wrong-label", ciphertext, plaintext));

  EXPECT_ER(
      pve_base_pke_ecies().encrypt(pve_keyref(rsa_pk), "test-label", "plain", crypto::gen_random(32), ciphertext));
  ASSERT_OK(
      pve_base_pke_ecies().encrypt(pve_keyref(ecc_pk), "test-label", "plain", crypto::gen_random(32), ciphertext));
  EXPECT_ER(pve_base_pke_ecies().decrypt(pve_keyref(rsa_sk), "test-label", ciphertext, plaintext));
  EXPECT_ER(pve_base_pke_ecies().decrypt(pve_keyref(ecc_sk), "test-label", buf_t("malformed-ciphertext"), plaintext));
  EXPECT_ER(pve_base_pke_ecies().decrypt(pve_keyref(ecc_sk), "wrong-label", ciphertext, plaintext));
}

TEST(PVE, RuntimeKemRejectsMissingCallbacks) {
  pve_runtime_kem_ek_t ek{mem_t("runtime-ek"), nullptr};
  pve_runtime_kem_dk_t dk{nullptr, nullptr};
  buf_t kem_ct;
  buf_t kem_ss;
  drbg_aes_ctr_t drbg(crypto::gen_random(32));

  EXPECT_ER(kem_policy_runtime_kem_t::encapsulate(ek, kem_ct, kem_ss, &drbg));
  EXPECT_ER(kem_policy_runtime_kem_t::decapsulate(dk, kem_ct, kem_ss));

  pve_runtime_kem_callbacks_t encap_missing;
  encap_missing.decap = runtime_decap_fail;
  ek.callbacks = &encap_missing;
  EXPECT_ER(kem_policy_runtime_kem_t::encapsulate(ek, kem_ct, kem_ss, &drbg));

  pve_runtime_kem_callbacks_t decap_missing;
  decap_missing.encap = runtime_encap_fail;
  dk.callbacks = &decap_missing;
  EXPECT_ER(kem_policy_runtime_kem_t::decapsulate(dk, kem_ct, kem_ss));
}

TEST(PVE, HsmKemWrappersValidateCallbackOutputs) {
  rsa_prv_key_t rsa_sk;
  rsa_sk.generate(2048);
  rsa_pub_key_t rsa_pk(rsa_sk.pub());
  drbg_aes_ctr_t rsa_drbg(crypto::gen_random(32));

  buf_t kem_ct;
  buf_t kem_ss;
  ASSERT_OK(kem_policy_rsa_oaep_hsm_t::encapsulate(rsa_pk, kem_ct, kem_ss, &rsa_drbg));

  pve_rsa_oaep_hsm_dk_t rsa_dk;
  EXPECT_ER(kem_policy_rsa_oaep_hsm_t::decapsulate(rsa_dk, kem_ct, kem_ss));
  rsa_dk.decap = rsa_hsm_decap_fail;
  EXPECT_ER(kem_policy_rsa_oaep_hsm_t::decapsulate(rsa_dk, kem_ct, kem_ss));
  rsa_dk.decap = rsa_hsm_decap_bad_size;
  EXPECT_ER(kem_policy_rsa_oaep_hsm_t::decapsulate(rsa_dk, kem_ct, kem_ss));

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());
  drbg_aes_ctr_t ecies_drbg(crypto::gen_random(32));
  ASSERT_OK(kem_policy_ecdh_p256_hsm_t::encapsulate(ecc_pk, kem_ct, kem_ss, &ecies_drbg));

  pve_ecies_p256_hsm_dk_t ecies_dk;
  EXPECT_ER(kem_policy_ecdh_p256_hsm_t::decapsulate(ecies_dk, kem_ct, kem_ss));
  ecies_dk.ecdh = ecies_hsm_ecdh_bad_size;
  EXPECT_ER(kem_policy_ecdh_p256_hsm_t::decapsulate(ecies_dk, kem_ct, kem_ss));
  ecies_dk.pub_key_oct = ecc_pk.to_oct();
  EXPECT_ER(kem_policy_ecdh_p256_hsm_t::decapsulate(ecies_dk, buf_t("bad-kem-ct"), kem_ss));
  EXPECT_ER(kem_policy_ecdh_p256_hsm_t::decapsulate(ecies_dk, kem_ct, kem_ss));
  ecies_dk.ecdh = ecies_hsm_ecdh_fail;
  EXPECT_ER(kem_policy_ecdh_p256_hsm_t::decapsulate(ecies_dk, kem_ct, kem_ss));
}

TEST(PVE, ECIES_Completeness) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  ec_pve_t pve;
  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;

  EXPECT_OK(pve.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, x));
  EXPECT_OK(pve.verify(pve_base_pke_ecies(), pve_keyref(ecc_pk), X, "test-label"));

  bn_t decrypted_x;
  EXPECT_OK(
      pve.decrypt(pve_base_pke_ecies(), pve_keyref(ecc_sk), pve_keyref(ecc_pk), "test-label", curve, decrypted_x));
  EXPECT_EQ(x, decrypted_x);
}

TEST(PVE, ECIES_VerifyWithWrongLabel) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  ec_pve_t pve;
  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;

  EXPECT_OK(pve.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, x));
  dylog_disable_scope_t no_log_err;
  EXPECT_ER(pve.verify(pve_base_pke_ecies(), pve_keyref(ecc_pk), X, "wrong-label"));
}

TEST(PVE, ECIES_VerifyWithWrongQ) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  ec_pve_t pve;
  bn_t x = bn_t::rand(q);

  EXPECT_OK(pve.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, x));
  dylog_disable_scope_t no_log_err;
  EXPECT_ER(pve.verify(pve_base_pke_ecies(), pve_keyref(ecc_pk), bn_t::rand(q) * G, "test-label"));
}

TEST(PVE, ECIES_DecryptWithWrongLabel) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  ec_pve_t pve;
  bn_t x = bn_t::rand(q);

  EXPECT_OK(pve.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, x));

  bn_t decrypted_x;
  dylog_disable_scope_t no_log_err;
  EXPECT_ER(
      pve.decrypt(pve_base_pke_ecies(), pve_keyref(ecc_sk), pve_keyref(ecc_pk), "wrong-label", curve, decrypted_x));
  EXPECT_NE(x, decrypted_x);
}

TEST(PVE, DecryptSkipVerifyStillAuthenticatesLabel) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  ec_pve_t pve;
  bn_t x = bn_t::rand(q);
  ASSERT_OK(pve.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, x));

  bn_t decrypted_x;
  dylog_disable_scope_t no_log_err;
  EXPECT_ER(pve.decrypt(pve_base_pke_ecies(), pve_keyref(ecc_sk), pve_keyref(ecc_pk), "wrong-label", curve, decrypted_x,
                        /*skip_verify=*/true));
  EXPECT_EQ(decrypted_x, 0);
}

TEST(PVE, CustomKEM_Completeness) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();

  const mpc::pve_base_pke_i& base_pke = mpc::kem_pve_base_pke<toy_kem_policy_t>();
  mpc::ec_pve_t pve;

  toy_kem_policy_t::ek_t ek;
  toy_kem_policy_t::dk_t dk;

  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;

  EXPECT_OK(pve.encrypt(base_pke, pve_keyref(ek), "test-label", curve, x));
  EXPECT_OK(pve.verify(base_pke, pve_keyref(ek), X, "test-label"));

  bn_t decrypted_x;
  EXPECT_OK(pve.decrypt(base_pke, pve_keyref(dk), pve_keyref(ek), "test-label", curve, decrypted_x));
  EXPECT_EQ(x, decrypted_x);
}

TEST(PVEBatch, Completeness_ECIES) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();

  const int n = 20;
  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  pve_batch_t pve_batch(n);
  EXPECT_EQ(pve_batch.batch_count(), n);
  std::vector<bn_t> xs(n);
  std::vector<ecc_point_t> Xs(n);
  for (int i = 0; i < n; i++) {
    xs[i] = (i > n / 2) ? bn_t(i) : bn_t::rand(q);
    Xs[i] = xs[i] * G;
  }

  EXPECT_OK(pve_batch.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, xs));
  EXPECT_OK(pve_batch.verify(pve_base_pke_ecies(), pve_keyref(ecc_pk), Xs, "test-label"));

  std::vector<bn_t> decrypted_xs;
  EXPECT_OK(pve_batch.decrypt(pve_base_pke_ecies(), pve_keyref(ecc_sk), pve_keyref(ecc_pk), "test-label", curve,
                              decrypted_xs));
  EXPECT_EQ(xs, decrypted_xs);

  pve_batch_t decoded(n);
  ASSERT_OK(coinbase::deser(coinbase::ser(pve_batch), decoded));
  EXPECT_EQ(decoded.batch_count(), n);
  EXPECT_EQ(decoded.get_Qs(), Xs);
  EXPECT_EQ(decoded.get_Label(), buf_t("test-label"));
}

TEST(PVEBatch, RejectsHugeBatchCount) {
  dylog_disable_scope_t no_log_err;
  EXPECT_CB_ASSERT(pve_batch_t(200000000), "batch_count");
}

TEST(PVEBatch, VerifyWithWrongLabel_ECIES) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  pve_batch_t pve_batch(1);
  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;

  EXPECT_OK(pve_batch.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, {x}));
  dylog_disable_scope_t no_log_err;
  EXPECT_ER(pve_batch.verify(pve_base_pke_ecies(), pve_keyref(ecc_pk), {X}, "wrong-label"));
}

TEST(PVEBatch, VerifyWithWrongPublicKey_ECIES) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  pve_batch_t pve_batch(1);
  bn_t x = bn_t::rand(q);

  EXPECT_OK(pve_batch.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, {x}));
  dylog_disable_scope_t no_log_err;
  EXPECT_ER(pve_batch.verify(pve_base_pke_ecies(), pve_keyref(ecc_pk), {bn_t::rand(q) * G}, "test-label"));
}

TEST(PVEBatch, DecryptWithWrongLabel_ECIES) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  pve_batch_t pve_batch(1);
  std::vector<bn_t> xs = {bn_t::rand(q)};

  EXPECT_OK(pve_batch.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, xs));

  std::vector<bn_t> decrypted_xs;
  dylog_disable_scope_t no_log_err;
  EXPECT_ER(pve_batch.decrypt(pve_base_pke_ecies(), pve_keyref(ecc_sk), pve_keyref(ecc_pk), "wrong-label", curve,
                              decrypted_xs));
  EXPECT_NE(xs, decrypted_xs);
}

TEST(PVEBatch, DecryptSkipVerifyStillAuthenticatesLabel) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  const int n = 3;
  pve_batch_t pve_batch(n);
  std::vector<bn_t> xs(n);
  for (int i = 0; i < n; i++) xs[i] = bn_t::rand(q);

  ASSERT_OK(pve_batch.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, xs));

  std::vector<bn_t> decrypted_xs;
  dylog_disable_scope_t no_log_err;
  EXPECT_ER(pve_batch.decrypt(pve_base_pke_ecies(), pve_keyref(ecc_sk), pve_keyref(ecc_pk), "wrong-label", curve,
                              decrypted_xs, /*skip_verify=*/true));
}

TEST(PVEBatch, DecryptSkipVerifyRejectsWrongPrivateKey) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();

  ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  ecc_pub_key_t ecc_pk(ecc_sk.pub());

  ecc_prv_key_t wrong_sk;
  wrong_sk.generate(crypto::curve_p256);

  const int n = 3;
  pve_batch_t pve_batch(n);
  std::vector<bn_t> xs(n);
  for (int i = 0; i < n; i++) xs[i] = bn_t::rand(q);

  ASSERT_OK(pve_batch.encrypt(pve_base_pke_ecies(), pve_keyref(ecc_pk), "test-label", curve, xs));

  std::vector<bn_t> decrypted_xs;
  dylog_disable_scope_t no_log_err;
  EXPECT_ER(pve_batch.decrypt(pve_base_pke_ecies(), pve_keyref(wrong_sk), pve_keyref(ecc_pk), "test-label", curve,
                              decrypted_xs, /*skip_verify=*/true));
  EXPECT_TRUE(decrypted_xs.empty());
}

TEST(PVEBatch, CustomKEM_Batch_Completeness) {
  const ecurve_t curve = crypto::curve_p256;
  const mod_t& q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();

  const mpc::pve_base_pke_i& base_pke = mpc::kem_pve_base_pke<toy_kem_policy_t>();
  const int n = 8;
  mpc::ec_pve_batch_t pve_batch(n);

  toy_kem_policy_t::ek_t ek;
  toy_kem_policy_t::dk_t dk;

  std::vector<bn_t> xs(n);
  std::vector<ecc_point_t> Xs(n);
  for (int i = 0; i < n; i++) {
    xs[i] = bn_t::rand(q);
    Xs[i] = xs[i] * G;
  }

  EXPECT_OK(pve_batch.encrypt(base_pke, pve_keyref(ek), "test-label", curve, xs));
  EXPECT_OK(pve_batch.verify(base_pke, pve_keyref(ek), Xs, "test-label"));

  std::vector<bn_t> decrypted_xs;
  EXPECT_OK(pve_batch.decrypt(base_pke, pve_keyref(dk), pve_keyref(ek), "test-label", curve, decrypted_xs));
  EXPECT_EQ(xs, decrypted_xs);
}

}  // namespace
