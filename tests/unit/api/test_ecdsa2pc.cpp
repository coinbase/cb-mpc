#include <atomic>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/ecdsa_2p.h>
#include <cbmpc/core/job.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/crypto/base_ecc.h>
#include <cbmpc/internal/crypto/base_paillier.h>

#include "utils/local_network/network_context.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;

using coinbase::api::curve_id;
using coinbase::api::data_transport_i;
using coinbase::api::party_idx_t;

using coinbase::api::ecdsa_2p::party_t;

using coinbase::testutils::mpc_net_context_t;

// Key blob codec (mirrors src/cbmpc/api/ecdsa2pc.cpp).
struct key_blob_v1_t {
  uint32_t version = 1;
  uint32_t role = 0;   // 0=p1, 1=p2
  uint32_t curve = 0;  // coinbase::api::curve_id

  buf_t Q_compressed;
  coinbase::crypto::bn_t x_share;
  coinbase::crypto::bn_t c_key;
  coinbase::crypto::paillier_t paillier;

  void convert(coinbase::converter_t& c) { c.convert(version, role, curve, Q_compressed, x_share, c_key, paillier); }
};

class local_api_transport_t final : public data_transport_i {
 public:
  explicit local_api_transport_t(std::shared_ptr<mpc_net_context_t> ctx) : ctx_(std::move(ctx)) {}

  error_t send(party_idx_t receiver, mem_t msg) override {
    ctx_->send(receiver, msg);
    return SUCCESS;
  }

  error_t receive(party_idx_t sender, buf_t& msg) override { return ctx_->receive(sender, msg); }

  error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& msgs) override {
    std::vector<coinbase::mpc::party_idx_t> s;
    s.reserve(senders.size());
    for (auto x : senders) s.push_back(static_cast<coinbase::mpc::party_idx_t>(x));
    return ctx_->receive_all(s, msgs);
  }

 private:
  std::shared_ptr<mpc_net_context_t> ctx_;
};

template <typename F1, typename F2>
static void run_2pc(const std::shared_ptr<mpc_net_context_t>& c1, const std::shared_ptr<mpc_net_context_t>& c2, F1&& f1,
                    F2&& f2, error_t& out_rv1, error_t& out_rv2) {
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

static void exercise_curve(curve_id curve, const coinbase::crypto::ecurve_t& verify_curve) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  local_api_transport_t t1(c1);
  local_api_transport_t t2(c2);

  buf_t key_blob_1;
  buf_t key_blob_2;
  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;

  const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};
  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::dkg(job1, curve, key_blob_1); },
      [&] { return coinbase::api::ecdsa_2p::dkg(job2, curve, key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t pub1;
  buf_t pub2;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(key_blob_1, pub1), SUCCESS);
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(key_blob_2, pub2), SUCCESS);
  ASSERT_EQ(pub1, pub2);

  // Deterministic 32-byte "hash" for testing.
  buf_t msg_hash(32);
  for (int i = 0; i < msg_hash.size(); i++) msg_hash[i] = static_cast<uint8_t>(i);

  buf_t sid1;
  buf_t sid2;
  buf_t sig1;
  buf_t sig2;

  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::sign(job1, key_blob_1, msg_hash, sid1, sig1); },
      [&] { return coinbase::api::ecdsa_2p::sign(job2, key_blob_2, msg_hash, sid2, sig2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(sid1, sid2);
  EXPECT_GT(sig1.size(), 0);
  EXPECT_EQ(sig2.size(), 0);

  // Verify the returned signature against the extracted public key.
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(verify_curve, pub1), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);
  ASSERT_EQ(verify_key.verify(msg_hash, sig1), SUCCESS);

  // Refresh shares, ensuring public key stays the same.
  buf_t new_key_blob_1;
  buf_t new_key_blob_2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::refresh(job1, key_blob_1, new_key_blob_1); },
      [&] { return coinbase::api::ecdsa_2p::refresh(job2, key_blob_2, new_key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t new_pub1;
  buf_t new_pub2;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(new_key_blob_1, new_pub1), SUCCESS);
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(new_key_blob_2, new_pub2), SUCCESS);
  EXPECT_EQ(new_pub1, pub1);
  EXPECT_EQ(new_pub2, pub2);
  EXPECT_EQ(new_pub1, new_pub2);

  // Sign again with refreshed shares.
  buf_t sid3;
  buf_t sid4;
  buf_t sig3;
  buf_t sig4;
  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::sign(job1, new_key_blob_1, msg_hash, sid3, sig3); },
      [&] { return coinbase::api::ecdsa_2p::sign(job2, new_key_blob_2, msg_hash, sid4, sig4); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(sid3, sid4);
  EXPECT_GT(sig3.size(), 0);
  EXPECT_EQ(sig4.size(), 0);
  ASSERT_EQ(verify_key.verify(msg_hash, sig3), SUCCESS);
}

