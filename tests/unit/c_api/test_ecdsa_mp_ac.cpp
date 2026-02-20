#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/c_api/access_structure.h>
#include <cbmpc/c_api/ecdsa_mp.h>
#include <cbmpc/c_api/job.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/crypto/base_ecc.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;
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

TEST(CApiEcdsaMpAc, ComplexAccess_DkgRefreshSign4p) {
  constexpr int n = 4;

  // Full 4-party network for threshold DKG/refresh.
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  make_peers(n, peers);

  std::vector<transport_ctx_t> ctxs;
  std::vector<cbmpc_transport_t> transports;
  make_transports(peers, ctxs, transports);

  const char* party_names[n] = {"p0", "p1", "p2", "p3"};

  // Access structure:
  // THRESHOLD[2](
  //   AND(p0, p1),
  //   OR(p2, p3)
  // )
  const int32_t child_indices[] = {1, 2, 3, 4, 5, 6};
  const cbmpc_access_structure_node_t nodes[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, /*leaf_name=*/nullptr, /*k=*/2, /*off=*/0, /*cnt=*/2},
      {CBMPC_ACCESS_STRUCTURE_NODE_AND, /*leaf_name=*/nullptr, /*k=*/0, /*off=*/2, /*cnt=*/2},
      {CBMPC_ACCESS_STRUCTURE_NODE_OR, /*leaf_name=*/nullptr, /*k=*/0, /*off=*/4, /*cnt=*/2},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p0", /*k=*/0, /*off=*/0, /*cnt=*/0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p1", /*k=*/0, /*off=*/0, /*cnt=*/0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p2", /*k=*/0, /*off=*/0, /*cnt=*/0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p3", /*k=*/0, /*off=*/0, /*cnt=*/0},
  };
  const cbmpc_access_structure_t ac = {
      /*nodes=*/nodes,
      /*nodes_count=*/static_cast<int32_t>(sizeof(nodes) / sizeof(nodes[0])),
      /*child_indices=*/child_indices,
      /*child_indices_count=*/static_cast<int32_t>(sizeof(child_indices) / sizeof(child_indices[0])),
      /*root_index=*/0,
  };

  // DKG quorum must satisfy the access structure. Use {p0, p1, p2}.
  const char* dkg_quorum[] = {"p0", "p1", "p2"};

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
        return cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1,
                                     /*sid_in=*/cmem_t{nullptr, 0}, &ac, dkg_quorum,
                                     /*quorum_party_names_count=*/3, &key_blobs[static_cast<size_t>(i)],
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
  ASSERT_EQ(cbmpc_ecdsa_mp_get_public_key_compressed(key_blobs[0], &pub0), CBMPC_SUCCESS);
  ASSERT_EQ(pub0.size, 33);
  for (int i = 1; i < n; i++) {
    cmem_t pub_i{nullptr, 0};
    ASSERT_EQ(cbmpc_ecdsa_mp_get_public_key_compressed(key_blobs[static_cast<size_t>(i)], &pub_i), CBMPC_SUCCESS);
    expect_eq(pub_i, pub0);
    cbmpc_cmem_free(pub_i);
  }

  const buf_t pub_buf(pub0.data, pub0.size);
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_secp256k1, pub_buf), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  uint8_t msg_hash_bytes[32];
  for (int i = 0; i < 32; i++) msg_hash_bytes[i] = static_cast<uint8_t>(i);
  const cmem_t msg_hash = {msg_hash_bytes, 32};

  // Signing quorum A: {p0, p1, p2}
  const char* quorum_a[] = {"p0", "p1", "p2"};
  const cmem_t quorum_a_key_blobs[] = {key_blobs[0], key_blobs[1], key_blobs[2]};

  {
    std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers;
    make_peers(3, sign_peers);

    std::vector<transport_ctx_t> sign_ctxs;
    std::vector<cbmpc_transport_t> sign_transports;
    make_transports(sign_peers, sign_ctxs, sign_transports);

    std::vector<cmem_t> sigs(3, cmem_t{nullptr, 0});
    run_mp(
        sign_peers,
        [&](int i) {
          const cbmpc_mp_job_t job = {
              /*self=*/i,
              /*party_names=*/quorum_a,
              /*party_names_count=*/3,
              /*transport=*/&sign_transports[static_cast<size_t>(i)],
          };
          return cbmpc_ecdsa_mp_sign_ac(&job, quorum_a_key_blobs[static_cast<size_t>(i)], &ac, msg_hash,
                                        /*sig_receiver=*/0, &sigs[static_cast<size_t>(i)]);
        },
        rvs);
    for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
    ASSERT_GT(sigs[0].size, 0);
    EXPECT_EQ(sigs[1].size, 0);
    EXPECT_EQ(sigs[2].size, 0);
    ASSERT_EQ(verify_key.verify(buf_t(msg_hash_bytes, 32), buf_t(sigs[0].data, sigs[0].size)), SUCCESS);

    for (auto m : sigs) cbmpc_cmem_free(m);
  }

  // Signing quorum B: {p0, p1, p3}
  const char* quorum_b[] = {"p0", "p1", "p3"};
  const cmem_t quorum_b_key_blobs[] = {key_blobs[0], key_blobs[1], key_blobs[3]};

  {
    std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers;
    make_peers(3, sign_peers);

    std::vector<transport_ctx_t> sign_ctxs;
    std::vector<cbmpc_transport_t> sign_transports;
    make_transports(sign_peers, sign_ctxs, sign_transports);

    std::vector<cmem_t> sigs(3, cmem_t{nullptr, 0});
    run_mp(
        sign_peers,
        [&](int i) {
          const cbmpc_mp_job_t job = {
              /*self=*/i,
              /*party_names=*/quorum_b,
              /*party_names_count=*/3,
              /*transport=*/&sign_transports[static_cast<size_t>(i)],
          };
          return cbmpc_ecdsa_mp_sign_ac(&job, quorum_b_key_blobs[static_cast<size_t>(i)], &ac, msg_hash,
                                        /*sig_receiver=*/0, &sigs[static_cast<size_t>(i)]);
        },
        rvs);
    for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
    ASSERT_GT(sigs[0].size, 0);
    EXPECT_EQ(sigs[1].size, 0);
    EXPECT_EQ(sigs[2].size, 0);
    ASSERT_EQ(verify_key.verify(buf_t(msg_hash_bytes, 32), buf_t(sigs[0].data, sigs[0].size)), SUCCESS);

    for (auto m : sigs) cbmpc_cmem_free(m);
  }

  // Threshold refresh with quorum B.
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
        return cbmpc_ecdsa_mp_refresh_ac(&job,
                                         /*sid_in=*/cmem_t{nullptr, 0}, key_blobs[static_cast<size_t>(i)], &ac,
                                         quorum_b,
                                         /*quorum_party_names_count=*/3, &refresh_sids[static_cast<size_t>(i)],
                                         &new_key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  for (int i = 1; i < n; i++) expect_eq(refresh_sids[0], refresh_sids[static_cast<size_t>(i)]);

  for (int i = 0; i < n; i++) {
    cmem_t pub_i{nullptr, 0};
    ASSERT_EQ(cbmpc_ecdsa_mp_get_public_key_compressed(new_key_blobs[static_cast<size_t>(i)], &pub_i), CBMPC_SUCCESS);
    expect_eq(pub_i, pub0);
    cbmpc_cmem_free(pub_i);
  }

  const cmem_t quorum_b_new_key_blobs[] = {new_key_blobs[0], new_key_blobs[1], new_key_blobs[3]};

  {
    std::vector<std::shared_ptr<mpc_net_context_t>> sign_peers;
    make_peers(3, sign_peers);

    std::vector<transport_ctx_t> sign_ctxs;
    std::vector<cbmpc_transport_t> sign_transports;
    make_transports(sign_peers, sign_ctxs, sign_transports);

    std::vector<cmem_t> sigs(3, cmem_t{nullptr, 0});
    run_mp(
        sign_peers,
        [&](int i) {
          const cbmpc_mp_job_t job = {
              /*self=*/i,
              /*party_names=*/quorum_b,
              /*party_names_count=*/3,
              /*transport=*/&sign_transports[static_cast<size_t>(i)],
          };
          return cbmpc_ecdsa_mp_sign_ac(&job, quorum_b_new_key_blobs[static_cast<size_t>(i)], &ac, msg_hash,
                                        /*sig_receiver=*/0, &sigs[static_cast<size_t>(i)]);
        },
        rvs);
    for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
    ASSERT_GT(sigs[0].size, 0);
    EXPECT_EQ(sigs[1].size, 0);
    EXPECT_EQ(sigs[2].size, 0);
    ASSERT_EQ(verify_key.verify(buf_t(msg_hash_bytes, 32), buf_t(sigs[0].data, sigs[0].size)), SUCCESS);

    for (auto m : sigs) cbmpc_cmem_free(m);
  }

  cbmpc_cmem_free(pub0);
  for (auto m : refresh_sids) cbmpc_cmem_free(m);
  for (auto m : new_key_blobs) cbmpc_cmem_free(m);
  for (auto m : sids) cbmpc_cmem_free(m);
  for (auto m : key_blobs) cbmpc_cmem_free(m);
}

