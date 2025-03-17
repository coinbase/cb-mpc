#include "mpc_runner.h"

#include <cbmpc/crypto/base_pki.h>

using namespace coinbase::mpc;

namespace coinbase::testutils {

error_t local_data_transport_t::send(const party_idx_t receiver, const mem_t& msg) {
  net_context_ptr->send(receiver, msg);
  return SUCCESS;
}

error_t local_data_transport_t::receive(const party_idx_t sender, mem_t& msg) {
  return net_context_ptr->receive(sender, msg);
}

error_t local_data_transport_t::receive_all(const std::vector<party_idx_t>& senders, std::vector<mem_t>& msgs) {
  return net_context_ptr->receive_all(senders, msgs);
}

void mpc_runner_t::init_network(int n_parties) {
  partners.resize(n_parties);
  data_transports.resize(n_parties);
  net_contexts.resize(n_parties);

  for (int i = 0; i < n; i++) {
    partners[i] = std::make_shared<partner_t>(i);
    net_contexts[i] = std::make_shared<mpc_net_context_t>(i);
    data_transports[i] = std::make_shared<local_data_transport_t>(net_contexts[i]);
  }
  for (int i = 0; i < n; i++) net_contexts[i]->init_with_peers(net_contexts);
}

mpc_runner_t::mpc_runner_t(int n_parties) : n(n_parties) {
  if (n == 2) {
    init_network(2);
    job_2ps[0] = std::make_shared<job_session_2p_t>(party_t::p1, test_pids[0], test_pids[1],
                                                    std::make_shared<network_t>(get_data_transport_ptr(0)), 0);
    job_2ps[1] = std::make_shared<job_session_2p_t>(party_t::p2, test_pids[0], test_pids[1],
                                                    std::make_shared<network_t>(get_data_transport_ptr(1)), 0);
  } else {
    if (n == -2) n = 2;
    init_network(n);
    std::vector<bn_t> pids(test_pids.begin(), test_pids.begin() + n);
    for (int i = 0; i < n; i++) {
      job_mps[i] = std::make_shared<job_session_mp_t>(party_idx_t(i), pids,
                                                      std::make_shared<network_t>(get_data_transport_ptr(i)), 0);
    }
  }
}

void mpc_runner_t::set_new_network_2p() {
  job_2ps[0]->set_network(party_t::p1, std::make_shared<network_t>(get_data_transport_ptr(0)));
  job_2ps[1]->set_network(party_t::p2, std::make_shared<network_t>(get_data_transport_ptr(1)));
}

void mpc_runner_t::set_new_network_mp() {
  for (int i = 0; i < n; i++) {
    job_mps[i]->set_network(party_idx_t(i), std::make_shared<network_t>(get_data_transport_ptr(i)));
  }
}

mpc_runner_t::mpc_runner_t(std::shared_ptr<job_session_2p_t> job1, std::shared_ptr<job_session_2p_t> job2) : n(2) {
  init_network(n);
  job_2ps[0] = job1;
  job_2ps[1] = job2;
  set_new_network_2p();
}

mpc_runner_t::mpc_runner_t(std::vector<std::shared_ptr<job_session_mp_t>> jobs) : n(jobs.size()) {
  init_network(n);
  for (int i = 0; i < n; i++) {
    job_mps[i] = jobs[i];
  }
  set_new_network_mp();
}

void mpc_runner_t::start_partners() {
  for (int i = 0; i < n; i++) partners[i]->start();
  for (int i = 0; i < n; i++) partners[i]->add_runner(*this);
}

void mpc_runner_t::stop_partners() {
  for (int i = 0; i < n; i++) partners[i]->stop();
  for (int i = 0; i < n; i++) partners[i]->join();
}

void mpc_runner_t::abort_connection() {
  for (int i = 0; i < n; i++) net_contexts[i]->abort();
}

void mpc_runner_t::reset_net_contexts() {
  for (int i = 0; i < n; i++) net_contexts[i]->reset();
}

void mpc_runner_t::run_on_partner(party_idx_t role) {
  protocol_f(role);
  std::unique_lock scoped(mutex);
  finished_parties++;
  cond.notify_all();
}

void mpc_runner_t::wait_for_partners() {
  std::unique_lock scoped(mutex);
  while (finished_parties < n) cond.wait(scoped);
}

void mpc_runner_t::abort() {
  for (int i = 0; i < n; i++) net_contexts[i]->abort();
}

void mpc_runner_t::run_mpc_role(lambda_role_t lambda) {
  finished_parties = 0;
  reset_net_contexts();
  protocol_f = lambda;

  start_partners();
  wait_for_partners();
  stop_partners();
}

void mpc_runner_t::run_2pc(lambda_2p_t f) {
  set_new_network_2p();
  run_mpc_role([&](party_idx_t party_index) { f(*job_2ps[party_index]); });
}

void mpc_runner_t::run_mpc(lambda_mp_t f) {
  set_new_network_mp();
  run_mpc_role([&](party_idx_t party_index) { f(*job_mps[party_index]); });
}

void mpc_runner_t::run_2pc_parallel_helper(std::shared_ptr<network_t> network, party_t role, int th_i,
                                           lambda_2p_parallel_t f) {
  jsid_t jsid = th_i;
  job_session_2p_t job(role, test_pids[0], test_pids[1], network, jsid);
  f(job, th_i);
}

void mpc_runner_t::run_2pc_parallel(int n_threads, lambda_2p_parallel_t f) {
  run_mpc_role([&](party_idx_t party_index) {
    std::shared_ptr<network_t> network = std::make_shared<network_t>(get_data_transport_ptr(party_index), n_threads);

    std::vector<std::thread> threads;
    for (int th_i = 0; th_i < n_threads; th_i++) {
      threads.emplace_back(run_2pc_parallel_helper, network, party_t(party_index), th_i, f);
    }
    for (auto& th : threads) th.join();
  });
}

void mpc_runner_t::run_mpc_parallel_helper(int n, std::shared_ptr<network_t> network, party_idx_t party_index, int th_i,
                                           lambda_mp_parallel_t f) {
  jsid_t jsid = th_i;
  std::vector<crypto::bn_t> pids(test_pids.begin(), test_pids.begin() + n);
  job_session_mp_t job(party_index, pids, network, jsid);
  f(job, th_i);
}

void mpc_runner_t::run_mpc_parallel(int n_threads, lambda_mp_parallel_t f) {
  run_mpc_role([&](party_idx_t party_index) {
    std::shared_ptr<network_t> network = std::make_shared<network_t>(get_data_transport_ptr(party_index), n_threads);

    std::vector<std::thread> threads;
    for (int th_i = 0; th_i < n_threads; th_i++) {
      threads.emplace_back(run_mpc_parallel_helper, n, network, party_index, th_i, f);
    }
    for (auto& th : threads) th.join();
  });
}

std::shared_ptr<local_data_transport_t> mpc_runner_t::get_data_transport_ptr(party_idx_t role) {
  return data_transports[role];
}

const std::vector<crypto::bn_t> mpc_runner_t::test_pids = {
    crypto::pid_from_name("test party 1"),  crypto::pid_from_name("test party 2"),
    crypto::pid_from_name("test party 3"),  crypto::pid_from_name("test party 4"),
    crypto::pid_from_name("test party 5"),  crypto::pid_from_name("test party 6"),
    crypto::pid_from_name("test party 7"),  crypto::pid_from_name("test party 8"),
    crypto::pid_from_name("test party 9"),  crypto::pid_from_name("test party 10"),
    crypto::pid_from_name("test party 11"), crypto::pid_from_name("test party 12"),
    crypto::pid_from_name("test party 13"), crypto::pid_from_name("test party 14"),
    crypto::pid_from_name("test party 15"), crypto::pid_from_name("test party 16"),
    crypto::pid_from_name("test party 17"), crypto::pid_from_name("test party 18"),
    crypto::pid_from_name("test party 19"), crypto::pid_from_name("test party 20"),
    crypto::pid_from_name("test party 21"), crypto::pid_from_name("test party 22"),
    crypto::pid_from_name("test party 23"), crypto::pid_from_name("test party 24"),
    crypto::pid_from_name("test party 25"), crypto::pid_from_name("test party 26"),
    crypto::pid_from_name("test party 27"), crypto::pid_from_name("test party 28"),
    crypto::pid_from_name("test party 29"), crypto::pid_from_name("test party 30"),
    crypto::pid_from_name("test party 31"), crypto::pid_from_name("test party 32"),
    crypto::pid_from_name("test party 33"), crypto::pid_from_name("test party 34"),
    crypto::pid_from_name("test party 35"), crypto::pid_from_name("test party 36"),
    crypto::pid_from_name("test party 37"), crypto::pid_from_name("test party 38"),
    crypto::pid_from_name("test party 39"), crypto::pid_from_name("test party 40"),
    crypto::pid_from_name("test party 41"), crypto::pid_from_name("test party 42"),
    crypto::pid_from_name("test party 43"), crypto::pid_from_name("test party 44"),
    crypto::pid_from_name("test party 45"), crypto::pid_from_name("test party 46"),
    crypto::pid_from_name("test party 47"), crypto::pid_from_name("test party 48"),
    crypto::pid_from_name("test party 49"), crypto::pid_from_name("test party 50"),
    crypto::pid_from_name("test party 51"), crypto::pid_from_name("test party 52"),
    crypto::pid_from_name("test party 53"), crypto::pid_from_name("test party 54"),
    crypto::pid_from_name("test party 55"), crypto::pid_from_name("test party 56"),
    crypto::pid_from_name("test party 57"), crypto::pid_from_name("test party 58"),
    crypto::pid_from_name("test party 59"), crypto::pid_from_name("test party 60"),
    crypto::pid_from_name("test party 61"), crypto::pid_from_name("test party 62"),
    crypto::pid_from_name("test party 63"), crypto::pid_from_name("test party 64")};
}  // namespace coinbase::testutils