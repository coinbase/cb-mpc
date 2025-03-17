#pragma once
#include <unordered_map>
#include <variant>
#include <vector>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/elgamal.h>
#include <cbmpc/crypto/ro.h>

namespace coinbase::test {
template <typename T_DIST>
struct dist_config_t {
  T_DIST dist;
  std::vector<std::string> dependencies;
  dist_config_t() {}
  dist_config_t(T_DIST d, std::vector<std::string> dep = std::vector<std::string>()) : dist(d), dependencies(dep) {}
};

template <typename FILTER_T>
struct filter_config_t {
  FILTER_T filter;
  std::vector<std::string> dependencies;
  filter_config_t() {}
  filter_config_t(FILTER_T f, std::vector<std::string> dep = std::vector<std::string>())
      : filter(f), dependencies(dep) {}
};

template <typename T_DIST, typename FILTER_T>
struct config_t {
  dist_config_t<T_DIST> dist_config;
  std::vector<filter_config_t<FILTER_T>> filter_configs;
  config_t() {}
  config_t(dist_config_t<T_DIST> d_c,
           std::vector<filter_config_t<FILTER_T>> f_c = std::vector<filter_config_t<FILTER_T>>())
      : dist_config(d_c), filter_configs(f_c) {}
};

using base_type_t = std::variant<bn_t, ecc_point_t, coinbase::crypto::paillier_t, elg_com_t, buf_t>;

template <typename T, typename T_DIST, typename FILTER_T>
class sampler_base_t {
 protected:
  virtual bool check_single_filter(const T& a, const FILTER_T& filter,
                                   const std::vector<base_type_t>& filter_dependencies) = 0;
  virtual T sample(const T_DIST& dist, const std::vector<base_type_t>& dist_dependencies) = 0;
  bool check_filters(const T& a, const std::vector<std::pair<FILTER_T, std::vector<base_type_t>>>& filter_configs) {
    for (auto& [filter, filter_dependencies] : filter_configs) {
      if (!check_single_filter(a, filter, filter_dependencies)) return false;
    }
    return true;
  }

 public:
  sampler_base_t() = default;
  ~sampler_base_t() = default;
  T generate(const T_DIST& dist, const std::vector<base_type_t>& dist_dependencies,
             const std::vector<std::pair<FILTER_T, std::vector<base_type_t>>>& filter_configs) {
    T a = sample(dist, dist_dependencies);
    if (filter_configs.empty()) return a;
    while (!check_filters(a, filter_configs)) a = sample(dist, dist_dependencies);
    return a;
  }
};

template <typename T, typename T_DIST, typename FILTER_T>
class curved_sampler_base_t : public sampler_base_t<T, T_DIST, FILTER_T> {
 protected:
  T sample(const T_DIST& dist, const std::vector<base_type_t>& dist_dependencies) {
    cb_assert(false);
    return T();
  }
  virtual T sample(const T_DIST& dist, const coinbase::crypto::ecurve_t& curve,
                   const std::vector<base_type_t>& dist_dependencies) = 0;

 public:
  curved_sampler_base_t() = default;
  ~curved_sampler_base_t() = default;
  T generate(const T_DIST& dist, const coinbase::crypto::ecurve_t& curve,
             const std::vector<base_type_t>& dist_dependencies,
             const std::vector<std::pair<FILTER_T, std::vector<base_type_t>>>& filter_configs) {
    T a = sample(dist, curve, dist_dependencies);
    if (filter_configs.empty()) return a;
    while (!this->check_filters(a, filter_configs)) a = sample(dist, curve, dist_dependencies);
    return a;
  }
};
}  // namespace coinbase::test