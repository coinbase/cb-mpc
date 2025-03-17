#include <gtest/gtest.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_pki.h>
#include <cbmpc/crypto/commitment.h>

#include "utils/test_macros.h"

namespace {

using namespace coinbase::crypto;

TEST(CryptoCommitment, AdditionalInputSid) {
  buf_t sid = gen_random_bitlen(SEC_P_COM);
  mpc_pid_t pid = pid_from_name("test");
  commitment_t com1(sid, pid);
  commitment_t com2(sid, pid);
  bn_t a = bn_t::rand_bitlen(256);
  bn_t b = bn_t::rand_bitlen(256);
  ASSERT_CB_ASSERT(com1.open(a), "msg.size() == HASH_SIZE");
  ASSERT_CB_ASSERT(com2.open(a), "msg.size() == HASH_SIZE");

  com1.gen(a);
  EXPECT_OK(com1.open(a));
  EXPECT_ER(com1.open(b));                                    // Wrong opening
  ASSERT_CB_ASSERT(com2.open(a), "msg.size() == HASH_SIZE");  // no commitment
}

TEST(CryptoCommitment, LocalSid) {
  mpc_pid_t pid = pid_from_name("test");
  commitment_t com1(pid);
  commitment_t com2(pid);
  bn_t a = bn_t::rand_bitlen(256);
  bn_t b = bn_t::rand_bitlen(256);
  ASSERT_CB_ASSERT(com1.open(a), "msg.size() == HASH_SIZE + LOCAL_SID_SIZE");
  ASSERT_CB_ASSERT(com2.open(a), "msg.size() == HASH_SIZE + LOCAL_SID_SIZE");

  com1.gen(a);
  EXPECT_OK(com1.open(a));
  EXPECT_ER(com1.open(b));  // Wrong opening
  ASSERT_CB_ASSERT(com2.open(a), "msg.size() == HASH_SIZE + LOCAL_SID_SIZE");
}

TEST(CryptoCommitment, LocalSidAndReceiverPid) {
  mpc_pid_t pid = pid_from_name("test");
  mpc_pid_t receiver_pid = pid_from_name("test2");
  commitment_t com1(pid, receiver_pid);
  commitment_t com2(pid, receiver_pid);
  commitment_t com3(pid, pid_from_name("test3"));
  bn_t a = bn_t::rand_bitlen(256);
  bn_t b = bn_t::rand_bitlen(256);
  ASSERT_CB_ASSERT(com1.open(a), "msg.size() == HASH_SIZE + LOCAL_SID_SIZE");
  ASSERT_CB_ASSERT(com2.open(a), "msg.size() == HASH_SIZE + LOCAL_SID_SIZE");
  ASSERT_CB_ASSERT(com3.open(a), "msg.size() == HASH_SIZE + LOCAL_SID_SIZE");

  com1.gen(a);
  com2.set(com1.rand, com1.msg);
  com3.set(com1.rand, com1.msg);
  EXPECT_OK(com1.open(a));
  EXPECT_OK(com2.open(a));
  EXPECT_ER(com3.open(a));  // incorrect receiver pid
}

TEST(CryptoCommitment, AdditionalInputSid_AlternativeFormat) {
  buf_t sid = gen_random_bitlen(SEC_P_COM);
  mpc_pid_t pid = pid_from_name("test");
  commitment_t com1;
  commitment_t com2;
  com1.id(sid, pid);
  com2.id(sid, pid);
  bn_t a = bn_t::rand_bitlen(256);
  bn_t b = bn_t::rand_bitlen(256);
  ASSERT_CB_ASSERT(com1.open(a), "msg.size() == HASH_SIZE");
  ASSERT_CB_ASSERT(com2.open(a), "msg.size() == HASH_SIZE");

  com1.gen(a);

  commitment_t com1_alt;
  commitment_t com2_alt;
  com1_alt.id(sid, pid);
  com2_alt.id(sid, pid);
  com1_alt.set(com1.rand, com1.msg);
  com2_alt.set(com2.rand, com2.msg);
  EXPECT_OK(com1_alt.open(a));
  EXPECT_ER(com1_alt.open(b));                                    // Wrong opening
  ASSERT_CB_ASSERT(com2_alt.open(a), "msg.size() == HASH_SIZE");  // No commitment
}

TEST(CryptoCommitment, LocalSid_AlternativeFormat) {
  mpc_pid_t pid = pid_from_name("test");
  commitment_t com1;
  commitment_t com2;
  com1.id(pid);
  com2.id(pid);
  bn_t a = bn_t::rand_bitlen(256);
  bn_t b = bn_t::rand_bitlen(256);
  ASSERT_CB_ASSERT(com1.open(a), "msg.size() == HASH_SIZE");
  ASSERT_CB_ASSERT(com2.open(a), "msg.size() == HASH_SIZE");

  com1.gen(a);

  commitment_t com1_alt;
  commitment_t com2_alt;
  com1_alt.id(pid);
  com2_alt.id(pid);
  com1_alt.set(com1.rand, com1.msg);
  com2_alt.set(com2.rand, com2.msg);
  EXPECT_OK(com1_alt.open(a));
  EXPECT_ER(com1_alt.open(b));                                    // Wrong opening
  ASSERT_CB_ASSERT(com2_alt.open(a), "msg.size() == HASH_SIZE");  // No commitment
}

}  // namespace