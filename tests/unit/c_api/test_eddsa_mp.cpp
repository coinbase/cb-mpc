#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/c_api/eddsa_mp.h>
#include <cbmpc/c_api/job.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/crypto/base_ecc.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;

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

}  // namespace

TEST(CApiEdDSAMp, DkgSignRefreshSignRoleChange4p) {
  constexpr int n = 4;
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  std::atomic<int> free_calls[n];
  transport_ctx_t ctx[n];
  cbmpc_transport_t transports[n];
  for (int i = 0; i < n; i++) {
    free_calls[i].store(0);
    ctx[i] = transport_ctx_t{peers[static_cast<size_t>(i)], &free_calls[i]};
    transports[i] = make_transport(&ctx[i]);
  }

  const char* party_names[n] = {"p0", "p1", "p2", "p3"};

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
            /*transport=*/&transports[i],
        };
        return cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key_blobs[static_cast<size_t>(i)],
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

  // Change the party ordering ("role" indices) between protocols.
  const char* party_names2[n] = {"p0", "p2", "p1", "p3"};
  // Map new role index -> old role index (DKG) for the same party name.
  const int perm[n] = {0, 2, 1, 3};

  uint8_t msg_bytes[32];
  for (int i = 0; i < 32; i++) msg_bytes[i] = static_cast<uint8_t>(i);
  const cmem_t msg = {msg_bytes, 32};

  std::vector<cmem_t> sigs(n, cmem_t{nullptr, 0});
  run_mp(
      peers,
      [&](int i) {
        const cbmpc_mp_job_t job = {
            /*self=*/i,
            /*party_names=*/party_names2,
            /*party_names_count=*/n,
            /*transport=*/&transports[i],
        };
        return cbmpc_eddsa_mp_sign_additive(&job, key_blobs[static_cast<size_t>(perm[i])], msg, /*sig_receiver=*/2,
                                            &sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  ASSERT_EQ(sigs[2].size, 64);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    ASSERT_EQ(sigs[static_cast<size_t>(i)].size, 0);
  }
  ASSERT_EQ(verify_key.verify(buf_t(msg_bytes, 32), buf_t(sigs[2].data, sigs[2].size)), SUCCESS);

  std::vector<cmem_t> new_key_blobs(n, cmem_t{nullptr, 0});
  std::vector<cmem_t> sid_outs(n, cmem_t{nullptr, 0});
  run_mp(
      peers,
      [&](int i) {
        const cbmpc_mp_job_t job = {
            /*self=*/i,
            /*party_names=*/party_names2,
            /*party_names_count=*/n,
            /*transport=*/&transports[i],
        };
        return cbmpc_eddsa_mp_refresh_additive(
            &job, sids[static_cast<size_t>(perm[i])], key_blobs[static_cast<size_t>(perm[i])],
            &sid_outs[static_cast<size_t>(i)], &new_key_blobs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  for (int i = 0; i < n; i++) ASSERT_GT(new_key_blobs[static_cast<size_t>(i)].size, 0);
  for (int i = 1; i < n; i++) expect_eq(sid_outs[0], sid_outs[static_cast<size_t>(i)]);
  expect_eq(sids[0], sid_outs[0]);

  for (int i = 0; i < n; i++) {
    cmem_t pub_i{nullptr, 0};
    ASSERT_EQ(cbmpc_eddsa_mp_get_public_key_compressed(new_key_blobs[static_cast<size_t>(i)], &pub_i), CBMPC_SUCCESS);
    expect_eq(pub_i, pub0);
    cbmpc_cmem_free(pub_i);
  }

  std::vector<cmem_t> new_sigs(n, cmem_t{nullptr, 0});
  run_mp(
      peers,
      [&](int i) {
        const cbmpc_mp_job_t job = {
            /*self=*/i,
            /*party_names=*/party_names2,
            /*party_names_count=*/n,
            /*transport=*/&transports[i],
        };
        return cbmpc_eddsa_mp_sign_additive(&job, new_key_blobs[static_cast<size_t>(i)], msg, /*sig_receiver=*/2,
                                            &new_sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  ASSERT_EQ(new_sigs[2].size, 64);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    ASSERT_EQ(new_sigs[static_cast<size_t>(i)].size, 0);
  }
  ASSERT_EQ(verify_key.verify(buf_t(msg_bytes, 32), buf_t(new_sigs[2].data, new_sigs[2].size)), SUCCESS);

  for (int i = 0; i < n; i++) EXPECT_GT(free_calls[i].load(), 0);

  cbmpc_cmem_free(pub0);
  for (auto m : new_sigs) cbmpc_cmem_free(m);
  for (auto m : sid_outs) cbmpc_cmem_free(m);
  for (auto m : new_key_blobs) cbmpc_cmem_free(m);
  for (auto m : sigs) cbmpc_cmem_free(m);
  for (auto m : sids) cbmpc_cmem_free(m);
  for (auto m : key_blobs) cbmpc_cmem_free(m);
}

TEST(CApiEdDSAMp, ValidatesArgs) {
  cmem_t key{reinterpret_cast<uint8_t*>(0x1), 123};
  cmem_t sid{reinterpret_cast<uint8_t*>(0x1), 123};

  const cbmpc_transport_t bad_transport = {/*ctx=*/nullptr, /*send=*/nullptr, /*receive=*/nullptr,
                                           /*receive_all=*/nullptr,
                                           /*free=*/nullptr};
  const char* names[2] = {"p0", "p1"};
  const cbmpc_mp_job_t bad_job = {/*self=*/0, /*party_names=*/names, /*party_names_count=*/2,
                                  /*transport=*/&bad_transport};

  EXPECT_EQ(cbmpc_eddsa_mp_dkg_additive(&bad_job, CBMPC_CURVE_ED25519, &key, &sid), E_BADARG);
  EXPECT_EQ(key.data, nullptr);
  EXPECT_EQ(key.size, 0);
  EXPECT_EQ(sid.data, nullptr);
  EXPECT_EQ(sid.size, 0);

  // Missing sig_out is invalid.
  EXPECT_EQ(cbmpc_eddsa_mp_sign_additive(nullptr, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, 0, nullptr), E_BADARG);
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

namespace {

static cbmpc_error_t noop_send(void*, int32_t, const uint8_t*, int) { return E_GENERAL; }
static cbmpc_error_t noop_receive(void*, int32_t, cmem_t*) { return E_GENERAL; }
static cbmpc_error_t noop_receive_all(void*, const int32_t*, int, cmems_t*) { return E_GENERAL; }

static const cbmpc_transport_t noop_capi_transport = {nullptr, noop_send, noop_receive, noop_receive_all, nullptr};

static void capi_generate_eddsa_mp_key_blobs(int n, std::vector<cmem_t>& blobs) {
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  std::vector<transport_ctx_t> ctxs(n);
  std::vector<cbmpc_transport_t> transports(n);
  for (int i = 0; i < n; i++) {
    ctxs[i] = transport_ctx_t{peers[static_cast<size_t>(i)], nullptr};
    transports[i] = make_transport(&ctxs[i]);
  }

  std::vector<std::string> names;
  for (int i = 0; i < n; i++) names.push_back("p" + std::to_string(i));
  std::vector<const char*> name_ptrs;
  for (const auto& nm : names) name_ptrs.push_back(nm.c_str());

  blobs.resize(n, cmem_t{nullptr, 0});
  std::vector<cmem_t> sids(n, cmem_t{nullptr, 0});
  std::vector<cbmpc_error_t> rvs;
  run_mp(
      peers,
      [&](int i) {
        const cbmpc_mp_job_t job = {i, name_ptrs.data(), n, &transports[static_cast<size_t>(i)]};
        return cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &blobs[static_cast<size_t>(i)],
                                           &sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  for (auto m : sids) cbmpc_cmem_free(m);
}

}  // namespace

class CApiEdDSAMpNegWithBlobs : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { capi_generate_eddsa_mp_key_blobs(3, blobs_); }
  static void TearDownTestSuite() {
    for (auto m : blobs_) cbmpc_cmem_free(m);
    blobs_.clear();
  }
  static std::vector<cmem_t> blobs_;
};
std::vector<cmem_t> CApiEdDSAMpNegWithBlobs::blobs_;

// ==========================================================================
// Negative: dkg
// ==========================================================================

TEST(CApiEdDSAMp, NegDkgNullOutKeyBlob) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_capi_transport};
  cmem_t sid{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, nullptr, &sid), E_BADARG);
}

TEST(CApiEdDSAMp, NegDkgNullOutSid) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_capi_transport};
  cmem_t key{nullptr, 0};
  EXPECT_EQ(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key, nullptr), E_BADARG);
}

TEST(CApiEdDSAMp, NegDkgNullJob) {
  cmem_t key{nullptr, 0};
  cmem_t sid{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_dkg_additive(nullptr, CBMPC_CURVE_ED25519, &key, &sid), CBMPC_SUCCESS);
  EXPECT_EQ(key.data, nullptr);
  EXPECT_EQ(sid.data, nullptr);
}

TEST(CApiEdDSAMp, NegDkgInvalidJobFields) {
  cmem_t key{nullptr, 0};
  cmem_t sid{nullptr, 0};

  {
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {0, names, 2, nullptr};
    EXPECT_EQ(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key, &sid), E_BADARG);
  }
  {
    cbmpc_transport_t bad_t = noop_capi_transport;
    bad_t.send = nullptr;
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {0, names, 2, &bad_t};
    EXPECT_EQ(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key, &sid), E_BADARG);
  }
  {
    cbmpc_transport_t bad_t = noop_capi_transport;
    bad_t.receive = nullptr;
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {0, names, 2, &bad_t};
    EXPECT_EQ(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key, &sid), E_BADARG);
  }
  {
    cbmpc_transport_t bad_t = noop_capi_transport;
    bad_t.receive_all = nullptr;
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {0, names, 2, &bad_t};
    EXPECT_EQ(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key, &sid), E_BADARG);
  }
}

