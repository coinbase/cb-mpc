#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include <cbmpc/api/ecdsa_mp.h>
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

static std::vector<std::shared_ptr<mpc_net_context_t>> make_peers(int n) {
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);
  return peers;
}

static std::vector<std::shared_ptr<local_api_transport_t>> make_transports(
    const std::vector<std::shared_ptr<mpc_net_context_t>>& peers) {
  std::vector<std::shared_ptr<local_api_transport_t>> transports;
  transports.reserve(peers.size());
  for (const auto& p : peers) transports.push_back(std::make_shared<local_api_transport_t>(p));
  return transports;
}

static buf_t make_msg_hash32() {
  buf_t msg_hash(32);
  for (int i = 0; i < msg_hash.size(); i++) msg_hash[i] = static_cast<uint8_t>(i);
  return msg_hash;
}

}  // namespace

TEST(ApiEcdsaMpAc, DkgRefreshSign4p) {
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
        return coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sids[static_cast<size_t>(i)], ac,
                                               quorum_party_names, key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);

  buf_t pub0;
  ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_key_compressed(key_blobs[0], pub0), SUCCESS);
  EXPECT_EQ(pub0.size(), 33);
  for (int i = 1; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_key_compressed(key_blobs[static_cast<size_t>(i)], pub_i), SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  // Verify a signature using only the online signing quorum parties.
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_secp256k1, pub0), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  buf_t msg_hash(32);
  for (int i = 0; i < msg_hash.size(); i++) msg_hash[i] = static_cast<uint8_t>(i);

  std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers = {peers[0], peers[1]};
  std::vector<std::shared_ptr<local_api_transport_t>> sign_transports = {transports[0], transports[1]};

  constexpr int quorum_n = 2;
  std::vector<buf_t> sigs(quorum_n);
  run_mp(
      sign_peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), quorum_party_names, *sign_transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::sign_ac(job, key_blobs[static_cast<size_t>(i)], ac, msg_hash,
                                                /*sig_receiver=*/0, sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  ASSERT_GT(sigs[0].size(), 0);
  EXPECT_EQ(sigs[1].size(), 0);
  ASSERT_EQ(verify_key.verify(msg_hash, sigs[0]), SUCCESS);

  // Threshold refresh.
  std::vector<buf_t> new_key_blobs(n);
  std::vector<buf_t> refresh_sids(n);
  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::refresh_ac(job, refresh_sids[static_cast<size_t>(i)],
                                                   key_blobs[static_cast<size_t>(i)], ac, quorum_party_names,
                                                   new_key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(refresh_sids[0], refresh_sids[static_cast<size_t>(i)]);

  for (int i = 0; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_key_compressed(new_key_blobs[static_cast<size_t>(i)], pub_i),
              SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  std::vector<buf_t> sigs2(quorum_n);
  run_mp(
      sign_peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), quorum_party_names, *sign_transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::sign_ac(job, new_key_blobs[static_cast<size_t>(i)], ac, msg_hash,
                                                /*sig_receiver=*/0, sigs2[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  ASSERT_GT(sigs2[0].size(), 0);
  EXPECT_EQ(sigs2[1].size(), 0);
  ASSERT_EQ(verify_key.verify(msg_hash, sigs2[0]), SUCCESS);
}

TEST(ApiEcdsaMpAc, RejectsAccessStructureLeafMismatch) {
  constexpr int n = 3;

  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  std::vector<std::shared_ptr<local_api_transport_t>> transports;
  transports.reserve(n);
  for (const auto& p : peers) transports.push_back(std::make_shared<local_api_transport_t>(p));

  std::vector<std::string> names = {"p0", "p1", "p2"};
  std::vector<std::string_view> name_views;
  name_views.reserve(names.size());
  for (const auto& name : names) name_views.emplace_back(name);

  // Missing leaf "p2" -> should reject before protocol starts.
  const coinbase::api::access_structure_t bad_ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
  });
  const std::vector<std::string_view> quorum_party_names = {names[0], names[1]};

  std::vector<buf_t> key_blobs(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sids[static_cast<size_t>(i)], bad_ac,
                                               quorum_party_names, key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, E_BADARG);
}

TEST(ApiEcdsaMpAc, ComplexAccess_DkgRefreshSign4p) {
  constexpr int n = 4;

  std::vector<std::shared_ptr<mpc_net_context_t>> peers = make_peers(n);
  std::vector<std::shared_ptr<local_api_transport_t>> transports = make_transports(peers);

  std::vector<std::string> names = {"p0", "p1", "p2", "p3"};
  std::vector<std::string_view> name_views;
  name_views.reserve(names.size());
  for (const auto& name : names) name_views.emplace_back(name);

  // Access structure:
  // THRESHOLD[2](
  //   AND(p0, p1),
  //   OR(p2, p3)
  // )
  //
  // Equivalent policy: p0 AND p1 AND (p2 OR p3).
  const coinbase::api::access_structure_t ac =
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

  // DKG quorum must satisfy the access structure. Choose {p0, p1, p2}.
  const std::vector<std::string_view> dkg_quorum_party_names = {names[0], names[1], names[2]};

  std::vector<buf_t> key_blobs(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sids[static_cast<size_t>(i)], ac,
                                               dkg_quorum_party_names, key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);

  buf_t pub0;
  ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_key_compressed(key_blobs[0], pub0), SUCCESS);
  EXPECT_EQ(pub0.size(), 33);
  for (int i = 1; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_key_compressed(key_blobs[static_cast<size_t>(i)], pub_i), SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_secp256k1, pub0), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);
  const buf_t msg_hash = make_msg_hash32();

  // Signing quorum A: {p0, p1, p2} (satisfies OR via p2).
  const std::vector<std::string_view> quorum_a = {names[0], names[1], names[2]};

  {
    std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers = make_peers(static_cast<int>(quorum_a.size()));
    std::vector<std::shared_ptr<local_api_transport_t>> sign_transports = make_transports(sign_peers);

    std::vector<buf_t> sigs(quorum_a.size());
    run_mp(
        sign_peers,
        [&](int i) {
          job_mp_t job{static_cast<party_idx_t>(i), quorum_a, *sign_transports[static_cast<size_t>(i)]};
          return coinbase::api::ecdsa_mp::sign_ac(job, key_blobs[static_cast<size_t>(i)], ac, msg_hash,
                                                  /*sig_receiver=*/0, sigs[static_cast<size_t>(i)]);
        },
        rvs);
    for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
    ASSERT_GT(sigs[0].size(), 0);
    for (size_t i = 1; i < sigs.size(); i++) EXPECT_EQ(sigs[i].size(), 0);
    ASSERT_EQ(verify_key.verify(msg_hash, sigs[0]), SUCCESS);
  }

  // Signing quorum B: {p0, p1, p3} (satisfies OR via p3).
  const std::vector<std::string_view> quorum_b = {names[0], names[1], names[3]};
  const std::vector<size_t> quorum_b_key_blob_indices = {0, 1, 3};

  {
    std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers = make_peers(static_cast<int>(quorum_b.size()));
    std::vector<std::shared_ptr<local_api_transport_t>> sign_transports = make_transports(sign_peers);

    std::vector<buf_t> sigs(quorum_b.size());
    run_mp(
        sign_peers,
        [&](int i) {
          job_mp_t job{static_cast<party_idx_t>(i), quorum_b, *sign_transports[static_cast<size_t>(i)]};
          const size_t key_blob_idx = quorum_b_key_blob_indices[static_cast<size_t>(i)];
          return coinbase::api::ecdsa_mp::sign_ac(job, key_blobs[key_blob_idx], ac, msg_hash, /*sig_receiver=*/0,
                                                  sigs[static_cast<size_t>(i)]);
        },
        rvs);
    for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
    ASSERT_GT(sigs[0].size(), 0);
    for (size_t i = 1; i < sigs.size(); i++) EXPECT_EQ(sigs[i].size(), 0);
    ASSERT_EQ(verify_key.verify(msg_hash, sigs[0]), SUCCESS);
  }

  // Threshold refresh with a different quorum that still satisfies the access structure.
  // Choose {p0, p1, p3}.
  std::vector<buf_t> new_key_blobs(n);
  std::vector<buf_t> refresh_sids(n);
  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::refresh_ac(job, refresh_sids[static_cast<size_t>(i)],
                                                   key_blobs[static_cast<size_t>(i)], ac, quorum_b,
                                                   new_key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(refresh_sids[0], refresh_sids[static_cast<size_t>(i)]);

  for (int i = 0; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_key_compressed(new_key_blobs[static_cast<size_t>(i)], pub_i),
              SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  // Ensure we can still sign using `sign_ac` for quorum B after refresh.

  {
    std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers = make_peers(static_cast<int>(quorum_b.size()));
    std::vector<std::shared_ptr<local_api_transport_t>> sign_transports = make_transports(sign_peers);

    std::vector<buf_t> sigs(quorum_b.size());
    run_mp(
        sign_peers,
        [&](int i) {
          job_mp_t job{static_cast<party_idx_t>(i), quorum_b, *sign_transports[static_cast<size_t>(i)]};
          const size_t key_blob_idx = quorum_b_key_blob_indices[static_cast<size_t>(i)];
          return coinbase::api::ecdsa_mp::sign_ac(job, new_key_blobs[key_blob_idx], ac, msg_hash, /*sig_receiver=*/0,
                                                  sigs[static_cast<size_t>(i)]);
        },
        rvs);
    for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
    ASSERT_GT(sigs[0].size(), 0);
    for (size_t i = 1; i < sigs.size(); i++) EXPECT_EQ(sigs[i].size(), 0);
    ASSERT_EQ(verify_key.verify(msg_hash, sigs[0]), SUCCESS);
  }
}

TEST(ApiEcdsaMpAc, RejectsMalformedAccessStructure) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};
  const std::vector<std::string_view> quorum = {names[0], names[1]};

  buf_t sid;
  buf_t key_blob;

  // Root leaf is not supported.
  {
    const auto ac = coinbase::api::access_structure_t::leaf(names[0]);
    EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sid, ac, quorum, key_blob), E_BADARG);
  }

  // Leaf node with children.
  {
    coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::And({
        coinbase::api::access_structure_t::leaf(names[0]),
        coinbase::api::access_structure_t::leaf(names[1]),
    });
    ac.children[0].children.push_back(coinbase::api::access_structure_t::leaf("x"));
    EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sid, ac, quorum, key_blob), E_BADARG);
  }

  // Internal node with no children.
  {
    coinbase::api::access_structure_t ac;
    ac.type = coinbase::api::access_structure_t::node_type::and_node;
    EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sid, ac, quorum, key_blob), E_BADARG);
  }

  // Invalid threshold_k.
  {
    coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
        /*k=*/0, {
                     coinbase::api::access_structure_t::leaf(names[0]),
                     coinbase::api::access_structure_t::leaf(names[1]),
                 });
    EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sid, ac, quorum, key_blob), E_BADARG);
  }

  // Duplicate leaf names should be rejected.
  {
    coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::And({
        coinbase::api::access_structure_t::leaf(names[0]),
        coinbase::api::access_structure_t::leaf(names[0]),
        coinbase::api::access_structure_t::leaf(names[1]),
    });
    EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sid, ac, quorum, key_blob), E_BADARG);
  }
}

