#include "elgamal.h"

using namespace coinbase::test;
elg_com_t elgamal_sampler_t::sample(const elgamal_distribution_t& dist, const coinbase::crypto::ecurve_t& curve,
                                    const std::vector<base_type_t>& dist_dependencies) {
  const ecc_point_t& G = curve.generator();
  const mod_t& q = curve.order();
  elg_com_t UV;
  switch (dist) {
    case elgamal_distribution_t::ENCRYPTION_0: {
      ecc_point_t E = bn_t::rand(q) * G;
      bn_t m = bn_t::rand(q);
      bn_t r = bn_t::rand(q);
      UV = elg_com_t(r * G, r * E + m * G);
      break;
    }
    case elgamal_distribution_t::ENCRYPTION_E_M_R_3: {
      auto E = std::get<ecc_point_t>(dist_dependencies[0]);
      auto m = std::get<bn_t>(dist_dependencies[1]);
      auto r = std::get<bn_t>(dist_dependencies[2]);
      UV = elg_com_t(r * G, r * E + m * G);
      break;
    }
    case elgamal_distribution_t::ENCRYPTION_E_MG_R_3: {
      auto E = std::get<ecc_point_t>(dist_dependencies[0]);
      auto M = std::get<ecc_point_t>(dist_dependencies[1]);
      auto r = std::get<bn_t>(dist_dependencies[2]);
      UV = elg_com_t(r * G, r * E + M);
      break;
    }
    case elgamal_distribution_t::ENCRYPTION_E_M_2: {
      auto E = std::get<ecc_point_t>(dist_dependencies[0]);
      auto m = std::get<bn_t>(dist_dependencies[1]);
      auto r = curve.get_random_value();
      UV = elg_com_t(r * G, r * E + m * G);
      break;
    }
    case elgamal_distribution_t::SCALAR_MUL_UV_1: {
      auto m = std::get<bn_t>(dist_dependencies[0]);
      auto old_UV = std::get<elg_com_t>(dist_dependencies[1]);
      UV = m * old_UV;
      break;
    }
    case elgamal_distribution_t::RERAND_3: {
      auto old_UV = std::get<elg_com_t>(dist_dependencies[0]);
      auto E = std::get<ecc_point_t>(dist_dependencies[1]);
      auto r = std::get<bn_t>(dist_dependencies[2]);
      UV = old_UV.rerand(E, r);
      break;
    }
    default:
      cb_assert(false);
      break;
  }
  return UV;
}

bool elgamal_sampler_t::check_single_filter(const elg_com_t& a, const elgamal_filter_t& filter,
                                            const std::vector<base_type_t>& filter_dependencies) {
  switch (filter) {
    case elgamal_filter_t::NOT_SAME_AS_1:
      return a != std::get<elg_com_t>(filter_dependencies[0]);
      break;
    default:
      break;
  }
  return false;
}