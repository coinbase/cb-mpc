#include "network.h"

#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_pki.h>
#include <cbmpc/crypto/lagrange.h>
#include <cbmpc/protocol/ecdsa_2p.h>
#include <cbmpc/protocol/mpc_job_session.h>

using namespace coinbase;
using namespace coinbase::mpc;

namespace {
constexpr int SUCCESS_CODE = 0;
constexpr int ERROR_CODE = -1;
constexpr int PARAM_ERROR_CODE = -2;

// Helper function to validate party names
bool validate_party_names(const char* const* pnames, int count) noexcept {
  if (!pnames) return false;
  for (int i = 0; i < count; ++i) {
    if (!pnames[i] || std::string_view(pnames[i]).empty()) {
      return false;
    }
  }
  return true;
}

// RAII wrapper for job references
template <typename JobType>
struct JobDeleter {
  void operator()(JobType* job) const noexcept {
    if constexpr (std::is_same_v<JobType, job_2p_ref>) {
      free_job_2p(job);
    } else {
      free_job_mp(job);
    }
  }
};

template <typename JobType>
using unique_job_ptr = std::unique_ptr<JobType, JobDeleter<JobType>>;
}  // namespace

void free_job_2p(job_2p_ref* ptr) {
  if (!ptr) return;

  if (ptr->opaque) {
    try {
      delete static_cast<job_2p_t*>(ptr->opaque);
    } catch (const std::exception& e) {
      std::cerr << "Error freeing job_2p: " << e.what() << std::endl;
    }
    ptr->opaque = nullptr;
  }
  delete ptr;
}

void free_job_mp(job_mp_ref* ptr) {
  if (!ptr) return;

  if (ptr->opaque) {
    try {
      delete static_cast<job_mp_t*>(ptr->opaque);
    } catch (const std::exception& e) {
      std::cerr << "Error freeing job_mp: " << e.what() << std::endl;
    }
    ptr->opaque = nullptr;
  }
  delete ptr;
}

class callback_data_transport_t : public data_transport_interface_t {
 private:
  const data_transport_callbacks_t callbacks;
  void* const go_impl_ptr;

 public:
  callback_data_transport_t(const data_transport_callbacks_t* callbacks_ptr, void* go_impl_ptr)
      : callbacks(*callbacks_ptr), go_impl_ptr(go_impl_ptr) {
    if (!callbacks_ptr) {
      throw std::invalid_argument("callbacks_ptr cannot be null");
    }
    if (!go_impl_ptr) {
      throw std::invalid_argument("go_impl_ptr cannot be null");
    }
    if (!callbacks.send_fun || !callbacks.receive_fun || !callbacks.receive_all_fun) {
      throw std::invalid_argument("all callback functions must be provided");
    }
  }

  error_t send(const party_idx_t receiver, mem_t msg) override {
    cmem_t cmsg = static_cast<cmem_t>(msg);
    int result = callbacks.send_fun(go_impl_ptr, receiver, cmsg);
    return error_t(result);
  }

  error_t receive(const party_idx_t sender, buf_t& msg) override {
    cmem_t cmsg{nullptr, 0};
    error_t rv = UNINITIALIZED_ERROR;
    if (rv = error_t(callbacks.receive_fun(go_impl_ptr, sender, &cmsg))) return rv;
    msg = buf_t::from_cmem(cmsg);
    return SUCCESS;
  }

  error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& msgs) override {
    const auto n = static_cast<int>(senders.size());
    if (n == 0) {
      msgs.clear();
      return SUCCESS;
    }

    // Use stack allocation for small arrays, heap for larger ones
    constexpr int STACK_THRESHOLD = 64;
    std::vector<int> c_senders;
    c_senders.reserve(n);

    for (const auto sender : senders) {
      c_senders.push_back(sender);
    }

    cmems_t cmsgs;
    int result = callbacks.receive_all_fun(go_impl_ptr, const_cast<int*>(c_senders.data()), n, &cmsgs);
    msgs = mems_t::from_cmems(cmsgs).bufs();

    return SUCCESS;
  }
};

job_2p_ref* new_job_2p(const data_transport_callbacks_t* callbacks, void* go_impl_ptr, int index,
                       const char* const* pnames, int pname_count) {
  // Input validation with specific error codes
  if (pname_count != 2) {
    std::cerr << "Error: expected exactly 2 pnames, got " << pname_count << std::endl;
    return nullptr;
  }

  if (!callbacks || !go_impl_ptr) {
    std::cerr << "Error: null parameters passed to new_job_2p" << std::endl;
    return nullptr;
  }

  if (!validate_party_names(pnames, pname_count)) {
    std::cerr << "Error: invalid party names" << std::endl;
    return nullptr;
  }

  try {
    auto data_transport_ptr = std::make_shared<callback_data_transport_t>(callbacks, go_impl_ptr);
    auto job_impl =
        std::make_unique<job_2p_t>(party_t(index), std::string(pnames[0]), std::string(pnames[1]), data_transport_ptr);

    auto result = std::make_unique<job_2p_ref>();
    result->opaque = job_impl.release();
    return result.release();

  } catch (const std::exception& e) {
    std::cerr << "Error creating job_2p: " << e.what() << std::endl;
    return nullptr;
  }
}

#define VALIDATE_JOB_2P(job)        \
  do {                              \
    if (!job || !job->opaque) {     \
      return NETWORK_INVALID_STATE; \
    }                               \
  } while (0)

#define GET_JOB_2P(job) static_cast<job_2p_t*>(job->opaque)

