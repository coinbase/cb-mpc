#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/c_api/access_structure.h>
#include <cbmpc/c_api/eddsa_mp.h>
#include <cbmpc/c_api/job.h>
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

TEST(CApiEdDSAMpAc, DkgRefreshSign2of3) {
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

// ------------ Disclaimer: All the following tests have been generated by AI ------------

// ===========================================================================
// Helpers for negative tests
// ===========================================================================

namespace {

static const cbmpc_transport_t noop_transport = {
    nullptr,
    [](void*, int32_t, const uint8_t*, int) -> cbmpc_error_t { return E_GENERAL; },
    [](void*, int32_t, cmem_t*) -> cbmpc_error_t { return E_GENERAL; },
    [](void*, const int32_t*, int, cmems_t*) -> cbmpc_error_t { return E_GENERAL; },
    nullptr,
};

static cbmpc_access_structure_t make_simple_ac_2of3() {
  static const int32_t ci[] = {1, 2, 3};
  static const cbmpc_access_structure_node_t nd[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, nullptr, 2, 0, 3},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p1", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p2", 0, 0, 0},
  };
  return {nd, 4, ci, 3, 0};
}

static void capi_generate_eddsa_ac_key_blobs(int n, std::vector<cmem_t>& blobs) {
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  make_peers(n, peers);

  std::vector<transport_ctx_t> ctxs;
  std::vector<cbmpc_transport_t> transports;
  make_transports(peers, ctxs, transports);

  std::vector<std::string> names;
  for (int i = 0; i < n; i++) names.push_back("p" + std::to_string(i));
  std::vector<const char*> name_ptrs;
  for (const auto& nm : names) name_ptrs.push_back(nm.c_str());

  const cbmpc_access_structure_t ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};

  blobs.resize(n, cmem_t{nullptr, 0});
  std::vector<cmem_t> sids(n, cmem_t{nullptr, 0});
  std::vector<cbmpc_error_t> rvs;
  run_mp(
      peers,
      [&](int i) {
        const cbmpc_mp_job_t job = {i, name_ptrs.data(), n, &transports[static_cast<size_t>(i)]};
        return cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2,
                                     &blobs[static_cast<size_t>(i)], &sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  for (auto m : sids) cbmpc_cmem_free(m);
}

}  // namespace

class CApiEdDSAMpAcNegWithBlobs : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { capi_generate_eddsa_ac_key_blobs(3, blobs_); }
  static void TearDownTestSuite() {
    for (auto m : blobs_) cbmpc_cmem_free(m);
    blobs_.clear();
  }
  static std::vector<cmem_t> blobs_;
};
std::vector<cmem_t> CApiEdDSAMpAcNegWithBlobs::blobs_;

// ===========================================================================
// Negative: DKG AC
// ===========================================================================

TEST(CApiEdDSAMpAc, NegDkgAcNullOutKey) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, nullptr, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcNullOutSid) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, nullptr),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcNullJob) {
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_dkg_ac(nullptr, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            CBMPC_SUCCESS);
}

TEST(CApiEdDSAMpAc, NegDkgAcJobNullTransport) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, nullptr};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcInvalidCurveSecp256k1) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_SECP256K1, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            CBMPC_SUCCESS);
}

TEST(CApiEdDSAMpAc, NegDkgAcInvalidCurveP256) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_P256, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            CBMPC_SUCCESS);
}

TEST(CApiEdDSAMpAc, NegDkgAcInvalidCurveZero) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_dkg_ac(&job, static_cast<cbmpc_curve_id_t>(0), cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key,
                                  &out_sid),
            CBMPC_SUCCESS);
}

TEST(CApiEdDSAMpAc, NegDkgAcInvalidCurve4) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_dkg_ac(&job, static_cast<cbmpc_curve_id_t>(4), cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key,
                                  &out_sid),
            CBMPC_SUCCESS);
}

TEST(CApiEdDSAMpAc, NegDkgAcInvalidCurve255) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_dkg_ac(&job, static_cast<cbmpc_curve_id_t>(255), cmem_t{nullptr, 0}, &ac, quorum, 2,
                                  &out_key, &out_sid),
            CBMPC_SUCCESS);
}

TEST(CApiEdDSAMpAc, NegDkgAcNullAccessStructure) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(
      cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, nullptr, quorum, 2, &out_key, &out_sid),
      E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcNodesNull) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  const cbmpc_access_structure_t ac = {nullptr, 4, nullptr, 0, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcNodesCountZero) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  const cbmpc_access_structure_node_t dummy_node = {CBMPC_ACCESS_STRUCTURE_NODE_AND, nullptr, 0, 0, 0};
  const cbmpc_access_structure_t ac = {&dummy_node, 0, nullptr, 0, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcNodesCountNegative) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  const cbmpc_access_structure_node_t dummy_node = {CBMPC_ACCESS_STRUCTURE_NODE_AND, nullptr, 0, 0, 0};
  const cbmpc_access_structure_t ac = {&dummy_node, -1, nullptr, 0, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcChildIndicesNullWithPositiveCount) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  const cbmpc_access_structure_node_t nodes[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, nullptr, 2, 0, 3},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p1", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p2", 0, 0, 0},
  };
  const cbmpc_access_structure_t ac = {nodes, 4, nullptr, 3, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcChildIndicesCountNegative) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  const int32_t ci[] = {1, 2, 3};
  const cbmpc_access_structure_node_t nodes[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, nullptr, 2, 0, 3},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p1", 0, 0, 0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p2", 0, 0, 0},
  };
  const cbmpc_access_structure_t ac = {nodes, 4, ci, -1, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcRootIndexNegative) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac_base = make_simple_ac_2of3();
  const cbmpc_access_structure_t ac = {ac_base.nodes, ac_base.nodes_count, ac_base.child_indices,
                                       ac_base.child_indices_count, -1};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcRootIndexTooLarge) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac_base = make_simple_ac_2of3();
  const cbmpc_access_structure_t ac = {ac_base.nodes, ac_base.nodes_count, ac_base.child_indices,
                                       ac_base.child_indices_count, 999};
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcNullQuorum) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, nullptr, 2, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcNegativeQuorumCount) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, -1, &out_key, &out_sid),
            E_BADARG);
}

