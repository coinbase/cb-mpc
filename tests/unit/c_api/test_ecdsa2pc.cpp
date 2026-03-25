#include <atomic>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

#include <cbmpc/c_api/ecdsa_2p.h>
#include <cbmpc/c_api/job.h>
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

// ------------ Disclaimer: All the following tests have been generated by AI ------------

// ==========================================================================
// Negative test helpers
// ==========================================================================

namespace {

static cbmpc_error_t noop_send(void*, int32_t, const uint8_t*, int) { return E_GENERAL; }
static cbmpc_error_t noop_receive(void*, int32_t, cmem_t*) { return E_GENERAL; }
static cbmpc_error_t noop_receive_all(void*, const int32_t*, int, cmems_t*) { return E_GENERAL; }

static const cbmpc_transport_t noop_capi_transport = {nullptr, noop_send, noop_receive, noop_receive_all, nullptr};

static void capi_generate_key_blobs(cbmpc_curve_id_t curve, cmem_t& blob1, cmem_t& blob2) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  transport_ctx_t ctx1{c1, nullptr};
  transport_ctx_t ctx2{c2, nullptr};

  const cbmpc_transport_t t1 = {&ctx1, transport_send, transport_receive, transport_receive_all, transport_free};
  const cbmpc_transport_t t2 = {&ctx2, transport_send, transport_receive, transport_receive_all, transport_free};

  blob1 = {nullptr, 0};
  blob2 = {nullptr, 0};
  cbmpc_error_t rv1 = UNINITIALIZED_ERROR;
  cbmpc_error_t rv2 = UNINITIALIZED_ERROR;

  const cbmpc_2pc_job_t job1 = {CBMPC_2PC_P1, "p1", "p2", &t1};
  const cbmpc_2pc_job_t job2 = {CBMPC_2PC_P2, "p1", "p2", &t2};
  run_2pc(
      c1, c2, [&] { return cbmpc_ecdsa_2p_dkg(&job1, curve, &blob1); },
      [&] { return cbmpc_ecdsa_2p_dkg(&job2, curve, &blob2); }, rv1, rv2);
  ASSERT_EQ(rv1, CBMPC_SUCCESS);
  ASSERT_EQ(rv2, CBMPC_SUCCESS);
  ASSERT_GT(blob1.size, 0);
  ASSERT_GT(blob2.size, 0);
}

}  // namespace

class CApiEcdsa2pcNegWithBlobs : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { capi_generate_key_blobs(CBMPC_CURVE_SECP256K1, blob1_, blob2_); }

  static void TearDownTestSuite() {
    cbmpc_cmem_free(blob1_);
    cbmpc_cmem_free(blob2_);
    blob1_ = {nullptr, 0};
    blob2_ = {nullptr, 0};
  }

  static cmem_t blob1_;
  static cmem_t blob2_;
};

cmem_t CApiEcdsa2pcNegWithBlobs::blob1_ = {nullptr, 0};
cmem_t CApiEcdsa2pcNegWithBlobs::blob2_ = {nullptr, 0};

// ==========================================================================
// Negative: dkg
// ==========================================================================

TEST(CApiEcdsa2pc, NegDkgNullOutput) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &noop_capi_transport};
  EXPECT_EQ(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_SECP256K1, nullptr), E_BADARG);
}

TEST(CApiEcdsa2pc, NegDkgNullJob) {
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_dkg(nullptr, CBMPC_CURVE_SECP256K1, &out), CBMPC_SUCCESS);
  EXPECT_EQ(out.data, nullptr);
}

