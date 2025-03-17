#include "zk_data_generator.h"

namespace coinbase::test {

template <>
config_map_t input_generator_t<uc_dl_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["w"] = curve_random_scalar_config(curve);
  config["Q"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_MUL_G_1, DEPEND("w")});
  return config;
}

template <>
uc_dl_input_t input_generator_t<uc_dl_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  uc_dl_input_t data;
  data.w = std::get<bn_t>(input["w"]);
  data.Q = std::get<ecc_point_t>(input["Q"]);
  return data;
}

template <>
config_map_t input_generator_t<uc_batch_dl_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["w"] = curve_random_scalar_config(curve);
  config["Q"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_MUL_G_1, DEPEND("w")});
  return config;
}

template <>
uc_batch_dl_input_t input_generator_t<uc_batch_dl_input_t>::generate(int size) {
  auto input = input_factory.generate(size);
  uc_batch_dl_input_t data;
  data.ws.resize(size);
  data.Qs.resize(size);
  for (int i = 0; i < size; ++i) {
    data.ws[i] = std::get<bn_t>(input[i]["w"]);
    data.Qs[i] = std::get<ecc_point_t>(input[i]["Q"]);
  }
  return data;
}

template <>
config_map_t input_generator_t<dh_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["Q"] = ecp_config_t(curve, DIST(ecp)::SCALAR_LESS_Q_0);
  config["w"] = curve_random_scalar_config(curve);
  config["A"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_MUL_G_1, DEPEND("w")});
  config["B"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_MUL_POINT_2, DEPEND("w", "Q")});
  return config;
}

template <>
dh_input_t input_generator_t<dh_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  dh_input_t data;
  data.Q = std::get<ecc_point_t>(input["Q"]);
  data.w = std::get<bn_t>(input["w"]);
  data.A = std::get<ecc_point_t>(input["A"]);
  data.B = std::get<ecc_point_t>(input["B"]);
  return data;
}

template <>
config_map_t input_generator_t<valid_paillier_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["pai_p"] = paillier_config_t(DIST(paillier)::P_PRIME1024_Q_PRIME1024_0);
  config["pai_v"] = paillier_config_t({DIST(paillier)::GET_PUB_FROM_PRIV_1, DEPEND("pai_p")});
  return config;
}

template <>
valid_paillier_input_t input_generator_t<valid_paillier_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  valid_paillier_input_t data;
  data.p_p = std::get<coinbase::crypto::paillier_t>(input["pai_p"]);
  data.v_p = std::get<coinbase::crypto::paillier_t>(input["pai_v"]);
  return data;
}

template <>
config_map_t input_generator_t<paillier_zero_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["pai_p"] = paillier_config_t(DIST(paillier)::P_PRIME1024_Q_PRIME1024_0);
  config["pai_v"] = paillier_config_t({DIST(paillier)::GET_PUB_FROM_PRIV_1, DEPEND("pai_p")});
  config["r"] = bn_config_t({DIST(bn)::RAND_PAILLIER_N_1, DEPEND("pai_p")});
  config["m"] = bn_config_t(DIST(bn)::ZERO_0);
  config["c"] = bn_config_t({DIST(bn)::PAILLIER_ENCRYPTION_3, DEPEND("pai_p", "m", "r")});
  config["pid"] = bn_config_t(DIST(bn)::INT128_POS_0);
  return config;
}

template <>
paillier_zero_input_t input_generator_t<paillier_zero_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  paillier_zero_input_t data;
  data.p_p = std::get<coinbase::crypto::paillier_t>(input["pai_p"]);
  data.v_p = std::get<coinbase::crypto::paillier_t>(input["pai_v"]);
  data.r = std::get<bn_t>(input["r"]);
  data.c = std::get<bn_t>(input["c"]);
  data.pid = std::get<bn_t>(input["pid"]);
  return data;
}

