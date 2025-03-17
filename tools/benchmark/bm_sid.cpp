#include <benchmark/benchmark.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/sid.h>

#include "mpc_util.h"

using namespace coinbase;
using namespace coinbase::mpc;

static void BM_generate_sid(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);
  int total_rounds = 3;

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [](job_2p_t& job) {
      buf_t out;
      generate_sid_fixed_2p(job, party_t::p1, out);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_generate_sid_dynamic(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);
  int total_rounds = 3;

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [](job_2p_t& job) {
      buf_t out;
      auto pid1 = job.get_pid(party_t::p1);
      auto pid2 = job.get_pid(party_t::p2);
      generate_sid_dynamic_2p(job, party_t::p1, pid1, pid2, out);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_generate_sid_mp(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [](job_mp_t& job) {
      buf_t sid;
      generate_sid_fixed_mp(job, sid);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

static void BM_generate_sid_dynamic_mp(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  std::vector<crypto::mpc_pid_t> pids(4);
  for (int i = 0; i < 4; i++) {
    pids[i] = bn_t::rand_bitlen(256);
  }
  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [&pids](job_mp_t& job) {
      buf_t sid;
      generate_sid_dynamic_mp(job, pids, sid);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

BENCHMARK(BM_generate_sid)
    ->UseManualTime()
    ->Name("GenerateSID-Fixed-2P")
    ->ArgsProduct({{1, 2, 3}, {1, 2}})
    ->Iterations(10000);
BENCHMARK(BM_generate_sid_dynamic)
    ->UseManualTime()
    ->Name("GenerateSID-Dynamic-2P")
    ->ArgsProduct({{1, 2, 3}, {1, 2}})
    ->Iterations(10000);
BENCHMARK(BM_generate_sid_mp)
    ->UseManualTime()
    ->Name("GenerateSID-Fixed-MP")
    ->ArgsProduct({{1, 2}, {0, 1, 2, 3}})
    ->Iterations(10000);
BENCHMARK(BM_generate_sid_dynamic_mp)
    ->UseManualTime()
    ->Name("GenerateSID-Dynamic-MP")
    ->ArgsProduct({{1, 2}, {0, 1, 2, 3}})
    ->Iterations(10000);
