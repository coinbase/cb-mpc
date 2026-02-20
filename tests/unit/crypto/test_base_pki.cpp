#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base_pki.h>

#include "utils/test_macros.h"

namespace {

using namespace coinbase::crypto;
using coinbase::buf_t;
using coinbase::mem_t;

class PKI : public ::testing::Test {
 protected:
  void SetUp() override {
    rsa_prv_key.generate(RSA_KEY_LENGTH);
    rsa_pub_key = rsa_prv_key.pub();
    ecc_prv_key.generate(curve_p256);
    ecc_pub_key = ecc_prv_key.pub();
  }

  void TearDown() override {}
  rsa_prv_key_t rsa_prv_key;
  rsa_pub_key_t rsa_pub_key;
  ecc_prv_key_t ecc_prv_key;
  ecc_pub_key_t ecc_pub_key;

  buf_t label = buf_t("label");
  buf_t plaintext = buf_t("plaintext");
};

TEST_F(PKI, ECIES_EncryptDecrypt) {
  ecurve_t curve = curve_p256;
  ecc_prv_key_t prv_key;
  prv_key.generate(curve);
  ecc_pub_key_t pub_key(prv_key.pub());

  buf_t seed = gen_random(32);
  drbg_aes_ctr_t drbg(seed);

  buf_t label = buf_t("label");
  buf_t plaintext = buf_t("plaintext");

  ecies_t::ct_t c1, c2;
  EXPECT_OK(c1.encrypt(pub_key, label, plaintext, &drbg));
  // Different drbg state should result in different ciphertexts
  EXPECT_OK(c2.encrypt(pub_key, label, plaintext, &drbg));
  EXPECT_NE(coinbase::convert(c1), coinbase::convert(c2));

  {
    buf_t decrypted;
    EXPECT_OK(c1.decrypt(prv_key, label, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }
}

TEST_F(PKI, ECDH_P256_KEM_EncapDecap_HPKE) {
  // Directly test the KEM policy per RFC9180-compatible flow
  drbg_aes_ctr_t drbg(gen_random(32));
  buf_t kem_ct, ss1, ss2;

  EXPECT_OK(kem_policy_ecdh_p256_t::encapsulate(ecc_pub_key, kem_ct, ss1, &drbg));
  EXPECT_OK(kem_policy_ecdh_p256_t::decapsulate(ecc_prv_key, kem_ct, ss2));
  EXPECT_EQ(ss1, ss2);
}

TEST_F(PKI, RSA_KEM_AEAD_EncryptDecrypt) {
  drbg_aes_ctr_t drbg(gen_random(32));

  rsa_pke_t::ct_t c1, c2;
  EXPECT_OK(c1.encrypt(rsa_pub_key, label, plaintext, &drbg));
  EXPECT_OK(c2.encrypt(rsa_pub_key, label, plaintext, &drbg));
  EXPECT_NE(coinbase::convert(c1), coinbase::convert(c2));

  buf_t decrypted;
  EXPECT_OK(c1.decrypt(rsa_prv_key, label, decrypted));
  EXPECT_EQ(decrypted, plaintext);
}

// -----------------------------------------------------------------------------
// Additional HPKE and FFI KEM tests
// -----------------------------------------------------------------------------

static bn_t hex_bn(const char* h) { return bn_t::from_hex(h); }

TEST(HPKE_KEM_P256, DeterministicVector) {
  const bn_t x = hex_bn("1C3");
  const bn_t e = hex_bn("A5B7");

  ecc_prv_key_t skR;
  skR.set(curve_p256, x);
  ecc_pub_key_t pkR = skR.pub();

  const ecc_point_t E = e * curve_p256.generator();
  const buf_t enc = E.to_oct();
  ASSERT_EQ(enc.size(), 65);
  ASSERT_EQ(enc[0], 0x04);

  const buf_t dh = (e * pkR).get_x().to_bin(32);
  ASSERT_EQ(dh.size(), 32);

  buf_t kem_context;
  kem_context += enc;
  kem_context += pkR.to_oct();

  const buf_t eae_prk = kem_policy_ecdh_p256_t::labeled_extract(mem_t("eae_prk"), dh, mem_t());
  const buf_t shared_secret = kem_policy_ecdh_p256_t::labeled_expand(eae_prk, mem_t("shared_secret"), kem_context, 32);
  ASSERT_EQ(shared_secret.size(), 32);

  buf_t ss2;
  EXPECT_OK(kem_policy_ecdh_p256_t::decapsulate(skR, enc, ss2));
  EXPECT_EQ(ss2, shared_secret);

  SUCCEED();
}

}  // namespace