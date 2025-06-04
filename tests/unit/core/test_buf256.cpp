#include <gtest/gtest.h>

#include <cbmpc/core/buf.h>

using namespace coinbase;

TEST(Buf256Test, MakeAndZero) {
  // Check zero
  buf256_t z = buf256_t::zero();
  EXPECT_TRUE(z == nullptr);  // zero equivalence
  EXPECT_FALSE(z != nullptr);

  // Make a buf256 with known lo and hi
  auto lo_part = buf128_t::make(0x1122334455667788ULL, 0x99AABBCCDDEEFF00ULL);
  auto hi_part = buf128_t::make(0x0101010101010101ULL, 0xA0A1A2A3A4A5A6A7ULL);
  buf256_t b = buf256_t::make(lo_part, hi_part);

  // Verify
  EXPECT_EQ(b.lo.lo(), lo_part.lo());
  EXPECT_EQ(b.lo.hi(), lo_part.hi());
  EXPECT_EQ(b.hi.lo(), hi_part.lo());
  EXPECT_EQ(b.hi.hi(), hi_part.hi());
}

TEST(Buf256Test, Equality) {
  auto b1 = buf256_t::make(buf128_t::make(0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL),
                           buf128_t::make(0x1111111122222222ULL, 0x3333333344444444ULL));
  auto b2 = buf256_t::make(buf128_t::make(0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL),
                           buf128_t::make(0x1111111122222222ULL, 0x3333333344444444ULL));
  auto b3 = buf256_t::make(buf128_t::make(0xABABABABABABABABULL, 0xFFFFFFFF00000000ULL),
                           buf128_t::make(0x1111111122222222ULL, 0x3333333344444444ULL));

  EXPECT_TRUE(b1 == b2);
  EXPECT_FALSE(b1 != b2);

  EXPECT_FALSE(b1 == b3);
  EXPECT_TRUE(b1 != b3);

  // Check equality to nullptr (i.e. zero)
  auto z = buf256_t::zero();
  EXPECT_TRUE(z == nullptr);
  EXPECT_FALSE(z != nullptr);
}

TEST(Buf256Test, BitManipulation) {
  buf256_t b = buf256_t::zero();
  // Lowest bit
  EXPECT_FALSE(b.get_bit(0));
  b.set_bit(0, true);
  EXPECT_TRUE(b.get_bit(0));
  // Turn it off
  b.set_bit(0, false);
  EXPECT_FALSE(b.get_bit(0));

  // High bit (e.g. bit 200)
  b.set_bit(200, true);
  EXPECT_TRUE(b.get_bit(200));
  EXPECT_FALSE(b.get_bit(199));
  EXPECT_FALSE(b.get_bit(201));

  // Turn on two more bits
  b.set_bit(63, true);
  b.set_bit(128, true);

  // Confirm
  EXPECT_TRUE(b.get_bit(63));
  EXPECT_TRUE(b.get_bit(128));
}

TEST(Buf256Test, BitwiseOperations) {
  auto lo1 = buf128_t::make(0xFFFF0000FFFF0000ULL, 0xABCD1234ABCD1234ULL);
  auto hi1 = buf128_t::make(0x1111FFFF2222FFFFULL, 0xAABBCCDDEEFF0011ULL);
  auto b1 = buf256_t::make(lo1, hi1);

  auto lo2 = buf128_t::make(0x1234567890ABCDEFULL, 0xFFFF0000FFFF0000ULL);
  auto hi2 = buf128_t::make(0x9999999999999999ULL, 0x000000000000FFFFULL);
  auto b2 = buf256_t::make(lo2, hi2);

  // NOT
  auto b_not = ~b1;
  EXPECT_EQ(~b_not, b1);

  // AND
  auto b_and = b1 & b2;
  EXPECT_EQ(b_and.lo.lo(), lo1.lo() & lo2.lo());
  EXPECT_EQ(b_and.lo.hi(), lo1.hi() & lo2.hi());
  EXPECT_EQ(b_and.hi.lo(), hi1.lo() & hi2.lo());
  EXPECT_EQ(b_and.hi.hi(), hi1.hi() & hi2.hi());

  // OR
  auto b_or = b1 | b2;
  EXPECT_EQ(b_or.lo.lo(), lo1.lo() | lo2.lo());
  EXPECT_EQ(b_or.lo.hi(), lo1.hi() | lo2.hi());
  EXPECT_EQ(b_or.hi.lo(), hi1.lo() | hi2.lo());
  EXPECT_EQ(b_or.hi.hi(), hi1.hi() | hi2.hi());

  // XOR
  auto b_xor = b1 ^ b2;
  EXPECT_EQ(b_xor.lo.lo(), lo1.lo() ^ lo2.lo());
  EXPECT_EQ(b_xor.lo.hi(), lo1.hi() ^ lo2.hi());
  EXPECT_EQ(b_xor.hi.lo(), hi1.lo() ^ hi2.lo());
  EXPECT_EQ(b_xor.hi.hi(), hi1.hi() ^ hi2.hi());

  // AND with bool
  auto b1_and_false = b1 & false;
  EXPECT_EQ(b1_and_false.lo.lo(), 0ULL);
  EXPECT_EQ(b1_and_false.lo.hi(), 0ULL);
  EXPECT_EQ(b1_and_false.hi.lo(), 0ULL);
  EXPECT_EQ(b1_and_false.hi.hi(), 0ULL);

  auto b2_and_true = b2 & true;
  EXPECT_EQ(b2_and_true.lo.lo(), b2.lo.lo());
  EXPECT_EQ(b2_and_true.lo.hi(), b2.lo.hi());
  EXPECT_EQ(b2_and_true.hi.lo(), b2.hi.lo());
  EXPECT_EQ(b2_and_true.hi.hi(), b2.hi.hi());
}

