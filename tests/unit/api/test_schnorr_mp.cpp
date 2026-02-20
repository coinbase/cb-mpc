#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include <cbmpc/api/schnorr_mp.h>
#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/crypto/base_ecc_secp256k1.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;

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
        return coinbase::api::schnorr_mp::dkg_additive(job, curve_id::secp256k1, keys[static_cast<size_t>(i)],
                                                       sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);

  buf_t pub0;
  ASSERT_EQ(coinbase::api::schnorr_mp::get_public_key_compressed(keys[0], pub0), SUCCESS);
  EXPECT_EQ(pub0.size(), 33);
  for (int i = 1; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::schnorr_mp::get_public_key_compressed(keys[static_cast<size_t>(i)], pub_i), SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_secp256k1, pub0), SUCCESS);

  buf_t xonly0;
  ASSERT_EQ(coinbase::api::schnorr_mp::extract_public_key_xonly(keys[0], xonly0), SUCCESS);
  EXPECT_EQ(xonly0.size(), 32);
  EXPECT_EQ(xonly0, Q.get_x().to_bin(32));

  // Change the party ordering ("role" indices) between protocols.
  const std::vector<std::string_view> name_views2 = {names[0], names[2], names[1], names[3]};
  // Map new role index -> old role index (DKG) for the same party name.
  const int perm[n] = {0, 2, 1, 3};

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views2, *transports[static_cast<size_t>(i)]};
        return coinbase::api::schnorr_mp::sign_additive(job, keys[static_cast<size_t>(perm[i])], msg,
                                                        /*sig_receiver=*/2, sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  EXPECT_EQ(sigs[2].size(), 64);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    EXPECT_EQ(sigs[static_cast<size_t>(i)].size(), 0);
  }
  ASSERT_EQ(coinbase::crypto::bip340::verify(Q, msg, sigs[2]), SUCCESS);

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views2, *transports[static_cast<size_t>(i)]};
        return coinbase::api::schnorr_mp::refresh_additive(job, sids[static_cast<size_t>(perm[i])],
                                                           keys[static_cast<size_t>(perm[i])],
                                                           new_keys[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);

  for (int i = 0; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::schnorr_mp::get_public_key_compressed(new_keys[static_cast<size_t>(i)], pub_i), SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views2, *transports[static_cast<size_t>(i)]};
        return coinbase::api::schnorr_mp::sign_additive(job, new_keys[static_cast<size_t>(i)], msg, /*sig_receiver=*/2,
                                                        new_sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  EXPECT_EQ(new_sigs[2].size(), 64);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    EXPECT_EQ(new_sigs[static_cast<size_t>(i)].size(), 0);
  }
  ASSERT_EQ(coinbase::crypto::bip340::verify(Q, msg, new_sigs[2]), SUCCESS);
}

}  // namespace

TEST(ApiSchnorrMp, DkgSignRefreshSignRoleChange4p) { exercise_4p_role_change(); }

TEST(ApiSchnorrMp, RejectsInvalidSigReceiver) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t sig;
  EXPECT_EQ(coinbase::api::schnorr_mp::sign_additive(job, mem_t(), mem_t(), /*sig_receiver=*/5, sig), E_BADARG);
}

TEST(ApiSchnorrMp, UnsupportedCurveRejected) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key;
  buf_t sid;
  EXPECT_EQ(coinbase::api::schnorr_mp::dkg_additive(job, curve_id::p256, key, sid), E_BADARG);
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

TEST(ApiSchnorrMpNeg, DkgInvalidCurve) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t key, sid;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::dkg_additive(job, curve_id(0), key, sid), SUCCESS);
  EXPECT_NE(coinbase::api::schnorr_mp::dkg_additive(job, curve_id(4), key, sid), SUCCESS);
  EXPECT_NE(coinbase::api::schnorr_mp::dkg_additive(job, curve_id(255), key, sid), SUCCESS);
}

TEST(ApiSchnorrMpNeg, DkgEmptyPartyName) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t key, sid;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::dkg_additive(job, curve_id::secp256k1, key, sid), SUCCESS);
}

