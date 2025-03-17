#include <gtest/gtest.h>

#include <cbmpc/protocol/agree_random.h>
#include <cbmpc/protocol/util.h>

#include "utils/local_network/mpc_tester.h"
#include "utils/test_macros.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;
using namespace coinbase::testutils;

class AgreeRandom2PC : public Network2PC {};
class AgreeRandomMPC : public NetworkMPC {};

TEST_F(AgreeRandom2PC, AgreeRandom) {
  for (int bitlen : {128, 129, 1024}) {
    std::array<buf_t, 2> results;

    mpc_runner->run_2pc(
        [&bitlen, &results](job_2p_t& job) { ASSERT_OK(agree_random(job, bitlen, results[job.get_party_idx()])); });

    EXPECT_EQ(results[0], results[1]);
    EXPECT_EQ(results[0].size(), coinbase::bits_to_bytes(bitlen));
  }
}

TEST_F(AgreeRandom2PC, WeakAgreeRandomP1First) {
  for (int bitlen : {128, 129, 1024}) {
    std::array<buf_t, 2> results;

    mpc_runner->run_2pc([&bitlen, &results](job_2p_t& job) {
      ASSERT_OK(weak_agree_random_p1_first(job, bitlen, results[job.get_party_idx()]));
    });

    EXPECT_EQ(results[0], results[1]);
    EXPECT_EQ(results[0].size(), coinbase::bits_to_bytes(bitlen));
  }
}

TEST_F(AgreeRandom2PC, WeakAgreeRandomP2First) {
  for (int bitlen : {128, 129, 1024}) {
    std::array<buf_t, 2> results;

    mpc_runner->run_2pc([&bitlen, &results](job_2p_t& job) {
      ASSERT_OK(weak_agree_random_p2_first(job, bitlen, results[job.get_party_idx()]));
    });

    EXPECT_EQ(results[0], results[1]);
    EXPECT_EQ(results[0].size(), coinbase::bits_to_bytes(bitlen));
  }
}

TEST_F(AgreeRandom2PC, WeakAgreeRandomTooShortP1First) {
  std::array<buf_t, 2> results;

  mpc_runner->run_2pc([&results](job_2p_t& job) {
    int bits_count = 127;
    ASSERT_ER(weak_agree_random_p1_first(job, bits_count, results[job.get_party_idx()]));
  });
}

TEST_F(AgreeRandom2PC, WeakAgreeRandomTooShortP2First) {
  std::array<buf_t, 2> results;

  mpc_runner->run_2pc([&results](job_2p_t& job) {
    int bits_count = 127;
    ASSERT_ER(weak_agree_random_p2_first(job, bits_count, results[job.get_party_idx()]));
  });
}

TEST_P(AgreeRandomMPC, MultiAgreeRandom) {
  int n = GetParam();
  for (int bitlen : {128, 129, 1024}) {
    std::vector<buf_t> results(n);

    mpc_runner->run_mpc([&bitlen, &results](job_mp_t& job) {
      ASSERT_OK(multi_agree_random(job, bitlen, results[job.get_party_idx()]));
    });

    for (int i = 1; i < n; i++) {
      EXPECT_EQ(results[0], results[i]);
    }
    EXPECT_EQ(results[0].size(), coinbase::bits_to_bytes(bitlen));
  }
}
// INSTANTIATE_TEST_SUITE_P(, AgreeRandomMPC, testing::Values(4, 5, 32));

TEST_P(AgreeRandomMPC, MultiPairwiseAgreeRandom) {
  int n = GetParam();
  for (int bitlen : {128, 129, 1024}) {
    std::vector<std::vector<buf_t>> results(n);

    mpc_runner->run_mpc([&bitlen, &results](job_mp_t& job) {
      ASSERT_OK(multi_pairwise_agree_random(job, bitlen, results[job.get_party_idx()]));
    });

    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        EXPECT_EQ(results[i][j], results[j][i]);
        EXPECT_EQ(results[i][j].size(), coinbase::bits_to_bytes(bitlen));
      }
    }
  }
}
INSTANTIATE_TEST_SUITE_P(, AgreeRandomMPC, testing::Values(4, 5, 32));

}  // namespace