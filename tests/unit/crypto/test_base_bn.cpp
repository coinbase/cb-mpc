#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/ro.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

TEST(BigNumber, Addition) {
  EXPECT_EQ(bn_t(123) + bn_t(456), 579);
  EXPECT_EQ(bn_t(-123) + bn_t(456), 333);
  EXPECT_EQ(bn_t(123) + bn_t(-456), -333);
  EXPECT_EQ(bn_t(-123) + bn_t(-456), -579);
  EXPECT_EQ(bn_t(1) + bn_t(999), 1000);
  EXPECT_EQ(bn_t(999) + bn_t(0), 999);
}

TEST(BigNumber, Subtraction) {
  EXPECT_EQ(bn_t(123) - bn_t(456), -333);
  EXPECT_EQ(bn_t(-123) - bn_t(456), -579);
  EXPECT_EQ(bn_t(123) - bn_t(-456), 579);
  EXPECT_EQ(bn_t(-123) - bn_t(-456), 333);
  EXPECT_EQ(bn_t(1) - bn_t(1000), -999);
  EXPECT_EQ(bn_t(999) - bn_t(0), 999);
}

TEST(BigNumber, IntOperatorsHandleIntMin) {
  const int v = INT_MIN;

  bn_t abs_v;
  abs_v.set_int64(2147483648LL);

  // Non-modular operators
  EXPECT_EQ(bn_t(0) + v, bn_t(v));
  EXPECT_EQ(bn_t(1) * v, bn_t(v));

  EXPECT_EQ(bn_t(0) - v, abs_v);
  EXPECT_EQ(bn_t(0) * v, 0);

  bn_t x = 0;
  EXPECT_NO_THROW(x += v);
  EXPECT_EQ(x, bn_t(v));

  bn_t y = 0;
  EXPECT_NO_THROW(y -= v);
  EXPECT_EQ(y, abs_v);

  bn_t z = 1;
  EXPECT_NO_THROW(z *= v);
  EXPECT_EQ(z, bn_t(v));

  // Modular path uses `mod_t::mod(int)` internally; ensure INT_MIN does not trigger UB.
  const mod_t& q = crypto::curve_ed25519.order();
  bn_t expected_mod;
  MODULO(q) { expected_mod = -abs_v; }
  MODULO(q) { EXPECT_EQ(bn_t(0) + v, expected_mod); }
}

TEST(BigNumber, Multiplication) {
  EXPECT_EQ(bn_t(123) * bn_t(456), 56088);
  EXPECT_EQ(bn_t(-123) * bn_t(456), -56088);
  EXPECT_EQ(bn_t(123) * bn_t(-456), -56088);
  EXPECT_EQ(bn_t(-123) * bn_t(-456), 56088);
  EXPECT_EQ(bn_t(1) * bn_t(1000), 1000);
  EXPECT_EQ(bn_t(999) * bn_t(0), 0);
}

TEST(BigNumber, GCD) {
  EXPECT_EQ(bn_t::gcd(123, 456), 3);
  EXPECT_EQ(bn_t::gcd(0, 456), 456);
}

// Newly added tests:
TEST(BigNumber, Pow) {
  bn_t base(2);
  bn_t exponent(10);
  bn_t result = bn_t::pow(base, exponent);
  EXPECT_EQ(result, 1024);

  // Testing negative exponent base scenario (exponent is still int64)
  bn_t base_neg(-2);
  bn_t exponent2(3);
  bn_t result2 = bn_t::pow(base_neg, exponent2);
  EXPECT_EQ(result2, -8);
}

TEST(BigNumber, PowMod) {
  // 3^5 mod 13 = 243 mod 13 = 9
  bn_t base(3);
  bn_t exponent(5);
  mod_t mod13(13);
  bn_t result = base.pow_mod(exponent, mod13);
  EXPECT_EQ(result, 9);
}

TEST(BigNumber, Neg) {
  bn_t val1(-123);
  bn_t neg1 = val1.neg();
  EXPECT_EQ(neg1, 123);

  bn_t val2(456);
  bn_t neg2 = val2.neg();
  EXPECT_EQ(neg2, -456);
}