TEST(CApiEdDSAMp, NegDkgInvalidCurves) {
  const char* names[] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {0, names, 2, &noop_capi_transport};

  {
    cmem_t key{nullptr, 0};
    cmem_t sid{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_SECP256K1, &key, &sid), CBMPC_SUCCESS);
    EXPECT_EQ(key.data, nullptr);
  }
  {
    cmem_t key{nullptr, 0};
    cmem_t sid{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_P256, &key, &sid), CBMPC_SUCCESS);
    EXPECT_EQ(key.data, nullptr);
  }
  for (int val : {0, 4, 255}) {
    cmem_t key{nullptr, 0};
    cmem_t sid{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_dkg_additive(&job, static_cast<cbmpc_curve_id_t>(val), &key, &sid), CBMPC_SUCCESS)
        << "Expected failure for curve_id=" << val;
    EXPECT_EQ(key.data, nullptr);
  }
}

TEST(CApiEdDSAMp, NegDkgInvalidParty) {
  {
    const char* names[] = {"p0", "p1", "p2"};
    const cbmpc_mp_job_t job = {3, names, 3, &noop_capi_transport};
    cmem_t key{nullptr, 0};
    cmem_t sid{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key, &sid), CBMPC_SUCCESS);
    EXPECT_EQ(key.data, nullptr);
  }
  {
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {-1, names, 2, &noop_capi_transport};
    cmem_t key{nullptr, 0};
    cmem_t sid{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key, &sid), CBMPC_SUCCESS);
    EXPECT_EQ(key.data, nullptr);
  }
  {
    const char* names[] = {"p0"};
    const cbmpc_mp_job_t job = {0, names, 1, &noop_capi_transport};
    cmem_t key{nullptr, 0};
    cmem_t sid{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key, &sid), CBMPC_SUCCESS);
    EXPECT_EQ(key.data, nullptr);
  }
  {
    const cbmpc_mp_job_t job = {0, nullptr, 0, &noop_capi_transport};
    cmem_t key{nullptr, 0};
    cmem_t sid{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key, &sid), CBMPC_SUCCESS);
    EXPECT_EQ(key.data, nullptr);
  }
}

