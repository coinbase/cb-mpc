#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/tdh2.h>

#include "utils/data/ac.h"
#include "utils/data/tdh2.h"
#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;
using namespace coinbase::crypto::tdh2;

namespace {

class TDH2 : public testutils::TestAC {};

TEST_F(TDH2, AddCompleteness) {
  int n = 10;
  std::vector<private_share_t> dec_shares;

  public_key_t enc_key;
  crypto::tdh2::pub_shares_t pub_shares;
  testutils::generate_additive_shares(n, enc_key, pub_shares, dec_shares, curve_p256);

  buf_t label = crypto::gen_random(10);

  buf_t plain = crypto::gen_random(32);  // 256 bits
  ciphertext_t ciphertext = enc_key.encrypt(plain, label);

  public_key_t wrong_pub_key = enc_key;
  wrong_pub_key.Gamma = 2 * enc_key.Gamma;
  EXPECT_OK(ciphertext.verify(enc_key, label));
  EXPECT_ER(ciphertext.verify(enc_key, crypto::gen_random(10)));  // wrong label
  EXPECT_ER(ciphertext.verify(wrong_pub_key, label));             // wrong pub key

  partial_decryptions_t partial_decryptions(n);

  for (int i = 0; i < n; i++) {
    EXPECT_OK(dec_shares[i].decrypt(ciphertext, label, partial_decryptions[i]));
  }

  buf_t decrypted;
  EXPECT_OK(combine_additive(enc_key, pub_shares, label, partial_decryptions, ciphertext, decrypted));
  EXPECT_EQ(plain, decrypted);
}

TEST_F(TDH2, ACCompleteness) {
  test_ac.curve = curve_p256;
  public_key_t enc_key;
  ss::ac_pub_shares_t pub_shares;
  ss::party_map_t<private_share_t> dec_shares;
  testutils::generate_ac_shares(test_ac, enc_key, pub_shares, dec_shares, curve_p256);

  buf_t label = crypto::gen_random(10);

  buf_t plain = crypto::gen_random(32);  // 256 bits
  ciphertext_t ciphertext = enc_key.encrypt(plain, label);

  public_key_t wrong_pub_key = enc_key;
  wrong_pub_key.Gamma = 2 * enc_key.Gamma;
  EXPECT_OK(ciphertext.verify(enc_key, label));
  EXPECT_ER(ciphertext.verify(enc_key, crypto::gen_random(10)));  // wrong label
  EXPECT_ER(ciphertext.verify(wrong_pub_key, label));             // wrong pub key

  ss::party_map_t<partial_decryption_t> partial_decryptions;

  for (const auto& [name, share] : dec_shares) {
    partial_decryption_t partial;
    EXPECT_OK(share.decrypt(ciphertext, label, partial));
    partial_decryptions[name] = std::move(partial);
  }

  buf_t decrypted;
  EXPECT_OK(combine(test_ac, enc_key, pub_shares, label, partial_decryptions, ciphertext, decrypted));
  EXPECT_EQ(plain, decrypted);
}

TEST_F(TDH2, CiphertextRoundTripKeepsLabel) {
  int n = 3;
  std::vector<private_share_t> dec_shares;

  public_key_t enc_key;
  crypto::tdh2::pub_shares_t pub_shares;
  testutils::generate_additive_shares(n, enc_key, pub_shares, dec_shares, curve_p256);

  const buf_t label = crypto::gen_random(10);
  const buf_t wrong_label = buf_t("wrong-label");
  const buf_t plain = crypto::gen_random(32);

  const ciphertext_t ciphertext = enc_key.encrypt(plain, label);
  const buf_t serialized = coinbase::convert(ciphertext);

  ciphertext_t roundtrip;
  ASSERT_EQ(coinbase::convert(roundtrip, serialized), SUCCESS);
  EXPECT_EQ(roundtrip.L, label);
  EXPECT_OK(roundtrip.verify(enc_key, label));
  EXPECT_ER(roundtrip.verify(enc_key, wrong_label));
}

TEST_F(TDH2, PublicKeyValidChecksGammaConsistency) {
  // public_key_t::valid() should verify that Gamma is correctly derived from Q and sid
  // This prevents attackers from using rogue Gamma values in encryption/decryption

  vartime_scope_t vartime_scope;
  ecurve_t curve = curve_secp256k1;

  // Build a legitimate public key
  bn_t sk = bn_t::rand(curve.order());
  ecc_point_t Q = sk * curve.generator();
  buf_t sid = crypto::gen_random(32);

  // Constructor derives Gamma from Q and sid
  public_key_t pk(Q, mem_t(sid));
  EXPECT_TRUE(pk.valid());  // Should be valid

  // Tamper with Gamma: replace it with a random unrelated point
  bn_t rogue_scalar = bn_t::rand(curve.order());
  ecc_point_t rogue_Gamma = rogue_scalar * curve.generator();
  pk.Gamma = rogue_Gamma;

  // valid() should detect the Gamma inconsistency and return false
  EXPECT_FALSE(pk.valid());

  // Test with another invalid Gamma
  pk.Gamma = curve.infinity();
  EXPECT_FALSE(pk.valid());

  // Restore correct Gamma - should be valid again
  pk.Gamma = ro::hash_curve(mem_t("TDH2-Gamma"), Q, sid).curve(curve);
  EXPECT_TRUE(pk.valid());
}

}  // namespace
