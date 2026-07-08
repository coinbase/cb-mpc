#include <gtest/gtest.h>
#include <map>
#include <vector>

#include <cbmpc/internal/core/utils.h>
#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/protocol/util.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

// Test bits_to_bytes and bytes_to_bits
TEST(CoreUtils, BitAndByteConversions) {
  EXPECT_EQ(bits_to_bytes_floor(0), 0);
  EXPECT_EQ(bits_to_bytes_floor(7), 0);
  EXPECT_EQ(bits_to_bytes_floor(8), 1);
  EXPECT_EQ(bits_to_bytes_floor(15), 1);

  EXPECT_EQ(bits_to_bytes(0), 0);
  EXPECT_EQ(bits_to_bytes(1), 1);
  EXPECT_EQ(bits_to_bytes(7), 1);
  EXPECT_EQ(bits_to_bytes(8), 1);
  EXPECT_EQ(bits_to_bytes(9), 2);
  EXPECT_EQ(bits_to_bytes(15), 2);
  EXPECT_EQ(bits_to_bytes(16), 2);

  EXPECT_EQ(bytes_to_bits(0), 0);
  EXPECT_EQ(bytes_to_bits(1), 8);
  EXPECT_EQ(bytes_to_bits(2), 16);
}

TEST(CoreUtils, BitAndByteConversionsRejectNegativeInputs) {
  EXPECT_CB_ASSERT(bits_to_bytes_floor(-1), "bits >= 0");
  EXPECT_CB_ASSERT(bits_to_bytes(-1), "bits >= 0");
  EXPECT_CB_ASSERT(bytes_to_bits(-1), "bytes >= 0");
}

// Test endianness functions
TEST(CoreUtils, Endianness) {
  // We'll use buffers to store/retrieve values and verify.
  unsigned char buf[9] = {0};
  byte_ptr unaligned = buf + 1;

  // Test little-endian get/set
  {
    uint16_t val16 = 0x1234;
    le_set_2(unaligned, val16);
    EXPECT_EQ(unaligned[0], 0x34);
    EXPECT_EQ(unaligned[1], 0x12);
    EXPECT_EQ(le_get_2(unaligned), val16);

    uint32_t val32 = 0x12345678;
    le_set_4(unaligned, val32);
    EXPECT_EQ(unaligned[0], 0x78);
    EXPECT_EQ(unaligned[1], 0x56);
    EXPECT_EQ(unaligned[2], 0x34);
    EXPECT_EQ(unaligned[3], 0x12);
    EXPECT_EQ(le_get_4(unaligned), val32);

    uint64_t val64 = 0x1234567890ABCDEFULL;
    le_set_8(unaligned, val64);
    EXPECT_EQ(unaligned[0], 0xEF);
    EXPECT_EQ(unaligned[1], 0xCD);
    EXPECT_EQ(unaligned[2], 0xAB);
    EXPECT_EQ(unaligned[3], 0x90);
    EXPECT_EQ(unaligned[4], 0x78);
    EXPECT_EQ(unaligned[5], 0x56);
    EXPECT_EQ(unaligned[6], 0x34);
    EXPECT_EQ(unaligned[7], 0x12);
    EXPECT_EQ(le_get_8(unaligned), val64);
  }

  // Test big-endian get/set
  {
    uint16_t val16 = 0x1234;
    be_set_2(unaligned, val16);
    EXPECT_EQ(unaligned[0], 0x12);
    EXPECT_EQ(unaligned[1], 0x34);
    EXPECT_EQ(be_get_2(unaligned), val16);

    uint32_t val32 = 0x12345678;
    be_set_4(unaligned, val32);
    EXPECT_EQ(unaligned[0], 0x12);
    EXPECT_EQ(unaligned[1], 0x34);
    EXPECT_EQ(unaligned[2], 0x56);
    EXPECT_EQ(unaligned[3], 0x78);
    EXPECT_EQ(be_get_4(unaligned), val32);

    uint64_t val64 = 0x1234567890ABCDEFULL;
    be_set_8(unaligned, val64);
    EXPECT_EQ(unaligned[0], 0x12);
    EXPECT_EQ(unaligned[1], 0x34);
    EXPECT_EQ(unaligned[2], 0x56);
    EXPECT_EQ(unaligned[3], 0x78);
    EXPECT_EQ(unaligned[4], 0x90);
    EXPECT_EQ(unaligned[5], 0xAB);
    EXPECT_EQ(unaligned[6], 0xCD);
    EXPECT_EQ(unaligned[7], 0xEF);
    EXPECT_EQ(be_get_8(unaligned), val64);
  }
}

// Test make_uint64
TEST(CoreUtils, MakeUInt64) {
  uint32_t lo = 0x89ABCDEF;
  uint32_t hi = 0x01234567;
  uint64_t combined = make_uint64(lo, hi);
  EXPECT_EQ(combined, 0x0123456789ABCDEFULL);
}

