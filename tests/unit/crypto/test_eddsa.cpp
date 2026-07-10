#include <gtest/gtest.h>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/ro.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

TEST(CryptoEdDSA, RejectTorsionAndFixInfinityEq) {
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

TEST(CryptoEdDSA, GeneratorMulByOrderIsInfinityForSubgroup) {
  crypto::vartime_scope_t vartime_scope;
  ecurve_t curve = crypto::curve_ed25519;
  const bn_t q = curve.order().value();
  const bn_t q_minus_1 = q - 1;

  const ecc_generator_point_t& G = curve.generator();
  const ecc_point_t I = curve.infinity();
  {
    ecc_point_t R = q * G;
    EXPECT_TRUE(R.is_infinity());
    EXPECT_TRUE(R == I);
  }
  {
    ecc_point_t R = q_minus_1 * G;
    EXPECT_TRUE(R == -G);
  }
  {
    ecc_point_t R = (q + 1) * G;
    EXPECT_TRUE(R == G);
  }
  {
    ecc_point_t R = I;
    R *= q_minus_1;
    EXPECT_TRUE(R == -I);
    EXPECT_TRUE(R.is_infinity());
  }

  // Additional coverage: for many subgroup points (hash_to_point clears cofactor),
  // multiplication by a scalar just below the subgroup order behaves as expected.
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

    ecc_point_t R2 = P;
    R2 *= q_minus_1;
    EXPECT_TRUE(R2 == -P);

    got++;
  }

  EXPECT_EQ(got, want);
}

TEST(CryptoEdDSA, GenericMulRejectsOrderScalarForTorsionPoint) {
  ecurve_t curve = crypto::curve_ed25519;
  const bn_t q = curve.order().value();

  uint8_t order2[32];
  order2[0] = 0xec;
  for (int i = 1; i < 31; i++) order2[i] = 0xff;
  order2[31] = 0x7f;

  ecc_point_t T(curve);
  ASSERT_EQ(T.from_bin(curve, mem_t(order2, 32)), SUCCESS);
  ASSERT_TRUE(T.is_on_curve());
  ASSERT_FALSE(T.is_in_subgroup());

  {
    dylog_disable_scope_t no_log_err;
    EXPECT_THROW(
        {
          ecc_point_t R = T;
          R *= q;
        },
        coinbase::assertion_failed_t);
  }
  {
    crypto::vartime_scope_t vartime_scope;
    dylog_disable_scope_t no_log_err;
    EXPECT_THROW(
        {
          ecc_point_t R = T;
          R *= q;
        },
        coinbase::assertion_failed_t);
  }
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

TEST(CryptoEdDSA, SetEdBinValidatesKeyLength) {
  // Ed25519 private keys must be exactly 32 bytes
  // set_ed_bin() should validate this to prevent out-of-bounds reads in sign()

  // Test with too short key (16 bytes)
  {
    buf_t short_key(16);
    short_key.bzero();

    ecc_prv_key_t key;
    // Should throw assertion_failed_t for wrong-length key
    EXPECT_THROW({ key.set_ed_bin(short_key); }, coinbase::assertion_failed_t);
  }

  // Test with too long key (48 bytes)
  {
    buf_t long_key(48);
    long_key.bzero();

    ecc_prv_key_t key;
    // Should throw assertion_failed_t for wrong-length key
    EXPECT_THROW({ key.set_ed_bin(long_key); }, coinbase::assertion_failed_t);
  }

  // Test with valid 32-byte key - should succeed
  {
    buf_t valid_key(32);
    valid_key.bzero();

    ecc_prv_key_t key;
    EXPECT_NO_THROW({ key.set_ed_bin(valid_key); });

    // Verify the key was set correctly
    ecc_point_t pub = key.pub();
    EXPECT_TRUE(pub.is_on_curve());
  }
}

TEST(CryptoEdDSA, ScalarKeySignsAndVerifies) {
  ecc_prv_key_t key;
  key.set(curve_ed25519, bn_t(7));
  const ecc_pub_key_t pub_key = key.pub();
  const buf_t message = buf_t("ed25519 scalar signing path");

  const buf_t signature = key.sign(message);
  EXPECT_EQ(signature.size(), size_t(ed25519::signature_size()));
  EXPECT_OK(pub_key.verify(message, signature));
  EXPECT_ER(pub_key.verify(buf_t("different message"), signature));
}

TEST(CryptoEdDSA, DerEncodingRoundTripsPublicMaterialAndRawPrivateSeed) {
  ecc_prv_key_t key;
  key.generate(curve_ed25519);
  const ecc_pub_key_t pub_key = key.pub();

  ecurve_ed_t ed_curve;
  const buf_t pub_der = pub_key.to_der();
  ecc_pub_key_t parsed_pub(curve_ed25519.infinity());
  EXPECT_OK(ed_curve.pub_from_der(parsed_pub, pub_der));
  EXPECT_TRUE(parsed_pub == pub_key);

  buf_t bad_pub_der = pub_der;
  bad_pub_der[0] ^= 0x01;
  EXPECT_ER(ed_curve.pub_from_der(parsed_pub, bad_pub_der));
  EXPECT_ER(ed_curve.pub_from_der(parsed_pub, pub_der.take(pub_der.size() - 1)));

  const buf_t prv_der = ed_curve.prv_to_der(key);
  ecc_prv_key_t parsed_prv;
  EXPECT_OK(ed_curve.prv_from_der(parsed_prv, prv_der));
  EXPECT_EQ(parsed_prv.get_ed_bin(), key.get_ed_bin());

  buf_t bad_prv_der = prv_der;
  bad_prv_der[0] ^= 0x01;
  EXPECT_ER(ed_curve.prv_from_der(parsed_prv, bad_prv_der));
  EXPECT_ER(ed_curve.prv_from_der(parsed_prv, prv_der.take(prv_der.size() - 1)));
}

TEST(CryptoEdDSA, CurveMetadataAndConditionalCopy) {
  EXPECT_EQ(ed25519::bits(), 256);
  EXPECT_EQ(curve_ed25519.p().value(),
            bn_t::from_string("57896044618658097711785492504343953926634992332820282019728792003956564819949"));

  const ecc_point_t src = bn_t(11) * curve_ed25519.generator();
  ecc_point_t dst = curve_ed25519.infinity();
  EXPECT_TRUE(curve_ed25519.cnd_copy_point(true, src, dst));
  EXPECT_TRUE(dst == src);

  ecc_point_t unchanged = curve_ed25519.infinity();
  EXPECT_TRUE(curve_ed25519.cnd_copy_point(false, src, unchanged));
  EXPECT_TRUE(unchanged.is_infinity());
}

}  // namespace
