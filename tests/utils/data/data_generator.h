#pragma once

#include "data/test_data_factory.h"

namespace coinbase::test {

template <typename T_INPUT>
struct input_generator_t {
  config_map_t config;
  test_data_factory_t input_factory;
  ecurve_t curve;
  input_generator_t(config_map_t cf = config_map_t(), ecurve_t cv = coinbase::crypto::curve_ed25519)
      : config(cf), curve(cv) {
    if (config.empty()) config = get_completeness_config();
    input_factory = test_data_factory_t(config);
  }
  input_generator_t(ecurve_t c) {
    curve = c;
    config = get_completeness_config(curve);
    input_factory = test_data_factory_t(config);
  }
  static config_map_t get_completeness_config(ecurve_t curve = coinbase::crypto::curve_ed25519) {
    return config_map_t{};
  }
  T_INPUT generate(int size = 0) { return T_INPUT(); }
  std::vector<T_INPUT> generate_batch(const int n, int size = 0)
  // n: batch size (number of repeats on the same config)
  // (optional) size: size of the vector in certain protocols that require a single input to be a vector
  {
    std::vector<T_INPUT> inputs;
    inputs.reserve(n);
    for (int i = 0; i < n; ++i) {
      inputs.push_back(generate(size));
    }
    return inputs;
  }
};

bn_config_t curve_random_scalar_config(ecurve_t c);
}  // namespace coinbase::test