#pragma once
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

#include "data/data_generator.h"

namespace coinbase::test {

struct zk_base_input_t {
  uint64_t aux = 0;
  buf_t sid = coinbase::crypto::gen_random(16);
};

struct uc_dl_input_t : zk_base_input_t {
  bn_t w;
  ecc_point_t Q;
};

template <>
config_map_t input_generator_t<uc_dl_input_t>::get_completeness_config(ecurve_t curve);

template <>
uc_dl_input_t input_generator_t<uc_dl_input_t>::generate(int size);

struct dh_input_t : zk_base_input_t {
  bn_t w;
  ecc_point_t Q, A, B;
};

template <>
config_map_t input_generator_t<dh_input_t>::get_completeness_config(ecurve_t curve);

template <>
dh_input_t input_generator_t<dh_input_t>::generate(int size);

struct uc_batch_dl_input_t : zk_base_input_t {
  std::vector<bn_t> ws;
  std::vector<ecc_point_t> Qs;
};

template <>
config_map_t input_generator_t<uc_batch_dl_input_t>::get_completeness_config(ecurve_t curve);

template <>
uc_batch_dl_input_t input_generator_t<uc_batch_dl_input_t>::generate(int size);

struct valid_paillier_input_t : zk_base_input_t {
  coinbase::crypto::paillier_t p_p, v_p;
};

template <>
input_generator_t<valid_paillier_input_t>::input_generator_t(ecurve_t curve) = delete;

template <>
config_map_t input_generator_t<valid_paillier_input_t>::get_completeness_config(ecurve_t curve);

template <>
valid_paillier_input_t input_generator_t<valid_paillier_input_t>::generate(int size);

struct paillier_zero_input_t : zk_base_input_t {
  bn_t pid;
  coinbase::crypto::paillier_t p_p, v_p;
  bn_t r, c;
};

template <>
input_generator_t<paillier_zero_input_t>::input_generator_t(ecurve_t curve) = delete;

template <>
config_map_t input_generator_t<paillier_zero_input_t>::get_completeness_config(ecurve_t curve);

template <>
paillier_zero_input_t input_generator_t<paillier_zero_input_t>::generate(int size);

struct two_paillier_equal_input_t : zk_base_input_t {
  bn_t pid;
  bn_t q, r1, r2, x, c1, c2;
  coinbase::crypto::paillier_t p_p_1, v_p_1, p_p_2, v_p_2;
};

template <>
input_generator_t<two_paillier_equal_input_t>::input_generator_t(ecurve_t curve) = delete;

template <>
config_map_t input_generator_t<two_paillier_equal_input_t>::get_completeness_config(ecurve_t curve);

template <>
two_paillier_equal_input_t input_generator_t<two_paillier_equal_input_t>::generate(int size);

struct two_paillier_equal_batch_input_t : zk_base_input_t {
  bn_t q;
  coinbase::crypto::paillier_t p_p_1, v_p_1, p_p_2, v_p_2;
  std::vector<bn_t> r1s, r2s, xs, c1s, c2s;
};

template <>
input_generator_t<two_paillier_equal_batch_input_t>::input_generator_t(ecurve_t curve) = delete;

template <>
config_map_t input_generator_t<two_paillier_equal_batch_input_t>::get_completeness_config(ecurve_t curve);
template <>
two_paillier_equal_batch_input_t input_generator_t<two_paillier_equal_batch_input_t>::generate(int size);

struct elgamal_com_input_t : zk_base_input_t {
  ecc_point_t Q;
  bn_t x, r;
  elg_com_t UV;
};

template <>
config_map_t input_generator_t<elgamal_com_input_t>::get_completeness_config(ecurve_t curve);

template <>
elgamal_com_input_t input_generator_t<elgamal_com_input_t>::generate(int size);

struct elgamal_com_pub_share_equal_input_t : zk_base_input_t {
  ecc_point_t E, A;
  bn_t r_eA;
  elg_com_t eA;
};

template <>
config_map_t input_generator_t<elgamal_com_pub_share_equal_input_t>::get_completeness_config(ecurve_t curve);

template <>
elgamal_com_pub_share_equal_input_t input_generator_t<elgamal_com_pub_share_equal_input_t>::generate(int size);

struct elgamal_com_mult_input_t : zk_base_input_t {
  ecc_point_t E;
  bn_t b, r_eB, r_eC;
  elg_com_t eA, eB, eC;
};

template <>
config_map_t input_generator_t<elgamal_com_mult_input_t>::get_completeness_config(ecurve_t curve);

template <>
elgamal_com_mult_input_t input_generator_t<elgamal_com_mult_input_t>::generate(int size);

struct elgamal_com_mult_private_scalar_input_t : zk_base_input_t {
  ecc_point_t E;
  bn_t c, r;
  elg_com_t eA, eB;
};

template <>
config_map_t input_generator_t<elgamal_com_mult_private_scalar_input_t>::get_completeness_config(ecurve_t curve);

template <>
elgamal_com_mult_private_scalar_input_t input_generator_t<elgamal_com_mult_private_scalar_input_t>::generate(int size);

struct nizk_pdl_input_t : zk_base_input_t {
  coinbase::crypto::paillier_t p_p, v_p;
  ecc_point_t Q1;
  bn_t x1, r, c;
};

template <>
config_map_t input_generator_t<nizk_pdl_input_t>::get_completeness_config(ecurve_t curve);

template <>
nizk_pdl_input_t input_generator_t<nizk_pdl_input_t>::generate(int size);

struct range_pedersen_input_t : zk_base_input_t {
  bn_t q, c, x, r;
};

template <>
config_map_t input_generator_t<range_pedersen_input_t>::get_completeness_config(ecurve_t curve);

template <>
range_pedersen_input_t input_generator_t<range_pedersen_input_t>::generate(int size);

struct paillier_pedersen_equal_input_t : zk_base_input_t {
  bn_t pid;
  coinbase::crypto::paillier_t p_p, v_p;
  bn_t q, g, h, c, Com, x, r, rho;
};

template <>
config_map_t input_generator_t<paillier_pedersen_equal_input_t>::get_completeness_config(ecurve_t curve);

template <>
paillier_pedersen_equal_input_t input_generator_t<paillier_pedersen_equal_input_t>::generate(int size);

struct paillier_range_exp_slack_input_t : zk_base_input_t {
  coinbase::crypto::paillier_t p_p, v_p;
  bn_t q, x, r, c;
};

template <>
config_map_t input_generator_t<paillier_range_exp_slack_input_t>::get_completeness_config(ecurve_t curve);

template <>
paillier_range_exp_slack_input_t input_generator_t<paillier_range_exp_slack_input_t>::generate(int size);

struct batch_pedersen_input_t : zk_base_input_t {
  ecc_point_t H;
  std::vector<bn_t> xs;
  std::vector<bn_t> rs;
  std::vector<ecc_point_t> Cs;
};

template <>
config_map_t input_generator_t<batch_pedersen_input_t>::get_completeness_config(ecurve_t curve);

template <>
batch_pedersen_input_t input_generator_t<batch_pedersen_input_t>::generate(int size);

struct unknown_order_dl_input_t : zk_base_input_t {
  bn_t a, b, w, N;
};

template <>
input_generator_t<unknown_order_dl_input_t>::input_generator_t(ecurve_t curve) = delete;

template <>
config_map_t input_generator_t<unknown_order_dl_input_t>::get_completeness_config(ecurve_t curve);

template <>
unknown_order_dl_input_t input_generator_t<unknown_order_dl_input_t>::generate(int size);

}  // namespace coinbase::test