TEST(CApiEdDSAMp, NegDkgDuplicatePartyNames) {
  const char* names[] = {"p0", "p0", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  cmem_t key{nullptr, 0};
  cmem_t sid{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_dkg_additive(&job, CBMPC_CURVE_ED25519, &key, &sid), CBMPC_SUCCESS);
  EXPECT_EQ(key.data, nullptr);
}

// ==========================================================================
// Negative: get_public_key_compressed
// ==========================================================================

TEST(CApiEdDSAMp, NegGetPubKeyNullOutput) {
  uint8_t dummy[] = {0x01};
  EXPECT_EQ(cbmpc_eddsa_mp_get_public_key_compressed(cmem_t{dummy, 1}, nullptr), E_BADARG);
}

TEST(CApiEdDSAMp, NegGetPubKeyBadBlob) {
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_get_public_key_compressed(cmem_t{garbage, 4}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    uint8_t data[] = {0x01};
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_get_public_key_compressed(cmem_t{data, -1}, &out), E_BADARG);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_get_public_key_compressed(cmem_t{nullptr, 10}, &out), E_BADARG);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    uint8_t zeros[64] = {};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_get_public_key_compressed(cmem_t{zeros, 64}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    uint8_t one = 0x00;
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_get_public_key_compressed(cmem_t{&one, 1}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_get_public_key_compressed(cmem_t{nullptr, 0}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
}

TEST(CApiEdDSAMp, NegGetPubKeyOversizedBlob) {
  uint8_t huge[4096];
  std::memset(huge, 0x42, sizeof(huge));
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_get_public_key_compressed(cmem_t{huge, 4096}, &out), CBMPC_SUCCESS);
  EXPECT_EQ(out.data, nullptr);
}

// ==========================================================================
// Negative: get_public_share_compressed
// ==========================================================================

TEST(CApiEdDSAMp, NegGetPubShareNullOutput) {
  uint8_t dummy[] = {0x01};
  EXPECT_EQ(cbmpc_eddsa_mp_get_public_share_compressed(cmem_t{dummy, 1}, nullptr), E_BADARG);
}

TEST(CApiEdDSAMp, NegGetPubShareBadBlob) {
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_get_public_share_compressed(cmem_t{garbage, 4}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    uint8_t data[] = {0x01};
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_get_public_share_compressed(cmem_t{data, -1}, &out), E_BADARG);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_get_public_share_compressed(cmem_t{nullptr, 10}, &out), E_BADARG);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    uint8_t zeros[64] = {};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_get_public_share_compressed(cmem_t{zeros, 64}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_get_public_share_compressed(cmem_t{nullptr, 0}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
}

TEST(CApiEdDSAMp, NegGetPubShareOversizedBlob) {
  uint8_t huge[4096];
  std::memset(huge, 0x42, sizeof(huge));
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_get_public_share_compressed(cmem_t{huge, 4096}, &out), CBMPC_SUCCESS);
  EXPECT_EQ(out.data, nullptr);
}

// ==========================================================================
// Negative: detach_private_scalar
// ==========================================================================

TEST(CApiEdDSAMp, NegDetachNullOutputs) {
  uint8_t dummy[] = {0x01};
  cmem_t blob = {dummy, 1};
  cmem_t out1{nullptr, 0};
  cmem_t out2{nullptr, 0};

  EXPECT_EQ(cbmpc_eddsa_mp_detach_private_scalar(blob, nullptr, &out2), E_BADARG);
  EXPECT_EQ(cbmpc_eddsa_mp_detach_private_scalar(blob, &out1, nullptr), E_BADARG);
  EXPECT_EQ(cbmpc_eddsa_mp_detach_private_scalar(blob, nullptr, nullptr), E_BADARG);
}

TEST(CApiEdDSAMp, NegDetachBadBlob) {
  {
    uint8_t zeros[64] = {};
    cmem_t pub{nullptr, 0};
    cmem_t scalar{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_detach_private_scalar(cmem_t{zeros, 64}, &pub, &scalar), CBMPC_SUCCESS);
    EXPECT_EQ(pub.data, nullptr);
    EXPECT_EQ(scalar.data, nullptr);
  }
  {
    cmem_t pub{nullptr, 0};
    cmem_t scalar{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_detach_private_scalar(cmem_t{nullptr, 0}, &pub, &scalar), CBMPC_SUCCESS);
    EXPECT_EQ(pub.data, nullptr);
    EXPECT_EQ(scalar.data, nullptr);
  }
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    cmem_t pub{nullptr, 0};
    cmem_t scalar{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_detach_private_scalar(cmem_t{garbage, 4}, &pub, &scalar), CBMPC_SUCCESS);
    EXPECT_EQ(pub.data, nullptr);
    EXPECT_EQ(scalar.data, nullptr);
  }
  {
    uint8_t data[] = {0x01};
    cmem_t pub{nullptr, 0};
    cmem_t scalar{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_detach_private_scalar(cmem_t{data, -1}, &pub, &scalar), E_BADARG);
    EXPECT_EQ(pub.data, nullptr);
    EXPECT_EQ(scalar.data, nullptr);
  }
}

// ==========================================================================
// Negative: attach_private_scalar
// ==========================================================================

TEST(CApiEdDSAMp, NegAttachNullOutput) {
  uint8_t dummy[] = {0x01};
  EXPECT_EQ(cbmpc_eddsa_mp_attach_private_scalar(cmem_t{dummy, 1}, cmem_t{dummy, 1}, cmem_t{dummy, 1}, nullptr),
            E_BADARG);
}

TEST(CApiEdDSAMp, NegAttachBadCmemInputs) {
  cmem_t out{nullptr, 0};

  {
    uint8_t scalar[] = {0x01};
    uint8_t point[32] = {};
    EXPECT_NE(cbmpc_eddsa_mp_attach_private_scalar(cmem_t{nullptr, 0}, cmem_t{scalar, 1}, cmem_t{point, 32}, &out),
              CBMPC_SUCCESS);
  }
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t scalar[] = {0x01};
    uint8_t point[32] = {};
    EXPECT_NE(cbmpc_eddsa_mp_attach_private_scalar(cmem_t{garbage, 4}, cmem_t{scalar, 1}, cmem_t{point, 32}, &out),
              CBMPC_SUCCESS);
  }
  {
    uint8_t data[] = {0x01};
    EXPECT_EQ(cbmpc_eddsa_mp_attach_private_scalar(cmem_t{data, -1}, cmem_t{data, 1}, cmem_t{data, 1}, &out), E_BADARG);
  }
  {
    uint8_t data[] = {0x01};
    EXPECT_EQ(cbmpc_eddsa_mp_attach_private_scalar(cmem_t{data, 1}, cmem_t{data, -1}, cmem_t{data, 1}, &out), E_BADARG);
  }
  {
    uint8_t data[] = {0x01};
    EXPECT_EQ(cbmpc_eddsa_mp_attach_private_scalar(cmem_t{data, 1}, cmem_t{data, 1}, cmem_t{data, -1}, &out), E_BADARG);
  }
  {
    uint8_t data[] = {0x01};
    EXPECT_EQ(cbmpc_eddsa_mp_attach_private_scalar(cmem_t{nullptr, 10}, cmem_t{data, 1}, cmem_t{data, 1}, &out),
              E_BADARG);
  }
  {
    uint8_t data[] = {0x01};
    EXPECT_EQ(cbmpc_eddsa_mp_attach_private_scalar(cmem_t{data, 1}, cmem_t{nullptr, 10}, cmem_t{data, 1}, &out),
              E_BADARG);
  }
  {
    uint8_t data[] = {0x01};
    EXPECT_EQ(cbmpc_eddsa_mp_attach_private_scalar(cmem_t{data, 1}, cmem_t{data, 1}, cmem_t{nullptr, 10}, &out),
              E_BADARG);
  }
}