TEST(CApiEdDSAMpAc, NegDkgAcZeroQuorumCount) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t out_key{nullptr, 0};
  cmem_t out_sid{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_dkg_ac(&job, CBMPC_CURVE_ED25519, cmem_t{nullptr, 0}, &ac, quorum, 0, &out_key, &out_sid),
            CBMPC_SUCCESS);
}

// ===========================================================================
// Negative: Sign AC
// ===========================================================================

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcNullSigOut) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  EXPECT_EQ(cbmpc_eddsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{msg, 32}, 0, nullptr), E_BADARG);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcNullJob) {
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_ac(nullptr, blobs_[0], &ac, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcJobNullTransport) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, nullptr};
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{msg, 32}, 0, &sig), E_BADARG);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcNullAccessStructure) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  uint8_t msg[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_sign_ac(&job, blobs_[0], nullptr, cmem_t{msg, 32}, 0, &sig), E_BADARG);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcGarbageKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_ac(&job, cmem_t{garbage, 4}, &ac, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcEmptyKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_ac(&job, cmem_t{nullptr, 0}, &ac, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcAllZeroKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  uint8_t zeros[64] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_ac(&job, cmem_t{zeros, 64}, &ac, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcNegativeSizeKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  uint8_t data[] = {0x01};
  cmem_t sig{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_sign_ac(&job, cmem_t{data, -1}, &ac, cmem_t{msg, 32}, 0, &sig), E_BADARG);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcOversizedKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  std::vector<uint8_t> huge(1024 * 1024, 0x42);
  cmem_t sig{nullptr, 0};
  EXPECT_NE(
      cbmpc_eddsa_mp_sign_ac(&job, cmem_t{huge.data(), static_cast<int>(huge.size())}, &ac, cmem_t{msg, 32}, 0, &sig),
      CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcEmptyMsg) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{nullptr, 0}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcNegativeSizeMsg) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{msg, -1}, 0, &sig), E_BADARG);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcSigReceiverNegative) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{msg, 32}, -1, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegSignAcSigReceiverTooLarge) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  uint8_t msg[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_ac(&job, blobs_[0], &ac, cmem_t{msg, 32}, 999, &sig), CBMPC_SUCCESS);
}

// ===========================================================================
// Negative: Refresh AC
// ===========================================================================

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegRefreshAcNullOutNewKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, blobs_[0], &ac, quorum, 2, &sid_out, nullptr),
            E_BADARG);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegRefreshAcNullJob) {
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_refresh_ac(nullptr, cmem_t{nullptr, 0}, blobs_[0], &ac, quorum, 2, &sid_out, &out_new),
            CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegRefreshAcJobNullTransport) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, nullptr};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, blobs_[0], &ac, quorum, 2, &sid_out, &out_new),
            E_BADARG);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegRefreshAcNullAccessStructure) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, blobs_[0], nullptr, quorum, 2, &sid_out, &out_new),
            E_BADARG);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegRefreshAcGarbageKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, cmem_t{garbage, 4}, &ac, quorum, 2, &sid_out, &out_new),
            CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegRefreshAcEmptyKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, &ac, quorum, 2, &sid_out, &out_new),
            CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegRefreshAcAllZeroKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  uint8_t zeros[64] = {};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, cmem_t{zeros, 64}, &ac, quorum, 2, &sid_out, &out_new),
            CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegRefreshAcOversizedKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  std::vector<uint8_t> huge(1024 * 1024, 0x42);
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, cmem_t{huge.data(), static_cast<int>(huge.size())}, &ac,
                                      quorum, 2, &sid_out, &out_new),
            CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegRefreshAcNullQuorum) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, blobs_[0], &ac, nullptr, 2, &sid_out, &out_new),
            E_BADARG);
}

TEST_F(CApiEdDSAMpAcNegWithBlobs, NegRefreshAcNegativeQuorumCount) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_transport};
  const auto ac = make_simple_ac_2of3();
  const char* quorum[] = {"p0", "p1"};
  cmem_t sid_out{nullptr, 0};
  cmem_t out_new{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_refresh_ac(&job, cmem_t{nullptr, 0}, blobs_[0], &ac, quorum, -1, &sid_out, &out_new),
            E_BADARG);
}