static void exercise_detach_attach(curve_id curve, const coinbase::crypto::ecurve_t& verify_curve) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  local_api_transport_t t1(c1);
  local_api_transport_t t2(c2);

  buf_t key_blob_1;
  buf_t key_blob_2;
  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;

  const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};
  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::dkg(job1, curve, key_blob_1); },
      [&] { return coinbase::api::ecdsa_2p::dkg(job2, curve, key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  // Refresh (exercise detach/attach on refreshed blobs too).
  buf_t refreshed_1;
  buf_t refreshed_2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::refresh(job1, key_blob_1, refreshed_1); },
      [&] { return coinbase::api::ecdsa_2p::refresh(job2, key_blob_2, refreshed_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t pub1;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(refreshed_1, pub1), SUCCESS);
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(verify_curve, pub1), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  // Detach into scalar-redacted blob + variable-length scalar.
  buf_t public_1;
  buf_t public_2;
  buf_t x1;
  buf_t x2;
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(refreshed_1, public_1, x1), SUCCESS);
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(refreshed_2, public_2, x2), SUCCESS);
  EXPECT_GT(public_1.size(), 0);
  EXPECT_GT(public_2.size(), 0);
  EXPECT_GT(x1.size(), 0);
  EXPECT_GT(x2.size(), 0);

  // Capture share points before detaching (public blobs no longer carry them).
  buf_t Qi_full_1;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_share_compressed(refreshed_1, Qi_full_1), SUCCESS);

  buf_t Qi_full_2;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_share_compressed(refreshed_2, Qi_full_2), SUCCESS);

  // Public blob should not be usable for signing.
  buf_t msg_hash(32);
  for (int i = 0; i < msg_hash.size(); i++) msg_hash[i] = static_cast<uint8_t>(i);
  {
    class unused_transport_t final : public data_transport_i {
     public:
      error_t send(party_idx_t /*receiver*/, mem_t /*msg*/) override { return E_GENERAL; }
      error_t receive(party_idx_t /*sender*/, buf_t& /*msg*/) override { return E_GENERAL; }
      error_t receive_all(const std::vector<party_idx_t>& /*senders*/, std::vector<buf_t>& /*msgs*/) override {
        return E_GENERAL;
      }
    };
    unused_transport_t t;
    const coinbase::api::job_2p_t bad_job{party_t::p1, "p1", "p2", t};
    buf_t sid;
    buf_t sig;
    EXPECT_NE(coinbase::api::ecdsa_2p::sign(bad_job, public_1, msg_hash, sid, sig), SUCCESS);
  }

  // Attach scalars back and sign.
  buf_t merged_1;
  buf_t merged_2;
  ASSERT_EQ(coinbase::api::ecdsa_2p::attach_private_scalar(public_1, x1, Qi_full_1, merged_1), SUCCESS);
  ASSERT_EQ(coinbase::api::ecdsa_2p::attach_private_scalar(public_2, x2, Qi_full_2, merged_2), SUCCESS);

  // Leading-zero padded encoding should also be accepted (variable-length encoding).
  buf_t x1_padded(static_cast<int>(x1.size()) + 1);
  x1_padded[0] = 0x00;
  std::memcpy(x1_padded.data() + 1, x1.data(), static_cast<size_t>(x1.size()));
  buf_t merged_1_padded;
  ASSERT_EQ(coinbase::api::ecdsa_2p::attach_private_scalar(public_1, x1_padded, Qi_full_1, merged_1_padded), SUCCESS);

  buf_t sid1;
  buf_t sid2;
  buf_t sig1;
  buf_t sig2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::sign(job1, merged_1, msg_hash, sid1, sig1); },
      [&] { return coinbase::api::ecdsa_2p::sign(job2, merged_2, msg_hash, sid2, sig2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(sid1, sid2);
  EXPECT_GT(sig1.size(), 0);
  EXPECT_EQ(sig2.size(), 0);
  ASSERT_EQ(verify_key.verify(msg_hash, sig1), SUCCESS);

  // Negative: wrong scalar should fail to attach.
  buf_t bad_x1 = x1;
  bad_x1[0] ^= 0x01;
  buf_t bad_merged;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(public_1, bad_x1, Qi_full_1, bad_merged), SUCCESS);
}

}  // namespace

