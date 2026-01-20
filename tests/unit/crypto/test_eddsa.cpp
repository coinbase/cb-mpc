#include <gtest/gtest.h>

#include <cbmpc/core/log.h>
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

TEST(CryptoEdDSA, RejectTorsionPointsAndFixInfinityEquality) {
  crypto::vartime_scope_t vartime_scope;
  ecurve_t curve = crypto::curve_ed25519;

  // Compressed encoding of the Ed25519 order-2 point (x=0, y=-1):
  // y = p-1 = 2^255-20, sign bit = 0.
  uint8_t order2[32];
  order2[0] = 0xec;
  for (int i = 1; i < 31; i++) order2[i] = 0xff;
  order2[31] = 0x7f;

  ecc_point_t P(curve);
  EXPECT_EQ(P.from_bin(curve, mem_t(order2, 32)), SUCCESS);
  EXPECT_TRUE(P.is_on_curve());
  EXPECT_FALSE(P.is_infinity());
  EXPECT_FALSE(P.is_in_subgroup());
  EXPECT_NE(curve.check(P), SUCCESS);

  // Sanity: infinity should not compare equal to the generator.
  const ecc_point_t G = curve.generator();
  const ecc_point_t I = curve.infinity();
  EXPECT_FALSE(G == I);
  EXPECT_TRUE(I.is_infinity());
}

TEST(CryptoEdDSA, from_bin) {
  int n = 1000;
  error_t rv = UNINITIALIZED_ERROR;
  ecurve_t curve = crypto::curve_ed25519;
  int point_counter = 0;
  int on_curve_counter = 0;
  int in_group_counter = 0;
  for (int i = 0; i < n; i++) {
    ecurve_ed_t ed_curve;
    ro::hash_string_t h;
    h.encode_and_update(i);

    buf_t bin = h.bitlen(curve.bits());
    ecc_point_t Q(curve);

    {
      dylog_disable_scope_t no_log_err;
      if (rv = ed_curve.from_bin(Q, bin)) continue;
    }

    point_counter++;
    if (ed_curve.is_on_curve(Q)) on_curve_counter++;
    if (ed_curve.is_in_subgroup(Q)) in_group_counter++;
  }

  // We expect some from_bin fails but not too much
  EXPECT_LE(point_counter, n);
  EXPECT_GE(point_counter, n / 10);

  // all points should be on the curve
  EXPECT_EQ(on_curve_counter, point_counter);

  // co-factor of ed25519 is 8. In expectation, 1/8 of points are in the subgroup
  EXPECT_GT(in_group_counter, point_counter / 12);
  EXPECT_LT(in_group_counter, point_counter / 6);
}

TEST(CryptoEdDSA, hash_to_point) {
  int n = 1000;
  ecurve_t curve = crypto::curve_ed25519;
  int point_counter = 0;
  int on_curve_counter = 0;
  int in_group_counter = 0;
  for (int i = 0; i < n; i++) {
    ecurve_ed_t ed_curve;
    ro::hash_string_t h;
    h.encode_and_update(i);

    buf_t bin = h.bitlen(curve.bits());
    ecc_point_t Q(curve);
    {
      dylog_disable_scope_t no_log_err;
      if (!ed_curve.hash_to_point(bin, Q)) continue;
    }

    point_counter++;
    if (ed_curve.is_on_curve(Q)) on_curve_counter++;
    if (ed_curve.is_in_subgroup(Q)) in_group_counter++;
  }

  // We expect some hash_to_point fails but not too much
  EXPECT_LE(point_counter, n);
  EXPECT_GE(point_counter, n / 10);

  // all points should be on the curve and in the subgroup
  EXPECT_EQ(on_curve_counter, point_counter);
  EXPECT_EQ(in_group_counter, point_counter);
}

