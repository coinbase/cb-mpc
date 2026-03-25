#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include <cbmpc/api/eddsa_mp.h>
#include <cbmpc/internal/crypto/base_ecc.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;

using coinbase::api::curve_id;
using coinbase::api::job_mp_t;
using coinbase::api::party_idx_t;

using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::api_harness::failing_transport_t;
using coinbase::testutils::api_harness::local_api_transport_t;
using coinbase::testutils::api_harness::run_mp;

static void exercise_4p_role_change() {
  constexpr int n = 4;
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  std::vector<std::shared_ptr<local_api_transport_t>> transports;
  transports.reserve(n);
  for (const auto& p : peers) transports.push_back(std::make_shared<local_api_transport_t>(p));

  std::vector<std::string> names = {"p0", "p1", "p2", "p3"};
  std::vector<std::string_view> name_views;
  name_views.reserve(names.size());
  for (const auto& name : names) name_views.emplace_back(name);

  std::vector<buf_t> keys(n);
  std::vector<buf_t> new_keys(n);
  std::vector<buf_t> sids(n);
  std::vector<buf_t> sigs(n);
  std::vector<buf_t> new_sigs(n);
  std::vector<error_t> rvs;

  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::dkg_additive(job, curve_id::ed25519, keys[static_cast<size_t>(i)],
                                                     sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);

  buf_t pub0;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_key_compressed(keys[0], pub0), SUCCESS);
  EXPECT_EQ(pub0.size(), 32);
  for (int i = 1; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::eddsa_mp::get_public_key_compressed(keys[static_cast<size_t>(i)], pub_i), SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_ed25519, pub0), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  // Change the party ordering ("role" indices) between protocols.
  const std::vector<std::string_view> name_views2 = {names[0], names[2], names[1], names[3]};
  // Map new role index -> old role index (DKG) for the same party name.
  const int perm[n] = {0, 2, 1, 3};

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views2, *transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::sign_additive(job, keys[static_cast<size_t>(perm[i])], msg, /*sig_receiver=*/2,
                                                      sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  EXPECT_EQ(sigs[2].size(), 64);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    EXPECT_EQ(sigs[static_cast<size_t>(i)].size(), 0);
  }
  ASSERT_EQ(verify_key.verify(msg, sigs[2]), SUCCESS);

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views2, *transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::refresh_additive(job, sids[static_cast<size_t>(perm[i])],
                                                         keys[static_cast<size_t>(perm[i])],
                                                         new_keys[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);

  for (int i = 0; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::eddsa_mp::get_public_key_compressed(new_keys[static_cast<size_t>(i)], pub_i), SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views2, *transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::sign_additive(job, new_keys[static_cast<size_t>(i)], msg, /*sig_receiver=*/2,
                                                      new_sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  EXPECT_EQ(new_sigs[2].size(), 64);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    EXPECT_EQ(new_sigs[static_cast<size_t>(i)].size(), 0);
  }
  ASSERT_EQ(verify_key.verify(msg, new_sigs[2]), SUCCESS);
}

}  // namespace

TEST(ApiEdDSAMp, DkgSignRefreshSignRoleChange4p) { exercise_4p_role_change(); }

TEST(ApiEdDSAMp, RejectsInvalidSigReceiver) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t sig;
  EXPECT_EQ(coinbase::api::eddsa_mp::sign_additive(job, coinbase::mem_t(), coinbase::mem_t(), /*sig_receiver=*/5, sig),
            E_BADARG);
}

TEST(ApiEdDSAMp, UnsupportedCurveRejected) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key;
  buf_t sid;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_additive(job, curve_id::secp256k1, key, sid), E_BADARG);
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

TEST(ApiEdDSAMp, NegDkgP256Curve) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_additive(job, curve_id::p256, key, sid), E_BADARG);
}

TEST(ApiEdDSAMp, NegDkgCurveZero) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_NE(coinbase::api::eddsa_mp::dkg_additive(job, static_cast<curve_id>(0), key, sid), SUCCESS);
}

TEST(ApiEdDSAMp, NegDkgCurveFour) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_NE(coinbase::api::eddsa_mp::dkg_additive(job, static_cast<curve_id>(4), key, sid), SUCCESS);
}

TEST(ApiEdDSAMp, NegDkgCurve255) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_NE(coinbase::api::eddsa_mp::dkg_additive(job, static_cast<curve_id>(255), key, sid), SUCCESS);
}

TEST(ApiEdDSAMp, NegDkgSingleParty) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_additive(job, curve_id::ed25519, key, sid), E_BADARG);
}

TEST(ApiEdDSAMp, NegDkgEmptyPartyNames) {
  failing_transport_t t;
  std::vector<std::string_view> names;
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_additive(job, curve_id::ed25519, key, sid), E_BADARG);
}

TEST(ApiEdDSAMp, NegDkgDuplicatePartyNames) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"dup", "dup"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_additive(job, curve_id::ed25519, key, sid), E_BADARG);
}

TEST(ApiEdDSAMp, NegDkgSelfOutOfRange) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/3, names, t};

  buf_t key, sid;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_additive(job, curve_id::ed25519, key, sid), E_BADARG);
}

