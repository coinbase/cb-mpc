#include <gtest/gtest.h>
#include <utility>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/crypto/base_paillier.h>

#include "utils/test_macros.h"
using namespace coinbase::crypto;

namespace {

TEST(Paillier, CreatePubRejectsInvalidModulus) {
  dylog_disable_scope_t no_log_err;
  paillier_t paillier;

  EXPECT_EQ(paillier.create_pub(-3), E_BADARG);
  EXPECT_EQ(paillier.create_pub(0), E_BADARG);
  EXPECT_EQ(paillier.create_pub(1), E_BADARG);
  EXPECT_EQ(paillier.create_pub(2), E_BADARG);
  EXPECT_EQ(paillier.create_pub(100), E_BADARG);
}

TEST(Paillier, CreatePubAcceptsValidModulus) {
  paillier_t private_paillier;
  private_paillier.generate();

  paillier_t public_paillier;
  ASSERT_EQ(public_paillier.create_pub(private_paillier.get_N()), SUCCESS);
  EXPECT_FALSE(public_paillier.has_private_key());
  EXPECT_EQ(public_paillier.get_N().value(), private_paillier.get_N().value());
}

TEST(Paillier, PrivateKeyAccessorsExposeConsistentKeyMaterial) {
  paillier_t original;
  original.generate();

  const bn_t& p = original.get_p();
  const bn_t& q = original.get_q();
  const bn_t& N = original.get_N();
  const bn_t& phi_N = original.get_phi_N();
  const bn_t& inv_phi_N = original.get_inv_phi_N();

  EXPECT_TRUE(original.has_private_key());
  EXPECT_EQ(p * q, N);
  EXPECT_EQ((p - 1) * (q - 1), phi_N);

  bn_t inverse_check;
  MODULO(original.get_N()) { inverse_check = phi_N * inv_phi_N; }
  EXPECT_EQ(inverse_check, 1);

  paillier_t rebuilt;
  rebuilt.create_prv(N, p, q);
  EXPECT_TRUE(rebuilt.has_private_key());
  EXPECT_EQ(rebuilt.get_N(), original.get_N());
  EXPECT_EQ(rebuilt.get_p(), p);
  EXPECT_EQ(rebuilt.get_q(), q);
  EXPECT_EQ(rebuilt.decrypt(rebuilt.encrypt(42)), 42);
}

TEST(Paillier, CreatePubRejectsOversizedModulus) {
  paillier_t paillier;
  bn_t oversized = (bn_t(1) << paillier_t::bit_size) + 1;

  EXPECT_EQ(paillier.create_pub(oversized), E_BADARG);
}

TEST(Paillier, HomomorphicIdentitiesAndCipherVerification) {
  paillier_t paillier;
  paillier.generate();

  const bn_t m1 = 17;
  const bn_t m2 = 25;
  const bn_t scalar = 3;

  const bn_t c1 = paillier.encrypt(m1);
  const bn_t c2 = paillier.encrypt(m2);

  EXPECT_EQ(paillier.decrypt(paillier.add_ciphers(c1, c2, paillier_t::rerand_e::off)), m1 + m2);
  EXPECT_EQ(paillier.decrypt(paillier.mul_scalar(c1, scalar, paillier_t::rerand_e::off)), m1 * scalar);
  EXPECT_EQ(paillier.decrypt(paillier.add_scalar(c1, scalar, paillier_t::rerand_e::off)), m1 + scalar);

  EXPECT_OK(paillier.verify_cipher(c1));
  EXPECT_ER(paillier.verify_cipher(bn_t(0)));
  EXPECT_ER(paillier.verify_cipher(paillier.get_N()));
}

TEST(Paillier, ElemWrappersPreserveHomomorphicSemantics) {
  paillier_t paillier;
  paillier.generate();
  paillier_t::rerand_scope_t no_rerand(paillier_t::rerand_e::off);

  auto c7 = paillier.enc(7);
  auto c5 = paillier.enc(5);

  EXPECT_EQ(paillier.decrypt(c7), 7);
  EXPECT_OK(paillier.verify_cipher(c7));
  EXPECT_OK(paillier.verify_ciphers(c7.to_bn(), c5.to_bn()));

  paillier_t::elem_t copied(c7);
  EXPECT_TRUE(copied == c7);
  EXPECT_TRUE(copied != c5);

  paillier_t::elem_t moved(std::move(copied));
  EXPECT_EQ(paillier.decrypt(moved), 7);

  paillier_t::elem_t assigned;
  assigned = c7;
  assigned = assigned;
  EXPECT_TRUE(assigned == c7);

  paillier_t::elem_t move_assigned;
  move_assigned = std::move(assigned);
  move_assigned = std::move(move_assigned);
  EXPECT_EQ(paillier.decrypt(move_assigned), 7);

  EXPECT_EQ(paillier.decrypt(c7 + c5), 12);
  EXPECT_EQ(paillier.decrypt(c7 - c5), 2);
  EXPECT_EQ(paillier.decrypt(c7 + bn_t(3)), 10);
  EXPECT_EQ(paillier.decrypt(c7 - bn_t(2)), 5);
  EXPECT_EQ(paillier.decrypt(bn_t(3) * c5), 15);
  EXPECT_EQ(paillier.decrypt(bn_t(20) - c7), 13);

  auto accum = paillier.enc(7);
  accum += c5;
  EXPECT_EQ(paillier.decrypt(accum), 12);
  accum += bn_t(3);
  EXPECT_EQ(paillier.decrypt(accum), 15);
  accum -= bn_t(2);
  EXPECT_EQ(paillier.decrypt(accum), 13);
  accum -= c5;
  EXPECT_EQ(paillier.decrypt(accum), 8);
  accum *= bn_t(4);
  EXPECT_EQ(paillier.decrypt(accum), 32);

  c7.rerand();
  EXPECT_EQ(paillier.decrypt(c7), 7);
  EXPECT_OK(paillier.verify_cipher(c7));
}

TEST(Paillier, PublicKeyRebuildPreservesHomomorphicVerify) {
  paillier_t private_paillier;
  private_paillier.generate();

  paillier_t public_paillier;
  ASSERT_EQ(public_paillier.create_pub(private_paillier.get_N()), SUCCESS);

  const bn_t m1 = 11;
  const bn_t m2 = 13;
  const bn_t c1 = public_paillier.encrypt(m1);
  const bn_t c2 = public_paillier.encrypt(m2);
  EXPECT_OK(public_paillier.verify_cipher(c1));
  EXPECT_OK(public_paillier.verify_cipher(c2));

  const bn_t sum_cipher = public_paillier.add_ciphers(c1, c2, paillier_t::rerand_e::off);
  EXPECT_OK(public_paillier.verify_cipher(sum_cipher));
  EXPECT_EQ(private_paillier.decrypt(sum_cipher), m1 + m2);
}

}  // namespace
