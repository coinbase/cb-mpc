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

static void rebind_ciphertext_to_public_key(ciphertext_t& ciphertext, const public_key_t& pub_key, const bn_t& r,
                                            const bn_t& s, mem_t label) {
  const auto& curve = pub_key.Q.get_curve();
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  ciphertext.R2 = r * pub_key.Gamma;
  const ecc_point_t W1 = s * G;
  const ecc_point_t W2 = s * pub_key.Gamma;
  ciphertext.e = ro::hash_number(ciphertext.c, label, ciphertext.R1, W1, ciphertext.R2, W2, ciphertext.iv).mod(q);
  MODULO(q) ciphertext.f = s + r * ciphertext.e;
}

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

TEST_F(TDH2, PublicAndPrivateKeyHelpersRoundTrip) {
  vartime_scope_t vartime_scope;
  const ecurve_t curve = curve_p256;
  const bn_t x = curve.get_random_value();
  const buf_t sid = crypto::gen_random(32);
  const public_key_t pub(x * curve.generator(), sid);

  EXPECT_TRUE(pub.valid());
  EXPECT_FALSE(public_key_t().valid());

  public_key_t decoded;
  ASSERT_OK(decoded.from_bin(pub.to_bin()));
  EXPECT_EQ(decoded, pub);
  EXPECT_FALSE(decoded != pub);

  public_key_t different_q = pub;
  different_q.Q = (x + 1) * curve.generator();
  EXPECT_FALSE(different_q == pub);
  EXPECT_TRUE(different_q != pub);

  public_key_t different_gamma = pub;
  different_gamma.Gamma = different_gamma.Gamma + curve.generator();
  EXPECT_FALSE(different_gamma == pub);
  EXPECT_TRUE(different_gamma != pub);
  EXPECT_FALSE(different_gamma.valid());

  public_key_t different_sid = decoded;
  different_sid.sid[0] ^= 0x01;
  EXPECT_FALSE(different_sid == pub);
  EXPECT_TRUE(different_sid != pub);
  EXPECT_FALSE(different_sid.valid());

  private_key_t private_key;
  EXPECT_FALSE(private_key.valid());
  private_key.x = x;
  private_key.pub_key = pub;
  EXPECT_TRUE(private_key.valid());
  EXPECT_EQ(private_key.pub(), pub);

  private_key_t private_key_roundtrip;
  ASSERT_OK(coinbase::convert(private_key_roundtrip, coinbase::convert(private_key)));
  EXPECT_EQ(private_key_roundtrip.x, x);
  EXPECT_EQ(private_key_roundtrip.pub(), pub);
  EXPECT_TRUE(private_key_roundtrip.valid());
}

TEST_F(TDH2, ExplicitEncryptIsDeterministicForFixedRandomness) {
  public_key_t enc_key;
  crypto::tdh2::pub_shares_t pub_shares;
  std::vector<private_share_t> dec_shares;
  testutils::generate_additive_shares(2, enc_key, pub_shares, dec_shares, curve_p256);

  const buf_t label = buf_t("tdh2-label");
  const buf_t plain = buf_t("tdh2-plain");
  const bn_t r = bn_t(17);
  const bn_t s = bn_t(19);
  const buf_t iv = bn_t(0xabcdef).to_bin(iv_size);

  const ciphertext_t ct1 = enc_key.encrypt(plain, label, r, s, iv);
  const ciphertext_t ct2 = enc_key.encrypt(plain, label, r, s, iv);
  EXPECT_EQ(coinbase::convert(ct1), coinbase::convert(ct2));
  EXPECT_OK(ct1.verify(enc_key, label));

  const ciphertext_t different_r = enc_key.encrypt(plain, label, r + 1, s, iv);
  EXPECT_NE(coinbase::convert(ct1), coinbase::convert(different_r));
}

