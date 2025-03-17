#pragma once
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

#include "data/data_generator.h"

namespace coinbase::test {

struct ecdsa2pc_sign_input_t {
  mpc::ecdsa2pc::key_t key1, key2;
  buf_t sid;
};

template <>
config_map_t input_generator_t<ecdsa2pc_sign_input_t>::get_completeness_config(ecurve_t curve);

template <>
ecdsa2pc_sign_input_t input_generator_t<ecdsa2pc_sign_input_t>::generate(int size);

struct eddsa2pc_sign_batch_input_t {
  std::vector<std::vector<coinbase::mpc::eddsa2pc::key_t>> keys;
  std::vector<buf_t> sids;
};

template <>
config_map_t input_generator_t<eddsa2pc_sign_batch_input_t>::get_completeness_config(ecurve_t curve);

template <>
eddsa2pc_sign_batch_input_t input_generator_t<eddsa2pc_sign_batch_input_t>::generate(int size);

struct ecdsa_mp_msg1_input_t {
  std::vector<coinbase::crypto::ecc_point_t> Qi;
};

template <>
config_map_t input_generator_t<ecdsa_mp_msg1_input_t>::get_completeness_config(ecurve_t curve);

template <>
ecdsa_mp_msg1_input_t input_generator_t<ecdsa_mp_msg1_input_t>::generate(int size);

}  // namespace coinbase::test