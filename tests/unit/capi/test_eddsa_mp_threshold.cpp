#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/capi/access_structure.h>
#include <cbmpc/capi/eddsa_mp.h>
#include <cbmpc/capi/job.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/crypto/base_ecc.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;

using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::capi_harness::make_transport;
using coinbase::testutils::capi_harness::run_mp;
using coinbase::testutils::capi_harness::transport_ctx_t;

static void expect_eq(cmem_t a, cmem_t b) {
  ASSERT_EQ(a.size, b.size);
  if (a.size > 0) {
    ASSERT_NE(a.data, nullptr);
    ASSERT_NE(b.data, nullptr);
    ASSERT_EQ(std::memcmp(a.data, b.data, static_cast<size_t>(a.size)), 0);
  }
}

static void make_peers(int n, std::vector<std::shared_ptr<mpc_net_context_t>>& peers) {
  peers.clear();
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);
}

static void make_transports(const std::vector<std::shared_ptr<mpc_net_context_t>>& peers,
                            std::vector<transport_ctx_t>& ctxs, std::vector<cbmpc_transport_t>& transports) {
  ctxs.resize(peers.size());
  transports.resize(peers.size());
  for (size_t i = 0; i < peers.size(); i++) {
    ctxs[i] = transport_ctx_t{peers[i], /*free_calls=*/nullptr};
    transports[i] = make_transport(&ctxs[i]);
  }
}

}  // namespace