// ==========================================================================
// Negative: sign_additive
// ==========================================================================

TEST(CApiEdDSAMp, NegSignNullSigOutput) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  uint8_t msg[32] = {};
  EXPECT_EQ(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{nullptr, 0}, cmem_t{msg, 32}, 0, nullptr), E_BADARG);
}

TEST(CApiEdDSAMp, NegSignNullJob) {
  uint8_t msg[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_additive(nullptr, cmem_t{nullptr, 0}, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST(CApiEdDSAMp, NegSignInvalidJob) {
  uint8_t msg[32] = {};

  {
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {0, names, 2, nullptr};
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{nullptr, 0}, cmem_t{msg, 32}, 0, &sig), E_BADARG);
  }
  {
    cbmpc_transport_t bad_t = noop_capi_transport;
    bad_t.send = nullptr;
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {0, names, 2, &bad_t};
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{nullptr, 0}, cmem_t{msg, 32}, 0, &sig), E_BADARG);
  }
  {
    cbmpc_transport_t bad_t = noop_capi_transport;
    bad_t.receive = nullptr;
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {0, names, 2, &bad_t};
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{nullptr, 0}, cmem_t{msg, 32}, 0, &sig), E_BADARG);
  }
  {
    cbmpc_transport_t bad_t = noop_capi_transport;
    bad_t.receive_all = nullptr;
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {0, names, 2, &bad_t};
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{nullptr, 0}, cmem_t{msg, 32}, 0, &sig), E_BADARG);
  }
}

