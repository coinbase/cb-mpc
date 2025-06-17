#include "network.h"

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_pki.h>
#include <cbmpc/crypto/lagrange.h>
#include <cbmpc/protocol/agree_random.h>
#include <cbmpc/protocol/ecdsa_2p.h>
#include <cbmpc/protocol/mpc_job_session.h>

using namespace coinbase;
using namespace coinbase::mpc;

// ---------------- Generic Network Interface ------------
class callback_data_transport_t : public data_transport_interface_t {
  data_transport_callbacks_t callbacks;
  void* go_impl_ptr;

 public:
  callback_data_transport_t(data_transport_callbacks_t* callbacks_ptr, void* go_impl_ptr)
      : callbacks(*callbacks_ptr), go_impl_ptr(go_impl_ptr) {}

  error_t send(const party_idx_t receiver, const mem_t& msg) {
    mem_t msg_copy = msg;
    return callbacks.send_fun(go_impl_ptr, receiver, msg_copy.data, msg_copy.size);
  }
  error_t receive(const party_idx_t sender, mem_t& msg) {
    return callbacks.receive_fun(go_impl_ptr, sender, &msg.data, &msg.size);
  }
  error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<mem_t>& msgs) {
    int n = senders.size();
    std::vector<int> c_senders(n);
    for (int i = 0; i < n; i++) c_senders[i] = senders[i];
    std::vector<byte_ptr> c_messages(n);
    std::vector<int> c_sizes(n);
    int c_error = callbacks.receive_all_fun(go_impl_ptr, &c_senders[0], senders.size(), &c_messages[0], &c_sizes[0]);
    if (c_error) return error_t(c_error);

    msgs.resize(n);
    for (int i = 0; i < n; i++) msgs[i] = mem_t(c_messages[i], c_sizes[i]);
    return SUCCESS;
  }
};

// ---------------- JOB_SESSION_2P_PTR ------------
JOB_SESSION_2P_PTR* new_job_session_2p(data_transport_callbacks_t* callbacks, void* go_impl_ptr, int index, char** pnames, int pname_count) {
  if (pname_count != 2) {
    std::cerr << "Error: expected exactly 2 pnames, got " << pname_count << std::endl;
    return nullptr;
  }
  
  std::shared_ptr<callback_data_transport_t> data_transport_ptr =
      std::make_shared<callback_data_transport_t>(callbacks, go_impl_ptr);
  std::shared_ptr<network_t> network = std::make_shared<network_t>(data_transport_ptr);
  return new JOB_SESSION_2P_PTR{new job_session_2p_t(party_t(index), std::string(pnames[0]), std::string(pnames[1]), network)};
}

int is_peer1(JOB_SESSION_2P_PTR* job) {
  job_session_2p_t* j = static_cast<job_session_2p_t*>(job->opaque);
  return j->is_p1();
}

int is_peer2(JOB_SESSION_2P_PTR* job) {
  job_session_2p_t* j = static_cast<job_session_2p_t*>(job->opaque);
  return j->is_p2();
}

int is_role_index(JOB_SESSION_2P_PTR* job, int party_index) {
  job_session_2p_t* j = static_cast<job_session_2p_t*>(job->opaque);
  return j->is_party_idx(party_index);
}

int get_role_index(JOB_SESSION_2P_PTR* job) {
  job_session_2p_t* j = static_cast<job_session_2p_t*>(job->opaque);
  return int(j->get_party_idx());
}

int mpc_2p_send(JOB_SESSION_2P_PTR* job, int receiver, const uint8_t* msg, const int msg_len) {
  job_session_2p_t* j = static_cast<job_session_2p_t*>(job->opaque);
  buf_t msg_buf(msg, msg_len);
  return j->send(party_idx_t(receiver), msg_buf);
}

int mpc_2p_receive(JOB_SESSION_2P_PTR* job, int sender, uint8_t** msg, int* msg_len) {
  error_t err;
  job_session_2p_t* j = static_cast<job_session_2p_t*>(job->opaque);
  buf_t msg_buf;
  if (err = j->receive(party_idx_t(sender), msg_buf)) return int(err);

  *msg_len = msg_buf.size();
  *msg = (uint8_t*)malloc(msg_buf.size() * sizeof(uint8_t));
  memcpy(*msg, msg_buf.data(), msg_buf.size());

  return 0;
}

// ---------------- JOB_SESSION_MP_PTR ------------

JOB_SESSION_MP_PTR* new_job_session_mp(data_transport_callbacks_t* callbacks, void* go_impl_ptr, int party_count,
                                       int index, int job_session_id, char** pnames, int pname_count) {
  if (pname_count != party_count) {
    std::cerr << "Error: pname_count does not match party_count" << std::endl;
    return nullptr;
  }
  std::shared_ptr<callback_data_transport_t> data_transport_ptr =
      std::make_shared<callback_data_transport_t>(callbacks, go_impl_ptr);
  std::shared_ptr<network_t> network = std::make_shared<network_t>(data_transport_ptr);
  std::vector<crypto::pname_t> pnames_vec(party_count);
  for (int i = 0; i < party_count; i++) {
    pnames_vec[i] = std::string(pnames[i]);
  }
  return new JOB_SESSION_MP_PTR{new job_session_mp_t(party_idx_t(index), pnames_vec, network, jsid_t(job_session_id))};
}


int is_party(JOB_SESSION_MP_PTR* job, int party_index) {
  job_session_mp_t* j = static_cast<job_session_mp_t*>(job->opaque);
  return j->is_party_idx(party_index);
}

int get_party_idx(JOB_SESSION_MP_PTR* job) {
  job_session_mp_t* j = static_cast<job_session_mp_t*>(job->opaque);
  return int(j->get_party_idx());
}

// ---------------- Agree Randoms ------------

int mpc_agree_random(JOB_SESSION_2P_PTR* job, int bit_len, cmem_t* out) {
  error_t err = 0;
  job_session_2p_t* j = static_cast<job_session_2p_t*>(job->opaque);

  buf_t out_buf;
  err = agree_random(*j, bit_len, out_buf);

  *out = out_buf.to_cmem();

  return err;
}
