#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/c_api/access_structure.h>
#include <cbmpc/c_api/ecdsa_mp.h>
#include <cbmpc/c_api/eddsa_mp.h>
#include <cbmpc/c_api/job.h>
#include <cbmpc/c_api/schnorr_mp.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/crypto/base.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::converter_t;
using coinbase::mem_t;
using coinbase::crypto::bn_t;
using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::capi_harness::make_transport;
using coinbase::testutils::capi_harness::run_mp;
using coinbase::testutils::capi_harness::transport_ctx_t;

struct key_blob_v1_t {
  uint32_t version = 0;
  uint32_t curve = 0;
  std::string party_name;
  buf_t Q_compressed;
  std::map<std::string, buf_t> Qis_compressed;
  bn_t x_share;

  void convert(converter_t& c) { c.convert(version, curve, party_name, Q_compressed, Qis_compressed, x_share); }
};

struct api_ops_t {
  cbmpc_curve_id_t curve;
  cbmpc_error_t (*dkg_ac)(const cbmpc_mp_job_t*, cbmpc_curve_id_t, cmem_t, const cbmpc_access_structure_t*,
                          const char* const*, int, cmem_t*, cmem_t*);
  cbmpc_error_t (*get_public_key)(cmem_t, cmem_t*);
  cbmpc_error_t (*get_public_share)(cmem_t, cmem_t*);
  cbmpc_error_t (*detach_private_scalar)(cmem_t, cmem_t*, cmem_t*);
  cbmpc_error_t (*attach_private_scalar)(cmem_t, cmem_t, cmem_t, cmem_t*);
};

static cbmpc_access_structure_t make_simple_and_ac() {
  static const int32_t child_indices[] = {1, 2};
  static const cbmpc_access_structure_node_t nodes[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_AND, /*leaf_name=*/nullptr, /*k=*/0, /*off=*/0, /*cnt=*/2},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p0", /*k=*/0, /*off=*/0, /*cnt=*/0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p1", /*k=*/0, /*off=*/0, /*cnt=*/0},
  };
  return cbmpc_access_structure_t{
      nodes,
      static_cast<int32_t>(sizeof(nodes) / sizeof(nodes[0])),
      child_indices,
      static_cast<int32_t>(sizeof(child_indices) / sizeof(child_indices[0])),
      0,
  };
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

static void generate_ac_key_blobs(const api_ops_t& api, std::vector<cmem_t>& out_key_blobs) {
  constexpr int n = 2;
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  make_peers(n, peers);

  std::vector<transport_ctx_t> ctxs;
  std::vector<cbmpc_transport_t> transports;
  make_transports(peers, ctxs, transports);

  const char* party_names[n] = {"p0", "p1"};
  const char* quorum[n] = {"p0", "p1"};
  const cbmpc_access_structure_t ac = make_simple_and_ac();

  out_key_blobs.assign(n, cmem_t{nullptr, 0});
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
        return api.dkg_ac(&job, api.curve, /*sid_in=*/cmem_t{nullptr, 0}, &ac, quorum, n,
                          &out_key_blobs[static_cast<size_t>(i)], &sids[static_cast<size_t>(i)]);
      },
      rvs);

  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  for (auto sid : sids) cbmpc_cmem_free(sid);
}

static buf_t tamper_blob_public_key(cmem_t key_blob, cmem_t replacement_pub_key) {
  key_blob_v1_t blob;
  EXPECT_EQ(coinbase::deser(mem_t(key_blob.data, key_blob.size), blob), SUCCESS);
  blob.Q_compressed = buf_t(replacement_pub_key.data, replacement_pub_key.size);
  return coinbase::convert(blob);
}

static void expect_ac_blob_public_key_binding(const api_ops_t& api) {
  std::vector<cmem_t> key_blobs_a;
  std::vector<cmem_t> key_blobs_b;
  generate_ac_key_blobs(api, key_blobs_a);
  generate_ac_key_blobs(api, key_blobs_b);

  cmem_t replacement_pub{nullptr, 0};
  ASSERT_EQ(api.get_public_key(key_blobs_b[0], &replacement_pub), CBMPC_SUCCESS);

  const buf_t tampered_key_blob = tamper_blob_public_key(key_blobs_a[0], replacement_pub);

  cmem_t out_pub{nullptr, 0};
  EXPECT_NE(api.get_public_key(cmem_t{const_cast<uint8_t*>(tampered_key_blob.data()), tampered_key_blob.size()}, &out_pub),
            CBMPC_SUCCESS);
  EXPECT_EQ(out_pub.data, nullptr);
  EXPECT_EQ(out_pub.size, 0);

  cmem_t public_blob{nullptr, 0};
  cmem_t private_scalar{nullptr, 0};
  ASSERT_EQ(api.detach_private_scalar(key_blobs_a[0], &public_blob, &private_scalar), CBMPC_SUCCESS);

  cmem_t public_share{nullptr, 0};
  ASSERT_EQ(api.get_public_share(key_blobs_a[0], &public_share), CBMPC_SUCCESS);

  const buf_t tampered_public_blob = tamper_blob_public_key(public_blob, replacement_pub);
  cmem_t restored{nullptr, 0};
  EXPECT_NE(api.attach_private_scalar(cmem_t{const_cast<uint8_t*>(tampered_public_blob.data()), tampered_public_blob.size()},
                                      private_scalar, public_share, &restored),
            CBMPC_SUCCESS);
  EXPECT_EQ(restored.data, nullptr);
  EXPECT_EQ(restored.size, 0);

  cbmpc_cmem_free(restored);
  cbmpc_cmem_free(public_share);
  cbmpc_cmem_free(private_scalar);
  cbmpc_cmem_free(public_blob);
  cbmpc_cmem_free(out_pub);
  cbmpc_cmem_free(replacement_pub);
  for (auto blob : key_blobs_a) cbmpc_cmem_free(blob);
  for (auto blob : key_blobs_b) cbmpc_cmem_free(blob);
}

TEST(CApiMpAcKeyBlobBinding, Schnorr) {
  const api_ops_t api = {
      CBMPC_CURVE_SECP256K1,
      cbmpc_schnorr_mp_dkg_ac,
      cbmpc_schnorr_mp_get_public_key_compressed,
      cbmpc_schnorr_mp_get_public_share_compressed,
      cbmpc_schnorr_mp_detach_private_scalar,
      cbmpc_schnorr_mp_attach_private_scalar,
  };
  expect_ac_blob_public_key_binding(api);
}

TEST(CApiMpAcKeyBlobBinding, Ecdsa) {
  const api_ops_t api = {
      CBMPC_CURVE_SECP256K1,
      cbmpc_ecdsa_mp_dkg_ac,
      cbmpc_ecdsa_mp_get_public_key_compressed,
      cbmpc_ecdsa_mp_get_public_share_compressed,
      cbmpc_ecdsa_mp_detach_private_scalar,
      cbmpc_ecdsa_mp_attach_private_scalar,
  };
  expect_ac_blob_public_key_binding(api);
}

TEST(CApiMpAcKeyBlobBinding, EdDsa) {
  const api_ops_t api = {
      CBMPC_CURVE_ED25519,
      cbmpc_eddsa_mp_dkg_ac,
      cbmpc_eddsa_mp_get_public_key_compressed,
      cbmpc_eddsa_mp_get_public_share_compressed,
      cbmpc_eddsa_mp_detach_private_scalar,
      cbmpc_eddsa_mp_attach_private_scalar,
  };
  expect_ac_blob_public_key_binding(api);
}

}  // namespace