TEST(CApiEdDSAMp, NegSignBadKeyBlob) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  uint8_t msg[32] = {};

  {
    cmem_t sig{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{nullptr, 0}, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
  }
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    cmem_t sig{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{garbage, 4}, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
  }
  {
    uint8_t data[] = {0x01};
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{data, -1}, cmem_t{msg, 32}, 0, &sig), E_BADARG);
  }
  {
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{nullptr, 10}, cmem_t{msg, 32}, 0, &sig), E_BADARG);
  }
  {
    uint8_t zeros[64] = {};
    cmem_t sig{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{zeros, 64}, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
  }
}

TEST(CApiEdDSAMp, NegSignBadMsg) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  uint8_t dummy_blob[] = {0x01};

  {
    uint8_t data[] = {0x01};
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{dummy_blob, 1}, cmem_t{data, -1}, 0, &sig), E_BADARG);
  }
  {
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{dummy_blob, 1}, cmem_t{nullptr, 10}, 0, &sig), E_BADARG);
  }
}

TEST(CApiEdDSAMp, NegSignOversizedKeyBlob) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  uint8_t msg[32] = {};
  uint8_t huge[4096];
  std::memset(huge, 0x42, sizeof(huge));
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{huge, 4096}, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST(CApiEdDSAMp, NegSignInvalidSigReceiver) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  uint8_t msg[32] = {};
  uint8_t dummy[] = {0x01};

  {
    cmem_t sig{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{dummy, 1}, cmem_t{msg, 32}, -1, &sig), CBMPC_SUCCESS);
  }
  {
    cmem_t sig{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{dummy, 1}, cmem_t{msg, 32}, 100, &sig), CBMPC_SUCCESS);
  }
}