TEST(ApiEcdsaMpAc, RejectDkgQuorumInsufficient) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2", "p3"};
  job_mp_t job{/*self=*/0, names, t};

  const coinbase::api::access_structure_t ac =
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

  // Missing p1 => does not satisfy AND(p0, p1).
  const std::vector<std::string_view> bad_quorum = {names[0], names[2], names[3]};

  buf_t sid;
  buf_t key_blob;
  EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sid, ac, bad_quorum, key_blob), E_BADARG);
}

TEST(ApiEcdsaMpAc, RejectsDkgQuorumUnknownPartyName) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  const coinbase::api::access_structure_t ac =
      coinbase::api::access_structure_t::Threshold(2, {
                                                          coinbase::api::access_structure_t::leaf(names[0]),
                                                          coinbase::api::access_structure_t::leaf(names[1]),
                                                          coinbase::api::access_structure_t::leaf(names[2]),
                                                      });

  const std::vector<std::string_view> bad_quorum = {names[0], "unknown"};

  buf_t sid;
  buf_t key_blob;
  EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sid, ac, bad_quorum, key_blob), E_BADARG);
}

TEST(ApiEcdsaMpAc, KeyBlobPrivScalar_NoPubSign) {
  constexpr int n = 5;

  std::vector<std::shared_ptr<mpc_net_context_t>> peers = make_peers(n);
  std::vector<std::shared_ptr<local_api_transport_t>> transports = make_transports(peers);

  std::vector<std::string> names = {"p0", "p1", "p2", "p3", "p4"};
  std::vector<std::string_view> name_views;
  name_views.reserve(names.size());
  for (const auto& name : names) name_views.emplace_back(name);

  // THRESHOLD[3](p0, p1, p2, p3, p4)
  const coinbase::api::access_structure_t ac =
      coinbase::api::access_structure_t::Threshold(3, {
                                                          coinbase::api::access_structure_t::leaf(names[0]),
                                                          coinbase::api::access_structure_t::leaf(names[1]),
                                                          coinbase::api::access_structure_t::leaf(names[2]),
                                                          coinbase::api::access_structure_t::leaf(names[3]),
                                                          coinbase::api::access_structure_t::leaf(names[4]),
                                                      });

  const std::vector<std::string_view> quorum_party_names = {names[0], names[1], names[2]};

  std::vector<buf_t> key_blobs(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sids[static_cast<size_t>(i)], ac,
                                               quorum_party_names, key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);

  // Detach/attach private scalar for each party and ensure the public blob preserves Qi_self extraction.
  std::vector<buf_t> redacted(n);
  std::vector<buf_t> x_fixed(n);
  std::vector<buf_t> merged(n);
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(
        coinbase::api::ecdsa_mp::detach_private_scalar(key_blobs[static_cast<size_t>(i)], redacted[i], x_fixed[i]),
        SUCCESS);
    EXPECT_GT(redacted[i].size(), 0);
    EXPECT_EQ(x_fixed[i].size(), 32);  // secp256k1 order size

    buf_t Qi_full;
    buf_t Qi_redacted;
    ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_share_compressed(key_blobs[static_cast<size_t>(i)], Qi_full),
              SUCCESS);
    ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_share_compressed(redacted[i], Qi_redacted), SUCCESS);
    EXPECT_EQ(Qi_full, Qi_redacted);

    ASSERT_EQ(coinbase::api::ecdsa_mp::attach_private_scalar(redacted[i], x_fixed[i], Qi_full, merged[i]), SUCCESS);
    EXPECT_GT(merged[i].size(), 0);
  }

  // Public blob should not be usable for signing.
  // Avoid spinning up a full protocol run here: sign_ac rejects at key blob parsing
  // before any transport calls, so a single local call is sufficient.
  const buf_t msg_hash = make_msg_hash32();
  {
    failing_transport_t t;
    job_mp_t job{/*self=*/0, quorum_party_names, t};
    buf_t sig;
    EXPECT_NE(coinbase::api::ecdsa_mp::sign_ac(job, redacted[0], ac, msg_hash, /*sig_receiver=*/0, sig), SUCCESS);
  }

  // Merged blobs should be usable for signing.
  std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers = {peers[0], peers[1], peers[2]};
  std::vector<std::shared_ptr<local_api_transport_t>> sign_transports = {transports[0], transports[1], transports[2]};
  std::vector<buf_t> sigs(quorum_party_names.size());
  run_mp(
      sign_peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), quorum_party_names, *sign_transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::sign_ac(job, merged[static_cast<size_t>(i)], ac, msg_hash, /*sig_receiver=*/0,
                                                sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  ASSERT_GT(sigs[0].size(), 0);
  for (size_t i = 1; i < sigs.size(); i++) EXPECT_EQ(sigs[i].size(), 0);

  // Negative: merging the wrong scalar should fail.
  buf_t Qi0;
  ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_share_compressed(key_blobs[0], Qi0), SUCCESS);
  buf_t bad_x = x_fixed[0];
  bad_x[0] ^= 0x01;
  buf_t bad_merged;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(redacted[0], bad_x, Qi0, bad_merged), SUCCESS);
}

