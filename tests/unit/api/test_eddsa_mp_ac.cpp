#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include <cbmpc/api/eddsa_mp.h>
#include <cbmpc/core/access_structure.h>
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

}  // namespace

TEST(ApiEdDSAMpAc, DkgRefreshSign4p) {
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

  // THRESHOLD[2](p0, p1, p2, p3)
  const coinbase::api::access_structure_t ac =
      coinbase::api::access_structure_t::Threshold(2, {
                                                          coinbase::api::access_structure_t::leaf(names[0]),
                                                          coinbase::api::access_structure_t::leaf(names[1]),
                                                          coinbase::api::access_structure_t::leaf(names[2]),
                                                          coinbase::api::access_structure_t::leaf(names[3]),
                                                      });

  // Only p0 and p1 actively contribute to the DKG/refresh.
  const std::vector<std::string_view> quorum_party_names = {names[0], names[1]};

  std::vector<buf_t> key_blobs(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sids[static_cast<size_t>(i)], ac,
                                               quorum_party_names, key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);

  buf_t pub0;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_key_compressed(key_blobs[0], pub0), SUCCESS);
  EXPECT_EQ(pub0.size(), 32);
  for (int i = 1; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::eddsa_mp::get_public_key_compressed(key_blobs[static_cast<size_t>(i)], pub_i), SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_ed25519, pub0), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);

  std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers = {peers[0], peers[1]};
  std::vector<std::shared_ptr<local_api_transport_t>> sign_transports = {transports[0], transports[1]};

  constexpr int quorum_n = 2;
  std::vector<buf_t> sigs(quorum_n);
  run_mp(
      sign_peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), quorum_party_names, *sign_transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::sign_ac(job, key_blobs[static_cast<size_t>(i)], ac, msg, /*sig_receiver=*/0,
                                                sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  ASSERT_EQ(sigs[0].size(), 64);
  EXPECT_EQ(sigs[1].size(), 0);
  ASSERT_EQ(verify_key.verify(msg, sigs[0]), SUCCESS);

  // Threshold refresh.
  std::vector<buf_t> new_key_blobs(n);
  std::vector<buf_t> refresh_sids(n);
  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::refresh_ac(job, refresh_sids[static_cast<size_t>(i)],
                                                   key_blobs[static_cast<size_t>(i)], ac, quorum_party_names,
                                                   new_key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(refresh_sids[0], refresh_sids[static_cast<size_t>(i)]);

  for (int i = 0; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::eddsa_mp::get_public_key_compressed(new_key_blobs[static_cast<size_t>(i)], pub_i),
              SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  std::vector<buf_t> sigs2(quorum_n);
  run_mp(
      sign_peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), quorum_party_names, *sign_transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::sign_ac(job, new_key_blobs[static_cast<size_t>(i)], ac, msg,
                                                /*sig_receiver=*/0, sigs2[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  ASSERT_EQ(sigs2[0].size(), 64);
  EXPECT_EQ(sigs2[1].size(), 0);
  ASSERT_EQ(verify_key.verify(msg, sigs2[0]), SUCCESS);
}

TEST(ApiEdDSAMpAc, KeyBlobPrivScalar_NoPubSign) {
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

  // THRESHOLD[2](p0, p1, p2, p3)
  const coinbase::api::access_structure_t ac =
      coinbase::api::access_structure_t::Threshold(2, {
                                                          coinbase::api::access_structure_t::leaf(names[0]),
                                                          coinbase::api::access_structure_t::leaf(names[1]),
                                                          coinbase::api::access_structure_t::leaf(names[2]),
                                                          coinbase::api::access_structure_t::leaf(names[3]),
                                                      });
  const std::vector<std::string_view> quorum_party_names = {names[0], names[1]};

  std::vector<buf_t> key_blobs(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sids[static_cast<size_t>(i)], ac,
                                               quorum_party_names, key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);

  buf_t pub0;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_key_compressed(key_blobs[0], pub0), SUCCESS);
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_ed25519, pub0), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  std::vector<buf_t> public_blobs(n);
  std::vector<buf_t> x_fixed(n);
  std::vector<buf_t> merged(n);
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(
        coinbase::api::eddsa_mp::detach_private_scalar(key_blobs[static_cast<size_t>(i)], public_blobs[i], x_fixed[i]),
        SUCCESS);
    EXPECT_GT(public_blobs[i].size(), 0);
    EXPECT_EQ(x_fixed[i].size(), 32);  // ed25519 order size

    buf_t Qi_full;
    buf_t Qi_public;
    ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(key_blobs[static_cast<size_t>(i)], Qi_full),
              SUCCESS);
    ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(public_blobs[i], Qi_public), SUCCESS);
    EXPECT_EQ(Qi_full, Qi_public);

    ASSERT_EQ(coinbase::api::eddsa_mp::attach_private_scalar(public_blobs[i], x_fixed[i], Qi_full, merged[i]), SUCCESS);
    EXPECT_GT(merged[i].size(), 0);
  }

  // Public blob should not be usable for signing.
  // Avoid spinning up a full protocol run here: sign_ac rejects at key blob parsing
  // before any transport calls, so a single local call is sufficient.
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  {
    failing_transport_t t;
    job_mp_t job{/*self=*/0, quorum_party_names, t};
    buf_t sig;
    EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, public_blobs[0], ac, msg, /*sig_receiver=*/0, sig), SUCCESS);
  }

  // Merged blobs should be usable for signing.
  std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers = {peers[0], peers[1]};
  std::vector<std::shared_ptr<local_api_transport_t>> sign_transports = {transports[0], transports[1]};
  constexpr int quorum_n = 2;
  std::vector<buf_t> sigs(quorum_n);
  run_mp(
      sign_peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), quorum_party_names, *sign_transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::sign_ac(job, merged[static_cast<size_t>(i)], ac, msg, /*sig_receiver=*/0,
                                                sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  ASSERT_EQ(sigs[0].size(), 64);
  EXPECT_EQ(sigs[1].size(), 0);
  ASSERT_EQ(verify_key.verify(msg, sigs[0]), SUCCESS);

  // Negative: merging the wrong scalar should fail.
  buf_t Qi0;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(key_blobs[0], Qi0), SUCCESS);
  buf_t bad_x = x_fixed[0];
  bad_x[0] ^= 0x01;
  buf_t bad_merged;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(public_blobs[0], bad_x, Qi0, bad_merged), SUCCESS);
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

