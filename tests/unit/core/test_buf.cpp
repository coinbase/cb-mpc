
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>

#include <cbmpc/core/buf.h>
#include <cbmpc/internal/core/convert.h>

#include "utils/test_macros.h"

namespace {

TEST(Buf, DefaultConstructor) {
  coinbase::buf_t buf;
  EXPECT_EQ(buf.size(), 0);
  EXPECT_TRUE(buf.empty());
}

TEST(Buf, ConstructWithSize) {
  const int size = 10;
  coinbase::buf_t buf(size);

  EXPECT_EQ(buf.size(), size);
  EXPECT_FALSE(buf.empty());
  for (int i = 0; i < size; ++i) {
    buf[i] = static_cast<uint8_t>(i);
  }
  for (int i = 0; i < size; ++i) {
    EXPECT_EQ(buf[i], static_cast<uint8_t>(i));
  }
}

TEST(Buf, ConstructFromMem) {
  std::string test_str = "Hello";
  coinbase::mem_t mem((const uint8_t*)test_str.data(), (int)test_str.size());
  coinbase::buf_t buf(mem);

  EXPECT_EQ(buf.size(), (int)test_str.size());
  EXPECT_EQ(buf.to_string(), test_str);
}

TEST(Mem, NegativeSizeToStringIsSafe) {
  const coinbase::mem_t mem(nullptr, -1);
  EXPECT_EQ(mem.to_string(), "");
}

TEST(Mem, NegativeSizeStreamIsSafe) {
  const coinbase::mem_t mem(nullptr, -1);
  std::ostringstream oss;
  oss << mem;
#ifdef _DEBUG
  EXPECT_EQ(oss.str(), "");
#else
  EXPECT_EQ(oss.str(), "<mem_t size=-1>");
#endif
}

TEST(Mem, NullDataPositiveSizeIsSafe) {
  const coinbase::mem_t mem(nullptr, 5);
  EXPECT_EQ(mem.to_string(), "");
  std::ostringstream oss;
  oss << mem;
#ifdef _DEBUG
  EXPECT_EQ(oss.str(), "");
#else
  EXPECT_EQ(oss.str(), "<mem_t size=5>");
#endif
}

TEST(Buf, CopyConstructor) {
  coinbase::buf_t original(5);
  for (int i = 0; i < 5; ++i) {
    original[i] = static_cast<uint8_t>(i + 1);
  }

  coinbase::buf_t copy(original);
  EXPECT_EQ(copy.size(), 5);
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(copy[i], static_cast<uint8_t>(i + 1));
  }
}

TEST(Buf, MoveConstructor) {
  coinbase::buf_t original(5);
  for (int i = 0; i < 5; ++i) {
    original[i] = static_cast<uint8_t>(i + 10);
  }

  // Move construct
  coinbase::buf_t moved(std::move(original));
  EXPECT_EQ(moved.size(), 5);
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(moved[i], static_cast<uint8_t>(i + 10));
  }
}

TEST(Buf, LongBufferCopyMoveAndPointerAccess) {
  coinbase::buf_t empty;
  EXPECT_EQ(empty.ptr(), nullptr);

  coinbase::buf_t original(80);
  for (int i = 0; i < original.size(); ++i) original[i] = static_cast<uint8_t>(i + 1);
  ASSERT_NE(original.ptr(), nullptr);

  coinbase::buf_t copy(original);
  EXPECT_EQ(copy, original);
  ASSERT_NE(copy.ptr(), nullptr);
  EXPECT_NE(copy.ptr(), original.ptr());

  coinbase::buf_t moved(std::move(original));
  EXPECT_EQ(moved, copy);
  EXPECT_TRUE(original.empty());
  EXPECT_EQ(original.ptr(), nullptr);
}

TEST(Buf, AssignmentOperator) {
  coinbase::buf_t buf1(3);
  buf1[0] = 'A';
  buf1[1] = 'B';
  buf1[2] = 'C';

  coinbase::buf_t buf2;
  buf2 = buf1;
  EXPECT_EQ(buf2.size(), 3);
  EXPECT_EQ(buf2[0], 'A');
  EXPECT_EQ(buf2[1], 'B');
  EXPECT_EQ(buf2[2], 'C');
}