TEST(Buf256Test, Shifts) {
  // left shift
  auto lo = buf128_t::make(0x00000000000000FFULL, 0ULL);
  auto hi = buf128_t::make(0ULL, 0ULL);
  auto b = buf256_t::make(lo, hi);

  // Shift by 8
  b = b << 8;
  EXPECT_EQ(b.lo.lo(), 0x000000000000FF00ULL);
  EXPECT_EQ(b.lo.hi(), 0ULL);
  EXPECT_EQ(b.hi.lo(), 0ULL);
  EXPECT_EQ(b.hi.hi(), 0ULL);

  // Shift by 64
  b = b << 64;
  EXPECT_EQ(b.lo.lo(), 0ULL);
  EXPECT_EQ(b.lo.hi(), 0x000000000000FF00ULL);
  EXPECT_EQ(b.hi.lo(), 0ULL);
  EXPECT_EQ(b.hi.hi(), 0ULL);

  // right shift
  auto lo2 = buf128_t::make(0ULL, 0ULL);
  auto hi2 = buf128_t::make(0x1122334455667788ULL, 0x99AABBCCDDEEFF00ULL);
  auto c = buf256_t::make(lo2, hi2);

  // Shift by 8
  c = c >> 8;
  // hi2 is right-shifted by 8, lower bits come from lo2 (which is 0)
  EXPECT_EQ(c.hi.lo(), 0x0011223344556677ULL);
  EXPECT_EQ(c.hi.hi(), 0x0099AABBCCDDEEFFULL);
  // // Also check any bits that shifted into lo
  EXPECT_NE(c.lo.hi(), 0ULL);  // part of the original hi might shift into lo
}

TEST(Buf256Test, ReverseBytes) {
  buf128_t lo = buf128_t::make(0x1122334455667788ULL, 0x99AABBCCDDEEFF00ULL);
  buf128_t hi = buf128_t::make(0x0001020304050607ULL, 0x08090A0B0C0D0E0FULL);
  auto b_in = buf256_t::make(lo, hi);

  // Reverse bytes
  auto b_out = b_in.reverse_bytes();

  // Manually check
  // b_in.lo:  (lowest 16 bytes)
  //   byte 0..7:  88 77 66 55 44 33 22 11
  //   byte 8..15: 00 FF EE DD CC BB AA 99
  // b_in.hi:  (highest 16 bytes)
  //   byte 16..23: 07 06 05 04 03 02 01 00
  //   byte 24..31: 0F 0E 0D 0C 0B 0A 09 08
  // After reversing, the highest 16 bytes become reversed lo, etc.
  // We'll just do a sanity check that the reversed bytes match the original
  // in reverse order.
  // Check that reversing again yields the original
  EXPECT_EQ(b_out.reverse_bytes().lo.lo(), b_in.lo.lo());
  EXPECT_EQ(b_out.reverse_bytes().lo.hi(), b_in.lo.hi());
  EXPECT_EQ(b_out.reverse_bytes().hi.lo(), b_in.hi.lo());
  EXPECT_EQ(b_out.reverse_bytes().hi.hi(), b_in.hi.hi());
}

TEST(Buf256Test, CarrylessMul) {
  // Quick check: carryless multiply of two small values
  auto a = buf128_t::from_bit_index(0);  // 1
  auto b = buf128_t::from_bit_index(1);  // 2
  auto r = buf256_t::caryless_mul(a, b);
  // 1 SHIFT 1 bit => 1 << 1 => 2 => 10 in binary => let's see
  // We can check that bit #1 in the result is set
  EXPECT_TRUE(r.get_bit(1));
  EXPECT_FALSE(r.get_bit(0));
  // Additional bigger test
  auto a2 = buf128_t::make(0xFFFF0000FFFF0000ULL, 0x1122334455667788ULL);
  auto b2 = buf128_t::make(0x1234567890ABCDEFULL, 0xAABBCCDDEEFF0011ULL);
  auto r2 = buf256_t::caryless_mul(a2, b2);
  // Not trivial to hand-check, but ensure we do not crash and we get a consistent result
  // We can simply verify it's not zero
  EXPECT_FALSE(r2 == buf256_t::zero());
}
