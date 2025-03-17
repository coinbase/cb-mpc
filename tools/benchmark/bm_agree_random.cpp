#include <benchmark/benchmark.h>

#include <local_network/mpc_runner.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/agree_random.h>

#include "mpc_util.h"

using namespace coinbase;
using namespace coinbase::mpc;

static void BM_weak_agree_random(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);

  int total_rounds = 3;
  int bitlen = state.range(2);

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [&bitlen](job_2p_t& job) {
      buf_t out;
      weak_agree_random_p1_first(job, bitlen, out);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_agree_random(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);

  int total_rounds = 4;
  int bitlen = state.range(2);

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [&bitlen](job_2p_t& job) {
      buf_t out;
      agree_random(job, bitlen, out);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_multi_agree_random(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  int bitlen = state.range(2);

  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [&bitlen](job_mp_t& job) {
      buf_t out;
      multi_agree_random(job, bitlen, out);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

static void BM_weak_multi_agree_random(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  int bitlen = state.range(2);

  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [&bitlen](job_mp_t& job) {
      buf_t out;
      weak_multi_agree_random(job, bitlen, out);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

static void BM_multi_pairwise_agree_random(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  int bitlen = state.range(2);

  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [&bitlen](job_mp_t& job) {
      std::vector<buf_t> out;
      multi_pairwise_agree_random(job, bitlen, out);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

BENCHMARK(BM_weak_agree_random)
    ->UseManualTime()
    ->Name("WeakAgreeRandom-2P")
    ->ArgsProduct({{1, 2, 3}, {1, 2}, {256, 1024, 4096}})
    ->Iterations(10000);
BENCHMARK(BM_agree_random)
    ->UseManualTime()
    ->Name("AgreeRandom-2P")
    ->ArgsProduct({{1, 2, 3, 4}, {1, 2}, {256, 1024, 4096}})
    ->Iterations(10000);
BENCHMARK(BM_multi_agree_random)
    ->UseManualTime()
    ->Name("MultiAgreeRandom-MP")
    ->ArgsProduct({{1, 2, 3}, {0, 1, 2, 3}, {256, 1024, 4096}})
    ->Iterations(1000);
BENCHMARK(BM_weak_multi_agree_random)
    ->UseManualTime()
    ->Name("WeakMultiAgreeRandom-MP")
    ->ArgsProduct({{1, 2, 3}, {0, 1, 2, 3}, {256, 1024, 4096}})
    ->Iterations(1000);
BENCHMARK(BM_multi_pairwise_agree_random)
    ->UseManualTime()
    ->Name("MultiPairwiseAgreeRandom-MP")
    ->ArgsProduct({{1, 2, 3}, {0, 1, 2, 3}, {256, 1024, 4096}})
    ->Iterations(1000);
