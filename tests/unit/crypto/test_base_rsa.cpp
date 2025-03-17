#include <gtest/gtest.h>

#include <cbmpc/crypto/base.h>

#include "utils/test_macros.h"

namespace {
using namespace coinbase::crypto;

TEST(RSA, EncryptDecrypt) {
  rsa_prv_key_t prv_key;
  prv_key.generate(RSA_KEY_LENGTH);
  rsa_pub_key_t pub_key(prv_key.pub());

  drbg_aes_ctr_t drbg(gen_random(32));

  buf_t label = buf_t("label");
  buf_t plaintext = buf_t("plaintext");

  rsa_kem_ciphertext_t kem;
  kem.encrypt(pub_key, label, plaintext, &drbg);

  {  // directly from kem
    buf_t decrypted;
    EXPECT_OK(kem.decrypt(prv_key, label, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }
  {
    buf_t enc_info;
    EXPECT_OK(kem.decrypt_begin(enc_info));
    std::cout << enc_info << std::endl;
    cmem_t out;
    EXPECT_OK(rsa_oaep_t::execute(&prv_key, NID_sha256, NID_sha256, mem_t(), enc_info, &out));
    buf_t dec_info(out);
    buf_t decrypted;
    EXPECT_OK(kem.decrypt_end(label, dec_info, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }
  {
    buf_t enc_info;
    EXPECT_OK(kem.decrypt_begin(enc_info));
    buf_t dec_info;
    rsa_oaep_t(prv_key).execute(hash_e::sha256, hash_e::sha256, mem_t(), enc_info, dec_info);
    buf_t decrypted;
    EXPECT_OK(kem.decrypt_end(label, dec_info, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }
}

}  // namespace