#pragma once
#include <local_network/mpc_runner.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/mpc_job.h>
#include <cbmpc/protocol/mpc_job_session.h>

//

#include <chrono>
#include <mutex>
#include <sstream>
#include <thread>

static std::mutex cout_mutex;

#define THREAD_SAFE_LOG(...) \
  // do {                                            \
    std::ostringstream oss;                       \
    oss << __VA_ARGS__;                           \
    std::lock_guard<std::mutex> lock(cout_mutex); \
    std::cout << oss.str() << std::endl;          \
  } while (0)

namespace coinbase {

struct abort_channel_t {
  std::mutex m;
  std::condition_variable cv;
  bool another_job_abort = false;
};

class bm_job_2p_t : public mpc::job_2p_t {
 public:
  // constructor that takes extra parameter -- target_round, then call the parent constructor
  bm_job_2p_t(mpc::party_t bm_party, int bm_round, mpc::party_t party,
              std::shared_ptr<mpc::parallel_data_transport_t> _network_ptr,
              std::shared_ptr<abort_channel_t> abort_channel)
      : job_2p_t(party, testutils::mpc_runner_t::test_pnames[0], testutils::mpc_runner_t::test_pnames[1], _network_ptr),
        bm_party(bm_party),
        bm_round(bm_round),
        abort_channel(abort_channel) {
    reset();
    reset_timer();
  }
  void reset() {
    current_round = 1;
    message_size = 0;
  }

  /* timer functions */

  void reset_timer() {
    accumulate = std::chrono::duration<double>::zero();
    paused = true;
  }
  void pause_timer() {
    if (paused) return;
    paused = true;
    accumulate +=
        std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start);
  }
  void resume_timer() {
    start = std::chrono::high_resolution_clock::now();
    paused = false;
  }

  // Get the timer result by calling accumulate.count()
  double get_timer_result() { return accumulate.count(); }

  /* override messaging implementation in order to control the timers and abort according the benchmarking goal */
  error_t mpc_abort(error_t rv, const std::string& message = "") {
    if (get_party_idx() != mpc::party_idx_t(bm_party)) reset();
    return mpc::job_2p_t::mpc_abort(rv, message);
  }

  error_t send_impl(mpc::party_idx_t to, mem_t msg) override {
    THREAD_SAFE_LOG(get_party_idx() << ": round " << current_round << " send to " << to);
    error_t rv = UNINITIALIZED_ERROR;

    if (bm_round == current_round) {
      if (get_party() != bm_party) assert(false);
      pause_timer();
      return mpc_abort(error(E_CF_MPC_BENCHMARK));
    }

    if (rv = job_2p_t::send_impl(to, msg)) return rv;

    current_round++;
    if (current_round == bm_round) {
      if (party_index == mpc::party_idx_t(bm_party)) {
        THREAD_SAFE_LOG("=============== Resuming timer after send ==============" << msg.size);
        std::unique_lock<std::mutex> lock(abort_channel->m);
        abort_channel->cv.wait(lock, [this] { return this->abort_channel->another_job_abort; });
        abort_channel->another_job_abort = false;
        resume_timer();
      } else {
        THREAD_SAFE_LOG(get_party_idx() << ": Abort");
        {
          std::lock_guard<std::mutex> lock(abort_channel->m);
          abort_channel->another_job_abort = true;
        }
        abort_channel->cv.notify_all();
        return mpc_abort(error(E_CF_MPC_BENCHMARK));
      }
    }

    return SUCCESS;
  }

  error_t receive_impl(mpc::party_idx_t from, buf_t& msg) override {
    THREAD_SAFE_LOG(get_party_idx() << ": round " << current_round << " receive from " << from);
    error_t rv = UNINITIALIZED_ERROR;

    if (bm_round == current_round) {
      if (get_party() != bm_party) assert(false);
      pause_timer();
      return mpc_abort(error(E_CF_MPC_BENCHMARK));
    }

    if (rv = job_2p_t::receive_impl(from, msg)) return rv;

    current_round++;
    if (current_round == bm_round) {
      if (party_index == mpc::party_idx_t(bm_party)) {
        THREAD_SAFE_LOG("=============== Resuming timer after receive ==============");
        message_size = msg.size();
        std::unique_lock<std::mutex> lock(abort_channel->m);
        abort_channel->cv.wait(lock, [this] { return this->abort_channel->another_job_abort; });
        abort_channel->another_job_abort = false;
        resume_timer();
      } else {
        THREAD_SAFE_LOG(get_party_idx() << ": Abort");
        {
          std::lock_guard<std::mutex> lock(abort_channel->m);
          abort_channel->another_job_abort = true;
        }
        abort_channel->cv.notify_all();
        return mpc_abort(error(E_CF_MPC_BENCHMARK));
      }
    }

    return SUCCESS;
  }

