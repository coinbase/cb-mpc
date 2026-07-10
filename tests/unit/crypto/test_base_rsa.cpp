#include <gtest/gtest.h>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_hash.h>
#include <cbmpc/internal/crypto/base_pki.h>

#include "utils/test_macros.h"

namespace {
using namespace coinbase::crypto;
using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;

struct rsa_components_t {
  bn_t n, e, d, p, q, dp, dq, qinv;
};

rsa_components_t get_components(const rsa_prv_key_t& key) {
  rsa_components_t components;
  components.n = key.get_n();
  components.e = key.get_e();
  components.p = key.get_p();
  components.q = key.get_q();

  const bn_t p_minus_1 = components.p - 1;
  const bn_t q_minus_1 = components.q - 1;
  const bn_t phi = p_minus_1 * q_minus_1;

  {
    vartime_scope_t scope;
    cb_assert(BN_mod_inverse(components.d, components.e, phi, bn_t::thread_local_storage_bn_ctx()));
    cb_assert(BN_mod_inverse(components.qinv, components.q, components.p, bn_t::thread_local_storage_bn_ctx()));
  }
  bn_t::div(components.d, p_minus_1, &components.dp);
  bn_t::div(components.d, q_minus_1, &components.dq);

  return components;
}

buf_t small_raw_rsa_plaintext(int size) {
  buf_t raw(size);
  memset(raw.data(), 0, raw.size());
  raw[raw.size() - 1] = 0x2a;
  return raw;
}

buf_t serialized_rsa_public_components(const bn_t& n, const bn_t& e) {
  constexpr uint8_t kPartsEAndN = 0x03;
  return coinbase::ser(kPartsEAndN, e, n);
}

void expect_rsa_private_key_works(const rsa_prv_key_t& key, const rsa_pub_key_t& pub_key) {
  const buf_t raw = small_raw_rsa_plaintext(pub_key.size());
  buf_t encrypted;
  ASSERT_OK(pub_key.encrypt_raw(raw, encrypted));

  buf_t decrypted;
  ASSERT_OK(key.decrypt_raw(encrypted, decrypted));
  EXPECT_EQ(decrypted, raw);

  const buf_t digest = buf_t(sha256_t::hash(mem_t("rsa message")));
  buf_t signature;
  ASSERT_OK(key.sign_pkcs1(digest, hash_e::sha256, signature));
  EXPECT_EQ(signature.size(), pub_key.size());
  EXPECT_OK(pub_key.verify_pkcs1(digest, hash_e::sha256, signature));
}

error_t rsa_oaep_callback(void* ctx, int hash_alg, int mgf_alg, mem_t label, mem_t input, buf_t& output) {
  return rsa_oaep_t::execute(ctx, hash_alg, mgf_alg, label, input, output);
}

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

TEST(RSA, PublicKeyConversionRoundTripsUsableKey) {
  rsa_prv_key_t prv_key;
  prv_key.generate(RSA_KEY_LENGTH);
  const rsa_pub_key_t pub_key = prv_key.pub();

  rsa_pub_key_t empty;
  EXPECT_EQ(empty.size(), 0);

  const buf_t encoded = coinbase::ser(pub_key);
  rsa_pub_key_t decoded;
  ASSERT_OK(coinbase::deser(encoded, decoded));
  EXPECT_TRUE(decoded == pub_key);
  EXPECT_FALSE(decoded != pub_key);

  rsa_prv_key_t other_prv_key;
  other_prv_key.generate(RSA_KEY_LENGTH);
  EXPECT_FALSE(decoded == other_prv_key.pub());
  EXPECT_TRUE(decoded != other_prv_key.pub());

  const buf_t message = buf_t("roundtrip message");
  buf_t ciphertext;
  ASSERT_OK(decoded.encrypt_oaep(message, hash_e::sha256, hash_e::sha256, mem_t(), ciphertext));

  buf_t decrypted;
  EXPECT_OK(prv_key.decrypt_oaep(ciphertext, hash_e::sha256, hash_e::sha256, mem_t(), decrypted));
  EXPECT_EQ(decrypted, message);

  const uint8_t no_parts = 0;
  rsa_pub_key_t decoded_empty;
  EXPECT_OK(coinbase::deser(mem_t(&no_parts, 1), decoded_empty));
  EXPECT_EQ(decoded_empty.size(), 0);

  const uint8_t unsupported_parts = 0x04;
  rsa_pub_key_t invalid;
  EXPECT_ER(coinbase::deser(mem_t(&unsupported_parts, 1), invalid));
  EXPECT_EQ(invalid.size(), 0);
}

TEST(RSA, PublicKeyConversionRejectsInvalidPublicParameters) {
  rsa_prv_key_t prv_key;
  prv_key.generate(RSA_KEY_LENGTH);
  const bn_t n = prv_key.get_n();

  dylog_disable_scope_t no_log_err;

  const buf_t encoded_e_one = serialized_rsa_public_components(n, bn_t(1));
  rsa_pub_key_t decoded_e_one;
  EXPECT_ER(coinbase::deser(encoded_e_one, decoded_e_one));
  EXPECT_EQ(decoded_e_one.size(), 0);

  const buf_t encoded_even_e = serialized_rsa_public_components(n, bn_t(4));
  rsa_pub_key_t decoded_even_e;
  EXPECT_ER(coinbase::deser(encoded_even_e, decoded_even_e));
  EXPECT_EQ(decoded_even_e.size(), 0);
}

TEST(RSA, PrivateKeyConversionRoundTripsUsableKey) {
  rsa_prv_key_t prv_key;
  prv_key.generate(RSA_KEY_LENGTH);
  const rsa_pub_key_t pub_key = prv_key.pub();

  rsa_prv_key_t empty;
  EXPECT_EQ(empty.size(), 0);

  const buf_t encoded = coinbase::ser(prv_key);
  rsa_prv_key_t decoded;
  ASSERT_OK(coinbase::deser(encoded, decoded));
  EXPECT_EQ(decoded.get_n(), prv_key.get_n());
  EXPECT_EQ(decoded.get_e(), prv_key.get_e());
  EXPECT_EQ(decoded.get_p() * decoded.get_q(), decoded.get_n());
  expect_rsa_private_key_works(decoded, pub_key);

  const uint8_t no_parts = 0;
  rsa_prv_key_t decoded_empty;
  EXPECT_OK(coinbase::deser(mem_t(&no_parts, 1), decoded_empty));
  EXPECT_EQ(decoded_empty.size(), 0);

  const uint8_t unsupported_parts = 0x04;
  rsa_prv_key_t invalid;
  EXPECT_ER(coinbase::deser(mem_t(&unsupported_parts, 1), invalid));
  EXPECT_EQ(invalid.size(), 0);
}

TEST(RSA, RawEncryptionAndPkcs1SigningRejectMalformedInputs) {
  rsa_prv_key_t prv_key;
  prv_key.generate(RSA_KEY_LENGTH);
  const rsa_pub_key_t pub_key = prv_key.pub();

  const buf_t raw = small_raw_rsa_plaintext(pub_key.size());
  buf_t encrypted;
  ASSERT_OK(pub_key.encrypt_raw(raw, encrypted));
  EXPECT_EQ(encrypted.size(), pub_key.size());

  buf_t decrypted;
  EXPECT_OK(prv_key.decrypt_raw(encrypted, decrypted));
  EXPECT_EQ(decrypted, raw);

  EXPECT_ER(pub_key.encrypt_raw(raw.take(raw.size() - 1), encrypted));
  EXPECT_ER(prv_key.decrypt_raw(encrypted.take(encrypted.size() - 1), decrypted));

  const buf_t digest = buf_t(sha256_t::hash(mem_t("rsa pkcs1")));
  buf_t signature;
  ASSERT_OK(prv_key.sign_pkcs1(digest, hash_e::sha256, signature));
  EXPECT_OK(pub_key.verify_pkcs1(digest, hash_e::sha256, signature));

  buf_t wrong_digest = digest;
  wrong_digest[0] ^= 0x01;
  EXPECT_ER(pub_key.verify_pkcs1(wrong_digest, hash_e::sha256, signature));
  EXPECT_ER(pub_key.verify_pkcs1(digest, hash_e::sha256, signature.take(signature.size() - 1)));
}

TEST(RSA, PrivateKeySettersReconstructUsableKeys) {
  rsa_prv_key_t original;
  original.generate(RSA_KEY_LENGTH);
  const rsa_pub_key_t pub_key = original.pub();
  const rsa_components_t components = get_components(original);

  rsa_prv_key_t without_factors;
  without_factors.set(components.n, components.e, components.d);
  expect_rsa_private_key_works(without_factors, pub_key);

  rsa_prv_key_t with_factors;
  with_factors.set(components.n, components.e, components.d, components.p, components.q);
  expect_rsa_private_key_works(with_factors, pub_key);

  rsa_prv_key_t with_crt;
  with_crt.set(components.n, components.e, components.d, components.p, components.q, components.dp, components.dq,
               components.qinv);
  expect_rsa_private_key_works(with_crt, pub_key);
}

TEST(RSA, PrivateExecuteAndCallbackOaepDecrypt) {
  rsa_prv_key_t prv_key;
  prv_key.generate(RSA_KEY_LENGTH);
  const rsa_pub_key_t pub_key = prv_key.pub();

  const buf_t message = buf_t("oaep callback message");
  buf_t ciphertext;
  ASSERT_OK(pub_key.encrypt_oaep(message, hash_e::sha256, hash_e::sha256, mem_t(), ciphertext));

  buf_t decrypted;
  EXPECT_OK(prv_key.execute(ciphertext, decrypted));
  EXPECT_EQ(decrypted, message);

  rsa_oaep_t callback_oaep(rsa_oaep_callback, &prv_key);
  decrypted = buf_t();
  EXPECT_OK(callback_oaep.execute(hash_e::sha256, hash_e::sha256, mem_t(), ciphertext, decrypted));
  EXPECT_EQ(decrypted, message);

  EXPECT_ER(callback_oaep.execute(hash_e::none, hash_e::sha256, mem_t(), ciphertext, decrypted));
  EXPECT_ER(callback_oaep.execute(hash_e::sha256, hash_e::none, mem_t(), ciphertext, decrypted));
  EXPECT_ER(rsa_oaep_t::execute(&prv_key, int(hash_e::none), int(hash_e::sha256), mem_t(), ciphertext, decrypted));
  EXPECT_ER(rsa_oaep_t::execute(&prv_key, int(hash_e::sha256), int(hash_e::none), mem_t(), ciphertext, decrypted));

  buf_t malformed = ciphertext;
  malformed[0] ^= 0x01;
  EXPECT_ER(callback_oaep.execute(hash_e::sha256, hash_e::sha256, mem_t(), malformed, decrypted));
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

TEST(RSA_OAEP, RejectsWrongLabelHashAndOversizedPlaintext) {
  rsa_prv_key_t sk;
  sk.generate(RSA_KEY_LENGTH);
  rsa_pub_key_t pk = sk.pub();

  const buf_t label = buf_t("label");
  const buf_t wrong_label = buf_t("wrong-label");
  const buf_t message = buf_t("short-message");

  buf_t ct;
  ASSERT_OK(pk.encrypt_oaep(message, hash_e::sha256, hash_e::sha256, label, ct));

  buf_t pt;
  EXPECT_OK(sk.decrypt_oaep(ct, hash_e::sha256, hash_e::sha256, label, pt));
  EXPECT_EQ(pt, message);

  EXPECT_ER(sk.decrypt_oaep(ct, hash_e::sha256, hash_e::sha256, wrong_label, pt));
  EXPECT_ER(sk.decrypt_oaep(ct, hash_e::sha512, hash_e::sha256, label, pt));

  const int max_plaintext = pk.size() - 2 * hash_alg_t::get(hash_e::sha256).size - 2;
  buf_t too_long(max_plaintext + 1);
  memset(too_long.data(), 0x61, too_long.size());
  EXPECT_ER(pk.encrypt_oaep(too_long, hash_e::sha256, hash_e::sha256, label, ct));

  buf_t malformed = ct;
  malformed[0] ^= 0x01;
  EXPECT_ER(sk.decrypt_oaep(malformed, hash_e::sha256, hash_e::sha256, label, pt));
}

}  // namespace
