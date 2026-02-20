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
