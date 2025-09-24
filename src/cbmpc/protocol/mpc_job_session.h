#pragma once

#include <cbmpc/protocol/mpc_job.h>

#include "data_transport.h"

namespace coinbase::mpc {

using parallel_id_t = int32_t;

class parallel_data_transport_t : public data_transport_interface_t {
 public:
  parallel_data_transport_t(std::shared_ptr<data_transport_interface_t> _data_transport_ptr, int _parallel_count = 1)
      : data_transport_ptr(_data_transport_ptr) {
    set_parallel(_parallel_count);
  }

  error_t send(const party_idx_t receiver, const parallel_id_t parallel_id, const mem_t msg);
  error_t receive(const party_idx_t sender, const parallel_id_t parallel_id, buf_t& msg);
  error_t receive_all(const std::vector<party_idx_t>& senders, parallel_id_t parallel_id, std::vector<buf_t>& msgs);

  // data_transport_interface_t overrides using jsid 0
  error_t send(const party_idx_t receiver, mem_t msg) override { return send(receiver, 0, msg); }
  error_t receive(const party_idx_t sender, buf_t& msg) override { return receive(sender, 0, msg); }
  error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& msgs) override {
    return receive_all(senders, 0, msgs);
  }

  void set_parallel(int _parallel_count);

 private:
  std::shared_ptr<data_transport_interface_t> data_transport_ptr;
  int parallel_count;

  // For parallel send
  int is_send_active = 0;
  std::mutex is_send_active_mtx;
  std::condition_variable send_active_cv;

  int send_ready = 0;
  std::mutex send_ready_mtx;
  std::condition_variable send_done_cv;
  std::condition_variable send_start_cv;

  std::vector<buf_t> send_msg;
  std::mutex send_msg_mutex;

  // For parallel receive
  int is_receive_active = 0;
  std::mutex is_receive_active_mtx;
  std::condition_variable receive_active_cv;

  int receive_ready = 0;
  std::mutex receive_ready_mtx;
  std::condition_variable receive_done_cv;
  std::condition_variable receive_start_cv;

  std::vector<buf_t> receive_msg;
  std::mutex receive_msg_mutex;

  // For parallel receive_all
  int is_receive_all_active = 0;
  std::mutex is_receive_all_mtx;
  std::condition_variable receive_all_active_cv;

  int receive_all_ready = 0;
  std::mutex receive_all_ready_mtx;
  std::condition_variable receive_all_done_cv;
  std::condition_variable receive_all_start_cv;

  std::unordered_map<party_idx_t, std::vector<buf_t>> receive_all_msgs;
  std::mutex receive_all_msgs_mutex;
};

class job_parallel_mp_t : public job_mp_t {
 protected:
  error_t send_impl(party_idx_t to, mem_t msg) override {
    auto network = std::static_pointer_cast<parallel_data_transport_t>(transport_ptr);
    if (!network) return E_NET_GENERAL;
    return network->send(to, parallel_id, msg);
  }

  error_t receive_impl(party_idx_t from, buf_t& mem) override {
    auto network = std::static_pointer_cast<parallel_data_transport_t>(transport_ptr);
    if (!network) return E_NET_GENERAL;
    return network->receive(from, parallel_id, mem);
  }

  error_t receive_many_impl(std::vector<party_idx_t> from_set, std::vector<buf_t>& outs) override {
    auto network = std::static_pointer_cast<parallel_data_transport_t>(transport_ptr);
    if (!network) return E_NET_GENERAL;
    return network->receive_all(from_set, parallel_id, outs);
  }

 public:
  job_parallel_mp_t(party_idx_t index, std::vector<crypto::pname_t> pnames,
                    std::shared_ptr<parallel_data_transport_t> _network_ptr, parallel_id_t _parallel_id)
      : job_mp_t(index, pnames, _network_ptr), parallel_id(_parallel_id) {}

  void set_network(party_idx_t party_idx, std::shared_ptr<parallel_data_transport_t> ptr) {
    set_transport(party_idx, ptr);
  }

  job_parallel_mp_t get_parallel_job(int parallel_count, parallel_id_t id) {
    return job_parallel_mp_t(party_index, names, std::static_pointer_cast<parallel_data_transport_t>(transport_ptr),
                             id);
  }

 protected:
  parallel_id_t parallel_id;
};

class job_parallel_2p_t : public job_2p_t {
 public:
  job_parallel_2p_t(party_t party, crypto::pname_t pname1, crypto::pname_t pname2,
                    std::shared_ptr<parallel_data_transport_t> ptr, parallel_id_t id = 0)
      : job_2p_t(party, pname1, pname2, ptr), parallel_id(id) {};

  void set_network(party_t party, std::shared_ptr<parallel_data_transport_t> ptr) {
    set_transport(party_idx_t(party), ptr);
  }

  job_parallel_2p_t get_parallel_job(int parallel_count, parallel_id_t id) {
    return job_parallel_2p_t(party_t(party_index), names[0], names[1],
                             std::static_pointer_cast<parallel_data_transport_t>(transport_ptr), id);
  }

  void set_parallel_count(int parallel_count) {
    auto network = std::static_pointer_cast<parallel_data_transport_t>(transport_ptr);
    if (network) network->set_parallel(parallel_count);
  }

  error_t send_impl(party_idx_t to, mem_t msg) override {
    auto network = std::static_pointer_cast<parallel_data_transport_t>(transport_ptr);
    if (!network) return E_NET_GENERAL;
    return network->send(to, parallel_id, msg);
  }
  error_t receive_impl(party_idx_t from, buf_t& mem) override {
    auto network = std::static_pointer_cast<parallel_data_transport_t>(transport_ptr);
    if (!network) return E_NET_GENERAL;
    return network->receive(from, parallel_id, mem);
  }

 protected:
  parallel_id_t parallel_id;
};

}  // namespace coinbase::mpc