  int get_message_size() { return message_size; }

 private:
  const int bm_round;
  int current_round;
  const mpc::party_t bm_party;
  int message_size;
  std::chrono::high_resolution_clock::time_point start;
  std::chrono::duration<double> accumulate;
  bool paused = true;

  std::shared_ptr<abort_channel_t> abort_channel;
};

struct bm_2pc_runner_t {
  int bm_round;
  mpc::party_t bm_party;
  std::shared_ptr<testutils::mpc_runner_t> mpc_runner;
  std::shared_ptr<bm_job_2p_t> main_job;
};

static bm_2pc_runner_t init_2pc_benchmarking(benchmark::State& state) {
  int bm_round = state.range(0);
  mpc::party_t bm_party = mpc::party_t::p1;
  if (state.range(1) == 2) bm_party = mpc::party_t::p2;

  /* create 2pc jobs and a runner for them */
  std::shared_ptr<bm_job_2p_t> p1_job, p2_job;
  auto abort_channel = std::make_shared<abort_channel_t>();
  p1_job = std::make_shared<bm_job_2p_t>(bm_party, bm_round, mpc::party_t::p1, nullptr, abort_channel);
  p2_job = std::make_shared<bm_job_2p_t>(bm_party, bm_round, mpc::party_t::p2, nullptr, abort_channel);
  auto mpc_runner = std::make_shared<testutils::mpc_runner_t>(p1_job, p2_job);
  std::shared_ptr<bm_job_2p_t> main_job = bm_party == mpc::party_t::p1 ? p1_job : p2_job;

  return {bm_round, bm_party, mpc_runner, main_job};
}

struct bm_2pc_result_t {
  double time;
  int message_size;
};

static bm_2pc_result_t run_bm_2pc(bm_2pc_runner_t bm_runner, int total_rounds,
                                  std::function<void(mpc::job_2p_t&)> protocol) {
  auto bm_round = bm_runner.bm_round;
  auto bm_party = bm_runner.bm_party;
  auto mpc_runner = bm_runner.mpc_runner;
  auto main_job = bm_runner.main_job;

  main_job->reset_timer();
  if (bm_round == 1) main_job->resume_timer();  // no message before the first round to resume (start) the timer.

  mpc_runner->run_2pc([&bm_round, &bm_party, &protocol](mpc::job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;

    // If measuring the first round, only the benchmarking party has to run the protocol.
    if (bm_round == 1 && job.get_party() != bm_party) return;

    protocol(job);
  });

  // If measuring the last round, we run the the end of the protocol, and did not pause the timer and reset the job.
  if (bm_round == total_rounds) {
    main_job->pause_timer();
  }

  auto result = bm_2pc_result_t{main_job->get_timer_result(), main_job->get_message_size()};

  main_job->reset();
  return result;
}

struct msg_count_t {
  int sent;
  int received;
};

struct abort_channel_mp_t {
  std::mutex m;
  std::condition_variable cv;
  int aborted_count = 0;
};

class bm_job_mp_t : public mpc::job_mp_t {
 public:
  // constructor that takes extra parameter -- target_round, then call the parent constructor
  bm_job_mp_t(int bm_party, int bm_round, std::vector<std::vector<msg_count_t>> msg_counts, int _parties,
              mpc::party_idx_t index, std::shared_ptr<mpc::parallel_data_transport_t> _network_ptr,
              std::shared_ptr<abort_channel_mp_t> abort_channel)
      : job_mp_t(index,
                 std::vector<crypto::pname_t>(testutils::mpc_runner_t::test_pnames.begin(),
                                              testutils::mpc_runner_t::test_pnames.begin() + _parties),
                 _network_ptr),
        bm_party(bm_party),
        bm_round(bm_round),
        msg_counts(msg_counts),
        abort_channel(abort_channel) {
    reset();
    reset_timer();
  }
  void reset() {
    current_round = 1;
    send_count = 0;
    receive_count = 0;
    send_message_size = 0;
    receive_message_size = 0;
    // abort_channel->aborted_count = 0;
  }

  /* timer functions */