TEST(CApiEdDSAMpThreshold, DkgRefreshSign2of3) {
  constexpr int n = 3;

  // Full 3-party network for threshold DKG/refresh.
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  make_peers(n, peers);

  std::vector<transport_ctx_t> ctxs;
  std::vector<cbmpc_transport_t> transports;
  make_transports(peers, ctxs, transports);

  const char* party_names[n] = {"p0", "p1", "p2"};

  // Access structure: THRESHOLD[2](p0, p1, p2)
  const int32_t child_indices[] = {1, 2, 3};
  const cbmpc_access_structure_node_t nodes[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, /*leaf_name=*/nullptr, /*k=*/2, /*off=*/0, /*cnt=*/3},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p0", /*k=*/0, /*off=*/0, /*cnt=*/0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p1", /*k=*/0, /*off=*/0, /*cnt=*/0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p2", /*k=*/0, /*off=*/0, /*cnt=*/0},
  };
  const cbmpc_access_structure_t ac = {
      /*nodes=*/nodes,
      /*nodes_count=*/static_cast<int32_t>(sizeof(nodes) / sizeof(nodes[0])),
      /*child_indices=*/child_indices,
      /*child_indices_count=*/static_cast<int32_t>(sizeof(child_indices) / sizeof(child_indices[0])),
      /*root_index=*/0,
  };

  // Only p0 and p1 actively contribute to DKG/refresh.
  const char* quorum[] = {"p0", "p1"};

  std::vector<cmem_t> key_blobs(n, cmem_t{nullptr, 0});
  std::vector<cmem_t> sids(n, cmem_t{nullptr, 0});
  std::vector<cbmpc_error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        const cbmpc_mp_job_t job = {
            /*self=*/i,
            /*party_names=*/party_names,
            /*party_names_count=*/n,
            /*transport=*/&transports[static_cast<size_t>(i)],
        };
        return cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, /*sid_in=*/cmem_t{nullptr, 0}, &ac, quorum,
                                     /*quorum_party_names_count=*/2, &key_blobs[static_cast<size_t>(i)],
                                     &sids[static_cast<size_t>(i)]);
      },
      rvs);

  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  for (int i = 0; i < n; i++) {
    ASSERT_GT(key_blobs[static_cast<size_t>(i)].size, 0);
    ASSERT_GT(sids[static_cast<size_t>(i)].size, 0);
  }
  for (int i = 1; i < n; i++) expect_eq(sids[0], sids[static_cast<size_t>(i)]);

  cmem_t pub0{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_get_public_key_compressed(key_blobs[0], &pub0), CBMPC_SUCCESS);
  ASSERT_EQ(pub0.size, 32);
  for (int i = 1; i < n; i++) {
    cmem_t pub_i{nullptr, 0};
    ASSERT_EQ(cbmpc_eddsa_mp_get_public_key_compressed(key_blobs[static_cast<size_t>(i)], &pub_i), CBMPC_SUCCESS);
    expect_eq(pub_i, pub0);
    cbmpc_cmem_free(pub_i);
  }

  const buf_t pub_buf(pub0.data, pub0.size);
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_ed25519, pub_buf), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  uint8_t msg_bytes[32];
  for (int i = 0; i < 32; i++) msg_bytes[i] = static_cast<uint8_t>(0x11 + i);
  const cmem_t msg = {msg_bytes, 32};

  // Signing quorum: {p0, p1}
  const char* sign_party_names[2] = {"p0", "p1"};
  const cmem_t sign_key_blobs[2] = {key_blobs[0], key_blobs[1]};

  {
    std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers;
    make_peers(2, sign_peers);

    std::vector<transport_ctx_t> sign_ctxs;
    std::vector<cbmpc_transport_t> sign_transports;
    make_transports(sign_peers, sign_ctxs, sign_transports);

    std::vector<cmem_t> sigs(2, cmem_t{nullptr, 0});
    run_mp(
        sign_peers,
        [&](int i) {
          const cbmpc_mp_job_t job = {
              /*self=*/i,
              /*party_names=*/sign_party_names,
              /*party_names_count=*/2,
              /*transport=*/&sign_transports[static_cast<size_t>(i)],
          };
          return cbmpc_eddsa_mp_sign_ac(&job, sign_key_blobs[static_cast<size_t>(i)], &ac, msg, /*sig_receiver=*/0,
                                        &sigs[static_cast<size_t>(i)]);
        },
        rvs);

    for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
    ASSERT_EQ(sigs[0].size, 64);
    EXPECT_EQ(sigs[1].size, 0);
    ASSERT_EQ(verify_key.verify(buf_t(msg_bytes, 32), buf_t(sigs[0].data, sigs[0].size)), SUCCESS);

    for (auto m : sigs) cbmpc_cmem_free(m);
  }

  // Threshold refresh.
  std::vector<cmem_t> new_key_blobs(n, cmem_t{nullptr, 0});
  std::vector<cmem_t> refresh_sids(n, cmem_t{nullptr, 0});
  run_mp(
      peers,
      [&](int i) {
        const cbmpc_mp_job_t job = {
            /*self=*/i,
            /*party_names=*/party_names,
            /*party_names_count=*/n,
            /*transport=*/&transports[static_cast<size_t>(i)],
        };
        return cbmpc_eddsa_mp_refresh_ac(&job, /*sid_in=*/cmem_t{nullptr, 0}, key_blobs[static_cast<size_t>(i)], &ac,
                                         quorum, /*quorum_party_names_count=*/2, &refresh_sids[static_cast<size_t>(i)],
                                         &new_key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  for (int i = 0; i < n; i++) ASSERT_GT(new_key_blobs[static_cast<size_t>(i)].size, 0);
  for (int i = 1; i < n; i++) expect_eq(refresh_sids[0], refresh_sids[static_cast<size_t>(i)]);

  for (int i = 0; i < n; i++) {
    cmem_t pub_i{nullptr, 0};
    ASSERT_EQ(cbmpc_eddsa_mp_get_public_key_compressed(new_key_blobs[static_cast<size_t>(i)], &pub_i), CBMPC_SUCCESS);
    expect_eq(pub_i, pub0);
    cbmpc_cmem_free(pub_i);
  }

  const cmem_t sign_new_key_blobs[2] = {new_key_blobs[0], new_key_blobs[1]};

  {
    std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers;
    make_peers(2, sign_peers);

    std::vector<transport_ctx_t> sign_ctxs;
    std::vector<cbmpc_transport_t> sign_transports;
    make_transports(sign_peers, sign_ctxs, sign_transports);

    std::vector<cmem_t> sigs(2, cmem_t{nullptr, 0});
    run_mp(
        sign_peers,
        [&](int i) {
          const cbmpc_mp_job_t job = {
              /*self=*/i,
              /*party_names=*/sign_party_names,
              /*party_names_count=*/2,
              /*transport=*/&sign_transports[static_cast<size_t>(i)],
          };
          return cbmpc_eddsa_mp_sign_ac(&job, sign_new_key_blobs[static_cast<size_t>(i)], &ac, msg, /*sig_receiver=*/0,
                                        &sigs[static_cast<size_t>(i)]);
        },
        rvs);

    for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
    ASSERT_EQ(sigs[0].size, 64);
    EXPECT_EQ(sigs[1].size, 0);
    ASSERT_EQ(verify_key.verify(buf_t(msg_bytes, 32), buf_t(sigs[0].data, sigs[0].size)), SUCCESS);

    for (auto m : sigs) cbmpc_cmem_free(m);
  }

  cbmpc_cmem_free(pub0);
  for (auto m : refresh_sids) cbmpc_cmem_free(m);
  for (auto m : new_key_blobs) cbmpc_cmem_free(m);
  for (auto m : sids) cbmpc_cmem_free(m);
  for (auto m : key_blobs) cbmpc_cmem_free(m);
}
