
#pragma once
#include <cbmpc/crypto/base.h>

namespace coinbase::crypto {

class ecurve_secp256k1_t final : public ecurve_interface_t {
 public:
  ecurve_secp256k1_t() noexcept;
  const mod_t& order() const override;
  const mod_t& p() const override;
  const ecc_generator_point_t& generator() const override;
  void get_params(bn_t& p, bn_t& a, bn_t& b) const override;
  void init_point(ecc_point_t& P) const override;
  void free_point(ecc_point_t& P) const override;
  void copy_point(ecc_point_t& Dst, const ecc_point_t& Src) const override;
  bool is_on_curve(const ecc_point_t& P) const override;
  bool is_in_subgroup(const ecc_point_t& P) const override;
  bool is_infinity(const ecc_point_t& P) const override;
  void set_infinity(ecc_point_t& P) const override;
  bool equ_points(const ecc_point_t& P1, const ecc_point_t& P2) const override;
  void set_ossl_point(ecc_point_t& P, const EC_POINT* point) const override;

  void invert_point(ecc_point_t& P) const override;
  void add(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const override;
  void add_consttime(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const override;
  void mul(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const override;
  void mul_vartime(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const override;
  void mul_to_generator(const bn_t& val, ecc_point_t& P) const override;
  void mul_to_generator_vartime(const bn_t& val, ecc_point_t& P) const override;
  void mul_add(const bn_t& n, const ecc_point_t& P, const bn_t& m, ecc_point_t& R) const override;  // R = G*n + P*m

  int to_compressed_bin(const ecc_point_t& P, byte_ptr out) const override;
  int to_bin(const ecc_point_t& P, byte_ptr out) const override;
  error_t from_bin(ecc_point_t& P, mem_t bin) const override;
  void get_coordinates(const ecc_point_t& P, bn_t& x, bn_t& y) const override;
  void set_coordinates(ecc_point_t& P, const bn_t& x, const bn_t& y) const override;
  bool hash_to_point(mem_t bin, ecc_point_t& Q) const override;

  buf_t pub_to_der(const ecc_pub_key_t& P) const override;
  buf_t prv_to_der(const ecc_prv_key_t& K) const override;
  error_t pub_from_der(ecc_pub_key_t& P, mem_t der) const override;
  error_t prv_from_der(ecc_prv_key_t& K, mem_t der) const override;
  error_t verify(const ecc_pub_key_t& P, mem_t hash, mem_t sig) const override;
  buf_t sign(const ecc_prv_key_t& K, mem_t hash) const override;

 private:
  mod_t q;
};

namespace bip340 {
bn_t hash_message(const bn_t& rx, const ecc_point_t& pub_key, mem_t message);
error_t verify(const ecc_point_t& pub_key, mem_t m, mem_t sig);
}  // namespace bip340

}  // namespace coinbase::crypto
