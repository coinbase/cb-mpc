#include <gtest/gtest.h>

#include <cbmpc/protocol/committed_broadcast.h>
#include <cbmpc/protocol/util.h>

#include "utils/local_network/mpc_tester.h"
#include "utils/test_macros.h"

using namespace coinbase;

class CommittedGroupBroadcast : public testutils::Network4PC {};
class CommittedPairwiseBroadcast : public testutils::Network4PC {};

TEST_F(CommittedGroupBroadcast, Completeness) {
  int n_parties = 4;

  std::vector<buf256_t> m(n_parties);
  for (int i = 0; i < n_parties; i++) crypto::gen_random(m[i]);  //  = buf256_t::random_value();

  mpc_runner->run_mpc([&m, &n_parties](mpc::job_mp_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();

    auto msg_r = job.uniform_msg<buf256_t>(m[party_index]);
    auto msg_str = job.uniform_msg<std::string>(std::string("test"));
    rv = committed_group_broadcast(job, msg_r, msg_str);
    EXPECT_EQ(rv, 0);

    for (int i = 0; i < n_parties; i++) {
      EXPECT_EQ(msg_r.received(i), m[i]);
      EXPECT_EQ(msg_str.received(i), std::string("test"));

      EXPECT_EQ(msg_r.all_received()[i], m[i]);
      EXPECT_EQ(msg_str.all_received()[i], std::string("test"));
    }
    EXPECT_EQ(msg_r.msg, m[party_index]);
    EXPECT_EQ(msg_str.msg, std::string("test"));
  });
}

TEST_F(CommittedPairwiseBroadcast, Completeness) {
  int n_parties = 4;

  std::vector<std::vector<buf256_t>> m(n_parties);
  for (int i = 0; i < n_parties; i++) {
    m[i].resize(n_parties);
    for (int j = 0; j < n_parties; j++) {
      crypto::gen_random(m[i][j]);
    }
  }

  mpc_runner->run_mpc([&m, &n_parties](mpc::job_mp_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();

    auto msg_r = job.nonuniform_msg<buf256_t>();
    for (int i = 0; i < n_parties; i++) msg_r[i] = m[party_index][i];
    rv = committed_pairwise_broadcast(job, msg_r);
    EXPECT_EQ(rv, 0);

    for (int i = 0; i < n_parties; i++) {
      EXPECT_EQ(msg_r.msgs[i], m[party_index][i]);
      EXPECT_EQ(msg_r.received(i), m[i][party_index]);
    }
  });
}
