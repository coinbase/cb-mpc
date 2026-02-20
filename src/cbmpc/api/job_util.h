#pragma once

#include <string>
#include <unordered_set>

#include <cbmpc/core/job.h>
#include <cbmpc/internal/protocol/mpc_job.h>

namespace coinbase::api::detail {

// Basic validation for the public 2PC job view.
//
// Shared across all 2-party public API wrappers.
inline error_t validate_job_2p(const coinbase::api::job_2p_t& job) {
  if (job.self != party_2p_t::p1 && job.self != party_2p_t::p2) return coinbase::error(E_BADARG, "invalid job.self");
  if (job.p1_name.empty()) return coinbase::error(E_BADARG, "p1_name must be non-empty");
  if (job.p2_name.empty()) return coinbase::error(E_BADARG, "p2_name must be non-empty");
  if (job.p1_name == job.p2_name) return coinbase::error(E_BADARG, "party names must be distinct");
  return SUCCESS;
}

// Map public 2PC role enum to internal protocol role enum.
inline coinbase::mpc::party_t to_internal_party(party_2p_t self) { return static_cast<coinbase::mpc::party_t>(self); }

// Convert the public 2PC job view to the internal protocol job.
//
// Note: the returned internal job copies party names, but does *not* take
// ownership of the transport. It stores a non-owning pointer/reference to
// `job.transport`, which must outlive any protocol call using the returned job.
inline coinbase::mpc::job_2p_t to_internal_job(const coinbase::api::job_2p_t& job) {
  return coinbase::mpc::job_2p_t(to_internal_party(job.self), std::string(job.p1_name), std::string(job.p2_name),
                                 job.transport);
}

// Convert the public MP job view to the internal protocol job.
//
// Note: the returned internal job copies party names, but does *not* take
// ownership of the transport. It stores a non-owning pointer/reference to
// `job.transport`, which must outlive any protocol call using the returned job.
inline coinbase::mpc::job_mp_t to_internal_job(const coinbase::api::job_mp_t& job) {
  std::vector<coinbase::crypto::pname_t> names;
  names.reserve(job.party_names.size());
  for (const auto& name : job.party_names) names.emplace_back(name);
  return coinbase::mpc::job_mp_t(job.self, std::move(names), job.transport);
}

// Basic validation for the public MP job view.
//
// Shared across all multi-party public API wrappers.
inline error_t validate_job_mp(const coinbase::api::job_mp_t& job) {
  const size_t n = job.party_names.size();
  if (n < 2) return coinbase::error(E_BADARG, "job must contain at least 2 parties");
  if (n > 64) return coinbase::error(E_RANGE, "at most 64 parties are supported");
  if (job.self < 0 || static_cast<size_t>(job.self) >= n) return coinbase::error(E_BADARG, "invalid job.self");

  std::unordered_set<std::string_view> names;
  names.reserve(n);
  for (const auto& name : job.party_names) {
    if (name.empty()) return coinbase::error(E_BADARG, "party name must be non-empty");
    if (!names.insert(name).second) return coinbase::error(E_BADARG, "duplicate party name");
  }
  return SUCCESS;
}

}  // namespace coinbase::api::detail
