#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/eddsa_2p.h>
#include <cbmpc/internal/crypto/base_ecc.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;

using coinbase::api::curve_id;
using coinbase::api::eddsa_2p::party_t;

using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::api_harness::failing_transport_t;
using coinbase::testutils::api_harness::local_api_transport_t;
using coinbase::testutils::api_harness::run_2pc;

static void exercise_ed25519() {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  local_api_transport_t t1(c1);
  local_api_transport_t t2(c2);

  buf_t key_blob_1;
  buf_t key_blob_2;
  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;

  const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};

  run_2pc(
      c1, c2, [&] { return coinbase::api::eddsa_2p::dkg(job1, curve_id::ed25519, key_blob_1); },
      [&] { return coinbase::api::eddsa_2p::dkg(job2, curve_id::ed25519, key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t pub1;
  buf_t pub2;
  ASSERT_EQ(coinbase::api::eddsa_2p::get_public_key_compressed(key_blob_1, pub1), SUCCESS);
  ASSERT_EQ(coinbase::api::eddsa_2p::get_public_key_compressed(key_blob_2, pub2), SUCCESS);
  EXPECT_EQ(pub1.size(), 32);
  EXPECT_EQ(pub1, pub2);

  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_ed25519, pub1), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  // Deterministic 32-byte message for testing.
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);

  buf_t sig1;
  buf_t sig2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::eddsa_2p::sign(job1, key_blob_1, msg, sig1); },
      [&] { return coinbase::api::eddsa_2p::sign(job2, key_blob_2, msg, sig2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(sig1.size(), 64);
  EXPECT_EQ(sig2.size(), 0);
  ASSERT_EQ(verify_key.verify(msg, sig1), SUCCESS);

  // Refresh and sign again.
  buf_t new_key_blob_1;
  buf_t new_key_blob_2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::eddsa_2p::refresh(job1, key_blob_1, new_key_blob_1); },
      [&] { return coinbase::api::eddsa_2p::refresh(job2, key_blob_2, new_key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t sig3;
  buf_t sig4;
  run_2pc(
      c1, c2, [&] { return coinbase::api::eddsa_2p::sign(job1, new_key_blob_1, msg, sig3); },
      [&] { return coinbase::api::eddsa_2p::sign(job2, new_key_blob_2, msg, sig4); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(sig3.size(), 64);
  EXPECT_EQ(sig4.size(), 0);
  ASSERT_EQ(verify_key.verify(msg, sig3), SUCCESS);

  buf_t pub3;
  buf_t pub4;
  ASSERT_EQ(coinbase::api::eddsa_2p::get_public_key_compressed(new_key_blob_1, pub3), SUCCESS);
  ASSERT_EQ(coinbase::api::eddsa_2p::get_public_key_compressed(new_key_blob_2, pub4), SUCCESS);
  EXPECT_EQ(pub3, pub4);
  EXPECT_EQ(pub3, pub1);

  // Role is fixed to the share: signing with the "wrong" job.self should fail.
  buf_t bad_sig1;
  buf_t bad_sig2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::eddsa_2p::sign(job1, key_blob_2, msg, bad_sig1); },
      [&] { return coinbase::api::eddsa_2p::sign(job2, key_blob_2, msg, bad_sig2); }, rv1, rv2);
  EXPECT_EQ(rv1, E_BADARG);
}

}  // namespace

TEST(ApiEdDSA2pc, DkgSignRefreshSign) { exercise_ed25519(); }

TEST(ApiEdDSA2pc, UnsupportedCurveRejected) {
  failing_transport_t t;
  buf_t key_blob;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  EXPECT_EQ(coinbase::api::eddsa_2p::dkg(job, curve_id::secp256k1, key_blob), E_BADARG);
}

TEST(ApiEdDSA2pc, KeyBlobPrivScalar_NoPubSign) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  local_api_transport_t t1(c1);
  local_api_transport_t t2(c2);

  buf_t key_blob_1;
  buf_t key_blob_2;
  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;

  const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};

  run_2pc(
      c1, c2, [&] { return coinbase::api::eddsa_2p::dkg(job1, curve_id::ed25519, key_blob_1); },
      [&] { return coinbase::api::eddsa_2p::dkg(job2, curve_id::ed25519, key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  // Refresh (exercise detach/attach on refreshed blobs too).
  buf_t refreshed_1;
  buf_t refreshed_2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::eddsa_2p::refresh(job1, key_blob_1, refreshed_1); },
      [&] { return coinbase::api::eddsa_2p::refresh(job2, key_blob_2, refreshed_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t public_1;
  buf_t public_2;
  buf_t x1_fixed;
  buf_t x2_fixed;
  ASSERT_EQ(coinbase::api::eddsa_2p::detach_private_scalar(refreshed_1, public_1, x1_fixed), SUCCESS);
  ASSERT_EQ(coinbase::api::eddsa_2p::detach_private_scalar(refreshed_2, public_2, x2_fixed), SUCCESS);
  EXPECT_EQ(x1_fixed.size(), 32);
  EXPECT_EQ(x2_fixed.size(), 32);

  buf_t Qi_full_1;
  ASSERT_EQ(coinbase::api::eddsa_2p::get_public_share_compressed(refreshed_1, Qi_full_1), SUCCESS);

  buf_t Qi_full_2;
  ASSERT_EQ(coinbase::api::eddsa_2p::get_public_share_compressed(refreshed_2, Qi_full_2), SUCCESS);

  // Public blob should not be usable for signing.
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  {
    failing_transport_t ft;
    const coinbase::api::job_2p_t bad_job{party_t::p1, "p1", "p2", ft};
    buf_t sig;
    EXPECT_NE(coinbase::api::eddsa_2p::sign(bad_job, public_1, msg, sig), SUCCESS);
  }

  buf_t merged_1;
  buf_t merged_2;
  ASSERT_EQ(coinbase::api::eddsa_2p::attach_private_scalar(public_1, x1_fixed, Qi_full_1, merged_1), SUCCESS);
  ASSERT_EQ(coinbase::api::eddsa_2p::attach_private_scalar(public_2, x2_fixed, Qi_full_2, merged_2), SUCCESS);

  // Sign again with merged blobs.
  buf_t pub;
  ASSERT_EQ(coinbase::api::eddsa_2p::get_public_key_compressed(merged_1, pub), SUCCESS);
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_ed25519, pub), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  buf_t sig1;
  buf_t sig2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::eddsa_2p::sign(job1, merged_1, msg, sig1); },
      [&] { return coinbase::api::eddsa_2p::sign(job2, merged_2, msg, sig2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  ASSERT_EQ(sig1.size(), 64);
  EXPECT_EQ(sig2.size(), 0);
  ASSERT_EQ(verify_key.verify(msg, sig1), SUCCESS);

  // Negative: wrong scalar should fail to attach.
  buf_t bad_x = x1_fixed;
  bad_x[0] ^= 0x01;
  buf_t bad_merged;
  EXPECT_NE(coinbase::api::eddsa_2p::attach_private_scalar(public_1, bad_x, Qi_full_1, bad_merged), SUCCESS);
}