int is_peer1(const job_2p_ref* job) {
  if (!job || !job->opaque) return 0;
  return static_cast<const job_2p_t*>(job->opaque)->is_p1() ? 1 : 0;
}

int is_peer2(const job_2p_ref* job) {
  if (!job || !job->opaque) return 0;
  return static_cast<const job_2p_t*>(job->opaque)->is_p2() ? 1 : 0;
}

int is_role_index(const job_2p_ref* job, int party_index) {
  if (!job || !job->opaque) return 0;
  return static_cast<const job_2p_t*>(job->opaque)->is_party_idx(party_index) ? 1 : 0;
}

int get_role_index(const job_2p_ref* job) {
  if (!job || !job->opaque) return -1;
  return static_cast<int>(static_cast<const job_2p_t*>(job->opaque)->get_party_idx());
}

int mpc_2p_send(job_2p_ref* job, int receiver, cmem_t msg) {
  if (!job || !job->opaque) return NETWORK_INVALID_STATE;
  if (!msg.data && msg.size > 0) return NETWORK_PARAM_ERROR;
  if (msg.size < 0) return NETWORK_PARAM_ERROR;

  try {
    job_2p_t* j = GET_JOB_2P(job);
    buf_t msg_buf{mem_t(msg)};
    error_t result = j->send(party_idx_t(receiver), msg_buf);
    return static_cast<int>(result);
  } catch (const std::exception& e) {
    std::cerr << "Error in mpc_2p_send: " << e.what() << std::endl;
    return NETWORK_ERROR;
  }
}

int mpc_2p_receive(job_2p_ref* job, int sender, cmem_t* msg) {
  if (!job || !job->opaque || !msg) return NETWORK_PARAM_ERROR;

  try {
    job_2p_t* j = GET_JOB_2P(job);
    buf_t msg_buf;
    error_t err = j->receive(party_idx_t(sender), msg_buf);

    if (err) return static_cast<int>(err);

    msg->size = static_cast<int>(msg_buf.size());
    if (msg->size > 0) {
      msg->data = static_cast<uint8_t*>(malloc(msg->size));
      if (!msg->data) return NETWORK_MEMORY_ERROR;
      memcpy(msg->data, msg_buf.data(), msg->size);
    } else {
      msg->data = nullptr;
    }

    return NETWORK_SUCCESS;
  } catch (const std::exception& e) {
    std::cerr << "Error in mpc_2p_receive: " << e.what() << std::endl;
    return NETWORK_ERROR;
  }
}

job_mp_ref* new_job_mp(const data_transport_callbacks_t* callbacks, void* go_impl_ptr, int party_count, int index,
                       const char* const* pnames, int pname_count) {
  // Input validation
  if (pname_count != party_count) {
    std::cerr << "Error: pname_count (" << pname_count << ") does not match party_count (" << party_count << ")"
              << std::endl;
    return nullptr;
  }

  if (party_count <= 0) {
    std::cerr << "Error: party_count must be positive, got " << party_count << std::endl;
    return nullptr;
  }

  if (!callbacks || !go_impl_ptr) {
    std::cerr << "Error: null parameters passed to new_job_mp" << std::endl;
    return nullptr;
  }

  if (!validate_party_names(pnames, pname_count)) {
    std::cerr << "Error: invalid party names" << std::endl;
    return nullptr;
  }

  try {
    auto data_transport_ptr = std::make_shared<callback_data_transport_t>(callbacks, go_impl_ptr);

    std::vector<crypto::pname_t> pnames_vec;
    pnames_vec.reserve(party_count);
    for (int i = 0; i < party_count; ++i) {
      pnames_vec.emplace_back(pnames[i]);
    }

    auto job_impl = std::make_unique<job_mp_t>(party_idx_t(index), std::move(pnames_vec), data_transport_ptr);

    auto result = std::make_unique<job_mp_ref>();
    result->opaque = job_impl.release();
    return result.release();

  } catch (const std::exception& e) {
    std::cerr << "Error creating job_mp: " << e.what() << std::endl;
    return nullptr;
  }
}

#define VALIDATE_JOB_MP(job)        \
  do {                              \
    if (!job || !job->opaque) {     \
      return NETWORK_INVALID_STATE; \
    }                               \
  } while (0)

#define GET_JOB_MP(job) static_cast<job_mp_t*>(job->opaque)

int is_party(const job_mp_ref* job, int party_index) {
  if (!job || !job->opaque) return 0;
  return static_cast<const job_mp_t*>(job->opaque)->is_party_idx(party_index) ? 1 : 0;
}

int get_party_idx(const job_mp_ref* job) {
  if (!job || !job->opaque) return -1;
  return static_cast<int>(static_cast<const job_mp_t*>(job->opaque)->get_party_idx());
}

int get_n_parties(const job_mp_ref* job) {
  if (!job || !job->opaque) return -1;
  return static_cast<int>(static_cast<const job_mp_t*>(job->opaque)->get_n_parties());
}

mpc_party_set_ref new_party_set() {
  party_set_t* set = new party_set_t();
  return mpc_party_set_ref{set};
}

void party_set_add(mpc_party_set_ref* set, int party_idx) {
  party_set_t* party_set = static_cast<party_set_t*>(set->opaque);
  party_set->add(party_idx);
}

void free_party_set(mpc_party_set_ref ctx) {
  if (ctx.opaque) {
    delete static_cast<party_set_t*>(ctx.opaque);
  }
}