TEST(CApiEcdsa2pc, NegDkgInvalidJobFields) {
  cmem_t out{nullptr, 0};

  {
    const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", nullptr};
    EXPECT_EQ(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_SECP256K1, &out), E_BADARG);
  }
  {
    const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, nullptr, "p2", &noop_capi_transport};
    EXPECT_EQ(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_SECP256K1, &out), E_BADARG);
  }
  {
    const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", nullptr, &noop_capi_transport};
    EXPECT_EQ(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_SECP256K1, &out), E_BADARG);
  }
  {
    const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "", "p2", &noop_capi_transport};
    EXPECT_EQ(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_SECP256K1, &out), E_BADARG);
  }
  {
    const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "", &noop_capi_transport};
    EXPECT_EQ(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_SECP256K1, &out), E_BADARG);
  }
  {
    const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "same", "same", &noop_capi_transport};
    EXPECT_EQ(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_SECP256K1, &out), E_BADARG);
  }
  {
    cbmpc_transport_t bad_t = noop_capi_transport;
    bad_t.send = nullptr;
    const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &bad_t};
    EXPECT_EQ(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_SECP256K1, &out), E_BADARG);
  }
  {
    cbmpc_transport_t bad_t = noop_capi_transport;
    bad_t.receive = nullptr;
    const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &bad_t};
    EXPECT_EQ(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_SECP256K1, &out), E_BADARG);
  }
}

TEST(CApiEcdsa2pc, NegDkgInvalidCurves) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &noop_capi_transport};

  {
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_ED25519, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  for (int val : {0, 4, 100, 255}) {
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_dkg(&job, static_cast<cbmpc_curve_id_t>(val), &out), CBMPC_SUCCESS)
        << "Expected failure for curve_id=" << val;
    EXPECT_EQ(out.data, nullptr);
  }
}

TEST(CApiEcdsa2pc, NegDkgInvalidParty) {
  const cbmpc_2pc_job_t job = {static_cast<cbmpc_2pc_party_t>(5), "p1", "p2", &noop_capi_transport};
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_dkg(&job, CBMPC_CURVE_SECP256K1, &out), CBMPC_SUCCESS);
  EXPECT_EQ(out.data, nullptr);
}

// ==========================================================================
// Negative: get_public_key_compressed
// ==========================================================================

TEST(CApiEcdsa2pc, NegGetPubKeyNullOutput) {
  uint8_t dummy[] = {0x01};
  EXPECT_EQ(cbmpc_ecdsa_2p_get_public_key_compressed(cmem_t{dummy, 1}, nullptr), E_BADARG);
}

TEST(CApiEcdsa2pc, NegGetPubKeyBadBlob) {
  {
    uint8_t zeros[64] = {};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_get_public_key_compressed(cmem_t{zeros, 64}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    uint8_t one = 0x00;
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_get_public_key_compressed(cmem_t{&one, 1}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_get_public_key_compressed(cmem_t{nullptr, 0}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_get_public_key_compressed(cmem_t{garbage, 4}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    uint8_t data[] = {0x01};
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_ecdsa_2p_get_public_key_compressed(cmem_t{data, -1}, &out), E_BADARG);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_ecdsa_2p_get_public_key_compressed(cmem_t{nullptr, 10}, &out), E_BADARG);
    EXPECT_EQ(out.data, nullptr);
  }
}

// ==========================================================================
// Negative: get_public_share_compressed
// ==========================================================================

TEST(CApiEcdsa2pc, NegGetPubShareNullOutput) {
  uint8_t dummy[] = {0x01};
  EXPECT_EQ(cbmpc_ecdsa_2p_get_public_share_compressed(cmem_t{dummy, 1}, nullptr), E_BADARG);
}

TEST(CApiEcdsa2pc, NegGetPubShareBadBlob) {
  {
    uint8_t zeros[64] = {};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_get_public_share_compressed(cmem_t{zeros, 64}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_get_public_share_compressed(cmem_t{nullptr, 0}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_get_public_share_compressed(cmem_t{garbage, 4}, &out), CBMPC_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
  }
  {
    uint8_t data[] = {0x01};
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_ecdsa_2p_get_public_share_compressed(cmem_t{data, -1}, &out), E_BADARG);
    EXPECT_EQ(out.data, nullptr);
  }
}

// ==========================================================================
// Negative: detach_private_scalar
// ==========================================================================

TEST(CApiEcdsa2pc, NegDetachNullOutputs) {
  uint8_t dummy[] = {0x01};
  cmem_t blob = {dummy, 1};
  cmem_t out1{nullptr, 0};
  cmem_t out2{nullptr, 0};

  EXPECT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob, nullptr, &out2), E_BADARG);
  EXPECT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob, &out1, nullptr), E_BADARG);
  EXPECT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob, nullptr, nullptr), E_BADARG);
}