// Test int_log2
TEST(CoreUtils, Logarithms2) {
  EXPECT_EQ(int_log2(0), 0);
  EXPECT_EQ(int_log2(1), 1);
  EXPECT_EQ(int_log2(2), 32 - __builtin_clz(1));  // i.e. 1
  EXPECT_EQ(int_log2(3), 2);
  EXPECT_EQ(int_log2(8), 32 - __builtin_clz(7));
  EXPECT_EQ(int_log2(16), 32 - __builtin_clz(15));
  EXPECT_EQ(int_log2(17), 5);
}

// Test lookup in a std::map
TEST(CoreUtils, LookupInMap) {
  std::map<int, std::string> sampleMap = {{1, "one"}, {2, "two"}, {3, "three"}};

  auto [found1, value1] = lookup(sampleMap, 2);
  EXPECT_TRUE(found1);
  ASSERT_NE(value1, nullptr);
  EXPECT_EQ(*value1, "two");

  auto [found2, value2] = lookup(sampleMap, 99);
  EXPECT_FALSE(found2);
  EXPECT_EQ(value2, nullptr);

  sampleMap.at(2) = "TWO";
  EXPECT_EQ(*value1, "TWO");
}

// Test has in container
TEST(CoreUtils, HasInContainer) {
  std::vector<int> vec = {1, 2, 3};
  EXPECT_TRUE(has(vec, 2));
  EXPECT_FALSE(has(vec, 99));

  std::map<int, int> myMap = {{42, 1}, {84, 2}};
  EXPECT_TRUE(has(myMap, 42));
  EXPECT_FALSE(has(myMap, 999));
}

// Test array_view_t basic usage
TEST(CoreUtils, ArrayView) {
  int data[] = {10, 20, 30, 40};
  array_view_t<int> view(data, 4);
  EXPECT_EQ(view.count, 4);
  for (int i = 0; i < view.count; ++i) {
    EXPECT_EQ(view.ptr[i], data[i]);
  }

  view.ptr[1] = 99;
  EXPECT_EQ(data[1], 99);
}

// Test for_tuple
TEST(CoreUtils, ForTuple) {
  int x = 0, y = 0, z = 0;
  auto tupleRef = std::tie(x, y, z);
  for_tuple(tupleRef, [](auto& ref) { ref = 42; });
  EXPECT_EQ(x, 42);
  EXPECT_EQ(y, 42);
  EXPECT_EQ(z, 42);
}

TEST(CoreUtils, MaskedSelectUsesMaskBitsForIntegralValues) {
  EXPECT_EQ(masked_select<uint8_t>(0xFF, 0x12, 0x34), 0x12);
  EXPECT_EQ(masked_select<uint8_t>(0x00, 0x12, 0x34), 0x34);
  EXPECT_EQ(masked_select<uint8_t>(0x0F, 0xA5, 0x5A), 0x55);
}

// Test constant_time_select_u64
TEST(CoreUtils, ConstantTimeSelectU64) {
  uint64_t val1 = 0xAAAAAAAAAAAAAAAAULL;
  uint64_t val2 = 0xBBBBBBBBBBBBBBBBULL;
  uint64_t result1 = constant_time_select_u64(true, val1, val2);
  uint64_t result2 = constant_time_select_u64(false, val1, val2);

  EXPECT_EQ(result1, val1);
  EXPECT_EQ(result2, val2);
}

TEST(CoreUtils, ConstantTimeSelectBytesCopiesOnlySelectedInput) {
  byte_t y[] = {0x10, 0x20, 0x30, 0x40};
  byte_t z[] = {0xA0, 0xB0, 0xC0, 0xD0};
  byte_t out[] = {0, 0, 0, 0};

  constant_time_select_bytes(true, y, z, out, sizeof(out));
  EXPECT_TRUE(std::equal(std::begin(out), std::end(out), std::begin(y)));

  constant_time_select_bytes(false, y, z, out, sizeof(out));
  EXPECT_TRUE(std::equal(std::begin(out), std::end(out), std::begin(z)));

  EXPECT_CB_ASSERT(constant_time_select_bytes(true, y, z, out, -1), "size >= 0");
}

TEST(CoreUtils, CtSelectBufRequiresEqualSizesAndReturnsSelectedBuffer) {
  byte_t y_raw[] = {0x01, 0x02, 0x03};
  byte_t z_raw[] = {0x04, 0x05, 0x06};
  mem_t y(y_raw, sizeof(y_raw));
  mem_t z(z_raw, sizeof(z_raw));

  buf_t selected_y = ct_select_buf(true, y, z);
  buf_t selected_z = ct_select_buf(false, y, z);
  EXPECT_EQ(selected_y, y);
  EXPECT_EQ(selected_z, z);

  EXPECT_CB_ASSERT(ct_select_buf(true, y, mem_t(z_raw, 2)), "y.size == z.size");
}

TEST(ProtocolUtil, SUMCrashesOnEmpty) {
  std::vector<int> empty_vec;
  EXPECT_NO_FATAL_FAILURE({ (void)SUM(empty_vec); });

  std::vector<std::reference_wrapper<int>> empty_refs;
  EXPECT_NO_FATAL_FAILURE({ (void)SUM(empty_refs); });

  std::map<pname_t, int> empty_map;
  EXPECT_NO_FATAL_FAILURE({ (void)SUM(empty_map); });
}