#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_bn256.h>
#include <cbmpc/internal/crypto/ro.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

TEST(BigNumber256, Elementary) {
  const mod_t q = bn_t::from_string("7237005577332262213973186563042994240857116359379907606001950938285454250989");

  bn256_t zero = 0;
  bn256_t one = 1;

  for (int i = 0; i < 10000; i++) {
    auto a = bn256_t::rand(q);
    bn_t a_bn = a;
    auto b = bn256_t::rand(q);
    bn_t b_bn = b;
    auto c = bn256_t::rand(q);
    bn_t c_bn;

    MODULO(q) {
      c_bn = a_bn + b_bn;
      ASSERT_EQ(bn_t(a + b), c_bn);

      c_bn = a_bn - b_bn;
      ASSERT_EQ(bn_t(a - b), c_bn);

      c_bn = a_bn * b_bn;
      ASSERT_EQ(bn_t(a * b), c_bn);

      ASSERT_EQ(a + zero, a);
      ASSERT_EQ(a - zero, a);
      ASSERT_EQ(zero - a, -a);
      ASSERT_EQ(a - b, -(b - a));
      ASSERT_NE(a + one, a);
      ASSERT_NE(a - one, a);
      ASSERT_EQ(a + b, b + a);
      ASSERT_EQ(a * one, a);
      ASSERT_EQ(a * zero, zero);
      ASSERT_EQ(a * b, b * a);
      ASSERT_EQ(a + (b + c), (a + b) + c);
      ASSERT_EQ(a * (b + c), a * b + a * c);
      ASSERT_EQ(a * (b - c), a * b - a * c);

      b = a;
      ASSERT_EQ(a * a.inv_mod(q), one);
    }
  }
}

TEST(BigNumber256, MontgomeryMatchesReferenceOnSecp256k1) {
  const mod_t& q = curve_secp256k1.order();

  const bn_t a = bn_t::rand(q);
  const bn_t b = bn_t::rand(q);
  const bn_t mul_expected = q.mul(a, b);
  const bn256_t a256(a);
  const bn256_t b256(b);

  MODULO(q) {
    ASSERT_EQ((bn_t)a256.to_mont().from_mont(), a);
    ASSERT_EQ((bn_t)bn256_t::mont_mul(a256.to_mont(), b256.to_mont()).from_mont(), mul_expected);
  }
}

}  // namespace
