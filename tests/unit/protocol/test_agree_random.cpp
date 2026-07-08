#include <gtest/gtest.h>

#include <cbmpc/internal/protocol/agree_random.h>
#include <cbmpc/internal/protocol/sid.h>
#include <cbmpc/internal/protocol/util.h>

#include "utils/local_network/mpc_tester.h"
#include "utils/test_macros.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;
using namespace coinbase::testutils;

class AgreeRandom2PC : public Network2PC {};
class AgreeRandomMPC : public NetworkMPC {};
class AgreeRandom4PC : public Network4PC {};
class GenerateSidMPC : public Network4PC {};

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

TEST_F(AgreeRandom2PC, GenerateSidFixed2PBothSenderOrders) {
  for (party_t first_sender : {party_t::p1, party_t::p2}) {
    std::array<buf_t, 2> results;

    mpc_runner->run_2pc([&first_sender, &results](job_2p_t& job) {
      ASSERT_OK(generate_sid_fixed_2p(job, first_sender, results[job.get_party_idx()]));
    });

    EXPECT_EQ(results[0], results[1]);
    EXPECT_EQ(results[0].size(), coinbase::bits_to_bytes(SEC_P_COM));
  }
}

TEST_F(AgreeRandom2PC, GenerateSidDynamic2PSortsPids) {
  const crypto::mpc_pid_t low_pid = crypto::pid_from_name("alice");
  const crypto::mpc_pid_t high_pid = crypto::pid_from_name("bob");

  std::array<buf_t, 2> low_first;
  std::array<buf_t, 2> high_first;

  mpc_runner->run_2pc([&](job_2p_t& job) {
    ASSERT_OK(generate_sid_dynamic_2p(job, party_t::p1, low_pid, high_pid, low_first[job.get_party_idx()]));
    ASSERT_OK(generate_sid_dynamic_2p(job, party_t::p2, high_pid, low_pid, high_first[job.get_party_idx()]));
  });

  EXPECT_EQ(low_first[0], low_first[1]);
  EXPECT_EQ(high_first[0], high_first[1]);
  EXPECT_EQ(low_first[0].size(), crypto::hash_alg_t::get(crypto::hash_e::sha256).size);
  EXPECT_EQ(high_first[0].size(), crypto::hash_alg_t::get(crypto::hash_e::sha256).size);
}

TEST_F(GenerateSidMPC, GenerateSidDynamicMPSortsPids) {
  std::vector<buf_t> results(4);

  mpc_runner->run_mpc([&results](job_mp_t& job) {
    std::vector<crypto::mpc_pid_t> pids = job.get_pids();
    std::reverse(pids.begin(), pids.end());
    ASSERT_OK(generate_sid_dynamic_mp(job, pids, results[job.get_party_idx()]));
  });

  for (int i = 1; i < 4; i++) EXPECT_EQ(results[0], results[i]);
  EXPECT_EQ(results[0].size(), crypto::hash_alg_t::get(crypto::hash_e::sha256).size);
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

TEST_P(AgreeRandomMPC, WeakMultiAgreeRandom) {
  int n = GetParam();
  for (int bitlen : {128, 129, 1024}) {
    std::vector<buf_t> results(n);

    mpc_runner->run_mpc([&bitlen, &results](job_mp_t& job) {
      ASSERT_OK(weak_multi_agree_random(job, bitlen, results[job.get_party_idx()]));
    });

    for (int i = 1; i < n; i++) EXPECT_EQ(results[0], results[i]);
    EXPECT_EQ(results[0].size(), coinbase::bits_to_bytes(bitlen));
  }
}

TEST_F(AgreeRandom4PC, WeakMultiAgreeRandomTooShort) {
  std::vector<buf_t> results(4);

  mpc_runner->run_mpc([&results](job_mp_t& job) {
    ASSERT_ER(weak_multi_agree_random(job, SEC_P_COM - 1, results[job.get_party_idx()]));
  });
}

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