TEST_F(CApiEdDSAMpNegWithBlobs, NegSignEmptyKeyBlob) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  uint8_t msg[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{nullptr, 0}, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpNegWithBlobs, NegSignAllZeroKeyBlob) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  uint8_t msg[32] = {};
  uint8_t zeros[256] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_additive(&job, cmem_t{zeros, 256}, cmem_t{msg, 32}, 0, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEdDSAMpNegWithBlobs, NegSignEmptyMsg) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_sign_additive(&job, blobs_[0], cmem_t{nullptr, 0}, 0, &sig), CBMPC_SUCCESS);
}

// ==========================================================================
// Negative: refresh_additive
// ==========================================================================

TEST(CApiEdDSAMp, NegRefreshNullOutput) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  uint8_t dummy[] = {0x01};
  EXPECT_EQ(cbmpc_eddsa_mp_refresh_additive(&job, cmem_t{nullptr, 0}, cmem_t{dummy, 1}, nullptr, nullptr), E_BADARG);
}

TEST(CApiEdDSAMp, NegRefreshNullJob) {
  uint8_t dummy[] = {0x01};
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_refresh_additive(nullptr, cmem_t{nullptr, 0}, cmem_t{dummy, 1}, nullptr, &out),
            CBMPC_SUCCESS);
}

TEST(CApiEdDSAMp, NegRefreshInvalidJob) {
  uint8_t dummy[] = {0x01};
  uint8_t sid_data[32] = {};

  {
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {0, names, 2, nullptr};
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_refresh_additive(&job, cmem_t{sid_data, 32}, cmem_t{dummy, 1}, nullptr, &out), E_BADARG);
  }
  {
    cbmpc_transport_t bad_t = noop_capi_transport;
    bad_t.send = nullptr;
    const char* names[] = {"p0", "p1"};
    const cbmpc_mp_job_t job = {0, names, 2, &bad_t};
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_refresh_additive(&job, cmem_t{sid_data, 32}, cmem_t{dummy, 1}, nullptr, &out), E_BADARG);
  }
}

TEST(CApiEdDSAMp, NegRefreshBadKeyBlob) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};

  {
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_refresh_additive(&job, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, nullptr, &out),
              CBMPC_SUCCESS);
  }
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_refresh_additive(&job, cmem_t{nullptr, 0}, cmem_t{garbage, 4}, nullptr, &out),
              CBMPC_SUCCESS);
  }
  {
    uint8_t data[] = {0x01};
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_eddsa_mp_refresh_additive(&job, cmem_t{nullptr, 0}, cmem_t{data, -1}, nullptr, &out), E_BADARG);
  }
  {
    uint8_t zeros[64] = {};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_refresh_additive(&job, cmem_t{nullptr, 0}, cmem_t{zeros, 64}, nullptr, &out),
              CBMPC_SUCCESS);
  }
}

