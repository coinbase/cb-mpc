#pragma once

#include "network_context.h"
#include "partner.h"

namespace coinbase::testutils {

class partner_t;

typedef std::function<void(mpc::party_idx_t role)> lambda_role_t;
typedef std::function<void(mpc::job_2p_t& job)> lambda_2p_t;
typedef std::function<void(mpc::job_mp_t& job)> lambda_mp_t;
typedef std::function<void(mpc::job_parallel_2p_t& job, int th_i)> lambda_2p_parallel_t;
typedef std::function<void(mpc::job_parallel_mp_t& job, int th_i)> lambda_mp_parallel_t;

class local_data_transport_t : public mpc::data_transport_interface_t {
 public:
  local_data_transport_t(const std::shared_ptr<mpc_net_context_t>& nc_ptr) : net_context_ptr(nc_ptr) {}
  error_t send(const mpc::party_idx_t receiver, mem_t msg) override;
  error_t receive(const mpc::party_idx_t sender, buf_t& msg) override;
  error_t receive_all(const std::vector<mpc::party_idx_t>& senders, std::vector<buf_t>& msgs) override;

  std::shared_ptr<mpc_net_context_t> net_context_ptr;
};

class mpc_runner_t {
 public:
  mpc_runner_t(int n_parties);
  mpc_runner_t(std::shared_ptr<mpc::job_2p_t> job1, std::shared_ptr<mpc::job_2p_t> job2);
  mpc_runner_t(std::vector<std::shared_ptr<mpc::job_mp_t>> jobs);
  mpc_runner_t(const std::vector<crypto::pname_t>& pnames);

  void start_partners();
  void stop_partners();
  void abort_connection();
  void reset_net_contexts();
  void run_on_partner(mpc::party_idx_t role);
  void wait_for_partners();
  void abort();

  std::shared_ptr<local_data_transport_t> get_data_transport_ptr(mpc::party_idx_t role);

  void run_2pc(lambda_2p_t f);
  void run_mpc(lambda_mp_t f);
  void run_2pc_parallel(int n_threads, lambda_2p_parallel_t f);
  void run_mpc_parallel(int n_threads, lambda_mp_parallel_t f);

  // In-class declaration (no initializer):
  static const std::vector<crypto::pname_t> test_pnames;

 private:
  lambda_role_t protocol_f;
  std::mutex mutex;
  std::condition_variable cond;
  int finished_parties = 0;
  int n;
  std::array<std::shared_ptr<mpc::job_2p_t>, 2> job_2ps;
  std::array<std::shared_ptr<mpc::job_mp_t>, 64> job_mps;
  std::vector<std::shared_ptr<partner_t>> partners;
  std::vector<std::shared_ptr<local_data_transport_t>> data_transports;
  std::vector<std::shared_ptr<mpc_net_context_t>> net_contexts;

  void init_network(int n_parties);
  void set_new_network_2p();
  void set_new_network_mp();

  void run_mpc_role(lambda_role_t f);
  static void run_2pc_parallel_helper(std::shared_ptr<mpc::parallel_data_transport_t> network, mpc::party_t role,
                                      int th_i, lambda_2p_parallel_t f);
  static void run_mpc_parallel_helper(int n, std::shared_ptr<mpc::parallel_data_transport_t> network,
                                      mpc::party_idx_t party_index, int th_i, lambda_mp_parallel_t f);
};  // namespace coinbase::testutils

}  // namespace coinbase::testutils