TEST(CApiEcdsaMpAc, RejectInvalidAccessStructEncoding) {
  // Dummy transport (won't be used; inputs fail before any I/O).
  const cbmpc_transport_t transport = {
      /*ctx=*/nullptr,
      /*send=*/[](void*, int32_t, const uint8_t*, int) -> cbmpc_error_t { return E_GENERAL; },
      /*receive=*/[](void*, int32_t, cmem_t*) -> cbmpc_error_t { return E_GENERAL; },
      /*receive_all=*/[](void*, const int32_t*, int, cmems_t*) -> cbmpc_error_t { return E_GENERAL; },
      /*free=*/nullptr,
  };

  const char* party_names[2] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {
      /*self=*/0,
      /*party_names=*/party_names,
      /*party_names_count=*/2,
      /*transport=*/&transport,
  };

  const char* quorum[] = {"p0", "p1"};

  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};

  // Root leaf is rejected.
  {
    const cbmpc_access_structure_node_t nodes[] = {
        {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p0", /*k=*/0, /*off=*/0, /*cnt=*/0},
    };
    const cbmpc_access_structure_t ac = {
        /*nodes=*/nodes,
        /*nodes_count=*/1,
        /*child_indices=*/nullptr,
        /*child_indices_count=*/0,
        /*root_index=*/0,
    };
    EXPECT_EQ(
        cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
        E_BADARG);
    EXPECT_EQ(out_key.data, nullptr);
    EXPECT_EQ(out_key.size, 0);
    EXPECT_EQ(out_sid.data, nullptr);
    EXPECT_EQ(out_sid.size, 0);
  }

  // Cycle (node 0 references itself).
  {
    const int32_t child_indices[] = {0};
    const cbmpc_access_structure_node_t nodes[] = {
        {CBMPC_ACCESS_STRUCTURE_NODE_AND, /*leaf_name=*/nullptr, /*k=*/0, /*off=*/0, /*cnt=*/1},
    };
    const cbmpc_access_structure_t ac = {
        /*nodes=*/nodes,
        /*nodes_count=*/1,
        /*child_indices=*/child_indices,
        /*child_indices_count=*/1,
        /*root_index=*/0,
    };
    EXPECT_EQ(
        cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
        E_BADARG);
  }
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

// ---------------------------------------------------------------------------
// Helpers for negative tests
// ---------------------------------------------------------------------------

namespace {

static const cbmpc_transport_t noop_transport = {
    nullptr,
    [](void*, int32_t, const uint8_t*, int) -> cbmpc_error_t { return E_GENERAL; },
    [](void*, int32_t, cmem_t*) -> cbmpc_error_t { return E_GENERAL; },
    [](void*, const int32_t*, int, cmems_t*) -> cbmpc_error_t { return E_GENERAL; },
    nullptr,
};

static cbmpc_access_structure_t make_simple_ac_2of4() {
  static const int32_t ci[] = {1, 2, 3, 4};
  static const cbmpc_access_structure_node_t nd[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, nullptr, 2, 0, 4}, {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p1", 0, 0, 0},         {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p2", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p3", 0, 0, 0},
  };
  return {nd, 5, ci, 4, 0};
}

static void capi_generate_mp_ac_key_blobs(cbmpc_curve_id_t curve, int n, std::vector<cmem_t>& blobs) {
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  make_peers(n, peers);

  std::vector<transport_ctx_t> ctxs;
  std::vector<cbmpc_transport_t> transports;
  make_transports(peers, ctxs, transports);

  std::vector<std::string> names;
  for (int i = 0; i < n; i++) names.push_back("p" + std::to_string(i));
  std::vector<const char*> name_ptrs;
  for (const auto& nm : names) name_ptrs.push_back(nm.c_str());

  const int32_t child_indices[] = {1, 2, 3, 4};
  const cbmpc_access_structure_node_t nodes[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, nullptr, 2, 0, 4}, {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p1", 0, 0, 0},         {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p2", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p3", 0, 0, 0},
  };
  const cbmpc_access_structure_t ac = {nodes, 5, child_indices, 4, 0};

  const char* quorum[] = {"p0", "p1"};

  blobs.resize(n, cmem_t{nullptr, 0});
  std::vector<cmem_t> sids(n, cmem_t{nullptr, 0});
  std::vector<cbmpc_error_t> rvs;
  run_mp(
      peers,
      [&](int i) {
        const cbmpc_mp_job_t job = {i, name_ptrs.data(), n, &transports[static_cast<size_t>(i)]};
        return cbmpc_ecdsa_mp_dkg_ac(&job, curve, cmem_t{nullptr, 0}, &ac, quorum, 2, &blobs[static_cast<size_t>(i)],
                                     &sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  for (auto m : sids) cbmpc_cmem_free(m);
}

}  // namespace

class CApiEcdsaMpAcNegWithBlobs : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { capi_generate_mp_ac_key_blobs(CBMPC_CURVE_SECP256K1, 4, blobs_); }
  static void TearDownTestSuite() {
    for (auto m : blobs_) cbmpc_cmem_free(m);
    blobs_.clear();
  }
  static std::vector<cmem_t> blobs_;
};
std::vector<cmem_t> CApiEcdsaMpAcNegWithBlobs::blobs_;

// ===========================================================================
// Negative: DKG AC
// ===========================================================================

TEST(CApiEcdsaMpAc, NegDkgAcNullOutKey) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, nullptr, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcNullOutSid) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, nullptr),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcNullJob) {
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(
      cbmpc_ecdsa_mp_dkg_ac(nullptr, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
      CBMPC_SUCCESS);
}

TEST(CApiEcdsaMpAc, NegDkgAcJobNullTransport) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, nullptr};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcInvalidCurveEd25519) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            CBMPC_SUCCESS);
}

