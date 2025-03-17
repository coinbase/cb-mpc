#pragma once
#include <unordered_map>
#include <vector>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

#include "base.h"

namespace coinbase::test {
enum class elgamal_distribution_t {
  ENCRYPTION_0,         // random valid E(pk), m(message), r(randomness)
  ENCRYPTION_E_M_R_3,   // given E, m, r, return (r*G, r*E + m*G)
  ENCRYPTION_E_MG_R_3,  // given E, M, r, return (r*G, r*E + M)
  ENCRYPTION_E_M_2,     // given E, m, sample random r, return (r*G, r*E + m*G)
  SCALAR_MUL_UV_1,
  RERAND_3  // given UV, E, r, do rerandomization
};

enum class elgamal_filter_t { NOT_SAME_AS_1 };

struct elgamal_config_t : config_t<elgamal_distribution_t, elgamal_filter_t> {
  coinbase::crypto::ecurve_t curve;
  elgamal_config_t(
      coinbase::crypto::ecurve_t c, dist_config_t<elgamal_distribution_t> d_c,
      std::vector<filter_config_t<elgamal_filter_t>> f_c = std::vector<filter_config_t<elgamal_filter_t>>())
      : curve(c), config_t<elgamal_distribution_t, elgamal_filter_t>(d_c, f_c) {}
};

class elgamal_sampler_t : public curved_sampler_base_t<elg_com_t, elgamal_distribution_t, elgamal_filter_t> {
 private:
  elg_com_t sample(const elgamal_distribution_t& dist, const coinbase::crypto::ecurve_t& curve,
                   const std::vector<base_type_t>& dist_dependencies) override;
  bool check_single_filter(const elg_com_t& a, const elgamal_filter_t& filter,
                           const std::vector<base_type_t>& filter_dependencies) override;

 public:
  elgamal_sampler_t() = default;
  ~elgamal_sampler_t() = default;
};

}  // namespace coinbase::test