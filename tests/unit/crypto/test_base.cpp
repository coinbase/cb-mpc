#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_bn256.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

TEST(BaseTest, TestError) {
  auto err = error("Test error");
  EXPECT_NE(err, 0);  // Just check that it returns something non-zero if that's expected
}

TEST(BaseTest, TestOpensslError) {
  // Simulate an error:
  auto err = openssl_error("Simulated openssl error");
  EXPECT_NE(err, 0);

  // Another version with int return
  auto err2 = openssl_error(-1, "Another error");
  EXPECT_NE(err2, 0);

  // Check the error string
  auto err_str = openssl_get_last_error_string();
  // The actual string might differ on your setup; just ensure it isn't empty.
  EXPECT_FALSE(err_str.empty());

  set_test_error_storing_mode(true);
  auto err3 = openssl_error(-1, "");
  EXPECT_NE(err3, 0);
  EXPECT_NE(g_test_log_str.find("OPENSSL error:"), std::string::npos);
  set_test_error_storing_mode(false);
}

TEST(BaseTest, InitializerConstructionIsSafe) {
  initializer_t init;
  SUCCEED();
}

TEST(BaseTest, TestSeedRandomAndGenRandom) {
  buf_t seed = buf_t("test");
  seed_random(seed);

  buf_t random_data = gen_random(32);  // Generate 32 random bytes
  ASSERT_EQ(random_data.size(), 32);
  seed_random(seed);
  buf_t random_data2 = gen_random(32);  // Generate 32 random bytes
  ASSERT_EQ(random_data2.size(), 32);
  EXPECT_NE(random_data, random_data2);
}

TEST(BaseTest, TestGenRandomBitlen) {
  buf_t bit_data = gen_random_bitlen(128);  // 128 bits
  // 128 bits = 16 bytes
  EXPECT_EQ(bit_data.size(), 16);
}

TEST(BaseTest, TestGenRandomHelpers) {
  // Test gen_random_bits
  bits_t bits = gen_random_bits(10);
  EXPECT_EQ(bits.count(), 10);

  // Test gen_random_bool
  bool random_bool = gen_random_bool();
  SUCCEED() << "Generated a random bool: " << (random_bool ? "true" : "false");

  // Test gen_random_int<uint32_t>
  auto r_int = gen_random_int<uint32_t>();
  SUCCEED() << "Generated a random int: " << r_int;
}

TEST(BaseTest, BitsSelfAppendIsSafe) {
  // Ensure `x += x` works correctly and does not rely on dangling views during resize().
  // Use a byte-aligned bit count to exercise the fast-path in bits_t::operator+=.
  bits_t x = gen_random_bits(128);
  bits_t expected = x + x;

  bits_t y = x;
  y += y;
  // `bits_t::equ` is intentionally not part of the API; compare the binary representation instead.
  EXPECT_TRUE(mem_t(expected) == mem_t(y));
}

TEST(BaseTest, TestAES_CTR) {
  buf_t key = bn_t(0x00).to_bin(16);
  buf_t iv = bn_t(0x01).to_bin(16);
  buf_t data = bn_t(0x02).to_bin(32);

  buf_t enc = aes_ctr_t::encrypt(key, iv.data(), data);
  buf_t dec = aes_ctr_t::decrypt(key, iv.data(), enc);

  EXPECT_EQ(dec, data);
}

TEST(BaseTest, AesCtrSupports192BitKeysAndCallerOutputBuffers) {
  buf_t key = bn_t::from_hex("00112233445566778899aabbccddeeff1021324354657687").to_bin(24);
  buf_t iv = bn_t::from_hex("8899aabbccddeeff0011223344556677").to_bin(16);
  buf_t data = buf_t("aes-192-ctr must round-trip through caller-owned buffers");

  buf_t enc(data.size());
  aes_ctr_t::encrypt(key, iv.data(), data, enc.data());

  buf_t dec(data.size());
  aes_ctr_t::decrypt(key, iv.data(), enc, dec.data());
  EXPECT_EQ(dec, data);

  aes_ctr_t ctr;
  ctr.init(key, iv.data());
  EXPECT_EQ(ctr.update(mem_t(), nullptr), 0);
}

