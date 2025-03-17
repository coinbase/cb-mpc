#include <fstream>
#include <gtest/gtest.h>

#define DUDECT_IMPLEMENTATION
#include "dudect_util/dudect_implementation.h"

namespace coinbase::dudect {

#define SECRET_LEN_BYTES (2)
#define NUMBER_MEASUREMENTS (250)
#define NUMBER_OPERANDS (2)
bn_t bn_arr[NUMBER_OPERANDS * NUMBER_MEASUREMENTS];
bn_t base_bn_a;
bn_t base_bn_b;
ecurve_t curve;
mod_t q;
mod_t small_q;

void generate_bn_array(uint8_t c, uint16_t idx) {
  // Creates random value for non-control group, sets fixed value for control group
  uint16_t start_idx = NUMBER_OPERANDS * idx;
  if (c == 1) {
    bn_arr[start_idx] = denormalize(bn_t::rand(q), q);
    bn_arr[start_idx + 1] = denormalize(bn_t::rand(q), q);
  } else {
    bn_arr[start_idx] = denormalize(base_bn_a, q);
    bn_arr[start_idx + 1] = denormalize(base_bn_b, q);
  }
}
uint8_t test_mod(uint8_t* data) {
  bn_t r;
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  MODULO(q) { bn_arr[start_idx]; }
  return 0;
}

uint8_t test_mod_neg(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  bn_t res = q.neg(bn_arr[start_idx]);
  return 0;
}
uint8_t test_mod_add(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  MODULO(q) { bn_arr[start_idx] + bn_arr[start_idx + 1]; }
  return 0;
}

uint8_t test_mod_sub(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  bn_t res = q.sub(bn_arr[start_idx], bn_arr[start_idx + 1]);
  return 0;
}
uint8_t test_mod_mul(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  bn_t res = q.mul(bn_arr[start_idx], bn_arr[start_idx + 1]);
  return 0;
}
uint8_t test_pow_mod(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  bn_t res = q.pow(bn_arr[start_idx], bn_arr[start_idx + 1]);
  return 0;
}
uint8_t test_mod_inv(uint8_t* data) {
  bn_t r;
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  bn_t res = q.inv(bn_arr[start_idx]);
  return 0;
}
uint8_t test_mod_inv_scr(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  bn_t res = q.inv(bn_arr[start_idx], mod_t::inv_algo_e::SCR);
  return 0;
}
uint8_t test_coprime(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  mod_t::coprime(bn_arr[start_idx], q);
  return 0;
}
uint8_t test_mod_rand(uint8_t* data) {
  bn_t res = q.rand();
  return 0;
}

void run_dudect_leakage_test(dudect_state_t expected_state, uint16_t baseline_bitlen) {
  coinbase::dudect::input_generator = generate_bn_array;
  curve = coinbase::crypto::curve_secp256k1;
  q = curve.order();
  small_q = bn_t::generate_prime(baseline_bitlen, true);
  base_bn_a = bn_t::rand(small_q);
  base_bn_b = bn_t::rand(small_q);

  dudect_config_t config = {
      .chunk_size = SECRET_LEN_BYTES,
      .number_measurements = NUMBER_MEASUREMENTS,
  };
  dudect_ctx_t ctx;
  dudect_init(&ctx, &config);

  dudect_state_t state = DUDECT_NO_LEAKAGE_EVIDENCE_YET;

  auto start = std::chrono::steady_clock::now();
  bool enough_measurements = false;
  bool measurement_threshold = true;
  std::ofstream base_csv("base_histogram.csv", std::ofstream::out | std::ofstream::trunc);
  std::ofstream var_csv("var_histogram.csv", std::ofstream::out | std::ofstream::trunc);
  base_csv << "ExecTime\n";
  var_csv << "ExecTime\n";
  while ((state == DUDECT_NO_LEAKAGE_EVIDENCE_YET || !enough_measurements) && measurement_threshold) {
    state = dudect_main(&ctx);

    ttest_ctx_t* t = max_test(&ctx);
    // Reported Values for T-test

    double max_t = fabs(t_compute(t));
    double number_traces_max_t = t->n[0] + t->n[1];
    double max_tau = max_t / sqrt(number_traces_max_t);
    double estimated_measurements = (double)(5 * 5) / (double)(max_tau * max_tau);

    enough_measurements = number_traces_max_t > DUDECT_ENOUGH_MEASUREMENTS;
    if (enough_measurements) {
      // Stop when estimated measurements to potential detect leakage is 10 M, prevent overflow issue
      measurement_threshold = (estimated_measurements < 1e7) && (number_traces_max_t < estimated_measurements * 100);
    } else {
      // Write to outfile for histogram creation
      for (uint16_t i = 0; i < NUMBER_MEASUREMENTS; i++) {
        if (ctx.classes[i] == 1) {
          var_csv << ctx.exec_times[i] << "\n";
        } else {
          base_csv << ctx.exec_times[i] << "\n";
        }
      }
    }
  }
  base_csv.close();
  var_csv.close();

  dudect_free(&ctx);

  EXPECT_EQ(state, expected_state);
}

TEST(DISABLED_DUDECT_VT_BN_CORE, BN_ModNeg) {
  active_funct = test_mod_neg;
  {
    coinbase::crypto::vartime_scope_t vartime_scope;
    run_dudect_leakage_test(DUDECT_LEAKAGE_FOUND, 100);
  }
}
TEST(DUDECT_VT_Mod, Add) {
  active_funct = test_mod_add;
  {
    coinbase::crypto::vartime_scope_t vartime_scope;
    run_dudect_leakage_test(DUDECT_LEAKAGE_FOUND, 100);
  }
}
TEST(DUDECT_VT_Mod, Sub) {
  active_funct = test_mod_sub;
  {
    coinbase::crypto::vartime_scope_t vartime_scope;
    run_dudect_leakage_test(DUDECT_LEAKAGE_FOUND, 100);
  }
}
TEST(DUDECT_VT_Mod, Mul) {
  active_funct = test_mod_mul;
  {
    coinbase::crypto::vartime_scope_t vartime_scope;
    run_dudect_leakage_test(DUDECT_LEAKAGE_FOUND, 256);
  }
}
TEST(DUDECT_VT_Mod, Pow) {
  active_funct = test_pow_mod;
  {
    coinbase::crypto::vartime_scope_t vartime_scope;
    run_dudect_leakage_test(DUDECT_LEAKAGE_FOUND, 100);
  }
}
TEST(DUDECT_VT_Mod, Inv) {
  active_funct = test_mod_inv;
  {
    coinbase::crypto::vartime_scope_t vartime_scope;
    run_dudect_leakage_test(DUDECT_LEAKAGE_FOUND, 100);
  }
}
TEST(DUDECT_VT_Mod, Coprime) {
  active_funct = test_coprime;
  {
    coinbase::crypto::vartime_scope_t vartime_scope;
    run_dudect_leakage_test(DUDECT_LEAKAGE_FOUND, 100);
  }
}
TEST(DUDECT_CT_Mod, Mod) {
  active_funct = test_mod;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_Mod, Neg) {
  active_funct = test_mod_neg;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_Mod, Add) {
  active_funct = test_mod_add;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_Mod, Sub) {
  active_funct = test_mod_sub;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_Mod, Mul) {
  active_funct = test_mod_mul;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_Mod, Inv) {
  active_funct = test_mod_inv;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_Mod, InvSCR) {
  active_funct = test_mod_inv_scr;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_Mod, Coprime) {
  active_funct = test_coprime;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_Mod, Rand) {
  active_funct = test_mod_rand;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
}  // namespace coinbase::dudect
