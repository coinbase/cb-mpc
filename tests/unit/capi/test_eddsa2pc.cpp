#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/capi/eddsa_2p.h>
#include <cbmpc/capi/job.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/crypto/base_ecc.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;

using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::capi_harness::make_transport;
using coinbase::testutils::capi_harness::run_2pc;
using coinbase::testutils::capi_harness::transport_ctx_t;

static void expect_eq(cmem_t a, cmem_t b) {
  ASSERT_EQ(a.size, b.size);
  if (a.size > 0) {
    ASSERT_NE(a.data, nullptr);
    ASSERT_NE(b.data, nullptr);
    ASSERT_EQ(std::memcmp(a.data, b.data, static_cast<size_t>(a.size)), 0);
  }
}

}  // namespace

TEST(CApiEdDSA2pc, DkgSignRefreshSign) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  std::atomic<int> free_calls_1{0};
  std::atomic<int> free_calls_2{0};
  transport_ctx_t ctx1{c1, &free_calls_1};
  transport_ctx_t ctx2{c2, &free_calls_2};

  const cbmpc_transport_t t1 = make_transport(&ctx1);
  const cbmpc_transport_t t2 = make_transport(&ctx2);

  cmem_t key_blob_1{nullptr, 0};
  cmem_t key_blob_2{nullptr, 0};
  cbmpc_error_t rv1 = UNINITIALIZED_ERROR;
  cbmpc_error_t rv2 = UNINITIALIZED_ERROR;

  const cbmpc_2pc_job_t job1 = {CBMPC_2PC_P1, "p1", "p2", &t1};
  const cbmpc_2pc_job_t job2 = {CBMPC_2PC_P2, "p1", "p2", &t2};
  run_2pc(
      c1, c2, [&] { return cbmpc_eddsa_2p_dkg(&job1, CBMPC_CURVE_ED25519, &key_blob_1); },
      [&] { return cbmpc_eddsa_2p_dkg(&job2, CBMPC_CURVE_ED25519, &key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, CBMPC_SUCCESS);
  ASSERT_EQ(rv2, CBMPC_SUCCESS);
  ASSERT_GT(key_blob_1.size, 0);
  ASSERT_GT(key_blob_2.size, 0);

  cmem_t pub1{nullptr, 0};
  cmem_t pub2{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_2p_get_public_key_compressed(key_blob_1, &pub1), CBMPC_SUCCESS);
  ASSERT_EQ(cbmpc_eddsa_2p_get_public_key_compressed(key_blob_2, &pub2), CBMPC_SUCCESS);
  expect_eq(pub1, pub2);
  ASSERT_EQ(pub1.size, 32);

  const buf_t pub_buf(pub1.data, pub1.size);
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_ed25519, pub_buf), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  uint8_t msg_bytes[32];
  for (int i = 0; i < 32; i++) msg_bytes[i] = static_cast<uint8_t>(i);
  const cmem_t msg = {msg_bytes, 32};

  cmem_t sig1{nullptr, 0};
  cmem_t sig2{nullptr, 0};
  run_2pc(
      c1, c2, [&] { return cbmpc_eddsa_2p_sign(&job1, key_blob_1, msg, &sig1); },
      [&] { return cbmpc_eddsa_2p_sign(&job2, key_blob_2, msg, &sig2); }, rv1, rv2);
  ASSERT_EQ(rv1, CBMPC_SUCCESS);
  ASSERT_EQ(rv2, CBMPC_SUCCESS);
  ASSERT_EQ(sig1.size, 64);
  ASSERT_EQ(sig2.size, 0);
  ASSERT_EQ(verify_key.verify(buf_t(msg_bytes, 32), buf_t(sig1.data, sig1.size)), SUCCESS);

  cmem_t new_key_blob_1{nullptr, 0};
  cmem_t new_key_blob_2{nullptr, 0};
  run_2pc(
      c1, c2, [&] { return cbmpc_eddsa_2p_refresh(&job1, key_blob_1, &new_key_blob_1); },
      [&] { return cbmpc_eddsa_2p_refresh(&job2, key_blob_2, &new_key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, CBMPC_SUCCESS);
  ASSERT_EQ(rv2, CBMPC_SUCCESS);
  ASSERT_GT(new_key_blob_1.size, 0);
  ASSERT_GT(new_key_blob_2.size, 0);

  cmem_t pub3{nullptr, 0};
  cmem_t pub4{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_2p_get_public_key_compressed(new_key_blob_1, &pub3), CBMPC_SUCCESS);
  ASSERT_EQ(cbmpc_eddsa_2p_get_public_key_compressed(new_key_blob_2, &pub4), CBMPC_SUCCESS);
  expect_eq(pub3, pub4);
  expect_eq(pub1, pub3);

  cmem_t sig3{nullptr, 0};
  cmem_t sig4{nullptr, 0};
  run_2pc(
      c1, c2, [&] { return cbmpc_eddsa_2p_sign(&job1, new_key_blob_1, msg, &sig3); },
      [&] { return cbmpc_eddsa_2p_sign(&job2, new_key_blob_2, msg, &sig4); }, rv1, rv2);
  ASSERT_EQ(rv1, CBMPC_SUCCESS);
  ASSERT_EQ(rv2, CBMPC_SUCCESS);
  ASSERT_EQ(sig3.size, 64);
  ASSERT_EQ(sig4.size, 0);
  ASSERT_EQ(verify_key.verify(buf_t(msg_bytes, 32), buf_t(sig3.data, sig3.size)), SUCCESS);

  EXPECT_GT(free_calls_1.load(), 0);
  EXPECT_GT(free_calls_2.load(), 0);

  cbmpc_cmem_free(pub1);
  cbmpc_cmem_free(pub2);
  cbmpc_cmem_free(pub3);
  cbmpc_cmem_free(pub4);
  cbmpc_cmem_free(sig1);
  cbmpc_cmem_free(sig2);
  cbmpc_cmem_free(sig3);
  cbmpc_cmem_free(sig4);
  cbmpc_cmem_free(key_blob_1);
  cbmpc_cmem_free(key_blob_2);
  cbmpc_cmem_free(new_key_blob_1);
  cbmpc_cmem_free(new_key_blob_2);
}

TEST(CApiEdDSA2pc, ValidatesArgs) {
  cmem_t out{reinterpret_cast<uint8_t*>(0x1), 123};
  const cbmpc_2pc_job_t bad_job = {CBMPC_2PC_P1, "p1", "p2", nullptr};
  EXPECT_EQ(cbmpc_eddsa_2p_dkg(&bad_job, CBMPC_CURVE_ED25519, &out), E_BADARG);
  EXPECT_EQ(out.data, nullptr);
  EXPECT_EQ(out.size, 0);

  // Missing sig_out is invalid.
  EXPECT_EQ(cbmpc_eddsa_2p_sign(nullptr, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, nullptr), E_BADARG);
}
