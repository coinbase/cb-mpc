#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/mpc_job_session.h>

#include "channel.h"

namespace coinbase::testutils {
typedef struct mpc_net_context_t* mpc_net_context_ptr_t;

struct mpc_net_context_t {
 public:
  mpc_net_context_t(int i) : index(i) {}

  void init_with_peers(const std::vector<std::shared_ptr<mpc_net_context_t>>& net_contexts) {
    out = net_contexts;
    in.resize(net_contexts.size());
  }

  void send(mpc::party_idx_t receiver_role, mem_t msg);
  error_t receive(mpc::party_idx_t sender_role, buf_t& result);
  error_t receive_all(const std::vector<mpc::party_idx_t>& senders, std::vector<buf_t>& result);

  void abort();
  void reset();

 private:
  const int index;
  bool is_abort = false;
  test_channel_sync_t channel_sync;
  std::vector<test_channel_t> in;
  std::vector<std::shared_ptr<mpc_net_context_t>> out;
};

}  // namespace coinbase::testutils