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
