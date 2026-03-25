#pragma once

#include <atomic>
#include <climits>
#include <cstring>
#include <memory>
#include <thread>
#include <vector>

#include <cbmpc/c_api/job.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

#include "utils/local_network/network_context.h"

namespace coinbase::testutils::capi_harness {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;
using coinbase::api::party_idx_t;
using coinbase::testutils::mpc_net_context_t;

struct transport_ctx_t {
  std::shared_ptr<mpc_net_context_t> net;
  std::atomic<int>* free_calls = nullptr;
};

inline cbmpc_error_t transport_send(void* ctx, int32_t receiver, const uint8_t* data, int size) {
  if (!ctx) return E_BADARG;
  if (size < 0) return E_BADARG;
  if (size > 0 && !data) return E_BADARG;
  auto* c = static_cast<transport_ctx_t*>(ctx);
  c->net->send(static_cast<party_idx_t>(receiver), mem_t(data, size));
  return CBMPC_SUCCESS;
}

inline cbmpc_error_t transport_receive(void* ctx, int32_t sender, cmem_t* out_msg) {
  if (!out_msg) return E_BADARG;
  *out_msg = cmem_t{nullptr, 0};
  if (!ctx) return E_BADARG;

  auto* c = static_cast<transport_ctx_t*>(ctx);
  buf_t msg;
  const error_t rv = c->net->receive(static_cast<party_idx_t>(sender), msg);
  if (rv) return rv;

  const int n = msg.size();
  if (n < 0) return E_FORMAT;
  if (n == 0) return CBMPC_SUCCESS;

  out_msg->data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(n)));
  if (!out_msg->data) return E_INSUFFICIENT;
  out_msg->size = n;
  std::memmove(out_msg->data, msg.data(), static_cast<size_t>(n));
  return CBMPC_SUCCESS;
}

inline cbmpc_error_t transport_receive_all(void* ctx, const int32_t* senders, int senders_count, cmems_t* out_msgs) {
  if (!out_msgs) return E_BADARG;
  *out_msgs = cmems_t{0, nullptr, nullptr};
  if (!ctx) return E_BADARG;
  if (senders_count < 0) return E_BADARG;
  if (senders_count > 0 && !senders) return E_BADARG;

  auto* c = static_cast<transport_ctx_t*>(ctx);
  std::vector<party_idx_t> s;
  s.reserve(static_cast<size_t>(senders_count));
  for (int i = 0; i < senders_count; i++) s.push_back(static_cast<party_idx_t>(senders[i]));

  std::vector<buf_t> msgs;
  const error_t rv = c->net->receive_all(s, msgs);
  if (rv) return rv;
  if (msgs.size() != static_cast<size_t>(senders_count)) return E_GENERAL;

  // Flatten into (data + sizes) buffers.
  int total = 0;
  for (const auto& m : msgs) {
    const int sz = m.size();
    if (sz < 0) return E_FORMAT;
    if (sz > INT_MAX - total) return E_RANGE;
    total += sz;
  }

  out_msgs->count = senders_count;
  out_msgs->sizes = static_cast<int*>(cbmpc_malloc(sizeof(int) * static_cast<size_t>(senders_count)));
  if (!out_msgs->sizes) {
    *out_msgs = cmems_t{0, nullptr, nullptr};
    return E_INSUFFICIENT;
  }

  if (total > 0) {
    out_msgs->data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(total)));
    if (!out_msgs->data) {
      cbmpc_free(out_msgs->sizes);
      *out_msgs = cmems_t{0, nullptr, nullptr};
      return E_INSUFFICIENT;
    }
  }

  int offset = 0;
  for (int i = 0; i < senders_count; i++) {
    const int sz = msgs[i].size();
    out_msgs->sizes[i] = sz;
    if (sz) {
      std::memmove(out_msgs->data + offset, msgs[i].data(), static_cast<size_t>(sz));
      offset += sz;
    }
  }

  return CBMPC_SUCCESS;
}

inline void transport_free(void* ctx, void* ptr) {
  if (!ptr) return;
  auto* c = static_cast<transport_ctx_t*>(ctx);
  if (c && c->free_calls) c->free_calls->fetch_add(1);
  cbmpc_free(ptr);
}

inline cbmpc_transport_t make_transport(transport_ctx_t* ctx) {
  return cbmpc_transport_t{
      /*ctx=*/ctx,
      /*send=*/transport_send,
      /*receive=*/transport_receive,
      /*receive_all=*/transport_receive_all,
      /*free=*/transport_free,
  };
}

template <typename F1, typename F2>
inline void run_2pc(const std::shared_ptr<mpc_net_context_t>& c1, const std::shared_ptr<mpc_net_context_t>& c2, F1&& f1,
                    F2&& f2, cbmpc_error_t& out_rv1, cbmpc_error_t& out_rv2) {
  c1->reset();
  c2->reset();

  std::atomic<bool> aborted{false};

  std::thread t1([&] {
    out_rv1 = f1();
    if (out_rv1 && !aborted.exchange(true)) {
      c1->abort();
      c2->abort();
    }
  });
  std::thread t2([&] {
    out_rv2 = f2();
    if (out_rv2 && !aborted.exchange(true)) {
      c1->abort();
      c2->abort();
    }
  });

  t1.join();
  t2.join();
}

template <typename F>
inline void run_mp(const std::vector<std::shared_ptr<mpc_net_context_t>>& peers, F&& f,
                   std::vector<cbmpc_error_t>& out_rv) {
  for (const auto& p : peers) p->reset();

  out_rv.assign(peers.size(), UNINITIALIZED_ERROR);
  std::atomic<bool> aborted{false};
  std::vector<std::thread> threads;
  threads.reserve(peers.size());

  for (size_t i = 0; i < peers.size(); i++) {
    threads.emplace_back([&, i] {
      out_rv[i] = f(static_cast<int>(i));
      if (out_rv[i] && !aborted.exchange(true)) {
        for (const auto& p : peers) p->abort();
      }
    });
  }
  for (auto& t : threads) t.join();
}

}  // namespace coinbase::testutils::capi_harness
