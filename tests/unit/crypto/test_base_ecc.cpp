#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_bn256.h>
#include <cbmpc/internal/crypto/base_ecc_secp256k1.h>
#include <cbmpc/internal/crypto/base_pki.h>

#include "utils/test_macros.h"

namespace {
using namespace coinbase::crypto;
using coinbase::buf_t;

class ECC : public ::testing::Test {
 protected:
  void SetUp() override {
    // Setup code if needed
  }

  void TearDown() override {
    // Cleanup code if needed
  }
};

TEST_F(ECC, secp256k1) {
  ecurve_t curve = curve_secp256k1;
  const mod_t& q = curve.order();
  const auto& G = curve.generator();
  EXPECT_TRUE(G.is_on_curve());

  ecc_point_t GG = G;

  for (int i = 0; i < 1000; i++) {
    bn_t a = bn_t::rand(q);
    bn_t b = bn_t::rand(q);
    bn_t c;
    MODULO(q) c = a + b;

    ecc_point_t A = a * G;
    EXPECT_TRUE(A == a * GG);
    ecc_point_t B = b * G;
    EXPECT_TRUE(B == b * GG);
    ecc_point_t C = c * G;
    EXPECT_TRUE(C == c * GG);

    EXPECT_TRUE(A.is_on_curve());
    EXPECT_TRUE(B.is_on_curve());
    EXPECT_TRUE(C.is_on_curve());
    {
      vartime_scope_t vartime_scope;
      EXPECT_TRUE(A + B == C);
    }

    MODULO(q) c = a - b;
    C = c * G;
    EXPECT_TRUE(C.is_on_curve());
    {
      vartime_scope_t vartime_scope;
      EXPECT_TRUE(A - B == C);
    }

    buf_t bin = C.to_compressed_bin();
    ecc_point_t D;
    EXPECT_OK(D.from_bin(curve, bin));
    EXPECT_TRUE(D.is_on_curve());
    EXPECT_TRUE(C == D);

    {
      vartime_scope_t vartime_scope;
      EXPECT_TRUE(((q - 1) * A + A).is_infinity());
      EXPECT_TRUE(((q - 1) * B + B).is_infinity());
      EXPECT_TRUE(((q - 1) * C + C).is_infinity());
    }
  }
}

TEST_F(ECC, secp256k1_batch_get_coordinates) {
  std::vector<ecc_point_t> points;
  for (int i = 1; i <= 8; ++i) points.push_back(bn_t(i) * curve_secp256k1.generator());

  std::vector<bn256_t> xs, ys;
  ecurve_secp256k1_t::get_coordinates(points, xs, ys);

  ASSERT_EQ(xs.size(), points.size());
  ASSERT_EQ(ys.size(), points.size());
  for (size_t i = 0; i < points.size(); ++i) {
    bn_t x, y;
    points[i].get_coordinates(x, y);
    EXPECT_EQ(xs[i], bn256_t(x));
    EXPECT_EQ(ys[i], bn256_t(y));
  }
}

TEST_F(ECC, secp256k1_batch_get_coordinates_rejects_wrong_curve) {
  std::vector<ecc_point_t> points = {ecc_point_t(curve_p256.generator())};
  std::vector<bn256_t> xs, ys;
  EXPECT_CB_ASSERT(ecurve_secp256k1_t::get_coordinates(points, xs, ys), "curve_secp256k1");
}

TEST_F(ECC, SigningScheme2) {
  for (const auto len : {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024}) {
    std::cout << "======================================== len: " << len << std::endl;
    for (int i = 0; i < 5; i++) {
      ecurve_t curve = curve_ed25519;
      const mod_t& q = curve.order();

      ecc_prv_key_t prv_key;
      prv_key.generate(curve);
      ecc_pub_key_t pub_key(prv_key.pub());

      buf_t hash = gen_random(len);
      buf_t signature = prv_key.sign(hash);
      EXPECT_OK(pub_key.verify(hash, signature));
    }
  }
}

}  // namespace
