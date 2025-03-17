#include <gtest/gtest.h>

#include <cbmpc/crypto/elgamal.h>

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

class ElGamal : public testing::Test {
 protected:
  void SetUp() override {
    curve = curve_p256;
    q = curve.order();
    G = curve.generator();
  }

  ecurve_t curve;
  bn_t q;
  ecc_generator_point_t G;
};

TEST_F(ElGamal, Commitment) {
  auto P = curve.mul_to_generator(bn_t::rand(q));
  auto m = bn_t::rand(q);
  auto r = bn_t::rand(q);

  auto E = ec_elgamal_commitment_t::make_commitment(P, m, r);

  EXPECT_EQ(E.L, r * G);
  EXPECT_EQ(E.R, curve.mul_add(m, P, r));
}

TEST_F(ElGamal, API) {
  auto [P, d] = ec_elgamal_commitment_t::local_keygen(curve);

  bn_t a = bn_t::rand_bitlen(250);  // 250 bits
  bn_t b = bn_t::rand_bitlen(250);
  bn_t c = bn_t::rand_bitlen(250);

  ec_elgamal_commitment_t A = ec_elgamal_commitment_t::random_commit(P, a);
  ec_elgamal_commitment_t B = ec_elgamal_commitment_t::random_commit(P, b);

  ec_elgamal_commitment_t A_plus_B = A + B;
  ec_elgamal_commitment_t A_plus_b = A + b;

  ec_elgamal_commitment_t A_plus_B_test =
      ec_elgamal_commitment_t::random_commit(P, a) + ec_elgamal_commitment_t::random_commit(P, b);

  EXPECT_TRUE(ec_elgamal_commitment_t::check_equ(A_plus_B, A_plus_B_test, d));
  EXPECT_TRUE(ec_elgamal_commitment_t::check_equ(A_plus_B_test, A_plus_b, d));

  ec_elgamal_commitment_t A1 = A;
  A1.randomize(P);
  EXPECT_TRUE(ec_elgamal_commitment_t::check_equ(A, A1, d));

  ec_elgamal_commitment_t A_mul_c = c * A;
  ec_elgamal_commitment_t A_mul_c_test = ec_elgamal_commitment_t::random_commit(P, a * c);
  EXPECT_TRUE(ec_elgamal_commitment_t::check_equ(A_mul_c_test, A_mul_c, d));

  int p = 17;
  const mod_t& q = ec_elgamal_commitment_t::order(curve);

  uint64_t d1 = 0, d2 = 0, d3 = 0, d4 = 0, d5 = 0, d6 = 0, f = 0;

  for (int i = 0; i < 20; i++) {
    for (int a = 0; a < p; a++) {
      for (int b = 0; b < p; b++) {
        bool test = (a + b) % p == 0;
        ec_elgamal_commitment_t C1 = ec_elgamal_commitment_t::random_commit(P, a);  // 1
        ec_elgamal_commitment_t X = C1;
        if (b != 0) {
          bn_t temp;
          MODULO(q) temp = bn_t(b) - bn_t(p);
          X += temp;
        }
        bn_t r = bn_t::rand(q);
        X = X * r;
        X.randomize(P);
        bool t = X.check_zero(d);
        EXPECT_EQ(t, test);
      }
    }
  }
}

}  // namespace