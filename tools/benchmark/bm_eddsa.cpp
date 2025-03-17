#include <benchmark/benchmark.h>

#include <local_network/mpc_runner.h>

#include <cbmpc/core/buf.h>
#include <cbmpc/protocol/eddsa.h>

#include "mpc_util.h"

using namespace coinbase;
using namespace coinbase::mpc;

static void BM_eddsa_2p_keygen(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);

  int total_rounds = 4;

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [](job_2p_t& job) {
      eddsa2pc::key_t key;
      buf_t sid;
      eckey::key_share_2p_t::dkg(job, crypto::curve_ed25519, key, sid);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_eddsa_2p_refresh(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);

  int total_rounds = 4;

  buf_t sid;
  auto pre_mpc_runner = std::make_unique<testutils::mpc_runner_t>(2);
  std::array<eddsa2pc::key_t, 2> keys;
  pre_mpc_runner->run_2pc([&keys, &sid](job_2p_t& job) {
    eckey::key_share_2p_t::dkg(job, crypto::curve_ed25519, keys[job.get_party_idx()], sid);
  });

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [&sid, &keys](job_2p_t& job) {
      eddsa2pc::key_t new_key;
      eckey::key_share_2p_t::refresh(job, keys[job.get_party_idx()], new_key);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_eddsa_2p_sign(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);

  int total_rounds = 5;
  int n_message = state.range(2);

  auto pre_mpc_runner = std::make_unique<testutils::mpc_runner_t>(2);
  buf_t sid;
  std::array<eddsa2pc::key_t, 2> keys;
  pre_mpc_runner->run_2pc([&keys, &sid](job_2p_t& job) {
    eckey::key_share_2p_t::dkg(job, crypto::curve_ed25519, keys[job.get_party_idx()], sid);
  });

  std::vector<buf_t> data(n_message);
  for (int i = 0; i < n_message; i++) data[i] = crypto::gen_random(32);
  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [&data, &keys](job_2p_t& job) {
      std::vector<buf_t> sig_bufs;
      eddsa2pc::sign_batch(job, keys[job.get_party_idx()], buf_t::to_mems(data), sig_bufs);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_eddsa_mp_keygen(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  /* start benchmarking */
  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [](job_mp_t& job) {
      buf_t sid;
      ecurve_t curve = crypto::curve_ed25519;
      eddsampc::key_t key;
      eckey::key_share_mp_t::dkg(job, curve, key, sid);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

static void BM_eddsa_mp_refresh(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  /* Setup protocol materials */
  auto pre_mpc_runner = std::make_unique<testutils::mpc_runner_t>(4);
  buf_t sid;
  ecurve_t curve = crypto::curve_ed25519;
  std::array<eddsampc::key_t, 4> keys;
  pre_mpc_runner->run_mpc(
      [&curve, &sid, &keys](job_mp_t& job) { eckey::key_share_mp_t::dkg(job, curve, keys[job.get_party_idx()], sid); });

  std::array<eddsampc::key_t, 4> new_keys;
  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [&sid, &keys, &new_keys](job_mp_t& job) {
      int i = job.get_party_idx();
      eckey::key_share_mp_t::refresh(job, sid, keys[i], new_keys[i]);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

static void BM_eddsa_mp_sign(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{0, 3}, {1, 0}, {1, 0}, {1, 0}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  /* Setup protocol materials */
  auto pre_mpc_runner = std::make_unique<testutils::mpc_runner_t>(4);
  buf_t sid;
  ecurve_t curve = crypto::curve_ed25519;
  std::array<eddsampc::key_t, 4> keys;
  pre_mpc_runner->run_mpc(
      [&curve, &sid, &keys](job_mp_t& job) { eckey::key_share_mp_t::dkg(job, curve, keys[job.get_party_idx()], sid); });

  std::vector<buf_t> data(3);
  for (int i = 0; i < data.size(); i++) data[i] = crypto::gen_random(32);
  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [&sid, &keys, &data](job_mp_t& job) {
      int i = job.get_party_idx();
      std::vector<buf_t> sig_buf;
      eddsampc::sign_batch(job, keys[i], buf_t::to_mems(data), party_idx_t(0), sig_buf);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

BENCHMARK(BM_eddsa_2p_keygen)
    ->UseManualTime()
    ->Name("Schnorr-2PC-KeyGen-2P")
    ->ArgsProduct({{1, 2, 3, 4}, {1, 2}})
    ->Iterations(10);
BENCHMARK(BM_eddsa_2p_refresh)
    ->UseManualTime()
    ->Name("Schnorr-2PC-Refresh-2P")
    ->ArgsProduct({{1, 2, 3, 4}, {1, 2}})
    ->Iterations(200);
BENCHMARK(BM_eddsa_2p_sign)
    ->UseManualTime()
    ->Name("Schnorr-2PC-Sign-2P")
    ->ArgsProduct({{1, 2, 3, 4, 5}, {1, 2}, {1, 4, 16}})
    ->Iterations(50);
BENCHMARK(BM_eddsa_mp_keygen)
    ->UseManualTime()
    ->Name("Schnorr-MPC-KeyGen-MP")
    ->ArgsProduct({{1, 2, 3}, {0, 1, 2, 3}})
    ->Iterations(1);
BENCHMARK(BM_eddsa_mp_refresh)
    ->UseManualTime()
    ->Name("Schnorr-MPC-Refresh-MP")
    ->ArgsProduct({{1, 2, 3}, {0, 1, 2, 3}})
    ->Iterations(10);
BENCHMARK(BM_eddsa_mp_sign)
    ->UseManualTime()
    ->Name("Schnorr-MPC-Sign-MP")
    ->ArgsProduct({{1, 2, 3, 4}, {0, 1, 2, 3}})
    ->Iterations(10);
