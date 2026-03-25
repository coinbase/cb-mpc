#pragma once

#include <cstdint>
#include <vector>

#include <cbmpc/core/error.h>
#include <cbmpc/internal/protocol/hd_tree_bip32.h>

namespace coinbase::api::detail {

// Convert a public bip32 path type (must have `.indices`) to internal representation.
template <typename PublicPathT>
inline coinbase::mpc::bip32_path_t to_internal_bip32_path(const PublicPathT& in) {
  coinbase::mpc::bip32_path_t out;
  for (const uint32_t idx : in.indices) out.append(idx);
  return out;
}

// Validate that a list of public bip32 paths contains no duplicates.
// The public path type must have `.indices` (vector<uint32_t>).
template <typename PublicPathT>
inline error_t validate_no_duplicate_bip32_paths(const std::vector<PublicPathT>& paths) {
  // O(n^2) is fine here: typical derivation batches are small (caller-controlled).
  for (size_t i = 0; i < paths.size(); i++) {
    for (size_t j = i + 1; j < paths.size(); j++) {
      if (paths[i].indices == paths[j].indices) return coinbase::error(E_BADARG, "duplicate non_hardened_paths");
    }
  }
  return SUCCESS;
}

}  // namespace coinbase::api::detail