TEST(ApiEdDSAMpAc, NegDkgWeierstrassCurves) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  {
    buf_t sid, key_blob;
    EXPECT_NE(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::p256, sid, ac, quorum, key_blob), SUCCESS);
  }
  {
    buf_t sid, key_blob;
    EXPECT_NE(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::secp256k1, sid, ac, quorum, key_blob), SUCCESS);
  }
}

TEST(ApiEdDSAMpAc, NegDkgInvalidCurveValues) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  for (uint32_t val : {0u, 4u, 255u}) {
    buf_t sid, key_blob;
    EXPECT_NE(coinbase::api::eddsa_mp::dkg_ac(job, static_cast<curve_id>(val), sid, ac, quorum, key_blob), SUCCESS)
        << "Expected failure for curve_id=" << val;
  }
}

TEST(ApiEdDSAMpAc, NegDkgEmptyQuorum) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  const std::vector<std::string_view> empty_quorum;
  buf_t sid, key_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, empty_quorum, key_blob), SUCCESS);
}

TEST(ApiEdDSAMpAc, NegDkgSinglePartyJob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::Threshold(1, {
                                                                      coinbase::api::access_structure_t::leaf(names[0]),
                                                                  });
  const std::vector<std::string_view> quorum = {names[0]};
  buf_t sid, key_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, quorum, key_blob), SUCCESS);
}

TEST(ApiEdDSAMpAc, NegDkgEmptyPartyName) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "", "p2"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
      coinbase::api::access_structure_t::leaf(names[2]),
  });
  const std::vector<std::string_view> quorum = {names[0], names[1], names[2]};
  buf_t sid, key_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, quorum, key_blob), SUCCESS);
}

TEST(ApiEdDSAMpAc, NegDkgRootLeaf) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::leaf(names[0]);
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, key_blob;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, quorum, key_blob), E_BADARG);
}

TEST(ApiEdDSAMpAc, NegDkgThresholdExceedsChildren) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::Threshold(3, {
                                                                      coinbase::api::access_structure_t::leaf(names[0]),
                                                                      coinbase::api::access_structure_t::leaf(names[1]),
                                                                  });
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, key_blob;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, quorum, key_blob), E_BADARG);
}

TEST(ApiEdDSAMpAc, NegDkgThresholdZero) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::Threshold(0, {
                                                                      coinbase::api::access_structure_t::leaf(names[0]),
                                                                      coinbase::api::access_structure_t::leaf(names[1]),
                                                                  });
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, key_blob;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, quorum, key_blob), E_BADARG);
}

