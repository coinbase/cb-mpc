#include <gtest/gtest.h>

#include <cbmpc/crypto/base_pki.h>

#include "utils/test_macros.h"

namespace {

using namespace coinbase::crypto;

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

TEST_F(PKI, RSAEncryptDecrypt) {
  prv_key_t prv_key = prv_key_t::from(rsa_prv_key);
  pub_key_t pub_key = pub_key_t::from(rsa_pub_key);

  drbg_aes_ctr_t drbg(gen_random(32));

  ciphertext_t ciphertext;
  ciphertext.encrypt(pub_key, label, plaintext, &drbg);
  EXPECT_EQ(ciphertext.key_type, key_type_e::RSA);

  {
    buf_t enc_info;
    EXPECT_OK(ciphertext.decrypt_begin(enc_info));
    buf_t dec_info;
    EXPECT_OK(prv_key.execute(enc_info, dec_info));
    buf_t decrypted;
    EXPECT_OK(ciphertext.decrypt_end(label, dec_info, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }
  {
    buf_t decrypted;
    EXPECT_OK(ciphertext.decrypt(prv_key, label, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }

  {
    buf_t enc_info;
    EXPECT_OK(ciphertext.decrypt_begin(enc_info));
    buf_t dec_info;
    EXPECT_OK(prv_key.execute(enc_info, dec_info));
    buf_t decrypted;
    EXPECT_OK(ciphertext.decrypt_end(label, dec_info, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }
  {
    buf_t decrypted;
    EXPECT_OK(ciphertext.decrypt(prv_key, label, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }
}

TEST_F(PKI, ECCEncryptDecrypt) {
  prv_key_t prv_key = prv_key_t::from(ecc_prv_key);
  pub_key_t pub_key = pub_key_t::from(ecc_pub_key);

  drbg_aes_ctr_t drbg(gen_random(32));

  ciphertext_t ciphertext;
  ciphertext.encrypt(pub_key, label, plaintext, &drbg);
  EXPECT_EQ(ciphertext.key_type, key_type_e::ECC);

  {
    buf_t enc_info;
    EXPECT_OK(ciphertext.decrypt_begin(enc_info));
    buf_t dec_info;
    EXPECT_OK(prv_key.execute(enc_info, dec_info));
    buf_t decrypted;
    EXPECT_OK(ciphertext.decrypt_end(label, dec_info, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }
  {
    buf_t decrypted;
    EXPECT_OK(ciphertext.decrypt(prv_key, label, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }
}

}  // namespace