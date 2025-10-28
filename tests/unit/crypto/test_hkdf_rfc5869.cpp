#include <gtest/gtest.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_hash.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

static buf_t from_hex(const char* hex) {
  buf_t out;
  strext::from_hex(out, std::string(hex));
  return out;
}

// RFC 5869, Appendix A.1 Test Case 1 (HKDF with SHA-256)
TEST(HKDF_RFC5869, TestCase1_SHA256) {
  const buf_t ikm = from_hex("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b");
  const buf_t salt = from_hex("000102030405060708090a0b0c");
  const buf_t info = from_hex("f0f1f2f3f4f5f6f7f8f9");
  const int L = 42;

  const buf_t prk_expected = from_hex("077709362c2e32df0ddc3f0dc47bba6390b6c73bb50f9c3122ec844ad7c2b3e5");
  const buf_t okm_expected = from_hex(
      "3cb25f25faacd57a90434f64d0362f2a2d2d0a90cf1a5a4c5db02d56ecc4c5bf"
      "34007208d5b887185865");

  buf_t prk = hkdf_extract_sha256(salt, ikm);
  EXPECT_EQ(prk, prk_expected);

  buf_t okm = hkdf_expand_sha256(prk, info, L);
  EXPECT_EQ(okm, okm_expected);
}

}  // namespace