TEST(CApiEcdsaMpAc, NegDkgAcInvalidCurveZero) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_dkg_ac(&job, static_cast<cbmpc_curve_id_t>(0), cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key,
                                  &out_sid),
            CBMPC_SUCCESS);
}

TEST(CApiEcdsaMpAc, NegDkgAcInvalidCurve4) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_dkg_ac(&job, static_cast<cbmpc_curve_id_t>(4), cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key,
                                  &out_sid),
            CBMPC_SUCCESS);
}

TEST(CApiEcdsaMpAc, NegDkgAcInvalidCurve255) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_dkg_ac(&job, static_cast<cbmpc_curve_id_t>(255), cmem_t{nullptr, 0}, &ac, quorum, 2,
                                  &out_key, &out_sid),
            CBMPC_SUCCESS);
}

TEST(CApiEcdsaMpAc, NegDkgAcNullAccessStructure) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(
      cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, nullptr, quorum, 2, &out_key, &out_sid),
      E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcNodesNull) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  const cbmpc_access_structure_t ac = {nullptr, 5, nullptr, 0, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcNodesCountZero) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  const cbmpc_access_structure_node_t dummy_node = {CBMPC_ACCESS_STRUCTURE_NODE_AND, nullptr, 0, 0, 0};
  const cbmpc_access_structure_t ac = {&dummy_node, 0, nullptr, 0, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcNodesCountNegative) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  const cbmpc_access_structure_node_t dummy_node = {CBMPC_ACCESS_STRUCTURE_NODE_AND, nullptr, 0, 0, 0};
  const cbmpc_access_structure_t ac = {&dummy_node, -1, nullptr, 0, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcChildIndicesNullWithPositiveCount) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  const cbmpc_access_structure_node_t nodes[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, nullptr, 2, 0, 4}, {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p1", 0, 0, 0},         {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p2", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p3", 0, 0, 0},
  };
  const cbmpc_access_structure_t ac = {nodes, 5, nullptr, 4, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcChildIndicesCountNegative) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  const int32_t ci[] = {1, 2, 3, 4};
  const cbmpc_access_structure_node_t nodes[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, nullptr, 2, 0, 4}, {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p1", 0, 0, 0},         {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p2", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p3", 0, 0, 0},
  };
  const cbmpc_access_structure_t ac = {nodes, 5, ci, -1, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcRootIndexNegative) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac_base = make_simple_ac_2of4();
  const cbmpc_access_structure_t ac = {ac_base.nodes, ac_base.nodes_count, ac_base.child_indices,
                                       ac_base.child_indices_count, -1};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcRootIndexTooLarge) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac_base = make_simple_ac_2of4();
  const cbmpc_access_structure_t ac = {ac_base.nodes, ac_base.nodes_count, ac_base.child_indices,
                                       ac_base.child_indices_count, 999};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcNullQuorum) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, nullptr, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcNegativeQuorumCount) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, -1, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEcdsaMpAc, NegDkgAcZeroQuorumCount) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 0, &out_key, &out_sid),
            CBMPC_SUCCESS);
}

