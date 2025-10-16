#include <gtest/gtest.h>

#include <cbmpc/crypto/lagrange.h>
#include <cbmpc/crypto/secret_sharing.h>
#include <cbmpc/protocol/ecdsa_mp.h>

#include "utils/local_network/mpc_tester.h"
#include "utils/test_macros.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;
using namespace coinbase::mpc::ecdsampc;
using namespace coinbase::testutils;

class ECDSA4PC : public Network4PC {
 protected:
  static void check_keys(const std::vector<ecdsampc::key_t>& keys) {
    crypto::vartime_scope_t vartime_scope;
    auto Q = keys[0].Q;
    auto curve = keys[0].curve;
    for (int i = 1; i < keys.size(); i++) {
      EXPECT_EQ(Q, keys[i].Q);
      EXPECT_EQ(curve, keys[i].curve);
    }
    const auto& G = curve.generator();
    auto Q_from_x_shares = keys[0].x_share * G;
    for (int i = 1; i < keys.size(); i++) {
      Q_from_x_shares += keys[i].x_share * G;
    }
    EXPECT_EQ(Q, Q_from_x_shares);
  }
};

std::vector<std::vector<int>> test_ot_role(int n) {
  std::vector<std::vector<int>> ot_role_map(n, std::vector<int>(n));
  for (int i = 0; i < n; i++) {
    ot_role_map[i][i] = ot_no_role;
  }

  for (int i = 0; i <= n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
      ot_role_map[i][j] = ot_sender;
      ot_role_map[j][i] = ot_receiver;
    }
  }
  return ot_role_map;
}

class ECDSAMPC : public NetworkMPC {};

TEST_P(ECDSAMPC, KeygenSignRefreshSign) {
  const int m = GetParam();

  buf_t data = crypto::gen_random(32);
  std::vector<ecdsampc::key_t> keys(m);
  std::vector<ecdsampc::key_t> new_keys(m);

  mpc_runner->run_mpc([&keys, &new_keys, &data, &m](job_mp_t& job) {
    std::vector<std::vector<int>> ot_role_map = test_ot_role(m);
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    ecdsampc::key_t& key = keys[party_index];
    ecurve_t curve = crypto::curve_secp256k1;
    buf_t sid;
    rv = ecdsampc::dkg(job, curve, key, sid);
    ASSERT_EQ(rv, 0);

    buf_t sig;
    rv = sign(job, key, data, party_idx_t(0), ot_role_map, sig);
    ASSERT_EQ(rv, 0);
    if (party_index == 0) {
      crypto::ecc_pub_key_t ecc_verify_key(key.Q);
      EXPECT_OK(ecc_verify_key.verify(data, sig));
    }

    ecdsampc::key_t& new_key = new_keys[party_index];
    rv = ecdsampc::refresh(job, sid, key, new_key);
    ASSERT_EQ(rv, 0);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    buf_t new_sig;
    rv = sign(job, new_key, data, party_idx_t(0), ot_role_map, new_sig);
    ASSERT_EQ(rv, 0);
    if (party_index == 0) {
      crypto::ecc_pub_key_t ecc_verify_key(key.Q);
      EXPECT_OK(ecc_verify_key.verify(data, sig));
    }
  });
}
INSTANTIATE_TEST_SUITE_P(, ECDSAMPC, testing::Values(2, 5, 10));

TEST_F(ECDSA4PC, KeygenSignRefreshSign) {
  buf_t data = crypto::gen_random(32);

  std::vector<ecdsampc::key_t> keys(4);
  std::vector<ecdsampc::key_t> new_keys(4);

  mpc_runner->run_mpc([&keys, &new_keys, &data](job_mp_t& job) {
    std::vector<std::vector<int>> ot_role_map = test_ot_role(4);
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    ecdsampc::key_t& key = keys[party_index];
    ecurve_t curve = crypto::curve_secp256k1;
    buf_t sid;
    rv = ecdsampc::dkg(job, curve, key, sid);
    ASSERT_EQ(rv, 0);

    buf_t sig;
    rv = sign(job, key, data, party_idx_t(0), ot_role_map, sig);
    ASSERT_EQ(rv, 0);
    if (party_index == 0) {
      crypto::ecc_pub_key_t ecc_verify_key(key.Q);
      EXPECT_OK(ecc_verify_key.verify(data, sig));
    }

    ecdsampc::key_t& new_key = new_keys[party_index];
    rv = ecdsampc::refresh(job, sid, key, new_key);
    ASSERT_EQ(rv, 0);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    buf_t new_sig;
    rv = sign(job, new_key, data, party_idx_t(0), ot_role_map, new_sig);
    ASSERT_EQ(rv, 0);
    if (party_index == 0) {
      crypto::ecc_pub_key_t ecc_verify_key(key.Q);
      EXPECT_OK(ecc_verify_key.verify(data, sig));
    }
  });

  check_keys(keys);
  check_keys(new_keys);
}

