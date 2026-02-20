#include <atomic>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

#include <cbmpc/capi/ecdsa_mp.h>
#include <cbmpc/capi/job.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/crypto/base_ecc.h>

#include "utils/local_network/network_context.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;

using coinbase::api::party_idx_t;
using coinbase::testutils::mpc_net_context_t;

struct transport_ctx_t {
  std::shared_ptr<mpc_net_context_t> net;
  std::atomic<int>* free_calls = nullptr;
};

static cbmpc_error_t transport_send(void* ctx, int32_t receiver, const uint8_t* data, int size) {
  if (!ctx) return E_BADARG;
  if (size < 0) return E_BADARG;
  if (size > 0 && !data) return E_BADARG;
  auto* c = static_cast<transport_ctx_t*>(ctx);
  c->net->send(static_cast<party_idx_t>(receiver), mem_t(data, size));
  return CBMPC_SUCCESS;
}

static cbmpc_error_t transport_receive(void* ctx, int32_t sender, cmem_t* out_msg) {
  if (!out_msg) return E_BADARG;
  *out_msg = cmem_t{nullptr, 0};
  if (!ctx) return E_BADARG;

  auto* c = static_cast<transport_ctx_t*>(ctx);
  buf_t msg;
  const error_t rv = c->net->receive(static_cast<party_idx_t>(sender), msg);
  if (rv) return rv;

  const int n = msg.size();
  if (n < 0) return E_FORMAT;
  if (n == 0) return CBMPC_SUCCESS;

  out_msg->data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(n)));
  if (!out_msg->data) return E_INSUFFICIENT;
  out_msg->size = n;
  std::memmove(out_msg->data, msg.data(), static_cast<size_t>(n));
  return CBMPC_SUCCESS;
}

static cbmpc_error_t transport_receive_all(void* ctx, const int32_t* senders, int senders_count, cmems_t* out_msgs) {
  if (!out_msgs) return E_BADARG;
  *out_msgs = cmems_t{0, nullptr, nullptr};
  if (!ctx) return E_BADARG;
  if (senders_count < 0) return E_BADARG;
  if (senders_count > 0 && !senders) return E_BADARG;

  auto* c = static_cast<transport_ctx_t*>(ctx);
  std::vector<party_idx_t> s;
  s.reserve(static_cast<size_t>(senders_count));
  for (int i = 0; i < senders_count; i++) s.push_back(static_cast<party_idx_t>(senders[i]));

  std::vector<buf_t> msgs;
  const error_t rv = c->net->receive_all(s, msgs);
  if (rv) return rv;
  if (msgs.size() != static_cast<size_t>(senders_count)) return E_GENERAL;

  // Flatten into (data + sizes) buffers.
  int total = 0;
  for (const auto& m : msgs) {
    const int sz = m.size();
    if (sz < 0) return E_FORMAT;
    if (sz > INT_MAX - total) return E_RANGE;
    total += sz;
  }

  out_msgs->count = senders_count;
  out_msgs->sizes = static_cast<int*>(cbmpc_malloc(sizeof(int) * static_cast<size_t>(senders_count)));
  if (!out_msgs->sizes) {
    *out_msgs = cmems_t{0, nullptr, nullptr};
    return E_INSUFFICIENT;
  }

  if (total > 0) {
    out_msgs->data = static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(total)));
    if (!out_msgs->data) {
      cbmpc_free(out_msgs->sizes);
      *out_msgs = cmems_t{0, nullptr, nullptr};
      return E_INSUFFICIENT;
    }
  }

  int offset = 0;
  for (int i = 0; i < senders_count; i++) {
    const int sz = msgs[i].size();
    out_msgs->sizes[i] = sz;
    if (sz) {
      std::memmove(out_msgs->data + offset, msgs[i].data(), static_cast<size_t>(sz));
      offset += sz;
    }
  }

  return CBMPC_SUCCESS;
}

static void transport_free(void* ctx, void* ptr) {
  if (!ptr) return;
  auto* c = static_cast<transport_ctx_t*>(ctx);
  if (c && c->free_calls) c->free_calls->fetch_add(1);
  cbmpc_free(ptr);
}

template <typename F>
static void run_mp(const std::vector<std::shared_ptr<mpc_net_context_t>>& peers, F&& f,
                   std::vector<cbmpc_error_t>& out_rv) {
  for (const auto& p : peers) p->reset();

  out_rv.assign(peers.size(), UNINITIALIZED_ERROR);
  std::atomic<bool> aborted{false};
  std::vector<std::thread> threads;
  threads.reserve(peers.size());

  for (size_t i = 0; i < peers.size(); i++) {
    threads.emplace_back([&, i] {
      out_rv[i] = f(static_cast<int>(i));
      if (out_rv[i] && !aborted.exchange(true)) {
        for (const auto& p : peers) p->abort();
      }
    });
  }
  for (auto& t : threads) t.join();
}