TEST(ApiEdDSAMp, NegGetPubKeyEmpty) {
  buf_t pub;
  EXPECT_NE(coinbase::api::eddsa_mp::get_public_key_compressed(coinbase::mem_t(), pub), SUCCESS);
}

TEST(ApiEdDSAMp, NegGetPubKeyGarbage) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t pub;
  EXPECT_NE(coinbase::api::eddsa_mp::get_public_key_compressed(coinbase::mem_t(garbage, sizeof(garbage)), pub),
            SUCCESS);
}

TEST(ApiEdDSAMp, NegGetPubKeyAllZero) {
  uint8_t zeros[64] = {};
  buf_t pub;
  EXPECT_NE(coinbase::api::eddsa_mp::get_public_key_compressed(coinbase::mem_t(zeros, sizeof(zeros)), pub), SUCCESS);
}

TEST(ApiEdDSAMp, NegGetPubKeyOneByte) {
  uint8_t one = 0x00;
  buf_t pub;
  EXPECT_NE(coinbase::api::eddsa_mp::get_public_key_compressed(coinbase::mem_t(&one, 1), pub), SUCCESS);
}

TEST(ApiEdDSAMp, NegGetPubKeyOversized) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t pub;
  EXPECT_NE(coinbase::api::eddsa_mp::get_public_key_compressed(big, pub), SUCCESS);
}

TEST(ApiEdDSAMp, NegGetPubShareEmpty) {
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::get_public_share_compressed(coinbase::mem_t(), out), SUCCESS);
}

TEST(ApiEdDSAMp, NegGetPubShareGarbage) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::get_public_share_compressed(coinbase::mem_t(garbage, sizeof(garbage)), out),
            SUCCESS);
}

TEST(ApiEdDSAMp, NegGetPubShareAllZero) {
  uint8_t zeros[64] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::get_public_share_compressed(coinbase::mem_t(zeros, sizeof(zeros)), out), SUCCESS);
}

TEST(ApiEdDSAMp, NegGetPubShareOneByte) {
  uint8_t one = 0x00;
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::get_public_share_compressed(coinbase::mem_t(&one, 1), out), SUCCESS);
}

TEST(ApiEdDSAMp, NegGetPubShareOversized) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::get_public_share_compressed(big, out), SUCCESS);
}

TEST(ApiEdDSAMp, NegDetachEmpty) {
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::eddsa_mp::detach_private_scalar(coinbase::mem_t(), pub_blob, scalar), SUCCESS);
}

TEST(ApiEdDSAMp, NegDetachGarbage) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::eddsa_mp::detach_private_scalar(coinbase::mem_t(garbage, sizeof(garbage)), pub_blob, scalar),
            SUCCESS);
}

TEST(ApiEdDSAMp, NegDetachAllZero) {
  uint8_t zeros[64] = {};
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::eddsa_mp::detach_private_scalar(coinbase::mem_t(zeros, sizeof(zeros)), pub_blob, scalar),
            SUCCESS);
}

TEST(ApiEdDSAMp, NegDetachOneByte) {
  uint8_t one = 0x00;
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::eddsa_mp::detach_private_scalar(coinbase::mem_t(&one, 1), pub_blob, scalar), SUCCESS);
}

TEST(ApiEdDSAMp, NegDetachOversized) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::eddsa_mp::detach_private_scalar(big, pub_blob, scalar), SUCCESS);
}

TEST(ApiEdDSAMp, NegAttachEmptyPublicKeyBlob) {
  uint8_t scalar[32] = {0x01};
  uint8_t point[32] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(coinbase::mem_t(), coinbase::mem_t(scalar, 32),
                                                           coinbase::mem_t(point, 32), out),
            SUCCESS);
}

TEST(ApiEdDSAMp, NegAttachGarbagePublicKeyBlob) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  uint8_t scalar[32] = {0x01};
  uint8_t point[32] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(
      coinbase::api::eddsa_mp::attach_private_scalar(coinbase::mem_t(garbage, sizeof(garbage)),
                                                     coinbase::mem_t(scalar, 32), coinbase::mem_t(point, 32), out),
      SUCCESS);
}

TEST(ApiEdDSAMp, NegAttachOversizedPublicKeyBlob) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  uint8_t scalar[32] = {0x01};
  uint8_t point[32] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(
      coinbase::api::eddsa_mp::attach_private_scalar(big, coinbase::mem_t(scalar, 32), coinbase::mem_t(point, 32), out),
      SUCCESS);
}

TEST(ApiEdDSAMp, NegRefreshEmptyKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t sid, new_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::refresh_additive(job, sid, coinbase::mem_t(), new_blob), SUCCESS);
}

TEST(ApiEdDSAMp, NegRefreshGarbageKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t sid, new_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::refresh_additive(job, sid, coinbase::mem_t(garbage, sizeof(garbage)), new_blob),
            SUCCESS);
}