TEST_F(ECDSA4PC, ParallelKSRS8) {
  int parallel_count = 8;
  std::vector<buf_t> data(parallel_count);
  for (int i = 0; i < parallel_count; i++) {
    data[i] = crypto::gen_random(32);
  }
  std::vector<std::vector<ecdsampc::key_t>> keys(parallel_count, std::vector<ecdsampc::key_t>(4));
  std::vector<std::vector<ecdsampc::key_t>> new_keys(parallel_count, std::vector<ecdsampc::key_t>(4));

  mpc_runner->run_mpc_parallel(parallel_count, [&keys, &new_keys, &data](job_parallel_mp_t& job, int th_i) {
    std::vector<std::vector<int>> ot_role_map = test_ot_role(4);
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    ecdsampc::key_t& key = keys[th_i][party_index];
    ecurve_t curve = crypto::curve_secp256k1;

    buf_t sid;
    rv = ecdsampc::dkg(job, curve, key, sid);
    ASSERT_EQ(rv, 0);

    buf_t sig;
    rv = sign(job, key, data[th_i], party_idx_t(0), ot_role_map, sig);
    ASSERT_EQ(rv, 0);

    if (party_index == 0) {
      crypto::ecc_pub_key_t ecc_verify_key(key.Q);
      EXPECT_OK(ecc_verify_key.verify(data[th_i], sig));
    }

    ecdsampc::key_t& new_key = new_keys[th_i][party_index];
    rv = ecdsampc::refresh(job, sid, key, new_key);
    ASSERT_EQ(rv, 0);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    buf_t new_sig;
    rv = sign(job, new_key, data[th_i], party_idx_t(0), ot_role_map, new_sig);
    ASSERT_EQ(rv, 0);

    if (party_index == 0) {
      crypto::ecc_pub_key_t ecc_verify_key(key.Q);
      EXPECT_OK(ecc_verify_key.verify(data[th_i], new_sig));
    }
  });

  for (int i = 0; i < parallel_count; i++) {
    check_keys(keys[i]);
    check_keys(new_keys[i]);
  }
}

