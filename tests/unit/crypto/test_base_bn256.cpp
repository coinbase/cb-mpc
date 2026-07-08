#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_bn256.h>
#include <cbmpc/internal/crypto/ro.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

bn_t limbs_to_bn(const uint64_t limbs[8]) {
  buf_t bin(64);
  for (int i = 0; i < 8; ++i) coinbase::be_set_8(bin.data() + 8 * (7 - i), limbs[i]);
  return bn_t::from_bin(bin);
}

bn_t wide_to_bn(const uint320_t& value) {
  uint64_t limbs[8] = {value.w0, value.w1, value.w2, value.w3, value.w4, 0, 0, 0};
  return limbs_to_bn(limbs);
}

TEST(BigNumber256, Elementary) {
  const mod_t q = bn_t::from_string("7237005577332262213973186563042994240857116359379907606001950938285454250989");

  bn256_t zero = 0;
  bn256_t one = 1;

  for (int i = 0; i < 10000; i++) {
    auto a = bn256_t::rand(q);
    bn_t a_bn = a;
    auto b = bn256_t::rand(q);
    bn_t b_bn = b;
    auto c = bn256_t::rand(q);
    bn_t c_bn;

    MODULO(q) {
      c_bn = a_bn + b_bn;
      ASSERT_EQ(bn_t(a + b), c_bn);

      c_bn = a_bn - b_bn;
      ASSERT_EQ(bn_t(a - b), c_bn);

      c_bn = a_bn * b_bn;
      ASSERT_EQ(bn_t(a * b), c_bn);

      ASSERT_EQ(a + zero, a);
      ASSERT_EQ(a - zero, a);
      ASSERT_EQ(zero - a, -a);
      ASSERT_EQ(a - b, -(b - a));
      ASSERT_NE(a + one, a);
      ASSERT_NE(a - one, a);
      ASSERT_EQ(a + b, b + a);
      ASSERT_EQ(a * one, a);
      ASSERT_EQ(a * zero, zero);
      ASSERT_EQ(a * b, b * a);
      ASSERT_EQ(a + (b + c), (a + b) + c);
      ASSERT_EQ(a * (b + c), a * b + a * c);
      ASSERT_EQ(a * (b - c), a * b - a * c);

      b = a;
      ASSERT_EQ(a * a.inv_mod(q), one);
    }
  }
}

TEST(BigNumber256, MontgomeryMatchesReferenceOnSecp256k1) {
  const mod_t& q = curve_secp256k1.order();

  const bn_t a = bn_t::rand(q);
  const bn_t b = bn_t::rand(q);
  const bn_t mul_expected = q.mul(a, b);
  const bn256_t a256(a);
  const bn256_t b256(b);

  MODULO(q) {
    ASSERT_EQ((bn_t)a256.to_mont().from_mont(), a);
    ASSERT_EQ((bn_t)bn256_t::mont_mul(a256.to_mont(), b256.to_mont()).from_mont(), mul_expected);
  }
}

TEST(BigNumber256, SerializationRoundTripAndBoundaries) {
  const mod_t& q = curve_secp256k1.order();
  const bn256_t zero = 0;
  const bn256_t one = 1;
  const bn256_t max_value(q.value() - 1);

  for (const bn256_t& value : {zero, one, max_value}) {
    const buf_t bin = value.to_bin();
    ASSERT_EQ(bin.size(), 32);
    const bn256_t roundtrip = bn256_t::from_bin(bin);
    EXPECT_TRUE(roundtrip == value);
  }

  buf_t short_bin(31);
  memset(short_bin.data(), 0, 31);
  EXPECT_CB_ASSERT(bn256_t::from_bin(short_bin), "bin.size == 32");
}

TEST(BigNumber256, ConvertRoundTrip) {
  const bn256_t value = bn256_t::rand(curve_secp256k1.order());
  const buf_t serialized = coinbase::convert(value);

  bn256_t roundtrip;
  ASSERT_EQ(coinbase::convert(roundtrip, serialized), SUCCESS);
  EXPECT_TRUE(roundtrip == value);
}

TEST(BigNumber256, UInt256StringHelpersAndLittleEndianRoundTrip) {
  const uint256_t from_hex = uint256_t::from_hex("010203");
  const uint256_t from_dec = uint256_t::from_str("66051");
  EXPECT_TRUE(from_hex == from_dec);
  EXPECT_FALSE(from_hex.is_zero());
  EXPECT_TRUE(from_hex.is_odd());

  const buf_t little_endian = from_hex.to_bin_le();
  ASSERT_EQ(little_endian.size(), 32);
  const uint256_t roundtrip = uint256_t::from_bin_le(little_endian);
  EXPECT_TRUE(roundtrip == from_hex);
}