TEST(ApiSchnorrMpNeg, DkgSingleParty) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t key, sid;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::dkg_additive(job, curve_id::secp256k1, key, sid), SUCCESS);
}

TEST(ApiSchnorrMpNeg, GetPubKeyCompressedEmptyBlob) {
  buf_t pub;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::get_public_key_compressed(mem_t(), pub), SUCCESS);
}

TEST(ApiSchnorrMpNeg, GetPubKeyCompressedGarbageBlob) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t pub;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::get_public_key_compressed(mem_t(garbage, 4), pub), SUCCESS);
}

TEST(ApiSchnorrMpNeg, ExtractPubKeyXonlyEmptyBlob) {
  buf_t pub;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::extract_public_key_xonly(mem_t(), pub), SUCCESS);
}

TEST(ApiSchnorrMpNeg, ExtractPubKeyXonlyGarbageBlob) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t pub;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::extract_public_key_xonly(mem_t(garbage, 4), pub), SUCCESS);
}

TEST(ApiSchnorrMpNeg, GetPublicShareCompressedEmptyBlob) {
  buf_t Qi;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::get_public_share_compressed(mem_t(), Qi), SUCCESS);
}

TEST(ApiSchnorrMpNeg, GetPublicShareCompressedGarbageBlob) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t Qi;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::get_public_share_compressed(mem_t(garbage, 4), Qi), SUCCESS);
}

TEST(ApiSchnorrMpNeg, DetachPrivateScalarEmptyBlob) {
  buf_t out_pub, out_scalar;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::detach_private_scalar(mem_t(), out_pub, out_scalar), SUCCESS);
}

TEST(ApiSchnorrMpNeg, DetachPrivateScalarGarbageBlob) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t out_pub, out_scalar;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::detach_private_scalar(mem_t(garbage, 4), out_pub, out_scalar), SUCCESS);
}

TEST(ApiSchnorrMpNeg, AttachPrivateScalarEmptyPubBlob) {
  buf_t scalar(32);
  buf_t Qi(33);
  buf_t out_blob;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::attach_private_scalar(mem_t(), scalar, Qi, out_blob), SUCCESS);
}

TEST(ApiSchnorrMpNeg, AttachPrivateScalarWrongSizeScalar) {
  const uint8_t pub[] = {0x01, 0x02, 0x03, 0x04};
  buf_t scalar_16(16);
  buf_t Qi(33);
  buf_t out_blob;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::attach_private_scalar(mem_t(pub, 4), scalar_16, Qi, out_blob), SUCCESS);
}

TEST(ApiSchnorrMpNeg, SignAdditiveInvalidSigReceiverNeg1) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t msg(32);
  buf_t sig;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::sign_additive(job, mem_t(), msg, /*sig_receiver=*/-1, sig), SUCCESS);
}

TEST(ApiSchnorrMpNeg, SignAdditiveMsg31Bytes) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t msg(31);
  buf_t sig;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::sign_additive(job, mem_t(), msg, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiSchnorrMpNeg, SignAdditiveMsg33Bytes) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t msg(33);
  buf_t sig;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::sign_additive(job, mem_t(), msg, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiSchnorrMpNeg, SignAdditiveMsg0Bytes) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t sig;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::sign_additive(job, mem_t(), mem_t(), /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiSchnorrMpNeg, SignAdditiveEmptyKeyBlob) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t msg(32);
  buf_t sig;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::sign_additive(job, mem_t(), msg, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiSchnorrMpNeg, SignAdditiveGarbageKeyBlob) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t msg(32);
  buf_t sig;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::sign_additive(job, mem_t(garbage, 4), msg, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiSchnorrMpNeg, RefreshAdditiveEmptyKeyBlob) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t sid, new_key;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::refresh_additive(job, sid, mem_t(), new_key), SUCCESS);
}

TEST(ApiSchnorrMpNeg, RefreshAdditiveGarbageKeyBlob) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t sid, new_key;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::schnorr_mp::refresh_additive(job, sid, mem_t(garbage, 4), new_key), SUCCESS);
}
