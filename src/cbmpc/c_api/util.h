#pragma once

#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/c_api/common.h>
#include <cbmpc/c_api/job.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>

#include "transport_adapter.h"

namespace coinbase::capi::detail {

inline cbmpc_error_t validate_cmem(cmem_t m) {
  if (m.size < 0) return E_BADARG;
  if (m.size > 0 && !m.data) return E_BADARG;
  return CBMPC_SUCCESS;
}

inline coinbase::mem_t view_cmem(cmem_t m) { return coinbase::mem_t(m.data, m.size); }

inline cbmpc_error_t validate_2pc_job(const cbmpc_2pc_job_t* job) {
  if (!job) return E_BADARG;
  if (!job->p1_name || !job->p2_name) return E_BADARG;
  if (job->p1_name[0] == '\0' || job->p2_name[0] == '\0') return E_BADARG;
  if (std::strcmp(job->p1_name, job->p2_name) == 0) return E_BADARG;
  if (!job->transport || !job->transport->send || !job->transport->receive) return E_BADARG;
  return CBMPC_SUCCESS;
}

inline cbmpc_error_t validate_mp_job(const cbmpc_mp_job_t* job) {
  if (!job) return E_BADARG;
  if (job->party_names_count < 0) return E_BADARG;
  // Match the public C++ API contract: MP jobs require at least 2 parties.
  if (job->party_names_count < 2) return E_BADARG;
  if (job->party_names_count > 64) return E_RANGE;
  if (job->self < 0 || job->self >= job->party_names_count) return E_BADARG;

  if (!job->party_names) return E_BADARG;
  std::unordered_set<std::string_view> names;
  names.reserve(static_cast<size_t>(job->party_names_count));
  for (int i = 0; i < job->party_names_count; i++) {
    const char* name = job->party_names[i];
    if (!name) return E_BADARG;
    if (name[0] == '\0') return E_BADARG;
    if (!names.insert(std::string_view(name)).second) return E_BADARG;  // duplicate
  }
  if (!job->transport || !job->transport->send || !job->transport->receive || !job->transport->receive_all)
    return E_BADARG;
  return CBMPC_SUCCESS;
}

inline cbmpc_error_t to_cpp_party(cbmpc_2pc_party_t p, coinbase::api::party_2p_t& out) {
  switch (p) {
    case CBMPC_2PC_P1:
      out = coinbase::api::party_2p_t::p1;
      return CBMPC_SUCCESS;
    case CBMPC_2PC_P2:
      out = coinbase::api::party_2p_t::p2;
      return CBMPC_SUCCESS;
  }
  return E_BADARG;
}

inline cbmpc_error_t to_cpp_curve(cbmpc_curve_id_t c, coinbase::api::curve_id& out) {
  switch (c) {
    case CBMPC_CURVE_P256:
      out = coinbase::api::curve_id::p256;
      return CBMPC_SUCCESS;
    case CBMPC_CURVE_SECP256K1:
      out = coinbase::api::curve_id::secp256k1;
      return CBMPC_SUCCESS;
    case CBMPC_CURVE_ED25519:
      out = coinbase::api::curve_id::ed25519;
      return CBMPC_SUCCESS;
  }
  return E_BADARG;
}

struct job_2p_cpp_ctx_t {
  c_transport_adapter_t transport;
  coinbase::api::job_2p_t job;

  job_2p_cpp_ctx_t(const cbmpc_2pc_job_t* job_c, coinbase::api::party_2p_t self)
      : transport(job_c->transport), job{self, job_c->p1_name, job_c->p2_name, transport} {}
};

struct job_mp_cpp_ctx_t {
  c_transport_adapter_t transport;
  coinbase::api::job_mp_t job;

  explicit job_mp_cpp_ctx_t(const cbmpc_mp_job_t* job_c)
      : transport(job_c->transport), job{job_c->self, make_party_names(job_c), transport} {}

 private:
  static std::vector<std::string_view> make_party_names(const cbmpc_mp_job_t* job_c) {
    std::vector<std::string_view> out;
    out.reserve(static_cast<size_t>(job_c->party_names_count));
    for (int i = 0; i < job_c->party_names_count; i++) out.emplace_back(job_c->party_names[i]);
    return out;
  }
};

inline cbmpc_error_t alloc_cmem_from_buf(const coinbase::buf_t& buf, cmem_t* out) {
  if (!out) return E_BADARG;
  out->data = nullptr;
  out->size = 0;

  const int n = buf.size();
  if (n < 0) return E_FORMAT;
  if (n == 0) return CBMPC_SUCCESS;

  uint8_t* data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(n)));
  if (!data) return E_INSUFFICIENT;
  std::memmove(data, buf.data(), static_cast<size_t>(n));
  out->data = data;
  out->size = n;
  return CBMPC_SUCCESS;
}

inline cbmpc_error_t alloc_cmems_from_bufs(const std::vector<coinbase::buf_t>& bufs, cmems_t* out) {
  if (!out) return E_BADARG;
  out->count = 0;
  out->data = nullptr;
  out->sizes = nullptr;
  if (bufs.empty()) return CBMPC_SUCCESS;

  if (bufs.size() > static_cast<size_t>(INT_MAX)) return E_RANGE;

  int total = 0;
  for (const auto& b : bufs) {
    const int sz = b.size();
    if (sz < 0) return E_FORMAT;
    if (sz > INT_MAX - total) return E_RANGE;
    total += sz;
  }

  const int count = static_cast<int>(bufs.size());
  out->count = count;

  out->sizes = static_cast<int*>(cbmpc_malloc(sizeof(int) * static_cast<size_t>(count)));
  if (!out->sizes) {
    *out = cmems_t{0, nullptr, nullptr};
    return E_INSUFFICIENT;
  }

  if (total > 0) {
    out->data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(total)));
    if (!out->data) {
      cbmpc_free(out->sizes);
      *out = cmems_t{0, nullptr, nullptr};
      return E_INSUFFICIENT;
    }
  }

  int offset = 0;
  for (int i = 0; i < count; i++) {
    const int sz = bufs[i].size();
    out->sizes[i] = sz;
    if (sz) {
      std::memmove(out->data + offset, bufs[i].data(), static_cast<size_t>(sz));
      offset += sz;
    }
  }

  return CBMPC_SUCCESS;
}

inline cbmpc_error_t view_cmems(cmems_t in, std::vector<coinbase::mem_t>& out) {
  out.clear();
  if (in.count < 0) return E_BADARG;
  if (in.count == 0) return CBMPC_SUCCESS;
  if (!in.sizes) return E_BADARG;

  int total = 0;
  for (int i = 0; i < in.count; i++) {
    const int sz = in.sizes[i];
    if (sz < 0) return E_BADARG;
    if (sz > INT_MAX - total) return E_RANGE;
    total += sz;
  }
  if (total > 0 && !in.data) return E_BADARG;

  out.reserve(static_cast<size_t>(in.count));
  int offset = 0;
  for (int i = 0; i < in.count; i++) {
    const int sz = in.sizes[i];
    if (sz == 0) {
      out.emplace_back(nullptr, 0);
      continue;
    }
    out.emplace_back(in.data + offset, sz);
    offset += sz;
  }
  return CBMPC_SUCCESS;
}

}  // namespace coinbase::capi::detail