TEST(BigNumber256, UInt256ReferenceOperations) {
  const uint256_t a = uint256_t::from_hex("1234567890abcdeffedcba0987654321112233445566778899aabbccddeeff01");
  const uint256_t b = uint256_t::from_hex("0fedcba98765432100112233445566778899aabbccddeeff1020304050607080");
  EXPECT_EQ(a.to_bn(), bn_t::from_hex("1234567890abcdeffedcba0987654321112233445566778899aabbccddeeff01"));
  EXPECT_TRUE(a != b);

  uint256_t acc = uint256_t::make(5);
  const uint64_t carry = acc.mul_add_regular(a, 7);
  EXPECT_EQ(acc.to_bn() + (bn_t(int(carry)) << 256), bn_t(5) + a.to_bn() * 7);

  uint64_t mul_regular[8] = {0};
  uint256_t::mul(mul_regular, a, b);
  EXPECT_EQ(limbs_to_bn(mul_regular), a.to_bn() * b.to_bn());

  uint64_t sqr_noasm[8] = {0};
  uint64_t sqr_regular[8] = {0};
  uint256_t::sqr_noasm(sqr_noasm, a);
  uint256_t::sqr(sqr_regular, a);
  EXPECT_EQ(limbs_to_bn(sqr_noasm), a.to_bn() * a.to_bn());
  EXPECT_EQ(limbs_to_bn(sqr_regular), a.to_bn() * a.to_bn());

  const uint256_t q = uint256_t::from_bn(curve_secp256k1.order().value());
  EXPECT_EQ(uint256_t::get_mont_rr(q).to_bn(), (bn_t(1) << 512) % curve_secp256k1.order());

  const bn_t wide = (bn_t(1) << 300) + (bn_t(0x1234) << 128) + 0x5678;
  uint320_t parsed_wide;
  parsed_wide.from_bn(wide);
  EXPECT_EQ(wide_to_bn(parsed_wide), wide);

  const uint320_t mu = uint320_t::get_barrett_mu(q);
  EXPECT_EQ(wide_to_bn(mu), (bn_t(1) << 512) / q.to_bn());
}

TEST(BigNumber256, ComparisonsBitsPowersAndCompoundOperators) {
  bn256_t value;
  value = uint64_t(5);
  EXPECT_TRUE(value == 5);
  EXPECT_FALSE(value != 5);

  const bn_t assigned = (bn_t(1) << 200) + 17;
  value = assigned;
  EXPECT_EQ(bn_t(value), assigned);

  const bn256_t a = bn_t(17);
  const bn256_t b = bn_t(23);
  EXPECT_TRUE(a != b);
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(a <= b);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(b >= a);
  EXPECT_TRUE(a <= a);
  EXPECT_TRUE(a >= a);

  for (const int bit : {0, 63, 64, 127, 128, 191, 192, 255}) {
    EXPECT_EQ(bn_t(bn256_t::two_to_pow(bit)), bn_t(1) << bit);
  }

  const bn_t bit_pattern_bn = (bn_t(1) << 130) + (bn_t(1) << 63) + 1;
  const bn256_t bit_pattern(bit_pattern_bn);
  EXPECT_EQ(bit_pattern.get_bit(0), 1);
  EXPECT_EQ(bit_pattern.get_bit(63), 1);
  EXPECT_EQ(bit_pattern.get_bit(64), 0);
  EXPECT_EQ(bit_pattern.get_bit(130), 1);
  EXPECT_EQ(bit_pattern.get_bit(255), 0);
  EXPECT_EQ(bit_pattern.get_bit(256), 0);

  const mod_t& q = curve_secp256k1.order();
  MODULO(q) {
    bn256_t x = bn_t(17);
    const bn256_t y = bn_t(5);

    x += y;
    EXPECT_EQ(bn_t(x), bn_t(17) + bn_t(5));
    x -= y;
    EXPECT_EQ(bn_t(x), bn_t(17));
    x *= y;
    EXPECT_EQ(bn_t(x), bn_t(17) * bn_t(5));
    x /= y;
    EXPECT_EQ(bn_t(x), bn_t(17));
    EXPECT_EQ(bn_t(bn256_t(17) / y), bn_t(17) / bn_t(5));
  }
}

TEST(BigNumber256, RawAccumulatorsAndSumsMatchReference) {
  const mod_t& q = curve_secp256k1.order();
  const bn256_t a = bn_t(3);
  const bn256_t b = bn_t(5);
  const bn256_t c = bn_t(7);
  const bn_t q_minus_2 = q.value() - 2;
  const std::vector<bn256_t> values = {bn256_t(1), bn256_t(q_minus_2), bn256_t(9)};

  MODULO(q) {
    uint64_t acc[8] = {0};
    bn256_t::add_no_reduce(acc, a);
    bn256_t::mul_add_no_reduce(acc, b, c);
    EXPECT_EQ(bn_t(bn256_t::reduce(acc)), bn_t(3) + bn_t(5) * bn_t(7));

    EXPECT_EQ(bn_t(SUM_MOD(values, q)), bn_t(1) + q_minus_2 + bn_t(9));
    EXPECT_EQ(SUM(values), SUM_MOD(values, q));

    const std::vector<bn256_t> empty;
    EXPECT_EQ(SUM_MOD(empty, q), bn256_t(0));
  }
}

TEST(BigNumber256, ConvertRejectsTruncatedInput) {
  const bn256_t value = bn256_t::rand(curve_secp256k1.order());
  buf_t serialized = coinbase::convert(value);
  serialized.resize(31);

  bn256_t decoded;
  EXPECT_ER(coinbase::convert(decoded, serialized));
}

}  // namespace
