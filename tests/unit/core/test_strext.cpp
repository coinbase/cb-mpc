#include <gtest/gtest.h>

#include <cbmpc/internal/core/strext.h>

namespace {

using coinbase::buf_t;
using coinbase::mem_t;

TEST(CoreStrext, TokenizeDiscardsEmptyFieldsAndTrimsTokens) {
  EXPECT_EQ(strext::tokenize(" alpha, beta,,gamma ", ","), (std::vector<std::string>{"alpha", "beta", "gamma"}));
  EXPECT_TRUE(strext::tokenize("", ",").empty());
}

TEST(CoreStrext, ItoaFormatsSignedIntegers) {
  EXPECT_EQ(strext::itoa(0), "0");
  EXPECT_EQ(strext::itoa(42), "42");
  EXPECT_EQ(strext::itoa(-17), "-17");
}

TEST(CoreStrext, HexRoundTripsBuffersAndScalars) {
  const uint8_t raw[] = {0x00, 0x7f, 0x80, 0xff};
  EXPECT_EQ(strext::to_hex(mem_t(raw, sizeof(raw))), "007f80ff");
  EXPECT_EQ(strext::to_hex(uint8_t(0xab)), "ab");
  EXPECT_EQ(strext::to_hex(uint16_t(0xabcd)), "abcd");
  EXPECT_EQ(strext::to_hex(uint32_t(0x01234567)), "01234567");
  EXPECT_EQ(strext::to_hex(uint64_t(0x0123456789abcdef)), "0123456789abcdef");

  buf_t decoded;
  ASSERT_TRUE(strext::from_hex(decoded, "007F80ff"));
  ASSERT_EQ(decoded.size(), 4);
  EXPECT_EQ(strext::to_hex(decoded), "007f80ff");

  uint8_t u8 = 0;
  uint16_t u16 = 0;
  uint32_t u32 = 0;
  uint64_t u64 = 0;
  EXPECT_TRUE(strext::from_hex(u8, "aB"));
  EXPECT_TRUE(strext::from_hex(u16, "BEEF"));
  EXPECT_TRUE(strext::from_hex(u32, "01234567"));
  EXPECT_TRUE(strext::from_hex(u64, "0123456789abcdef"));
  EXPECT_EQ(u8, 0xab);
  EXPECT_EQ(u16, 0xbeef);
  EXPECT_EQ(u32, 0x01234567u);
  EXPECT_EQ(u64, 0x0123456789abcdefULL);
}

TEST(CoreStrext, HexRejectsMalformedInput) {
  buf_t decoded;
  EXPECT_FALSE(strext::from_hex(decoded, "abc"));
  EXPECT_FALSE(strext::from_hex(decoded, "00xz"));

  uint8_t u8 = 0;
  uint32_t u32 = 0;
  EXPECT_FALSE(strext::from_hex(u8, ""));
  EXPECT_FALSE(strext::from_hex(u8, "zz"));
  EXPECT_FALSE(strext::from_hex(u32, "001122"));
}

TEST(CoreStrext, MemWrapsStringBytes) {
  const std::string value = "abc";
  mem_t mem = strext::mem(value);
  EXPECT_EQ(mem.size, 3);
  EXPECT_EQ(memcmp(mem.data, "abc", 3), 0);
}

}  // namespace