TEST(CApiEcdsa2pc, NegDetachBadBlob) {
  {
    uint8_t zeros[64] = {};
    cmem_t pub{nullptr, 0};
    cmem_t scalar{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_detach_private_scalar(cmem_t{zeros, 64}, &pub, &scalar), CBMPC_SUCCESS);
    EXPECT_EQ(pub.data, nullptr);
    EXPECT_EQ(scalar.data, nullptr);
  }
  {
    cmem_t pub{nullptr, 0};
    cmem_t scalar{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_detach_private_scalar(cmem_t{nullptr, 0}, &pub, &scalar), CBMPC_SUCCESS);
    EXPECT_EQ(pub.data, nullptr);
    EXPECT_EQ(scalar.data, nullptr);
  }
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    cmem_t pub{nullptr, 0};
    cmem_t scalar{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_detach_private_scalar(cmem_t{garbage, 4}, &pub, &scalar), CBMPC_SUCCESS);
    EXPECT_EQ(pub.data, nullptr);
    EXPECT_EQ(scalar.data, nullptr);
  }
  {
    uint8_t data[] = {0x01};
    cmem_t pub{nullptr, 0};
    cmem_t scalar{nullptr, 0};
    EXPECT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(cmem_t{data, -1}, &pub, &scalar), E_BADARG);
    EXPECT_EQ(pub.data, nullptr);
    EXPECT_EQ(scalar.data, nullptr);
  }
}

// ==========================================================================
// Negative: attach_private_scalar
// ==========================================================================

TEST(CApiEcdsa2pc, NegAttachNullOutput) {
  uint8_t dummy[] = {0x01};
  EXPECT_EQ(cbmpc_ecdsa_2p_attach_private_scalar(cmem_t{dummy, 1}, cmem_t{dummy, 1}, cmem_t{dummy, 1}, nullptr),
            E_BADARG);
}