TEST(ApiEcdsa2pc, DkgSignRefreshSign) {
  exercise_curve(curve_id::secp256k1, coinbase::crypto::curve_secp256k1);
  exercise_curve(curve_id::p256, coinbase::crypto::curve_p256);
}

TEST(ApiEcdsa2pc, KeyBlobPrivScalar_NoPubSign) {
  exercise_detach_attach(curve_id::secp256k1, coinbase::crypto::curve_secp256k1);
  exercise_detach_attach(curve_id::p256, coinbase::crypto::curve_p256);
}

TEST(ApiEcdsa2pc, UnsupportedCurveRejected) {
  class unused_transport_t final : public data_transport_i {
   public:
    error_t send(party_idx_t /*receiver*/, mem_t /*msg*/) override { return E_GENERAL; }
    error_t receive(party_idx_t /*sender*/, buf_t& /*msg*/) override { return E_GENERAL; }
    error_t receive_all(const std::vector<party_idx_t>& /*senders*/, std::vector<buf_t>& /*msgs*/) override {
      return E_GENERAL;
    }
  };

  unused_transport_t t;
  buf_t key_blob;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  const error_t rv = coinbase::api::ecdsa_2p::dkg(job, static_cast<curve_id>(42), key_blob);
  EXPECT_EQ(rv, E_BADARG);
}

TEST(ApiEcdsa2pc, InvalidKeyBlobRejected) {
  buf_t pub_key;
  const error_t rv = coinbase::api::ecdsa_2p::get_public_key_compressed(mem_t(), pub_key);
  EXPECT_NE(rv, SUCCESS);
}

TEST(ApiEcdsa2pc, RejectTamperedP1ShareBinding) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  local_api_transport_t t1(c1);
  local_api_transport_t t2(c2);

  buf_t key_blob_1;
  buf_t key_blob_2;
  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;

  const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};
  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::dkg(job1, curve_id::secp256k1, key_blob_1); },
      [&] { return coinbase::api::ecdsa_2p::dkg(job2, curve_id::secp256k1, key_blob_2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  key_blob_v1_t blob;
  ASSERT_EQ(coinbase::convert(blob, key_blob_1), SUCCESS);

  // Tamper with P1's share while keeping `c_key` unchanged; deserialization should reject it.
  blob.x_share = blob.x_share + 1;
  buf_t malformed = coinbase::convert(blob);

  buf_t pub_key;
  EXPECT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(malformed, pub_key), E_FORMAT);
}

TEST(ApiEcdsa2pc, TransportSendFailNoDeadlock) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  class fail_first_send_transport_t final : public data_transport_i {
   public:
    explicit fail_first_send_transport_t(std::shared_ptr<mpc_net_context_t> ctx) : ctx_(std::move(ctx)) {}

    error_t send(party_idx_t /*receiver*/, mem_t /*msg*/) override {
      if (!failed_.exchange(true)) return E_NET_GENERAL;
      return E_NET_GENERAL;
    }

    error_t receive(party_idx_t sender, buf_t& msg) override { return ctx_->receive(sender, msg); }

    error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& msgs) override {
      std::vector<coinbase::mpc::party_idx_t> s;
      s.reserve(senders.size());
      for (auto x : senders) s.push_back(static_cast<coinbase::mpc::party_idx_t>(x));
      return ctx_->receive_all(s, msgs);
    }

   private:
    std::shared_ptr<mpc_net_context_t> ctx_;
    std::atomic<bool> failed_{false};
  };

  fail_first_send_transport_t t1(c1);
  local_api_transport_t t2(c2);

  buf_t key_blob_1;
  buf_t key_blob_2;
  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;

  const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};
  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::dkg(job1, curve_id::secp256k1, key_blob_1); },
      [&] { return coinbase::api::ecdsa_2p::dkg(job2, curve_id::secp256k1, key_blob_2); }, rv1, rv2);

  EXPECT_NE(rv1, SUCCESS);
  EXPECT_NE(rv2, SUCCESS);
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