TEST(Buf, AssignFixedBuffersReleasesLongStorage) {
  coinbase::buf_t buf(80);
  memset(buf.data(), 0xA5, buf.size());

  const coinbase::buf128_t small = coinbase::buf128_t::make(0x0102030405060708ULL, 0x1112131415161718ULL);
  buf = small;
  EXPECT_EQ(buf.size(), 16);
  EXPECT_EQ(static_cast<coinbase::buf128_t>(buf), small);

  coinbase::buf_t long_again(80);
  memset(long_again.data(), 0x5A, long_again.size());
  const coinbase::buf256_t wide = coinbase::buf256_t::make(small, coinbase::buf128_t::make(0x21, 0x22));
  long_again = wide;
  EXPECT_EQ(long_again.size(), 32);
  EXPECT_EQ(static_cast<coinbase::buf256_t>(long_again), wide);
}

TEST(Buf, Resize) {
  coinbase::buf_t buf(5);
  for (int i = 0; i < 5; ++i) {
    buf[i] = static_cast<uint8_t>(i);
  }
  buf.resize(10);

  EXPECT_EQ(buf.size(), 10);
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(buf[i], static_cast<uint8_t>(i));
  }
  // Note: `resize()` preserves previous bytes but does not guarantee initialization
  // of newly-grown regions.
}

TEST(Buf, ResizeTransitionsPreserveExpectedPrefix) {
  coinbase::buf_t buf(coinbase::mem_t("abcdefghijklmnopqrstuvwxyz0123456789"));
  ASSERT_EQ(buf.size(), 36);

  buf.resize(40);
  EXPECT_EQ(buf.take(36).to_string(), "abcdefghijklmnopqrstuvwxyz0123456789");

  buf.resize(20);
  EXPECT_EQ(buf.to_string(), "abcdefghijklmnopqrst");

  buf.resize(48);
  EXPECT_EQ(buf.take(20).to_string(), "abcdefghijklmnopqrst");

  buf.resize(44);
  EXPECT_EQ(buf.take(20).to_string(), "abcdefghijklmnopqrst");

  EXPECT_EQ(buf.resize(44), buf.data());
}

TEST(Buf, AllocReusesWhenSizeUnchangedAndHandlesLongBuffers) {
  coinbase::buf_t buf(64);
  byte_ptr ptr = buf.data();
  EXPECT_EQ(buf.alloc(64), ptr);

  byte_ptr short_ptr = buf.alloc(12);
  ASSERT_EQ(buf.size(), 12);
  EXPECT_EQ(short_ptr, buf.data());
}

TEST(Buf, PlusOperator) {
  std::string left_str = "Hello";
  std::string right_str = "World";
  coinbase::mem_t left_mem((const uint8_t*)left_str.data(), (int)left_str.size());
  coinbase::mem_t right_mem((const uint8_t*)right_str.data(), (int)right_str.size());

  auto combined = left_mem + right_mem;
  EXPECT_EQ(combined.to_string(), left_str + right_str);
}

TEST(Buf, XOROperator) {
  const int size = 5;
  coinbase::buf_t buf1(size);
  coinbase::buf_t buf2(size);

  for (int i = 0; i < size; ++i) {
    buf1[i] = static_cast<uint8_t>(i);
    buf2[i] = static_cast<uint8_t>(i + 1);
  }

  auto xor_result = coinbase::operator^(buf1, buf2);
  for (int i = 0; i < size; ++i) {
    EXPECT_EQ(xor_result[i], static_cast<uint8_t>(i) ^ static_cast<uint8_t>(i + 1));
  }
}

TEST(Buf, SelfXOROperator) {
  coinbase::buf_t buf1(3);
  coinbase::buf_t buf2(3);

  buf1[0] = 0xFF;
  buf1[1] = 0x00;
  buf1[2] = 0xAA;
  buf2[0] = 0x01;
  buf2[1] = 0x02;
  buf2[2] = 0x03;

  buf1 ^= buf2;  // XOR in place
  EXPECT_EQ(buf1[0], (uint8_t)(0xFF ^ 0x01));
  EXPECT_EQ(buf1[1], (uint8_t)(0x00 ^ 0x02));
  EXPECT_EQ(buf1[2], (uint8_t)(0xAA ^ 0x03));
}

