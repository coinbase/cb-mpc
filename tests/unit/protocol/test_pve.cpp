#include <gtest/gtest.h>

#include <cbmpc/core/log.h>
#include <cbmpc/protocol/pve.h>
#include <cbmpc/protocol/pve_ac.h>
#include <cbmpc/protocol/util.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::mpc;

namespace {

typedef ec_pve_t<crypto::hybrid_cipher_t> pve_t;
typedef ec_pve_batch_t<crypto::hybrid_cipher_t> pve_batch_t;

class PVE : public testing::Test {
 protected:
  void SetUp() override {
    crypto::rsa_prv_key_t rsa_prv_key1, rsa_prv_key2;
    rsa_prv_key1.generate(2048);
    rsa_prv_key2.generate(2048);
    crypto::ecc_prv_key_t ecc_prv_key;
    ecc_prv_key.generate(crypto::curve_p256);

    valid_keys = {
        {crypto::pub_key_t::from(rsa_prv_key1.pub()), crypto::prv_key_t::from(rsa_prv_key1)},
        {crypto::pub_key_t::from(rsa_prv_key2.pub()), crypto::prv_key_t::from(rsa_prv_key2)},
        {crypto::pub_key_t::from(ecc_prv_key.pub()), crypto::prv_key_t::from(ecc_prv_key)},
    };

    invalid_keys = {
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
  std::vector<std::pair<crypto::pub_key_t, crypto::prv_key_t>> valid_keys;
  std::vector<std::pair<crypto::pub_key_t, crypto::prv_key_t>> invalid_keys;
};

TEST_F(PVE, Completeness) {
  for (const auto &[pub_key, prv_key] : valid_keys) {
    pve_t pve;
    bn_t x = bn_t::rand(q);
    ecc_point_t X = x * G;

    pve.encrypt(pub_key, "test-label", curve, x);
    EXPECT_OK(pve.verify(pub_key, X, "test-label"));

    bn_t decrypted_x;
    EXPECT_OK(pve.decrypt(prv_key, "test-label", curve, decrypted_x));
    EXPECT_EQ(x, decrypted_x);
  }
}

TEST_F(PVE, VerifyWithWrongLabel) {
  for (const auto &[pub_key, prv_key] : valid_keys) {
    pve_t pve;
    bn_t x = bn_t::rand(q);
    ecc_point_t X = x * G;

    pve.encrypt(pub_key, "test-label", curve, x);
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve.verify(pub_key, X, "wrong-label"));
  }
}

TEST_F(PVE, VerifyWithWrongQ) {
  for (const auto &[pub_key, prv_key] : valid_keys) {
    pve_t pve;
    bn_t x = bn_t::rand(q);
    ecc_point_t X = x * G;

    pve.encrypt(pub_key, "test-label", curve, x);
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve.verify(pub_key, bn_t::rand(q) * G, "test-label"));
  }
}

TEST_F(PVE, DecryptWithWrongLabel) {
  for (const auto &[pub_key, prv_key] : valid_keys) {
    pve_t pve;
    bn_t x = bn_t::rand(q);

    pve.encrypt(pub_key, "test-label", curve, x);

    bn_t decrypted_x;
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve.decrypt(prv_key, "wrong-label", curve, decrypted_x));
    EXPECT_NE(x, decrypted_x);
  }
}

TEST_F(PVE, DecryptWithWrongKey) {
  for (const auto &[pub_key, prv_key] : invalid_keys) {
    pve_t pve;
    bn_t x = bn_t::rand(q);

    pve.encrypt(pub_key, "test-label", curve, x);

    bn_t decrypted_x;
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve.decrypt(prv_key, "test-label", curve, decrypted_x));
    EXPECT_NE(x, decrypted_x);
  }
}