TEST(CryptoEdDSA, mul_by_order_is_infinity_for_subgroup_points) {
  crypto::vartime_scope_t vartime_scope;
  ecurve_t curve = crypto::curve_ed25519;
  const bn_t q = curve.order().value();
  const bn_t q_minus_1 = q - 1;

  // Generator and infinity are in the prime-order subgroup.
  const ecc_point_t G = curve.generator();
  const ecc_point_t I = curve.infinity();
  {
    ecc_point_t R = G;
    R *= q;
    EXPECT_TRUE(R.is_infinity());
    EXPECT_TRUE(R == I);
  }
  {
    ecc_point_t R = G;
    R *= q_minus_1;
    EXPECT_TRUE(R == -G);
  }
  {
    ecc_point_t R = I;
    R *= q;
    EXPECT_TRUE(R.is_infinity());
    EXPECT_TRUE(R == I);
  }
  {
    ecc_point_t R = I;
    R *= q_minus_1;
    EXPECT_TRUE(R == -I);
    EXPECT_TRUE(R.is_infinity());
  }

  // Additional coverage: for many subgroup points (hash_to_point clears cofactor),
  // multiplying by the subgroup order yields infinity.
  const int want = 64;
  const int max_tries = 10000;
  int got = 0;

  for (int i = 0; i < max_tries && got < want; i++) {
    ro::hash_string_t h;
    h.encode_and_update(i);
    buf_t bin = h.bitlen(curve.bits());

    ecc_point_t P(curve);
    {
      dylog_disable_scope_t no_log_err;
      if (!curve.hash_to_point(bin, P)) continue;
    }

    ASSERT_TRUE(P.is_on_curve());
    ASSERT_TRUE(P.is_in_subgroup());

    ecc_point_t R = P;
    R *= q;
    EXPECT_TRUE(R.is_infinity());
    EXPECT_TRUE(R == I);

    ecc_point_t R2 = P;
    R2 *= q_minus_1;
    EXPECT_TRUE(R2 == -P);

    got++;
  }

  EXPECT_EQ(got, want);
}

TEST(CryptoEdDSA, subgroup_check) {
  crypto::vartime_scope_t vartime_scope;
  ecurve_t curve = crypto::curve_ed25519;

  const ecc_point_t G = curve.generator();
  const ecc_point_t I = curve.infinity();

  EXPECT_TRUE(G.is_on_curve());
  EXPECT_TRUE(G.is_in_subgroup());
  EXPECT_EQ(curve.check(G), SUCCESS);

  EXPECT_TRUE(I.is_infinity());
  EXPECT_TRUE(I.is_in_subgroup());
  // By default, curve.check() rejects infinity unless allow_ecc_infinity_t is in scope.
  EXPECT_NE(curve.check(I), SUCCESS);

  // Known torsion point: compressed encoding of the Ed25519 order-2 point (x=0, y=-1):
  // y = p-1 = 2^255-20, sign bit = 0.
  uint8_t order2[32];
  order2[0] = 0xec;
  for (int i = 1; i < 31; i++) order2[i] = 0xff;
  order2[31] = 0x7f;

  ecc_point_t T(curve);
  ASSERT_EQ(T.from_bin(curve, mem_t(order2, 32)), SUCCESS);
  EXPECT_TRUE(T.is_on_curve());
  EXPECT_FALSE(T.is_infinity());
  EXPECT_FALSE(T.is_in_subgroup());
  EXPECT_NE(curve.check(T), SUCCESS);

  // hash_to_point is required to clear cofactor, so outputs should always be subgroup points.
  // Some inputs may map to torsion points that become infinity after cofactor clearing; those
  // are still in the subgroup but will be rejected by curve.check().
  const int want = 64;  // non-infinity subgroup points
  const int max_tries = 10000;
  int got = 0;
  for (int i = 0; i < max_tries && got < want; i++) {
    ro::hash_string_t h;
    h.encode_and_update(i);
    buf_t bin = h.bitlen(curve.bits());

    ecc_point_t P(curve);
    {
      dylog_disable_scope_t no_log_err;
      if (!curve.hash_to_point(bin, P)) continue;
    }
    EXPECT_TRUE(P.is_in_subgroup());
    if (P.is_infinity()) {
      EXPECT_NE(curve.check(P), SUCCESS);
      continue;
    }

    EXPECT_TRUE(P.is_on_curve());
    EXPECT_EQ(curve.check(P), SUCCESS);
    got++;
  }
  EXPECT_EQ(got, want);
}

}  // namespace