TEST(ApiEdDSAMpAc, NegDkgNegativeThreshold) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac =
      coinbase::api::access_structure_t::Threshold(-1, {
                                                           coinbase::api::access_structure_t::leaf(names[0]),
                                                           coinbase::api::access_structure_t::leaf(names[1]),
                                                       });
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, key_blob;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, quorum, key_blob), E_BADARG);
}

TEST(ApiEdDSAMpAc, NegDkgEmptyAnd) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({});
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, key_blob;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, quorum, key_blob), E_BADARG);
}

TEST(ApiEdDSAMpAc, NegDkgDuplicateLeaves) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, key_blob;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, quorum, key_blob), E_BADARG);
}

TEST(ApiEdDSAMpAc, NegDkgInternalNodeNoChildren) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  coinbase::api::access_structure_t ac;
  ac.type = coinbase::api::access_structure_t::node_type::and_node;
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, key_blob;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, quorum, key_blob), E_BADARG);
}

TEST(ApiEdDSAMpAc, NegDkgUnknownPartyInQuorum) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::Threshold(2, {
                                                                      coinbase::api::access_structure_t::leaf(names[0]),
                                                                      coinbase::api::access_structure_t::leaf(names[1]),
                                                                      coinbase::api::access_structure_t::leaf(names[2]),
                                                                  });
  const std::vector<std::string_view> bad_quorum = {names[0], "unknown"};
  buf_t sid, key_blob;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, bad_quorum, key_blob), E_BADARG);
}

TEST(ApiEdDSAMpAc, NegDkgInsufficientQuorum) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2", "p3"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac =
      coinbase::api::access_structure_t::Threshold(2, {
                                                          coinbase::api::access_structure_t::And({
                                                              coinbase::api::access_structure_t::leaf(names[0]),
                                                              coinbase::api::access_structure_t::leaf(names[1]),
                                                          }),
                                                          coinbase::api::access_structure_t::Or({
                                                              coinbase::api::access_structure_t::leaf(names[2]),
                                                              coinbase::api::access_structure_t::leaf(names[3]),
                                                          }),
                                                      });
  const std::vector<std::string_view> bad_quorum = {names[0], names[2], names[3]};
  buf_t sid, key_blob;
  EXPECT_EQ(coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sid, ac, bad_quorum, key_blob), E_BADARG);
}

#include <cstring>

namespace {

using coinbase::mem_t;

static void generate_eddsa_mp_ac_key_blobs(int n, std::vector<buf_t>& blobs) {
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

  const auto ac = coinbase::api::access_structure_t::Threshold(2, {
                                                                      coinbase::api::access_structure_t::leaf(names[0]),
                                                                      coinbase::api::access_structure_t::leaf(names[1]),
                                                                      coinbase::api::access_structure_t::leaf(names[2]),
                                                                      coinbase::api::access_structure_t::leaf(names[3]),
                                                                  });
  const std::vector<std::string_view> quorum = {names[0], names[1]};

  blobs.resize(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;
  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::dkg_ac(job, curve_id::ed25519, sids[static_cast<size_t>(i)], ac, quorum,
                                               blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
}

}  // namespace

class ApiEdDSAMpAcNegWithBlobs : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { generate_eddsa_mp_ac_key_blobs(4, blobs_); }
  static std::vector<buf_t> blobs_;
};

std::vector<buf_t> ApiEdDSAMpAcNegWithBlobs::blobs_;

TEST(ApiEdDSAMpAc, NegSignAcEmptyKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, mem_t(), ac, msg, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiEdDSAMpAc, NegSignAcGarbageKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, mem_t(garbage, sizeof(garbage)), ac, msg, /*sig_receiver=*/0, sig),
            SUCCESS);
}

TEST(ApiEdDSAMpAc, NegSignAcAllZeroKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  uint8_t zeros[64] = {};
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, mem_t(zeros, sizeof(zeros)), ac, msg, /*sig_receiver=*/0, sig),
            SUCCESS);
}

TEST(ApiEdDSAMpAc, NegSignAcOversizedKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, big, ac, msg, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiEdDSAMpAc, NegSignAcEmptyMsg) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t sig;
  EXPECT_NE(
      coinbase::api::eddsa_mp::sign_ac(job, mem_t(garbage, sizeof(garbage)), ac, mem_t(), /*sig_receiver=*/0, sig),
      SUCCESS);
}