// ==========================================================================
// Negative test helpers
// ==========================================================================

namespace {

class noop_transport_t final : public data_transport_i {
 public:
  error_t send(party_idx_t, mem_t) override { return E_GENERAL; }
  error_t receive(party_idx_t, buf_t&) override { return E_GENERAL; }
  error_t receive_all(const std::vector<party_idx_t>&, std::vector<buf_t>&) override { return E_GENERAL; }
};

static coinbase::api::job_2p_t make_noop_job(noop_transport_t& t, party_t self = party_t::p1) {
  return {self, "p1", "p2", t};
}

static void generate_key_blobs(curve_id curve, buf_t& blob1, buf_t& blob2) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  local_api_transport_t t1(c1);
  local_api_transport_t t2(c2);

  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;

  const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};
  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::dkg(job1, curve, blob1); },
      [&] { return coinbase::api::ecdsa_2p::dkg(job2, curve, blob2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
}

}  // namespace

class ApiEcdsa2pcNegWithBlobs : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { generate_key_blobs(curve_id::secp256k1, blob1_, blob2_); }

  static buf_t blob1_;
  static buf_t blob2_;
};

buf_t ApiEcdsa2pcNegWithBlobs::blob1_;
buf_t ApiEcdsa2pcNegWithBlobs::blob2_;

// ==========================================================================
// Negative: dkg
// ==========================================================================

TEST(ApiEcdsa2pc, NegDkgEdwardsCurve) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  buf_t key_blob;
  EXPECT_NE(coinbase::api::ecdsa_2p::dkg(job, curve_id::ed25519, key_blob), SUCCESS);
}

TEST(ApiEcdsa2pc, NegDkgInvalidCurveValues) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  for (uint32_t val : {0u, 4u, 100u, 255u}) {
    buf_t key_blob;
    EXPECT_NE(coinbase::api::ecdsa_2p::dkg(job, static_cast<curve_id>(val), key_blob), SUCCESS)
        << "Expected failure for curve_id=" << val;
  }
}

// ==========================================================================
// Negative: get_public_key_compressed
// ==========================================================================

TEST(ApiEcdsa2pc, NegGetPubKeyGarbageBlob) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_2p::get_public_key_compressed(mem_t(garbage, sizeof(garbage)), pub), SUCCESS);
}

TEST(ApiEcdsa2pc, NegGetPubKeyOversizedBlob) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_2p::get_public_key_compressed(big, pub), SUCCESS);
}

TEST(ApiEcdsa2pc, NegGetPubKeyAllZeroBlob) {
  uint8_t zeros[64] = {};
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_2p::get_public_key_compressed(mem_t(zeros, sizeof(zeros)), pub), SUCCESS);
}

TEST(ApiEcdsa2pc, NegGetPubKeyOneByte) {
  uint8_t one = 0x00;
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_2p::get_public_key_compressed(mem_t(&one, 1), pub), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegKeyBlobVersionZero) {
  buf_t tampered(blob1_.size());
  std::memcpy(tampered.data(), blob1_.data(), static_cast<size_t>(blob1_.size()));
  tampered[0] = 0x00;
  tampered[1] = 0x00;
  tampered[2] = 0x00;
  tampered[3] = 0x00;
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_2p::get_public_key_compressed(tampered, pub), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegKeyBlobWrongVersion) {
  buf_t tampered(blob1_.size());
  std::memcpy(tampered.data(), blob1_.data(), static_cast<size_t>(blob1_.size()));
  tampered[0] = 0x00;
  tampered[1] = 0x00;
  tampered[2] = 0x00;
  tampered[3] = 0x02;
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_2p::get_public_key_compressed(tampered, pub), SUCCESS);
}

// ==========================================================================
// Negative: get_public_share_compressed
// ==========================================================================

