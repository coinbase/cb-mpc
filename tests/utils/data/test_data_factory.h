#pragma once
#include <queue>
#include <unordered_map>
#include <variant>
#include <vector>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_ecc.h>
#include <cbmpc/crypto/ro.h>
#include <cbmpc/protocol/ecdsa_2p.h>
#include <cbmpc/protocol/eddsa.h>
#include <cbmpc/protocol/mpc_job.h>

#include "sampler/bn.h"
#include "sampler/buf.h"
#include "sampler/ecp.h"
#include "sampler/elgamal.h"
#include "sampler/paillier.h"

namespace coinbase::test {

using param_config_t = std::variant<bn_config_t, ecp_config_t, paillier_config_t, elgamal_config_t, buf_config_t>;
using config_map_t = std::unordered_map<std::string, param_config_t>;
using data_map_t = std::unordered_map<std::string, base_type_t>;

class test_data_factory_t {
 private:
  config_map_t configs;
  void generate_helper(data_map_t& data_map);
  bool generate_data_for_param(const std::string& param, data_map_t& data_map);

 public:
  test_data_factory_t() = default;
  test_data_factory_t(config_map_t c) : configs(c){};
  ~test_data_factory_t() = default;
  data_map_t generate_one();
  std::vector<data_map_t> generate(int repeats);
  std::vector<data_map_t> generate_one_batch(int repeats, std::vector<std::string> fixed_params);
};

template <typename T, typename CONFIG_T, typename FILTER_T>
class generate_single_data_helper_base {
 protected:
  static bool config_helper(const CONFIG_T& config, const data_map_t& data_map,
                            std::vector<base_type_t>& dist_dependencies,
                            std::vector<std::pair<FILTER_T, std::vector<base_type_t>>>& filter_configs) {
    for (auto param : config.dist_config.dependencies) {
      auto it = data_map.find(param);
      if (it == data_map.end()) return false;
      dist_dependencies.push_back(it->second);
    }

    for (const auto& fc : config.filter_configs) {
      std::vector<base_type_t> config_dependencies;
      for (auto param : fc.dependencies) {
        auto it = data_map.find(param);
        if (it == data_map.end()) return false;
        config_dependencies.push_back(it->second);
      }
      filter_configs.push_back(std::pair<FILTER_T, std::vector<base_type_t>>(fc.filter, config_dependencies));
    }
    return true;
  }
};

template <typename T, typename SAMPLER_T, typename CONFIG_T, typename FILTER_T>
class generate_single_data_helper : public generate_single_data_helper_base<T, CONFIG_T, FILTER_T> {
 public:
  static bool generate(const CONFIG_T& config, data_map_t& data_map, const std::string param) {
    SAMPLER_T sampler;
    std::vector<base_type_t> dist_dependencies;
    std::vector<std::pair<FILTER_T, std::vector<base_type_t>>> filter_configs;
    if (generate_single_data_helper_base<T, CONFIG_T, FILTER_T>::config_helper(config, data_map, dist_dependencies,
                                                                               filter_configs)) {
      data_map[param] = sampler.generate(config.dist_config.dist, dist_dependencies, filter_configs);
      return true;
    } else
      return false;
  }
};

template <typename T, typename SAMPLER_T, typename CONFIG_T, typename FILTER_T>
class generate_single_data_curve_helper : public generate_single_data_helper_base<T, CONFIG_T, FILTER_T> {
 public:
  static bool generate(const CONFIG_T& config, data_map_t& data_map, const std::string param) {
    SAMPLER_T sampler;
    std::vector<base_type_t> dist_dependencies;
    std::vector<std::pair<FILTER_T, std::vector<base_type_t>>> filter_configs;
    if (generate_single_data_helper_base<T, CONFIG_T, FILTER_T>::config_helper(config, data_map, dist_dependencies,
                                                                               filter_configs)) {
      data_map[param] = sampler.generate(config.dist_config.dist, config.curve, dist_dependencies, filter_configs);
      return true;
    } else
      return false;
  }
};

#define DIST(t) t##_distribution_t
#define DEPEND(...) \
  std::vector<std::string> { __VA_ARGS__ }

}  // namespace coinbase::test