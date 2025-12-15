#include <gtest/gtest.h>

#include <cbmpc/core/strext.h>
#include <cbmpc/crypto/secret_sharing.h>
#include <cbmpc/protocol/ec_dkg.h>

#include "utils/local_network/mpc_tester.h"
#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;
using namespace coinbase::crypto::ss;
using namespace coinbase::mpc::eckey;

namespace {

static void RunDkgAndAdditiveShareTest(crypto::ss::node_t* root_node, const std::vector<crypto::pname_t>& pnames,
                                       const std::set<int>& dkg_quorum_indices,
                                       const std::set<crypto::pname_t>& additive_quorum_names) {
  using namespace coinbase;
  using namespace coinbase::mpc::eckey;
  using namespace coinbase::crypto;
  using namespace coinbase::crypto::ss;
  using namespace coinbase::testutils;

  ecurve_t curve = crypto::curve_secp256k1;
  const auto& G = curve.generator();
  ss::ac_t ac;
  ac.G = G;
  ac.root = root_node;

  mpc::party_set_t quorum_party_set;
  for (int idx : dkg_quorum_indices) quorum_party_set.add(idx);

  std::vector<coinbase::mpc::eckey::key_share_mp_t> keyshares(pnames.size());
  buf_t sid_dkg = crypto::gen_random(16);
  mpc_runner_t all_parties_runner(pnames);
  all_parties_runner.run_mpc([&](mpc::job_mp_t& job) {
    ASSERT_OK(coinbase::mpc::eckey::key_share_mp_t::threshold_dkg(job, curve, sid_dkg, ac, quorum_party_set,
                                                                  keyshares[job.get_party_idx()]));
  });

  // Basic key consistency
  for (int i = 0; i < pnames.size(); i++) {
    EXPECT_EQ(keyshares[i].x_share * G, keyshares[i].Qis[pnames[i]]);
    EXPECT_EQ(keyshares[i].Q, keyshares[0].Q);
  }

  // Test to_additive_share for each member in the additive quorum
  std::map<crypto::pname_t, int> index_map;
  for (int i = 0; i < pnames.size(); i++) index_map[pnames[i]] = i;
  for (const auto& name : additive_quorum_names) {
    coinbase::mpc::eckey::key_share_mp_t additive_share;
    ASSERT_OK(keyshares[index_map[name]].to_additive_share(ac, additive_quorum_names, additive_share));
    EXPECT_EQ(additive_share.Q, keyshares[0].Q);
    for (const auto& qn : additive_quorum_names) {
      ASSERT_TRUE(additive_share.Qis.count(qn));
      EXPECT_TRUE(additive_share.Qis[qn].valid());
    }
    EXPECT_EQ(additive_share.x_share * G, additive_share.Qis[name]);
  }
}

TEST(ECDKG, ReconstructPubAdditiveShares) {
  // Build access structure:
  // AND(
  //   AND(p0, p1),
  //   THRESHOLD[1](p2, p3)
  // )
  node_t* root_node = new node_t(
      node_e::AND, "", 0,
      {new node_t(node_e::AND, "group-1", 0, {new node_t(node_e::LEAF, "p0"), new node_t(node_e::LEAF, "p1")}),
       new node_t(node_e::THRESHOLD, "threshold-node", 1,
                  {new node_t(node_e::LEAF, "p2"), new node_t(node_e::LEAF, "p3")})});

  ecurve_t curve = crypto::curve_secp256k1;
  const auto& G = curve.generator();
  ss::ac_t ac;
  ac.G = G;
  ac.root = root_node;

  std::set<crypto::pname_t> quorum = {"p0", "p1", "p2"};

  // Construct key share for party p2, with Q and Qis taken from a real DKG run
  key_share_mp_t ks;
  ks.curve = curve;
  ks.party_name = "p2";
  ks.x_share = bn_t::from_hex("e4f333d0bae7b038d6520e3898a420b0ec45a3816c783b1f1b51ddd7e5ed4d5b");

  buf_t q;
  ASSERT_TRUE(strext::from_hex(q,
                               "043ba974482f15ea45d22ad2022c5168e36ff3e320ef49c36b65388090c2e7bf50fb79a1648f194fdd38733"
                               "a6503a13e5f6be7bf7979ebbf0f33a7849f69886311"));
  ASSERT_EQ(ks.Q.from_bin(curve, mem_t(q.data(), q.size())), 0);

  ASSERT_TRUE(strext::from_hex(q,
                               "046df7e34ba10dd371efb4b3c508918115d258a9e05c69869e6bd33804cf1450d1e5a64c161b97063a3d662"
                               "29169d79db391a9f8bfaba0661c9f8aab2f2882409d"));
  ASSERT_EQ(ks.Qis["p0"].from_bin(curve, mem_t(q.data(), q.size())), 0);
  ASSERT_TRUE(strext::from_hex(q,
                               "049a17a7674840e077daf26c7a0968eac8b1682b35d2d5dac09be5421b70da590ff9bb515f4bd6e30a5d77c"
                               "87dfeaf9fbf7bf81f7386b5650276afb082d685875a"));
  ASSERT_EQ(ks.Qis["p1"].from_bin(curve, mem_t(q.data(), q.size())), 0);
  ASSERT_TRUE(strext::from_hex(q,
                               "048ce1b47d641157ae2ce9636b72f3345e162ea904b8830e96c92a6ec3d5842b8f2955d0ff48d08ef46856e"
                               "f593a71b29be6092e4a5929e606c7eaf75a099394bf"));
  ASSERT_EQ(ks.Qis["p2"].from_bin(curve, mem_t(q.data(), q.size())), 0);
  ASSERT_TRUE(strext::from_hex(q,
                               "048ce1b47d641157ae2ce9636b72f3345e162ea904b8830e96c92a6ec3d5842b8f2955d0ff48d08ef46856e"
                               "f593a71b29be6092e4a5929e606c7eaf75a099394bf"));
  ASSERT_EQ(ks.Qis["p3"].from_bin(curve, mem_t(q.data(), q.size())), 0);

  key_share_mp_t additive_share;
  error_t rv = ks.to_additive_share(ac, quorum, additive_share);
  ASSERT_EQ(rv, 0);

  // Expect that p0, p1 remain as provided and p2 is reconstructed, not invalid
  EXPECT_TRUE(additive_share.Qis["p0"].valid());
  EXPECT_TRUE(additive_share.Qis["p1"].valid());
  EXPECT_TRUE(additive_share.Qis["p2"].valid());

  // Additionally check that p2 equals the expected hex point
  buf_t expected_bin;
  ASSERT_TRUE(strext::from_hex(expected_bin,
                               "048ce1b47d641157ae2ce9636b72f3345e162ea904b8830e96c92a6ec3d5842b8f2955d0ff48d08ef46856e"
                               "f593a71b29be6092e4a5929e606c7eaf75a099394bf"));
  ecc_point_t expected_p2;
  ASSERT_EQ(expected_p2.from_bin(curve, mem_t(expected_bin.data(), expected_bin.size())), 0);
  EXPECT_EQ(additive_share.Qis["p2"], expected_p2);
}

TEST(ECDKG, ReconstructPubAdditiveShares_ORNode) {
  // OR(p0, AND(p1, THRESHOLD[1](p2, p3))) with additive quorum {p1, p2}
  ss::node_t* root_node = new ss::node_t(
      ss::node_e::OR, "", 0,
      {new ss::node_t(ss::node_e::LEAF, "p0"),
       new ss::node_t(
           ss::node_e::AND, "and-group", 0,
           {new ss::node_t(ss::node_e::LEAF, "p1"),
            new ss::node_t(ss::node_e::THRESHOLD, "inner-th", 1,
                           {new ss::node_t(ss::node_e::LEAF, "p2"), new ss::node_t(ss::node_e::LEAF, "p3")})})});

  std::vector<crypto::pname_t> pnames = {"p0", "p1", "p2", "p3"};
  std::set<int> dkg_quorum_indices = {1, 3};
  std::set<crypto::pname_t> additive_quorum = {"p1", "p2"};
  RunDkgAndAdditiveShareTest(root_node, pnames, dkg_quorum_indices, additive_quorum);
}

TEST(ECDKG, ReconstructPubAdditiveShares_Threshold2of3) {
  // THRESHOLD[2](p0, p1, p2) with additive quorum {p0, p2}
  ss::node_t* root_node =
      new ss::node_t(ss::node_e::THRESHOLD, "th-root", 2,
                     {new ss::node_t(ss::node_e::LEAF, "p0"), new ss::node_t(ss::node_e::LEAF, "p1"),
                      new ss::node_t(ss::node_e::LEAF, "p2")});

  std::vector<crypto::pname_t> pnames = {"p0", "p1", "p2"};
  std::set<int> dkg_quorum_indices = {0, 2};
  std::set<crypto::pname_t> additive_quorum = {"p0", "p2"};
  RunDkgAndAdditiveShareTest(root_node, pnames, dkg_quorum_indices, additive_quorum);
}

TEST(ECDKG, ReconstructPubAdditiveShares_ThresholdNofN_ANDEquivalent) {
  // THRESHOLD[3](p0, p1, p2) with additive quorum {p0, p1, p2} (equivalent to AND)
  ss::node_t* root_node =
      new ss::node_t(ss::node_e::THRESHOLD, "th-all", 3,
                     {new ss::node_t(ss::node_e::LEAF, "p0"), new ss::node_t(ss::node_e::LEAF, "p1"),
                      new ss::node_t(ss::node_e::LEAF, "p2")});

  std::vector<crypto::pname_t> pnames = {"p0", "p1", "p2"};
  std::set<int> dkg_quorum_indices = {0, 1, 2};
  std::set<crypto::pname_t> additive_quorum = {"p0", "p1", "p2"};
  RunDkgAndAdditiveShareTest(root_node, pnames, dkg_quorum_indices, additive_quorum);
}

TEST(ECDKG, ReconstructPubAdditiveShares_Threshold3of4_LargerLeaves) {
  // THRESHOLD[3](p0, p1, p2, p3) with additive quorum {p0, p1, p2}
  ss::node_t* root_node =
      new ss::node_t(ss::node_e::THRESHOLD, "th-3of4", 3,
                     {new ss::node_t(ss::node_e::LEAF, "p0"), new ss::node_t(ss::node_e::LEAF, "p1"),
                      new ss::node_t(ss::node_e::LEAF, "p2"), new ss::node_t(ss::node_e::LEAF, "p3")});

  std::vector<crypto::pname_t> pnames = {"p0", "p1", "p2", "p3"};
  std::set<int> dkg_quorum_indices = {0, 1, 2};
  std::set<crypto::pname_t> additive_quorum = {"p3", "p1", "p2"};
  RunDkgAndAdditiveShareTest(root_node, pnames, dkg_quorum_indices, additive_quorum);
}

}  // namespace
