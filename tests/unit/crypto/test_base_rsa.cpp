#include <gtest/gtest.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_pki.h>

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

  kem_aead_ciphertext_t<kem_policy_rsa_oaep_t> kem;
  EXPECT_OK(kem.encrypt(pub_key, label, plaintext, &drbg));

  {  // directly from kem
    buf_t decrypted;
    EXPECT_OK(kem.decrypt(prv_key, label, decrypted));
    EXPECT_EQ(decrypted, plaintext);
  }
}

}  // namespace