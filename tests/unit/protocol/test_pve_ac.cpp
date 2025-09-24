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
  std::map<std::string, crypto::pub_key_t> pub_keys_val;
  std::map<std::string, crypto::prv_key_t> prv_keys_val;
  ec_pve_ac_t::pks_t pub_keys;
  ec_pve_ac_t::sks_t prv_keys;

  int participant_index = 0;
  for (auto path : leaves) {
    auto prv_key = get_prv_key(participant_index);
    if (!ac.enough_for_quorum(pub_keys_val)) {
      prv_keys_val[path] = prv_key;
    }
    pub_keys_val[path] = prv_key.pub();
    participant_index++;
  }

  for (auto &kv : pub_keys_val) pub_keys[kv.first] = &kv.second;
  for (auto &kv : prv_keys_val) prv_keys[kv.first] = &kv.second;

  const int n = 20;
  ec_pve_ac_t pve;
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

  int row_index = 0;
  crypto::ss::party_map_t<bn_t> shares;
  for (auto &[path, prv_key] : prv_keys) {
    bn_t share;
    rv = pve.party_decrypt_row(ac, row_index, path, prv_key, label, share);
    ASSERT_EQ(rv, 0);
    shares[path] = share;
  }
  std::vector<bn_t> decrypted_xs;
  rv = pve.aggregate_to_restore_row(ac, row_index, label, shares, decrypted_xs, /*skip_verify=*/true);
  ASSERT_EQ(rv, 0);
  EXPECT_TRUE(xs == decrypted_xs);
}

TEST_F(PVEAC, ECC) {
  error_t rv = UNINITIALIZED_ERROR;
  ss::ac_t ac(test_root);

  auto leaves = ac.list_leaf_names();
  std::map<std::string, crypto::ecc_pub_key_t> pub_keys_val;
  std::map<std::string, crypto::ecc_prv_key_t> prv_keys_val;
  ec_pve_ac_t::pks_t pub_keys;
  ec_pve_ac_t::sks_t prv_keys;

  int participant_index = 0;
  for (auto path : leaves) {
    auto prv_key = get_ecc_prv_key(participant_index);
    if (!ac.enough_for_quorum(pub_keys_val)) {
      prv_keys_val[path] = prv_key;
    }
    pub_keys_val[path] = prv_key.pub();
    participant_index++;
  }

  for (auto &kv : pub_keys_val) pub_keys[kv.first] = &kv.second;
  for (auto &kv : prv_keys_val) prv_keys[kv.first] = &kv.second;

  const int n = 20;
  ec_pve_ac_t pve(pve_base_pke_ecies());
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

  int row_index = 0;
  crypto::ss::party_map_t<bn_t> shares;
  for (auto &[path, prv_key] : prv_keys) {
    bn_t share;
    rv = pve.party_decrypt_row(ac, row_index, path, prv_key, label, share);
    ASSERT_EQ(rv, 0);
    shares[path] = share;
  }
  std::vector<bn_t> decrypted_xs;
  rv = pve.aggregate_to_restore_row(ac, row_index, label, shares, decrypted_xs, /*skip_verify=*/true);
  ASSERT_EQ(rv, 0);
  EXPECT_TRUE(xs == decrypted_xs);
}

TEST_F(PVEAC, RSA) {
  error_t rv = UNINITIALIZED_ERROR;
  ss::ac_t ac(test_root);

  auto leaves = ac.list_leaf_names();
  std::map<std::string, crypto::rsa_pub_key_t> pub_keys_val;
  std::map<std::string, crypto::rsa_prv_key_t> prv_keys_val;
  ec_pve_ac_t::pks_t pub_keys;
  ec_pve_ac_t::sks_t prv_keys;

  int participant_index = 0;
  for (auto path : leaves) {
    auto prv_key = get_rsa_prv_key(participant_index);
    if (!ac.enough_for_quorum(pub_keys_val)) {
      prv_keys_val[path] = prv_key;
    }
    pub_keys_val[path] = prv_key.pub();
    participant_index++;
  }

  for (auto &kv : pub_keys_val) pub_keys[kv.first] = &kv.second;
  for (auto &kv : prv_keys_val) prv_keys[kv.first] = &kv.second;

  const int n = 20;
  ec_pve_ac_t pve(pve_base_pke_rsa());
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

  int row_index = 0;
  crypto::ss::party_map_t<bn_t> shares;
  for (auto &[path, prv_key] : prv_keys) {
    bn_t share;
    rv = pve.party_decrypt_row(ac, row_index, path, prv_key, label, share);
    ASSERT_EQ(rv, 0);
    shares[path] = share;
  }
  std::vector<bn_t> decrypted_xs;
  rv = pve.aggregate_to_restore_row(ac, row_index, label, shares, decrypted_xs, /*skip_verify=*/true);
  ASSERT_EQ(rv, 0);
  EXPECT_TRUE(xs == decrypted_xs);
}

}  // namespace
