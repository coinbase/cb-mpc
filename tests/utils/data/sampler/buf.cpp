#include "buf.h"

using namespace coinbase::test;

buf_t buf_sampler_t::sample(const buf_distribution_t& dist, const std::vector<base_type_t>& dist_dependencies) {
  buf_t a;
  switch (dist) {
    case buf_distribution_t::RANDOM_32BYTES_0: {
      a = crypto::gen_random(32);
      break;
    }
    case buf_distribution_t::RANDOM_16BYTES_0: {
      a = crypto::gen_random(16);
      break;
    }
    case buf_distribution_t::SAME_AS_1:
      a = std::get<buf_t>(dist_dependencies[0]);
      break;
    default:
      break;
  }
  return a;
}

bool buf_sampler_t::check_single_filter(const buf_t& a, const buf_filter_t& filter,
                                        const std::vector<base_type_t>& filter_dependencies) {
  switch (filter) {
    case buf_filter_t::NOT_SAME_AS_1:
      return a != std::get<buf_t>(filter_dependencies[0]);
      break;
    default:
      break;
  }
  return false;
}
