#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/mlkem768.h>

#include "utils/test_macros.h"

namespace {

using namespace coinbase::crypto;
using coinbase::buf_t;

TEST(MLKEM768, PubRejectsUninitializedPrivateKey) {
  mlkem768_prv_key_t prv_key;
  EXPECT_CB_ASSERT(prv_key.pub(), "ctx");
}

TEST(MLKEM768, EncapsulateDecapsulateRoundTrip) {
  mlkem768_prv_key_t prv_key;
  prv_key.generate();
  mlkem768_pub_key_t pub_key = prv_key.pub();

  buf_t shared_secret_enc;
  buf_t encapsulated;
  pub_key.encapsulate(shared_secret_enc, encapsulated);

  buf_t shared_secret_dec;
  EXPECT_OK(prv_key.decapsulate(encapsulated, shared_secret_dec));
  EXPECT_FALSE(shared_secret_enc.empty());
  EXPECT_FALSE(encapsulated.empty());
  EXPECT_EQ(shared_secret_enc, shared_secret_dec);
}

TEST(MLKEM768, SeededEncapsulationIsDeterministic) {
  mlkem768_prv_key_t prv_key;
  prv_key.generate();
  mlkem768_pub_key_t pub_key = prv_key.pub();
  buf_t seed = gen_random(32);

  buf_t shared_secret_1, shared_secret_2;
  buf_t encapsulated_1, encapsulated_2;
  pub_key.encapsulate(seed, shared_secret_1, encapsulated_1);
  pub_key.encapsulate(seed, shared_secret_2, encapsulated_2);

  EXPECT_EQ(shared_secret_1, shared_secret_2);
  EXPECT_EQ(encapsulated_1, encapsulated_2);

  buf_t shared_secret_dec;
  EXPECT_OK(prv_key.decapsulate(encapsulated_1, shared_secret_dec));
  EXPECT_EQ(shared_secret_1, shared_secret_dec);
}

}  // namespace
