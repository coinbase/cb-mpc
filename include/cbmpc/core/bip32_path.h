#pragma once

#include <cstdint>
#include <vector>

namespace coinbase::api {

// BIP32 derivation path: a sequence of 32-bit child indices.
//
// This is a lightweight value type shared across HD keyset APIs.
// Index interpretation (hardened vs non-hardened) is defined by the caller / spec.
struct bip32_path_t {
  std::vector<uint32_t> indices;
};

}  // namespace coinbase::api
