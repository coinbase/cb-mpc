#include "ecp.h"

using namespace coinbase::test;
ecc_point_t ecp_sampler_t::sample(const ecp_distribution_t& dist, const coinbase::crypto::ecurve_t& curve,
                                  const std::vector<base_type_t>& dist_dependencies) {
  auto G = curve.generator();
  bn_t a;
  switch (dist) {
    case ecp_distribution_t::INFINITY_POINT_0:
      a = bn_t(0);
      break;
    case ecp_distribution_t::SMALL_COUNTER_MUL_G_0:
      static int small_number = 0;
      a = bn_t(small_number + 1);
      small_number = (small_number + 1) % 10;
      break;
    case ecp_distribution_t::SCALAR_LESS_Q_0:
      a = bn_t::rand(curve.order());
      break;
    case ecp_distribution_t::SCALAR_GREATER_Q_0:
      do {
        a = bn_t::rand_bitlen(256, false);
      } while (a <= curve.order());
      break;
    case ecp_distribution_t::SCALAR_MUL_G_1:
      return std::get<bn_t>(dist_dependencies[0]) * curve.generator();
    case ecp_distribution_t::SCALAR_MUL_G_2:
      return std::get<bn_t>(dist_dependencies[0]) * std::get<bn_t>(dist_dependencies[1]) * curve.generator();
    case ecp_distribution_t::SCALAR_MUL_POINT_2:
      return std::get<bn_t>(dist_dependencies[0]) * std::get<ecc_point_t>(dist_dependencies[1]);
    case ecp_distribution_t::SUM_2:
      return std::get<ecc_point_t>(dist_dependencies[0]) + std::get<ecc_point_t>(dist_dependencies[1]);
    default:
      cb_assert(false);
      break;
  }
  return a * G;
}

bool ecp_sampler_t::check_single_filter(const ecc_point_t& a, const ecp_filter_t& filter,
                                        const std::vector<base_type_t>& filter_dependencies) {
  switch (filter) {
    case ecp_filter_t::NOT_SAME_AS_1:
      return a != std::get<ecc_point_t>(filter_dependencies[0]);
      break;
    default:
      break;
  }
  return false;
}
