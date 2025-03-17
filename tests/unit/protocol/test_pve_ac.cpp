#include <gtest/gtest.h>

#include <cbmpc/core/log.h>
#include <cbmpc/protocol/pve.h>
#include <cbmpc/protocol/pve_ac.h>
#include <cbmpc/protocol/util.h>

#include "utils/data/ac.h"
#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;
using namespace coinbase::mpc;
using namespace coinbase::testutils;

namespace {

class PVEAC : public testutils::TestAC {
 protected:
  void SetUp() override {
    testutils::TestNodes::SetUp();
    curve = crypto::curve_p256;
    q = curve.order();
    G = curve.generator();
  }

  ecurve_t curve;
  mod_t q;
  ecc_generator_point_t G;
  crypto::prv_key_t get_prv_key(int participant_index) const {
    if (participant_index & 1)
      return crypto::prv_key_t::from(get_ecc_prv_key(participant_index));
    else
      return crypto::prv_key_t::from(get_rsa_prv_key(participant_index));
  }

  crypto::ecc_prv_key_t get_ecc_prv_key(int participant_index) const {
    crypto::ecc_prv_key_t prv_key_ecc;
    prv_key_ecc.generate(crypto::curve_p256);
    return prv_key_ecc;
  }

  crypto::rsa_prv_key_t get_rsa_prv_key(int participant_index) const {
    crypto::rsa_prv_key_t prv_key_rsa;
    prv_key_rsa.generate(2048);
    return prv_key_rsa;
  }
};

TEST_F(PVEAC, PKI) {
  error_t rv = UNINITIALIZED_ERROR;
  ss::ac_t ac(test_root);

  auto leaves = ac.list_leaf_names();
  std::map<std::string, crypto::pub_key_t> pub_keys;
  std::map<std::string, crypto::prv_key_t> prv_keys;

  int participant_index = 0;
  for (auto path : leaves) {
    auto prv_key = get_prv_key(participant_index);
    if (!ac.enough_for_quorum(pub_keys)) {
      prv_keys[path] = prv_key;
    }
    pub_keys[path] = prv_key.pub();
    participant_index++;
  }

  const int n = 20;
  ec_pve_ac_t<hybrid_cipher_t> pve;
  std::vector<bn_t> xs(n);
  std::vector<ecc_point_t> Xs(n);
  for (int i = 0; i < n; i++) {
    xs[i] = bn_t::rand(q);
    Xs[i] = xs[i] * G;
  }

  std::string label = "test-label";
  pve.encrypt(ac, pub_keys, label, curve, xs);
  rv = pve.verify(ac, pub_keys, Xs, label);
  EXPECT_EQ(rv, 0);

  std::vector<bn_t> decrypted_xs;
  rv = pve.decrypt(ac, prv_keys, pub_keys, label, decrypted_xs);
  EXPECT_EQ(rv, 0);
  ASSERT_EQ(rv, 0);
  EXPECT_TRUE(xs == decrypted_xs);
}

TEST_F(PVEAC, ECC) {
  error_t rv = UNINITIALIZED_ERROR;
  ss::ac_t ac(test_root);

  auto leaves = ac.list_leaf_names();
  std::map<std::string, crypto::ecc_pub_key_t> pub_keys;
  std::map<std::string, crypto::ecc_prv_key_t> prv_keys;

  int participant_index = 0;
  for (auto path : leaves) {
    auto prv_key = get_ecc_prv_key(participant_index);
    if (!ac.enough_for_quorum(pub_keys)) {
      prv_keys[path] = prv_key;
    }
    pub_keys[path] = prv_key.pub();
    participant_index++;
    std::cout << "path -- orig: " << path << ", pub_key: " << pub_keys[path].to_oct() << std::endl;
  }

  const int n = 20;
  ec_pve_ac_t<ecies_t> pve;
  std::vector<bn_t> xs(n);
  std::vector<ecc_point_t> Xs(n);
  for (int i = 0; i < n; i++) {
    xs[i] = bn_t::rand(q);
    Xs[i] = xs[i] * G;
  }

  std::string label = "test-label";
  pve.encrypt(ac, pub_keys, label, curve, xs);
  rv = pve.verify(ac, pub_keys, Xs, label);
  EXPECT_EQ(rv, 0);

  std::vector<bn_t> decrypted_xs;
  rv = pve.decrypt(ac, prv_keys, pub_keys, label, decrypted_xs);
  ASSERT_EQ(rv, 0);
  EXPECT_TRUE(xs == decrypted_xs);
}

TEST_F(PVEAC, RSA) {
  error_t rv = UNINITIALIZED_ERROR;
  ss::ac_t ac(test_root);

  auto leaves = ac.list_leaf_names();
  std::map<std::string, crypto::rsa_pub_key_t> pub_keys;
  std::map<std::string, crypto::rsa_prv_key_t> prv_keys;

  int participant_index = 0;
  for (auto path : leaves) {
    auto prv_key = get_rsa_prv_key(participant_index);
    if (!ac.enough_for_quorum(pub_keys)) {
      prv_keys[path] = prv_key;
    }
    pub_keys[path] = prv_key.pub();
    participant_index++;
  }

  const int n = 20;
  ec_pve_ac_t<rsa_kem_t> pve;
  std::vector<bn_t> xs(n);
  std::vector<ecc_point_t> Xs(n);
  for (int i = 0; i < n; i++) {
    xs[i] = bn_t::rand(q);
    Xs[i] = xs[i] * G;
  }

  std::string label = "test-label";
  pve.encrypt(ac, pub_keys, label, curve, xs);
  rv = pve.verify(ac, pub_keys, Xs, label);
  EXPECT_EQ(rv, 0);

  std::vector<bn_t> decrypted_xs;
  rv = pve.decrypt(ac, prv_keys, pub_keys, label, decrypted_xs);
  ASSERT_EQ(rv, 0);
  EXPECT_TRUE(xs == decrypted_xs);
}

}  // namespace