template <>
config_map_t input_generator_t<two_paillier_equal_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["q"] = bn_config_t(DIST(bn)::PRIME256_0);
  config["p_p_1"] = paillier_config_t(DIST(paillier)::P_PRIME1024_Q_PRIME1024_0);
  config["v_p_1"] = paillier_config_t({DIST(paillier)::GET_PUB_FROM_PRIV_1, DEPEND("p_p_1")});
  config["r1"] = bn_config_t({DIST(bn)::RAND_PAILLIER_N_1, DEPEND("p_p_1")});
  config["p_p_2"] = paillier_config_t(DIST(paillier)::P_PRIME1024_Q_PRIME1024_0);
  config["v_p_2"] = paillier_config_t({DIST(paillier)::GET_PUB_FROM_PRIV_1, DEPEND("p_p_2")});
  config["r2"] = bn_config_t({DIST(bn)::RAND_PAILLIER_N_1, DEPEND("p_p_2")});
  config["x"] = bn_config_t({DIST(bn)::RAND_BN_1, DEPEND("q")});
  config["c1"] = bn_config_t({DIST(bn)::PAILLIER_ENCRYPTION_3, DEPEND("p_p_1", "x", "r1")});
  config["c2"] = bn_config_t({DIST(bn)::PAILLIER_ENCRYPTION_3, DEPEND("p_p_2", "x", "r2")});
  config["pid"] = bn_config_t(DIST(bn)::INT128_POS_0);
  return config;
}

template <>
two_paillier_equal_input_t input_generator_t<two_paillier_equal_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  two_paillier_equal_input_t data;
  data.q = std::get<bn_t>(input["q"]);
  data.p_p_1 = std::get<coinbase::crypto::paillier_t>(input["p_p_1"]);
  data.v_p_1 = std::get<coinbase::crypto::paillier_t>(input["v_p_1"]);
  data.r1 = std::get<bn_t>(input["r1"]);
  data.p_p_2 = std::get<coinbase::crypto::paillier_t>(input["p_p_2"]);
  data.v_p_2 = std::get<coinbase::crypto::paillier_t>(input["v_p_2"]);
  data.r2 = std::get<bn_t>(input["r2"]);
  data.x = std::get<bn_t>(input["x"]);
  data.c1 = std::get<bn_t>(input["c1"]);
  data.c2 = std::get<bn_t>(input["c2"]);
  data.pid = std::get<bn_t>(input["pid"]);
  return data;
}

template <>
config_map_t input_generator_t<two_paillier_equal_batch_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["q"] = bn_config_t(DIST(bn)::PRIME256_0);
  config["p_p_1"] = paillier_config_t(DIST(paillier)::P_PRIME1024_Q_PRIME1024_0);
  config["v_p_1"] = paillier_config_t({DIST(paillier)::GET_PUB_FROM_PRIV_1, DEPEND("p_p_1")});
  config["p_p_2"] = paillier_config_t(DIST(paillier)::P_PRIME1024_Q_PRIME1024_0);
  config["v_p_2"] = paillier_config_t({DIST(paillier)::GET_PUB_FROM_PRIV_1, DEPEND("p_p_2")});
  config["r1"] = bn_config_t({DIST(bn)::RAND_PAILLIER_N_1, DEPEND("p_p_1")});
  config["r2"] = bn_config_t({DIST(bn)::RAND_PAILLIER_N_1, DEPEND("p_p_2")});
  config["x"] = bn_config_t({DIST(bn)::RAND_BN_1, DEPEND("q")});
  config["c1"] = bn_config_t({DIST(bn)::PAILLIER_ENCRYPTION_3, DEPEND("p_p_1", "x", "r1")});
  config["c2"] = bn_config_t({DIST(bn)::PAILLIER_ENCRYPTION_3, DEPEND("p_p_2", "x", "r2")});
  return config;
}

template <>
two_paillier_equal_batch_input_t input_generator_t<two_paillier_equal_batch_input_t>::generate(int size) {
  auto inputs = input_factory.generate_one_batch(size, DEPEND("q", "p_p_1", "v_p_1", "p_p_2", "v_p_2"));
  two_paillier_equal_batch_input_t data;
  data.r1s.resize(size);
  data.r2s.resize(size);
  data.xs.resize(size);
  data.c1s.resize(size);
  data.c2s.resize(size);
  data.q = std::get<bn_t>(inputs[0]["q"]);
  data.p_p_1 = std::get<coinbase::crypto::paillier_t>(inputs[0]["p_p_1"]);
  data.v_p_1 = std::get<coinbase::crypto::paillier_t>(inputs[0]["v_p_1"]);
  data.p_p_2 = std::get<coinbase::crypto::paillier_t>(inputs[0]["p_p_2"]);
  data.v_p_2 = std::get<coinbase::crypto::paillier_t>(inputs[0]["v_p_2"]);
  data.r1s[0] = std::get<bn_t>(inputs[0]["r1"]);
  data.r2s[0] = std::get<bn_t>(inputs[0]["r2"]);
  data.xs[0] = std::get<bn_t>(inputs[0]["x"]);
  data.c1s[0] = std::get<bn_t>(inputs[0]["c1"]);
  data.c2s[0] = std::get<bn_t>(inputs[0]["c2"]);
  for (int i = 1; i < size; ++i) {
    data.r1s[i] = std::get<bn_t>(inputs[i]["r1"]);
    data.r2s[i] = std::get<bn_t>(inputs[i]["r2"]);
    data.xs[i] = std::get<bn_t>(inputs[i]["x"]);
    data.c1s[i] = std::get<bn_t>(inputs[i]["c1"]);
    data.c2s[i] = std::get<bn_t>(inputs[i]["c2"]);
  }
  return data;
}

