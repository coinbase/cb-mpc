#include <gtest/gtest.h>

#include <cbmpc/crypto/lagrange.h>

#include "utils/test_macros.h"

using namespace coinbase::crypto;

class Lagrange : public ::testing::Test {
 protected:
  void SetUp() override {
    // Initialize common variables for tests
    t = 3;
    n = 5;
    curve = curve_secp256k1;
    q = curve.order();
    secret = bn_t::rand(q);
  }

  mod_t q;
  bn_t secret;
  int t;
  int n;
  ecurve_t curve;
  vartime_scope_t v;
};

TEST_F(Lagrange, Basis) {
  std::vector<int> pids = {1, 3, 4, 5, 7};
  bn_t x = 0;
  bn_t numerator, denominator;
  lagrange_basis(x, pids, 1, q, numerator, denominator);
  EXPECT_EQ(numerator, 3 * 4 * 5 * 7);
  EXPECT_EQ(denominator, 2 * 3 * 4 * 6);

  lagrange_basis(x, pids, 3, q, numerator, denominator);
  EXPECT_EQ(numerator, 1 * 4 * 5 * 7);
  EXPECT_EQ(denominator, q.mod((-2) * 1 * 2 * 4));

  lagrange_basis(x, pids, 4, q, numerator, denominator);
  EXPECT_EQ(numerator, 1 * 3 * 5 * 7);
  EXPECT_EQ(denominator, q.mod((-3) * (-1) * 1 * 3));

  lagrange_basis(x, pids, 5, q, numerator, denominator);
  EXPECT_EQ(numerator, 1 * 3 * 4 * 7);
  EXPECT_EQ(denominator, q.mod((-4) * (-2) * (-1) * 2));

  lagrange_basis(x, pids, 7, q, numerator, denominator);
  EXPECT_EQ(numerator, 1 * 3 * 4 * 5);
  EXPECT_EQ(denominator, q.mod((-6) * (-4) * (-3) * (-2)));

  EXPECT_CB_ASSERT(lagrange_basis(x, {-1, 3, 4, 5, 7}, 0, q, numerator, denominator), "pids must be positive");
}

TEST_F(Lagrange, Interpolate) {
  std::vector<bn_t> pids = {1, 4, 5};
  std::vector<bn_t> a(t);
  std::vector<bn_t> shares(t);
  for (int i = 0; i < t; i++) a[i] = bn_t::rand(q);
  for (int i = 0; i < t; i++) shares[i] = horner_poly(q, a, pids[i]);
  bn_t secret = lagrange_interpolate(0, shares, pids, q);
  EXPECT_EQ(a[0], secret);

  for (int i = 0; i < 5; i++) {
    bn_t r = bn_t::rand(q);
    EXPECT_EQ(horner_poly(q, a, r), lagrange_interpolate(r, shares, pids, q));
  }
}

TEST_F(Lagrange, PartialInterpolate) {
  std::vector<bn_t> pids = {1, 4, 5};
  std::vector<bn_t> a(t);
  std::vector<bn_t> all_shares(t);
  for (int i = 0; i < t; i++) a[i] = bn_t::rand(q);
  for (int i = 0; i < t; i++) all_shares[i] = horner_poly(q, a, pids[i]);

  std::vector<bn_t> pids_half_1(t / 2);
  std::vector<bn_t> pids_half_2(t - t / 2);
  std::vector<bn_t> shares_half_1(t / 2);
  std::vector<bn_t> shares_half_2(t - t / 2);
  for (int i = 0; i < t / 2; i++) {
    pids_half_1[i] = pids[i];
    shares_half_1[i] = all_shares[i];
  }
  for (int i = 0; i < t - t / 2; i++) {
    pids_half_2[i] = pids[i + t / 2];
    shares_half_2[i] = all_shares[i + t / 2];
  }
  bn_t secret_half_1 = lagrange_partial_interpolate(0, shares_half_1, pids_half_1, pids, q);
  bn_t secret_half_2 = lagrange_partial_interpolate(0, shares_half_2, pids_half_2, pids, q);
  EXPECT_EQ(a[0], (secret_half_1 + secret_half_2) % q);
}

TEST_F(Lagrange, InterpolateExponent) {
  std::vector<bn_t> pids = {1, 4, 5};
  std::vector<ecc_point_t> A(t);
  std::vector<ecc_point_t> pub_shares(t);
  auto G = curve.generator();
  for (int i = 0; i < t; i++) A[i] = bn_t::rand(q) * G;
  for (int i = 0; i < t; i++) pub_shares[i] = horner_poly(A, pids[i]);
  ecc_point_t interpolated_point = lagrange_interpolate_exponent(0, pub_shares, pids);
  EXPECT_EQ(interpolated_point, A[0]);

  for (int i = 0; i < 5; i++) {
    bn_t r = bn_t::rand(q);
    EXPECT_EQ(horner_poly(A, r), lagrange_interpolate_exponent(r, pub_shares, pids));
  }
}

TEST_F(Lagrange, PartialInterpolateExponent) {
  std::vector<bn_t> pids = {1, 4, 5};
  std::vector<ecc_point_t> A(t);
  std::vector<ecc_point_t> all_shares(t);
  auto G = curve.generator();
  for (int i = 0; i < t; i++) A[i] = bn_t::rand(q) * G;
  for (int i = 0; i < t; i++) all_shares[i] = horner_poly(A, pids[i]);

  std::vector<bn_t> pids_half_1(t / 2);
  std::vector<bn_t> pids_half_2(t - t / 2);
  std::vector<ecc_point_t> shares_half_1(t / 2);
  std::vector<ecc_point_t> shares_half_2(t - t / 2);
  for (int i = 0; i < t / 2; i++) {
    pids_half_1[i] = pids[i];
    shares_half_1[i] = all_shares[i];
  }
  for (int i = 0; i < t - t / 2; i++) {
    pids_half_2[i] = pids[i + t / 2];
    shares_half_2[i] = all_shares[i + t / 2];
  }
  ecc_point_t interpolated_point_half_1 = lagrange_partial_interpolate_exponent(0, shares_half_1, pids_half_1, pids);
  ecc_point_t interpolated_point_half_2 = lagrange_partial_interpolate_exponent(0, shares_half_2, pids_half_2, pids);
  EXPECT_EQ(A[0], interpolated_point_half_1 + interpolated_point_half_2);
}