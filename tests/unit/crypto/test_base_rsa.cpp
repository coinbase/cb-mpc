#include <gtest/gtest.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_hash.h>
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

TEST(RSA, KEMPolicyEncapDecapConsistency) {
  rsa_prv_key_t prv_key;
  prv_key.generate(RSA_KEY_LENGTH);
  rsa_pub_key_t pub_key(prv_key.pub());

  drbg_aes_ctr_t drbg(gen_random(32));

  buf_t kem_ct, ss1, ss2;
  EXPECT_OK(kem_policy_rsa_oaep_t::encapsulate(pub_key, kem_ct, ss1, &drbg));
  EXPECT_OK(kem_policy_rsa_oaep_t::decapsulate(prv_key, kem_ct, ss2));
  EXPECT_EQ(ss1, ss2);
}

// -----------------------------------------------------------------------------
// Additional RSA OAEP vector test with deterministic seed
// -----------------------------------------------------------------------------

TEST(RSA_OAEP, DeterministicVectorWithSeed) {
  rsa_prv_key_t sk;
  sk.generate(RSA_KEY_LENGTH);
  rsa_pub_key_t pk = sk.pub();

  const buf_t label = buf_t("label");
  const buf_t message = buf_t("HPKE/RSA OAEP test message");

  const int hlen = hash_alg_t::get(hash_e::sha256).size;
  buf_t seed(hlen);
  for (int i = 0; i < hlen; ++i) seed[i] = static_cast<uint8_t>(i);

  buf_t ct;
  EXPECT_OK(pk.encrypt_oaep_with_seed(message, hash_e::sha256, hash_e::sha256, label, seed, ct));

  buf_t pt;
  EXPECT_OK(sk.decrypt_oaep(ct, hash_e::sha256, hash_e::sha256, label, pt));
  EXPECT_EQ(pt, message);
}

}  // namespace