TEST(Buf, AppendAliasesSourceSafely) {
  coinbase::buf_t buf(coinbase::mem_t("abcdef"));
  buf += buf.take(3);
  EXPECT_EQ(buf.to_string(), "abcdefabc");

  buf = buf.skip(2);
  EXPECT_EQ(buf.to_string(), "cdefabc");
}

TEST(Buf, ReverseHelpersAndMemEquality) {
  coinbase::buf_t buf(coinbase::mem_t("abcdef"));
  EXPECT_EQ(buf.rev().to_string(), "fedcba");

  buf.reverse();
  EXPECT_EQ(buf.to_string(), "fedcba");

  const coinbase::mem_t same = buf;
  EXPECT_TRUE(same == buf);
  EXPECT_FALSE(same != buf);
  EXPECT_FALSE(coinbase::mem_t("abcdef") == buf);
  EXPECT_TRUE(coinbase::mem_t("abcdef") != buf);
}

TEST(Mem, NonCryptoHashIsStableAndContentSensitive) {
  const coinbase::mem_t empty;
  const coinbase::mem_t a("abcdefg");
  const coinbase::mem_t same("abcdefg");
  const coinbase::mem_t different("abcdeff");

  EXPECT_EQ(empty.non_crypto_hash(), empty.non_crypto_hash());
  EXPECT_EQ(a.non_crypto_hash(), same.non_crypto_hash());
  EXPECT_NE(a.non_crypto_hash(), different.non_crypto_hash());
}

TEST(Buf, ToString) {
  coinbase::buf_t buf(5);
  const char msg[] = "Hello";
  for (int i = 0; i < 5; ++i) {
    buf[i] = static_cast<uint8_t>(msg[i]);
  }
  EXPECT_EQ(buf.to_string(), std::string(msg));
}

TEST(Buf, BzeroAndSecureBzero) {
  coinbase::buf_t buf(4);
  buf[0] = 10;
  buf[1] = 20;
  buf[2] = 30;
  buf[3] = 40;

  // Zero the buffer using bzero
  buf.bzero();
  for (int i = 0; i < buf.size(); ++i) {
    EXPECT_EQ(buf[i], 0);
  }

  // Refill and secure_bzero
  for (int i = 0; i < buf.size(); ++i) {
    buf[i] = (uint8_t)(i + 1);
  }
  buf.secure_bzero();
  for (int i = 0; i < buf.size(); ++i) {
    EXPECT_EQ(buf[i], 0);
  }
}

TEST(Buf, FixedSizeConvertRoundTripsAndRejectsTruncation) {
  coinbase::buf_t in(coinbase::mem_t("1234567890"));

  coinbase::converter_t size_counter(true);
  in.convert_fixed_size(size_counter, 10);
  ASSERT_EQ(size_counter.get_rv(), SUCCESS);
  ASSERT_EQ(size_counter.get_offset(), 10);

  coinbase::buf_t encoded(size_counter.get_offset());
  coinbase::converter_t writer(encoded.data());
  in.convert_fixed_size(writer, 10);
  ASSERT_EQ(writer.get_rv(), SUCCESS);

  coinbase::buf_t out;
  coinbase::converter_t reader(encoded);
  out.convert_fixed_size(reader, 10);
  ASSERT_EQ(reader.get_rv(), SUCCESS);
  EXPECT_EQ(out, in);

  coinbase::buf_t truncated(9);
  memset(truncated.data(), 0, truncated.size());
  coinbase::converter_t bad_reader(truncated);
  out.convert_fixed_size(bad_reader, 10);
  EXPECT_NE(bad_reader.get_rv(), SUCCESS);

  EXPECT_EQ(coinbase::buf_t::get_convert_size(in.size()), int(coinbase::ser(in).size()));
}

