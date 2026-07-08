#include <gtest/gtest.h>
#include <utility>
#include <vector>

#include <cbmpc/internal/core/strext.h>
#include <cbmpc/internal/crypto/base.h>

#include "utils/test_macros.h"

namespace {

using namespace coinbase;
using namespace coinbase::crypto;

buf_t hex_to_buf(const char* hex) {
  buf_t out;
  EXPECT_TRUE(strext::from_hex(out, hex));
  return out;
}

buf_t hash_once(hash_e alg, mem_t message) {
  hash_t hash(alg);
  hash.init();
  hash.update(message);
  return hash.final();
}

template <typename T>
buf256_t hash_single_framed_value(const T& value, uint64_t element_size) {
  sha256_t expected;
  expected.update(uint64_t(1));
  expected.update(element_size);
  expected.update(value);
  return expected.final();
}

template <typename T>
void expect_single_element_vector_hash_uses_size(const T& value, uint64_t element_size) {
  EXPECT_EQ(get_bin_size(value), int(element_size));

  std::vector<T> values;
  values.push_back(value);
  EXPECT_EQ(sha256_t::hash(values), hash_single_framed_value(value, element_size));
}

TEST(BaseHash, MemVecEncodesBoundsAndLen) {
  const std::vector<mem_t> msgs_a = {mem_t("a"), mem_t("bc")};  // concat: "abc"
  const std::vector<mem_t> msgs_b = {mem_t("ab"), mem_t("c")};  // concat: "abc"
  const std::vector<mem_t> msgs_c = {mem_t("abc")};             // concat: "abc"

  const auto ha = sha256_t::hash(msgs_a);
  const auto hb = sha256_t::hash(msgs_b);
  const auto hc = sha256_t::hash(msgs_c);

  EXPECT_NE(ha, hb);
  EXPECT_NE(ha, hc);
  EXPECT_NE(hb, hc);
}

TEST(BaseHash, VectorHashFramesFixedWidthElements) {
  expect_single_element_vector_hash_uses_size(byte_t(0xab), 1);
  expect_single_element_vector_hash_uses_size(uint16_t(0x1234), 2);
  expect_single_element_vector_hash_uses_size(int16_t(-2), 2);
  expect_single_element_vector_hash_uses_size(uint32_t(0x01020304), 4);
  expect_single_element_vector_hash_uses_size(int64_t(-1), 8);

  bits_t bits(9);
  bits.set(0, true);
  bits.set(8, true);
  expect_single_element_vector_hash_uses_size(bits, 2);

  EXPECT_EQ(get_bin_size(false), 1);
  EXPECT_EQ(get_bin_size(true), 1);
}

TEST(BaseHash, UpdateRejectsNegativeSize) {
  // hash_t::update() should validate that size >= 0
  // Negative sizes would be implicitly converted to huge positive size_t values
  // when passed to EVP_DigestUpdate(), causing out-of-bounds reads

  hash_t hash(hash_e::sha256);
  hash.init();
  byte_t data[4] = {0x01, 0x02, 0x03, 0x04};

  // Should throw assertion_failed_t for negative size
  EXPECT_THROW({ hash.update(data, -1); }, coinbase::assertion_failed_t);
  EXPECT_THROW({ hash.update(data, -100); }, coinbase::assertion_failed_t);

  // Valid size should work
  EXPECT_NO_THROW({ hash.update(data, 4); });
  EXPECT_NO_THROW({ hash.update(data, 0); });
}

TEST(BaseHash, HmacSha256IsDeterministicAndDistinctFromSha256) {
  buf_t key(20);
  memset(key.data(), 0x0b, 20);
  const buf_t data = buf_t("Hi There");

  hmac_sha256_t hmac1(key);
  hmac1.update(data);
  const buf_t mac1 = hmac1.final();

  hmac_sha256_t hmac2(key);
  hmac2.update(data);
  EXPECT_EQ(mac1, hmac2.final());
  EXPECT_NE(mac1, sha256_t::hash(data));
}

TEST(BaseHash, UpdateStateEncodesScalarsAndBuffers) {
  hash_t hash1(hash_e::sha256);
  hash1.init();
  update_state(hash1, uint32_t(0x01020304));
  update_state(hash1, int16_t(-1));
  update_state(hash1, true);
  update_state(hash1, false);
  update_state(hash1, buf_t("tail"));
  const buf_t digest1 = hash1.final();

  hash_t hash2(hash_e::sha256);
  hash2.init();
  byte_t u32_be[4] = {0x01, 0x02, 0x03, 0x04};
  hash2.update(u32_be, 4);
  byte_t i16_be[2] = {0xff, 0xff};
  hash2.update(i16_be, 2);
  hash2.update(byte_t(1));
  hash2.update(byte_t(0));
  const byte_t tail[] = {'t', 'a', 'i', 'l'};
  hash2.update(tail, 4);
  const buf_t digest2 = hash2.final();

  EXPECT_EQ(digest1, digest2);
}

TEST(BaseHash, Sha256FinalWritesIntoCallerBuffer) {
  sha256_t hash;
  hash.update(mem_t("abc"));

  buf256_t digest;
  hash.final(digest);

  EXPECT_EQ(digest, sha256_t::hash(mem_t("abc")));
}

TEST(BaseHash, HkdfSha256ExtractExpandIsDeterministic) {
  buf_t ikm;
  buf_t salt;
  buf_t info;
  ASSERT_TRUE(strext::from_hex(ikm, "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"));
  ASSERT_TRUE(strext::from_hex(salt, "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"));
  ASSERT_TRUE(strext::from_hex(info, "f0f1f2f3f4f5f6f7f8f9"));

  const buf_t prk = hkdf_extract_sha256(salt, ikm);
  const buf_t okm = hkdf_expand_sha256(prk, info, 42);
  const buf_t repeat = hkdf_expand_sha256(prk, info, 42);

  EXPECT_EQ(okm.size(), 42u);
  EXPECT_EQ(okm, repeat);
  EXPECT_NE(okm, hkdf_expand_sha256(prk, info, 41));
}

TEST(BaseHash, HashAlgMetadata) {
  const auto& sha256 = hash_alg_t::get(hash_e::sha256);
  EXPECT_TRUE(sha256.valid());
  EXPECT_EQ(sha256.size, 32);
  EXPECT_EQ(mem_t(sha256_t::hash(mem_t("abc"))).size, 32);

  const auto& sha512 = hash_alg_t::get(hash_e::sha512);
  EXPECT_TRUE(sha512.valid());
  EXPECT_EQ(sha512.size, 64);
}

TEST(BaseHash, HashAlgorithmsKnownVectorsAndSizes) {
  EXPECT_FALSE(hash_alg_t::get(hash_e::none).valid());

  EXPECT_EQ(hash_once(hash_e::sha384, mem_t("abc")),
            hex_to_buf("cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7"
                       "cc2358baeca134c825a7"));
  EXPECT_EQ(hash_once(hash_e::sha3_256, mem_t("abc")),
            hex_to_buf("3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532"));
  EXPECT_EQ(hash_once(hash_e::sha3_384, mem_t("abc")),
            hex_to_buf("ec01498288516fc926459f58e2c6ad8df9b473cb0fc08c2596da7cf0e49be4b298d88cea927ac7"
                       "f539f1edf228376d25"));
  EXPECT_EQ(hash_once(hash_e::sha3_512, mem_t("abc")),
            hex_to_buf("b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192"
                       "af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0"));

  for (const auto& [alg, size] :
       {std::pair(hash_e::sha256, 32), std::pair(hash_e::sha384, 48), std::pair(hash_e::sha512, 64),
        std::pair(hash_e::sha3_256, 32), std::pair(hash_e::sha3_384, 48), std::pair(hash_e::sha3_512, 64),
        std::pair(hash_e::blake2s, 32), std::pair(hash_e::blake2b, 64), std::pair(hash_e::ripemd160, 20)}) {
    const hash_alg_t& meta = hash_alg_t::get(alg);
    EXPECT_TRUE(meta.valid());
    EXPECT_EQ(meta.size, size);
    EXPECT_EQ(hash_once(alg, mem_t("abc")).size(), size_t(size));
  }
}

TEST(BaseHash, HashAndHmacCopyStateForksTranscript) {
  hash_t left(hash_e::sha512);
  left.init();
  left.update(mem_t("prefix-"));
  hash_t right(hash_e::sha512);
  right.init();
  left.copy_state(right);

  left.update(mem_t("left"));
  right.update(mem_t("right"));
  EXPECT_EQ(left.final(), hash_once(hash_e::sha512, mem_t("prefix-left")));
  EXPECT_EQ(right.final(), hash_once(hash_e::sha512, mem_t("prefix-right")));

  const buf_t key = buf_t("copy-state-key");
  hmac_sha256_t hmac_left(key);
  hmac_left.update(mem_t("prefix-"));
  hmac_sha256_t hmac_right(key);
  hmac_left.copy_state(hmac_right);

  hmac_left.update(mem_t("left"));
  hmac_right.update(mem_t("right"));

  hmac_sha256_t expected_left(key);
  expected_left.update(mem_t("prefix-"));
  expected_left.update(mem_t("left"));
  hmac_sha256_t expected_right(key);
  expected_right.update(mem_t("prefix-"));
  expected_right.update(mem_t("right"));

  EXPECT_EQ(hmac_left.final(), expected_left.final());
  EXPECT_EQ(hmac_right.final(), expected_right.final());
}

}  // namespace
