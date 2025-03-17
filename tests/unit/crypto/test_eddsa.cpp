#include <gtest/gtest.h>

#include <cbmpc/core/log.h>
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

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

}  // namespace
