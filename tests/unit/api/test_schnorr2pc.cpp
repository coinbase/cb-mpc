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

// ------------ Disclaimer: All the following tests have been generated by AI ------------

namespace {

using coinbase::mem_t;

static void generate_schnorr_key_blobs(buf_t& blob1, buf_t& blob2) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  local_api_transport_t t1(c1);
  local_api_transport_t t2(c2);

  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;

  const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};
  run_2pc(
      c1, c2, [&] { return coinbase::api::schnorr_2p::dkg(job1, curve_id::secp256k1, blob1); },
      [&] { return coinbase::api::schnorr_2p::dkg(job2, curve_id::secp256k1, blob2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
}

}  // namespace

class ApiSchnorr2pcNegWithBlobs : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { generate_schnorr_key_blobs(blob1_, blob2_); }
  static buf_t blob1_;
  static buf_t blob2_;
};

buf_t ApiSchnorr2pcNegWithBlobs::blob1_;
buf_t ApiSchnorr2pcNegWithBlobs::blob2_;

TEST(ApiSchnorr2pcNeg, DkgInvalidCurve0) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  buf_t key_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::dkg(job, static_cast<curve_id>(0), key_blob), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, DkgInvalidCurve4) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  buf_t key_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::dkg(job, static_cast<curve_id>(4), key_blob), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, DkgInvalidCurve255) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  buf_t key_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::dkg(job, static_cast<curve_id>(255), key_blob), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, DkgUnsupportedCurveP256) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  buf_t key_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::dkg(job, curve_id::p256, key_blob), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, DkgUnsupportedCurveEd25519) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  buf_t key_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::dkg(job, curve_id::ed25519, key_blob), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, DkgEmptyP1Name) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "", "p2", ft};
  buf_t key_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::dkg(job, curve_id::secp256k1, key_blob), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, DkgSameP1P2Name) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p1", ft};
  buf_t key_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::dkg(job, curve_id::secp256k1, key_blob), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, GetPubKeyCompressedEmptyBlob) {
  buf_t pub;
  EXPECT_NE(coinbase::api::schnorr_2p::get_public_key_compressed(mem_t(), pub), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, GetPubKeyCompressedGarbageBlob) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};
  buf_t pub;
  EXPECT_NE(coinbase::api::schnorr_2p::get_public_key_compressed(mem_t(garbage, sizeof(garbage)), pub), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, GetPubKeyCompressedOneByteBlob) {
  const uint8_t one = 0x00;
  buf_t pub;
  EXPECT_NE(coinbase::api::schnorr_2p::get_public_key_compressed(mem_t(&one, 1), pub), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, GetPubKeyCompressedAllZeroBlob) {
  uint8_t zeros[64] = {};
  buf_t pub;
  EXPECT_NE(coinbase::api::schnorr_2p::get_public_key_compressed(mem_t(zeros, sizeof(zeros)), pub), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, ExtractPubKeyXonlyEmptyBlob) {
  buf_t pub;
  EXPECT_NE(coinbase::api::schnorr_2p::extract_public_key_xonly(mem_t(), pub), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, ExtractPubKeyXonlyGarbageBlob) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};
  buf_t pub;
  EXPECT_NE(coinbase::api::schnorr_2p::extract_public_key_xonly(mem_t(garbage, sizeof(garbage)), pub), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, GetPublicShareCompressedEmptyBlob) {
  buf_t out;
  EXPECT_NE(coinbase::api::schnorr_2p::get_public_share_compressed(mem_t(), out), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, GetPublicShareCompressedGarbageBlob) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};
  buf_t out;
  EXPECT_NE(coinbase::api::schnorr_2p::get_public_share_compressed(mem_t(garbage, sizeof(garbage)), out), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, DetachPrivateScalarEmptyBlob) {
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::schnorr_2p::detach_private_scalar(mem_t(), pub_blob, scalar), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, DetachPrivateScalarGarbageBlob) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::schnorr_2p::detach_private_scalar(mem_t(garbage, sizeof(garbage)), pub_blob, scalar),
            SUCCESS);
}

TEST(ApiSchnorr2pcNeg, AttachPrivateScalarEmptyPubBlob) {
  uint8_t scalar_bytes[32] = {0x01};
  uint8_t point[33] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(coinbase::api::schnorr_2p::attach_private_scalar(mem_t(), mem_t(scalar_bytes, 32), mem_t(point, 33), out),
            SUCCESS);
}

