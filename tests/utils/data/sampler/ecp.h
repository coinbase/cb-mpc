#pragma once
#include <unordered_map>
#include <vector>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>
#include <cbmpc/zk/zk_pedersen.h>

#include "base.h"

namespace coinbase::test {
enum class ecp_distribution_t {
  INFINITY_POINT_0,
  SMALL_COUNTER_MUL_G_0,  // 1*G, 2*G, ..
  SCALAR_LESS_Q_0,        // n*G with random n less than the curve group order
  SCALAR_GREATER_Q_0,     // n*G with random n greater than the curve group order
  SCALAR_MUL_G_1,         // n*G given scalar n
  SCALAR_MUL_G_2,         // a*b*G given scalar a, b
  SCALAR_MUL_POINT_2,     // a*H given scalar a, and point H
  PEDERSEN_COMMITMENT_3,  // x * G + r * H; given scalar x,r and point H
  SUM_2                   // a+b; given points a and b
};

enum class ecp_filter_t { NOT_SAME_AS_1 };

struct ecp_config_t : config_t<ecp_distribution_t, ecp_filter_t> {
  coinbase::crypto::ecurve_t curve;
  ecp_config_t(coinbase::crypto::ecurve_t c, dist_config_t<ecp_distribution_t> d_c,
               std::vector<filter_config_t<ecp_filter_t>> f_c = std::vector<filter_config_t<ecp_filter_t>>())
      : curve(c), config_t<ecp_distribution_t, ecp_filter_t>(d_c, f_c) {}
};

class ecp_sampler_t : public curved_sampler_base_t<ecc_point_t, ecp_distribution_t, ecp_filter_t> {
 private:
  ecc_point_t sample(const ecp_distribution_t& dist, const coinbase::crypto::ecurve_t& curve,
                     const std::vector<base_type_t>& dist_dependencies) override;
  bool check_single_filter(const ecc_point_t& a, const ecp_filter_t& filter,
                           const std::vector<base_type_t>& filter_dependencies) override;

 public:
  ecp_sampler_t() = default;
  ~ecp_sampler_t() = default;
};

}  // namespace coinbase::test