TEST(BaseTest, TestDRBG) {
  // Initialize and generate some data
  buf_t seed = bn_t(0xAB).to_bin(32);
  drbg_aes_ctr_t drbg(seed);
  buf_t random_data = drbg.gen(16);
  EXPECT_EQ(random_data.size(), 16);

  // Reseed
  buf_t more_seed = bn_t(0xCD).to_bin(32);
  drbg.seed(more_seed);
  buf_t second_data = drbg.gen(16);
  EXPECT_EQ(second_data.size(), 16);

  // RNG might differ, so we won't strictly compare random_data vs second_data
}

TEST(BaseTest, TestAES_GCM) {
  buf_t key = bn_t(0x00).to_bin(16);
  buf_t iv = bn_t(0x01).to_bin(12);
  buf_t auth = bn_t(0x02).to_bin(16);
  buf_t data = bn_t(0x03).to_bin(32);

  buf_t enc;
  aes_gcm_t::encrypt(key, iv, auth, /*tag_size=*/16, data, enc);

  buf_t dec;
  EXPECT_OK(aes_gcm_t::decrypt(key, iv, auth, /*tag_size=*/16, enc, dec));
  EXPECT_EQ(dec, data);
}

TEST(BaseTest, TestAES_GMAC) {
  buf_t key = bn_t(0xAA).to_bin(16);
  buf_t iv = bn_t(0xBB).to_bin(12);
  buf_t data = bn_t(0xCC).to_bin(64);
  int out_size = 16;

  buf_t tag = aes_gmac_t::calculate(key, iv, data, out_size);
  EXPECT_EQ(tag.size(), out_size);
}

TEST(BaseTest, DrbgReseedIsDeterministic) {
  const buf_t seed = bn_t(0xAB).to_bin(32);
  const buf_t reseed = bn_t(0xCD).to_bin(32);

  drbg_aes_ctr_t drbg1(seed);
  drbg1.gen(8);
  drbg1.seed(reseed);
  const buf_t out1 = drbg1.gen(16);

  drbg_aes_ctr_t drbg2(seed);
  drbg2.gen(8);
  drbg2.seed(reseed);
  const buf_t out2 = drbg2.gen(16);

  EXPECT_EQ(out1, out2);
}

TEST(BaseTest, DrbgHelperOutputsHaveExpectedSizes) {
  drbg_aes_ctr_t drbg(bn_t(0x42).to_bin(32));

  const byte_t one_byte = drbg.gen_byte();
  (void)one_byte;
  const uint32_t one_int = drbg.gen_int();
  (void)one_int;
  const uint64_t one_int64 = drbg.gen_int64();
  (void)one_int64;
  const buf128_t one_buf128 = drbg.gen_buf128();
  EXPECT_EQ(sizeof(one_buf128), 16u);
  const buf256_t one_buf256 = drbg.gen_buf256();
  EXPECT_EQ(mem_t(one_buf256).size, 32);
  const bool one_bool = drbg.gen_bool();
  (void)one_bool;

  bits_t ten_bits = drbg.gen_bits(10);
  EXPECT_EQ(ten_bits.count(), 10);

  const bn_t bounded = drbg.gen_bn(curve_secp256k1.order());
  EXPECT_TRUE(bounded < curve_secp256k1.order().value());
}