template <>
config_map_t input_generator_t<elgamal_com_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["Q"] = ecp_config_t(curve, DIST(ecp)::SCALAR_LESS_Q_0);
  config["x"] = curve_random_scalar_config(curve);
  config["r"] = curve_random_scalar_config(curve);
  config["UV"] = elgamal_config_t(curve, {DIST(elgamal)::ENCRYPTION_E_M_R_3, DEPEND("Q", "x", "r")});
  return config;
}

template <>
elgamal_com_input_t input_generator_t<elgamal_com_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  elgamal_com_input_t data;
  data.Q = std::get<ecc_point_t>(input["Q"]);
  data.x = std::get<bn_t>(input["x"]);
  data.r = std::get<bn_t>(input["r"]);
  data.UV = std::get<elg_com_t>(input["UV"]);
  return data;
}

template <>
config_map_t input_generator_t<elgamal_com_pub_share_equal_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["E"] = ecp_config_t(curve, DIST(ecp)::SCALAR_LESS_Q_0);
  config["A"] = ecp_config_t(curve, DIST(ecp)::SCALAR_LESS_Q_0);
  config["r_eA"] = curve_random_scalar_config(curve);
  config["eA"] = elgamal_config_t(curve, {DIST(elgamal)::ENCRYPTION_E_MG_R_3, DEPEND("E", "A", "r_eA")});
  return config;
}

template <>
elgamal_com_pub_share_equal_input_t input_generator_t<elgamal_com_pub_share_equal_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  elgamal_com_pub_share_equal_input_t data;
  data.E = std::get<ecc_point_t>(input["E"]);
  data.A = std::get<ecc_point_t>(input["A"]);
  data.r_eA = std::get<bn_t>(input["r_eA"]);
  data.eA = std::get<elg_com_t>(input["eA"]);
  return data;
}

template <>
config_map_t input_generator_t<elgamal_com_mult_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["E"] = ecp_config_t(curve, DIST(ecp)::SCALAR_LESS_Q_0);
  config["m"] = curve_random_scalar_config(curve);
  config["b"] = curve_random_scalar_config(curve);
  config["r_eB"] = curve_random_scalar_config(curve);
  config["r_eC"] = curve_random_scalar_config(curve);
  config["eA"] = elgamal_config_t(curve, {DIST(elgamal)::ENCRYPTION_E_M_2, DEPEND("E", "m")});
  config["eB"] = elgamal_config_t(curve, {DIST(elgamal)::ENCRYPTION_E_M_R_3, DEPEND("E", "b", "r_eB")});
  config["aux_algamal"] = elgamal_config_t(curve, {DIST(elgamal)::SCALAR_MUL_UV_1, DEPEND("b", "eA")});
  config["eC"] = elgamal_config_t(curve, {DIST(elgamal)::RERAND_3, DEPEND("aux_algamal", "E", "r_eC")});
  return config;
}

template <>
elgamal_com_mult_input_t input_generator_t<elgamal_com_mult_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  elgamal_com_mult_input_t data;
  data.E = std::get<ecc_point_t>(input["E"]);
  data.b = std::get<bn_t>(input["b"]);
  data.r_eB = std::get<bn_t>(input["r_eB"]);
  data.r_eC = std::get<bn_t>(input["r_eC"]);
  data.eA = std::get<elg_com_t>(input["eA"]);
  data.eB = std::get<elg_com_t>(input["eB"]);
  data.eC = std::get<elg_com_t>(input["eC"]);
  return data;
}

