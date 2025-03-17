#include "hd_tree_bip32.h"

namespace coinbase::mpc {

std::vector<bn_t> non_hard_derive(const ecc_point_t& Q, mem_t chain_code, const std::vector<bip32_path_t>& paths) {
  int n_paths = (int)paths.size();
  ecurve_t curve = Q.get_curve();
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  std::vector<bn_t> delta(n_paths);

  for (int i = 0; i < n_paths; i++) {
    const bip32_path_t& path = paths[i];
    buf_t chain_code_temp = chain_code;
    ecc_point_t Q_temp = Q;

    for (int j = 0; j < path.count(); j++) {
      uint32_t index = path[j];
      buf_t I = coinbase::crypto::hmac_sha512_t(chain_code_temp).calculate(Q_temp, index);
      bn_t x_temp = bn_t::from_bin(I.range(0, 32)) % q;
      chain_code_temp = I.range(32, 32);
      ecc_point_t x_temp_G = x_temp * G;
      Q_temp += x_temp_G;
      MODULO(q) delta[i] += x_temp;
    }
  }
  return delta;
}

bool bip32_path_t::has_duplicate(const std::vector<bip32_path_t>& paths) {
  std::unordered_set<bip32_path_t> set;

  for (const bip32_path_t& path : paths) {
    if (set.find(path) != set.end()) return true;
    set.insert(path);
  }

  return false;
}

}  // namespace coinbase::mpc