TEST(BigNumber, ShiftOperators) {
  bn_t val(1);
  val <<= 10;  // Shift left by 10
  EXPECT_EQ(val, 1024);

  val >>= 5;  // Shift right by 5
  EXPECT_EQ(val, 32);

  // Next test using operator<< and operator>>
  bn_t val2 = bn_t(5) << 3;
  EXPECT_EQ(val2, 40);

  bn_t val3 = val2 >> 2;
  EXPECT_EQ(val3, 10);

  // Negative shifts are treated as no-ops.
  bn_t neg1(32);
  neg1 <<= -5;
  EXPECT_EQ(neg1, 32);

  bn_t neg2(1);
  neg2 >>= -10;
  EXPECT_EQ(neg2, 1);

  EXPECT_EQ(bn_t(5) << -3, 5);
  EXPECT_EQ(bn_t(10) >> -2, 10);
}

TEST(BigNumber, BitwiseSetAndCheck) {
  bn_t val(0);
  val.set_bit(3, true);  // set the 3rd bit
  EXPECT_TRUE(val.is_bit_set(3));
  EXPECT_FALSE(val.is_bit_set(2));
  EXPECT_EQ(val, 8);

  // Clearing the bit again
  val.set_bit(3, false);
  EXPECT_FALSE(val.is_bit_set(3));
  EXPECT_EQ(val, 0);
}

TEST(BigNumber, GeneratePrime) {
  // This test checks that generated prime has the right bit length.
  // It also checks prime-ness but the real test
  // might require a larger bit length to be meaningful.
  bn_t prime = bn_t::generate_prime(64, false);
  EXPECT_TRUE(prime.prime());
  EXPECT_GE(prime.get_bits_count(), 63);  // Should be close to 64 bits
}

TEST(BigNumber, RangeCheck) {
  EXPECT_ER_MSG(check_closed_range(bn_t(3), bn_t(2), bn_t(5)), "check_closed_range failed");
  EXPECT_OK(check_closed_range(bn_t(3), bn_t(3), bn_t(5)));
  EXPECT_OK(check_closed_range(bn_t(3), bn_t(4), bn_t(5)));
  EXPECT_OK(check_closed_range(bn_t(3), bn_t(5), bn_t(5)));
  EXPECT_ER_MSG(check_closed_range(bn_t(3), bn_t(6), bn_t(5)), "check_closed_range failed");

  EXPECT_ER_MSG(check_right_open_range(bn_t(3), bn_t(2), bn_t(5)), "check_right_open_range failed");
  EXPECT_OK(check_right_open_range(bn_t(3), bn_t(3), bn_t(5)));
  EXPECT_OK(check_right_open_range(bn_t(3), bn_t(4), bn_t(5)));
  EXPECT_ER_MSG(check_right_open_range(bn_t(3), bn_t(5), bn_t(5)), "check_right_open_range failed");

  EXPECT_ER_MSG(check_open_range(bn_t(3), bn_t(3), bn_t(5)), "check_open_range failed");
  EXPECT_OK(check_open_range(bn_t(3), bn_t(4), bn_t(5)));
  EXPECT_ER_MSG(check_open_range(bn_t(3), bn_t(5), bn_t(5)), "check_open_range failed");
}

TEST(BigNumber, FromStringValidInput) {
  bn_t result;
  EXPECT_OK(bn_t::from_string("0", result));
  EXPECT_EQ(result, 0);
  EXPECT_OK(bn_t::from_string("12345", result));
  EXPECT_EQ(result, 12345);
  EXPECT_OK(bn_t::from_string("-42", result));
  EXPECT_EQ(result, -42);
}

TEST(BigNumber, FromStringRejectsInvalidInput) {
  bn_t result;
  EXPECT_ER(bn_t::from_string("", result));
  EXPECT_ER(bn_t::from_string("not_a_number", result));
  EXPECT_ER(bn_t::from_string("1234ncc", result));
  EXPECT_ER(bn_t::from_string("0xAB", result));
  EXPECT_ER(bn_t::from_string(nullptr, result));
}

TEST(BigNumber, FromHexValidInput) {
  bn_t result;
  EXPECT_OK(bn_t::from_hex("0", result));
  EXPECT_EQ(result, 0);
  EXPECT_OK(bn_t::from_hex("FF", result));
  EXPECT_EQ(result, 255);
  EXPECT_OK(bn_t::from_hex("-1A", result));
  EXPECT_EQ(result, -26);
  EXPECT_OK(bn_t::from_hex("abc", result));
  EXPECT_EQ(result, 0xabc);
}

