#include <gtest/gtest.h>

#include <cbmpc/crypto/tdh2.h>

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

}  // namespace
