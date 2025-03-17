#pragma once
#include <unordered_map>
#include <vector>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>
#include <cbmpc/zk/zk_pedersen.h>
#include <cbmpc/zk/zk_unknown_order.h>

#include "base.h"

namespace coinbase::test {
enum class bn_distribution_t {
  // the number at the end "_x" means the distribution has x dependencies
  ZERO_0,
  SMALL_COUNTER_POS_0,
  INT32_POS_0,
  INT128_POS_0,
  INT256_POS_0,
  INT4096_POS_0,
  INT32_NEG_0,
  INT256_NEG_0,
  PRIME256_SAFE_0,
  PRIME1024_SAFE_0,
  PRIME256_0,
  PRIME1024_0,
  GENERAL_NUMBER_0,
  GENERAL_POS_NUMBER_0,
  TWO_TIMES_OF_1,
  RAND_BN_1,                      // rand(q)
  RAND_PAILLIER_N_1,              // rand(mod_t(N))
  RAND_PAILLIER_NOT_COPRIME_N_1,  // given (N,p,q), return rand()*p
  PAILLIER_ENCRYPTION_3,          // given p, m, r, return p.encrypt(m, r)
  PEDERSEN_COMMITMENT_2,          // given m, r, return g^m * h^r
  POWER_MOD_3,                    // given m, r, N return m^r mod N
  RAND_PEDERSEN_PTAG_0,
  RSA_2048_N_0,
  RAND_MULTIPLE_OF_1,  // given a return a*rand_bit(256)
  MULTIPLICATION_2     // given a,b return a*b
};

enum class bn_filter_t {
  ED25519_COEF_FIELD_0,     // less than ed25519 curve group order
  SECP256K1_COEF_FIELD_0,   // less than secp256k1 curve group order
  P256_COEF_FIELD_0,        // less than p256 curve group order
  GREATER_ED25519_ORDER_0,  // greater than ed25519 curve group order
  NOT_SAME_AS_1,
  GREATER_THAN_1
};

typedef config_t<bn_distribution_t, bn_filter_t> bn_config_t;
typedef filter_config_t<bn_filter_t> bn_filter_config_t;

class bn_sampler_t : public sampler_base_t<bn_t, bn_distribution_t, bn_filter_t> {
 private:
  bn_t sample(const bn_distribution_t& dist, const std::vector<base_type_t>& dist_dependencies) override;
  bn_t sample_general_number();
  bn_t sample_general_pos_number();
  bool check_single_filter(const bn_t& a, const bn_filter_t& filter,
                           const std::vector<base_type_t>& filter_dependencies) override;

 public:
  bn_sampler_t() = default;
  ~bn_sampler_t() = default;
};

}  // namespace coinbase::test