TEST(ApiSchnorr2pcNeg, AttachPrivateScalarGarbagePubBlob) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  uint8_t scalar_bytes[32] = {0x01};
  uint8_t point[33] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(coinbase::api::schnorr_2p::attach_private_scalar(mem_t(garbage, sizeof(garbage)), mem_t(scalar_bytes, 32),
                                                             mem_t(point, 33), out),
            SUCCESS);
}

TEST(ApiSchnorr2pcNeg, AttachPrivateScalarWrongSizeScalar) {
  uint8_t point[33] = {};
  point[0] = 0x02;
  buf_t dummy_pub(64);
  buf_t out;

  uint8_t short_scalar[31] = {0x01};
  EXPECT_NE(coinbase::api::schnorr_2p::attach_private_scalar(dummy_pub, mem_t(short_scalar, 31), mem_t(point, 33), out),
            SUCCESS);

  uint8_t long_scalar[33] = {0x01};
  EXPECT_NE(coinbase::api::schnorr_2p::attach_private_scalar(dummy_pub, mem_t(long_scalar, 33), mem_t(point, 33), out),
            SUCCESS);
}

TEST(ApiSchnorr2pcNeg, AttachPrivateScalarEmptyScalar) {
  uint8_t point[33] = {};
  point[0] = 0x02;
  buf_t dummy_pub(64);
  buf_t out;
  EXPECT_NE(coinbase::api::schnorr_2p::attach_private_scalar(dummy_pub, mem_t(), mem_t(point, 33), out), SUCCESS);
}

TEST(ApiSchnorr2pcNeg, AttachPrivateScalarEmptyPubShare) {
  uint8_t scalar_bytes[32] = {0x01};
  buf_t dummy_pub(64);
  buf_t out;
  EXPECT_NE(coinbase::api::schnorr_2p::attach_private_scalar(dummy_pub, mem_t(scalar_bytes, 32), mem_t(), out),
            SUCCESS);
}

TEST_F(ApiSchnorr2pcNegWithBlobs, RefreshRoleMismatchP1BlobP2Job) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p2, "p1", "p2", ft};
  buf_t new_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::refresh(job, blob1_, new_blob), SUCCESS);
}

TEST_F(ApiSchnorr2pcNegWithBlobs, RefreshRoleMismatchP2BlobP1Job) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  buf_t new_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::refresh(job, blob2_, new_blob), SUCCESS);
}

TEST_F(ApiSchnorr2pcNegWithBlobs, RefreshEmptyBlob) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  buf_t new_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::refresh(job, mem_t(), new_blob), SUCCESS);
}

TEST_F(ApiSchnorr2pcNegWithBlobs, RefreshGarbageBlob) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t new_blob;
  EXPECT_NE(coinbase::api::schnorr_2p::refresh(job, mem_t(garbage, sizeof(garbage)), new_blob), SUCCESS);
}

TEST_F(ApiSchnorr2pcNegWithBlobs, SignMsgNot32Bytes) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  buf_t sig;

  buf_t msg_31(31);
  EXPECT_NE(coinbase::api::schnorr_2p::sign(job, blob1_, msg_31, sig), SUCCESS);

  buf_t msg_33(33);
  EXPECT_NE(coinbase::api::schnorr_2p::sign(job, blob1_, msg_33, sig), SUCCESS);

  EXPECT_NE(coinbase::api::schnorr_2p::sign(job, blob1_, mem_t(), sig), SUCCESS);
}

TEST_F(ApiSchnorr2pcNegWithBlobs, SignEmptyBlob) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  buf_t msg(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::schnorr_2p::sign(job, mem_t(), msg, sig), SUCCESS);
}

TEST_F(ApiSchnorr2pcNegWithBlobs, SignGarbageBlob) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", ft};
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t msg(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::schnorr_2p::sign(job, mem_t(garbage, sizeof(garbage)), msg, sig), SUCCESS);
}

TEST_F(ApiSchnorr2pcNegWithBlobs, SignRoleMismatch) {
  failing_transport_t ft;
  const coinbase::api::job_2p_t job{party_t::p2, "p1", "p2", ft};
  buf_t msg(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::schnorr_2p::sign(job, blob1_, msg, sig), SUCCESS);
}
