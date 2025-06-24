#include <gtest/gtest.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

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
}  // namespace
