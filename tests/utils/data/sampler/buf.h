#pragma once
#include <unordered_map>
#include <vector>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

#include "base.h"

namespace coinbase::test {

enum class buf_distribution_t { RANDOM_32BYTES_0, RANDOM_16BYTES_0, SAME_AS_1 };

enum class buf_filter_t { NOT_SAME_AS_1 };

typedef config_t<buf_distribution_t, buf_filter_t> buf_config_t;

class buf_sampler_t : public sampler_base_t<buf_t, buf_distribution_t, buf_filter_t> {
 private:
  buf_t sample(const buf_distribution_t& dist, const std::vector<base_type_t>& dist_dependencies) override;
  bool check_single_filter(const buf_t& a, const buf_filter_t& filter,
                           const std::vector<base_type_t>& filter_dependencies) override;

 public:
  buf_sampler_t() = default;
  ~buf_sampler_t() = default;
};

}  // namespace coinbase::test