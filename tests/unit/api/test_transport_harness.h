#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>

#include "utils/local_network/network_context.h"

namespace coinbase::testutils::api_harness {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;
using coinbase::api::data_transport_i;
using coinbase::api::party_idx_t;
using coinbase::testutils::mpc_net_context_t;

class local_api_transport_t final : public data_transport_i {
 public:
  explicit local_api_transport_t(std::shared_ptr<mpc_net_context_t> ctx) : ctx_(std::move(ctx)) {}

  error_t send(party_idx_t receiver, mem_t msg) override {
    ctx_->send(receiver, msg);
    return SUCCESS;
  }

  error_t receive(party_idx_t sender, buf_t& msg) override { return ctx_->receive(sender, msg); }

  error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& msgs) override {
    std::vector<coinbase::mpc::party_idx_t> s;
    s.reserve(senders.size());
    for (auto x : senders) s.push_back(static_cast<coinbase::mpc::party_idx_t>(x));
    return ctx_->receive_all(s, msgs);
  }

 private:
  std::shared_ptr<mpc_net_context_t> ctx_;
};

template <typename F1, typename F2>
inline void run_2pc(const std::shared_ptr<mpc_net_context_t>& c1, const std::shared_ptr<mpc_net_context_t>& c2, F1&& f1,
                    F2&& f2, error_t& out_rv1, error_t& out_rv2) {
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
inline void run_mp(const std::vector<std::shared_ptr<mpc_net_context_t>>& peers, F&& f, std::vector<error_t>& out_rv) {
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

class failing_transport_t final : public data_transport_i {
 public:
  error_t send(party_idx_t /*receiver*/, mem_t /*msg*/) override { return E_GENERAL; }
  error_t receive(party_idx_t /*sender*/, buf_t& /*msg*/) override { return E_GENERAL; }
  error_t receive_all(const std::vector<party_idx_t>& /*senders*/, std::vector<buf_t>& /*msgs*/) override {
    return E_GENERAL;
  }
};

}  // namespace coinbase::testutils::api_harness