TEST(CApiEdDSAMp, NegRefreshOversizedKeyBlob) {
  const char* names[] = {"p0", "p1", "p2"};
  const cbmpc_mp_job_t job = {0, names, 3, &noop_capi_transport};
  uint8_t huge[4096];
  std::memset(huge, 0x42, sizeof(huge));
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_refresh_additive(&job, cmem_t{nullptr, 0}, cmem_t{huge, 4096}, nullptr, &out),
            CBMPC_SUCCESS);
}

// ==========================================================================
// Negative (fixture): attach_private_scalar
// ==========================================================================

TEST_F(CApiEdDSAMpNegWithBlobs, NegAttachEmptyScalar) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_detach_private_scalar(blobs_[0], &pub, &x), CBMPC_SUCCESS);

  cmem_t Qi{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_get_public_share_compressed(blobs_[0], &Qi), CBMPC_SUCCESS);

  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_attach_private_scalar(pub, cmem_t{nullptr, 0}, Qi, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
  cbmpc_cmem_free(Qi);
}

TEST_F(CApiEdDSAMpNegWithBlobs, NegAttachZeroScalar) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_detach_private_scalar(blobs_[0], &pub, &x), CBMPC_SUCCESS);

  cmem_t Qi{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_get_public_share_compressed(blobs_[0], &Qi), CBMPC_SUCCESS);

  uint8_t zero[32] = {};
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_attach_private_scalar(pub, cmem_t{zero, 32}, Qi, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
  cbmpc_cmem_free(Qi);
}

TEST_F(CApiEdDSAMpNegWithBlobs, NegAttachGarbageScalar) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_detach_private_scalar(blobs_[0], &pub, &x), CBMPC_SUCCESS);

  cmem_t Qi{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_get_public_share_compressed(blobs_[0], &Qi), CBMPC_SUCCESS);

  uint8_t garbage[512];
  std::memset(garbage, 0xFF, sizeof(garbage));
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_attach_private_scalar(pub, cmem_t{garbage, 512}, Qi, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
  cbmpc_cmem_free(Qi);
}

TEST_F(CApiEdDSAMpNegWithBlobs, NegAttachWrongSizeScalar) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_detach_private_scalar(blobs_[0], &pub, &x), CBMPC_SUCCESS);

  cmem_t Qi{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_get_public_share_compressed(blobs_[0], &Qi), CBMPC_SUCCESS);

  {
    uint8_t short_scalar[31];
    std::memset(short_scalar, 0x42, sizeof(short_scalar));
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_attach_private_scalar(pub, cmem_t{short_scalar, 31}, Qi, &out), CBMPC_SUCCESS);
  }
  {
    uint8_t long_scalar[33];
    std::memset(long_scalar, 0x42, sizeof(long_scalar));
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_eddsa_mp_attach_private_scalar(pub, cmem_t{long_scalar, 33}, Qi, &out), CBMPC_SUCCESS);
  }

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
  cbmpc_cmem_free(Qi);
}

TEST_F(CApiEdDSAMpNegWithBlobs, NegAttachEmptyPublicShare) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_detach_private_scalar(blobs_[0], &pub, &x), CBMPC_SUCCESS);

  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_attach_private_scalar(pub, x, cmem_t{nullptr, 0}, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
}

TEST_F(CApiEdDSAMpNegWithBlobs, NegAttachAllZeroPublicShare) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_detach_private_scalar(blobs_[0], &pub, &x), CBMPC_SUCCESS);

  uint8_t zero_share[32] = {};
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_attach_private_scalar(pub, x, cmem_t{zero_share, 32}, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
}

TEST_F(CApiEdDSAMpNegWithBlobs, NegAttachGarbagePublicShare) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_eddsa_mp_detach_private_scalar(blobs_[0], &pub, &x), CBMPC_SUCCESS);

  uint8_t garbage_share[32];
  std::memset(garbage_share, 0xFF, sizeof(garbage_share));
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_eddsa_mp_attach_private_scalar(pub, x, cmem_t{garbage_share, 32}, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
}
