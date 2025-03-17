#include <gtest/gtest.h>

#include <cbmpc/protocol/int_commitment.h>
#include <cbmpc/zk/small_primes.h>
#include <cbmpc/zk/zk_paillier.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

constexpr int t = zk::paillier_non_interactive_param_t::t;

TEST(IntegerCommitment, Parameters) {
  const unknown_order_pedersen_params_t& params = unknown_order_pedersen_params_t::get();

  zk::unknown_order_dl_t unknown_order;
  unknown_order.e = bn_t::from_string(params.e_str).to_bin();
  for (int i = 0; i < SEC_P_COM; i++) unknown_order.z[i] = bn_t::from_string(params.z_str[i]);
  ASSERT_OK(params.N.get_bin_size() >= 2048);
  ASSERT_OK(check_open_range(0, params.h, params.N));
  ASSERT_OK(check_open_range(0, params.g, params.N));
  ASSERT_OK(unknown_order.verify(params.h, params.g, params.N, params.N.get_bits_count() + SEC_P_STAT, params.sid, 0));
}

TEST(IntegerCommitment, GenerateParameters) {
  const unknown_order_pedersen_params_t& params = unknown_order_pedersen_params_t::generate();

  zk::unknown_order_dl_t unknown_order;
  unknown_order.e = bn_t::from_string(params.e_str).to_bin();
  for (int i = 0; i < SEC_P_COM; i++) unknown_order.z[i] = bn_t::from_string(params.z_str[i]);
  ASSERT_OK(params.N.get_bin_size() >= 2048);
  ASSERT_OK(check_open_range(0, params.h, params.N));
  ASSERT_OK(check_open_range(0, params.g, params.N));
  ASSERT_OK(unknown_order.verify(params.h, params.g, params.N, params.N.get_bits_count() + SEC_P_STAT, params.sid, 0));
}

}  // namespace