#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/elgamal.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

// Helper functions for testing (moved from the library as they are test-only)
bool check_zero(const ec_elgamal_commitment_t& E, const bn_t& d) { return E.R == d * E.L; }

bool check_equ(const ec_elgamal_commitment_t& E1, const ec_elgamal_commitment_t& E2, const bn_t& d) {
  return check_zero(E1 - E2, d);
}

class ElGamal : public testing::Test {
 protected:
  void SetUp() override {
    // ElGamal requires a curve backend with constant-time point addition when not in vartime scope.
    curve = curve_secp256k1;
    q = curve.order();
    G = curve.generator();
  }

  ecurve_t curve;
  bn_t q;
  ecc_generator_point_t G;
};

TEST_F(ElGamal, Commitment) {
  auto P = curve.mul_to_generator(bn_t::rand(q));
  auto m = bn_t::rand(q);
  auto r = bn_t::rand(q);

  auto E = ec_elgamal_commitment_t::make_commitment(P, m, r);

  EXPECT_EQ(E.L, r * G);
  EXPECT_EQ(E.R, curve.mul_add(m, P, r));
}

TEST_F(ElGamal, API) {
  auto [P, d] = ec_elgamal_commitment_t::local_keygen(curve);

  bn_t a = bn_t::rand_bitlen(250);  // 250 bits
  bn_t b = bn_t::rand_bitlen(250);
  bn_t c = bn_t::rand_bitlen(250);

  ec_elgamal_commitment_t A = ec_elgamal_commitment_t::random_commit(P, a);
  ec_elgamal_commitment_t B = ec_elgamal_commitment_t::random_commit(P, b);

  ec_elgamal_commitment_t A_plus_B = A + B;
  ec_elgamal_commitment_t A_plus_b = A + b;

  ec_elgamal_commitment_t A_plus_B_test =
      ec_elgamal_commitment_t::random_commit(P, a) + ec_elgamal_commitment_t::random_commit(P, b);

  EXPECT_TRUE(check_equ(A_plus_B, A_plus_B_test, d));
  EXPECT_TRUE(check_equ(A_plus_B_test, A_plus_b, d));

  ec_elgamal_commitment_t A1 = A;
  A1.randomize(P);
  EXPECT_TRUE(check_equ(A, A1, d));

  ec_elgamal_commitment_t A_mul_c = c * A;
  ec_elgamal_commitment_t A_mul_c_test = ec_elgamal_commitment_t::random_commit(P, a * c);
  EXPECT_TRUE(check_equ(A_mul_c_test, A_mul_c, d));

  int p = 17;
  const mod_t& q = ec_elgamal_commitment_t::order(curve);

  uint64_t d1 = 0, d2 = 0, d3 = 0, d4 = 0, d5 = 0, d6 = 0, f = 0;

  for (int i = 0; i < 20; i++) {
    for (int a = 0; a < p; a++) {
      for (int b = 0; b < p; b++) {
        bool test = (a + b) % p == 0;
        ec_elgamal_commitment_t C1 = ec_elgamal_commitment_t::random_commit(P, a);  // 1
        ec_elgamal_commitment_t X = C1;
        if (b != 0) {
          bn_t temp;
          MODULO(q) temp = bn_t(b) - bn_t(p);
          X += temp;
        }
        bn_t r = bn_t::rand(q);
        X = X * r;
        X.randomize(P);
        bool t = check_zero(X, d);
        EXPECT_EQ(t, test);
      }
    }
  }
}

TEST_F(ElGamal, InPlaceOperatorsMatchNonMutatingForms) {
  auto [P, d] = ec_elgamal_commitment_t::local_keygen(curve);
  (void)d;

  const ec_elgamal_commitment_t A = ec_elgamal_commitment_t::make_commitment(P, bn_t(5), bn_t(11));
  const ec_elgamal_commitment_t B = ec_elgamal_commitment_t::make_commitment(P, bn_t(2), bn_t(13));
  const bn_t scalar = 7;

  ec_elgamal_commitment_t plus = A;
  plus += B;
  EXPECT_EQ(A + B, plus);

  ec_elgamal_commitment_t minus = A;
  minus -= B;
  EXPECT_EQ(A - B, minus);

  ec_elgamal_commitment_t add_scalar = A;
  add_scalar += scalar;
  EXPECT_EQ(A + scalar, add_scalar);

  ec_elgamal_commitment_t sub_scalar = A;
  sub_scalar -= scalar;
  EXPECT_EQ(A - scalar, sub_scalar);

  ec_elgamal_commitment_t mul = A;
  mul *= scalar;
  EXPECT_EQ(A * scalar, mul);

  ec_elgamal_commitment_t div = A;
  div /= scalar;
  EXPECT_EQ(A / scalar, div);
  EXPECT_EQ(A, div * scalar);
  EXPECT_NE(A * scalar, div);
}

