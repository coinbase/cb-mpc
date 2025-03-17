#include <benchmark/benchmark.h>

#include <local_network/mpc_runner.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/agree_random.h>

#include "mpc_util.h"

using namespace coinbase;
using namespace coinbase::mpc;

static void BM_TEST_SLEEP(benchmark::State& state) {
  for (auto _ : state) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}
BENCHMARK(BM_TEST_SLEEP)->Name("Test/Sleep")->Iterations(10);

error_t test_2pc_protocol(job_2p_t& job) {
  error_t rv = UNINITIALIZED_ERROR;
  // int party = job.get_party();

  // p1 ~ 300 ms
  // p2 ~ 100 ms
  THREAD_SAFE_LOG(party_idx << " ================= round 1");
  std::this_thread::sleep_for(std::chrono::milliseconds(100 * (2 - job.get_party_idx())));
  THREAD_SAFE_LOG(party_idx << " ================= round 1 after sleep");

  buf_t buf;
  if (job.is_p1()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    buf = crypto::gen_random_bitlen(SEC_P_COM);
  }

  if (rv = job.p1_to_p2(buf)) return rv;

  // p1 ~ 100 ms
  // p2 ~ 100 ms
  THREAD_SAFE_LOG(party_idx << " ================= round 2");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  THREAD_SAFE_LOG(party_idx << " ================= round 2 after sleep");

  if (rv = job.p2_to_p1(buf)) return rv;

  // p1 ~ 0 ms
  // p2 ~ 0 ms
  THREAD_SAFE_LOG(party_idx << " ================= round 3");

  if (rv = job.p1_to_p2(buf)) return rv;

  // p1 ~ 30 ms
  // p2 ~ 60 ms
  THREAD_SAFE_LOG(party_idx << " ================= round 4");
  if (job.is_p1()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
  }

  return SUCCESS;
}

static void BM_TEST_2PC_BENCHMARKING(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);

  int total_rounds = 4;

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [](job_2p_t& job) {
      error_t rv = UNINITIALIZED_ERROR;
      rv = test_2pc_protocol(job);
    });

    state.SetIterationTime(result.time);
  }
}
BENCHMARK(BM_TEST_2PC_BENCHMARKING)
    ->UseManualTime()
    ->Name("Test/2PC-4R")
    ->ArgsProduct({{1, 2, 3, 4}, {1, 2}})
    ->Iterations(1);

error_t test_mpc_protocol(job_mp_t& job) {
  error_t rv = UNINITIALIZED_ERROR;
  int party_idx = job.get_party_idx();

  THREAD_SAFE_LOG(party_idx << " ================= round 1");
  std::this_thread::sleep_for(std::chrono::milliseconds(50 * party_idx));
  THREAD_SAFE_LOG(party_idx << " ================= round 1 after sleep");

  buf_t buf = crypto::gen_random_bitlen(SEC_P_COM);
  auto buf_msg = job.uniform_msg<buf_t>(buf);

  if (rv = job.mpc_broadcast(buf_msg)) return rv;

  THREAD_SAFE_LOG(party_idx << " ================= round 2");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  THREAD_SAFE_LOG(party_idx << " ================= round 2 after sleep");

  auto bn_msg = job.nonuniform_msg<bn_t>();
  for (int i = 0; i < job.get_n_parties(); i++) {
    bn_msg[i] = i;
  }

  if (rv = job.mpc_broadcast(bn_msg)) return rv;

  THREAD_SAFE_LOG(party_idx << " ================= round 3");
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  THREAD_SAFE_LOG(party_idx << " ================= round 3 after sleep");

  party_set_t receivers = party_set_t(0);
  for (int i = job.get_party_idx() + 1; i < job.get_n_parties(); i++) {
    receivers.add(i);
  }
  THREAD_SAFE_LOG(party_idx << " ZZZZZZ receiver" << (receivers.peers & 0xf));
  party_set_t senders = party_set_t(0);
  for (int i = 0; i < job.get_n_parties(); i++) {
    if (i == job.get_party_idx()) continue;
    if (receivers.has(i)) continue;
    senders.add(i);
  }
  THREAD_SAFE_LOG(party_idx << " ZZZZZZ sender" << (senders.peers & 0xf));
  auto bn_inplace_msg = job.inplace_msg<bn_t>([](int j) -> auto{ return bn_t(j); });
  if (rv = job.group_message(receivers, senders, bn_inplace_msg)) return rv;

  THREAD_SAFE_LOG(party_idx << " ================= round 4");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  THREAD_SAFE_LOG(party_idx << " ================= round 4 after sleep");

  auto bn_msg2 = job.nonuniform_msg<bn_t>();
  for (int i = 0; i < job.get_n_parties(); i++) {
    bn_msg2[i] = i;
  }

  if (rv = job.mpc_broadcast(bn_msg2)) return rv;

  THREAD_SAFE_LOG(party_idx << " ================= round 5");
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  THREAD_SAFE_LOG(party_idx << " ================= round 5 after sleep");

  return SUCCESS;
}

static void BM_TEST_MPC_BENCHMARKING(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}}, {{3, 3}, {3, 3}, {3, 3}, {3, 3}},         {{3, 0}, {2, 1}, {1, 2}, {0, 3}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}}, {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };

  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  /* start benchmarking */
  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [](job_mp_t& job) { test_mpc_protocol(job); });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}
BENCHMARK(BM_TEST_MPC_BENCHMARKING)
    ->UseManualTime()
    ->Name("Test/MPC-5R-4P")
    ->ArgsProduct({{1, 2, 3, 4, 5}, {0, 1, 2, 3}})
    ->Iterations(1);