TEST(ApiEdDSAMpAc, NegSignAcNegativeSigReceiver) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, mem_t(garbage, sizeof(garbage)), ac, msg, /*sig_receiver=*/-1, sig),
            SUCCESS);
}

TEST(ApiEdDSAMpAc, NegSignAcSigReceiverTooLarge) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, mem_t(garbage, sizeof(garbage)), ac, msg,
                                             /*sig_receiver=*/static_cast<party_idx_t>(names.size()), sig),
            SUCCESS);
}

TEST(ApiEdDSAMpAc, NegSignAcEmptyAndAccessStructure) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({});
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, mem_t(garbage, sizeof(garbage)), ac, msg, /*sig_receiver=*/0, sig),
            SUCCESS);
}

TEST(ApiEdDSAMpAc, NegSignAcEmptyOrAccessStructure) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::Or({});
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, mem_t(garbage, sizeof(garbage)), ac, msg, /*sig_receiver=*/0, sig),
            SUCCESS);
}

TEST(ApiEdDSAMpAc, NegRefreshAcEmptyKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, new_key_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::refresh_ac(job, sid, mem_t(), ac, quorum, new_key_blob), SUCCESS);
}

TEST(ApiEdDSAMpAc, NegRefreshAcGarbageKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, new_key_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::refresh_ac(job, sid, mem_t(garbage, sizeof(garbage)), ac, quorum, new_key_blob),
            SUCCESS);
}

TEST(ApiEdDSAMpAc, NegRefreshAcAllZeroKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  uint8_t zeros[64] = {};
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, new_key_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::refresh_ac(job, sid, mem_t(zeros, sizeof(zeros)), ac, quorum, new_key_blob),
            SUCCESS);
}

TEST(ApiEdDSAMpAc, NegRefreshAcOversizedKeyBlob) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  const std::vector<std::string_view> quorum = {names[0], names[1]};
  buf_t sid, new_key_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::refresh_ac(job, sid, big, ac, quorum, new_key_blob), SUCCESS);
}

TEST(ApiEdDSAMpAc, NegRefreshAcEmptyQuorum) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const auto ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const std::vector<std::string_view> empty_quorum;
  buf_t sid, new_key_blob;
  EXPECT_NE(
      coinbase::api::eddsa_mp::refresh_ac(job, sid, mem_t(garbage, sizeof(garbage)), ac, empty_quorum, new_key_blob),
      SUCCESS);
}

TEST(ApiEdDSAMpAc, NegRefreshAcAdditiveKeyBlob) {
  constexpr int quorum_n = 2;
  std::vector<std::shared_ptr<mpc_net_context_t>> dkg_peers;
  dkg_peers.reserve(quorum_n);
  for (int i = 0; i < quorum_n; i++) dkg_peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : dkg_peers) p->init_with_peers(dkg_peers);

  std::vector<std::shared_ptr<local_api_transport_t>> dkg_transports;
  dkg_transports.reserve(quorum_n);
  for (const auto& p : dkg_peers) dkg_transports.push_back(std::make_shared<local_api_transport_t>(p));

  const std::vector<std::string_view> quorum = {"p0", "p1"};

  std::vector<buf_t> additive_key_blobs(quorum_n);
  std::vector<buf_t> additive_sids(quorum_n);
  std::vector<error_t> rvs;
  run_mp(
      dkg_peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), quorum, *dkg_transports[static_cast<size_t>(i)]};
        return coinbase::api::eddsa_mp::dkg_additive(job, curve_id::ed25519, additive_key_blobs[static_cast<size_t>(i)],
                                                     additive_sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);

  failing_transport_t t;
  job_mp_t job{/*self=*/0, quorum, t};

  const auto quorum_ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(quorum[0]),
      coinbase::api::access_structure_t::leaf(quorum[1]),
  });

  buf_t sid, new_key_blob;
  EXPECT_EQ(coinbase::api::eddsa_mp::refresh_ac(job, sid, additive_key_blobs[0], quorum_ac, quorum, new_key_blob),
            E_FORMAT);
}

