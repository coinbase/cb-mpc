#include <benchmark/benchmark.h>

#include <cbmpc/protocol/ecdsa_2p.h>
#include <cbmpc/protocol/ecdsa_mp.h>

#include "local_network/mpc_runner.h"
#include "mpc_util.h"

using namespace coinbase;
using namespace coinbase::mpc;

static void BM_ecdsa_2p_keygen(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);

  int total_rounds = 4;

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [](job_2p_t& job) {
      ecdsa2pc::key_t key;
      ecdsa2pc::dkg(job, crypto::curve_secp256k1, key);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_ecdsa_2p_refresh(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);

  int total_rounds = 4;

  auto pre_mpc_runner = std::make_unique<testutils::mpc_runner_t>(2);
  std::array<ecdsa2pc::key_t, 2> keys;
  pre_mpc_runner->run_2pc(
      [&keys](job_2p_t& job) { ecdsa2pc::dkg(job, crypto::curve_secp256k1, keys[job.get_party_idx()]); });

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [&keys](job_2p_t& job) {
      ecdsa2pc::key_t new_key;
      ecdsa2pc::refresh(job, keys[job.get_party_idx()], new_key);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_ecdsa_2p_sign(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);

  int total_rounds = 5;
  int n_message = state.range(2);

  std::vector<buf_t> data(n_message);
  for (int i = 0; i < n_message; i++) data[i] = crypto::gen_random(32);
  buf_t sid = crypto::gen_random_bitlen(SEC_P_COM);
  auto pre_mpc_runner = std::make_unique<testutils::mpc_runner_t>(2);
  std::array<ecdsa2pc::key_t, 2> keys;
  pre_mpc_runner->run_2pc(
      [&keys](job_2p_t& job) { ecdsa2pc::dkg(job, crypto::curve_secp256k1, keys[job.get_party_idx()]); });

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [&sid, &keys, &data](job_2p_t& job) {
      std::vector<buf_t> sig_bufs;
      ecdsa2pc::sign_batch(job, sid, keys[job.get_party_idx()], buf_t::to_mems(data), sig_bufs);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_ecdsa_2p_sign_global_abort(benchmark::State& state) {
  auto bm_2pc_runner = init_2pc_benchmarking(state);

  int total_rounds = 6;
  int n_message = state.range(2);

  std::vector<buf_t> data(n_message);
  for (int i = 0; i < n_message; i++) data[i] = crypto::gen_random(32);
  buf_t sid = crypto::gen_random_bitlen(SEC_P_COM);
  auto pre_mpc_runner = std::make_unique<testutils::mpc_runner_t>(2);
  std::array<ecdsa2pc::key_t, 2> keys;
  pre_mpc_runner->run_2pc(
      [&keys](job_2p_t& job) { ecdsa2pc::dkg(job, crypto::curve_secp256k1, keys[job.get_party_idx()]); });

  for (auto _ : state) {
    auto result = run_bm_2pc(bm_2pc_runner, total_rounds, [&sid, &keys, &data](job_2p_t& job) {
      std::vector<buf_t> sig_bufs;
      ecdsa2pc::sign_with_global_abort_batch(job, sid, keys[job.get_party_idx()], buf_t::to_mems(data), sig_bufs);
    });

    state.SetIterationTime(result.time);
    if (result.message_size > 0) state.counters["size"] = result.message_size;
  }
}

static void BM_ecdsa_mp_keygen(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  /* start benchmarking */
  buf_t sid;
  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [](job_mp_t& job) {
      buf_t sid;
      ecurve_t curve = crypto::curve_secp256k1;
      ecdsampc::key_t key;
      ecdsampc::dkg(job, curve, key, sid);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

static void BM_ecdsa_mp_refresh(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  /* Setup protocol materials */
  auto pre_mpc_runner = std::make_unique<testutils::mpc_runner_t>(4);
  buf_t sid;
  ecurve_t curve = crypto::curve_secp256k1;
  std::array<ecdsampc::key_t, 4> keys;
  pre_mpc_runner->run_mpc(
      [&curve, &sid, &keys](job_mp_t& job) { ecdsampc::dkg(job, curve, keys[job.get_party_idx()], sid); });

  std::array<ecdsampc::key_t, 4> new_keys;
  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [&sid, &keys, &new_keys](job_mp_t& job) {
      int i = job.get_party_idx();
      ecdsampc::refresh(job, sid, keys[i], new_keys[i]);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

static void BM_ecdsa_mp_sign(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},         {{3, 3}, {3, 3}, {3, 3}, {3, 3}}, {{0, 3}, {1, 2}, {2, 1}, {3, 0}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},         {{0, 3}, {1, 2}, {2, 1}, {3, 0}}, {{3, 3}, {3, 3}, {3, 3}, {3, 3}},
      {{3, 3}, {3, 3}, {3, 3}, {3, 3}},         {{3, 3}, {3, 3}, {3, 3}, {3, 3}}, {{0, 3}, {1, 0}, {1, 0}, {1, 0}},
      {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  /* Setup protocol materials */
  auto pre_mpc_runner = std::make_unique<testutils::mpc_runner_t>(4);
  buf_t sid;
  ecurve_t curve = crypto::curve_secp256k1;
  std::array<ecdsampc::key_t, 4> keys;
  pre_mpc_runner->run_mpc(
      [&curve, &sid, &keys](job_mp_t& job) { ecdsampc::dkg(job, curve, keys[job.get_party_idx()], sid); });

  buf_t data = crypto::gen_random(32);

  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [&sid, &keys, &data](job_mp_t& job) {
      int i = job.get_party_idx();
      buf_t sig;
      ecdsampc::sign(job, keys[i], data, party_idx_t(0), sig);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

std::vector<std::vector<int>> test_ot_role(int n) {
  std::vector<std::vector<int>> ot_role_map(n, std::vector<int>(n));
  for (int i = 0; i < n; i++) {
    ot_role_map[i][i] = ecdsampc::ot_no_role;
  }

  for (int i = 0; i <= n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
      ot_role_map[i][j] = ecdsampc::ot_sender;
      ot_role_map[j][i] = ecdsampc::ot_receiver;
    }
  }
  return ot_role_map;
}

static void BM_ecdsa_mp_sign_2p(benchmark::State& state) {
  std::vector<std::vector<msg_count_t>> msg_counts = {
      {{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}, {{0, 1}, {1, 0}}, {{1, 1}, {1, 1}}, {{0, 1}, {1, 0}},
      {{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}, {{0, 1}, {1, 0}}, {{-1, -1}, {-1, -1}},
  };
  auto bm_runner = init_mpc_benchmarking(state, msg_counts);

  /* start benchmarking */
  auto pre_mpc_runner = std::make_unique<testutils::mpc_runner_t>(-2);
  buf_t sid;
  ecurve_t curve = crypto::curve_secp256k1;
  std::array<ecdsampc::key_t, 2> keys;
  pre_mpc_runner->run_mpc(
      [&curve, &sid, &keys](job_mp_t& job) { ecdsampc::dkg(job, curve, keys[job.get_party_idx()], sid); });

  buf_t data = crypto::gen_random(32);
  for (auto _ : state) {
    auto result = run_bm_mpc(bm_runner, [&keys, &data](job_mp_t& job) {
      int i = job.get_party_idx();
      buf_t sig;
      ecdsampc::sign(job, keys[i], data, party_idx_t(0), test_ot_role(2), sig);
    });

    state.SetIterationTime(result.time);
    state.counters["s_size"] = result.send_message_size;
    state.counters["r_size"] = result.receive_message_size;
  }
}

BENCHMARK(BM_ecdsa_2p_keygen)
    ->UseManualTime()
    ->Name("ECDSA-2PC-KeyGen-2P")
    ->ArgsProduct({{1, 2, 3, 4}, {1, 2}})
    ->Iterations(20);
BENCHMARK(BM_ecdsa_2p_refresh)
    ->UseManualTime()
    ->Name("ECDSA-2PC-Refresh-2P")
    ->ArgsProduct({{1, 2, 3, 4}, {1, 2}})
    ->Iterations(20);
BENCHMARK(BM_ecdsa_2p_sign)
    ->UseManualTime()
    ->Name("ECDSA-2PC-Sign-2P")
    ->ArgsProduct({{1, 2, 3, 4, 5}, {1, 2}, {1, 4, 16}})
    ->Iterations(20);
BENCHMARK(BM_ecdsa_2p_sign_global_abort)
    ->UseManualTime()
    ->Name("ECDSA-2PC-Sign-With-Global-Abort-2P")
    ->ArgsProduct({{1, 2, 3, 4, 5}, {1, 2}, {1, 4, 16}})
    ->Iterations(20);

BENCHMARK(BM_ecdsa_mp_keygen)
    ->UseManualTime()
    ->Name("ECDSA-MPC-KeyGen-MP")
    ->ArgsProduct({{1, 2, 3}, {0, 1, 2, 3}})
    ->Iterations(10);
BENCHMARK(BM_ecdsa_mp_refresh)
    ->UseManualTime()
    ->Name("ECDSA-MPC-Refresh-MP")
    ->ArgsProduct({{1, 2, 3}, {0, 1, 2, 3}})
    ->Iterations(10);
BENCHMARK(BM_ecdsa_mp_sign)
    ->UseManualTime()
    ->Name("ECDSA-MPC-Sign-MP")
    ->ArgsProduct({{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, {0, 1, 2, 3}})
    ->Iterations(10);

BENCHMARK(BM_ecdsa_mp_sign_2p)
    ->UseManualTime()
    ->Name("ECDSA-MPC-Sign-2P")
    ->ArgsProduct({{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, {0, 1}})
    ->Iterations(10);