// ===========================================================================
// Negative: Sign AC
// ===========================================================================

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcNullSigDerOut) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  EXPECT_EQ(cbmpc_ecdsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{hash, 32}, 0, nullptr), E_BADARG);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcNullJob) {
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_sign_ac(nullptr, blobs_[0], &ac, cmem_t{hash, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcJobNullTransport) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, nullptr};
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{hash, 32}, 0, &sig), E_BADARG);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcNullAccessStructure) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  uint8_t hash[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_sign_ac(&job, blobs_[0], nullptr, cmem_t{hash, 32}, 0, &sig), E_BADARG);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcGarbageKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_sign_ac(&job, cmem_t{garbage, 4}, &ac, cmem_t{hash, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcEmptyKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_sign_ac(&job, cmem_t{nullptr, 0}, &ac, cmem_t{hash, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcAllZeroKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  uint8_t zeros[64] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_sign_ac(&job, cmem_t{zeros, 64}, &ac, cmem_t{hash, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcNegativeSizeKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  uint8_t data[] = {0x01};
  cmem_t sig{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_sign_ac(&job, cmem_t{data, -1}, &ac, cmem_t{hash, 32}, 0, &sig), E_BADARG);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcOversizedKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  std::vector<uint8_t> huge(1024 * 1024, 0x42);
  cmem_t sig{nullptr, 0};
  EXPECT_NE(
      cbmpc_ecdsa_mp_sign_ac(&job, cmem_t{huge.data(), static_cast<int>(huge.size())}, &ac, cmem_t{hash, 32}, 0, &sig),
      CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcEmptyMsgHash) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{nullptr, 0}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcGarbageMsgHash) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t garbage[] = {0xDE, 0xAD};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{garbage, 2}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcNegativeSizeMsgHash) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{hash, -1}, 0, &sig), E_BADARG);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcOversizedMsgHash) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t huge_hash[65];
  std::memset(huge_hash, 0x42, sizeof(huge_hash));
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{huge_hash, 65}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcSigReceiverNegative) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{hash, 32}, -1, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegSignAcSigReceiverTooLarge) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  uint8_t hash[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{hash, 32}, 999, &sig), CBMPC_SUCCESS);
}

