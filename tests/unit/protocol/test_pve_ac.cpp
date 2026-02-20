#include <gtest/gtest.h>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/protocol/pve.h>
#include <cbmpc/internal/protocol/pve_ac.h>
#include <cbmpc/internal/protocol/util.h>

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

  for (auto &kv : pub_keys_val) pub_keys[kv.first] = pve_keyref(kv.second);
  for (auto &kv : prv_keys_val) prv_keys[kv.first] = pve_keyref(kv.second);

  const int n = 20;
  ec_pve_ac_t pve;
  std::vector<bn_t> xs(n);
  std::vector<ecc_point_t> Xs(n);
  for (int i = 0; i < n; i++) {
    xs[i] = bn_t::rand(q);
    Xs[i] = xs[i] * G;
  }

  std::string label = "test-label";
  rv = pve.encrypt(pve_base_pke_ecies(), ac, pub_keys, label, curve, xs);
  ASSERT_EQ(rv, 0);
  rv = pve.verify(pve_base_pke_ecies(), ac, pub_keys, Xs, label);
  EXPECT_EQ(rv, 0);

  int row_index = 0;
  crypto::ss::party_map_t<bn_t> shares;
  for (auto &[path, prv_key] : prv_keys) {
    bn_t share;
    rv = pve.party_decrypt_row(pve_base_pke_ecies(), ac, row_index, path, prv_key, label, share);
    ASSERT_EQ(rv, 0);
    shares[path] = share;
  }
  std::vector<bn_t> decrypted_xs;
  rv = pve.aggregate_to_restore_row(pve_base_pke_ecies(), ac, row_index, label, shares, decrypted_xs,
                                    /*skip_verify=*/true);
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

  for (auto &kv : pub_keys_val) pub_keys[kv.first] = pve_keyref(kv.second);
  for (auto &kv : prv_keys_val) prv_keys[kv.first] = pve_keyref(kv.second);

  const int n = 20;
  ec_pve_ac_t pve;
  std::vector<bn_t> xs(n);
  std::vector<ecc_point_t> Xs(n);
  for (int i = 0; i < n; i++) {
    xs[i] = bn_t::rand(q);
    Xs[i] = xs[i] * G;
  }

  std::string label = "test-label";
  rv = pve.encrypt(pve_base_pke_rsa(), ac, pub_keys, label, curve, xs);
  ASSERT_EQ(rv, 0);
  rv = pve.verify(pve_base_pke_rsa(), ac, pub_keys, Xs, label);
  EXPECT_EQ(rv, 0);

  int row_index = 0;
  crypto::ss::party_map_t<bn_t> shares;
  for (auto &[path, prv_key] : prv_keys) {
    bn_t share;
    rv = pve.party_decrypt_row(pve_base_pke_rsa(), ac, row_index, path, prv_key, label, share);
    ASSERT_EQ(rv, 0);
    shares[path] = share;
  }
  std::vector<bn_t> decrypted_xs;
  rv = pve.aggregate_to_restore_row(pve_base_pke_rsa(), ac, row_index, label, shares, decrypted_xs,
                                    /*skip_verify=*/true);
  ASSERT_EQ(rv, 0);
  EXPECT_TRUE(xs == decrypted_xs);
}

TEST_F(PVEAC, AggFail_VerifyOn_MissingLeafPub) {
  error_t rv = UNINITIALIZED_ERROR;
  ss::ac_t ac(test_root);

  auto leaves = ac.list_leaf_names();
  std::map<std::string, crypto::ecc_pub_key_t> pub_keys_val;
  std::map<std::string, crypto::ecc_prv_key_t> prv_keys_val;
  ec_pve_ac_t::pks_t pub_keys;
  ec_pve_ac_t::sks_t prv_keys;

  int participant_index = 0;
  for (const auto &path : leaves) {
    auto prv_key = get_ecc_prv_key(participant_index);
    if (!ac.enough_for_quorum(pub_keys_val)) {
      prv_keys_val[path] = prv_key;
    }
    pub_keys_val[path] = prv_key.pub();
    participant_index++;
  }

  for (const auto &kv : pub_keys_val) pub_keys[kv.first] = pve_keyref(kv.second);
  for (const auto &kv : prv_keys_val) prv_keys[kv.first] = pve_keyref(kv.second);

  const int n = 8;
  ec_pve_ac_t pve;
  std::vector<bn_t> xs(n);
  for (int i = 0; i < n; i++) xs[i] = bn_t::rand(q);

  std::string label = "test-label";
  ASSERT_OK(pve.encrypt(pve_base_pke_ecies(), ac, pub_keys, label, curve, xs));

  int row_index = 0;
  crypto::ss::party_map_t<bn_t> shares;
  for (auto &[path, prv_key] : prv_keys) {
    bn_t share;
    ASSERT_OK(pve.party_decrypt_row(pve_base_pke_ecies(), ac, row_index, path, prv_key, label, share));
    shares[path] = share;
  }

  std::vector<bn_t> decrypted_xs;
  EXPECT_ER(pve.aggregate_to_restore_row(pve_base_pke_ecies(), ac, row_index, label, shares, decrypted_xs,
                                         /*skip_verify=*/false,
                                         /*all_ac_pks=*/ec_pve_ac_t::pks_t{}));
}

}  // namespace