TEST_F(ElGamal, EqualityChecksBothCoordinates) {
  auto [P, d] = ec_elgamal_commitment_t::local_keygen(curve);
  (void)d;

  const ec_elgamal_commitment_t A = ec_elgamal_commitment_t::make_commitment(P, bn_t(5), bn_t(11));
  const ec_elgamal_commitment_t same = A;
  const ec_elgamal_commitment_t different_R = A + bn_t(1);
  const ec_elgamal_commitment_t different_L(bn_t(2) * A.L, A.R);

  EXPECT_TRUE(A == same);
  EXPECT_FALSE(A != same);
  EXPECT_FALSE(A == different_R);
  EXPECT_TRUE(A != different_R);
  EXPECT_FALSE(A == different_L);
  EXPECT_TRUE(A != different_L);
}

TEST_F(ElGamal, RerandBuilderAndExplicitRandomizeMatch) {
  auto [P, d] = ec_elgamal_commitment_t::local_keygen(curve);
  const ec_elgamal_commitment_t A = ec_elgamal_commitment_t::make_commitment(P, bn_t(5), bn_t(11));
  const bn_t r = 19;

  const ec_elgamal_commitment_t expected = A.rerand(P, r);
  EXPECT_EQ(expected, ec_elgamal_commitment_t::rerand(P, A).rand(r));

  ec_elgamal_commitment_t in_place = A;
  in_place.randomize(r, P);
  EXPECT_EQ(expected, in_place);
  EXPECT_TRUE(check_equ(A, expected, d));
  EXPECT_NE(A, expected);
}

TEST_F(ElGamal, HashUpdateStateUsesBothCoordinates) {
  auto [P, d] = ec_elgamal_commitment_t::local_keygen(curve);
  (void)d;

  const ec_elgamal_commitment_t A = ec_elgamal_commitment_t::make_commitment(P, bn_t(5), bn_t(11));
  const ec_elgamal_commitment_t different_R = A + bn_t(1);
  const ec_elgamal_commitment_t different_L(bn_t(2) * A.L, A.R);

  EXPECT_EQ(sha256_t::hash(A), sha256_t::hash(A.L, A.R));
  EXPECT_NE(sha256_t::hash(A), sha256_t::hash(different_R));
  EXPECT_NE(sha256_t::hash(A), sha256_t::hash(different_L));
}

TEST_F(ElGamal, SerializationRoundTripAndWrongCurveCheck) {
  auto [P, d] = ec_elgamal_commitment_t::local_keygen(curve);
  const bn_t message = bn_t(9);
  ec_elgamal_commitment_t commitment = ec_elgamal_commitment_t::random_commit(P, message);

  EXPECT_OK(commitment.check_curve(curve));
  EXPECT_ER(commitment.check_curve(curve_p256));

  const buf_t serialized = coinbase::convert(commitment);
  ec_elgamal_commitment_t roundtrip;
  ASSERT_EQ(coinbase::convert(roundtrip, serialized), SUCCESS);
  EXPECT_TRUE(check_equ(commitment, roundtrip, d));
}

TEST_F(ElGamal, CheckCurveRejectsInvalidR) {
  auto [P, d] = ec_elgamal_commitment_t::local_keygen(curve);
  (void)d;
  const ec_elgamal_commitment_t commitment = ec_elgamal_commitment_t::make_commitment(P, bn_t(9), bn_t(17));

  ecc_point_t wrong_curve_R;
  {
    vartime_scope_t vartime_scope;
    wrong_curve_R = bn_t(2) * curve_p256.generator();
  }

  ec_elgamal_commitment_t invalid_R(commitment.L, wrong_curve_R);
  EXPECT_ER(invalid_R.check_curve(curve));
}

}  // namespace
