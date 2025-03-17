#include "test_data_factory.h"
using namespace coinbase::test;

// ------------------------------------------------------
// New helper method for generating data for a single param
// ------------------------------------------------------
bool test_data_factory_t::generate_data_for_param(const std::string& param, data_map_t& data_map) {
  const auto& config_variant = configs.at(param);

  if (std::holds_alternative<bn_config_t>(config_variant)) {
    return generate_single_data_helper<bn_t, bn_sampler_t, bn_config_t, bn_filter_t>::generate(
        std::get<bn_config_t>(config_variant), data_map, param);
  } else if (std::holds_alternative<ecp_config_t>(config_variant)) {
    return generate_single_data_curve_helper<ecc_point_t, ecp_sampler_t, ecp_config_t, ecp_filter_t>::generate(
        std::get<ecp_config_t>(config_variant), data_map, param);
  } else if (std::holds_alternative<paillier_config_t>(config_variant)) {
    return generate_single_data_helper<coinbase::crypto::paillier_t, paillier_sampler_t, paillier_config_t,
                                       paillier_filter_t>::generate(std::get<paillier_config_t>(config_variant),
                                                                    data_map, param);
  } else if (std::holds_alternative<elgamal_config_t>(config_variant)) {
    return generate_single_data_curve_helper<elg_com_t, elgamal_sampler_t, elgamal_config_t,
                                             elgamal_filter_t>::generate(std::get<elgamal_config_t>(config_variant),
                                                                         data_map, param);
  } else if (std::holds_alternative<buf_config_t>(config_variant)) {
    return generate_single_data_helper<buf_t, buf_sampler_t, buf_config_t, buf_filter_t>::generate(
        std::get<buf_config_t>(config_variant), data_map, param);
  }

  // Fallback (if there's a type we don't handle, we can just return true or false)
  return true;
}

// ------------------------------------------------------
// Refactored generate_helper to use the new helper
// ------------------------------------------------------
void test_data_factory_t::generate_helper(data_map_t& data_map) {
  crypto::vartime_scope_t v;
  std::queue<std::string> params;

  // Use structured bindings, pushing only params that are missing from data_map
  for (auto&& [paramName, configVariant] : configs) {
    if (data_map.find(paramName) == data_map.end()) {
      params.push(paramName);
    }
  }

  // Process params until all are generated
  while (!params.empty()) {
    std::string paramName = params.front();
    params.pop();

    if (!generate_data_for_param(paramName, data_map)) {
      // If it fails, requeue for another pass
      params.push(paramName);
    }
  }
}

data_map_t test_data_factory_t::generate_one() {
  data_map_t data_map;
  generate_helper(data_map);
  return data_map;
}

std::vector<data_map_t> test_data_factory_t::generate(int repeats) {
  std::vector<data_map_t> data_matrix;
  for (int i = 0; i < repeats; ++i) data_matrix.push_back(generate_one());
  return data_matrix;
}

std::vector<data_map_t> test_data_factory_t::generate_one_batch(int repeats, std::vector<std::string> fixed_params) {
  std::vector<data_map_t> data_matrix;
  auto base = generate_one();
  data_matrix.push_back(base);
  data_map_t data;
  for (auto param : fixed_params) {
    data[param] = base[param];
  }
  for (int i = 1; i < repeats; ++i) {
    auto data_copy = data;
    generate_helper(data_copy);
    data_matrix.push_back(data_copy);
  }
  return data_matrix;
}