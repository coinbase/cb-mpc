#include <gtest/gtest.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/lagrange.h>
#include <cbmpc/crypto/secret_sharing.h>

#include "utils/data/ac.h"
#include "utils/test_macros.h"

namespace {

using namespace coinbase::crypto;
using namespace coinbase::crypto::ss;

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
  node_t *child1 = new node_t(node_e::LEAF, "child1", 0);
  node_t *child2 = new node_t(node_e::LEAF, "child2", 0);

  root.add_child_node(child1);
  root.add_child_node(child2);

  EXPECT_ER(root.validate_tree());  // root shouldn't have name
  root.name = "";
  EXPECT_OK(root.validate_tree());

  node_t *child3 = new node_t(node_e::THRESHOLD, "child3", 2);
  root.add_child_node(child3);
  EXPECT_ER(root.validate_tree());  // threshold node with no child
  node_t *child31 = new node_t(node_e::LEAF, "child31", 0);
  child3->add_child_node(child31);
  EXPECT_ER(root.validate_tree());  // threshold node with not enough child
  node_t *child32 = new node_t(node_e::LEAF, "child32", 0);
  child3->add_child_node(child32);
  EXPECT_OK(root.validate_tree());  // threshold node with not enough child

  EXPECT_OK(test_root->validate_tree());
}

TEST_F(SSNode, NodeClone) {
  for (const auto &root : all_roots) {
    node_t *clone = root->clone();
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

  bool correctly_reconstructable(const ac_t &ac_ref, const ac_shares_t &shares, const ss::node_t *root) {
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
  for (const auto &leaf : leaves) {
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
  for (const auto &share : shares) {
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
  for (const auto &name : valid_quorum) {
    minimal_shares[name] = shares[name];
  }
  EXPECT_TRUE(ac.enough_for_quorum(minimal_shares));
  EXPECT_TRUE(correctly_reconstructable(ac, minimal_shares, test_root));

  ac_shares_t malicious_shares;
  for (const auto &name : valid_quorum) {
    malicious_shares[name] = bn_t::rand(q);
  }
  EXPECT_TRUE(ac.enough_for_quorum(malicious_shares));
  EXPECT_FALSE(correctly_reconstructable(ac, malicious_shares, test_root));
}

}  // namespace