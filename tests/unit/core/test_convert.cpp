#include <gtest/gtest.h>
#include <map>

#include <cbmpc/core/buf.h>
#include <cbmpc/core/convert.h>
#include <cbmpc/protocol/util.h>

#include "utils/test_macros.h"

namespace {

using namespace coinbase;

TEST(CoreConvert, BaseTypes) {
  bool b, b2;
  uint8_t u8, u82;
  uint16_t u16, u162;
  uint32_t u32, u322;
  uint64_t u64, u642;
  int8_t i8, i82;
  int16_t i16, i162;
  int32_t i32, i322;
  int64_t i64, i642;
  std::string s, s2;
  buf128_t buf128, buf128_2;
  buf256_t buf256, buf256_2;

  b = true;
  u8 = 42;
  u16 = 42;
  u32 = 42;
  u64 = 42;
  i8 = -42;
  i16 = -42;
  i32 = -42;
  i64 = -42;
  s = "test_string";
  buf128 = buf128_t::make(0x1234567890abcdef, 0x1234567890abcdef);
  buf256 = buf256_t::make(buf128_t::make(0x1234567890abcdef, 0x1234567890abcdef),
                          buf128_t::make(0x1234567890abcdef, 0x1234567890abcdef));
  buf_t buf = coinbase::ser(b, u8, u16, u32, u64, i8, i16, i32, i64, s, buf128, buf256);
  EXPECT_OK(deser(buf, b2, u82, u162, u322, u642, i82, i162, i322, i642, s2, buf128_2, buf256_2));
  EXPECT_EQ(b, b2);
  EXPECT_EQ(u8, u82);
  EXPECT_EQ(u16, u162);
  EXPECT_EQ(u32, u322);
  EXPECT_EQ(u64, u642);
  EXPECT_EQ(i8, i82);
  EXPECT_EQ(i16, i162);
  EXPECT_EQ(i32, i322);
  EXPECT_EQ(i64, i642);
  EXPECT_EQ(s, s2);
  EXPECT_EQ(buf128, buf128_2);
  EXPECT_EQ(buf256, buf256_2);
}

TEST(CoreConvert, CompositeType) {
  {  // std::array
    std::array<int, 3> arr, arr2;
    arr[0] = 21;
    arr[1] = 42;
    arr[2] = 58;

    buf_t buf = coinbase::ser(arr);
    EXPECT_OK(deser(buf, arr2));
    EXPECT_EQ(arr, arr2);
  }

  {  // std::vector
    std::vector<int> vec, vec2;
    vec.push_back(21);
    vec.push_back(42);
    vec.push_back(58);

    buf_t buf = coinbase::ser(vec);
    EXPECT_OK(deser(buf, vec2));
    EXPECT_EQ(vec, vec2);
  }

  {  // std::map
    std::map<int, std::string> in, out;
    in[21] = "test_string_1";
    in[42] = "test_string_2";
    in[58] = "";

    buf_t buf = coinbase::ser(in);
    EXPECT_OK(deser(buf, out));
    EXPECT_EQ(in, out);
  }

  {  // std::tuple
    std::tuple<int, bool, std::string> in, out;
    std::get<0>(in) = 42;
    std::get<1>(in) = true;
    std::get<2>(in) = "test_string";

    buf_t buf = coinbase::ser(in);
    EXPECT_OK(deser(buf, out));
    EXPECT_EQ(in, out);
  }
}

TEST(CoreConvert, CustomStruct) {
  struct custom_t {
    int a;
    bool b;
    std::string s;

    void convert(converter_t& converter) { converter.convert(a, b); }
  };

  custom_t in, out;
  in.a = 42;
  in.b = true;
  in.s = "this should not be serialized";

  buf_t buf = coinbase::ser(in);
  EXPECT_OK(deser(buf, out));
  EXPECT_EQ(in.a, out.a);
  EXPECT_EQ(in.b, out.b);
  EXPECT_NE(in.s, out.s);
  EXPECT_EQ(out.s, "");
}

}  // namespace