TEST(ApiEcdsaMpAc, SignAcAndRefreshNegativeCases) {
  constexpr int n = 4;

  std::vector<std::shared_ptr<mpc_net_context_t>> peers = make_peers(n);
  std::vector<std::shared_ptr<local_api_transport_t>> transports = make_transports(peers);

  std::vector<std::string> names = {"p0", "p1", "p2", "p3"};
  std::vector<std::string_view> name_views;
  name_views.reserve(names.size());
  for (const auto& name : names) name_views.emplace_back(name);

  // Strict access structure requires all 4 parties to satisfy: AND(p0, p1, p2, p3).
  const coinbase::api::access_structure_t strict_ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
      coinbase::api::access_structure_t::leaf(names[2]),
      coinbase::api::access_structure_t::leaf(names[3]),
  });

  // DKG quorum must satisfy the access structure.
  const std::vector<std::string_view> dkg_quorum_party_names = {names[0], names[1], names[2], names[3]};

  std::vector<buf_t> key_blobs(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sids[static_cast<size_t>(i)], strict_ac,
                                               dkg_quorum_party_names, key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);

  const buf_t msg_hash = make_msg_hash32();

  // `sign_ac` should reject when the key blob does not match `job.self`.
  {
    const std::vector<std::string_view> quorum_missing_self = {names[1], names[2]};
    failing_transport_t t;
    job_mp_t job{/*self=*/0, quorum_missing_self, t};  // self == "p1"
    buf_t sig;
    EXPECT_EQ(coinbase::api::ecdsa_mp::sign_ac(job, key_blobs[0], strict_ac, msg_hash, /*sig_receiver=*/0, sig),
              E_BADARG);
  }

  // `sign_ac` should reject when the online signing quorum does not satisfy the access structure.
  {
    const std::vector<std::string_view> insufficient_quorum = {names[0], names[1]};
    failing_transport_t t;
    job_mp_t job{/*self=*/0, insufficient_quorum, t};
    buf_t sig;
    EXPECT_EQ(coinbase::api::ecdsa_mp::sign_ac(job, key_blobs[0], strict_ac, msg_hash, /*sig_receiver=*/0, sig),
              E_INSUFFICIENT);
  }

  // refresh_ac should reject additive (v1) key blobs.
  {
    const std::vector<std::string_view> quorum = {names[0], names[1]};

    // Produce an additive-share (v1) key blob via additive DKG.
    constexpr int quorum_n = 2;
    std::vector<std::shared_ptr<mpc_net_context_t>> dkg_peers = make_peers(quorum_n);
    std::vector<std::shared_ptr<local_api_transport_t>> dkg_transports = make_transports(dkg_peers);

    std::vector<buf_t> additive_key_blobs(quorum_n);
    std::vector<buf_t> additive_sids(quorum_n);
    run_mp(
        dkg_peers,
        [&](int i) {
          job_mp_t job{static_cast<party_idx_t>(i), quorum, *dkg_transports[static_cast<size_t>(i)]};
          return coinbase::api::ecdsa_mp::dkg_additive(job, curve_id::secp256k1,
                                                       additive_key_blobs[static_cast<size_t>(i)],
                                                       additive_sids[static_cast<size_t>(i)]);
        },
        rvs);
    for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);

    failing_transport_t t;
    job_mp_t job{/*self=*/0, quorum, t};

    // Any access structure will do since the call should fail at key blob deserialization.
    const coinbase::api::access_structure_t quorum_ac = coinbase::api::access_structure_t::And({
        coinbase::api::access_structure_t::leaf(quorum[0]),
        coinbase::api::access_structure_t::leaf(quorum[1]),
    });

    buf_t sid;
    buf_t new_key_blob;
    EXPECT_EQ(coinbase::api::ecdsa_mp::refresh_ac(job, sid, additive_key_blobs[0], quorum_ac, quorum, new_key_blob),
              E_FORMAT);
  }
}

