#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/capi/job.h>
#include <cbmpc/capi/schnorr_mp.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/crypto/base_ecc_secp256k1.h>

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

TEST(CApiSchnorrMp, DkgSignRefreshSignRoleChange4p) {
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
        return cbmpc_schnorr_mp_dkg_additive(&job, CBMPC_CURVE_SECP256K1, &key_blobs[static_cast<size_t>(i)],
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
  ASSERT_EQ(cbmpc_schnorr_mp_get_public_key_compressed(key_blobs[0], &pub0), CBMPC_SUCCESS);
  ASSERT_EQ(pub0.size, 33);
  for (int i = 1; i < n; i++) {
    cmem_t pub_i{nullptr, 0};
    ASSERT_EQ(cbmpc_schnorr_mp_get_public_key_compressed(key_blobs[static_cast<size_t>(i)], &pub_i), CBMPC_SUCCESS);
    expect_eq(pub_i, pub0);
    cbmpc_cmem_free(pub_i);
  }

  const buf_t pub_buf(pub0.data, pub0.size);
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_secp256k1, pub_buf), SUCCESS);

  cmem_t xonly0{nullptr, 0};
  ASSERT_EQ(cbmpc_schnorr_mp_extract_public_key_xonly(key_blobs[0], &xonly0), CBMPC_SUCCESS);
  ASSERT_EQ(xonly0.size, 32);
  const buf_t expected_xonly = Q.get_x().to_bin(32);
  ASSERT_EQ(std::memcmp(expected_xonly.data(), xonly0.data, 32), 0);

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
        return cbmpc_schnorr_mp_sign_additive(&job, key_blobs[static_cast<size_t>(perm[i])], msg, /*sig_receiver=*/2,
                                              &sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  ASSERT_EQ(sigs[2].size, 64);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    ASSERT_EQ(sigs[static_cast<size_t>(i)].size, 0);
  }
  ASSERT_EQ(coinbase::crypto::bip340::verify(Q, coinbase::mem_t(msg_bytes, 32), coinbase::mem_t(sigs[2].data, 64)),
            SUCCESS);

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
        return cbmpc_schnorr_mp_refresh_additive(
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
    ASSERT_EQ(cbmpc_schnorr_mp_get_public_key_compressed(new_key_blobs[static_cast<size_t>(i)], &pub_i), CBMPC_SUCCESS);
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
        return cbmpc_schnorr_mp_sign_additive(&job, new_key_blobs[static_cast<size_t>(i)], msg, /*sig_receiver=*/2,
                                              &new_sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  ASSERT_EQ(new_sigs[2].size, 64);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    ASSERT_EQ(new_sigs[static_cast<size_t>(i)].size, 0);
  }
  ASSERT_EQ(coinbase::crypto::bip340::verify(Q, coinbase::mem_t(msg_bytes, 32), coinbase::mem_t(new_sigs[2].data, 64)),
            SUCCESS);

  for (int i = 0; i < n; i++) EXPECT_GT(free_calls[i].load(), 0);

  cbmpc_cmem_free(pub0);
  cbmpc_cmem_free(xonly0);
  for (auto m : new_sigs) cbmpc_cmem_free(m);
  for (auto m : sid_outs) cbmpc_cmem_free(m);
  for (auto m : new_key_blobs) cbmpc_cmem_free(m);
  for (auto m : sigs) cbmpc_cmem_free(m);
  for (auto m : sids) cbmpc_cmem_free(m);
  for (auto m : key_blobs) cbmpc_cmem_free(m);
}

TEST(CApiSchnorrMp, ValidatesArgs) {
  cmem_t key{reinterpret_cast<uint8_t*>(0x1), 123};
  cmem_t sid{reinterpret_cast<uint8_t*>(0x1), 123};

  const cbmpc_transport_t bad_transport = {/*ctx=*/nullptr, /*send=*/nullptr, /*receive=*/nullptr,
                                           /*receive_all=*/nullptr,
                                           /*free=*/nullptr};
  const char* names[2] = {"p0", "p1"};
  const cbmpc_mp_job_t bad_job = {/*self=*/0, /*party_names=*/names, /*party_names_count=*/2,
                                  /*transport=*/&bad_transport};

  EXPECT_EQ(cbmpc_schnorr_mp_dkg_additive(&bad_job, CBMPC_CURVE_SECP256K1, &key, &sid), E_BADARG);
  EXPECT_EQ(key.data, nullptr);
  EXPECT_EQ(key.size, 0);
  EXPECT_EQ(sid.data, nullptr);
  EXPECT_EQ(sid.size, 0);

  // Missing sig_out is invalid.
  EXPECT_EQ(cbmpc_schnorr_mp_sign_additive(nullptr, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, 0, nullptr), E_BADARG);
}