TEST(BaseTest, DrbgNon32ByteSeedAndBoundedSamplersAreDeterministic) {
  const buf_t seed = bn_t::from_hex("0102030405060708090a0b0c0d0e0f101112131415161718").to_bin(24);
  drbg_aes_ctr_t drbg1(seed);
  drbg_aes_ctr_t drbg2(seed);

  const bn_t small_bound = bn_t::from_hex("010000000000000000000000000000000000000000000001");
  const bn_t bounded1 = drbg1.gen_bn(small_bound);
  const bn_t bounded2 = drbg2.gen_bn(small_bound);
  EXPECT_EQ(bounded1, bounded2);
  EXPECT_TRUE(bounded1 >= 0);
  EXPECT_TRUE(bounded1 < small_bound);

  const mod_t& q = curve_secp256k1.order();
  const bn_t mod_bounded1 = drbg1.gen_bn(q);
  const bn_t mod_bounded2 = drbg2.gen_bn(q);
  EXPECT_EQ(mod_bounded1, mod_bounded2);
  EXPECT_TRUE(mod_bounded1 >= 0);
  EXPECT_TRUE(mod_bounded1 < q.value());

  const bn256_t near_2_256_1 = drbg1.gen_bn256(q);
  const bn256_t near_2_256_2 = drbg2.gen_bn256(q);
  EXPECT_EQ(near_2_256_1, near_2_256_2);

  const mod_t slow_mod(bn_t::from_hex("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed"), true);
  const bn256_t reduced1 = drbg1.gen_bn256(slow_mod);
  const bn256_t reduced2 = drbg2.gen_bn256(slow_mod);
  EXPECT_EQ(reduced1, reduced2);
  EXPECT_TRUE(bn_t(reduced1) < slow_mod.value());
}

TEST(BaseTest, AesCtrInitOverloadsMatchStaticEncrypt) {
  const buf128_t iv = buf128_t::make(1, 2);
  const buf_t data = buf_t("aes-ctr-overload-contract");

  {
    const buf128_t key = buf128_t::make(3, 4);
    aes_ctr_t ctr;
    ctr.init(key, iv);
    buf_t out(data.size());
    ASSERT_EQ(ctr.update(data, out.data()), data.size());
    EXPECT_EQ(out, aes_ctr_t::encrypt(mem_t(key), const_byte_ptr(&iv), data));
    ctr.clear();
  }

  {
    const buf256_t key = buf256_t::make(buf128_t::make(5, 6), buf128_t::make(7, 8));
    aes_ctr_t ctr;
    ctr.init(key, iv);
    buf_t out(data.size());
    ASSERT_EQ(ctr.update(data, out.data()), data.size());
    EXPECT_EQ(out, aes_ctr_t::encrypt(mem_t(key), const_byte_ptr(&iv), data));
    ctr.clear();
  }
}

TEST(BaseTest, AesGcmTamperAndWrongAuthFail) {
  buf_t key = bn_t(0x00).to_bin(16);
  buf_t iv = bn_t(0x01).to_bin(12);
  buf_t auth = bn_t(0x02).to_bin(16);
  buf_t data = bn_t(0x03).to_bin(32);

  buf_t enc;
  aes_gcm_t::encrypt(key, iv, auth, 16, data, enc);

  buf_t dec;
  EXPECT_OK(aes_gcm_t::decrypt(key, iv, auth, 16, enc, dec));
  EXPECT_EQ(dec, data);

  buf_t tampered = enc;
  tampered[0] ^= 0x01;
  EXPECT_ER(aes_gcm_t::decrypt(key, iv, auth, 16, tampered, dec));
  EXPECT_TRUE(dec.empty());

  buf_t wrong_auth = auth;
  wrong_auth[0] ^= 0x01;
  EXPECT_ER(aes_gcm_t::decrypt(key, iv, wrong_auth, 16, enc, dec));
  EXPECT_TRUE(dec.empty());
}

TEST(BaseTest, AesGcmSupports192BitKeysAndRejectsShortCiphertext) {
  buf_t key = bn_t::from_hex("00112233445566778899aabbccddeeff1021324354657687").to_bin(24);
  buf_t iv = bn_t::from_hex("0102030405060708090a0b0c").to_bin(12);
  buf_t auth = buf_t("gcm-authenticated-data");
  buf_t data = buf_t("aes-192-gcm plaintext");

  buf_t enc;
  aes_gcm_t::encrypt(key, iv, auth, 16, data, enc);

  buf_t dec;
  EXPECT_OK(aes_gcm_t::decrypt(key, iv, auth, 16, enc, dec));
  EXPECT_EQ(dec, data);
  EXPECT_ER(aes_gcm_t::decrypt(key, iv, auth, 16, mem_t(enc.data(), 15), dec));
  EXPECT_TRUE(dec.empty());
}

