#pragma once

#include <climits>
#include <cstdint>
#include <vector>

#include <cbmpc/c_api/common.h>
#include <cbmpc/c_api/job.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>

namespace coinbase::capi::detail {

inline void transport_free(const cbmpc_transport_t* t, void* ptr) {
  if (!ptr) return;
  if (t && t->free) {
    t->free(t->ctx, ptr);
  } else {
    cbmpc_free(ptr);
  }
}

class c_transport_adapter_t final : public coinbase::api::data_transport_i {
 public:
  explicit c_transport_adapter_t(const cbmpc_transport_t* t) : t_(t) {}

  coinbase::error_t send(coinbase::api::party_idx_t receiver, coinbase::mem_t msg) override {
    if (!t_ || !t_->send) return E_BADARG;
    return t_->send(t_->ctx, receiver, msg.data, msg.size);
  }

  coinbase::error_t receive(coinbase::api::party_idx_t sender, coinbase::buf_t& msg) override {
    if (!t_ || !t_->receive) return E_BADARG;

    cmem_t in{nullptr, 0};
    const coinbase::error_t rv = t_->receive(t_->ctx, sender, &in);
    if (rv) {
      // Best-effort cleanup: integrators may allocate output buffers before
      // returning an error.
      transport_free(t_, in.data);
      return rv;
    }

    if (in.size < 0 || (in.size > 0 && !in.data)) {
      transport_free(t_, in.data);
      return E_FORMAT;
    }
    msg = (in.size == 0) ? coinbase::buf_t() : coinbase::buf_t(in.data, in.size);
    transport_free(t_, in.data);
    return SUCCESS;
  }

  coinbase::error_t receive_all(const std::vector<coinbase::api::party_idx_t>& senders,
                                std::vector<coinbase::buf_t>& msgs) override {
    if (!t_ || !t_->receive_all) return E_NOT_SUPPORTED;
    if (senders.size() > static_cast<size_t>(INT_MAX)) return E_RANGE;

    std::vector<int32_t> senders_i32;
    senders_i32.reserve(senders.size());
    for (auto s : senders) senders_i32.push_back(static_cast<int32_t>(s));

    cmems_t out{0, nullptr, nullptr};
    const coinbase::error_t rv =
        t_->receive_all(t_->ctx, senders_i32.data(), static_cast<int>(senders_i32.size()), &out);
    if (rv) {
      // Best-effort cleanup: integrators may allocate output buffers before
      // returning an error.
      transport_free(t_, out.data);
      if (out.sizes && reinterpret_cast<void*>(out.sizes) != out.data) transport_free(t_, out.sizes);
      return rv;
    }

    if (out.count < 0 || out.count != static_cast<int>(senders.size())) {
      transport_free(t_, out.data);
      transport_free(t_, out.sizes);
      return E_FORMAT;
    }
    if (out.count > 0 && !out.sizes) {
      transport_free(t_, out.data);
      return E_FORMAT;
    }

    int total = 0;
    for (int i = 0; i < out.count; i++) {
      const int sz = out.sizes[i];
      if (sz < 0) {
        transport_free(t_, out.data);
        transport_free(t_, out.sizes);
        return E_FORMAT;
      }
      if (sz > INT_MAX - total) {
        transport_free(t_, out.data);
        transport_free(t_, out.sizes);
        return E_RANGE;
      }
      total += sz;
    }
    if (total > 0 && !out.data) {
      transport_free(t_, out.sizes);
      return E_FORMAT;
    }

    msgs.clear();
    msgs.reserve(static_cast<size_t>(out.count));

    int offset = 0;
    for (int i = 0; i < out.count; i++) {
      const int sz = out.sizes[i];
      if (sz == 0) {
        msgs.emplace_back();
        continue;
      }
      msgs.emplace_back(out.data + offset, sz);
      offset += sz;
    }

    transport_free(t_, out.data);
    transport_free(t_, out.sizes);
    return SUCCESS;
  }

 private:
  const cbmpc_transport_t* t_;
};

}  // namespace coinbase::capi::detail
