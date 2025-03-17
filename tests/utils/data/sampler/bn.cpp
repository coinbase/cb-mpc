#include "bn.h"
using namespace coinbase::test;

bn_t bn_sampler_t::sample(const bn_distribution_t& dist, const std::vector<base_type_t>& dist_dependencies) {
  bn_t a;
  switch (dist) {
    case bn_distribution_t::ZERO_0:
      a = 0;
      break;
    case bn_distribution_t::SMALL_COUNTER_POS_0:
      static int small_number = 0;
      a = small_number + 1;
      small_number = (small_number + 1) % 10;
      break;
    case bn_distribution_t::INT32_POS_0:
      a = bn_t::rand_bitlen(32, false);
      break;
    case bn_distribution_t::INT128_POS_0:
      a = bn_t::rand_bitlen(128, false);
      break;
    case bn_distribution_t::INT256_POS_0:
      a = bn_t::rand_bitlen(256, false);
      break;
    case bn_distribution_t::INT4096_POS_0:
      a = bn_t::rand_bitlen(4096, false);
      break;
    case bn_distribution_t::INT32_NEG_0:
      a = bn_t::rand_bitlen(32, false) * (-1);
      break;
    case bn_distribution_t::INT256_NEG_0:
      a = bn_t::rand_bitlen(256, false) * (-1);
      break;
    case bn_distribution_t::PRIME256_SAFE_0:
      a = bn_t::generate_prime(256, true);
      break;
    case bn_distribution_t::PRIME1024_SAFE_0:
      a = bn_t::generate_prime(1024, true);
      break;
    case bn_distribution_t::PRIME256_0:
      a = bn_t::generate_prime(256, false);
      break;
    case bn_distribution_t::PRIME1024_0:
      a = bn_t::generate_prime(1024, false);
      break;
    case bn_distribution_t::GENERAL_NUMBER_0:
      a = sample_general_number();
      break;
    case bn_distribution_t::GENERAL_POS_NUMBER_0:
      a = sample_general_pos_number();
      break;
    case bn_distribution_t::TWO_TIMES_OF_1:
      a = std::get<bn_t>(dist_dependencies[0]) * 2;
      break;
    case bn_distribution_t::RAND_BN_1:
      a = bn_t::rand(std::get<bn_t>(dist_dependencies[0]));
      break;
    case bn_distribution_t::RAND_PAILLIER_N_1:
      a = bn_t::rand(std::get<coinbase::crypto::paillier_t>(dist_dependencies[0]).get_N());
      break;
    case bn_distribution_t::RAND_PAILLIER_NOT_COPRIME_N_1:
      a = std::get<coinbase::crypto::paillier_t>(dist_dependencies[0]).get_p() * bn_t::rand_bitlen(256, false);
      break;
    case bn_distribution_t::PAILLIER_ENCRYPTION_3:
      a = std::get<coinbase::crypto::paillier_t>(dist_dependencies[0])
              .encrypt(std::get<bn_t>(dist_dependencies[1]), std::get<bn_t>(dist_dependencies[2]));
      break;
    case bn_distribution_t::PEDERSEN_COMMITMENT_2:
      MODULO(coinbase::zk::pedersen_commitment_params_t::get().p) {
        a = coinbase::zk::pedersen_commitment_params_t::get().g.pow(std::get<bn_t>(dist_dependencies[0])) *
            coinbase::zk::pedersen_commitment_params_t::get().h.pow(std::get<bn_t>(dist_dependencies[1]));
      }
      break;
    case bn_distribution_t::POWER_MOD_3:
      a = std::get<bn_t>(dist_dependencies[0])
              .pow_mod(std::get<bn_t>(dist_dependencies[1]), std::get<bn_t>(dist_dependencies[2]));
      break;
    case bn_distribution_t::RSA_2048_N_0:
      a = bn_t::generate_prime(1024, true) * bn_t::generate_prime(1024, true);
      break;
    case bn_distribution_t::RAND_PEDERSEN_PTAG_0:
      a = bn_t::rand(coinbase::zk::pedersen_commitment_params_t::get().p_tag);
      break;
    case bn_distribution_t::RAND_MULTIPLE_OF_1:
      a = std::get<bn_t>(dist_dependencies[0]) * bn_t::rand_bitlen(256, false);
      break;
    case bn_distribution_t::MULTIPLICATION_2:
      a = std::get<bn_t>(dist_dependencies[0]) * std::get<bn_t>(dist_dependencies[1]);
      break;
    default:
      cb_assert(false);
      break;
  }

  return a;
}

bool bn_sampler_t::check_single_filter(const bn_t& a, const bn_filter_t& filter,
                                       const std::vector<base_type_t>& filter_dependencies) {
  switch (filter) {
    case bn_filter_t::NOT_SAME_AS_1:
      return a != std::get<bn_t>(filter_dependencies[0]);
    case bn_filter_t::GREATER_THAN_1:
      return a > std::get<bn_t>(filter_dependencies[0]);
    case bn_filter_t::ED25519_COEF_FIELD_0:
      return a < bn_t(coinbase::crypto::curve_ed25519.order());
    case bn_filter_t::SECP256K1_COEF_FIELD_0:
      return a < bn_t(coinbase::crypto::curve_secp256k1.order());
    case bn_filter_t::P256_COEF_FIELD_0:
      return a < bn_t(coinbase::crypto::curve_p256.order());
    case bn_filter_t::GREATER_ED25519_ORDER_0:
      return a > bn_t(coinbase::crypto::curve_ed25519.order());
    default:
      break;
  }
  return false;
}

bn_t bn_sampler_t::sample_general_number() {
  static int idx = 0;
  bn_distribution_t dists[9] = {
      bn_distribution_t::ZERO_0,       bn_distribution_t::SMALL_COUNTER_POS_0, bn_distribution_t::INT32_POS_0,
      bn_distribution_t::INT256_POS_0, bn_distribution_t::INT4096_POS_0,       bn_distribution_t::INT32_NEG_0,
      bn_distribution_t::INT256_NEG_0, bn_distribution_t::PRIME256_SAFE_0,     bn_distribution_t::PRIME256_0};
  idx = (idx + 1) % 9;
  return sample(dists[idx], std::vector<base_type_t>());
}

bn_t bn_sampler_t::sample_general_pos_number() {
  static int idx = 0;
  bn_distribution_t dists[7] = {bn_distribution_t::ZERO_0,        bn_distribution_t::SMALL_COUNTER_POS_0,
                                bn_distribution_t::INT32_POS_0,   bn_distribution_t::INT256_POS_0,
                                bn_distribution_t::INT4096_POS_0, bn_distribution_t::PRIME256_SAFE_0,
                                bn_distribution_t::PRIME256_0};
  idx = (idx + 1) % 7;
  return sample(dists[idx], std::vector<base_type_t>());
}