  void reset_timer() {
    accumulate = std::chrono::duration<double>::zero();
    paused = true;
  }
  void pause_timer() {
    if (paused) return;
    paused = true;
    accumulate +=
        std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start);
  }
  void resume_timer() {
    start = std::chrono::high_resolution_clock::now();
    paused = false;
  }

  // Get the timer result by calling accumulate.count()
  double get_timer_result() { return accumulate.count(); }

  /* override messaging implementation in order to control the timers and abort according the benchmarking goal */
  error_t mpc_abort(error_t rv, const std::string& message = "") {
    if (get_party_idx() != bm_party) reset();
    return mpc::job_mp_t::mpc_abort(rv, message);
  }

  error_t send_impl(mpc::party_idx_t to, mem_t msg) override {
    THREAD_SAFE_LOG(get_party_idx() << ": round " << current_round << " send to " << to);
    error_t rv = UNINITIALIZED_ERROR;

    if (bm_round == current_round) {
      if (get_party_idx() != bm_party) assert(false);
      pause_timer();
      return mpc_abort(error(E_CF_MPC_BENCHMARK));
    }

    if (rv = job_mp_t::send_impl(to, msg)) return rv;

    if (bm_party == get_party_idx() && bm_round == current_round + 1) {
      // THREAD_SAFE_LOG(get_party_idx() << " #################### send " << msg.size);
      send_message_size += msg.size;
    }

    auto msg_goal = get_msg_count(current_round);
    send_count++;

    // THREAD_SAFE_LOG(get_party_idx() << " #### send count " << send_count << " goal " << msg_goal.sent);
    // THREAD_SAFE_LOG(get_party_idx() << " #### receive count " << receive_count << " goal " << msg_goal.received);
    if (msg_goal.sent == send_count && msg_goal.received == receive_count) {
      current_round++;
      send_count = 0;
      receive_count = 0;
      if (rv = bm_round_start_handler()) return rv;
    }

    return SUCCESS;
  }

  error_t receive_impl(mpc::party_idx_t from, buf_t& msg) override {
    THREAD_SAFE_LOG(get_party_idx() << ": round " << current_round << " receive from " << from);
    error_t rv = UNINITIALIZED_ERROR;

    if (bm_round == current_round) {
      if (get_party_idx() != bm_party) assert(false);
      pause_timer();
      return mpc_abort(error(E_CF_MPC_BENCHMARK));
    }

    if (rv = job_mp_t::receive_impl(from, msg)) return rv;

    if (bm_party == get_party_idx() && bm_round == current_round + 1) {
      // THREAD_SAFE_LOG("#################### receive " << msg.size());
      receive_message_size += msg.size();
    }

    auto msg_goal = get_msg_count(current_round);
    receive_count++;

    if (msg_goal.sent == send_count && msg_goal.received == receive_count) {
      current_round++;
      send_count = 0;
      receive_count = 0;
      if (rv = bm_round_start_handler()) return rv;
    }

    return SUCCESS;
  }

  error_t receive_many_impl(std::vector<mpc::party_idx_t> from_set, std::vector<buf_t>& outs) override {
    THREAD_SAFE_LOG(get_party_idx() << ": round " << current_round << " receive many");
    error_t rv = UNINITIALIZED_ERROR;

    if (bm_round == current_round) {
      if (get_party_idx() != bm_party) assert(false);
      pause_timer();
      return mpc_abort(error(E_CF_MPC_BENCHMARK));
    }

    if (rv = job_mp_t::receive_many_impl(from_set, outs)) return rv;

    if (bm_party == get_party_idx() && bm_round == current_round + 1) {
      for (auto& msg : outs) {
        // THREAD_SAFE_LOG("#################### receive many " << msg.size);
        receive_message_size += msg.size();
      }
    }

    auto msg_goal = get_msg_count(current_round);
    // THREAD_SAFE_LOG(get_party_idx() << " #### send count " << send_count << " goal " << msg_goal.sent);
    // THREAD_SAFE_LOG(get_party_idx() << " #### receive count " << receive_count << " goal " << msg_goal.received);
    receive_count = msg_goal.received;

    if (msg_goal.sent == send_count && msg_goal.received == receive_count) {
      current_round++;
      send_count = 0;
      receive_count = 0;
      if (rv = bm_round_start_handler()) return rv;
    }

    return SUCCESS;
  }

  error_t bm_round_start_handler() {
    if (bm_round == current_round) {
      if (get_party_idx() == bm_party) {
        THREAD_SAFE_LOG("=============== Resuming timer ==============");
        std::unique_lock<std::mutex> lock(abort_channel->m);
        // Wait until all other threads finish their work
        abort_channel->cv.wait(lock,
                               [this] { return this->abort_channel->aborted_count == this->get_n_parties() - 1; });
        abort_channel->aborted_count = 0;
        resume_timer();
      } else {
        THREAD_SAFE_LOG(get_party_idx() << ": Abort");
        {
          std::lock_guard<std::mutex> lock(abort_channel->m);
          abort_channel->aborted_count++;
          abort_channel->cv.notify_all();
        }
        return mpc_abort(error(E_CF_MPC_BENCHMARK));
      }
    }
    return SUCCESS;
  }

  const int bm_round;
  int current_round;
  int message_size;
  const int bm_party = 0;
  std::vector<std::vector<msg_count_t>> msg_counts;

  std::tuple<int, int> get_message_size() { return std::make_tuple(send_message_size, receive_message_size); }

 private:
  int send_count = 0;
  int receive_count = 0;
  int send_message_size = 0;
  int receive_message_size = 0;

  std::chrono::high_resolution_clock::time_point start;
  std::chrono::duration<double> accumulate;
  bool paused = true;

  std::shared_ptr<abort_channel_mp_t> abort_channel;

  msg_count_t get_msg_count(int round) { return msg_counts[round - 1][get_party_idx()]; }
};