TEST(BigNumber, FromHexRejectsInvalidInput) {
  bn_t result;
  EXPECT_ER(bn_t::from_hex("", result));
  EXPECT_ER(bn_t::from_hex("0xAB", result));
  EXPECT_ER(bn_t::from_hex("ZZZZ", result));
  EXPECT_ER(bn_t::from_hex("1234ncc", result));
  EXPECT_ER(bn_t::from_hex(nullptr, result));
}

TEST(BigNumber, CompareMatchesOpenSSL) {
  auto expect_cmp = [](const bn_t& a, const bn_t& b) {
    int ct = bn_t::compare(a, b);
    int ref = BN_cmp((const BIGNUM*)a, (const BIGNUM*)b);
    EXPECT_EQ((ct < 0), (ref < 0));
    EXPECT_EQ((ct == 0), (ref == 0));
    EXPECT_EQ((ct > 0), (ref > 0));
  };

  // Same sign / positive
  expect_cmp(bn_t(0), bn_t(0));
  expect_cmp(bn_t(1), bn_t(2));
  expect_cmp(bn_t(2), bn_t(1));

  // Mixed sign
  expect_cmp(bn_t(-1), bn_t(0));
  expect_cmp(bn_t(0), bn_t(-1));
  expect_cmp(bn_t(-5), bn_t(7));
  expect_cmp(bn_t(7), bn_t(-5));

  // Both negative (this is where a naive magnitude compare is wrong)
  expect_cmp(bn_t(-1), bn_t(-2));
  expect_cmp(bn_t(-2), bn_t(-1));

  // Multi-limb pattern similar to the report (constructed via shifts/adds).
  bn_t a;
  BN_zero(a);
  BN_set_word(a, (BN_ULONG)2);
  BN_lshift(a, a, 64);
  BN_add_word(a, (BN_ULONG)0);
  BN_set_negative(a, 1);

  bn_t b;
  BN_zero(b);
  BN_set_word(b, (BN_ULONG)1);
  BN_lshift(b, b, 64);
  BN_add_word(b, (BN_ULONG)0xFFFFFFFFFFFFFFFFULL);
  BN_set_negative(b, 1);

  expect_cmp(a, b);
  expect_cmp(b, a);
}

TEST(BigNumber, GetBinSize) {
  // Test basic cases
  EXPECT_EQ(bn_t(0).get_bin_size(), 0);  // Zero takes 0 bytes in binary representation
  EXPECT_EQ(bn_t(1).get_bin_size(), 1);
  EXPECT_EQ(bn_t(127).get_bin_size(), 1);
  EXPECT_EQ(bn_t(255).get_bin_size(), 1);    // Maximum 1-byte value
  EXPECT_EQ(bn_t(256).get_bin_size(), 2);    // Minimum 2-byte value
  EXPECT_EQ(bn_t(65535).get_bin_size(), 2);  // Maximum 2-byte value
  EXPECT_EQ(bn_t(65536).get_bin_size(), 3);  // Minimum 3-byte value

  // Test negative numbers
  EXPECT_EQ(bn_t(-1).get_bin_size(), 1);
  EXPECT_EQ(bn_t(-255).get_bin_size(), 1);
  EXPECT_EQ(bn_t(-256).get_bin_size(), 2);

  // Test that the leading zero will not be considered
  bn_t a(1);
  MODULO(crypto::curve_ed25519.order()) { a += 0; };
  EXPECT_EQ(a, 1);
  EXPECT_EQ(a.get_bin_size(), 1);
}

TEST(BigNumber, ConvertDeserializeOversizedNoThrow) {
  const uint32_t value_size = bn_t::MAX_SERIALIZED_BIGNUM_BYTES + 1;
  const uint32_t header = value_size << 1;  // neg=0

  int header_size = 0;
  {
    converter_t sizer(true);
    uint32_t tmp = header;
    sizer.convert_len(tmp);
    header_size = sizer.get_offset();
  }

  ASSERT_GT(header_size, 0);
  ASSERT_LE(value_size, static_cast<uint32_t>(INT_MAX - header_size));
  const int value_size_int = static_cast<int>(value_size);

  buf_t buffer(header_size + value_size_int);
  {
    converter_t writer(buffer.data());
    uint32_t tmp = header;
    writer.convert_len(tmp);
    memset(writer.current(), 0x7F, static_cast<size_t>(value_size_int));
    writer.forward(value_size_int);
  }

  bn_t result;
  converter_t reader{mem_t(buffer)};
  EXPECT_NO_THROW(result.convert(reader));
  EXPECT_TRUE(reader.is_error());
  EXPECT_EQ(reader.get_offset(), header_size);
}
}  // namespace
