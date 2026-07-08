#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_pki.h>
#include <cbmpc/internal/crypto/commitment.h>

#include "utils/test_macros.h"

namespace {

using namespace coinbase::crypto;
using coinbase::buf128_t;
using coinbase::buf256_t;
using coinbase::buf_t;

TEST(CryptoCommitment, AdditionalInputSid) {
  buf_t sid = gen_random_bitlen(SEC_P_COM);
  mpc_pid_t pid = pid_from_name("test");
  commitment_t com1(sid, pid);
  commitment_t com2(sid, pid);
  bn_t a = bn_t::rand_bitlen(256);
  bn_t b = bn_t::rand_bitlen(256);
  EXPECT_ER(com1.open(a));
  EXPECT_ER(com2.open(a));

  com1.gen(a);
  EXPECT_OK(com1.open(a));
  EXPECT_ER(com1.open(b));  // Wrong opening
  EXPECT_ER(com2.open(a));  // no commitment
}

TEST(CryptoCommitment, LocalSid) {
  mpc_pid_t pid = pid_from_name("test");
  commitment_t com1(pid);
  commitment_t com2(pid);
  bn_t a = bn_t::rand_bitlen(256);
  bn_t b = bn_t::rand_bitlen(256);
  EXPECT_ER(com1.open(a));
  EXPECT_ER(com2.open(a));

  com1.gen(a);
  EXPECT_OK(com1.open(a));
  EXPECT_ER(com1.open(b));  // Wrong opening
  EXPECT_ER(com2.open(a));
}

TEST(CryptoCommitment, LocalSidAndReceiverPid) {
  mpc_pid_t pid = pid_from_name("test");
  mpc_pid_t receiver_pid = pid_from_name("test2");
  commitment_t com1(pid, receiver_pid);
  commitment_t com2(pid, receiver_pid);
  commitment_t com3(pid, pid_from_name("test3"));
  bn_t a = bn_t::rand_bitlen(256);
  bn_t b = bn_t::rand_bitlen(256);
  EXPECT_ER(com1.open(a));
  EXPECT_ER(com2.open(a));
  EXPECT_ER(com3.open(a));

  com1.gen(a);
  com2.set(com1.rand, com1.msg);
  com3.set(com1.rand, com1.msg);
  EXPECT_OK(com1.open(a));
  EXPECT_OK(com2.open(a));
  EXPECT_ER(com3.open(a));  // incorrect receiver pid
}

TEST(CryptoCommitment, ExternalSidAndReceiverPid) {
  buf_t sid = gen_random_bitlen(SEC_P_COM);
  mpc_pid_t pid = pid_from_name("test");
  mpc_pid_t receiver_pid = pid_from_name("test2");
  mpc_pid_t other_receiver_pid = pid_from_name("test3");
  bn_t a = bn_t::rand_bitlen(256);

  commitment_t com(sid, pid, receiver_pid);
  com.gen(a);

  commitment_t receiver(sid, pid, receiver_pid);
  receiver.set(com.rand, com.msg);
  EXPECT_OK(receiver.open(a));

  commitment_t receiver_from_id;
  receiver_from_id.id(sid, pid, receiver_pid).set(com.rand, com.msg);
  EXPECT_OK(receiver_from_id.open(a));

  commitment_t wrong_receiver(sid, pid, other_receiver_pid);
  wrong_receiver.set(com.rand, com.msg);
  EXPECT_ER(wrong_receiver.open(a));
}

TEST(CryptoCommitment, ExternalSidSetRandIsDeterministic) {
  buf_t sid = gen_random_bitlen(SEC_P_COM);
  mpc_pid_t pid = pid_from_name("test");
  mpc_pid_t receiver_pid = pid_from_name("test2");
  const bn_t secret = bn_t::rand_bitlen(256);
  const buf256_t fixed_rand = buf256_t::make(buf128_t::make(1, 2), buf128_t::make(3, 4));

  commitment_t com1(sid, pid);
  commitment_t com2(sid, pid);
  com1.rand = fixed_rand;
  com2.rand = fixed_rand;
  com1.gen_with_set_rand(secret);
  com2.gen_with_set_rand(secret);

  EXPECT_EQ(fixed_rand, com1.rand);
  EXPECT_EQ(fixed_rand, com2.rand);
  EXPECT_EQ(com1.msg, com2.msg);

  commitment_t receiver_bound(sid, pid, receiver_pid);
  receiver_bound.rand = fixed_rand;
  receiver_bound.gen_with_set_rand(secret);
  EXPECT_NE(com1.msg, receiver_bound.msg);
}

TEST(CryptoCommitment, AdditionalSid_AltFormat) {
  buf_t sid = gen_random_bitlen(SEC_P_COM);
  mpc_pid_t pid = pid_from_name("test");
  commitment_t com1;
  commitment_t com2;
  com1.id(sid, pid);
  com2.id(sid, pid);
  bn_t a = bn_t::rand_bitlen(256);
  bn_t b = bn_t::rand_bitlen(256);
  EXPECT_ER(com1.open(a));
  EXPECT_ER(com2.open(a));

  com1.gen(a);

  commitment_t com1_alt;
  commitment_t com2_alt;
  com1_alt.id(sid, pid);
  com2_alt.id(sid, pid);
  com1_alt.set(com1.rand, com1.msg);
  com2_alt.set(com2.rand, com2.msg);
  EXPECT_OK(com1_alt.open(a));
  EXPECT_ER(com1_alt.open(b));  // Wrong opening
  EXPECT_ER(com2_alt.open(a));  // No commitment
}

TEST(CryptoCommitment, LocalSid_AlternativeFormat) {
  mpc_pid_t pid = pid_from_name("test");
  commitment_t com1;
  commitment_t com2;
  com1.id(pid);
  com2.id(pid);
  bn_t a = bn_t::rand_bitlen(256);
  bn_t b = bn_t::rand_bitlen(256);
  EXPECT_ER(com1.open(a));
  EXPECT_ER(com2.open(a));

  com1.gen(a);

  commitment_t com1_alt;
  commitment_t com2_alt;
  com1_alt.id(pid);
  com2_alt.id(pid);
  com1_alt.set(com1.rand, com1.msg);
  com2_alt.set(com2.rand, com2.msg);
  EXPECT_OK(com1_alt.open(a));
  EXPECT_ER(com1_alt.open(b));  // Wrong opening
  EXPECT_ER(com2_alt.open(a));  // No commitment
}

TEST(CryptoCommitment, TamperedCommitmentMessageRejectsOpen) {
  mpc_pid_t pid = pid_from_name("test");
  commitment_t com(pid);
  const bn_t secret = bn_t::rand_bitlen(256);
  com.gen(secret);

  commitment_t receiver(pid);
  receiver.set(com.rand, com.msg);
  buf_t tampered = receiver.msg;
  tampered[0] ^= 0x01;
  receiver.msg = tampered;
  EXPECT_ER(receiver.open(secret));
}

TEST(CryptoCommitment, MalformedCommitmentMessageSizeRejectsOpen) {
  const bn_t secret = bn_t::rand_bitlen(256);
  mpc_pid_t pid = pid_from_name("test");
  buf_t sid = gen_random_bitlen(SEC_P_COM);

  commitment_t external(sid, pid);
  external.gen(secret);
  commitment_t truncated_external(sid, pid);
  truncated_external.set(external.rand, buf_t(external.msg.take(commitment_t::HASH_SIZE - 1)));
  EXPECT_ER(truncated_external.open(secret));

  commitment_t oversized_external(sid, pid);
  oversized_external.set(external.rand, external.msg + gen_random(1));
  EXPECT_ER(oversized_external.open(secret));

  commitment_t local(pid);
  local.gen(secret);
  commitment_t truncated_local(pid);
  truncated_local.set(local.rand, buf_t(local.msg.take(commitment_t::HASH_SIZE + commitment_t::LOCAL_SID_SIZE - 1)));
  EXPECT_ER(truncated_local.open(secret));

  commitment_t oversized_local(pid);
  oversized_local.set(local.rand, local.msg + gen_random(1));
  EXPECT_ER(oversized_local.open(secret));
}

}  // namespace