struct bm_mpc_runner_t {
  int bm_round;
  mpc::party_idx_t bm_party;
  int n_rounds;
  std::shared_ptr<testutils::mpc_runner_t> mpc_runner;
  std::shared_ptr<bm_job_mp_t> main_job;
};

static bm_mpc_runner_t init_mpc_benchmarking(benchmark::State& state,
                                             std::vector<std::vector<msg_count_t>> msg_counts) {
  int bm_round = state.range(0);
  mpc::party_idx_t bm_party = state.range(1);
  int n_rounds = msg_counts.size();
  int n_parties = msg_counts[0].size();

  /* create mpc jobs and a runner for them */
  auto abort_channel = std::make_shared<abort_channel_mp_t>();
  std::vector<std::shared_ptr<mpc::job_mp_t>> jobs(n_parties);
  std::shared_ptr<bm_job_mp_t> main_job;
  main_job = std::make_shared<bm_job_mp_t>(bm_party, bm_round, msg_counts, n_parties, bm_party, nullptr, abort_channel);
  jobs[bm_party] = main_job;
  for (int i = 0; i < n_parties; i++) {
    if (i == bm_party) continue;
    jobs[i] = std::make_shared<bm_job_mp_t>(bm_party, bm_round, msg_counts, n_parties, mpc::party_idx_t(i), nullptr,
                                            abort_channel);
  }
  auto mpc_runner = std::make_shared<testutils::mpc_runner_t>(jobs);

  return {bm_round, bm_party, n_rounds, mpc_runner, main_job};
}

struct bm_mpc_result_t {
  double time;
  int send_message_size = 0;
  int receive_message_size = 0;
};

static bm_mpc_result_t run_bm_mpc(bm_mpc_runner_t bm_runner, std::function<void(mpc::job_mp_t&)> protocol) {
  bm_runner.main_job->reset_timer();
  bm_runner.main_job->reset();
  if (bm_runner.bm_round == 1)
    bm_runner.main_job->resume_timer();  // no message before the first round to resume (start) the timer.

  bm_runner.mpc_runner->run_mpc([&bm_runner, &protocol](mpc::job_mp_t& job) {
    error_t rv = UNINITIALIZED_ERROR;

    // If measuring the first round, only the benchmarking party has to run the protocol.
    if (bm_runner.bm_round == 1 && job.get_party_idx() != bm_runner.bm_party) return;

    protocol(job);
  });

  // If measuring the last round, we run until the end of the protocol, and did not pause the timer and reset the job.
  if (bm_runner.bm_round == bm_runner.n_rounds) {
    bm_runner.main_job->pause_timer();
  }

  auto [a, b] = bm_runner.main_job->get_message_size();
  THREAD_SAFE_LOG("=============== send " << a << " receive " << b);

  bm_mpc_result_t res;
  std::tie(res.send_message_size, res.receive_message_size) = bm_runner.main_job->get_message_size();
  res.time = bm_runner.main_job->get_timer_result();
  return res;
}

}  // namespace coinbase