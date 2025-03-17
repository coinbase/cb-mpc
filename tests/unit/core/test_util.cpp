#include <gtest/gtest.h>
#include <map>
#include <vector>

#include <cbmpc/core/utils.h>

using namespace coinbase;

// Test bits_to_bytes and bytes_to_bits
TEST(CoreUtils, BitAndByteConversions) {
  EXPECT_EQ(bits_to_bytes(1), 1);
  EXPECT_EQ(bits_to_bytes(7), 1);
  EXPECT_EQ(bits_to_bytes(8), 1);
  EXPECT_EQ(bits_to_bytes(9), 2);

  EXPECT_EQ(bytes_to_bits(1), 8);
  EXPECT_EQ(bytes_to_bits(2), 16);
}

// Test endianness functions
TEST(CoreUtils, Endianness) {
  // We'll use buffers to store/retrieve values and verify.
  unsigned char buf[8];

  // Test little-endian get/set
  {
    uint16_t val16 = 0x1234;
    le_set_2(buf, val16);
    EXPECT_EQ(le_get_2(buf), val16);

    uint32_t val32 = 0x12345678;
    le_set_4(buf, val32);
    EXPECT_EQ(le_get_4(buf), val32);

    uint64_t val64 = 0x1234567890ABCDEFULL;
    le_set_8(buf, val64);
    EXPECT_EQ(le_get_8(buf), val64);
  }

  // Test big-endian get/set
  {
    uint16_t val16 = 0x1234;
    be_set_2(buf, val16);
    EXPECT_EQ(be_get_2(buf), val16);

    uint32_t val32 = 0x12345678;
    be_set_4(buf, val32);
    EXPECT_EQ(be_get_4(buf), val32);

    uint64_t val64 = 0x1234567890ABCDEFULL;
    be_set_8(buf, val64);
    EXPECT_EQ(be_get_8(buf), val64);
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
  EXPECT_EQ(int_log2(1), 1);
  EXPECT_EQ(int_log2(2), 32 - __builtin_clz(1));  // i.e. 1
  EXPECT_EQ(int_log2(8), 32 - __builtin_clz(7));
  EXPECT_EQ(int_log2(16), 32 - __builtin_clz(15));
}

// Test lookup in a std::map
TEST(CoreUtils, LookupInMap) {
  std::map<int, std::string> sampleMap = {{1, "one"}, {2, "two"}, {3, "three"}};

  auto [found1, value1] = lookup(sampleMap, 2);
  EXPECT_TRUE(found1);
  EXPECT_EQ(value1, "two");

  auto [found2, value2] = lookup(sampleMap, 99);
  EXPECT_FALSE(found2);
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

// Test constant_time_select_u64
TEST(CoreUtils, ConstantTimeSelectU64) {
  uint64_t val1 = 0xAAAAAAAAAAAAAAAAULL;
  uint64_t val2 = 0xBBBBBBBBBBBBBBBBULL;
  uint64_t result1 = constant_time_select_u64(true, val1, val2);
  uint64_t result2 = constant_time_select_u64(false, val1, val2);

  EXPECT_EQ(result1, val1);
  EXPECT_EQ(result2, val2);
}