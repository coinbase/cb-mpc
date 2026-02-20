#pragma once

#include <cbmpc/api/curve.h>
#include <cbmpc/internal/crypto/base.h>

namespace coinbase::api::detail {

// Map public curve identifiers to internal curve objects.
inline coinbase::crypto::ecurve_t to_internal_curve(curve_id curve) {
  switch (curve) {
    case curve_id::p256:
      return coinbase::crypto::curve_p256;
    case curve_id::secp256k1:
      return coinbase::crypto::curve_secp256k1;
    case curve_id::ed25519:
      return coinbase::crypto::curve_ed25519;
  }
  return coinbase::crypto::ecurve_t();
}

// Map internal curves back to public curve identifiers.
//
// Note: Only curves supported by the public API wrappers are mapped here.
inline bool from_internal_curve(coinbase::crypto::ecurve_t curve, curve_id& out) {
  if (curve == coinbase::crypto::curve_p256) {
    out = curve_id::p256;
    return true;
  }
  if (curve == coinbase::crypto::curve_secp256k1) {
    out = curve_id::secp256k1;
    return true;
  }
  if (curve == coinbase::crypto::curve_ed25519) {
    out = curve_id::ed25519;
    return true;
  }
  return false;
}

}  // namespace coinbase::api::detail