TEST(ApiEdDSAMp, NegRefreshAllZeroKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t zeros[64] = {};
  buf_t sid, new_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::refresh_additive(job, sid, coinbase::mem_t(zeros, sizeof(zeros)), new_blob),
            SUCCESS);
}

TEST(ApiEdDSAMp, NegRefreshOneByteKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t one = 0x00;
  buf_t sid, new_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::refresh_additive(job, sid, coinbase::mem_t(&one, 1), new_blob), SUCCESS);
}

TEST(ApiEdDSAMp, NegRefreshOversizedKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t sid, new_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::refresh_additive(job, sid, big, new_blob), SUCCESS);
}

TEST(ApiEdDSAMp, NegSignEmptyKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t msg(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_additive(job, coinbase::mem_t(), msg, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiEdDSAMp, NegSignGarbageKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t msg(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_additive(job, coinbase::mem_t(garbage, sizeof(garbage)), msg,
                                                   /*sig_receiver=*/0, sig),
            SUCCESS);
}

TEST(ApiEdDSAMp, NegSignAllZeroKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t zeros[64] = {};
  buf_t msg(32);
  buf_t sig;
  EXPECT_NE(
      coinbase::api::eddsa_mp::sign_additive(job, coinbase::mem_t(zeros, sizeof(zeros)), msg, /*sig_receiver=*/0, sig),
      SUCCESS);
}

TEST(ApiEdDSAMp, NegSignOneByteKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t one = 0x00;
  buf_t msg(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_additive(job, coinbase::mem_t(&one, 1), msg, /*sig_receiver=*/0, sig),
            SUCCESS);
}

TEST(ApiEdDSAMp, NegSignOversizedKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t msg(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_additive(job, big, msg, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiEdDSAMp, NegSignSigReceiverNegative) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t sig;
  EXPECT_EQ(coinbase::api::eddsa_mp::sign_additive(job, coinbase::mem_t(), coinbase::mem_t(), /*sig_receiver=*/-1, sig),
            E_BADARG);
}

namespace {
using coinbase::mem_t;

static void generate_eddsa_mp_key_blobs(int n, std::vector<buf_t>& blobs) {
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  std::vector<std::shared_ptr<local_api_transport_t>> transports;
  transports.reserve(n);
  for (const auto& p : peers) transports.push_back(std::make_shared<local_api_transport_t>(p));

  std::vector<std::string> names;
  std::vector<std::string_view> name_views;
  for (int i = 0; i < n; i++) names.push_back("p" + std::to_string(i));
  for (const auto& nm : names) name_views.emplace_back(nm);

  blobs.resize(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;
  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::dkg_additive(job, curve_id::ed25519, blobs[static_cast<size_t>(i)],
                                                     sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
}
}  // namespace

class ApiEdDSAMpNegWithBlobs : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { generate_eddsa_mp_key_blobs(3, blobs_); }
  static std::vector<buf_t> blobs_;
};
std::vector<buf_t> ApiEdDSAMpNegWithBlobs::blobs_;

TEST_F(ApiEdDSAMpNegWithBlobs, NegSignEmptyMsg) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_additive(job, blobs_[0], mem_t(), /*sig_receiver=*/0, sig), SUCCESS);
}

TEST_F(ApiEdDSAMpNegWithBlobs, NegAttachEmptyPrivateScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, mem_t(), Qi, out), SUCCESS);
}

TEST_F(ApiEdDSAMpNegWithBlobs, NegAttachGarbagePrivateScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  uint8_t garbage[512];
  std::memset(garbage, 0xFF, sizeof(garbage));
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, mem_t(garbage, sizeof(garbage)), Qi, out),
            SUCCESS);
}

TEST_F(ApiEdDSAMpNegWithBlobs, NegAttachWrongSizeScalar31) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  uint8_t short_scalar[31];
  std::memset(short_scalar, 0x01, sizeof(short_scalar));
  buf_t out;
  EXPECT_NE(
      coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, mem_t(short_scalar, sizeof(short_scalar)), Qi, out),
      SUCCESS);
}

TEST_F(ApiEdDSAMpNegWithBlobs, NegAttachWrongSizeScalar33) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  uint8_t long_scalar[33];
  std::memset(long_scalar, 0x01, sizeof(long_scalar));
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, mem_t(long_scalar, sizeof(long_scalar)), Qi, out),
            SUCCESS);
}

TEST_F(ApiEdDSAMpNegWithBlobs, NegAttachZeroScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  uint8_t zero[32] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, mem_t(zero, 32), Qi, out), SUCCESS);
}

TEST_F(ApiEdDSAMpNegWithBlobs, NegAttachEmptyPublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, x, mem_t(), out), SUCCESS);
}

TEST_F(ApiEdDSAMpNegWithBlobs, NegAttachAllZeroPublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  uint8_t zero_point[32] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, x, mem_t(zero_point, 32), out), SUCCESS);
}

TEST_F(ApiEdDSAMpNegWithBlobs, NegAttachGarbagePublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  uint8_t bad_point[32];
  std::memset(bad_point, 0xAB, sizeof(bad_point));
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, x, mem_t(bad_point, 32), out), SUCCESS);
}
