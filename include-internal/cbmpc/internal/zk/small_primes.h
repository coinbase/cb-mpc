#pragma once

#include <limits>

#include <cbmpc/internal/crypto/base.h>

namespace coinbase::zk {

constexpr int small_primes_count = 10000;

extern const unsigned small_primes[small_primes_count];

static error_t check_integer_with_small_primes(const bn_t& prime, int alpha) {
  cb_assert(crypto::is_vartime_scope());

  const auto* factor = small_primes;
  const auto* const factors_end = small_primes + small_primes_count;
  while (factor != factors_end && static_cast<int>(*factor) <= alpha) {
    BN_ULONG product = 1;
    const auto* batch_end = factor;
    while (batch_end != factors_end && static_cast<int>(*batch_end) <= alpha &&
           product <= std::numeric_limits<BN_ULONG>::max() / *batch_end) {
      product *= *batch_end++;
    }

    const BN_ULONG remainder = BN_mod_word(prime, product);
    cb_assert(remainder != std::numeric_limits<BN_ULONG>::max());
    while (factor != batch_end) {
      if (remainder % *factor++ == 0) return coinbase::error(E_CRYPTO);
    }
  }
  return SUCCESS;
}

}  // namespace coinbase::zk
