#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/lagrange.h>
#include <cbmpc/internal/crypto/secret_sharing.h>

#include "utils/data/ac.h"
#include "utils/test_macros.h"

namespace {

using namespace coinbase::crypto;
using namespace coinbase::crypto::ss;
using coinbase::buf_t;

class SSNode : public coinbase::testutils::TestNodes {};

TEST_F(SSNode, ValidateTestNodes) {
  EXPECT_OK(simple_and_node->validate_tree());
  EXPECT_EQ(simple_and_node->get_n(), 3);

  EXPECT_EQ(simple_and_node->get_path(), "");
  EXPECT_EQ(simple_and_node->get_sorted_children()[0]->get_path(), "/leaf1");
  EXPECT_EQ(simple_and_node->get_sorted_children()[1]->get_path(), "/leaf2");
  EXPECT_EQ(simple_and_node->get_sorted_children()[2]->get_path(), "/leaf3");

  EXPECT_OK(simple_or_node->validate_tree());
  EXPECT_OK(simple_threshold_node->validate_tree());
  EXPECT_OK(test_root->validate_tree());

  EXPECT_EQ(simple_or_node->get_n(), 3);
  EXPECT_EQ(simple_threshold_node->get_n(), 3);
  EXPECT_EQ(test_root->get_n(), 3);
}

TEST_F(SSNode, InvalidNode) {
  node_t root(node_e::AND, "root", 0);
  node_t* child1 = new node_t(node_e::LEAF, "child1", 0);
  node_t* child2 = new node_t(node_e::LEAF, "child2", 0);

  root.add_child_node(child1);
  root.add_child_node(child2);

  EXPECT_ER(root.validate_tree());  // root shouldn't have name
  root.name = "";
  EXPECT_OK(root.validate_tree());

  node_t* child3 = new node_t(node_e::THRESHOLD, "child3", 2);
  root.add_child_node(child3);
  EXPECT_ER(root.validate_tree());  // threshold node with no child
  node_t* child31 = new node_t(node_e::LEAF, "child31", 0);
  child3->add_child_node(child31);
  EXPECT_ER(root.validate_tree());  // threshold node with not enough child
  node_t* child32 = new node_t(node_e::LEAF, "child32", 0);
  child3->add_child_node(child32);
  EXPECT_OK(root.validate_tree());  // threshold node with not enough child

  EXPECT_OK(test_root->validate_tree());
}

TEST_F(SSNode, InvalidNodeKindsAreRejected) {
  {
    node_t root(node_e::AND, "", 0, {new node_t(node_e::LEAF, "")});
    EXPECT_ER(root.validate_tree());
  }
  {
    node_t root(node_e::AND, "", 0, {new node_t(node_e::LEAF, "dup"), new node_t(node_e::LEAF, "dup")});
    EXPECT_ER(root.validate_tree());
  }
  {
    node_t root(node_e::LEAF, "", 1);
    EXPECT_ER(root.validate_tree());
  }
  {
    node_t root(node_e::LEAF, "", 0, {new node_t(node_e::LEAF, "child")});
    EXPECT_ER(root.validate_tree());
  }
  {
    node_t root(node_e::AND, "", 1, {new node_t(node_e::LEAF, "child")});
    EXPECT_ER(root.validate_tree());
  }
  {
    node_t root(node_e::AND, "", 0);
    EXPECT_ER(root.validate_tree());
  }
  {
    node_t root(node_e::OR, "", 1, {new node_t(node_e::LEAF, "child")});
    EXPECT_ER(root.validate_tree());
  }
  {
    node_t root(node_e::OR, "", 0);
    EXPECT_ER(root.validate_tree());
  }
  {
    node_t root(node_e::THRESHOLD, "", 0, {new node_t(node_e::LEAF, "child")});
    EXPECT_ER(root.validate_tree());
  }
  {
    node_t root(node_e::NONE, "");
    EXPECT_ER(root.validate_tree());
    EXPECT_FALSE(root.enough_for_quorum({"leaf"}));
  }
}

TEST_F(SSNode, NodePathsFindAndRemoval) {
  node_t root(node_e::AND, "", 0,
              {
                  new node_t(node_e::LEAF, "leaf1"),
                  new node_t(node_e::OR, "branch", 0,
                             {
                                 new node_t(node_e::LEAF, "leaf2"),
                                 new node_t(node_e::LEAF, "leaf3"),
                             }),
              });

  EXPECT_EQ(root.get_path(), "");
  EXPECT_EQ(root.find("branch")->get_path(), "/branch");
  EXPECT_EQ(root.find("leaf2")->get_path(), "/branch/leaf2");
  EXPECT_EQ(root.find("missing"), nullptr);

  const std::vector<std::string> paths = root.list_leaf_paths();
  EXPECT_EQ(paths, (std::vector<std::string>{"/leaf1", "/branch/leaf2", "/branch/leaf3"}));
  EXPECT_EQ(node_t::pid_from_path("/branch/leaf2"), node_t(node_e::LEAF, "leaf2").get_pid());

  node_t* removed = new node_t(node_e::LEAF, "removed");
  root.add_child_node(removed);
  ASSERT_EQ(root.get_n(), 3);
  removed->remove_and_delete();
  EXPECT_EQ(root.get_n(), 2);
  EXPECT_EQ(root.find("removed"), nullptr);
}

TEST_F(SSNode, NodeClone) {
  for (const auto& root : all_roots) {
    node_t* clone = root->clone();
    EXPECT_EQ(clone->children.size(), root->children.size());
    delete clone;
  }
}

class SecretSharing : public coinbase::testutils::TestAC {
 protected:
  mod_t q;
  bn_t x;
  int n;
  void SetUp() override {
    coinbase::testutils::TestAC::SetUp();
    ecurve_t curve = curve_secp256k1;
    // Initialize q, x, n, and drbg as needed for your tests
    q = curve.order();
    x = bn_t::rand(q);
    n = 5;
  }