TEST(CApiEcdsa2pc, NegAttachBadCmemInputs) {
  cmem_t out{nullptr, 0};

  {
    uint8_t scalar[] = {0x01};
    uint8_t point[33] = {};
    point[0] = 0x02;
    EXPECT_NE(cbmpc_ecdsa_2p_attach_private_scalar(cmem_t{nullptr, 0}, cmem_t{scalar, 1}, cmem_t{point, 33}, &out),
              CBMPC_SUCCESS);
  }
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t scalar[] = {0x01};
    uint8_t point[33] = {};
    point[0] = 0x02;
    EXPECT_NE(cbmpc_ecdsa_2p_attach_private_scalar(cmem_t{garbage, 4}, cmem_t{scalar, 1}, cmem_t{point, 33}, &out),
              CBMPC_SUCCESS);
  }
  {
    uint8_t data[] = {0x01};
    EXPECT_EQ(cbmpc_ecdsa_2p_attach_private_scalar(cmem_t{data, -1}, cmem_t{data, 1}, cmem_t{data, 1}, &out), E_BADARG);
  }
  {
    uint8_t data[] = {0x01};
    EXPECT_EQ(cbmpc_ecdsa_2p_attach_private_scalar(cmem_t{data, 1}, cmem_t{data, -1}, cmem_t{data, 1}, &out), E_BADARG);
  }
  {
    uint8_t data[] = {0x01};
    EXPECT_EQ(cbmpc_ecdsa_2p_attach_private_scalar(cmem_t{data, 1}, cmem_t{data, 1}, cmem_t{data, -1}, &out), E_BADARG);
  }
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegAttachEmptyPrivateScalar) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob1_, &pub, &x), CBMPC_SUCCESS);

  cmem_t Qi{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_get_public_share_compressed(blob1_, &Qi), CBMPC_SUCCESS);

  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_attach_private_scalar(pub, cmem_t{nullptr, 0}, Qi, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
  cbmpc_cmem_free(Qi);
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegAttachGarbagePrivateScalar) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob1_, &pub, &x), CBMPC_SUCCESS);

  cmem_t Qi{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_get_public_share_compressed(blob1_, &Qi), CBMPC_SUCCESS);

  uint8_t garbage[512];
  std::memset(garbage, 0xFF, sizeof(garbage));
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_attach_private_scalar(pub, cmem_t{garbage, 512}, Qi, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
  cbmpc_cmem_free(Qi);
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegAttachGarbagePublicShare) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob1_, &pub, &x), CBMPC_SUCCESS);

  uint8_t bad_point[33];
  bad_point[0] = 0x05;
  std::memset(bad_point + 1, 0xAB, 32);
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_attach_private_scalar(pub, x, cmem_t{bad_point, 33}, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegAttachSwappedScalars) {
  cmem_t pub1{nullptr, 0};
  cmem_t x1{nullptr, 0};
  cmem_t pub2{nullptr, 0};
  cmem_t x2{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob1_, &pub1, &x1), CBMPC_SUCCESS);
  ASSERT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob2_, &pub2, &x2), CBMPC_SUCCESS);

  cmem_t Qi1{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_get_public_share_compressed(blob1_, &Qi1), CBMPC_SUCCESS);

  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_attach_private_scalar(pub1, x2, Qi1, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub1);
  cbmpc_cmem_free(x1);
  cbmpc_cmem_free(pub2);
  cbmpc_cmem_free(x2);
  cbmpc_cmem_free(Qi1);
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegAttachSwappedPublicShares) {
  cmem_t pub1{nullptr, 0};
  cmem_t x1{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob1_, &pub1, &x1), CBMPC_SUCCESS);

  cmem_t Qi2{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_get_public_share_compressed(blob2_, &Qi2), CBMPC_SUCCESS);

  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_attach_private_scalar(pub1, x1, Qi2, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub1);
  cbmpc_cmem_free(x1);
  cbmpc_cmem_free(Qi2);
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegAttachZeroScalar) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob1_, &pub, &x), CBMPC_SUCCESS);

  cmem_t Qi{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_get_public_share_compressed(blob1_, &Qi), CBMPC_SUCCESS);

  uint8_t zero[32] = {};
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_attach_private_scalar(pub, cmem_t{zero, 32}, Qi, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
  cbmpc_cmem_free(Qi);
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegAttachSingleByteZeroScalar) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob1_, &pub, &x), CBMPC_SUCCESS);

  cmem_t Qi{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_get_public_share_compressed(blob1_, &Qi), CBMPC_SUCCESS);

  uint8_t zero_byte = 0x00;
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_attach_private_scalar(pub, cmem_t{&zero_byte, 1}, Qi, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
  cbmpc_cmem_free(Qi);
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegAttachAllZeroPublicShare) {
  cmem_t pub{nullptr, 0};
  cmem_t x{nullptr, 0};
  ASSERT_EQ(cbmpc_ecdsa_2p_detach_private_scalar(blob1_, &pub, &x), CBMPC_SUCCESS);

  uint8_t zero_point[33] = {};
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_attach_private_scalar(pub, x, cmem_t{zero_point, 33}, &out), CBMPC_SUCCESS);

  cbmpc_cmem_free(pub);
  cbmpc_cmem_free(x);
}

// ==========================================================================
// Negative: sign
// ==========================================================================

TEST(CApiEcdsa2pc, NegSignNullSigOutput) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &noop_capi_transport};
  uint8_t hash[32] = {};
  EXPECT_EQ(cbmpc_ecdsa_2p_sign(&job, cmem_t{nullptr, 0}, cmem_t{hash, 32}, cmem_t{nullptr, 0}, nullptr, nullptr),
            E_BADARG);
}

TEST(CApiEcdsa2pc, NegSignNullJob) {
  uint8_t hash[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_sign(nullptr, cmem_t{nullptr, 0}, cmem_t{hash, 32}, cmem_t{nullptr, 0}, nullptr, &sig),
            CBMPC_SUCCESS);
}

