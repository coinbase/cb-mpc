#include <gtest/gtest.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

TEST(CryptoECC, ExpOnCurve) {
  crypto::vartime_scope_t vartime_scope;
  for (const auto& curve : {coinbase::crypto::curve_p256, coinbase::crypto::curve_p384, coinbase::crypto::curve_p521,
                            coinbase::crypto::curve_secp256k1, coinbase::crypto::curve_ed25519}) {
    bn_t p, a, b;
    curve.get_params(p, a, b);
    mod_t mod_p = mod_t(p, /* multiplicative_dense */ true);

    ecc_point_t Q;
    ecc_point_t G = curve.generator();
    for (int i = 1; i < 10; i++) {
      Q = ro::hash_curve(i).curve(curve);

      EXPECT_TRUE(Q.is_on_curve());
      EXPECT_FALSE(Q.is_infinity());

      bn_t x = Q.get_x();
      bn_t y = Q.get_y();
      bn_t lhs, rhs;
      MODULO(mod_p) {
        lhs = y * y;
        if (curve == coinbase::crypto::curve_ed25519) {
          // curve25519 uses a Montgomery form y^2=x^3+486662x^2+x, with a = 486662 and b = 1
          // But here we are using Twisted Edward curve −x^2+y^2=1−(121665/121666)x^2y^2
          // So a and b are not used and unrelated in the context
          rhs = (x * x + 1) / (x * x * 121665 / 121666 + 1);
        } else
          rhs = x * x * x + a * x + b;
      }

      EXPECT_EQ(lhs, rhs);
    }
  }
}

}  // namespace