
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include <cbmpc/core/buf.h>

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
  // The remaining bytes might be uninitialized, but ensure no crash occurs.
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

}  // namespace