TEST(ECDSAMPCThreshold, DKG) {
  int n = 5;
  std::vector<crypto::pname_t> pnames = {"party-0", "party-1", "party-2", "party-3", "party-4"};
  std::map<crypto::pname_t, int> quorum_party_map;
  quorum_party_map[pnames[0]] = 0;
  quorum_party_map[pnames[1]] = 1;
  quorum_party_map[pnames[2]] = 2;
  quorum_party_map[pnames[3]] = 3;
  quorum_party_map[pnames[4]] = 4;

  // Hardwired for the test. If changed, many other things here should be changed
  // Also for simplicity of testing, we assume the first t parties are active
  int t = 3;

  ecurve_t curve = crypto::curve_secp256k1;
  const auto& G = curve.generator();
  mod_t q = curve.order();
  std::vector<eckey::key_share_mp_t> keyshares(n);
  std::vector<eckey::key_share_mp_t> new_keyshares(n);
  std::set<crypto::pname_t> quorum1;
  party_set_t quorum_party_set;

  quorum_party_set.add(1);
  quorum1.insert(pnames[1]);
  quorum_party_set.add(2);
  quorum1.insert(pnames[2]);
  quorum_party_set.add(4);
  quorum1.insert(pnames[4]);

  buf_t sid_dkg = crypto::gen_random(16);
  buf_t sid_refresh = crypto::gen_random(16);

  crypto::ss::node_t* root_node = new crypto::ss::node_t(
      crypto::ss::node_e::AND, "", 0,
      {new crypto::ss::node_t(crypto::ss::node_e::THRESHOLD, "threshold-node", 2,
                              {
                                  new crypto::ss::node_t(crypto::ss::node_e::LEAF, pnames[0]),  // active
                                  new crypto::ss::node_t(crypto::ss::node_e::LEAF, pnames[1]),  // active
                                  new crypto::ss::node_t(crypto::ss::node_e::LEAF, pnames[2]),
                              }),
       new crypto::ss::node_t(crypto::ss::node_e::OR, "or-node", 0,
                              {
                                  new crypto::ss::node_t(crypto::ss::node_e::LEAF, pnames[3]),  // active
                                  new crypto::ss::node_t(crypto::ss::node_e::LEAF, pnames[4]),
                              })});
  crypto::ss::ac_t ac;
  ac.G = G;
  ac.root = root_node;

  // DKG is an n-party protocol
  mpc_runner_t all_parties_runner(pnames);
  all_parties_runner.run_mpc([&curve, &keyshares, &quorum_party_set, &ac, &sid_dkg](mpc::job_mp_t& job) {
    EXPECT_OK(eckey::key_share_mp_t::threshold_dkg(job, curve, sid_dkg, ac, quorum_party_set,
                                                   keyshares[job.get_party_idx()]));
  });

  for (int i = 0; i < n; i++) {
    ASSERT_EQ(keyshares[i].x_share * G, keyshares[0].Qis[pnames[i]]);
  }

  ASSERT_EQ(sid_dkg.size(), 16);

  // Signing is a t-party protocol
  mpc_runner_t quorum_runner({"party-1", "party-2", "party-4"});

  buf_t data = crypto::gen_random(32);
  std::vector<std::vector<int>> ot_role_map = test_ot_role(t);
  quorum_runner.run_mpc([&](mpc::job_mp_t& job) {
    eckey::key_share_mp_t additive_share;
    EXPECT_OK(keyshares[quorum_party_map[job.get_name()]].to_additive_share(ac, quorum1, additive_share));
    buf_t sig;
    error_t rv = sign(job, additive_share, data, party_idx_t(0), ot_role_map, sig);
    ASSERT_EQ(rv, 0);

    if (job.get_party_idx() == 0) {
      crypto::ecc_pub_key_t ecc_verify_key(additive_share.Q);
      EXPECT_OK(ecc_verify_key.verify(data, sig));
    }
  });

  // Refresh is an n-party protocol
  all_parties_runner.run_mpc([&](mpc::job_mp_t& job) {
    ASSERT_OK(eckey::key_share_mp_t::threshold_refresh(job, curve, sid_refresh, ac, quorum_party_set,
                                                       keyshares[job.get_party_idx()],
                                                       new_keyshares[job.get_party_idx()]));
  });
  ASSERT_EQ(sid_refresh.size(), 16);
  ASSERT_NE(sid_refresh, sid_dkg);

  for (int i = 0; i < n; i++) {
    EXPECT_EQ(new_keyshares[i].Q, keyshares[i].Q);
    EXPECT_NE(new_keyshares[i].x_share, keyshares[i].x_share);
  }

  mpc_runner_t quorum2_runner({"party-0", "party-1", "party-3"});
  std::set<crypto::pname_t> quorum2;
  quorum2.insert(pnames[0]);
  quorum2.insert(pnames[1]);
  quorum2.insert(pnames[3]);

  data = crypto::gen_random(32);
  quorum2_runner.run_mpc([&](mpc::job_mp_t& job) {
    eckey::key_share_mp_t additive_share;
    EXPECT_OK(new_keyshares[quorum_party_map[job.get_name()]].to_additive_share(ac, quorum2, additive_share));
    buf_t sig;
    error_t rv = sign(job, additive_share, data, party_idx_t(0), ot_role_map, sig);
    ASSERT_EQ(rv, 0);

    if (job.get_party_idx() == 0) {
      crypto::ecc_pub_key_t ecc_verify_key(additive_share.Q);
      EXPECT_OK(ecc_verify_key.verify(data, sig));
    }
  });

  for (int i = 0; i < n; i++) {
    ASSERT_EQ(keyshares[i].x_share * G, keyshares[i].Qis[pnames[i]]);
    ASSERT_EQ(new_keyshares[i].x_share * G, new_keyshares[i].Qis[pnames[i]]);
  }

  for (int i = 1; i < n; i++) {
    for (int j = 0; j < n; j++) {
      EXPECT_EQ(keyshares[i].Qis[pnames[j]], keyshares[0].Qis[pnames[j]]);
    }
  }

  std::vector<eckey::key_share_mp_t> new_additive_shares(n);
  for (int i = 0; i < n; i++) {
    EXPECT_OK(new_keyshares[i].to_additive_share(ac, quorum2, new_additive_shares[i]));
  }
  EXPECT_EQ(
      ((new_additive_shares[0].x_share + new_additive_shares[1].x_share + new_additive_shares[3].x_share) % q) * G,
      new_keyshares[0].Q);
}

}  // namespace