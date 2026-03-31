#include <gtest/gtest.h>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/crypto/base.h>

using namespace coinbase;
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

TEST(Paillier, CreatePubRejectsOversizedModulus) {
  paillier_t paillier;
  bn_t oversized = (bn_t(1) << paillier_t::bit_size) + 1;

  EXPECT_EQ(paillier.create_pub(oversized), E_BADARG);
}

}  // namespace
