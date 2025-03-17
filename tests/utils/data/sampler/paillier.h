#pragma once
#include <unordered_map>
#include <vector>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

#include "base.h"

namespace coinbase::test {

enum class paillier_distribution_t {
  P_PRIME1024_Q_PRIME1024_0,
  P_SMALL_PRIME_Q_PRIME1024_0,
  N_MULTIPLE_OF_THREE_PRIMES_0,
  GET_PUB_FROM_PRIV_1
};

enum class paillier_filter_t { NOT_SAME_AS_1 };

typedef config_t<paillier_distribution_t, paillier_filter_t> paillier_config_t;

class paillier_sampler_t
    : public sampler_base_t<coinbase::crypto::paillier_t, paillier_distribution_t, paillier_filter_t> {
 private:
  coinbase::crypto::paillier_t sample(const paillier_distribution_t& dist,
                                      const std::vector<base_type_t>& dist_dependencies) override;
  bool check_single_filter(const coinbase::crypto::paillier_t& a, const paillier_filter_t& filter,
                           const std::vector<base_type_t>& filter_dependencies) override;

 public:
  paillier_sampler_t() = default;
  ~paillier_sampler_t() = default;
};

}  // namespace coinbase::test