#include <gtest/gtest.h>

#include <cbmpc/protocol/ecdsa_mp.h>

#include "utils/local_network/mpc_tester.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;
using namespace coinbase::testutils;

TEST_F(Network2PC, BasicMessaging) {
  mpc_runner->run_2pc([](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    buf_t data;
    buf_t want(mem_t("test_string"));

    if (job.is_p1()) data = want;
    if (job.is_p2()) EXPECT_NE(data, want);
    rv = job.p1_to_p2(data);
    ASSERT_EQ(rv, 0);

    EXPECT_EQ(data, want);
  });

  mpc_runner->run_2pc([](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    buf_t data;
    buf_t want(mem_t("test_string"));

    if (job.is_p2()) data = want;
    if (job.is_p1()) EXPECT_NE(data, want);
    rv = job.p2_to_p1(data);
    ASSERT_EQ(rv, 0);

    EXPECT_EQ(data, want);
  });
}

typedef std::function<void(job_parallel_2p_t& job)> lambda_2p_t;

TEST_F(Network2PC, ParallelMessaging) {
  int parallel_count = 50;
  std::atomic<int> finished(0);
  std::mutex send_cond_mutex;

  mpc_runner->run_2pc_parallel(parallel_count, [&finished](job_parallel_2p_t& job, int th_i) {
    error_t rv = UNINITIALIZED_ERROR;
    buf_t data;
    buf_t want(mem_t("test_data:" + std::to_string(th_i * 10000)));
    if (job.is_p1()) data = want;
    if (job.is_p2()) EXPECT_NE(data, want);

    rv = job.p1_to_p2(data);
    ASSERT_EQ(rv, 0);

    EXPECT_EQ(data, want);
    finished++;
  });

  // To verify that that all threads finished
  EXPECT_EQ(finished, parallel_count * 2);
}

TEST_F(Network4PC, BasicBroadcast) {
  mpc_runner->run_mpc([](job_mp_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    auto data = job.uniform_msg<buf_t>(buf_t("test_data:" + std::to_string(party_index)));
    rv = job.plain_broadcast(data);
    ASSERT_EQ(rv, 0);

    for (int j = 0; j < 4; j++) {
      EXPECT_EQ(data.received(j), buf_t("test_data:" + std::to_string(j)));
      EXPECT_EQ(data.all_received_refs()[j].get(), buf_t("test_data:" + std::to_string(j)));
      EXPECT_EQ(data.all_received_values()[j], buf_t("test_data:" + std::to_string(j)));
    }
    EXPECT_EQ(data.msg, buf_t("test_data:" + std::to_string(party_index)));
  });
}

TEST_F(Network4PC, ParallelBroadcasting) {
  int parallel_count = 3;
  std::atomic<int> finished(0);

  mpc_runner->run_mpc_parallel(parallel_count, [&finished](job_mp_t& job, int th_i) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    auto data =
        job.uniform_msg<buf_t>(buf_t("test_data:" + std::to_string(party_index) + "-thread" + std::to_string(th_i)));
    rv = job.plain_broadcast(data);
    ASSERT_EQ(rv, 0);

    for (int j = 0; j < 4; j++) {
      EXPECT_EQ(data.received(j), buf_t("test_data:" + std::to_string(j) + "-thread" + std::to_string(th_i)));
    }
    finished++;
  });

  EXPECT_EQ(finished, parallel_count * 4);
}