TEST_F(TDH2, CiphertextVerifyRejectsMalformedFields) {
  public_key_t enc_key;
  crypto::tdh2::pub_shares_t pub_shares;
  std::vector<private_share_t> dec_shares;
  testutils::generate_additive_shares(2, enc_key, pub_shares, dec_shares, curve_p256);

  const buf_t label = crypto::gen_random(10);
  const buf_t plain = crypto::gen_random(32);
  const ciphertext_t ciphertext = enc_key.encrypt(plain, label);
  const mod_t& q = curve_p256.order();

  ciphertext_t bad_iv = ciphertext;
  bad_iv.iv = crypto::gen_random(iv_size - 1);
  EXPECT_ER(bad_iv.verify(enc_key, label));

  ciphertext_t bad_e = ciphertext;
  bad_e.e = q.value();
  EXPECT_ER(bad_e.verify(enc_key, label));

  ciphertext_t bad_f = ciphertext;
  bad_f.f = q.value();
  EXPECT_ER(bad_f.verify(enc_key, label));

  ciphertext_t bad_R1 = ciphertext;
  bad_R1.R1 = curve_secp256k1.generator();
  EXPECT_ER(bad_R1.verify(enc_key, label));

  ciphertext_t bad_R2 = ciphertext;
  bad_R2.R2 = curve_secp256k1.generator();
  EXPECT_ER(bad_R2.verify(enc_key, label));

  ciphertext_t bad_challenge = ciphertext;
  bad_challenge.c[0] ^= 0x01;
  EXPECT_ER(bad_challenge.verify(enc_key, label));
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

TEST_F(TDH2, PartialDecryptionSerializationAndTamperChecks) {
  public_key_t enc_key;
  crypto::tdh2::pub_shares_t pub_shares;
  std::vector<private_share_t> dec_shares;
  testutils::generate_additive_shares(2, enc_key, pub_shares, dec_shares, curve_p256);

  const buf_t label = crypto::gen_random(10);
  const buf_t plain = crypto::gen_random(32);
  const ciphertext_t ciphertext = enc_key.encrypt(plain, label);
  const mod_t& q = curve_p256.order();

  partial_decryption_t partial;
  ASSERT_OK(dec_shares[0].decrypt(ciphertext, label, partial));
  EXPECT_OK(partial.check_partial_decryption_helper(pub_shares[0], ciphertext, curve_p256));

  partial_decryption_t decoded;
  ASSERT_OK(coinbase::convert(decoded, coinbase::convert(partial)));
  EXPECT_EQ(decoded.rid, partial.rid);
  EXPECT_EQ(decoded.Xi, partial.Xi);
  EXPECT_EQ(decoded.ei, partial.ei);
  EXPECT_EQ(decoded.fi, partial.fi);

  EXPECT_ER(dec_shares[0].decrypt(ciphertext, buf_t("wrong-label"), decoded));

  partial_decryption_t bad_ei = partial;
  bad_ei.ei = q.value();
  EXPECT_ER(bad_ei.check_partial_decryption_helper(pub_shares[0], ciphertext, curve_p256));

  partial_decryption_t bad_fi = partial;
  bad_fi.fi = q.value();
  EXPECT_ER(bad_fi.check_partial_decryption_helper(pub_shares[0], ciphertext, curve_p256));

  partial_decryption_t bad_Qi = partial;
  EXPECT_ER(bad_Qi.check_partial_decryption_helper(curve_secp256k1.generator(), ciphertext, curve_p256));

  partial_decryption_t bad_Xi = partial;
  bad_Xi.Xi = curve_secp256k1.generator();
  EXPECT_ER(bad_Xi.check_partial_decryption_helper(pub_shares[0], ciphertext, curve_p256));

  partial_decryption_t bad_proof = partial;
  MODULO(q) bad_proof.ei += 1;
  EXPECT_ER(bad_proof.check_partial_decryption_helper(pub_shares[0], ciphertext, curve_p256));
}

TEST_F(TDH2, CombineAdditiveRejectsForgedPublicShareContext) {
  constexpr int n = 3;
  const bn_t victim_x = curve_p256.get_random_value();
  const public_key_t victim_key(victim_x * curve_p256.generator(), gen_random(32));

  public_key_t fake_key;
  crypto::tdh2::pub_shares_t fake_pub_shares;
  std::vector<private_share_t> fake_dec_shares;
  testutils::generate_additive_shares(n, fake_key, fake_pub_shares, fake_dec_shares, curve_p256);

  const buf_t label = buf_t("tdh2-label");
  const buf_t plain = buf_t("forged-plaintext");
  const bn_t r = bn_t(17);
  const bn_t s = bn_t(19);
  const buf_t iv = bn_t(0xabcdef).to_bin(iv_size);
  ciphertext_t forged_ciphertext = fake_key.encrypt(plain, label, r, s, iv);

  partial_decryptions_t fake_partials(n);
  for (int i = 0; i < n; i++) {
    ASSERT_OK(fake_dec_shares[i].decrypt(forged_ciphertext, label, fake_partials[i]));
  }

  rebind_ciphertext_to_public_key(forged_ciphertext, victim_key, r, s, label);
  ASSERT_OK(forged_ciphertext.verify(victim_key, label));

  buf_t decrypted;
  EXPECT_ER(combine_additive(victim_key, fake_pub_shares, label, fake_partials, forged_ciphertext, decrypted));
}

TEST_F(TDH2, CombineAdditiveRejectsDuplicateRoleInForgedPublicShareContext) {
  const auto& G = curve_p256.generator();
  const mod_t& q = curve_p256.order();
  const bn_t victim_x = bn_t(101);
  const bn_t x1 = bn_t(31);
  const bn_t x2 = bn_t(37);
  bn_t fake_x;
  MODULO(q) fake_x = x1 + x2 + x2;

  const public_key_t victim_key(victim_x * G, gen_random(32));
  const public_key_t fake_key(fake_x * G, gen_random(32));
  const ecc_point_t Q1 = x1 * G;
  const ecc_point_t Q2 = x2 * G;
  crypto::tdh2::pub_shares_t forged_pub_shares = {Q1, Q2, victim_key.Q - Q1 - Q2};
  ASSERT_EQ(forged_pub_shares[0] + forged_pub_shares[1] + forged_pub_shares[2], victim_key.Q);

  const buf_t label = buf_t("tdh2-label");
  const buf_t plain = buf_t("forged-plaintext");
  const bn_t r = bn_t(41);
  const bn_t s = bn_t(43);
  const buf_t iv = bn_t(0x654321).to_bin(iv_size);
  ciphertext_t forged_ciphertext = fake_key.encrypt(plain, label, r, s, iv);

  private_share_t share1;
  share1.pub_key = fake_key;
  share1.x = x1;
  share1.rid = 1;
  private_share_t share2;
  share2.pub_key = fake_key;
  share2.x = x2;
  share2.rid = 2;

  partial_decryptions_t forged_partials(3);
  ASSERT_OK(share1.decrypt(forged_ciphertext, label, forged_partials[0]));
  ASSERT_OK(share2.decrypt(forged_ciphertext, label, forged_partials[1]));
  forged_partials[2] = forged_partials[1];
  ASSERT_EQ(forged_partials[2].rid, 2);

  rebind_ciphertext_to_public_key(forged_ciphertext, victim_key, r, s, label);
  ASSERT_OK(forged_ciphertext.verify(victim_key, label));

  buf_t decrypted;
  EXPECT_ER(combine_additive(victim_key, forged_pub_shares, label, forged_partials, forged_ciphertext, decrypted));
}

TEST_F(TDH2, CombineAdditiveRejectsMalformedInputs) {
  const int n = 3;
  public_key_t enc_key;
  crypto::tdh2::pub_shares_t pub_shares;
  std::vector<private_share_t> dec_shares;
  testutils::generate_additive_shares(n, enc_key, pub_shares, dec_shares, curve_p256);

  const buf_t label = crypto::gen_random(10);
  const buf_t plain = crypto::gen_random(32);
  const ciphertext_t ciphertext = enc_key.encrypt(plain, label);

  partial_decryptions_t partial_decryptions(n);
  for (int i = 0; i < n; i++) {
    ASSERT_OK(dec_shares[i].decrypt(ciphertext, label, partial_decryptions[i]));
  }

  buf_t decrypted;
  partial_decryptions_t short_partials = partial_decryptions;
  short_partials.pop_back();
  EXPECT_ER(combine_additive(enc_key, pub_shares, label, short_partials, ciphertext, decrypted));

  partial_decryptions_t bad_low_rid = partial_decryptions;
  bad_low_rid[0].rid = 0;
  EXPECT_ER(combine_additive(enc_key, pub_shares, label, bad_low_rid, ciphertext, decrypted));

  partial_decryptions_t bad_high_rid = partial_decryptions;
  bad_high_rid[0].rid = n + 1;
  EXPECT_ER(combine_additive(enc_key, pub_shares, label, bad_high_rid, ciphertext, decrypted));

  partial_decryptions_t bad_partial = partial_decryptions;
  MODULO(curve_p256.order()) bad_partial[0].fi += 1;
  EXPECT_ER(combine_additive(enc_key, pub_shares, label, bad_partial, ciphertext, decrypted));

  crypto::tdh2::pub_shares_t bad_pub_shares = pub_shares;
  bad_pub_shares[0] = curve_secp256k1.generator();
  EXPECT_ER(combine_additive(enc_key, bad_pub_shares, label, partial_decryptions, ciphertext, decrypted));

  ciphertext_t bad_ciphertext = ciphertext;
  bad_ciphertext.c[0] ^= 0x01;
  EXPECT_ER(combine_additive(enc_key, pub_shares, label, partial_decryptions, bad_ciphertext, decrypted));
}

TEST_F(TDH2, CombineFailsWithInsufficientQuorum) {
  test_ac.curve = curve_p256;
  public_key_t enc_key;
  ss::ac_pub_shares_t pub_shares;
  ss::party_map_t<private_share_t> dec_shares;
  testutils::generate_ac_shares(test_ac, enc_key, pub_shares, dec_shares, curve_p256);

  const buf_t label = crypto::gen_random(10);
  const buf_t plain = crypto::gen_random(32);
  const ciphertext_t ciphertext = enc_key.encrypt(plain, label);

  ss::party_map_t<partial_decryption_t> partial_decryptions;
  auto it = dec_shares.begin();
  partial_decryption_t partial;
  EXPECT_OK(it->second.decrypt(ciphertext, label, partial));
  partial_decryptions[it->first] = std::move(partial);

  buf_t decrypted;
  EXPECT_ER(combine(test_ac, enc_key, pub_shares, label, partial_decryptions, ciphertext, decrypted));
}

TEST_F(TDH2, CombineACRejectsForgedPublicShareContext) {
  test_ac.curve = curve_p256;
  const bn_t victim_x = curve_p256.get_random_value();
  const public_key_t victim_key(victim_x * curve_p256.generator(), gen_random(32));

  public_key_t fake_key;
  ss::ac_pub_shares_t fake_pub_shares;
  ss::party_map_t<private_share_t> fake_dec_shares;
  testutils::generate_ac_shares(test_ac, fake_key, fake_pub_shares, fake_dec_shares, curve_p256);

  const buf_t label = buf_t("tdh2-label");
  const buf_t plain = buf_t("forged-plaintext");
  const bn_t r = bn_t(23);
  const bn_t s = bn_t(29);
  const buf_t iv = bn_t(0x123456).to_bin(iv_size);
  ciphertext_t forged_ciphertext = fake_key.encrypt(plain, label, r, s, iv);

  ss::party_map_t<partial_decryption_t> fake_partials;
  for (const auto& [name, share] : fake_dec_shares) {
    partial_decryption_t partial;
    ASSERT_OK(share.decrypt(forged_ciphertext, label, partial));
    fake_partials[name] = std::move(partial);
  }

  rebind_ciphertext_to_public_key(forged_ciphertext, victim_key, r, s, label);
  ASSERT_OK(forged_ciphertext.verify(victim_key, label));

  buf_t decrypted;
  EXPECT_ER(combine(test_ac, victim_key, fake_pub_shares, label, fake_partials, forged_ciphertext, decrypted));
}

TEST_F(TDH2, CombineACRejectsTamperedInputs) {
  test_ac.curve = curve_p256;
  public_key_t enc_key;
  ss::ac_pub_shares_t pub_shares;
  ss::party_map_t<private_share_t> dec_shares;
  testutils::generate_ac_shares(test_ac, enc_key, pub_shares, dec_shares, curve_p256);

  const buf_t label = crypto::gen_random(10);
  const buf_t plain = crypto::gen_random(32);
  const ciphertext_t ciphertext = enc_key.encrypt(plain, label);

  ss::party_map_t<partial_decryption_t> partial_decryptions;
  for (const auto& [name, share] : dec_shares) {
    partial_decryption_t partial;
    ASSERT_OK(share.decrypt(ciphertext, label, partial));
    partial_decryptions[name] = std::move(partial);
  }

  buf_t decrypted;
  ss::party_map_t<partial_decryption_t> bad_partial = partial_decryptions;
  MODULO(curve_p256.order()) bad_partial.begin()->second.fi += 1;
  EXPECT_ER(combine(test_ac, enc_key, pub_shares, label, bad_partial, ciphertext, decrypted));

  ss::party_map_t<partial_decryption_t> bad_Xi = partial_decryptions;
  bad_Xi.begin()->second.Xi = curve_secp256k1.generator();
  EXPECT_ER(combine(test_ac, enc_key, pub_shares, label, bad_Xi, ciphertext, decrypted));

  ss::ac_pub_shares_t missing_pub_share = pub_shares;
  missing_pub_share.erase(partial_decryptions.begin()->first);
  EXPECT_ER(combine(test_ac, enc_key, missing_pub_share, label, partial_decryptions, ciphertext, decrypted));

  ss::ac_pub_shares_t bad_pub_share = pub_shares;
  bad_pub_share.begin()->second = bad_pub_share.begin()->second + curve_p256.generator();
  EXPECT_ER(combine(test_ac, enc_key, bad_pub_share, label, partial_decryptions, ciphertext, decrypted));

  ciphertext_t bad_ciphertext = ciphertext;
  bad_ciphertext.c[0] ^= 0x01;
  EXPECT_ER(combine(test_ac, enc_key, pub_shares, label, partial_decryptions, bad_ciphertext, decrypted));
}

}  // namespace
