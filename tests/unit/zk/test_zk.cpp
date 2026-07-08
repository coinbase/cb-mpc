#include <gtest/gtest.h>
#include <limits>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/zk/fischlin.h>
#include <cbmpc/internal/zk/small_primes.h>
#include <cbmpc/internal/zk/zk_ec.h>
#include <cbmpc/internal/zk/zk_paillier.h>
#include <cbmpc/internal/zk/zk_unknown_order.h>

#include "utils/data/zk_completeness.h"
#include "utils/test_macros.h"

#define REPEAT_COMPLETENESS 1

using namespace coinbase::test::data;
using namespace coinbase::crypto;

namespace {
template <typename T>
T RoundTrip(const T& value) {
  T decoded;
  EXPECT_OK(coinbase::deser(coinbase::ser(value), decoded));
  return decoded;
}

#define TEST_NIZK_COMPLETENESS_CURVES(name, ZKClass, ...) \
  TEST(name, Completeness) {                              \
    for (const auto& curve : {                            \
             coinbase::crypto::curve_p256,                \
             coinbase::crypto::curve_p384,                \
             coinbase::crypto::curve_p521,                \
             coinbase::crypto::curve_secp256k1,           \
             coinbase::crypto::curve_ed25519,             \
         }) {                                             \
      auto zk = new ZKClass(curve, ##__VA_ARGS__);        \
      for (int i = 0; i < REPEAT_COMPLETENESS; i++) {     \
        zk->setup();                                      \
        zk->prove();                                      \
        ASSERT_OK(zk->verify());                          \
        EXPECT_GT(zk->proof_size(), 0);                   \
      }                                                   \
      delete zk;                                          \
    }                                                     \
  }

#define TEST_NIZK_COMPLETENESS_CURVES_CT_ONLY(name, ZKClass, ...) \
  TEST(name, Completeness) {                                      \
    for (const auto& curve : {                                    \
             coinbase::crypto::curve_secp256k1,                   \
             coinbase::crypto::curve_ed25519,                     \
         }) {                                                     \
      auto zk = new ZKClass(curve, ##__VA_ARGS__);                \
      for (int i = 0; i < REPEAT_COMPLETENESS; i++) {             \
        zk->setup();                                              \
        zk->prove();                                              \
        ASSERT_OK(zk->verify());                                  \
        EXPECT_GT(zk->proof_size(), 0);                           \
      }                                                           \
      delete zk;                                                  \
    }                                                             \
  }

#define TEST_NIZK_COMPLETENESS(name, test_nizk_obj) \
  TEST(name, Completeness) {                        \
    auto zk = test_nizk_obj;                        \
    for (int i = 0; i < REPEAT_COMPLETENESS; i++) { \
      zk->setup();                                  \
      zk->prove();                                  \
      ASSERT_OK(zk->verify());                      \
      EXPECT_GT(zk->proof_size(), 0);               \
    }                                               \
  }

#define TEST_2RZK_COMPLETENESS(name, test_nizk_obj) \
  TEST(name, Completeness) {                        \
    auto zk = test_nizk_obj;                        \
    for (int i = 0; i < REPEAT_COMPLETENESS; i++) { \
      zk->setup();                                  \
      zk->v1();                                     \
      EXPECT_GT(zk->v1_size(), 0);                  \
      zk->p2();                                     \
      EXPECT_GT(zk->p2_size(), 0);                  \
      ASSERT_OK(zk->verify());                      \
    }                                               \
  }

#define TEST_3RZK_COMPLETENESS(name, test_nizk_obj) \
  TEST(name, Completeness) {                        \
    auto zk = test_nizk_obj;                        \
    for (int i = 0; i < REPEAT_COMPLETENESS; i++) { \
      zk->setup();                                  \
      zk->p1();                                     \
      EXPECT_GT(zk->p1_size(), 0);                  \
      zk->v2();                                     \
      EXPECT_GT(zk->v2_size(), 0);                  \
      zk->p3();                                     \
      EXPECT_GT(zk->p3_size(), 0);                  \
      ASSERT_OK(zk->verify());                      \
    }                                               \
  }

TEST_NIZK_COMPLETENESS_CURVES(UC_ZK_DL, test_niuc_dl_t);
TEST_NIZK_COMPLETENESS_CURVES(UC_ZK_BatchDL, test_niuc_batch_dl_t, 10);
TEST_NIZK_COMPLETENESS_CURVES(ZK_DH, test_nidh_t);
TEST_NIZK_COMPLETENESS_CURVES_CT_ONLY(UC_ZK_ElGamalCom, test_nizk_uc_elgamal_com);
TEST_NIZK_COMPLETENESS_CURVES_CT_ONLY(ZK_ElGamalComPubShareEqual, test_nizk_elgamal_com_pub_share_equ);
TEST_NIZK_COMPLETENESS_CURVES_CT_ONLY(ZK_ElGamalComMult, test_nizk_elgamal_com_mult);
TEST_NIZK_COMPLETENESS_CURVES_CT_ONLY(ZK_ElGamalComMultPrivateScalar, test_nizk_elgamal_com_mult_private_scalar);
TEST_NIZK_COMPLETENESS(ZK_ValidPaillier, new test_nizk_valid_paillier());
TEST_2RZK_COMPLETENESS(ZK_ValidPaillier_Interactive, new test_2rzk_valid_paillier());
TEST_NIZK_COMPLETENESS(ZK_PaillierZero, new test_nizk_paillier_zero());
TEST_NIZK_COMPLETENESS(ZK_TwoPaillierEqual, new test_nizk_two_paillier_equal());
TEST_3RZK_COMPLETENESS(ZK_TwoPaillierEqualInteractive, new test_3rzk_two_paillier_equal());
TEST_NIZK_COMPLETENESS(ZK_RangePedersen, new test_nizk_range_pedersen());
TEST_3RZK_COMPLETENESS(ZK_RangePedersenInteractiveOpt, new test_i3rzk_range_pedersen());
TEST_NIZK_COMPLETENESS(ZK_PaillierPedersenEqual, new test_nizk_paillier_pedersen_equal());
TEST_3RZK_COMPLETENESS(ZK_PaillierPedersenEqualInteractive, new test_i3rzk_paillier_pedersen_equal());
TEST_NIZK_COMPLETENESS(ZK_PaillierRangeExpSlack, new test_nizk_paillier_range_exp_slack());
TEST_NIZK_COMPLETENESS_CURVES(ZK_PDL, test_nizk_pdl);
TEST_NIZK_COMPLETENESS(ZK_UnknownOrderDL, new test_unknown_order_dl());

TEST(ZKVerifierRejection, UCDLRejectsMalformedAndOutOfRangeTranscripts) {
  dylog_disable_scope_t no_log_err;
  test_niuc_dl_t proof(coinbase::crypto::curve_secp256k1);
  proof.setup();
  proof.prove();
  ASSERT_OK(proof.verify());

  auto missing_a = proof.zk;
  missing_a.A.pop_back();
  EXPECT_ER(missing_a.verify(proof.Q, proof.sid, proof.aux));

  auto missing_e = proof.zk;
  missing_e.e.pop_back();
  EXPECT_ER(missing_e.verify(proof.Q, proof.sid, proof.aux));

  auto out_of_range_z = proof.zk;
  out_of_range_z.z[0] = proof.q;
  EXPECT_ER(out_of_range_z.verify(proof.Q, proof.sid, proof.aux));
}

TEST(ZKVerifierRejection, UCBatchDLRejectsMalformedAndTamperedTranscripts) {
  dylog_disable_scope_t no_log_err;
  test_niuc_batch_dl_t proof(coinbase::crypto::curve_secp256k1, 3);
  proof.setup();
  proof.prove();
  ASSERT_OK(proof.verify());

  auto missing_r = proof.zk;
  missing_r.R.pop_back();
  EXPECT_ER(missing_r.verify(proof.Qs, proof.sid, proof.aux));

  auto missing_e = proof.zk;
  missing_e.e.pop_back();
  EXPECT_ER(missing_e.verify(proof.Qs, proof.sid, proof.aux));

  auto missing_z = proof.zk;
  missing_z.z.pop_back();
  EXPECT_ER(missing_z.verify(proof.Qs, proof.sid, proof.aux));

  auto tampered_commitment = proof.zk;
  tampered_commitment.R[0] += proof.G;
  EXPECT_ER(tampered_commitment.verify(proof.Qs, proof.sid, proof.aux));

  auto out_of_range_z = proof.zk;
  out_of_range_z.z[0] = proof.q;
  EXPECT_ER(out_of_range_z.verify(proof.Qs, proof.sid, proof.aux));

  EXPECT_ER(proof.zk.verify({}, proof.sid, proof.aux));
}

TEST(ZKVerifierRejection, DHRejectsWrongStatementAndTamperedChallenge) {
  dylog_disable_scope_t no_log_err;
  test_nidh_t proof(coinbase::crypto::curve_secp256k1);
  proof.setup();
  proof.prove();
  ASSERT_OK(proof.verify());

  EXPECT_ER(proof.zk.verify(proof.Q, proof.A, proof.B + proof.G, proof.sid, proof.aux));

  auto bad_challenge = proof.zk;
  bad_challenge.e += 1;
  EXPECT_ER(bad_challenge.verify(proof.Q, proof.A, proof.B, proof.sid, proof.aux));

  auto out_of_range_z = proof.zk;
  out_of_range_z.z = proof.q;
  EXPECT_ER(out_of_range_z.verify(proof.Q, proof.A, proof.B, proof.sid, proof.aux));
}

TEST(ZKVerifierRejection, ElGamalProofsRejectMalformedAndTamperedTranscripts) {
  dylog_disable_scope_t no_log_err;

  test_nizk_uc_elgamal_com uc_com(coinbase::crypto::curve_secp256k1);
  uc_com.setup();
  uc_com.prove();
  ASSERT_OK(uc_com.verify());

  auto missing_ab = uc_com.zk;
  missing_ab.AB.pop_back();
  EXPECT_ER(missing_ab.verify(uc_com.Q, *uc_com.UV, uc_com.sid, uc_com.aux));

  auto missing_z1 = uc_com.zk;
  missing_z1.z1.pop_back();
  EXPECT_ER(missing_z1.verify(uc_com.Q, *uc_com.UV, uc_com.sid, uc_com.aux));

  auto missing_e = uc_com.zk;
  missing_e.e.pop_back();
  EXPECT_ER(missing_e.verify(uc_com.Q, *uc_com.UV, uc_com.sid, uc_com.aux));

  auto missing_z2 = uc_com.zk;
  missing_z2.z2.pop_back();
  EXPECT_ER(missing_z2.verify(uc_com.Q, *uc_com.UV, uc_com.sid, uc_com.aux));

  auto out_of_range_z1 = uc_com.zk;
  out_of_range_z1.z1[0] = uc_com.q;
  EXPECT_ER(out_of_range_z1.verify(uc_com.Q, *uc_com.UV, uc_com.sid, uc_com.aux));

  auto out_of_range_z2 = uc_com.zk;
  out_of_range_z2.z2[0] = uc_com.q;
  EXPECT_ER(out_of_range_z2.verify(uc_com.Q, *uc_com.UV, uc_com.sid, uc_com.aux));

  test_nizk_elgamal_com_pub_share_equ pub_share(coinbase::crypto::curve_secp256k1);
  pub_share.setup();
  pub_share.prove();
  ASSERT_OK(pub_share.verify());

  auto bad_pub_share = pub_share.zk;
  bad_pub_share.zk_dh.z += 1;
  EXPECT_ER(bad_pub_share.verify(pub_share.E, pub_share.A, pub_share.eA, pub_share.sid, pub_share.aux));

  test_nizk_elgamal_com_mult mult(coinbase::crypto::curve_secp256k1);
  mult.setup();
  mult.prove();
  ASSERT_OK(mult.verify());

  auto bad_mult = mult.zk;
  bad_mult.e += 1;
  EXPECT_ER(bad_mult.verify(mult.E, mult.eA, mult.eB, mult.eC, mult.sid, mult.aux));

  auto out_of_range_mult_challenge = mult.zk;
  out_of_range_mult_challenge.e = mult.q;
  EXPECT_ER(out_of_range_mult_challenge.verify(mult.E, mult.eA, mult.eB, mult.eC, mult.sid, mult.aux));

  auto out_of_range_mult_response = mult.zk;
  out_of_range_mult_response.z1 = mult.q;
  EXPECT_ER(out_of_range_mult_response.verify(mult.E, mult.eA, mult.eB, mult.eC, mult.sid, mult.aux));

  test_nizk_elgamal_com_mult_private_scalar private_scalar(coinbase::crypto::curve_secp256k1);
  private_scalar.setup();
  private_scalar.prove();
  ASSERT_OK(private_scalar.verify());

  auto missing_a1 = private_scalar.zk;
  missing_a1.A1_tag.pop_back();
  EXPECT_ER(missing_a1.verify(private_scalar.E, private_scalar.eA, private_scalar.eB, private_scalar.sid,
                              private_scalar.aux));

  auto missing_a2 = private_scalar.zk;
  missing_a2.A2_tag.pop_back();
  EXPECT_ER(missing_a2.verify(private_scalar.E, private_scalar.eA, private_scalar.eB, private_scalar.sid,
                              private_scalar.aux));

  auto missing_private_e = private_scalar.zk;
  missing_private_e.e.pop_back();
  EXPECT_ER(missing_private_e.verify(private_scalar.E, private_scalar.eA, private_scalar.eB, private_scalar.sid,
                                     private_scalar.aux));

  auto missing_private_z1 = private_scalar.zk;
  missing_private_z1.z1.pop_back();
  EXPECT_ER(missing_private_z1.verify(private_scalar.E, private_scalar.eA, private_scalar.eB, private_scalar.sid,
                                      private_scalar.aux));

  auto missing_private_z2 = private_scalar.zk;
  missing_private_z2.z2.pop_back();
  EXPECT_ER(missing_private_z2.verify(private_scalar.E, private_scalar.eA, private_scalar.eB, private_scalar.sid,
                                      private_scalar.aux));

  auto out_of_range_private_z1 = private_scalar.zk;
  out_of_range_private_z1.z1[0] = private_scalar.q;
  EXPECT_ER(out_of_range_private_z1.verify(private_scalar.E, private_scalar.eA, private_scalar.eB, private_scalar.sid,
                                           private_scalar.aux));

  auto out_of_range_private_z = private_scalar.zk;
  out_of_range_private_z.z2[0] = private_scalar.q;
  EXPECT_ER(out_of_range_private_z.verify(private_scalar.E, private_scalar.eA, private_scalar.eB, private_scalar.sid,
                                          private_scalar.aux));
}

TEST(ZKVerifierRejection, PaillierProofsRejectMalformedAndTamperedTranscripts) {
  dylog_disable_scope_t no_log_err;

  test_nizk_valid_paillier valid_key;
  valid_key.setup();
  valid_key.prove();
  ASSERT_OK(valid_key.verify());

  auto bad_valid_key = valid_key.zk;
  bad_valid_key.sigma[0] = 0;
  EXPECT_ER(bad_valid_key.verify(valid_key.v_p, valid_key.sid, valid_key.aux));

  test_2rzk_valid_paillier valid_key_interactive;
  valid_key_interactive.setup();
  valid_key_interactive.v1();
  valid_key_interactive.p2();
  ASSERT_OK(valid_key_interactive.verify());

  auto bad_valid_key_msg = valid_key_interactive.p2_msg;
  bad_valid_key_msg.sigma[0] = 0;
  EXPECT_ER(
      valid_key_interactive.zk.verify(valid_key_interactive.v_p, valid_key_interactive.prover_pid, bad_valid_key_msg));

  test_nizk_paillier_zero zero;
  zero.setup();
  zero.prove();
  ASSERT_OK(zero.verify());

  auto missing_zero_challenge = zero.zk;
  missing_zero_challenge.e = coinbase::buf_t();
  EXPECT_ER(missing_zero_challenge.verify(zero.v_p, zero.c, zero.sid, zero.aux));

  auto bad_zero_response = zero.zk;
  bad_zero_response.z[0] = 0;
  EXPECT_ER(bad_zero_response.verify(zero.v_p, zero.c, zero.sid, zero.aux));

  auto bad_zero_challenge = zero.zk;
  bad_zero_challenge.e[0] ^= 1;
  EXPECT_ER(bad_zero_challenge.verify(zero.v_p, zero.c, zero.sid, zero.aux));

  test_nizk_two_paillier_equal equal;
  equal.setup();
  equal.prove();
  ASSERT_OK(equal.verify());

  auto missing_equal_challenge = equal.zk;
  missing_equal_challenge.e = coinbase::buf_t();
  EXPECT_ER(missing_equal_challenge.verify(equal.q, equal.v_p1, equal.c1, equal.v_p2, equal.c2, equal.sid, equal.aux));

  auto bad_equal_randomizer = equal.zk;
  bad_equal_randomizer.r0_hat[0] = 0;
  EXPECT_ER(bad_equal_randomizer.verify(equal.q, equal.v_p1, equal.c1, equal.v_p2, equal.c2, equal.sid, equal.aux));

  auto bad_equal_challenge = equal.zk;
  bad_equal_challenge.e[0] ^= 1;
  EXPECT_ER(bad_equal_challenge.verify(equal.q, equal.v_p1, equal.c1, equal.v_p2, equal.c2, equal.sid, equal.aux));

  auto out_of_range_equal_d = equal.zk;
  out_of_range_equal_d.d[0] = equal.q << (coinbase::zk::two_paillier_equal_t::param::log_alpha + SEC_P_STAT);
  EXPECT_ER(out_of_range_equal_d.verify(equal.q, equal.v_p1, equal.c1, equal.v_p2, equal.c2, equal.sid, equal.aux));

  auto bad_equal_second_randomizer = equal.zk;
  bad_equal_second_randomizer.r1_hat[0] = 0;
  EXPECT_ER(
      bad_equal_second_randomizer.verify(equal.q, equal.v_p1, equal.c1, equal.v_p2, equal.c2, equal.sid, equal.aux));

  test_3rzk_two_paillier_equal interactive_equal;
  interactive_equal.setup();
  interactive_equal.p1();
  interactive_equal.v2();

  auto short_challenge = interactive_equal.msg2;
  short_challenge.e = coinbase::buf_t("short");
  coinbase::zk::two_paillier_equal_interactive_t::prover_msg2_t unused_msg3;
  EXPECT_ER(interactive_equal.zk.prover_msg2(interactive_equal.p_p1, interactive_equal.p_p2, interactive_equal.x,
                                             interactive_equal.r1, interactive_equal.r2, short_challenge, unused_msg3));

  interactive_equal.p3();
  ASSERT_OK(interactive_equal.verify());

  auto bad_msg3 = interactive_equal.msg3;
  bad_msg3.d[0] += 1;
  EXPECT_ER(interactive_equal.zk.verify(interactive_equal.q, interactive_equal.v_p1, interactive_equal.c1,
                                        interactive_equal.v_p2, interactive_equal.c2, interactive_equal.msg1,
                                        bad_msg3));

  test_nizk_pdl pdl(coinbase::crypto::curve_secp256k1);
  pdl.setup();
  pdl.prove();
  ASSERT_OK(pdl.verify());

  auto bad_pdl = pdl.zk;
  bad_pdl.z += 1;
  EXPECT_ER(bad_pdl.verify(pdl.c, pdl.v_p, pdl.Q1, pdl.sid, pdl.aux));

  auto bad_pdl_cipher = pdl.zk;
  bad_pdl_cipher.c_r = pdl.v_p.get_NN();
  EXPECT_ER(bad_pdl_cipher.verify(pdl.c, pdl.v_p, pdl.Q1, pdl.sid, pdl.aux));
}

TEST(ZKVerifierRejection, PedersenProofsRejectInvalidStatementsAndTampering) {
  dylog_disable_scope_t no_log_err;

  test_nizk_range_pedersen range;
  range.setup();
  range.prove();
  ASSERT_OK(range.verify());

  EXPECT_ER(range.zk.verify(range.q, 0, range.sid, range.aux));

  auto out_of_range_d = range.zk;
  out_of_range_d.d[0] = range.p_tag;
  EXPECT_ER(out_of_range_d.verify(range.q, range.c, range.sid, range.aux));

  auto bad_range_challenge = range.zk;
  bad_range_challenge.e.set_bit(0, !bad_range_challenge.e.get_bit(0));
  EXPECT_ER(bad_range_challenge.verify(range.q, range.c, range.sid, range.aux));

  auto bad_range_commitment = range.zk;
  bad_range_commitment.c_tilde[0] = 0;
  EXPECT_ER(bad_range_commitment.verify(range.q, range.c, range.sid, range.aux));

  test_i3rzk_range_pedersen interactive_range;
  interactive_range.setup();
  interactive_range.p1();
  interactive_range.v2();
  interactive_range.p3();
  ASSERT_OK(interactive_range.verify());

  test_i3rzk_range_pedersen bad_interactive_range;
  bad_interactive_range.setup();
  bad_interactive_range.p1();
  bad_interactive_range.v2();
  bad_interactive_range.p3();
  bad_interactive_range.zk->d[0] = bad_interactive_range.p_tag;
  EXPECT_ER(bad_interactive_range.verify());

  test_nizk_paillier_pedersen_equal equal;
  equal.setup();
  equal.prove();
  ASSERT_OK(equal.verify());

  EXPECT_ER(equal.zk.verify(equal.v_p, 0, equal.q, equal.Com, equal.sid, equal.aux));

  auto bad_equal_response = equal.zk;
  bad_equal_response.D[0] = 0;
  EXPECT_ER(bad_equal_response.verify(equal.v_p, equal.c, equal.q, equal.Com, equal.sid, equal.aux));

  auto negative_challenge = equal.zk;
  negative_challenge.e = -1;
  EXPECT_ER(negative_challenge.verify(equal.v_p, equal.c, equal.q, equal.Com, equal.sid, equal.aux));

  auto negative_nonce = equal.zk;
  negative_nonce.nu = -1;
  EXPECT_ER(negative_nonce.verify(equal.v_p, equal.c, equal.q, equal.Com, equal.sid, equal.aux));

  auto bad_equal_hash = equal.zk;
  bad_equal_hash.e += 1;
  EXPECT_ER(bad_equal_hash.verify(equal.v_p, equal.c, equal.q, equal.Com, equal.sid, equal.aux));

  auto out_of_range_equal_di = equal.zk;
  out_of_range_equal_di.di[0] = equal.q << (coinbase::zk::paillier_pedersen_equal_t::param::log_alpha + SEC_P_STAT);
  EXPECT_ER(out_of_range_equal_di.verify(equal.v_p, equal.c, equal.q, equal.Com, equal.sid, equal.aux));

  auto bad_equal_opening = equal.zk;
  bad_equal_opening.nu += 1;
  EXPECT_ER(bad_equal_opening.verify(equal.v_p, equal.c, equal.q, equal.Com, equal.sid, equal.aux));

  test_i3rzk_paillier_pedersen_equal interactive_equal;
  interactive_equal.setup();
  interactive_equal.p1();
  interactive_equal.v2();
  interactive_equal.p3();
  ASSERT_OK(interactive_equal.verify());

  interactive_equal.zk.di[0] += 1;
  EXPECT_ER(interactive_equal.verify());

  interactive_equal.zk.e = -1;
  EXPECT_ER(interactive_equal.verify());

  test_nizk_paillier_range_exp_slack range_exp_slack;
  range_exp_slack.setup();
  range_exp_slack.prove();
  ASSERT_OK(range_exp_slack.verify());

  auto bad_range_exp_slack = range_exp_slack.zk;
  bad_range_exp_slack.zk_range_pedersen.d[0] = range_exp_slack.q << SEC_P_STAT;
  EXPECT_ER(bad_range_exp_slack.verify(range_exp_slack.v_p, range_exp_slack.q, range_exp_slack.c, range_exp_slack.sid,
                                       range_exp_slack.aux));
}

TEST(ZKVerifierRejection, UnknownOrderDLRejectsMalformedStatementsAndTranscripts) {
  dylog_disable_scope_t no_log_err;
  test_unknown_order_dl proof;
  proof.setup();
  proof.prove();
  ASSERT_OK(proof.verify());

  auto missing_challenge = proof.zk;
  missing_challenge.e = coinbase::buf_t();
  EXPECT_ER(missing_challenge.verify(proof.a, proof.b, proof.N, proof.l, proof.sid, proof.aux));

  EXPECT_ER(proof.zk.verify(proof.a, 0, proof.N, proof.l, proof.sid, proof.aux));
  EXPECT_ER(proof.zk.verify(proof.a, proof.pai.get_p(), proof.N, proof.l, proof.sid, proof.aux));
  EXPECT_ER(proof.zk.verify(proof.pai.get_p(), proof.b, proof.N, proof.l, proof.sid, proof.aux));
  EXPECT_ER(proof.zk.verify(proof.a, proof.b, proof.N, 0, proof.sid, proof.aux));

  auto negative_response = proof.zk;
  negative_response.z[0] = -1;
  EXPECT_ER(negative_response.verify(proof.a, proof.b, proof.N, proof.l, proof.sid, proof.aux));

  auto too_large_response = proof.zk;
  too_large_response.z[0] = bn_t(1) << (proof.l + SEC_P_STAT + 3);
  EXPECT_ER(too_large_response.verify(proof.a, proof.b, proof.N, proof.l, proof.sid, proof.aux));

  auto bad_response = proof.zk;
  bad_response.z[0] += 1;
  EXPECT_ER(bad_response.verify(proof.a, proof.b, proof.N, proof.l, proof.sid, proof.aux));
}

TEST(ZKSerialization, NonInteractiveProofsRoundTripAndVerify) {
  test_nizk_valid_paillier valid_paillier;
  valid_paillier.setup();
  valid_paillier.prove();
  auto valid_paillier_proof = RoundTrip(valid_paillier.zk);
  ASSERT_OK(valid_paillier_proof.verify(valid_paillier.v_p, valid_paillier.sid, valid_paillier.aux));

  test_nizk_paillier_zero zero;
  zero.setup();
  zero.prove();
  auto zero_proof = RoundTrip(zero.zk);
  zero_proof.paillier_valid_key = coinbase::zk::zk_flag::verified;
  ASSERT_OK(zero_proof.verify(zero.v_p, zero.c, zero.sid, zero.aux));

  test_nizk_two_paillier_equal equal;
  equal.setup();
  equal.prove();
  auto equal_proof = RoundTrip(equal.zk);
  equal_proof.p0_valid_key = coinbase::zk::zk_flag::verified;
  equal_proof.p1_valid_key = coinbase::zk::zk_flag::verified;
  equal_proof.c0_plaintext_range = coinbase::zk::zk_flag::verified;
  ASSERT_OK(equal_proof.verify(equal.q, equal.v_p1, equal.c1, equal.v_p2, equal.c2, equal.sid, equal.aux));

  test_unknown_order_dl unknown_order;
  unknown_order.setup();
  unknown_order.prove();
  auto unknown_order_proof = RoundTrip(unknown_order.zk);
  ASSERT_OK(unknown_order_proof.verify(unknown_order.a, unknown_order.b, unknown_order.N, unknown_order.l,
                                       unknown_order.sid, unknown_order.aux));
}

TEST(UCZKBatchDLHelpers, ConstAccessPreservesSignedOffsets) {
  coinbase::zk::uc_batch_dl_t::matrix_sum_t matrix(5);
  matrix[-2][0] = bn_t(11);
  matrix[0][2] = bn_t(13);
  matrix[2][5] = bn_t(17);

  const auto& const_matrix = matrix;
  EXPECT_EQ(const_matrix[-2][0], bn_t(11));
  EXPECT_EQ(const_matrix[0][2], bn_t(13));
  EXPECT_EQ(const_matrix[2][5], bn_t(17));

  coinbase::zk::uc_batch_dl_t::vector_sum_t vector(5, 3);
  vector[-2] = bn_t(19);
  vector[0] = bn_t(23);
  vector[2] = bn_t(29);

  const auto& const_vector = vector;
  EXPECT_EQ(const_vector[-2], bn_t(19));
  EXPECT_EQ(const_vector[0], bn_t(23));
  EXPECT_EQ(const_vector[2], bn_t(29));
}

TEST(FischlinParams, RejectsB31ToPreventUB) {
  coinbase::zk::fischlin_params_t p{128, 31, 4};
  EXPECT_NE(p.check(), SUCCESS);
}

TEST(FischlinParams, RejectsNonPositiveRho) {
  coinbase::zk::fischlin_params_t p{0, 16, 4};
  EXPECT_NE(p.check(), SUCCESS);
}

TEST(FischlinParams, RejectsNonPositiveB) {
  coinbase::zk::fischlin_params_t p{128, 0, 4};
  EXPECT_NE(p.check(), SUCCESS);
}

TEST(FischlinParams, RejectsInsufficientEntropy) {
  coinbase::zk::fischlin_params_t p{16, 4, 4};
  EXPECT_NE(p.check(), SUCCESS);
}

TEST(FischlinParams, RejectsB32) {
  coinbase::zk::fischlin_params_t p{128, 32, 4};
  EXPECT_NE(p.check(), SUCCESS);
}

TEST(FischlinParams, AcceptsValidParams) {
  coinbase::zk::fischlin_params_t p{128, 16, 4};
  EXPECT_EQ(p.check(), SUCCESS);
}

TEST(FischlinParams, AcceptsB30) {
  coinbase::zk::fischlin_params_t p{128, 30, 4};
  EXPECT_EQ(p.check(), SUCCESS);
}

TEST(FischlinParams, AcceptsT30) {
  coinbase::zk::fischlin_params_t p{128, 16, 30};
  EXPECT_EQ(p.check(), SUCCESS);
}

TEST(FischlinParams, CheckWithEffectiveBRejectsInvalidInputs) {
  EXPECT_NE((coinbase::zk::fischlin_params_t{0, 16, 4}).check_with_effective_b(16), SUCCESS);
  EXPECT_NE((coinbase::zk::fischlin_params_t{128, 0, 4}).check_with_effective_b(16), SUCCESS);
  EXPECT_NE((coinbase::zk::fischlin_params_t{128, 31, 4}).check_with_effective_b(16), SUCCESS);
  EXPECT_NE((coinbase::zk::fischlin_params_t{128, 16, 4}).check_with_effective_b(0), SUCCESS);
  EXPECT_NE((coinbase::zk::fischlin_params_t{16, 16, 4}).check_with_effective_b(7), SUCCESS);
}

TEST(FischlinParams, BMaskWorksCorrectly) {
  coinbase::zk::fischlin_params_t p{128, 8, 4};
  EXPECT_EQ(p.b_mask(), 0xFFu);
}

TEST(FischlinParams, EMaxWorksCorrectly) {
  coinbase::zk::fischlin_params_t p{128, 16, 4};
  EXPECT_EQ(p.e_max(), 16);
}

TEST(SmallPrimes, RejectsValueWithSmallPrimeFactor) {
  coinbase::crypto::vartime_scope_t vartime_scope;
  EXPECT_NE(coinbase::zk::check_integer_with_small_primes(bn_t(21), 32), SUCCESS);
}

TEST(SmallPrimes, AcceptsPrimeAfterScanningAllConfiguredPrimes) {
  coinbase::crypto::vartime_scope_t vartime_scope;
  const bn_t prime = bn_t::generate_prime(20, false, nullptr, nullptr);
  EXPECT_EQ(coinbase::zk::check_integer_with_small_primes(prime, std::numeric_limits<int>::max()), SUCCESS);
}

TEST(SmallPrimes, StopsAtAlphaLimit) {
  coinbase::crypto::vartime_scope_t vartime_scope;
  EXPECT_EQ(coinbase::zk::check_integer_with_small_primes(bn_t(101), 32), SUCCESS);
}

}  // namespace