template <>
config_map_t input_generator_t<elgamal_com_mult_private_scalar_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["E"] = ecp_config_t(curve, DIST(ecp)::SCALAR_LESS_Q_0);
  config["m"] = curve_random_scalar_config(curve);
  config["c"] = curve_random_scalar_config(curve);
  config["r"] = curve_random_scalar_config(curve);
  config["eA"] = elgamal_config_t(curve, {DIST(elgamal)::ENCRYPTION_E_M_2, DEPEND("E", "m")});
  config["aux_elgamal"] = elgamal_config_t(curve, {DIST(elgamal)::SCALAR_MUL_UV_1, DEPEND("c", "eA")});
  config["eB"] = elgamal_config_t(curve, {DIST(elgamal)::RERAND_3, DEPEND("aux_elgamal", "E", "r")});
  return config;
}

template <>
elgamal_com_mult_private_scalar_input_t input_generator_t<elgamal_com_mult_private_scalar_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  elgamal_com_mult_private_scalar_input_t data;
  data.E = std::get<ecc_point_t>(input["E"]);
  data.c = std::get<bn_t>(input["c"]);
  data.r = std::get<bn_t>(input["r"]);
  data.eA = std::get<elg_com_t>(input["eA"]);
  data.eB = std::get<elg_com_t>(input["eB"]);
  return data;
}

template <>
config_map_t input_generator_t<nizk_pdl_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["pai_p"] = paillier_config_t(DIST(paillier)::P_PRIME1024_Q_PRIME1024_0);
  config["pai_v"] = paillier_config_t({DIST(paillier)::GET_PUB_FROM_PRIV_1, DEPEND("pai_p")});
  config["x1"] = curve_random_scalar_config(curve);
  config["Q1"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_MUL_G_1, DEPEND("x1")});
  config["r"] = bn_config_t({DIST(bn)::RAND_PAILLIER_N_1, DEPEND("pai_p")});
  config["c"] = bn_config_t({DIST(bn)::PAILLIER_ENCRYPTION_3, DEPEND("pai_p", "x1", "r")});
  return config;
}

template <>
nizk_pdl_input_t input_generator_t<nizk_pdl_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  nizk_pdl_input_t data;
  data.p_p = std::get<coinbase::crypto::paillier_t>(input["pai_p"]);
  data.v_p = std::get<coinbase::crypto::paillier_t>(input["pai_v"]);
  data.x1 = std::get<bn_t>(input["x1"]);
  data.Q1 = std::get<ecc_point_t>(input["Q1"]);
  data.r = std::get<bn_t>(input["r"]);
  data.c = std::get<bn_t>(input["c"]);
  return data;
}

template <>
config_map_t input_generator_t<range_pedersen_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["q"] = bn_config_t(DIST(bn)::PRIME256_0);
  config["x"] = bn_config_t({DIST(bn)::RAND_BN_1, DEPEND("q")});
  config["r"] = bn_config_t(DIST(bn)::RAND_PEDERSEN_PTAG_0);
  config["c"] = bn_config_t({DIST(bn)::PEDERSEN_COMMITMENT_2, DEPEND("x", "r")});
  return config;
}

template <>
range_pedersen_input_t input_generator_t<range_pedersen_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  range_pedersen_input_t data;
  data.q = std::get<bn_t>(input["q"]);
  data.x = std::get<bn_t>(input["x"]);
  data.r = std::get<bn_t>(input["r"]);
  data.c = std::get<bn_t>(input["c"]);
  return data;
}

template <>
config_map_t input_generator_t<paillier_pedersen_equal_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["q"] = bn_config_t(DIST(bn)::PRIME256_0);
  config["pai_p"] = paillier_config_t(DIST(paillier)::P_PRIME1024_Q_PRIME1024_0);
  config["pai_v"] = paillier_config_t({DIST(paillier)::GET_PUB_FROM_PRIV_1, DEPEND("pai_p")});
  config["x"] = bn_config_t({DIST(bn)::RAND_BN_1, DEPEND("q")});
  config["r"] = bn_config_t({DIST(bn)::RAND_PAILLIER_N_1, DEPEND("pai_p")});
  config["c"] = bn_config_t({DIST(bn)::PAILLIER_ENCRYPTION_3, DEPEND("pai_p", "x", "r")});
  config["rho"] = bn_config_t(DIST(bn)::RAND_PEDERSEN_PTAG_0);
  config["com"] = bn_config_t({DIST(bn)::PEDERSEN_COMMITMENT_2, DEPEND("x", "rho")});
  config["pid"] = bn_config_t(DIST(bn)::INT128_POS_0);
  return config;
}

