#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/schnorr_2p.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/crypto/base_ecc_secp256k1.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;

using coinbase::api::curve_id;
using coinbase::api::schnorr_2p::party_t;

using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::api_harness::failing_transport_t;
using coinbase::testutils::api_harness::local_api_transport_t;
using coinbase::testutils::api_harness::run_2pc;

struct key_blob_v1_t {
  uint32_t version = 1;
  uint32_t role = 0;
  uint32_t curve = 0;
  buf_t Q_compressed;
  coinbase::crypto::bn_t x_share;

  void convert(coinbase::converter_t& c) { c.convert(version, role, curve, Q_compressed, x_share); }
};

static void exercise_secp256k1_bip340() {
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
      c1, c2, [&] { return coinbase::api::schnorr_2p::dkg(job1, curve_id::secp256k1, key_blob_1); },
      [&] { return coinbase::api::schnorr_2p::dkg(job2, curve_id::secp256k1, key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t pub1;
  buf_t pub2;
  ASSERT_EQ(coinbase::api::schnorr_2p::get_public_key_compressed(key_blob_1, pub1), SUCCESS);
  ASSERT_EQ(coinbase::api::schnorr_2p::get_public_key_compressed(key_blob_2, pub2), SUCCESS);
  EXPECT_EQ(pub1.size(), 33);
  EXPECT_EQ(pub1, pub2);

  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_secp256k1, pub1), SUCCESS);

  buf_t pub_xonly;
  ASSERT_EQ(coinbase::api::schnorr_2p::extract_public_key_xonly(key_blob_1, pub_xonly), SUCCESS);
  EXPECT_EQ(pub_xonly.size(), 32);
  EXPECT_EQ(pub_xonly, Q.get_x().to_bin(32));

  // Deterministic 32-byte message for testing.
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);

  buf_t sig1;
  buf_t sig2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::schnorr_2p::sign(job1, key_blob_1, msg, sig1); },
      [&] { return coinbase::api::schnorr_2p::sign(job2, key_blob_2, msg, sig2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(sig1.size(), 64);
  EXPECT_EQ(sig2.size(), 0);
  ASSERT_EQ(coinbase::crypto::bip340::verify(Q, msg, sig1), SUCCESS);

  // Refresh and sign again.
  buf_t new_key_blob_1;
  buf_t new_key_blob_2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::schnorr_2p::refresh(job1, key_blob_1, new_key_blob_1); },
      [&] { return coinbase::api::schnorr_2p::refresh(job2, key_blob_2, new_key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t sig3;
  buf_t sig4;
  run_2pc(
      c1, c2, [&] { return coinbase::api::schnorr_2p::sign(job1, new_key_blob_1, msg, sig3); },
      [&] { return coinbase::api::schnorr_2p::sign(job2, new_key_blob_2, msg, sig4); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(sig3.size(), 64);
  EXPECT_EQ(sig4.size(), 0);
  ASSERT_EQ(coinbase::crypto::bip340::verify(Q, msg, sig3), SUCCESS);

  buf_t pub3;
  buf_t pub4;
  ASSERT_EQ(coinbase::api::schnorr_2p::get_public_key_compressed(new_key_blob_1, pub3), SUCCESS);
  ASSERT_EQ(coinbase::api::schnorr_2p::get_public_key_compressed(new_key_blob_2, pub4), SUCCESS);
  EXPECT_EQ(pub3, pub4);
  EXPECT_EQ(pub3, pub1);

  // Role is fixed to the share: signing with the "wrong" job.self should fail.
  buf_t bad_sig1;
  buf_t bad_sig2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::schnorr_2p::sign(job1, key_blob_2, msg, bad_sig1); },
      [&] { return coinbase::api::schnorr_2p::sign(job2, key_blob_2, msg, bad_sig2); }, rv1, rv2);
  EXPECT_EQ(rv1, E_BADARG);
}

}  // namespace

TEST(ApiSchnorr2pc, DkgSignRefreshSign) { exercise_secp256k1_bip340(); }

TEST(ApiSchnorr2pc, UnsupportedCurveRejected) {
  failing_transport_t t;
  buf_t key_blob;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  EXPECT_EQ(coinbase::api::schnorr_2p::dkg(job, curve_id::p256, key_blob), E_BADARG);
}

TEST(ApiSchnorr2pc, RejectsOutOfRangeXShareInKeyBlob) {
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
      c1, c2, [&] { return coinbase::api::schnorr_2p::dkg(job1, curve_id::secp256k1, key_blob_1); },
      [&] { return coinbase::api::schnorr_2p::dkg(job2, curve_id::secp256k1, key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  key_blob_v1_t blob;
  ASSERT_EQ(coinbase::convert(blob, key_blob_1), SUCCESS);

  // `x_share == q` is out of range; valid shares must satisfy 0 <= x_share < q.
  blob.x_share = coinbase::crypto::bn_t(coinbase::crypto::curve_secp256k1.order());
  buf_t malformed_blob = coinbase::convert(blob);

  buf_t pub;
  EXPECT_EQ(coinbase::api::schnorr_2p::get_public_key_compressed(malformed_blob, pub), E_FORMAT);
}

TEST(ApiSchnorr2pc, KeyBlobPrivScalar_NoPubSign) {
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
      c1, c2, [&] { return coinbase::api::schnorr_2p::dkg(job1, curve_id::secp256k1, key_blob_1); },
      [&] { return coinbase::api::schnorr_2p::dkg(job2, curve_id::secp256k1, key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  // Refresh (exercise detach/attach on refreshed blobs too).
  buf_t refreshed_1;
  buf_t refreshed_2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::schnorr_2p::refresh(job1, key_blob_1, refreshed_1); },
      [&] { return coinbase::api::schnorr_2p::refresh(job2, key_blob_2, refreshed_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  // Detach into public blob + scalar.
  buf_t public_1;
  buf_t public_2;
  buf_t x1_fixed;
  buf_t x2_fixed;
  ASSERT_EQ(coinbase::api::schnorr_2p::detach_private_scalar(refreshed_1, public_1, x1_fixed), SUCCESS);
  ASSERT_EQ(coinbase::api::schnorr_2p::detach_private_scalar(refreshed_2, public_2, x2_fixed), SUCCESS);
  EXPECT_EQ(x1_fixed.size(), 32);
  EXPECT_EQ(x2_fixed.size(), 32);

  // Capture share points before detaching (public blobs no longer carry them).
  buf_t Qi_full_1;
  ASSERT_EQ(coinbase::api::schnorr_2p::get_public_share_compressed(refreshed_1, Qi_full_1), SUCCESS);

  buf_t Qi_full_2;
  ASSERT_EQ(coinbase::api::schnorr_2p::get_public_share_compressed(refreshed_2, Qi_full_2), SUCCESS);

  // Public blob should not be usable for signing.
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  {
    failing_transport_t ft;
    const coinbase::api::job_2p_t bad_job{party_t::p1, "p1", "p2", ft};
    buf_t sig;
    EXPECT_NE(coinbase::api::schnorr_2p::sign(bad_job, public_1, msg, sig), SUCCESS);
  }

  // Attach scalars back and sign.
  buf_t merged_1;
  buf_t merged_2;
  ASSERT_EQ(coinbase::api::schnorr_2p::attach_private_scalar(public_1, x1_fixed, Qi_full_1, merged_1), SUCCESS);
  ASSERT_EQ(coinbase::api::schnorr_2p::attach_private_scalar(public_2, x2_fixed, Qi_full_2, merged_2), SUCCESS);

  buf_t pub;
  ASSERT_EQ(coinbase::api::schnorr_2p::get_public_key_compressed(merged_1, pub), SUCCESS);
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_secp256k1, pub), SUCCESS);

  buf_t sig1;
  buf_t sig2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::schnorr_2p::sign(job1, merged_1, msg, sig1); },
      [&] { return coinbase::api::schnorr_2p::sign(job2, merged_2, msg, sig2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  ASSERT_EQ(sig1.size(), 64);
  EXPECT_EQ(sig2.size(), 0);
  ASSERT_EQ(coinbase::crypto::bip340::verify(Q, msg, sig1), SUCCESS);

  // Negative: wrong scalar should fail to attach.
  buf_t bad_x = x1_fixed;
  bad_x[0] ^= 0x01;
  buf_t bad_merged;
  EXPECT_NE(coinbase::api::schnorr_2p::attach_private_scalar(public_1, bad_x, Qi_full_1, bad_merged), SUCCESS);
}