// ===========================================================================
// Negative: Refresh AC
// ===========================================================================

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegRefreshAcNullOutNewKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, blobs_[0], &ac, quorum, 2, &sid_out, nullptr),
            E_BADARG);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegRefreshAcNullJob) {
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_refresh_ac(nullptr, cmem_t{nullptr, 0}, blobs_[0], &ac, quorum, 2, &sid_out, &out_new),
            CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegRefreshAcJobNullTransport) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, nullptr};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, blobs_[0], &ac, quorum, 2, &sid_out, &out_new),
            E_BADARG);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegRefreshAcNullAccessStructure) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, blobs_[0], nullptr, quorum, 2, &sid_out, &out_new),
            E_BADARG);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegRefreshAcGarbageKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, cmem_t{garbage, 4}, &ac, quorum, 2, &sid_out, &out_new),
            CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegRefreshAcEmptyKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, &ac, quorum, 2, &sid_out, &out_new),
            CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegRefreshAcAllZeroKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  uint8_t zeros[64] = {};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, cmem_t{zeros, 64}, &ac, quorum, 2, &sid_out, &out_new),
            CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegRefreshAcOversizedKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  std::vector<uint8_t> huge(1024 * 1024, 0x42);
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, cmem_t{huge.data(), static_cast<int>(huge.size())}, &ac,
                                      quorum, 2, &sid_out, &out_new),
            CBMPC_SUCCESS);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegRefreshAcNullQuorum) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, blobs_[0], &ac, nullptr, 2, &sid_out, &out_new),
            E_BADARG);
}

TEST_F(CApiEcdsaMpAcNegWithBlobs, NegRefreshAcNegativeQuorumCount) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of4();
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_EQ(cbmpc_ecdsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, blobs_[0], &ac, quorum, -1, &sid_out, &out_new),
            E_BADARG);
}
