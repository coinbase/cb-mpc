#include <gtest/gtest.h>

#include <cbmpc/zk/zk_ec.h>

#include "utils/data/zk_completeness.h"
#include "utils/test_macros.h"

#define REPEAT_COMPLETENESS 1

using namespace coinbase::test::data;

namespace {
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
      }                                                   \
      delete zk;                                          \
    }                                                     \
  }

#define TEST_NIZK_COMPLETENESS(name, test_nizk_obj) \
  TEST(name, Completeness) {                        \
    auto zk = test_nizk_obj;                        \
    for (int i = 0; i < REPEAT_COMPLETENESS; i++) { \
      zk->setup();                                  \
      zk->prove();                                  \
      ASSERT_OK(zk->verify());                      \
    }                                               \
  }

#define TEST_2RZK_COMPLETENESS(name, test_nizk_obj) \
  TEST(name, Completeness) {                        \
    auto zk = test_nizk_obj;                        \
    for (int i = 0; i < REPEAT_COMPLETENESS; i++) { \
      zk->setup();                                  \
      zk->v1();                                     \
      zk->p2();                                     \
      ASSERT_OK(zk->verify());                      \
    }                                               \
  }

#define TEST_3RZK_COMPLETENESS(name, test_nizk_obj) \
  TEST(name, Completeness) {                        \
    auto zk = test_nizk_obj;                        \
    for (int i = 0; i < REPEAT_COMPLETENESS; i++) { \
      zk->setup();                                  \
      zk->p1();                                     \
      zk->v2();                                     \
      zk->p3();                                     \
      ASSERT_OK(zk->verify());                      \
    }                                               \
  }

TEST_NIZK_COMPLETENESS_CURVES(UC_ZK_DL, test_niuc_dl_t);
TEST_NIZK_COMPLETENESS_CURVES(UC_ZK_BatchDL, test_niuc_batch_dl_t, 10);
TEST_NIZK_COMPLETENESS_CURVES(ZK_DH, test_nidh_t);
TEST_NIZK_COMPLETENESS_CURVES(UC_ZK_ElGamalCom, test_nizk_uc_elgamal_com);
TEST_NIZK_COMPLETENESS_CURVES(ZK_ElGamalComPubShareEqual, test_nizk_elgamal_com_pub_share_equ);
TEST_NIZK_COMPLETENESS_CURVES(ZK_ElGamalComMult, test_nizk_elgamal_com_mult);
TEST_NIZK_COMPLETENESS_CURVES(ZK_ElGamalComMultPrivateScalar, test_nizk_elgamal_com_mult_private_scalar);
TEST_NIZK_COMPLETENESS(ZK_ValidPaillier, new test_nizk_valid_paillier());
TEST_2RZK_COMPLETENESS(ZK_ValidPaillier_Interactive, new test_2rzk_valid_paillier());
TEST_NIZK_COMPLETENESS(ZK_PaillierZero, new test_nizk_paillier_zero());
TEST_3RZK_COMPLETENESS(ZK_PaillierZeroInteractive, new test_3rzk_paillier_zero());
TEST_NIZK_COMPLETENESS(ZK_TwoPaillierEqual, new test_nizk_two_paillier_equal());
TEST_3RZK_COMPLETENESS(ZK_TwoPaillierEqualInteractive, new test_3rzk_paillier_zero());
TEST_NIZK_COMPLETENESS(ZK_RangePedersen, new test_nizk_range_pedersen());
TEST_3RZK_COMPLETENESS(ZK_RangePedersenInteractiveOpt, new test_i3rzk_range_pedersen());
TEST_NIZK_COMPLETENESS(ZK_PaillierPedersenEqual, new test_nizk_paillier_pedersen_equal());
TEST_3RZK_COMPLETENESS(ZK_PaillierPedersenEqualInteractive, new test_i3rzk_paillier_pedersen_equal());
TEST_NIZK_COMPLETENESS(ZK_PaillierRangeExpSlack, new test_nizk_paillier_range_exp_slack());
TEST_NIZK_COMPLETENESS_CURVES(ZK_PDL, test_nizk_pdl);
TEST_NIZK_COMPLETENESS(ZK_UnknownOrderDL, new test_unknown_order_dl());

}  // namespace