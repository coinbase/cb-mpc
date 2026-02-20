#pragma once

#include <string>
#include <vector>

#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

namespace coinbase::api::detail {

// Conservative size limits for untrusted byte blobs passed into the public API.
//
// These APIs frequently parse/deserialize opaque blobs into internal structures,
// which can otherwise lead to large allocations and CPU usage on attacker-controlled
// inputs (e.g., blobs received over the network).
inline constexpr int MAX_OPAQUE_BLOB_SIZE = 1 * 1024 * 1024;       // 1 MiB
inline constexpr int MAX_CIPHERTEXT_BLOB_SIZE = 64 * 1024 * 1024;  // 64 MiB
inline constexpr int MAX_MESSAGE_DIGEST_SIZE = 64;                 // 64 bytes (e.g., SHA-512 / SHA3-512)

// Validate the basic invariants of a mem_t passed into the public API.
//
// Important: Do not use cb_assert() here. These checks are for *untrusted*
// inputs (including data that may be adversary-controlled) and must fail
// gracefully with an error code.
inline error_t validate_mem_arg(mem_t m, const char* name) {
  if (m.size < 0) return coinbase::error(E_BADARG, std::string("invalid ") + name, /*to_print_stack_trace=*/false);
  if (m.size > 0 && !m.data)
    return coinbase::error(E_BADARG, std::string("invalid ") + name, /*to_print_stack_trace=*/false);
  return SUCCESS;
}

inline error_t validate_mem_vec_arg(const std::vector<mem_t>& ms, const char* name) {
  for (size_t i = 0; i < ms.size(); i++) {
    const error_t rv = validate_mem_arg(ms[i], name);
    if (rv) return rv;
  }
  return SUCCESS;
}

inline error_t validate_mem_arg_max_size(mem_t m, const char* name, int max_size) {
  if (const error_t rv = validate_mem_arg(m, name)) return rv;
  if (max_size < 0) return coinbase::error(E_BADARG, "invalid max_size", /*to_print_stack_trace=*/false);
  if (m.size > max_size)
    return coinbase::error(E_RANGE, std::string(name) + " too large", /*to_print_stack_trace=*/false);
  return SUCCESS;
}

inline error_t validate_mem_vec_arg_max_size(const std::vector<mem_t>& ms, const char* name, int max_size) {
  for (size_t i = 0; i < ms.size(); i++) {
    const error_t rv = validate_mem_arg_max_size(ms[i], name, max_size);
    if (rv) return rv;
  }
  return SUCCESS;
}

}  // namespace coinbase::api::detail
