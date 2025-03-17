#include "paillier.h"

using namespace coinbase::test;

coinbase::crypto::paillier_t paillier_sampler_t::sample(const paillier_distribution_t& dist,
                                                        const std::vector<base_type_t>& dist_dependencies) {
  coinbase::crypto::paillier_t a;
  switch (dist) {
    case paillier_distribution_t::P_PRIME1024_Q_PRIME1024_0: {
      bn_t p = bn_t::generate_prime(1024, true);
      bn_t q = bn_t::generate_prime(1024, true);
      bn_t N = p * q;
      a.create_prv(N, p, q);
      break;
    }
    case paillier_distribution_t::GET_PUB_FROM_PRIV_1:
      a.create_pub(std::get<coinbase::crypto::paillier_t>(dist_dependencies[0]).get_N());
      break;
    case paillier_distribution_t::P_SMALL_PRIME_Q_PRIME1024_0: {
      bn_t p = bn_t::generate_prime(7, true);
      bn_t q = bn_t::generate_prime(1024, true);
      bn_t N = p * q;
      a.create_prv(N, p, q);
      break;
    }
    case paillier_distribution_t::N_MULTIPLE_OF_THREE_PRIMES_0: {
      bn_t p = bn_t::generate_prime(1024, true);
      bn_t q = bn_t::generate_prime(512, true);
      bn_t r = bn_t::generate_prime(512, true);
      bn_t N = p * q * r;
      a.create_prv(N, p, q * r);
      break;
    }
    default:
      break;
  }
  return a;
}

bool paillier_sampler_t::check_single_filter(const coinbase::crypto::paillier_t& a, const paillier_filter_t& filter,
                                             const std::vector<base_type_t>& filter_dependencies) {
  switch (filter) {
    case paillier_filter_t::NOT_SAME_AS_1:
      return a.get_N() != std::get<coinbase::crypto::paillier_t>(filter_dependencies[0]).get_N();
      break;
    default:
      break;
  }
  return false;
}