  bool correctly_reconstructable(const ac_t& ac_ref, const ac_shares_t& shares, const ss::node_t* root) {
    bn_t reconstructed_x;

    if (ac_ref.enough_for_quorum(shares)) {
      if (auto rv = ac_ref.reconstruct(q, shares, reconstructed_x); rv) {
        return false;
      }
      return reconstructed_x == x;
    }
    return false;
  }
};

TEST_F(SecretSharing, ListLeaves) {
  ac_t ac(test_root);
  auto leaves = ac.list_leaf_names();
  EXPECT_EQ(leaves.size(), 24);
  for (const auto& leaf : leaves) {
    EXPECT_TRUE(test_root->find(leaf));
  }
  std::set<std::string> leaves_set(leaves.begin(), leaves.end());
  EXPECT_EQ(leaves_set.count("leaf1"), 1);
  EXPECT_EQ(leaves_set.count("leaf211"), 1);
  EXPECT_EQ(leaves_set.count("leaf212"), 1);
  EXPECT_EQ(leaves_set.count("leaf213"), 1);
  EXPECT_EQ(leaves_set.count("leaf214"), 1);
  EXPECT_EQ(leaves_set.count("leaf215"), 1);
  EXPECT_EQ(leaves_set.count("leaf22"), 1);
  EXPECT_EQ(leaves_set.count("leaf231"), 1);
  EXPECT_EQ(leaves_set.count("leaf232"), 1);
  EXPECT_EQ(leaves_set.count("leaf233"), 1);
  EXPECT_EQ(leaves_set.count("leaf234"), 1);
  EXPECT_EQ(leaves_set.count("leaf235"), 1);
  EXPECT_EQ(leaves_set.count("leaf236"), 1);
  EXPECT_EQ(leaves_set.count("leaf237"), 1);
  EXPECT_EQ(leaves_set.count("leaf238"), 1);
  EXPECT_EQ(leaves_set.count("leaf239"), 1);
  EXPECT_EQ(leaves_set.count("leaf311"), 1);
  EXPECT_EQ(leaves_set.count("leaf312"), 1);
  EXPECT_EQ(leaves_set.count("leaf32"), 1);
  EXPECT_EQ(leaves_set.count("leaf331"), 1);
  EXPECT_EQ(leaves_set.count("leaf332"), 1);
  EXPECT_EQ(leaves_set.count("leaf341"), 1);
  EXPECT_EQ(leaves_set.count("leaf342"), 1);
  EXPECT_EQ(leaves_set.count("leaf343"), 1);
}

TEST_F(SecretSharing, ListPubDataNodes) {
  ac_t ac(test_root);
  auto nodes = ac.list_pub_data_nodes();
  EXPECT_EQ(nodes.size(), 6);

  std::set<pname_t> node_names;
  for (auto node : nodes) node_names.insert(node->name);

  EXPECT_EQ(node_names.count(""), 1);
  EXPECT_EQ(node_names.count("and21"), 1);
  EXPECT_EQ(node_names.count("th23"), 1);
  EXPECT_EQ(node_names.count("th3"), 1);
  EXPECT_EQ(node_names.count("and31"), 1);
  EXPECT_EQ(node_names.count("th34"), 1);
}

TEST_F(SecretSharing, ShareAnd) {
  std::vector<bn_t> shares = share_and(q, x, n, nullptr);
  EXPECT_EQ(shares.size(), n);

  bn_t sum = 0;
  for (const auto& share : shares) {
    MODULO(q) sum += share;
  }
  EXPECT_EQ(sum, x);
}

TEST_F(SecretSharing, ShareThreshold) {
  int threshold = 3;                          // Example value
  std::vector<bn_t> pids = {1, 3, 8, 10, 5};  // Example values
  ASSERT_EQ(pids.size(), n);

  auto [shares, b] = share_threshold(q, x, threshold, n, pids, nullptr);
  EXPECT_EQ(shares.size(), n);
  EXPECT_EQ(b.size(), threshold);
  EXPECT_EQ(x, b[0]);
  for (int i = 0; i < n; i++) EXPECT_EQ(shares[i], horner_poly(q, b, pids[i]));
}

TEST_F(SecretSharing, ShareHelpersUseDrbgDeterministically) {
  const buf_t seed = bn_t(0x123456).to_bin(32);
  drbg_aes_ctr_t drbg1(seed);
  drbg_aes_ctr_t drbg2(seed);

  const std::vector<bn_t> shares1 = share_and(q, x, n, &drbg1);
  const std::vector<bn_t> shares2 = share_and(q, x, n, &drbg2);
  EXPECT_EQ(shares1, shares2);

  bn_t sum = 0;
  for (const auto& share : shares1) {
    MODULO(q) sum += share;
  }
  EXPECT_EQ(sum, x);

  drbg_aes_ctr_t threshold_drbg1(seed);
  drbg_aes_ctr_t threshold_drbg2(seed);
  const std::vector<bn_t> pids = {1, 3, 8, 10, 5};
  const auto [threshold_shares1, coeffs1] = share_threshold(q, x, 3, n, pids, &threshold_drbg1);
  const auto [threshold_shares2, coeffs2] = share_threshold(q, x, 3, n, pids, &threshold_drbg2);

  EXPECT_EQ(threshold_shares1, threshold_shares2);
  EXPECT_EQ(coeffs1, coeffs2);
  EXPECT_EQ(x, coeffs1[0]);
}

TEST_F(SecretSharing, AccessStructureAccessorsAndValidation) {
  ac_t empty;
  EXPECT_FALSE(empty.has_root());
  EXPECT_FALSE(empty.has_curve());
  EXPECT_EQ(empty.get_root(), nullptr);
  EXPECT_FALSE(empty.enough_for_quorum(std::set<pname_t>{"leaf1"}));
  EXPECT_FALSE(empty.enough_for_quorum(ac_shares_t{{"leaf1", bn_t(1)}}));
  EXPECT_ER(empty.validate_tree());

  ac_t curve_only(curve_secp256k1);
  EXPECT_FALSE(curve_only.has_root());
  EXPECT_TRUE(curve_only.has_curve());
  EXPECT_EQ(curve_only.get_curve(), curve_secp256k1);
  EXPECT_FALSE(curve_only.enough_for_quorum(std::set<pname_t>{"leaf1"}));
  EXPECT_ER(curve_only.validate_tree());

  ac_t missing_curve(simple_and_node);
  EXPECT_TRUE(missing_curve.has_root());
  EXPECT_FALSE(missing_curve.has_curve());
  EXPECT_ER(missing_curve.validate_tree());

  ac_t ac(simple_threshold_node, curve_secp256k1);
  EXPECT_EQ(ac.get_root(), simple_threshold_node);
  EXPECT_TRUE(ac.has_curve());
  EXPECT_EQ(ac.get_curve(), curve_secp256k1);
  EXPECT_OK(ac.validate_tree());
  EXPECT_TRUE(ac.enough_for_quorum(std::set<pname_t>{"leaf1", "leaf2"}));
  EXPECT_FALSE(ac.enough_for_quorum(std::set<pname_t>{"leaf1"}));

  EXPECT_EQ(ac.get_pub_data_size(simple_and_node), simple_and_node->get_n());
  EXPECT_EQ(ac.get_pub_data_size(simple_threshold_node), simple_threshold_node->threshold);
  EXPECT_EQ(ac.get_pub_data_size(simple_threshold_node->find("leaf1")), 0);
}

TEST_F(SecretSharing, ACShare) {
  ac_t ac(test_root);
  ac_shares_t shares = ac.share(q, x, nullptr);

  EXPECT_EQ(shares.size(), 24);  // Only leaf nodes have private shares
  EXPECT_TRUE(shares.find("leaf1") != shares.end());
  EXPECT_TRUE(shares.find("leaf211") != shares.end());
  EXPECT_TRUE(shares.find("leaf212") != shares.end());
  EXPECT_TRUE(shares.find("leaf213") != shares.end());
  EXPECT_TRUE(shares.find("leaf214") != shares.end());
  EXPECT_TRUE(shares.find("leaf215") != shares.end());
  EXPECT_TRUE(shares.find("leaf22") != shares.end());
  EXPECT_TRUE(shares.find("leaf231") != shares.end());
  EXPECT_TRUE(shares.find("leaf232") != shares.end());
  EXPECT_TRUE(shares.find("leaf233") != shares.end());
  EXPECT_TRUE(shares.find("leaf234") != shares.end());
  EXPECT_TRUE(shares.find("leaf235") != shares.end());
  EXPECT_TRUE(shares.find("leaf236") != shares.end());
  EXPECT_TRUE(shares.find("leaf237") != shares.end());
  EXPECT_TRUE(shares.find("leaf238") != shares.end());
  EXPECT_TRUE(shares.find("leaf239") != shares.end());
  EXPECT_TRUE(shares.find("leaf311") != shares.end());
  EXPECT_TRUE(shares.find("leaf312") != shares.end());
  EXPECT_TRUE(shares.find("leaf32") != shares.end());
  EXPECT_TRUE(shares.find("leaf331") != shares.end());
  EXPECT_TRUE(shares.find("leaf332") != shares.end());
  EXPECT_TRUE(shares.find("leaf341") != shares.end());
  EXPECT_TRUE(shares.find("leaf342") != shares.end());
  EXPECT_TRUE(shares.find("leaf343") != shares.end());

  bn_t reconstructed_x;
  EXPECT_OK(ac.reconstruct(q, shares, reconstructed_x));
  EXPECT_EQ(reconstructed_x, x);
}

TEST_F(SecretSharing, ACEnoughQuorumAndReconstruct) {
  ac_t ac(test_root);
  ac_shares_t shares = ac.share(q, x, nullptr);

  EXPECT_TRUE(correctly_reconstructable(ac, shares, test_root));

  shares.erase("leaf211");
  shares.erase("leaf212");
  shares.erase("leaf213");
  shares.erase("leaf214");
  shares.erase("leaf215");
  shares.erase("leaf22");
  shares.erase("leaf231");
  shares.erase("leaf233");
  shares.erase("leaf235");
  shares.erase("leaf237");
  shares.erase("leaf239");
  EXPECT_TRUE(ac.enough_for_quorum(shares));
  EXPECT_TRUE(correctly_reconstructable(ac, shares, test_root));

  auto shares_backup = shares;
  shares.erase("leaf232");
  EXPECT_FALSE(ac.enough_for_quorum(shares));
  EXPECT_FALSE(correctly_reconstructable(ac, shares, test_root));

  shares = shares_backup;
  shares.erase("leaf1");
  EXPECT_FALSE(ac.enough_for_quorum(shares));
  EXPECT_FALSE(correctly_reconstructable(ac, shares, test_root));

  shares = shares_backup;
  shares.erase("leaf32");
  shares.erase("leaf311");
  EXPECT_TRUE(ac.enough_for_quorum(shares));
  EXPECT_TRUE(correctly_reconstructable(ac, shares, test_root));

  shares.erase("leaf341");
  EXPECT_TRUE(ac.enough_for_quorum(shares));
  EXPECT_TRUE(correctly_reconstructable(ac, shares, test_root));
  shares.erase("leaf343");
  EXPECT_FALSE(ac.enough_for_quorum(shares));
  EXPECT_FALSE(correctly_reconstructable(ac, shares, test_root));

  shares = ac.share(q, x, nullptr);

  ac_shares_t minimal_shares;
  for (const auto& name : valid_quorum) {
    minimal_shares[name] = shares[name];
  }
  EXPECT_TRUE(ac.enough_for_quorum(minimal_shares));
  EXPECT_TRUE(correctly_reconstructable(ac, minimal_shares, test_root));

  ac_shares_t malicious_shares;
  for (const auto& name : valid_quorum) {
    malicious_shares[name] = bn_t::rand(q);
  }
  EXPECT_TRUE(ac.enough_for_quorum(malicious_shares));
  EXPECT_FALSE(correctly_reconstructable(ac, malicious_shares, test_root));
}

TEST_F(SecretSharing, ACReconstructExponentEd25519) {
  vartime_scope_t vartime_scope;
  ecurve_t curve = curve_ed25519;
  const mod_t q = curve.order();
  const bn_t x = bn_t::rand(q);

  ac_t ac(test_root);
  ac.curve = curve;

  const ac_shares_t shares = ac.share(q, x, nullptr);
  ac_pub_shares_t pub_shares;
  for (const auto& [name, si] : shares) {
    pub_shares[name] = si * curve.generator();
  }

  ecc_point_t P;
  EXPECT_OK(ac.reconstruct_exponent(pub_shares, P));
  EXPECT_EQ(P, x * curve.generator());
}

TEST_F(SecretSharing, ReconstructExpRejectsNonSubgroup) {
  vartime_scope_t vartime_scope;
  ecurve_t curve = curve_ed25519;
  const mod_t q = curve.order();
  const bn_t x = bn_t::rand(q);

  ac_t ac(test_root);
  ac.curve = curve;

  const ac_shares_t shares = ac.share(q, x, nullptr);
  ac_pub_shares_t pub_shares;
  for (const auto& [name, si] : shares) {
    pub_shares[name] = si * curve.generator();
  }

  // Ed25519 order-2 torsion point (x=0, y=-1): on-curve but not in the prime-order subgroup.
  uint8_t order2[32];
  order2[0] = 0xec;
  for (int i = 1; i < 31; i++) order2[i] = 0xff;
  order2[31] = 0x7f;

  ecc_point_t T(curve);
  ASSERT_EQ(T.from_bin(curve, coinbase::mem_t(order2, 32)), SUCCESS);
  ASSERT_TRUE(T.is_on_curve());
  ASSERT_FALSE(T.is_infinity());
  ASSERT_FALSE(T.is_in_subgroup());

  pub_shares["leaf1"] = T;

  ecc_point_t P;
  EXPECT_ER(ac.reconstruct_exponent(pub_shares, P));
}

TEST_F(SecretSharing, VerifyShareAcceptsValidShare) {
  ecurve_t curve = curve_secp256k1;
  ac_t ac(simple_and_node, curve);

  ac_shares_t shares;
  ac_internal_shares_t internal_shares;
  ac_internal_pub_shares_t pub_data;
  ASSERT_OK(ac.share_with_internals(q, x, shares, internal_shares, pub_data));

  vartime_scope_t vartime_scope;
  const ecc_point_t Q = x * curve.generator();
  EXPECT_OK(ac.verify_share_against_ancestors_pub_data(Q, shares.at("leaf1"), pub_data, "leaf1"));
}

TEST_F(SecretSharing, VerifyShareCoversThresholdPubData) {
  ecurve_t curve = curve_secp256k1;
  ac_t ac(simple_threshold_node, curve);

  ac_shares_t shares;
  ac_internal_shares_t internal_shares;
  ac_internal_pub_shares_t pub_data;
  ASSERT_OK(ac.share_with_internals(q, x, shares, internal_shares, pub_data));

  vartime_scope_t vartime_scope;
  const ecc_point_t Q = x * curve.generator();
  EXPECT_OK(ac.verify_share_against_ancestors_pub_data(Q, shares.at("leaf3"), pub_data, "leaf3"));

  ac_internal_pub_shares_t missing_quorum_pub_data = pub_data;
  missing_quorum_pub_data.erase("leaf1");
  EXPECT_ER(ac.verify_share_against_ancestors_pub_data(Q, shares.at("leaf3"), missing_quorum_pub_data, "leaf3"));

  ac_internal_pub_shares_t tampered_root_pub_data = pub_data;
  tampered_root_pub_data[""] = tampered_root_pub_data[""] + curve.generator();
  EXPECT_ER(ac.verify_share_against_ancestors_pub_data(Q, shares.at("leaf3"), tampered_root_pub_data, "leaf3"));
}

TEST_F(SecretSharing, ShareWithInternalsRejectsInvalidInputs) {
  ac_shares_t shares;
  ac_internal_shares_t internal_shares;
  ac_internal_pub_shares_t pub_data;

  ac_t missing_root(nullptr, curve_secp256k1);
  EXPECT_ER(missing_root.share_with_internals(q, x, shares, internal_shares, pub_data));

  ac_t missing_curve(simple_and_node);
  EXPECT_ER(missing_curve.share_with_internals(q, x, shares, internal_shares, pub_data));

  ac_t wrong_modulus(simple_and_node, curve_p256);
  EXPECT_ER(wrong_modulus.share_with_internals(q, x, shares, internal_shares, pub_data));

  ac_t ac(simple_and_node, curve_secp256k1);
  vartime_scope_t vartime_scope;
  EXPECT_ER(ac.verify_share_against_ancestors_pub_data(x * curve_p256.generator(), x, pub_data, "leaf1"));
  EXPECT_ER(ac.verify_share_against_ancestors_pub_data(x * curve_secp256k1.generator(), x, pub_data, "missing"));
}

TEST_F(SecretSharing, AcOwnedCopyMovePreservesTreeState) {
  ac_owned_t root_only(simple_and_node);
  EXPECT_TRUE(root_only.has_root());
  EXPECT_FALSE(root_only.has_curve());
  EXPECT_NE(root_only.get_root(), simple_and_node);
  EXPECT_EQ(root_only.list_leaf_names(), simple_and_node->list_leaf_names());
  EXPECT_ER(root_only.validate_tree());

  ac_owned_t owned(simple_and_node, curve_secp256k1);
  EXPECT_OK(owned.validate_tree());

  ac_owned_t copied = owned;
  EXPECT_OK(copied.validate_tree());
  EXPECT_EQ(copied.list_leaf_names(), owned.list_leaf_names());
  EXPECT_EQ(copied.get_curve(), curve_secp256k1);

  ac_owned_t moved = std::move(owned);
  EXPECT_OK(moved.validate_tree());
  EXPECT_EQ(moved.get_curve(), curve_secp256k1);
}

TEST_F(SecretSharing, AcOwnedAssignmentAndSerializationPreserveTreeState) {
  ac_owned_t empty;
  const buf_t empty_bin = coinbase::convert(empty);
  ac_owned_t empty_roundtrip;
  ASSERT_OK(coinbase::convert(empty_roundtrip, empty_bin));
  EXPECT_FALSE(empty_roundtrip.has_root());
  EXPECT_FALSE(empty_roundtrip.has_curve());

  ac_t threshold_ref(simple_threshold_node, curve_secp256k1);
  ac_owned_t owned(threshold_ref);
  EXPECT_OK(owned.validate_tree());

  ac_owned_t assigned;
  assigned = owned;
  EXPECT_OK(assigned.validate_tree());
  EXPECT_EQ(assigned.list_leaf_names(), owned.list_leaf_names());
  EXPECT_EQ(assigned.get_curve(), curve_secp256k1);

  assigned = assigned;
  EXPECT_OK(assigned.validate_tree());
  EXPECT_EQ(assigned.list_leaf_names(), owned.list_leaf_names());

  const buf_t encoded = coinbase::convert(assigned);
  ac_owned_t decoded;
  ASSERT_OK(coinbase::convert(decoded, encoded));
  EXPECT_OK(decoded.validate_tree());
  EXPECT_EQ(decoded.list_leaf_names(), owned.list_leaf_names());
  EXPECT_EQ(decoded.get_curve(), curve_secp256k1);

  ac_owned_t moved_target(simple_and_node, curve_p256);
  moved_target = std::move(decoded);
  EXPECT_OK(moved_target.validate_tree());
  EXPECT_EQ(moved_target.list_leaf_names(), owned.list_leaf_names());
  EXPECT_EQ(moved_target.get_curve(), curve_secp256k1);

  moved_target = std::move(moved_target);
  EXPECT_OK(moved_target.validate_tree());
  EXPECT_EQ(moved_target.list_leaf_names(), owned.list_leaf_names());
}

TEST_F(SecretSharing, VerifyShareMissingPubDataKeyCrashes) {
  ecurve_t curve = curve_secp256k1;
  ac_t ac(simple_and_node, curve);

  ac_shares_t shares;
  ac_internal_shares_t internal_shares;
  ac_internal_pub_shares_t pub_data;
  ASSERT_OK(ac.share_with_internals(q, x, shares, internal_shares, pub_data));

  pub_data.erase("leaf2");

  vartime_scope_t vartime_scope;
  ecc_point_t Q = x * curve.generator();

  EXPECT_NO_THROW({
    auto rv = ac.verify_share_against_ancestors_pub_data(Q, shares.at("leaf1"), pub_data, "leaf1");
    EXPECT_ER(rv);
  });
}

TEST_F(SecretSharing, ShareThresholdRejectsNegativeN) {
  // share_threshold() should validate that n > 0
  // Negative n values would convert to SIZE_MAX when constructing std::vector<bn_t>(n)
  // causing std::bad_alloc or std::length_error

  int threshold = 3;
  std::vector<bn_t> pids = {1, 3, 8};  // Dummy PIDs

  // Should throw assertion_failed_t for negative n
  EXPECT_THROW({ share_threshold(q, x, threshold, -1, pids, nullptr); }, coinbase::assertion_failed_t);

  EXPECT_THROW({ share_threshold(q, x, threshold, -100, pids, nullptr); }, coinbase::assertion_failed_t);

  // Valid n should work
  {
    auto result = share_threshold(q, x, threshold, 3, pids, nullptr);
    EXPECT_EQ(result.first.size(), 3);
    EXPECT_EQ(result.second.size(), threshold);
  }
}

}  // namespace
