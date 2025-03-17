#pragma once

#include <cbmpc/crypto/base.h>

constexpr int small_primes_count = 10000;

extern const unsigned small_primes[small_primes_count];

static error_t check_integer_with_small_primes(const bn_t& prime, int alpha) {
  for (int i = 0; i < small_primes_count; i++) {
    int small_prime = small_primes[i];
    if (small_prime > alpha) break;
    if (mod_t::mod(prime, small_prime) == 0) return coinbase::error(E_CRYPTO);
  }
  return SUCCESS;
}
