#include <atomic>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

#include <cbmpc/capi/ecdsa_2p.h>
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

template <typename F1, typename F2>
static void run_2pc(const std::shared_ptr<mpc_net_context_t>& c1, const std::shared_ptr<mpc_net_context_t>& c2, F1&& f1,
                    F2&& f2, cbmpc_error_t& out_rv1, cbmpc_error_t& out_rv2) {
  c1->reset();
  c2->reset();

  std::atomic<bool> aborted{false};

  std::thread t1([&] {
    out_rv1 = f1();
    if (out_rv1 && !aborted.exchange(true)) {
      c1->abort();
      c2->abort();
    }
  });
  std::thread t2([&] {
    out_rv2 = f2();
    if (out_rv2 && !aborted.exchange(true)) {
      c1->abort();
      c2->abort();
    }
  });

  t1.join();
  t2.join();
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

TEST(CApiEcdsa2pc, DkgSignRefreshSign) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  std::atomic<int> free_calls_1{0};
  std::atomic<int> free_calls_2{0};
  transport_ctx_t ctx1{c1, &free_calls_1};
  transport_ctx_t ctx2{c2, &free_calls_2};

  const cbmpc_transport_t t1 = {
      /*ctx=*/&ctx1,
      /*send=*/transport_send,
      /*receive=*/transport_receive,
      /*receive_all=*/transport_receive_all,
      /*free=*/transport_free,
  };
  const cbmpc_transport_t t2 = {
      /*ctx=*/&ctx2,
      /*send=*/transport_send,
      /*receive=*/transport_receive,
      /*receive_all=*/transport_receive_all,
      /*free=*/transport_free,
  };

  cmem_t key_blob_1{nullptr, 0};
  cmem_t key_blob_2{nullptr, 0};
  cbmpc_error_t rv1 = UNINITIALIZED_ERROR;
  cbmpc_error_t rv2 = UNINITIALIZED_ERROR;

  const cbmpc_2pc_job_t job1 = {CBMPC_2PC_P1, "p1", "p2", &t1};
  const cbmpc_2pc_job_t job2 = {CBMPC_2PC_P2, "p1", "p2", &t2};
  run_2pc(
      c1, c2, [&] { return cbmpc_ecdsa_2p_dkg(&job1, CBMPC_CURVE_SECP256K1, &key_blob_1); },
      [&] { return cbmpc_ecdsa_2p_dkg(&job2, CBMPC_CURVE_SECP256K1, &key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, CBMPC_SUCCESS);
  ASSERT_EQ(rv2, CBMPC_SUCCESS);
  ASSERT_GT(key_blob_1.size, 0);
  ASSERT_GT(key_blob_2.size, 0);

  cmem_t pub1{nullptr, 0};
  cmem_t pub2{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_get_public_key_compressed(key_blob_1, &pub1), CBMPC_SUCCESS);
  ASSERT_EQ(cbmpc_ecdsa_2p_get_public_key_compressed(key_blob_2, &pub2), CBMPC_SUCCESS);
  expect_eq(pub1, pub2);

  uint8_t msg_hash_bytes[32];
  for (int i = 0; i < 32; i++) msg_hash_bytes[i] = static_cast<uint8_t>(i);
  const cmem_t msg_hash = {msg_hash_bytes, 32};
  const cmem_t sid_in = {nullptr, 0};

  cmem_t sid_out1{nullptr, 0};
  cmem_t sid_out2{nullptr, 0};
  cmem_t sig1{nullptr, 0};
  cmem_t sig2{nullptr, 0};

  run_2pc(
      c1, c2, [&] { return cbmpc_ecdsa_2p_sign(&job1, key_blob_1, msg_hash, sid_in, &sid_out1, &sig1); },
      [&] { return cbmpc_ecdsa_2p_sign(&job2, key_blob_2, msg_hash, sid_in, &sid_out2, &sig2); }, rv1, rv2);
  ASSERT_EQ(rv1, CBMPC_SUCCESS);
  ASSERT_EQ(rv2, CBMPC_SUCCESS);
  ASSERT_GT(sig1.size, 0);
  ASSERT_EQ(sig2.size, 0);
  expect_eq(sid_out1, sid_out2);

  // Verify signature against the returned public key.
  const buf_t pub_buf(pub1.data, pub1.size);
  const buf_t hash_buf(msg_hash_bytes, 32);
  const buf_t sig_buf(sig1.data, sig1.size);
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_secp256k1, pub_buf), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);
  ASSERT_EQ(verify_key.verify(hash_buf, sig_buf), SUCCESS);

  cmem_t new_key_blob_1{nullptr, 0};
  cmem_t new_key_blob_2{nullptr, 0};
  run_2pc(
      c1, c2, [&] { return cbmpc_ecdsa_2p_refresh(&job1, key_blob_1, &new_key_blob_1); },
      [&] { return cbmpc_ecdsa_2p_refresh(&job2, key_blob_2, &new_key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, CBMPC_SUCCESS);
  ASSERT_EQ(rv2, CBMPC_SUCCESS);

  cmem_t pub3{nullptr, 0};
  cmem_t pub4{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_get_public_key_compressed(new_key_blob_1, &pub3), CBMPC_SUCCESS);
  ASSERT_EQ(cbmpc_ecdsa_2p_get_public_key_compressed(new_key_blob_2, &pub4), CBMPC_SUCCESS);
  expect_eq(pub3, pub4);
  expect_eq(pub1, pub3);

  // The adapter must free buffers returned by receive/receive_all using our callback.
  EXPECT_GT(free_calls_1.load(), 0);
  EXPECT_GT(free_calls_2.load(), 0);

  cbmpc_cmem_free(pub1);
  cbmpc_cmem_free(pub2);
  cbmpc_cmem_free(pub3);
  cbmpc_cmem_free(pub4);
  cbmpc_cmem_free(sig1);
  cbmpc_cmem_free(sig2);
  cbmpc_cmem_free(sid_out1);
  cbmpc_cmem_free(sid_out2);
  cbmpc_cmem_free(key_blob_1);
  cbmpc_cmem_free(key_blob_2);
  cbmpc_cmem_free(new_key_blob_1);
  cbmpc_cmem_free(new_key_blob_2);
}

TEST(CApiEcdsa2pc, ValidatesArgs) {
  cmem_t out{reinterpret_cast<uint8_t*>(0x1), 123};
  const cbmpc_2pc_job_t bad_job = {CBMPC_2PC_P1, "p1", "p2", nullptr};
  EXPECT_EQ(cbmpc_ecdsa_2p_dkg(&bad_job, CBMPC_CURVE_SECP256K1, &out), E_BADARG);
  EXPECT_EQ(out.data, nullptr);
  EXPECT_EQ(out.size, 0);

  // Missing sig_der_out is invalid.
  EXPECT_EQ(cbmpc_ecdsa_2p_sign(nullptr, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, nullptr, nullptr),
            E_BADARG);
}