TEST(BaseTest, AesGcmSupports256BitKeysAndEmptyAuth) {
  buf_t key = bn_t::from_hex("00112233445566778899aabbccddeeff102132435465768798a9bacbdcedfe0f").to_bin(32);
  buf_t iv = bn_t::from_hex("0102030405060708090a0b0c").to_bin(12);
  buf_t data = buf_t("aes-256-gcm plaintext without aad");

  buf_t enc;
  aes_gcm_t::encrypt(key, iv, mem_t(), 16, data, enc);
  EXPECT_EQ(enc.size(), data.size() + 16);

  buf_t dec;
  EXPECT_OK(aes_gcm_t::decrypt(key, iv, mem_t(), 16, enc, dec));
  EXPECT_EQ(dec, data);
  EXPECT_ER(aes_gcm_t::decrypt(key, iv, mem_t("unexpected-aad"), 16, enc, dec));
}

TEST(BaseTest, AesGcmTagSizeBoundaries) {
  buf_t key = bn_t(0x10).to_bin(16);
  buf_t iv = bn_t(0x20).to_bin(12);
  buf_t auth = bn_t(0x30).to_bin(8);
  buf_t data = buf_t("gcm-tag-boundary");

  for (const int tag_size : {12, 16}) {
    buf_t enc;
    aes_gcm_t::encrypt(key, iv, auth, tag_size, data, enc);

    buf_t dec;
    EXPECT_OK(aes_gcm_t::decrypt(key, iv, auth, tag_size, enc, dec));
    EXPECT_EQ(dec, data);
  }
}

TEST(BaseTest, AesGmacIncrementalUpdatesMatchAcrossFinalOverloads) {
  const buf_t key = bn_t::from_hex("00112233445566778899aabbccddeeff").to_bin(16);
  const buf_t iv = bn_t::from_hex("0102030405060708090a0b0c").to_bin(12);
  const buf128_t block = buf128_t::make(0x1020304050607080ULL, 0x90a0b0c0d0e0f001ULL);

  auto update_transcript = [&](aes_gmac_t& gmac) {
    gmac.update(mem_t("gmac-domain"));
    gmac.update(true);
    gmac.update(false);
    gmac.update(block);
    gmac.update(mem_t());
  };

  aes_gmac_t gmac1;
  gmac1.init(key, iv);
  update_transcript(gmac1);
  const buf_t tag1 = gmac1.final(16);

  aes_gmac_t gmac2;
  gmac2.init(key, iv);
  update_transcript(gmac2);
  const buf128_t tag2 = gmac2.final();

  EXPECT_TRUE(mem_t(tag1) == mem_t(tag2));
}

TEST(BaseTest, AesGmacSupports256BitKeysAndExplicitOutputBuffer) {
  const buf_t key = bn_t::from_hex("00112233445566778899aabbccddeeff102132435465768798a9bacbdcedfe0f").to_bin(32);
  const buf_t iv = bn_t::from_hex("0102030405060708090a0b0c").to_bin(12);
  const buf_t data = buf_t("gmac with aes-256");

  buf_t tag(12);
  aes_gmac_t::calculate(key, iv, data, tag);
  EXPECT_EQ(tag, aes_gmac_t::calculate(key, iv, data, 12));
}

TEST(BaseTest, VartimeScopeRestoresOnExit) {
  EXPECT_FALSE(is_vartime_scope());
  {
    vartime_scope_t scope;
    EXPECT_TRUE(is_vartime_scope());
  }
  EXPECT_FALSE(is_vartime_scope());
}