TEST(Buf, ConvertLastRoundTripsRemainingBytes) {
  coinbase::buf_t payload(coinbase::mem_t("tail-bytes"));

  coinbase::converter_t size_counter(true);
  payload.convert_last(size_counter);
  ASSERT_EQ(size_counter.get_offset(), payload.size());

  coinbase::buf_t encoded(size_counter.get_offset());
  coinbase::converter_t writer(encoded.data());
  payload.convert_last(writer);
  ASSERT_EQ(writer.get_rv(), SUCCESS);

  coinbase::converter_t reader(encoded);
  coinbase::buf_t decoded;
  decoded.convert_last(reader);
  ASSERT_EQ(reader.get_rv(), SUCCESS);
  EXPECT_EQ(decoded, payload);
}

TEST(Buf, VectorMemHelpersPreserveViewsAndCopies) {
  std::vector<coinbase::buf_t> bufs;
  bufs.emplace_back(coinbase::mem_t("alpha"));
  bufs.emplace_back(coinbase::mem_t("beta"));

  std::vector<coinbase::mem_t> views = coinbase::buf_t::to_mems(bufs);
  ASSERT_EQ(views.size(), 2);
  EXPECT_EQ(views[0].to_string(), "alpha");
  EXPECT_EQ(views[1].to_string(), "beta");

  std::vector<coinbase::buf_t> copies = coinbase::buf_t::from_mems(views);
  ASSERT_EQ(copies.size(), 2);
  EXPECT_EQ(copies[0].to_string(), "alpha");
  EXPECT_EQ(copies[1].to_string(), "beta");

  const std::vector<std::string> strings = {"red", "", "blue"};
  std::vector<coinbase::mem_t> string_views = coinbase::buf_t::to_mems(strings);
  ASSERT_EQ(string_views.size(), 3);
  EXPECT_EQ(string_views[0].to_string(), "red");
  EXPECT_EQ(string_views[1].to_string(), "");
  EXPECT_EQ(string_views[2].to_string(), "blue");
}

TEST(Buf, ConstructorRejectsNegativeSize) {
  // buf_t constructor should validate that size >= 0
  // Negative sizes would bypass guards like `if (buf.size() > 0)`
  // and could cause downstream memory corruption
  EXPECT_THROW({ coinbase::buf_t buf(-1); }, coinbase::assertion_failed_t);
  EXPECT_THROW({ coinbase::buf_t buf(-100); }, coinbase::assertion_failed_t);
}

TEST(Bits, StaticBitHelpersSetAndClearBits) {
  uint8_t data[2] = {0, 0};
  coinbase::bits_t::set(data, 0, true);
  coinbase::bits_t::set(data, 9, true);
  EXPECT_TRUE(coinbase::bits_t::get(data, 0));
  EXPECT_TRUE(coinbase::bits_t::get(data, 9));

  coinbase::bits_t::set(data, 0, false);
  EXPECT_FALSE(coinbase::bits_t::get(data, 0));
  EXPECT_TRUE(coinbase::bits_t::get(data, 9));
}

TEST(Bits, AppendCopyMoveResizeAndBzero) {
  coinbase::bits_t bits;
  bits.append(true);
  bits.append(false);
  bits.append(true);
  ASSERT_EQ(bits.count(), 3);
  EXPECT_TRUE(bits[0]);
  EXPECT_FALSE(bits[1]);
  EXPECT_TRUE(bits[2]);

  coinbase::bits_t copy(bits);
  EXPECT_TRUE(copy[0]);
  EXPECT_FALSE(copy[1]);
  EXPECT_TRUE(copy[2]);

  coinbase::bits_t moved(std::move(copy));
  EXPECT_EQ(moved.count(), 3);
  EXPECT_TRUE(copy.empty());

  bits.resize(70);
  EXPECT_TRUE(bits[0]);
  EXPECT_FALSE(bits[69]);
  bits.set(69, true);
  EXPECT_TRUE(bits.get(69));

  bits.bzero();
  EXPECT_FALSE(bits[0]);
  EXPECT_FALSE(bits[69]);

  bits.resize(0);
  EXPECT_TRUE(bits.empty());
}

