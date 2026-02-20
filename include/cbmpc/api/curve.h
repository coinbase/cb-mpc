#pragma once

#include <cstdint>

namespace coinbase::api {

// Public curve identifiers for API/FFI stability.
enum class curve_id : uint32_t {
  p256 = 1,       // NIST P-256 (aka prime256v1 / secp256r1)
  secp256k1 = 2,  // secp256k1
  ed25519 = 3,    // Edwards25519
};

}  // namespace coinbase::api
