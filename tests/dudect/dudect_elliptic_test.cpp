#include <gtest/gtest.h>

#define DUDECT_IMPLEMENTATION

#include "dudect_util/dudect_implementation.h"
namespace coinbase::dudect {

#define SECRET_LEN_BYTES (2)
#define NUMBER_MEASUREMENTS (250)
#define NUMBER_OPERANDS (2)

constexpr double kMaxNoLeakageMeasurements = 100000;
constexpr double kMaxEstimatedMeasurements = 1000000;

static ecurve_t curve;

static ecc_point_t G;
static ecc_point_t P;
static ecc_point_t ecc_pt_base_a;
static ecc_point_t ecc_pt_base_b;
static mod_t q;
static mod_t small_q;
static bn_t base_bn;
static int secret_scalar_bitlen;

static ecc_point_t ecc_pt_arr[NUMBER_MEASUREMENTS * NUMBER_OPERANDS];
static bn_t bn_arr[NUMBER_MEASUREMENTS * NUMBER_OPERANDS];

static bn_t rand_min_bitlen(const mod_t& q, int bits) {
  cb_assert(bits > 192);
  cb_assert(bits <= q.get_bits_count());
  const bn_t min_value = bn_t(1).lshift(bits - 1);
  return min_value + bn_t::rand(q.value() - min_value);
}

void generate_ecc_array(uint8_t c, uint16_t idx) {
  uint16_t start_idx = NUMBER_OPERANDS * idx;
  if (c == 1) {
    ecc_pt_arr[start_idx] = curve.mul_to_generator(rand_min_bitlen(q, secret_scalar_bitlen));
    ecc_pt_arr[start_idx + 1] = curve.mul_to_generator(rand_min_bitlen(q, secret_scalar_bitlen));

    bn_arr[start_idx] = denormalize(rand_min_bitlen(q, secret_scalar_bitlen), q);
    bn_arr[start_idx + 1] = denormalize(rand_min_bitlen(q, secret_scalar_bitlen), q);
  } else {
    ecc_pt_arr[start_idx] = ecc_pt_base_a;
    ecc_pt_arr[start_idx + 1] = ecc_pt_base_b;

    bn_arr[start_idx] = denormalize(base_bn, q);
    bn_arr[start_idx + 1] = denormalize(base_bn, q);
  }
}

uint8_t test_ecc_add(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  ecc_point_t::add(ecc_pt_arr[start_idx], ecc_pt_arr[start_idx + 1]);
  return 0;
}
uint8_t test_ecc_sub(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  ecc_point_t::sub(ecc_pt_arr[start_idx], ecc_pt_arr[start_idx + 1]);
  return 0;
}
uint8_t test_ecc_mul_P(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  ecc_point_t::mul(P, bn_arr[start_idx]);
  return 0;
}
uint8_t test_mul_G(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  curve.mul_to_generator(bn_arr[start_idx]);

  return 0;
}
uint8_t test_muladd(uint8_t* data) {
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  curve.mul_add(bn_arr[start_idx], P, bn_arr[start_idx + 1]);
  return 0;
}
static void run_dudect_leakage_test(dudect_state_t expected_state, uint16_t baseline_bitlen) {
  input_generator = generate_ecc_array;

  G = curve.generator();
  q = curve.order();
  secret_scalar_bitlen = baseline_bitlen;
  small_q = mod_t(bn_t::generate_prime(baseline_bitlen, true), /* multiplicative_dense */ true);
  P = curve.mul_to_generator(rand_min_bitlen(q, secret_scalar_bitlen));
  base_bn = rand_min_bitlen(small_q, secret_scalar_bitlen);
  ecc_pt_base_a = curve.mul_to_generator(rand_min_bitlen(small_q, secret_scalar_bitlen));
  ecc_pt_base_b = curve.mul_to_generator(rand_min_bitlen(small_q, secret_scalar_bitlen));

  dudect_config_t config = {
      .chunk_size = SECRET_LEN_BYTES,
#ifdef MEASUREMENTS_PER_CHUNK
      .number_measurements = MEASUREMENTS_PER_CHUNK,
#else
      .number_measurements = NUMBER_MEASUREMENTS,
#endif
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
    double raw_measurements = ctx.ttest_ctxs[0]->n[0] + ctx.ttest_ctxs[0]->n[1];

    enough_measurements = number_traces_max_t > DUDECT_ENOUGH_MEASUREMENTS;
    if (enough_measurements) {
      // Stop bounded CI runs once no leak has been found within a practical budget.
      measurement_threshold = (state == DUDECT_NO_LEAKAGE_EVIDENCE_YET) &&
                              (raw_measurements < kMaxNoLeakageMeasurements) &&
                              (estimated_measurements < kMaxEstimatedMeasurements);
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

TEST(DUDECT_CT_ECC, Secp256k1MulP) {
  curve = coinbase::crypto::curve_secp256k1;
  active_funct = test_ecc_mul_P;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_ECC, Secp256k1MulG) {
  curve = coinbase::crypto::curve_secp256k1;
  active_funct = test_mul_G;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_ECC, Secp256k1MulAdd) {
  curve = coinbase::crypto::curve_secp256k1;
  active_funct = test_muladd;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}

TEST(DUDECT_CT_ECC, Ed25519MulP) {
  curve = coinbase::crypto::curve_ed25519;
  active_funct = test_ecc_mul_P;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_ECC, Ed25519Add) {
  curve = coinbase::crypto::curve_ed25519;
  active_funct = test_ecc_add;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_ECC, Ed25519MulG) {
  curve = coinbase::crypto::curve_ed25519;
  active_funct = test_mul_G;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
TEST(DUDECT_CT_ECC, Ed25519MulAdd) {
  curve = coinbase::crypto::curve_ed25519;
  active_funct = test_muladd;
  run_dudect_leakage_test(DUDECT_NO_LEAKAGE_EVIDENCE_YET, 200);
}
}  // namespace coinbase::dudect