TEST(Bits, FromBinRoundTripXorAndConcat) {
  const uint8_t raw[] = {0b10101010, 0b00001111};
  coinbase::bits_t from_bin = coinbase::bits_t::from_bin(coinbase::mem_t(raw, sizeof(raw)));
  ASSERT_EQ(from_bin.count(), 16);
  EXPECT_FALSE(from_bin[0]);
  EXPECT_TRUE(from_bin[1]);
  EXPECT_TRUE(from_bin[8]);

  coinbase::bits_t mask(16);
  mask.set(1, true);
  mask.set(8, true);

  coinbase::bits_t xored = from_bin ^ mask;
  EXPECT_FALSE(xored[1]);
  EXPECT_FALSE(xored[8]);

  from_bin ^= mask;
  EXPECT_EQ(from_bin[1], xored[1]);
  EXPECT_EQ(from_bin[8], xored[8]);

  coinbase::bits_t lhs(5);
  lhs.set(0, true);
  lhs.set(4, true);
  coinbase::bits_t rhs(4);
  rhs.set(1, true);

  coinbase::bits_t concat = lhs + rhs;
  ASSERT_EQ(concat.count(), 9);
  EXPECT_TRUE(concat[0]);
  EXPECT_TRUE(concat[4]);
  EXPECT_TRUE(concat[6]);

  lhs += rhs;
  ASSERT_EQ(lhs.count(), 9);
  EXPECT_TRUE(lhs[0]);
  EXPECT_TRUE(lhs[4]);
  EXPECT_TRUE(lhs[6]);
}

TEST(Bits, ByteAlignedConcatCopiesWholeBytes) {
  coinbase::bits_t lhs(8);
  lhs.set(0, true);
  lhs.set(7, true);

  coinbase::bits_t rhs(8);
  rhs.set(1, true);
  rhs.set(6, true);

  lhs += rhs;
  ASSERT_EQ(lhs.count(), 16);
  EXPECT_TRUE(lhs[0]);
  EXPECT_TRUE(lhs[7]);
  EXPECT_TRUE(lhs[9]);
  EXPECT_TRUE(lhs[14]);
}

TEST(Bits, SelfAppendHandlesByteAlignedAndBitAlignedInputs) {
  coinbase::bits_t byte_aligned(8);
  byte_aligned.set(0, true);
  byte_aligned.set(7, true);
  byte_aligned += byte_aligned;
  ASSERT_EQ(byte_aligned.count(), 16);
  EXPECT_TRUE(byte_aligned[0]);
  EXPECT_TRUE(byte_aligned[7]);
  EXPECT_TRUE(byte_aligned[8]);
  EXPECT_TRUE(byte_aligned[15]);

  coinbase::bits_t bit_aligned(5);
  bit_aligned.set(1, true);
  bit_aligned.set(4, true);
  bit_aligned += bit_aligned;
  ASSERT_EQ(bit_aligned.count(), 10);
  EXPECT_TRUE(bit_aligned[1]);
  EXPECT_TRUE(bit_aligned[4]);
  EXPECT_TRUE(bit_aligned[6]);
  EXPECT_TRUE(bit_aligned[9]);
}

TEST(Bits, NonEmptySerializationClearsUnusedTailBits) {
  coinbase::bits_t bits(10);
  bits.set(0, true);
  bits.set(9, true);

  coinbase::buf_t encoded = coinbase::ser(bits);
  coinbase::bits_t decoded;
  ASSERT_OK(coinbase::deser(encoded, decoded));
  ASSERT_EQ(decoded.count(), 10);
  EXPECT_TRUE(decoded[0]);
  EXPECT_TRUE(decoded[9]);
  EXPECT_FALSE(decoded[8]);
}

TEST(Bits, DeserializationRejectsTruncatedStorage) {
  byte_t bin[] = {0x0a, 0xff};  // 10-bit count requires two storage bytes.
  coinbase::bits_t out;
  coinbase::converter_t converter(coinbase::mem_t(bin, sizeof(bin)));
  converter.convert(out);

  EXPECT_NE(converter.get_rv(), SUCCESS);
}

}  // namespace
