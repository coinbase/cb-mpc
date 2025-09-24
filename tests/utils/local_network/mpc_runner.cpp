#include "mpc_runner.h"

#include <cbmpc/crypto/base_pki.h>

using namespace coinbase::mpc;

namespace coinbase::testutils {

error_t local_data_transport_t::send(const party_idx_t receiver, mem_t msg) {
  net_context_ptr->send(receiver, msg);
  return SUCCESS;
}

error_t local_data_transport_t::receive(const party_idx_t sender, buf_t& msg) {
  return net_context_ptr->receive(sender, msg);
}

error_t local_data_transport_t::receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& msgs) {
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
    job_2ps[0] = std::make_shared<job_2p_t>(party_t::p1, test_pnames[0], test_pnames[1], get_data_transport_ptr(0));
    job_2ps[1] = std::make_shared<job_2p_t>(party_t::p2, test_pnames[0], test_pnames[1], get_data_transport_ptr(1));
  } else {
    if (n == -2) n = 2;
    init_network(n);
    std::vector<crypto::pname_t> pnames(test_pnames.begin(), test_pnames.begin() + n);
    for (int i = 0; i < n; i++) {
      job_mps[i] = std::make_shared<job_mp_t>(party_idx_t(i), pnames, get_data_transport_ptr(i));
    }
  }
}

void mpc_runner_t::set_new_network_2p() {
  job_2ps[0]->set_transport(party_idx_t(party_t::p1), get_data_transport_ptr(0));
  job_2ps[1]->set_transport(party_idx_t(party_t::p2), get_data_transport_ptr(1));
}

void mpc_runner_t::set_new_network_mp() {
  for (int i = 0; i < n; i++) {
    job_mps[i]->set_transport(party_idx_t(i), get_data_transport_ptr(i));
  }
}

mpc_runner_t::mpc_runner_t(std::shared_ptr<job_2p_t> job1, std::shared_ptr<job_2p_t> job2) : n(2) {
  init_network(n);
  job_2ps[0] = job1;
  job_2ps[1] = job2;
  set_new_network_2p();
}

mpc_runner_t::mpc_runner_t(std::vector<std::shared_ptr<job_mp_t>> jobs) : n(jobs.size()) {
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

void mpc_runner_t::run_2pc_parallel_helper(std::shared_ptr<parallel_data_transport_t> network, party_t role, int th_i,
                                           lambda_2p_parallel_t f) {
  parallel_id_t parallel_id = th_i;
  job_parallel_2p_t job(role, test_pnames[0], test_pnames[1], network, parallel_id);
  f(job, th_i);
}

void mpc_runner_t::run_2pc_parallel(int n_threads, lambda_2p_parallel_t f) {
  run_mpc_role([&](party_idx_t party_index) {
    std::shared_ptr<parallel_data_transport_t> network =
        std::make_shared<parallel_data_transport_t>(get_data_transport_ptr(party_index), n_threads);

    std::vector<std::thread> threads;
    for (int th_i = 0; th_i < n_threads; th_i++) {
      threads.emplace_back(run_2pc_parallel_helper, network, party_t(party_index), th_i, f);
    }
    for (auto& th : threads) th.join();
  });
}

void mpc_runner_t::run_mpc_parallel_helper(int n, std::shared_ptr<parallel_data_transport_t> network,
                                           party_idx_t party_index, int th_i, lambda_mp_parallel_t f) {
  parallel_id_t parallel_id = th_i;
  std::vector<crypto::pname_t> pnames(test_pnames.begin(), test_pnames.begin() + n);
  job_parallel_mp_t job(party_index, pnames, network, parallel_id);
  f(job, th_i);
}

void mpc_runner_t::run_mpc_parallel(int n_threads, lambda_mp_parallel_t f) {
  run_mpc_role([&](party_idx_t party_index) {
    std::shared_ptr<parallel_data_transport_t> network =
        std::make_shared<parallel_data_transport_t>(get_data_transport_ptr(party_index), n_threads);

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

const std::vector<crypto::pname_t> mpc_runner_t::test_pnames = {
    "test party 1",  "test party 2",  "test party 3",  "test party 4",  "test party 5",  "test party 6",
    "test party 7",  "test party 8",  "test party 9",  "test party 10", "test party 11", "test party 12",
    "test party 13", "test party 14", "test party 15", "test party 16", "test party 17", "test party 18",
    "test party 19", "test party 20", "test party 21", "test party 22", "test party 23", "test party 24",
    "test party 25", "test party 26", "test party 27", "test party 28", "test party 29", "test party 30",
    "test party 31", "test party 32", "test party 33", "test party 34", "test party 35", "test party 36",
    "test party 37", "test party 38", "test party 39", "test party 40", "test party 41", "test party 42",
    "test party 43", "test party 44", "test party 45", "test party 46", "test party 47", "test party 48",
    "test party 49", "test party 50", "test party 51", "test party 52", "test party 53", "test party 54",
    "test party 55", "test party 56", "test party 57", "test party 58", "test party 59", "test party 60",
    "test party 61", "test party 62", "test party 63", "test party 64"};

mpc_runner_t::mpc_runner_t(const std::vector<crypto::pname_t>& pnames) : n(static_cast<int>(pnames.size())) {
  init_network(n);
  if (n == 2) {
    job_2ps[0] = std::make_shared<job_2p_t>(party_t::p1, pnames[0], pnames[1], get_data_transport_ptr(0));
    job_2ps[1] = std::make_shared<job_2p_t>(party_t::p2, pnames[0], pnames[1], get_data_transport_ptr(1));
  } else {
    for (int i = 0; i < n; i++) {
      job_mps[i] = std::make_shared<job_mp_t>(party_idx_t(i), pnames, get_data_transport_ptr(i));
    }
  }
}
}  // namespace coinbase::testutils