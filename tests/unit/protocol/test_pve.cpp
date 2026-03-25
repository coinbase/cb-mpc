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
