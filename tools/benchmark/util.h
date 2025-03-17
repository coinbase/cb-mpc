#pragma once

#include <cbmpc/crypto/base.h>

inline coinbase::crypto::ecurve_t get_curve(int index) {
  switch (index) {
    case 0:
      return coinbase::crypto::curve_p256;
    case 1:
      return coinbase::crypto::curve_p384;
    case 2:
      return coinbase::crypto::curve_p521;
    case 3:
      return coinbase::crypto::curve_secp256k1;
    case 4:
      return coinbase::crypto::curve_ed25519;
    default:
      throw std::runtime_error("Unknown curve index");
  }
  return coinbase::crypto::curve_secp256k1;
}