TEST(ApiEcdsa2pc, NegGetPubShareAllZeroBlob) {
  uint8_t zeros[64] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::get_public_share_compressed(mem_t(zeros, sizeof(zeros)), out), SUCCESS);
}

TEST(ApiEcdsa2pc, NegGetPubShareEmptyBlob) {
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::get_public_share_compressed(mem_t(), out), SUCCESS);
}

TEST(ApiEcdsa2pc, NegGetPubShareGarbageBlob) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::get_public_share_compressed(mem_t(garbage, sizeof(garbage)), out), SUCCESS);
}

TEST(ApiEcdsa2pc, NegGetPubShareOversizedBlob) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::get_public_share_compressed(big, out), SUCCESS);
}

// ==========================================================================
// Negative: detach_private_scalar
// ==========================================================================

TEST(ApiEcdsa2pc, NegDetachAllZeroBlob) {
  uint8_t zeros[64] = {};
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::ecdsa_2p::detach_private_scalar(mem_t(zeros, sizeof(zeros)), pub_blob, scalar), SUCCESS);
}

TEST(ApiEcdsa2pc, NegDetachEmptyBlob) {
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::ecdsa_2p::detach_private_scalar(mem_t(), pub_blob, scalar), SUCCESS);
}

TEST(ApiEcdsa2pc, NegDetachGarbageBlob) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::ecdsa_2p::detach_private_scalar(mem_t(garbage, sizeof(garbage)), pub_blob, scalar), SUCCESS);
}

TEST(ApiEcdsa2pc, NegDetachOversizedBlob) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::ecdsa_2p::detach_private_scalar(big, pub_blob, scalar), SUCCESS);
}

// ==========================================================================
// Negative: attach_private_scalar
// ==========================================================================

TEST(ApiEcdsa2pc, NegAttachEmptyPublicKeyBlob) {
  uint8_t scalar[] = {0x01};
  uint8_t point[33] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(mem_t(), mem_t(scalar, 1), mem_t(point, 33), out), SUCCESS);
}

TEST(ApiEcdsa2pc, NegAttachGarbagePublicKeyBlob) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  uint8_t scalar[] = {0x01};
  uint8_t point[33] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(mem_t(garbage, sizeof(garbage)), mem_t(scalar, 1),
                                                           mem_t(point, 33), out),
            SUCCESS);
}

TEST(ApiEcdsa2pc, NegAttachOversizedPublicKeyBlob) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  uint8_t scalar[] = {0x01};
  uint8_t point[33] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(big, mem_t(scalar, 1), mem_t(point, 33), out), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegAttachEmptyPrivateScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(blob1_, pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_share_compressed(blob1_, Qi), SUCCESS);

  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(pub_blob, mem_t(), Qi, out), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegAttachGarbagePrivateScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(blob1_, pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_share_compressed(blob1_, Qi), SUCCESS);

  uint8_t garbage[512];
  std::memset(garbage, 0xFF, sizeof(garbage));
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(pub_blob, mem_t(garbage, sizeof(garbage)), Qi, out),
            SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegAttachGarbagePublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(blob1_, pub_blob, x), SUCCESS);

  uint8_t bad_point[33];
  bad_point[0] = 0x05;
  std::memset(bad_point + 1, 0xAB, 32);
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(pub_blob, x, mem_t(bad_point, 33), out), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegAttachEmptyPublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(blob1_, pub_blob, x), SUCCESS);

  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(pub_blob, x, mem_t(), out), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegAttachSwappedScalars) {
  buf_t pub1, x1, pub2, x2;
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(blob1_, pub1, x1), SUCCESS);
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(blob2_, pub2, x2), SUCCESS);

  buf_t Qi1;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_share_compressed(blob1_, Qi1), SUCCESS);

  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(pub1, x2, Qi1, out), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegAttachSwappedPublicShares) {
  buf_t pub1, x1;
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(blob1_, pub1, x1), SUCCESS);

  buf_t Qi2;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_share_compressed(blob2_, Qi2), SUCCESS);

  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(pub1, x1, Qi2, out), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegAttachZeroScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(blob1_, pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_share_compressed(blob1_, Qi), SUCCESS);

  uint8_t zero[32] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(pub_blob, mem_t(zero, 32), Qi, out), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegAttachSingleByteZeroScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(blob1_, pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_share_compressed(blob1_, Qi), SUCCESS);

  uint8_t zero_byte = 0x00;
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(pub_blob, mem_t(&zero_byte, 1), Qi, out), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegAttachAllZeroPublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_2p::detach_private_scalar(blob1_, pub_blob, x), SUCCESS);

  uint8_t zero_point[33] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_2p::attach_private_scalar(pub_blob, x, mem_t(zero_point, 33), out), SUCCESS);
}