TEST(CApiEcdsa2pc, NegSignBadKeyBlob) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &noop_capi_transport};
  uint8_t hash[32] = {};

  {
    cmem_t sig{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_sign(&job, cmem_t{nullptr, 0}, cmem_t{hash, 32}, cmem_t{nullptr, 0}, nullptr, &sig),
              CBMPC_SUCCESS);
  }
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    cmem_t sig{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_sign(&job, cmem_t{garbage, 4}, cmem_t{hash, 32}, cmem_t{nullptr, 0}, nullptr, &sig),
              CBMPC_SUCCESS);
  }
  {
    uint8_t data[] = {0x01};
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_ecdsa_2p_sign(&job, cmem_t{data, -1}, cmem_t{hash, 32}, cmem_t{nullptr, 0}, nullptr, &sig),
              E_BADARG);
  }
  {
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_ecdsa_2p_sign(&job, cmem_t{nullptr, 10}, cmem_t{hash, 32}, cmem_t{nullptr, 0}, nullptr, &sig),
              E_BADARG);
  }
  {
    uint8_t zeros[64] = {};
    cmem_t sig{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_sign(&job, cmem_t{zeros, 64}, cmem_t{hash, 32}, cmem_t{nullptr, 0}, nullptr, &sig),
              CBMPC_SUCCESS);
  }
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegSignEmptyMsgHash) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &noop_capi_transport};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_sign(&job, blob1_, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, nullptr, &sig), CBMPC_SUCCESS);
}

TEST(CApiEcdsa2pc, NegSignBadMsgHash) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &noop_capi_transport};
  uint8_t dummy_blob[] = {0x01};

  {
    uint8_t data[] = {0x01};
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_ecdsa_2p_sign(&job, cmem_t{dummy_blob, 1}, cmem_t{data, -1}, cmem_t{nullptr, 0}, nullptr, &sig),
              E_BADARG);
  }
  {
    cmem_t sig{nullptr, 0};
    EXPECT_EQ(cbmpc_ecdsa_2p_sign(&job, cmem_t{dummy_blob, 1}, cmem_t{nullptr, 10}, cmem_t{nullptr, 0}, nullptr, &sig),
              E_BADARG);
  }
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegSignOversizedMsgHash) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &noop_capi_transport};
  uint8_t huge_hash[65];
  std::memset(huge_hash, 0x42, sizeof(huge_hash));
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_sign(&job, blob1_, cmem_t{huge_hash, 65}, cmem_t{nullptr, 0}, nullptr, &sig), CBMPC_SUCCESS);
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegSignRoleMismatch) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P2, "p1", "p2", &noop_capi_transport};
  uint8_t hash[32] = {};
  cmem_t sig{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_sign(&job, blob1_, cmem_t{hash, 32}, cmem_t{nullptr, 0}, nullptr, &sig), CBMPC_SUCCESS);
}

// ==========================================================================
// Negative: refresh
// ==========================================================================

TEST(CApiEcdsa2pc, NegRefreshNullOutput) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &noop_capi_transport};
  uint8_t dummy[] = {0x01};
  EXPECT_EQ(cbmpc_ecdsa_2p_refresh(&job, cmem_t{dummy, 1}, nullptr), E_BADARG);
}

TEST(CApiEcdsa2pc, NegRefreshNullJob) {
  uint8_t dummy[] = {0x01};
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_refresh(nullptr, cmem_t{dummy, 1}, &out), CBMPC_SUCCESS);
}

TEST(CApiEcdsa2pc, NegRefreshBadKeyBlob) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &noop_capi_transport};

  {
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_refresh(&job, cmem_t{nullptr, 0}, &out), CBMPC_SUCCESS);
  }
  {
    uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_refresh(&job, cmem_t{garbage, 4}, &out), CBMPC_SUCCESS);
  }
  {
    uint8_t data[] = {0x01};
    cmem_t out{nullptr, 0};
    EXPECT_EQ(cbmpc_ecdsa_2p_refresh(&job, cmem_t{data, -1}, &out), E_BADARG);
  }
  {
    uint8_t zeros[64] = {};
    cmem_t out{nullptr, 0};
    EXPECT_NE(cbmpc_ecdsa_2p_refresh(&job, cmem_t{zeros, 64}, &out), CBMPC_SUCCESS);
  }
}

TEST_F(CApiEcdsa2pcNegWithBlobs, NegRefreshRoleMismatch) {
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P2, "p1", "p2", &noop_capi_transport};
  cmem_t out{nullptr, 0};
  EXPECT_NE(cbmpc_ecdsa_2p_refresh(&job, blob1_, &out), CBMPC_SUCCESS);
}
