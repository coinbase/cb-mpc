#include "mpc_data_generator.h"

namespace coinbase::test {

template <>
config_map_t input_generator_t<ecdsa2pc_sign_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["pai_p1"] = paillier_config_t(DIST(paillier)::P_PRIME1024_Q_PRIME1024_0);
  config["pai_p2"] = paillier_config_t({DIST(paillier)::GET_PUB_FROM_PRIV_1, DEPEND("pai_p1")});
  config["x1"] = curve_random_scalar_config(curve);
  config["x2"] = curve_random_scalar_config(curve);
  config["Q1"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_MUL_G_1, DEPEND("x1")});
  config["Q2"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_MUL_G_1, DEPEND("x2")});
  config["Q"] = ecp_config_t(curve, {DIST(ecp)::SUM_2, DEPEND("Q1", "Q2")});
  config["r"] = bn_config_t({DIST(bn)::RAND_PAILLIER_N_1, DEPEND("pai_p1")});
  config["c"] = bn_config_t({DIST(bn)::PAILLIER_ENCRYPTION_3, DEPEND("pai_p1", "x1", "r")});
  config["sid"] = buf_config_t(DIST(buf)::RANDOM_16BYTES_0);
  return config;
}

template <>
ecdsa2pc_sign_input_t input_generator_t<ecdsa2pc_sign_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  ecdsa2pc_sign_input_t data;
  data.key1.role = coinbase::mpc::party_t::p1;
  data.key2.role = coinbase::mpc::party_t::p2;
  data.key1.curve = curve;
  data.key2.curve = curve;
  data.key1.paillier = std::get<coinbase::crypto::paillier_t>(input["pai_p1"]);
  data.key2.paillier = std::get<coinbase::crypto::paillier_t>(input["pai_p2"]);
  data.key1.x_share = std::get<bn_t>(input["x1"]);
  data.key2.x_share = std::get<bn_t>(input["x2"]);
  data.key1.Q = data.key2.Q = std::get<ecc_point_t>(input["Q"]);
  data.key1.c_key = data.key2.c_key = std::get<bn_t>(input["c"]);
  data.sid = std::get<buf_t>(input["sid"]);
  return data;
}

template <>
config_map_t input_generator_t<eddsa2pc_sign_batch_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["x1"] = curve_random_scalar_config(curve);
  config["x2"] = curve_random_scalar_config(curve);
  config["Q1"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_MUL_G_1, DEPEND("x1")});
  config["Q2"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_MUL_G_1, DEPEND("x2")});
  config["Q"] = ecp_config_t(curve, {DIST(ecp)::SUM_2, DEPEND("Q1", "Q2")});
  config["sid"] = buf_config_t(DIST(buf)::RANDOM_16BYTES_0);
  return config;
}

template <>
eddsa2pc_sign_batch_input_t input_generator_t<eddsa2pc_sign_batch_input_t>::generate(int size) {
  auto input = input_factory.generate(size);
  eddsa2pc_sign_batch_input_t data;
  data.keys.resize(size);
  data.sids.resize(size);
  for (int i = 0; i < size; ++i) {
    data.keys[i].resize(2);
    data.keys[i][0].role = coinbase::mpc::party_t::p1;
    data.keys[i][1].role = coinbase::mpc::party_t::p2;
    data.keys[i][0].curve = curve;
    data.keys[i][1].curve = curve;
    data.keys[i][0].x_share = std::get<bn_t>(input[i]["x1"]);
    data.keys[i][1].x_share = std::get<bn_t>(input[i]["x2"]);
    data.keys[i][0].Q = data.keys[i][1].Q = std::get<ecc_point_t>(input[i]["Q"]);
    data.sids[i] = std::get<buf_t>(input[i]["sid"]);
  }
  return data;
}

template <>
config_map_t input_generator_t<ecdsa_mp_msg1_input_t>::get_completeness_config(ecurve_t curve) {
  config_map_t config;
  config["P1_Qi"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_LESS_Q_0});
  config["P2_Qi"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_LESS_Q_0});
  config["P3_Qi"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_LESS_Q_0});
  config["P4_Qi"] = ecp_config_t(curve, {DIST(ecp)::SCALAR_LESS_Q_0});
  return config;
}

template <>
ecdsa_mp_msg1_input_t input_generator_t<ecdsa_mp_msg1_input_t>::generate(int size) {
  auto input = input_factory.generate_one();
  ecdsa_mp_msg1_input_t data;
  data.Qi.resize(4);
  data.Qi[0] = std::get<ecc_point_t>(input["P1_Qi"]);
  data.Qi[1] = std::get<ecc_point_t>(input["P2_Qi"]);
  data.Qi[2] = std::get<ecc_point_t>(input["P3_Qi"]);
  data.Qi[3] = std::get<ecc_point_t>(input["P4_Qi"]);
  return data;
}

}  // namespace coinbase::test