TEST(ApiEcdsaMpAc, SignAcRejectsWrongAccessStructure) {
  constexpr int n = 4;

  std::vector<std::shared_ptr<mpc_net_context_t>> peers = make_peers(n);
  std::vector<std::shared_ptr<local_api_transport_t>> transports = make_transports(peers);

  std::vector<std::string> names = {"p0", "p1", "p2", "p3"};
  std::vector<std::string_view> name_views;
  name_views.reserve(names.size());
  for (const auto& name : names) name_views.emplace_back(name);

  // DKG access structure requires all parties: AND(p0, p1, p2, p3)
  const coinbase::api::access_structure_t strict_ac = coinbase::api::access_structure_t::And({
      coinbase::api::access_structure_t::leaf(names[0]),
      coinbase::api::access_structure_t::leaf(names[1]),
      coinbase::api::access_structure_t::leaf(names[2]),
      coinbase::api::access_structure_t::leaf(names[3]),
  });
  const std::vector<std::string_view> strict_quorum = {names[0], names[1], names[2], names[3]};

  std::vector<buf_t> key_blobs(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::dkg_ac(job, curve_id::secp256k1, sids[static_cast<size_t>(i)], strict_ac,
                                               strict_quorum, key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);

  // Try to "weaken" the policy by supplying a different access structure that allows a 2-of-4 quorum.
  const coinbase::api::access_structure_t wrong_ac =
      coinbase::api::access_structure_t::Threshold(2, {
                                                          coinbase::api::access_structure_t::leaf(names[0]),
                                                          coinbase::api::access_structure_t::leaf(names[1]),
                                                          coinbase::api::access_structure_t::leaf(names[2]),
                                                          coinbase::api::access_structure_t::leaf(names[3]),
                                                      });
  const std::vector<std::string_view> wrong_quorum = {names[0], names[1]};

  failing_transport_t t;
  job_mp_t job{/*self=*/0, wrong_quorum, t};
  buf_t sig_der;
  const buf_t msg_hash = make_msg_hash32();
  EXPECT_NE(coinbase::api::ecdsa_mp::sign_ac(job, key_blobs[0], wrong_ac, msg_hash, /*sig_receiver=*/0, sig_der),
            SUCCESS);
}