template <>
paillier_pedersen_equal_input_t input_generator_t<paillier_pedersen_equal_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  paillier_pedersen_equal_input_t data;
  data.q = std::get<bn_t>(input["q"]);
  data.p_p = std::get<coinbase::crypto::paillier_t>(input["pai_p"]);
  data.v_p = std::get<coinbase::crypto::paillier_t>(input["pai_v"]);
  data.x = std::get<bn_t>(input["x"]);
  data.r = std::get<bn_t>(input["r"]);
  data.c = std::get<bn_t>(input["c"]);
  data.rho = std::get<bn_t>(input["rho"]);
  data.Com = std::get<bn_t>(input["com"]);
  data.pid = std::get<bn_t>(input["pid"]);
  return data;
}

template <>
config_map_t input_generator_t<paillier_range_exp_slack_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["q"] = bn_config_t(DIST(bn)::PRIME256_0);
  config["pai_p"] = paillier_config_t(DIST(paillier)::P_PRIME1024_Q_PRIME1024_0);
  config["pai_v"] = paillier_config_t({DIST(paillier)::GET_PUB_FROM_PRIV_1, DEPEND("pai_p")});
  config["x"] = bn_config_t({DIST(bn)::RAND_BN_1, DEPEND("q")});
  config["r"] = bn_config_t({DIST(bn)::RAND_PAILLIER_N_1, DEPEND("pai_p")});
  config["c"] = bn_config_t({DIST(bn)::PAILLIER_ENCRYPTION_3, DEPEND("pai_p", "x", "r")});
  return config;
}

template <>
paillier_range_exp_slack_input_t input_generator_t<paillier_range_exp_slack_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  paillier_range_exp_slack_input_t data;
  data.q = std::get<bn_t>(input["q"]);
  data.p_p = std::get<coinbase::crypto::paillier_t>(input["pai_p"]);
  data.v_p = std::get<coinbase::crypto::paillier_t>(input["pai_v"]);
  data.x = std::get<bn_t>(input["x"]);
  data.r = std::get<bn_t>(input["r"]);
  data.c = std::get<bn_t>(input["c"]);
  return data;
}

template <>
config_map_t input_generator_t<batch_pedersen_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["H"] = ecp_config_t(curve, DIST(ecp)::SCALAR_LESS_Q_0);
  config["x"] = curve_random_scalar_config(curve);
  config["r"] = curve_random_scalar_config(curve);
  config["c"] = ecp_config_t(curve, {DIST(ecp)::PEDERSEN_COMMITMENT_3, DEPEND("x", "r", "H")});
  return config;
}

template <>
batch_pedersen_input_t input_generator_t<batch_pedersen_input_t>::generate(int size) {
  auto inputs = input_factory.generate_one_batch(size, DEPEND("H"));
  batch_pedersen_input_t data;
  data.xs.resize(size);
  data.rs.resize(size);
  data.Cs.resize(size);
  data.H = std::get<ecc_point_t>(inputs[0]["H"]);
  data.xs[0] = std::get<bn_t>(inputs[0]["x"]);
  data.rs[0] = std::get<bn_t>(inputs[0]["r"]);
  data.Cs[0] = std::get<ecc_point_t>(inputs[0]["c"]);
  for (int i = 1; i < size; ++i) {
    data.xs[i] = std::get<bn_t>(inputs[i]["x"]);
    data.rs[i] = std::get<bn_t>(inputs[i]["r"]);
    data.Cs[i] = std::get<ecc_point_t>(inputs[i]["c"]);
  }
  return data;
}

template <>
config_map_t input_generator_t<unknown_order_dl_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["N"] = bn_config_t(DIST(bn)::RSA_2048_N_0);
  config["a"] = bn_config_t({DIST(bn)::RAND_BN_1, DEPEND("N")});
  config["w"] = bn_config_t({DIST(bn)::RAND_BN_1, DEPEND("N")});
  config["b"] = bn_config_t({DIST(bn)::POWER_MOD_3, DEPEND("a", "w", "N")});
  return config;
}
template <>
unknown_order_dl_input_t input_generator_t<unknown_order_dl_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  unknown_order_dl_input_t data;
  data.N = std::get<bn_t>(input["N"]);
  data.a = std::get<bn_t>(input["a"]);
  data.w = std::get<bn_t>(input["w"]);
  data.b = std::get<bn_t>(input["b"]);
  return data;
}

}  // namespace coinbase::test