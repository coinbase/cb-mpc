#include "network_context.h"

using namespace coinbase::mpc;

namespace coinbase::testutils {

void mpc_net_context_t::send(party_idx_t receiver, mem_t msg) {
  auto rec = out[receiver];
  test_channel_t& rec_in = rec->in[index];

  rec_in.send(rec->channel_sync, msg);
}

error_t mpc_net_context_t::receive(party_idx_t sender, buf_t& result) {
  return in[sender].receive(channel_sync, is_abort, result);
}

error_t mpc_net_context_t::receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& result) {
  int n = (int)senders.size();
  // Instead of using out, clear and resize `result` and use it directly.
  result.clear();
  result.resize(n);
  int received = 0;

  std::unique_lock lock(channel_sync.mutex);
  while (received < n || is_abort) {
    if (is_abort) return E_GENERAL;
    int old_received = received;
    for (int i = 0; i < n; i++) {
      if (result[i].size() > 0) continue;
      party_idx_t sender = senders[i];
      cb_assert(sender != index);
      test_channel_t& channel = in[sender];
      if (channel.queue_is_empty()) {
        continue;
      }
      result[i] = channel.receive();
      cb_assert(result[i].size() > 0);
      received++;
    }
    if (received == old_received && !is_abort) channel_sync.cond.wait(lock);
  }

  return SUCCESS;
}

void mpc_net_context_t::abort() {
  std::unique_lock lock(channel_sync.mutex);
  is_abort = true;
  channel_sync.cond.notify_all();
}

void mpc_net_context_t::reset() {
  is_abort = false;
  for (int i = 0; i < in.size(); i++) in[i].reset();
}

}  // namespace coinbase::testutils