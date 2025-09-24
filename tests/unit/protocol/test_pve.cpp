#include <gtest/gtest.h>

#include <cbmpc/core/log.h>
#include <cbmpc/protocol/pve.h>
#include <cbmpc/protocol/pve_ac.h>
#include <cbmpc/protocol/pve_batch.h>
#include <cbmpc/protocol/util.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::mpc;

namespace {

typedef ec_pve_batch_t pve_batch_t;

struct toy_kem_policy_t {
  struct ek_t {};
  struct dk_t {};

  static error_t encapsulate(const ek_t &, buf_t &kem_ct, buf_t &kem_ss, crypto::drbg_aes_ctr_t *drbg) {
    kem_ss = drbg ? drbg->gen(32) : crypto::gen_random(32);
    kem_ct = kem_ss;  // trivial, not secure, only for test
    return SUCCESS;
  }

  static error_t decapsulate(const dk_t &, mem_t kem_ct, buf_t &kem_ss) {
    kem_ss = buf_t(kem_ct);
    return SUCCESS;
  }
};

class PVE : public testing::Test {
 protected:
  void SetUp() override {
    // Generate RSA keys
    rsa_prv_key1.generate(2048);
    rsa_prv_key2.generate(2048);

    // Generate ECC key
    ecc_prv_key.generate(crypto::curve_p256);

    // Unified valid pairs
    valid_unified = {
        {crypto::pub_key_t::from(rsa_prv_key1.pub()), crypto::prv_key_t::from(rsa_prv_key1)},
        {crypto::pub_key_t::from(rsa_prv_key2.pub()), crypto::prv_key_t::from(rsa_prv_key2)},
        {crypto::pub_key_t::from(ecc_prv_key.pub()), crypto::prv_key_t::from(ecc_prv_key)},
    };

    // Unified invalid pairs (mismatched)
    invalid_unified = {
        {crypto::pub_key_t::from(rsa_prv_key1.pub()), crypto::prv_key_t::from(rsa_prv_key2)},
        {crypto::pub_key_t::from(rsa_prv_key2.pub()), crypto::prv_key_t::from(rsa_prv_key1)},
        {crypto::pub_key_t::from(rsa_prv_key1.pub()), crypto::prv_key_t::from(ecc_prv_key)},
        {crypto::pub_key_t::from(rsa_prv_key2.pub()), crypto::prv_key_t::from(ecc_prv_key)},
        {crypto::pub_key_t::from(ecc_prv_key.pub()), crypto::prv_key_t::from(rsa_prv_key1)},
        {crypto::pub_key_t::from(ecc_prv_key.pub()), crypto::prv_key_t::from(rsa_prv_key2)},
    };
  }

  const ecurve_t curve = crypto::curve_p256;
  const mod_t &q = curve.order();
  const crypto::ecc_generator_point_t &G = curve.generator();

  // Keys
  crypto::rsa_prv_key_t rsa_prv_key1, rsa_prv_key2;
  crypto::ecc_prv_key_t ecc_prv_key;

  // Unified pairs
  std::vector<std::pair<crypto::pub_key_t, crypto::prv_key_t>> valid_unified;
  std::vector<std::pair<crypto::pub_key_t, crypto::prv_key_t>> invalid_unified;
};

// Define alias for fixture used by batch tests
typedef PVE PVEBatch;

TEST_F(PVE, DefaultUnified_Completeness) {
  for (const auto &kp : valid_unified) {
    const auto &pub_key = kp.first;
    const auto &prv_key = kp.second;

    ec_pve_t pve;  // defaults to base_pke_unified
    bn_t x = bn_t::rand(q);
    ecc_point_t X = x * G;

    pve.encrypt(&pub_key, "test-label", curve, x);
    EXPECT_OK(pve.verify(&pub_key, X, "test-label"));

    bn_t decrypted_x;
    EXPECT_OK(pve.decrypt(&prv_key, &pub_key, "test-label", curve, decrypted_x));
    EXPECT_EQ(x, decrypted_x);
  }
}

TEST_F(PVE, DefaultUnified_VerifyWithWrongLabel) {
  for (const auto &kp : valid_unified) {
    const auto &pub_key = kp.first;
    ec_pve_t pve;
    bn_t x = bn_t::rand(q);
    ecc_point_t X = x * G;

    pve.encrypt(&pub_key, "test-label", curve, x);
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve.verify(&pub_key, X, "wrong-label"));
  }
}

TEST_F(PVE, DefaultUnified_VerifyWithWrongQ) {
  for (const auto &kp : valid_unified) {
    const auto &pub_key = kp.first;
    ec_pve_t pve;
    bn_t x = bn_t::rand(q);

    pve.encrypt(&pub_key, "test-label", curve, x);
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve.verify(&pub_key, bn_t::rand(q) * G, "test-label"));
  }
}

TEST_F(PVE, DefaultUnified_DecryptWithWrongLabel) {
  for (const auto &kp : valid_unified) {
    const auto &pub_key = kp.first;
    const auto &prv_key = kp.second;

    ec_pve_t pve;
    bn_t x = bn_t::rand(q);

    pve.encrypt(&pub_key, "test-label", curve, x);

    bn_t decrypted_x;
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve.decrypt(&prv_key, &pub_key, "wrong-label", curve, decrypted_x));
    EXPECT_NE(x, decrypted_x);
  }
}

TEST_F(PVE, DefaultUnified_DecryptWithWrongKey) {
  for (const auto &kp : invalid_unified) {
    const auto &pub_key = kp.first;
    const auto &prv_key = kp.second;

    ec_pve_t pve;
    bn_t x = bn_t::rand(q);

    pve.encrypt(&pub_key, "test-label", curve, x);

    bn_t decrypted_x;
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve.decrypt(&prv_key, &pub_key, "test-label", curve, decrypted_x));
    EXPECT_NE(x, decrypted_x);
  }
}

