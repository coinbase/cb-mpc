#pragma once
#include <cbmpc/crypto/base.h>

namespace coinbase::mpc {

struct hd_root_t {
  bn_t x_share;
  bn_t k_share;
  ecc_point_t Q;
  ecc_point_t K;
  ecc_point_t Q_share() const { return Q.get_curve().mul_to_generator(x_share); }
  ecc_point_t K_share() const { return K.get_curve().mul_to_generator(k_share); }
  ecc_point_t other_Q_share() const { return Q - Q_share(); }
  ecc_point_t other_K_share() const { return K - K_share(); }

  void convert(coinbase::converter_t& converter) {
    x_share.convert(converter);
    converter.convert(Q);
    k_share.convert(converter);
    converter.convert(K);
  }
};

class bip32_path_t {
 public:
  bip32_path_t() {}
  bip32_path_t(const uint32_t* p, int n) : indices(p, p + n) {}

  void convert(coinbase::converter_t& converter) { converter.convert(indices); }

  bool operator==(const bip32_path_t& other) const { return indices == other.indices; }

  std::size_t hash() const {
    std::size_t seed = indices.size();
    for (auto& i : indices) seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }

  void append(uint32_t index) { indices.push_back(index); }
  int count() const { return (int)indices.size(); }
  uint32_t operator[](int i) const { return indices[i]; }
  bool empty() const { return indices.empty(); }
  const uint32_t* get_indices() const { return indices.data(); }
  const std::vector<uint32_t>& get() const { return indices; }

  static bool has_duplicate(const std::vector<bip32_path_t>& paths);

 private:
  std::vector<uint32_t> indices;
};

std::vector<bn_t> non_hard_derive(const ecc_point_t& Q, mem_t chain_code, const std::vector<bip32_path_t>& paths);

}  // namespace coinbase::mpc

namespace std {
template <>
struct hash<coinbase::mpc::bip32_path_t> {
  std::size_t operator()(const coinbase::mpc::bip32_path_t& k) const { return k.hash(); }
};
}  // namespace std