#include <cstdint>
#include <gtest/gtest.h>

#include <cbmpc/core/buf.h>

using namespace coinbase;

TEST(Buf128, MakeAndAccess) {
  // Make a buf128 with a known lo and hi
  buf128_t b = buf128_t::make(0x1122334455667788ULL, 0x99AABBCCDDEEFF00ULL);

  // Check lo() and hi()
  EXPECT_EQ(b.lo(), 0x1122334455667788ULL);
  EXPECT_EQ(b.hi(), 0x99AABBCCDDEEFF00ULL);

  // Check that buf128_t::zero() works
  buf128_t z;
  z = nullptr;  // sets to zero
  EXPECT_EQ(z.lo(), 0ULL);
  EXPECT_EQ(z.hi(), 0ULL);
}

TEST(Buf128, Equality) {
  // Prepare two identical objects
  auto b1 = buf128_t::make(0x1234567890ABCDEFULL, 0x0123456789ABCDEFULL);
  auto b2 = buf128_t::make(0x1234567890ABCDEFULL, 0x0123456789ABCDEFULL);
  // Prepare a different object
  auto b3 = buf128_t::make(0xFFFFULL, 0ULL);

  EXPECT_TRUE(b1 == b2);
  EXPECT_FALSE(b1 != b2);

  EXPECT_FALSE(b1 == b3);
  EXPECT_TRUE(b1 != b3);

  // Check equality to nullptr
  auto z = buf128_t::make(0ULL, 0ULL);
  EXPECT_TRUE(z == nullptr);
  EXPECT_FALSE(z != nullptr);
}

TEST(Buf128, BitManipulation) {
  // Test get_bit / set_bit
  buf128_t b = buf128_t::make(0ULL, 0ULL);
  EXPECT_FALSE(b.get_bit(0));
  b.set_bit(0, true);
  EXPECT_TRUE(b.get_bit(0));
  b.set_bit(0, false);
  EXPECT_FALSE(b.get_bit(0));

  // Set a higher bit (e.g. bit 70)
  b.set_bit(70, true);
  EXPECT_TRUE(b.get_bit(70));
  // Check that the rest are false
  EXPECT_FALSE(b.get_bit(69));
  EXPECT_FALSE(b.get_bit(71));

  // Test get_bits_count
  EXPECT_EQ(b.get_bits_count(), 1);
  // Turn on two more bits
  b.set_bit(0, true);
  b.set_bit(127, true);
  EXPECT_TRUE(b.get_bit(127));
  EXPECT_EQ(b.get_bits_count(), 3);
}

TEST(Buf128, MSBLSB) {
  // Make a buf128
  auto b = buf128_t::make(0x0000000000000001ULL, 0ULL);
  EXPECT_TRUE(b.lsb());   // LSB set
  EXPECT_FALSE(b.msb());  // MSB not set

  auto b2 = buf128_t::make(0ULL, 0x8000000000000000ULL);  // sign bit set in hi
  EXPECT_FALSE(b2.lsb());
  EXPECT_TRUE(b2.msb());
}

TEST(Buf128, BitwiseOperations) {
  auto b1 = buf128_t::make(0xFFFF0000FFFF0000ULL, 0xABCD1234ABCD1234ULL);
  auto b2 = buf128_t::make(0x1234567890ABCDEFULL, 0xFFFF0000FFFF0000ULL);

  // NOT
  auto b_not = ~b1;
  EXPECT_EQ(~b_not, b1);

  // AND
  auto b_and = b1 & b2;
  EXPECT_EQ(b_and.lo(), (0xFFFF0000FFFF0000ULL & 0x1234567890ABCDEFULL));
  EXPECT_EQ(b_and.hi(), (0xABCD1234ABCD1234ULL & 0xFFFF0000FFFF0000ULL));

  // OR
  auto b_or = b1 | b2;
  EXPECT_EQ(b_or.lo(), (0xFFFF0000FFFF0000ULL | 0x1234567890ABCDEFULL));
  EXPECT_EQ(b_or.hi(), (0xABCD1234ABCD1234ULL | 0xFFFF0000FFFF0000ULL));

  // XOR
  auto b_xor = b1 ^ b2;
  EXPECT_EQ(b_xor.lo(), (0xFFFF0000FFFF0000ULL ^ 0x1234567890ABCDEFULL));
  EXPECT_EQ(b_xor.hi(), (0xABCD1234ABCD1234ULL ^ 0xFFFF0000FFFF0000ULL));

  // AND with bool
  auto b1_and_true = b1 & true;
  EXPECT_EQ(b1_and_true, b1);
  auto b1_and_false = b1 & false;
  EXPECT_EQ(b1_and_false.lo(), 0ULL);
  EXPECT_EQ(b1_and_false.hi(), 0ULL);
}

TEST(Buf128, Shifts) {
  // left shift
  auto b = buf128_t::make(0x00000000000000FFULL, 0ULL);
  b = b << 8;
  EXPECT_EQ(b.lo(), 0x000000000000FF00ULL);
  EXPECT_EQ(b.hi(), 0ULL);

  // shifting past 64 bits
  b = b << 64;
  EXPECT_EQ(b.lo(), 0ULL);
  EXPECT_EQ(b.hi(), 0x000000000000FF00ULL);

  // right shift
  auto c = buf128_t::make(0ULL, 0x1122334455667788ULL);
  c = c >> 8;
  // hi part gets shifted right by 8 bits, some bits come from low?
  // But in this case, lo = 0, so we'll see a straightforward shift.
  EXPECT_EQ(c.hi(), 0x0011223344556677ULL);
  EXPECT_EQ(c.lo(), 0x88ULL << (64 - 8));  // part from the original hi
}

TEST(Buf128, ReverseBytes) {
  auto b_in = buf128_t::make(0x1122334455667788ULL, 0x99AABBCCDDEEFF00ULL);
  auto b_out = b_in.reverse_bytes();

  // After reversing, the low 8 bytes become the reversed high 8 bytes
  // and the high 8 bytes become the reversed low 8 bytes
  // So let's confirm manually
  // Original hi:  99 AA BB CC DD EE FF 00
  // Original lo:  11 22 33 44 55 66 77 88
  // Reversed hi:  88 77 66 55 44 33 22 11
  // Reversed lo:  00 FF EE DD CC BB AA 99
  EXPECT_EQ(b_out.hi(), 0x8877665544332211ULL);
  EXPECT_EQ(b_out.lo(), 0x00FFEEDDCCBBAA99ULL);
}

TEST(Buf128, FromBitIndex) {
  // from_bit_index sets single bit
  auto b = buf128_t::from_bit_index(63);
  EXPECT_EQ(b.lo(), 1ULL << 63);
  EXPECT_EQ(b.hi(), 0ULL);

  // test bit >=64
  auto b2 = buf128_t::from_bit_index(64);
  EXPECT_EQ(b2.lo(), 0ULL);
  EXPECT_EQ(b2.hi(), 1ULL << 0);

  auto b3 = buf128_t::from_bit_index(127);
  EXPECT_EQ(b3.lo(), 0ULL);
  EXPECT_EQ(b3.hi(), 1ULL << 63);
}