static void expect_eq(cmem_t a, cmem_t b) {
  ASSERT_EQ(a.size, b.size);
  if (a.size > 0) {
    ASSERT_NE(a.data, nullptr);
    ASSERT_NE(b.data, nullptr);
    ASSERT_EQ(std::memcmp(a.data, b.data, static_cast<size_t>(a.size)), 0);
  }
}

}  // namespace

TEST(CApiEcdsaMp, DkgSignRefreshSign4p) {
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
    transports[i] = cbmpc_transport_t{
        /*ctx=*/&ctx[i],
        /*send=*/transport_send,
        /*receive=*/transport_receive,
        /*receive_all=*/transport_receive_all,
        /*free=*/transport_free,
    };
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
        return cbmpc_ecdsa_mp_dkg_additive(&job, CBMPC_CURVE_SECP256K1, &key_blobs[static_cast<size_t>(i)],
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

  // Change the party ordering ("role" indices) between protocols.
  // Example: a party that was at index 1 ("p1") moves to index 2.
  const char* party_names2[n] = {"p0", "p2", "p1", "p3"};
  // Map new role index -> old role index (DKG) for the same party name.
  const int perm[n] = {0, 2, 1, 3};

  uint8_t msg_hash_bytes[32];
  for (int i = 0; i < 32; i++) msg_hash_bytes[i] = static_cast<uint8_t>(i);
  const cmem_t msg_hash = {msg_hash_bytes, 32};

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
        return cbmpc_ecdsa_mp_sign_additive(&job, key_blobs[static_cast<size_t>(perm[i])], msg_hash,
                                            /*sig_receiver=*/2, &sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  ASSERT_GT(sigs[2].size, 0);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    ASSERT_EQ(sigs[static_cast<size_t>(i)].size, 0);
  }
  ASSERT_EQ(verify_key.verify(buf_t(msg_hash_bytes, 32), buf_t(sigs[2].data, sigs[2].size)), SUCCESS);

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
        return cbmpc_ecdsa_mp_refresh_additive(
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
    ASSERT_EQ(cbmpc_ecdsa_mp_get_public_key_compressed(new_key_blobs[static_cast<size_t>(i)], &pub_i), CBMPC_SUCCESS);
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
        return cbmpc_ecdsa_mp_sign_additive(&job, new_key_blobs[static_cast<size_t>(i)], msg_hash, /*sig_receiver=*/2,
                                            &new_sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  ASSERT_GT(new_sigs[2].size, 0);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    ASSERT_EQ(new_sigs[static_cast<size_t>(i)].size, 0);
  }
  ASSERT_EQ(verify_key.verify(buf_t(msg_hash_bytes, 32), buf_t(new_sigs[2].data, new_sigs[2].size)), SUCCESS);

  for (int i = 0; i < n; i++) EXPECT_GT(free_calls[i].load(), 0);

  cbmpc_cmem_free(pub0);
  for (auto m : new_sigs) cbmpc_cmem_free(m);
  for (auto m : sid_outs) cbmpc_cmem_free(m);
  for (auto m : new_key_blobs) cbmpc_cmem_free(m);
  for (auto m : sigs) cbmpc_cmem_free(m);
  for (auto m : sids) cbmpc_cmem_free(m);
  for (auto m : key_blobs) cbmpc_cmem_free(m);
}

TEST(CApiEcdsaMp, ValidatesArgs) {
  cmem_t key{reinterpret_cast<uint8_t*>(0x1), 123};
  cmem_t sid{reinterpret_cast<uint8_t*>(0x1), 123};

  const cbmpc_transport_t bad_transport = {/*ctx=*/nullptr, /*send=*/nullptr, /*receive=*/nullptr,
                                           /*receive_all=*/nullptr,
                                           /*free=*/nullptr};
  const char* names[2] = {"p0", "p1"};
  const cbmpc_mp_job_t bad_job = {/*self=*/0, /*party_names=*/names, /*party_names_count=*/2,
                                  /*transport=*/&bad_transport};

  EXPECT_EQ(cbmpc_ecdsa_mp_dkg_additive(&bad_job, CBMPC_CURVE_SECP256K1, &key, &sid), E_BADARG);
  EXPECT_EQ(key.data, nullptr);
  EXPECT_EQ(key.size, 0);
  EXPECT_EQ(sid.data, nullptr);
  EXPECT_EQ(sid.size, 0);

  // Missing sig_der_out is invalid.
  EXPECT_EQ(cbmpc_ecdsa_mp_sign_additive(nullptr, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, 0, nullptr), E_BADARG);
}