TEST_P(NetworkMPC, PairwiseAndBroadcast) {
  const int m = GetParam();
  // This is a special case only used in ecdsa mpc to send both OT messages (pairwise) and a common message (broadcast).
  std::vector<std::vector<int>> ot_role_map(m, std::vector<int>(m));
  for (int i = 0; i <= m - 1; i++) {
    for (int j = i + 1; j < m; j++) {
      ot_role_map[i][j] = ecdsampc::ot_sender;
      ot_role_map[j][i] = ecdsampc::ot_receiver;
    }
  }

  mpc_runner->run_mpc([&ot_role_map, m](job_mp_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    auto data = job.uniform_msg<buf_t>(buf_t("test_data:" + std::to_string(party_index)));
    party_set_t ot_receivers = ecdsampc::ot_receivers_for(party_index, m, ot_role_map);
    auto ot_msg = job.inplace_msg<buf_t>([&party_index](int j) -> auto {
      return buf_t("test_data:" + std::to_string(party_index) + std::to_string(j));
    });
    rv = ecdsampc::plain_broadcast_and_pairwise_message(job, ot_receivers, ot_msg, data);
    ASSERT_EQ(rv, 0);

    for (int j = 0; j < m; j++) {
      EXPECT_EQ(data.received(j), buf_t("test_data:" + std::to_string(j)));
      EXPECT_EQ(data.all_received_refs()[j].get(), buf_t("test_data:" + std::to_string(j)));
      EXPECT_EQ(data.all_received_values()[j], buf_t("test_data:" + std::to_string(j)));

      if (ot_role_map[j][party_index] == ecdsampc::ot_sender) {
        EXPECT_EQ(ot_msg.received(j), buf_t("test_data:" + std::to_string(j) + std::to_string(party_index)));
      } else if (ot_role_map[party_index][j] == ecdsampc::ot_receiver) {
        EXPECT_EQ(ot_msg.received(j), buf_t());
      }
    }
    EXPECT_EQ(data.msg, buf_t("test_data:" + std::to_string(party_index)));
  });
}

TEST_P(NetworkMPC, ParallelBroadcasting) {
  int n_parties = GetParam();
  int parallel_count = 16;

  auto mpc_runner = std::make_unique<mpc_runner_t>(n_parties);
  std::atomic<int> finished(0);

  mpc_runner->run_mpc_parallel(parallel_count, [&finished, &n_parties](job_mp_t& job, int th_i) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    auto data =
        job.uniform_msg<buf_t>(buf_t("test_data:" + std::to_string(party_index) + "-thread" + std::to_string(th_i)));
    rv = job.plain_broadcast(data);
    ASSERT_EQ(rv, 0);

    for (int j = 0; j < n_parties; j++) {
      EXPECT_EQ(data.received(j), buf_t("test_data:" + std::to_string(j) + "-thread" + std::to_string(th_i)));
    }
    for (int i = 0; i < 10; i++) {
      auto data2 =
          job.uniform_msg<buf_t>(buf_t("test_data:" + std::to_string(party_index) + "-thread" + std::to_string(th_i)));
      rv = job.plain_broadcast(data2);
      ASSERT_EQ(rv, 0);
    }
    finished++;
  });

  // To verify that that all threads finished
  EXPECT_EQ(finished, parallel_count * n_parties);
}
INSTANTIATE_TEST_SUITE_P(, NetworkMPC, testing::Values(2, 4, 5, 10, 32, 64));

TEST_F(Network2PC, SequentialThenParallel) {
  int PARALLEL_COUNT = 3;
  std::vector<buf_t> data(PARALLEL_COUNT);
  for (int i = 0; i < data.size(); i++) data[i] = crypto::gen_random_bitlen(128);

  mpc_runner->run_2pc_parallel(1, [&data, PARALLEL_COUNT](job_parallel_2p_t& job, int dummy) {
    error_t rv = UNINITIALIZED_ERROR;
    auto role = job.get_party();

    rv = job.p1_to_p2(data[0]);

    std::vector<std::thread> threads;
    job.set_parallel_count(PARALLEL_COUNT);

    for (int i = 0; i < PARALLEL_COUNT; i++) {
      threads.emplace_back([&data, &job, PARALLEL_COUNT, i]() {
        job_parallel_2p_t parallel_job = job.get_parallel_job(PARALLEL_COUNT, parallel_id_t(i));

        error_t rv = parallel_job.p1_to_p2(data[i]);
        ASSERT_EQ(rv, 0);
      });
    }
    for (auto& th : threads) th.join();

    job.set_parallel_count(0);
  });
}

}  // namespace