// ==========================================================================
// Negative: sign (input validation, pre-protocol)
// ==========================================================================

TEST(ApiEcdsa2pc, NegSignEmptyKeyBlob) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  buf_t msg_hash(32);
  buf_t sid, sig;
  EXPECT_NE(coinbase::api::ecdsa_2p::sign(job, mem_t(), msg_hash, sid, sig), SUCCESS);
}

TEST(ApiEcdsa2pc, NegSignGarbageKeyBlob) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t msg_hash(32);
  buf_t sid, sig;
  EXPECT_NE(coinbase::api::ecdsa_2p::sign(job, mem_t(garbage, sizeof(garbage)), msg_hash, sid, sig), SUCCESS);
}

TEST(ApiEcdsa2pc, NegSignOversizedKeyBlob) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t msg_hash(32);
  buf_t sid, sig;
  EXPECT_NE(coinbase::api::ecdsa_2p::sign(job, big, msg_hash, sid, sig), SUCCESS);
}

TEST(ApiEcdsa2pc, NegSignAllZeroKeyBlob) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  uint8_t zeros[64] = {};
  buf_t msg_hash(32);
  buf_t sid, sig;
  EXPECT_NE(coinbase::api::ecdsa_2p::sign(job, mem_t(zeros, sizeof(zeros)), msg_hash, sid, sig), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegSignEmptyMsgHash) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  buf_t sid, sig;
  EXPECT_NE(coinbase::api::ecdsa_2p::sign(job, blob1_, mem_t(), sid, sig), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegSignOversizedMsgHash) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  buf_t huge_hash(65);
  std::memset(huge_hash.data(), 0x42, static_cast<size_t>(huge_hash.size()));
  buf_t sid, sig;
  EXPECT_NE(coinbase::api::ecdsa_2p::sign(job, blob1_, huge_hash, sid, sig), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegSignRoleMismatch) {
  noop_transport_t t;
  auto job = make_noop_job(t, party_t::p2);
  buf_t msg_hash(32);
  buf_t sid, sig;
  EXPECT_NE(coinbase::api::ecdsa_2p::sign(job, blob1_, msg_hash, sid, sig), SUCCESS);
}

// ==========================================================================
// Negative: refresh (input validation, pre-protocol)
// ==========================================================================

TEST(ApiEcdsa2pc, NegRefreshEmptyKeyBlob) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  buf_t new_blob;
  EXPECT_NE(coinbase::api::ecdsa_2p::refresh(job, mem_t(), new_blob), SUCCESS);
}

TEST(ApiEcdsa2pc, NegRefreshGarbageKeyBlob) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t new_blob;
  EXPECT_NE(coinbase::api::ecdsa_2p::refresh(job, mem_t(garbage, sizeof(garbage)), new_blob), SUCCESS);
}

TEST(ApiEcdsa2pc, NegRefreshOversizedKeyBlob) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t new_blob;
  EXPECT_NE(coinbase::api::ecdsa_2p::refresh(job, big, new_blob), SUCCESS);
}

TEST(ApiEcdsa2pc, NegRefreshAllZeroKeyBlob) {
  noop_transport_t t;
  auto job = make_noop_job(t);
  uint8_t zeros[64] = {};
  buf_t new_blob;
  EXPECT_NE(coinbase::api::ecdsa_2p::refresh(job, mem_t(zeros, sizeof(zeros)), new_blob), SUCCESS);
}

TEST_F(ApiEcdsa2pcNegWithBlobs, NegRefreshRoleMismatch) {
  noop_transport_t t;
  auto job = make_noop_job(t, party_t::p2);
  buf_t new_blob;
  EXPECT_NE(coinbase::api::ecdsa_2p::refresh(job, blob1_, new_blob), SUCCESS);
}