TEST_F(PVE, RSA_Completeness) {
  crypto::rsa_prv_key_t rsa_sk;
  rsa_sk.generate(2048);
  crypto::rsa_pub_key_t rsa_pk(rsa_sk.pub());

  ec_pve_t pve(pve_base_pke_rsa());
  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;

  pve.encrypt(&rsa_pk, "test-label", curve, x);
  EXPECT_OK(pve.verify(&rsa_pk, X, "test-label"));

  bn_t decrypted_x;
  EXPECT_OK(pve.decrypt(&rsa_sk, &rsa_pk, "test-label", curve, decrypted_x));
  EXPECT_EQ(x, decrypted_x);
}

TEST_F(PVE, ECIES_Completeness) {
  crypto::ecc_prv_key_t ecc_sk;
  ecc_sk.generate(crypto::curve_p256);
  crypto::ecc_pub_key_t ecc_pk(ecc_sk.pub());

  ec_pve_t pve(pve_base_pke_ecies());
  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;

  pve.encrypt(&ecc_pk, "test-label", curve, x);
  EXPECT_OK(pve.verify(&ecc_pk, X, "test-label"));

  bn_t decrypted_x;
  EXPECT_OK(pve.decrypt(&ecc_sk, &ecc_pk, "test-label", curve, decrypted_x));
  EXPECT_EQ(x, decrypted_x);
}

TEST_F(PVE, CustomKEM_Completeness) {
  const mpc::pve_base_pke_i &base_pke = mpc::kem_pve_base_pke<toy_kem_policy_t>();
  mpc::ec_pve_t pve(base_pke);

  toy_kem_policy_t::ek_t ek;
  toy_kem_policy_t::dk_t dk;

  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;

  pve.encrypt(&ek, "test-label", curve, x);
  EXPECT_OK(pve.verify(&ek, X, "test-label"));

  bn_t decrypted_x;
  EXPECT_OK(pve.decrypt(&dk, &ek, "test-label", curve, decrypted_x));
  EXPECT_EQ(x, decrypted_x);
}

typedef PVE PVEBatch;

TEST_F(PVEBatch, Completeness) {
  int n = 20;
  for (const auto &[pub_key, prv_key] : valid_unified) {
    pve_batch_t pve_batch(n);
    std::vector<bn_t> xs(n);
    std::vector<ecc_point_t> Xs(n);
    for (int i = 0; i < n; i++) {
      xs[i] = (i > n / 2) ? bn_t(i) : bn_t::rand(q);
      Xs[i] = xs[i] * G;
    }

    pve_batch.encrypt(&pub_key, "test-label", curve, xs);
    EXPECT_OK(pve_batch.verify(&pub_key, Xs, "test-label"));

    std::vector<bn_t> decrypted_xs;
    EXPECT_OK(pve_batch.decrypt(&prv_key, &pub_key, "test-label", curve, decrypted_xs));
    EXPECT_EQ(xs, decrypted_xs);
  }
}

TEST_F(PVEBatch, VerifyWithWrongLabel) {
  for (const auto &[pub_key, prv_key] : valid_unified) {
    pve_batch_t pve_batch(1);
    bn_t x = bn_t::rand(q);
    ecc_point_t X = x * G;

    pve_batch.encrypt(&pub_key, "test-label", curve, {x});
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve_batch.verify(&pub_key, {X}, "wrong-label"));
  }
}

TEST_F(PVEBatch, VerifyWithWrongPublicKey) {
  for (const auto &[pub_key, prv_key] : valid_unified) {
    pve_batch_t pve_batch(1);
    bn_t x = bn_t::rand(q);
    ecc_point_t X = x * G;

    pve_batch.encrypt(&pub_key, "test-label", curve, {x});
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve_batch.verify(&pub_key, {bn_t::rand(q) * G}, "test-label"));
  }
}

TEST_F(PVEBatch, DecryptWithWrongLabel) {
  for (const auto &[pub_key, prv_key] : valid_unified) {
    pve_batch_t pve_batch(1);
    std::vector<bn_t> xs = {bn_t::rand(q)};

    pve_batch.encrypt(&pub_key, "test-label", curve, xs);

    std::vector<bn_t> decrypted_xs;
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve_batch.decrypt(&prv_key, &pub_key, "wrong-label", curve, decrypted_xs));
    EXPECT_NE(xs, decrypted_xs);
  }
}

TEST_F(PVEBatch, CustomKEM_Batch_Completeness) {
  const mpc::pve_base_pke_i &base_pke = mpc::kem_pve_base_pke<toy_kem_policy_t>();
  int n = 8;
  mpc::ec_pve_batch_t pve_batch(n, base_pke);

  toy_kem_policy_t::ek_t ek;
  toy_kem_policy_t::dk_t dk;

  std::vector<bn_t> xs(n);
  std::vector<ecc_point_t> Xs(n);
  for (int i = 0; i < n; i++) {
    xs[i] = bn_t::rand(q);
    Xs[i] = xs[i] * G;
  }

  pve_batch.encrypt(&ek, "test-label", curve, xs);
  EXPECT_OK(pve_batch.verify(&ek, Xs, "test-label"));

  std::vector<bn_t> decrypted_xs;
  EXPECT_OK(pve_batch.decrypt(&dk, &ek, "test-label", curve, decrypted_xs));
  EXPECT_EQ(xs, decrypted_xs);
}

}  // namespace