TEST_F(ApiEdDSAMpAcNegWithBlobs, NegSignAcEmptyMsgValidBlob) {
  std::vector<std::string> names = {"p0", "p1"};
  std::vector<std::string_view> name_views(names.begin(), names.end());
  failing_transport_t t;
  job_mp_t job{/*self=*/0, name_views, t};
  const auto ac = coinbase::api::access_structure_t::Threshold(2, {
                                                                      coinbase::api::access_structure_t::leaf("p0"),
                                                                      coinbase::api::access_structure_t::leaf("p1"),
                                                                      coinbase::api::access_structure_t::leaf("p2"),
                                                                      coinbase::api::access_structure_t::leaf("p3"),
                                                                  });
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, blobs_[0], ac, mem_t(), /*sig_receiver=*/0, sig), SUCCESS);
}

TEST_F(ApiEdDSAMpAcNegWithBlobs, NegAttachWrongScalarSize) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  {
    buf_t short_scalar(31);
    std::memset(short_scalar.data(), 0x01, static_cast<size_t>(short_scalar.size()));
    buf_t out;
    EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, short_scalar, Qi, out), SUCCESS);
  }
  {
    buf_t long_scalar(33);
    std::memset(long_scalar.data(), 0x01, static_cast<size_t>(long_scalar.size()));
    buf_t out;
    EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, long_scalar, Qi, out), SUCCESS);
  }
}

TEST_F(ApiEdDSAMpAcNegWithBlobs, NegAttachZeroScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  buf_t zero_scalar(x.size());
  std::memset(zero_scalar.data(), 0x00, static_cast<size_t>(zero_scalar.size()));
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, zero_scalar, Qi, out), SUCCESS);
}

TEST_F(ApiEdDSAMpAcNegWithBlobs, NegAttachGarbageScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::eddsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  buf_t garbage_scalar(x.size());
  std::memset(garbage_scalar.data(), 0xDE, static_cast<size_t>(garbage_scalar.size()));
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, garbage_scalar, Qi, out), SUCCESS);
}

TEST_F(ApiEdDSAMpAcNegWithBlobs, NegAttachEmptyPublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, x, mem_t(), out), SUCCESS);
}

TEST_F(ApiEdDSAMpAcNegWithBlobs, NegAttachAllZeroPublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::eddsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  uint8_t zero_point[32] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::eddsa_mp::attach_private_scalar(pub_blob, x, mem_t(zero_point, 32), out), SUCCESS);
}

TEST_F(ApiEdDSAMpAcNegWithBlobs, NegRefreshAcInvalidAccessStructure) {
  std::vector<std::string> names = {"p0", "p1", "p2", "p3"};
  std::vector<std::string_view> name_views(names.begin(), names.end());
  failing_transport_t t;
  job_mp_t job{/*self=*/0, name_views, t};
  const auto bad_ac = coinbase::api::access_structure_t::leaf("p0");
  const std::vector<std::string_view> quorum = {"p0", "p1"};
  buf_t sid, new_key_blob;
  EXPECT_NE(coinbase::api::eddsa_mp::refresh_ac(job, sid, blobs_[0], bad_ac, quorum, new_key_blob), SUCCESS);
}

TEST_F(ApiEdDSAMpAcNegWithBlobs, NegSignAcWrongAccessStructure) {
  std::vector<std::string> names = {"p0", "p1"};
  std::vector<std::string_view> name_views(names.begin(), names.end());
  failing_transport_t t;
  job_mp_t job{/*self=*/0, name_views, t};
  const auto wrong_ac =
      coinbase::api::access_structure_t::Threshold(3, {
                                                          coinbase::api::access_structure_t::leaf("p0"),
                                                          coinbase::api::access_structure_t::leaf("p1"),
                                                          coinbase::api::access_structure_t::leaf("p2"),
                                                          coinbase::api::access_structure_t::leaf("p3"),
                                                      });
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, blobs_[0], wrong_ac, msg, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST_F(ApiEdDSAMpAcNegWithBlobs, NegSignAcInsufficientQuorum) {
  std::vector<std::string_view> name_views = {"p0"};
  failing_transport_t t;
  job_mp_t job{/*self=*/0, name_views, t};
  const auto ac = coinbase::api::access_structure_t::Threshold(2, {
                                                                      coinbase::api::access_structure_t::leaf("p0"),
                                                                      coinbase::api::access_structure_t::leaf("p1"),
                                                                      coinbase::api::access_structure_t::leaf("p2"),
                                                                      coinbase::api::access_structure_t::leaf("p3"),
                                                                  });
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(i);
  buf_t sig;
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job, blobs_[0], ac, msg, /*sig_receiver=*/0, sig), SUCCESS);
}
