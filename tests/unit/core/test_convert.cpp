#include <gtest/gtest.h>
#include <map>

#include <cbmpc/core/buf.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/protocol/util.h>

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

TEST(CoreConvert, VectorBoolRoundTripPreservesPackedBooleanValues) {
  std::vector<bool> in = {true, false, true, true, false};
  std::vector<bool> out;

  buf_t buf = coinbase::ser(in);
  EXPECT_OK(deser(buf, out));
  EXPECT_EQ(in, out);
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

TEST(CoreConvert, ReferenceWrapperSerializesReferencedValue) {
  int value = 42;
  std::reference_wrapper<int> ref(value);
  buf_t buf = coinbase::ser(ref);

  int out = 0;
  EXPECT_OK(deser(buf, out));
  EXPECT_EQ(out, value);
}

TEST(CoreConvert, EmptyBitsRoundTrip) {
  bits_t in;
  buf_t buf = coinbase::ser(in);

  bits_t out;
  EXPECT_OK(deser(buf, out));
  EXPECT_TRUE(out.empty());
  EXPECT_EQ(out.count(), 0);
}

TEST(CoreConvert, ConvertLenBoundaryEncodingsRoundTrip) {
  const std::vector<uint32_t> values = {
      0, 0x7f, 0x80, 0x3fff, 0x4000, 0x1fffff, 0x200000, converter_t::MAX_CONVERT_LEN,
  };
  const std::vector<int> expected_sizes = {1, 1, 2, 2, 3, 3, 4, 4};

  for (size_t i = 0; i < values.size(); ++i) {
    uint32_t len = values[i];
    converter_t size_counter(true);
    size_counter.convert_len(len);
    ASSERT_EQ(size_counter.get_rv(), SUCCESS);
    EXPECT_EQ(size_counter.get_offset(), expected_sizes[i]);

    buf_t encoded(size_counter.get_offset());
    converter_t writer(encoded.data());
    writer.convert_len(len);
    ASSERT_EQ(writer.get_rv(), SUCCESS);

    uint32_t decoded = 0;
    converter_t reader(encoded);
    reader.convert_len(decoded);
    ASSERT_EQ(reader.get_rv(), SUCCESS);
    EXPECT_EQ(decoded, values[i]);
    EXPECT_EQ(reader.get_offset(), expected_sizes[i]);
  }
}

TEST(CoreConvert, ConvertLenRejectsOversizedLengths) {
  // Encode a 4-byte length prefix > converter_t::MAX_CONVERT_LEN (64 MiB).
  // len = 0x04000001 -> bytes: 0xE4 0x00 0x00 0x01
  byte_t bin[] = {0xE4, 0x00, 0x00, 0x01};
  converter_t converter(mem_t(bin, sizeof(bin)));
  uint32_t len = 0;
  converter.convert_len(len);
  EXPECT_NE(converter.get_rv(), SUCCESS);
  EXPECT_EQ(len, 0u);
}

TEST(CoreConvert, ConvertLenAllowsMaxValue) {
  // len = 0x04000000 (64 MiB) -> bytes: 0xE4 0x00 0x00 0x00
  byte_t bin[] = {0xE4, 0x00, 0x00, 0x00};
  converter_t converter(mem_t(bin, sizeof(bin)));
  uint32_t len = 0;
  converter.convert_len(len);
  EXPECT_EQ(converter.get_rv(), SUCCESS);
  EXPECT_EQ(len, converter_t::MAX_CONVERT_LEN);
}

TEST(CoreConvert, ConvertLenTruncatedInputFailsWithoutConsumingGarbageLength) {
  byte_t bin[] = {0x80};
  converter_t converter(mem_t(bin, sizeof(bin)));

  uint32_t len = 123;
  converter.convert_len(len);

  EXPECT_NE(converter.get_rv(), SUCCESS);
  EXPECT_EQ(len, 0u);
}

TEST(CoreConvert, ConvertLenTruncatedMultiByteInputsFailCleanly) {
  for (const std::vector<byte_t>& bin : {
           std::vector<byte_t>{0xC0, 0x01},
           std::vector<byte_t>{0xE0, 0x01, 0x02},
       }) {
    converter_t converter(mem_t(bin.data(), int(bin.size())));
    uint32_t len = 123;
    converter.convert_len(len);
    EXPECT_NE(converter.get_rv(), SUCCESS);
    EXPECT_EQ(len, 0u);
  }
}

TEST(CoreConvert, PrimitiveReadsRejectTruncatedInput) {
  byte_t one_byte[] = {0x12};
  byte_t three_bytes[] = {0x12, 0x34, 0x56};
  byte_t seven_bytes[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE};

  uint16_t u16 = 0;
  converter_t read_u16(mem_t(one_byte, sizeof(one_byte)));
  read_u16.convert(u16);
  EXPECT_NE(read_u16.get_rv(), SUCCESS);

  uint32_t u32 = 0;
  converter_t read_u32(mem_t(three_bytes, sizeof(three_bytes)));
  read_u32.convert(u32);
  EXPECT_NE(read_u32.get_rv(), SUCCESS);

  uint64_t u64 = 0;
  converter_t read_u64(mem_t(seven_bytes, sizeof(seven_bytes)));
  read_u64.convert(u64);
  EXPECT_NE(read_u64.get_rv(), SUCCESS);
}

TEST(CoreConvert, StringDeserializationRejectsNegativeAndTruncatedLengths) {
  {
    byte_t bin[] = {0xFF, 0xFF};
    std::string out = "unchanged";
    converter_t converter(mem_t(bin, sizeof(bin)));
    converter.convert(out);
    EXPECT_NE(converter.get_rv(), SUCCESS);
  }

  {
    byte_t bin[] = {0x00, 0x03, 'a', 'b'};
    std::string out;
    converter_t converter(mem_t(bin, sizeof(bin)));
    converter.convert(out);
    EXPECT_NE(converter.get_rv(), SUCCESS);
  }
}

TEST(CoreConvert, VectorBoolRejectsNegativeCount) {
  byte_t bin[] = {0xFF, 0xFF};
  std::vector<bool> out = {true};
  converter_t converter(mem_t(bin, sizeof(bin)));
  converter.convert(out);

  EXPECT_NE(converter.get_rv(), SUCCESS);
  EXPECT_TRUE(out.empty());
}

TEST(CoreConvert, ConvertFlagsRejectsUnknownBits) {
  byte_t bin[] = {0, 0, 0, 0, 0, 0, 0, 4};
  converter_t converter(mem_t(bin, sizeof(bin)));

  bool first = false;
  bool second = false;
  converter.convert_flags(first, second);

  EXPECT_NE(converter.get_rv(), SUCCESS);
  EXPECT_FALSE(first);
  EXPECT_FALSE(second);
}

TEST(CoreConvert, ConvertFlagsRoundTripKnownBits) {
  bool first = true;
  bool second = false;
  bool third = true;

  converter_t size_counter(true);
  size_counter.convert_flags(first, second, third);
  ASSERT_EQ(size_counter.get_rv(), SUCCESS);
  ASSERT_EQ(size_counter.get_offset(), 8);

  buf_t encoded(size_counter.get_offset());
  converter_t writer(encoded.data());
  writer.convert_flags(first, second, third);
  ASSERT_EQ(writer.get_rv(), SUCCESS);

  bool out_first = false;
  bool out_second = true;
  bool out_third = false;
  converter_t reader(encoded);
  reader.convert_flags(out_first, out_second, out_third);
  EXPECT_EQ(reader.get_rv(), SUCCESS);
  EXPECT_TRUE(out_first);
  EXPECT_FALSE(out_second);
  EXPECT_TRUE(out_third);
}

TEST(CoreConvert, EmptyFlagsRoundTripAndCodeTypeProbe) {
  converter_t size_counter(true);
  size_counter.convert_flags();
  EXPECT_EQ(size_counter.get_rv(), SUCCESS);
  ASSERT_EQ(size_counter.get_offset(), 8);

  buf_t encoded(size_counter.get_offset());
  converter_t writer(encoded.data());
  writer.convert_flags();
  EXPECT_EQ(writer.get_rv(), SUCCESS);

  converter_t reader(encoded);
  reader.convert_flags();
  EXPECT_EQ(reader.get_rv(), SUCCESS);

  constexpr uint64_t code_type = 0x0102030405060708ULL;
  buf_t code = coinbase::ser(code_type);
  EXPECT_TRUE(converter_t::is_code_type(code, code_type));
  EXPECT_FALSE(converter_t::is_code_type(code, 0x1111111111111111ULL));
  EXPECT_FALSE(converter_t::is_code_type(mem_t(code.data(), 7), code_type));
}

TEST(CoreConvert, BoundsHelpersRejectInvalidSizes) {
  converter_t reader(mem_t(nullptr, 0));
  EXPECT_FALSE(reader.at_least(-1));
  EXPECT_FALSE(reader.at_least(1));
  EXPECT_EQ(reader.get_size(), 0);

  converter_t writer(true);
  EXPECT_EQ(writer.get_size(), 0);
  EXPECT_CB_ASSERT(writer.forward(-1), "false");
}

TEST(CoreConvert, ConvertCodeTypeAcceptsAlternatesAndRejectsUnexpectedCode) {
  constexpr uint64_t code1 = 0x1111111111111111ULL;
  constexpr uint64_t code2 = 0x2222222222222222ULL;
  constexpr uint64_t bad_code = 0x3333333333333333ULL;

  {
    buf_t encoded = coinbase::ser(code2);
    converter_t converter(encoded);
    EXPECT_EQ(converter.convert_code_type(code1, code2), code2);
    EXPECT_EQ(converter.get_rv(), SUCCESS);
  }

  {
    buf_t encoded = coinbase::ser(bad_code);
    converter_t converter(encoded);
    EXPECT_EQ(converter.convert_code_type(code1, code2), 0u);
    EXPECT_NE(converter.get_rv(), SUCCESS);
  }
}

TEST(CoreConvert, ConvertCodeTypeWritesPrimaryCode) {
  constexpr uint64_t code1 = 0x1111111111111111ULL;
  constexpr uint64_t code2 = 0x2222222222222222ULL;

  converter_t size_counter(true);
  EXPECT_EQ(size_counter.convert_code_type(code1, code2), code1);
  ASSERT_EQ(size_counter.get_rv(), SUCCESS);
  ASSERT_EQ(size_counter.get_offset(), 8);

  buf_t encoded(size_counter.get_offset());
  converter_t writer(encoded.data());
  EXPECT_EQ(writer.convert_code_type(code1, code2), code1);
  ASSERT_EQ(writer.get_rv(), SUCCESS);

  converter_t reader(encoded);
  EXPECT_EQ(reader.convert_code_type(code1, code2), code1);
  EXPECT_EQ(reader.get_rv(), SUCCESS);
}

TEST(CoreConvert, ConvertCodeTypeAcceptsAllAlternateSlots) {
  constexpr uint64_t code1 = 0x1111111111111111ULL;
  constexpr uint64_t code2 = 0x2222222222222222ULL;
  constexpr uint64_t code3 = 0x3333333333333333ULL;
  constexpr uint64_t code4 = 0x4444444444444444ULL;
  constexpr uint64_t code5 = 0x5555555555555555ULL;
  constexpr uint64_t code6 = 0x6666666666666666ULL;
  constexpr uint64_t code7 = 0x7777777777777777ULL;
  constexpr uint64_t code8 = 0x8888888888888888ULL;

  for (uint64_t expected : {code1, code2, code3, code4, code5, code6, code7, code8}) {
    buf_t encoded = coinbase::ser(expected);
    converter_t converter(encoded);
    EXPECT_EQ(converter.convert_code_type(code1, code2, code3, code4, code5, code6, code7, code8), expected);
    EXPECT_EQ(converter.get_rv(), SUCCESS);
  }
}

TEST(CoreConvert, MapDeserializationRejectsDuplicateKeys) {
  byte_t bin[] = {
      0x02,                    // two map entries
      0x00, 0x00, 0x00, 0x2a,  // key 42
      0x00, 0x00, 0x00, 0x01,  // value 1
      0x00, 0x00, 0x00, 0x2a,  // duplicate key 42
      0x00, 0x00, 0x00, 0x02,  // value 2
  };

  std::map<int32_t, int32_t> out;
  EXPECT_NE(deser(mem_t(bin, sizeof(bin)), out), SUCCESS);
}

TEST(CoreConvert, VectorDeserializationRejectsOversizedElementCountBeforeAllocating) {
  constexpr uint32_t too_many = converter_t::MAX_CONTAINER_ELEMENTS + 1;
  byte_t bin[] = {
      byte_t((too_many >> 16) | 0xc0),
      byte_t(too_many >> 8),
      byte_t(too_many),
  };

  std::vector<int32_t> out = {1, 2, 3};
  converter_t converter(mem_t(bin, sizeof(bin)));
  converter.convert(out);

  EXPECT_NE(converter.get_rv(), SUCCESS);
  EXPECT_TRUE(out.empty());
}

TEST(CoreConvert, MapDeserializationRejectsOversizedElementCountBeforeReadingEntries) {
  constexpr uint32_t too_many = converter_t::MAX_CONTAINER_ELEMENTS + 1;
  byte_t bin[] = {
      byte_t((too_many >> 16) | 0xc0),
      byte_t(too_many >> 8),
      byte_t(too_many),
  };

  std::map<int32_t, int32_t> out = {{1, 2}};
  converter_t converter(mem_t(bin, sizeof(bin)));
  converter.convert(out);

  EXPECT_NE(converter.get_rv(), SUCCESS);
  EXPECT_TRUE(out.empty());
}

TEST(CoreConvert, ConvertRejectsInvalidSourceMemory) {
  int out = 0;
  EXPECT_NE(convert(out, mem_t(nullptr, 1)), SUCCESS);
  EXPECT_NE(convert(out, mem_t(reinterpret_cast<const_byte_ptr>(""), -1)), SUCCESS);
}

TEST(CoreConvert, ConvertLastRejectsNegRemainingSize) {
  byte_t bin[] = {0x42};
  converter_t converter(mem_t(bin, sizeof(bin)));
  // Simulate parser-state corruption / misuse: offset moved past the source size.
  converter.forward(2);

  buf_t out;
  out.convert_last(converter);
  EXPECT_NE(converter.get_rv(), SUCCESS);
  EXPECT_EQ(out.size(), 0);
}

TEST(CoreConvert, RejectsTrailingBytes) {
  // Strict deserialization should fail when there are unconsumed trailing bytes.
  // This prevents message malleability where two different byte sequences
  // could deserialize to the same value.
  int original_value = 42;
  buf_t serialized = coinbase::ser(original_value);

  byte_t garbage[4] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t with_trailing = serialized + mem_t(garbage, 4);

  int result = 0;
  error_t rv = deser(with_trailing, result);

  EXPECT_NE(rv, SUCCESS);

  EXPECT_OK(deser(serialized, result));
  EXPECT_EQ(result, original_value);
}

}  // namespace