TEST_F(PVE, Templates) {
  crypto::rsa_prv_key_t rsa_prv_key;
  rsa_prv_key.generate(2048);
  crypto::rsa_pub_key_t rsa_pub_key(rsa_prv_key.pub());

  crypto::ecc_prv_key_t ecc_prv_key;
  ecc_prv_key.generate(crypto::curve_p256);
  crypto::ecc_pub_key_t ecc_pub_key(ecc_prv_key.pub());

  crypto::pub_key_t pub_key = crypto::pub_key_t::from(rsa_prv_key.pub());
  crypto::prv_key_t prv_key = crypto::prv_key_t::from(rsa_prv_key);

  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;
  {
    ec_pve_t<> pve;  // using hybrid_cipher_t by default

    pve.encrypt(pub_key, "test-label", curve, x);
    EXPECT_OK(pve.verify(pub_key, X, "test-label"));

    bn_t decrypted_x;
    EXPECT_OK(pve.decrypt(prv_key, "test-label", curve, decrypted_x));
    EXPECT_EQ(x, decrypted_x);
  }
  {
    ec_pve_t<crypto::hybrid_cipher_t> pve;

    pve.encrypt(pub_key, "test-label", curve, x);
    EXPECT_OK(pve.verify(pub_key, X, "test-label"));

    bn_t decrypted_x;
    EXPECT_OK(pve.decrypt(prv_key, "test-label", curve, decrypted_x));
    EXPECT_EQ(x, decrypted_x);
  }
  {
    ec_pve_t<crypto::rsa_kem_t> pve;

    pve.encrypt(rsa_pub_key, "test-label", curve, x);
    EXPECT_OK(pve.verify(rsa_pub_key, X, "test-label"));

    bn_t decrypted_x;
    EXPECT_OK(pve.decrypt(rsa_prv_key, "test-label", curve, decrypted_x));
    EXPECT_EQ(x, decrypted_x);
  }
  {
    ec_pve_t<crypto::ecies_t> pve;

    pve.encrypt(ecc_pub_key, "test-label", curve, x);
    EXPECT_OK(pve.verify(ecc_pub_key, X, "test-label"));

    bn_t decrypted_x;
    EXPECT_OK(pve.decrypt(ecc_prv_key, "test-label", curve, decrypted_x));
    EXPECT_EQ(x, decrypted_x);
  }
}

typedef PVE PVEBatch;

TEST_F(PVEBatch, Completeness) {
  int n = 20;
  for (const auto &[pub_key, prv_key] : valid_keys) {
    pve_batch_t pve_batch(n);
    std::vector<bn_t> xs(n);
    std::vector<ecc_point_t> Xs(n);
    for (int i = 0; i < n; i++) {
      xs[i] = (i > n / 2) ? bn_t(i) : bn_t::rand(q);
      Xs[i] = xs[i] * G;
    }

    pve_batch.encrypt(pub_key, "test-label", curve, xs);
    EXPECT_OK(pve_batch.verify(pub_key, Xs, "test-label"));

    std::vector<bn_t> decrypted_xs;
    EXPECT_OK(pve_batch.decrypt(prv_key, "test-label", curve, decrypted_xs));
    EXPECT_EQ(xs, decrypted_xs);
  }
}

TEST_F(PVEBatch, VerifyWithWrongLabel) {
  for (const auto &[pub_key, prv_key] : valid_keys) {
    pve_batch_t pve_batch(1);
    bn_t x = bn_t::rand(q);
    ecc_point_t X = x * G;

    pve_batch.encrypt(pub_key, "test-label", curve, {x});
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve_batch.verify(pub_key, {X}, "wrong-label"));
  }
}

TEST_F(PVEBatch, VerifyWithWrongPublicKey) {
  for (const auto &[pub_key, prv_key] : valid_keys) {
    pve_batch_t pve_batch(1);
    bn_t x = bn_t::rand(q);
    ecc_point_t X = x * G;

    pve_batch.encrypt(pub_key, "test-label", curve, {x});
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve_batch.verify(pub_key, {bn_t::rand(q) * G}, "test-label"));
  }
}

TEST_F(PVEBatch, DecryptWithWrongLabel) {
  for (const auto &[pub_key, prv_key] : valid_keys) {
    pve_batch_t pve_batch(1);
    std::vector<bn_t> xs = {bn_t::rand(q)};

    pve_batch.encrypt(pub_key, "test-label", curve, xs);

    std::vector<bn_t> decrypted_xs;
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(pve_batch.decrypt(prv_key, "wrong-label", curve, decrypted_xs));
    EXPECT_NE(xs, decrypted_xs);
  }
}

}  // namespace
