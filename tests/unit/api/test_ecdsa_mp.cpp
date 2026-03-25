#include <atomic>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <cbmpc/api/ecdsa_mp.h>
#include <cbmpc/core/job.h>
#include <cbmpc/internal/crypto/base_ecc.h>

#include "utils/local_network/network_context.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;

using coinbase::api::curve_id;
using coinbase::api::data_transport_i;
using coinbase::api::job_mp_t;
using coinbase::api::party_idx_t;

using coinbase::testutils::mpc_net_context_t;

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

template <typename F>
static void run_mp(const std::vector<std::shared_ptr<mpc_net_context_t>>& peers, F&& f, std::vector<error_t>& out_rv) {
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

static void exercise_4p() {
  constexpr int n = 4;
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  std::vector<std::shared_ptr<local_api_transport_t>> transports;
  transports.reserve(n);
  for (const auto& p : peers) transports.push_back(std::make_shared<local_api_transport_t>(p));

  std::vector<std::string> names = {"p0", "p1", "p2", "p3"};
  std::vector<std::string_view> name_views;
  name_views.reserve(names.size());
  for (const auto& name : names) name_views.emplace_back(name);

  std::vector<buf_t> keys(n);
  std::vector<buf_t> new_keys(n);
  std::vector<buf_t> sids(n);
  std::vector<buf_t> sigs(n);
  std::vector<buf_t> new_sigs(n);
  std::vector<error_t> rvs;

  buf_t msg_hash(32);
  for (int i = 0; i < msg_hash.size(); i++) msg_hash[i] = static_cast<uint8_t>(i);

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::dkg_additive(job, curve_id::secp256k1, keys[static_cast<size_t>(i)],
                                                     sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);

  buf_t pub0;
  ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_key_compressed(keys[0], pub0), SUCCESS);
  EXPECT_EQ(pub0.size(), 33);
  for (int i = 1; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_key_compressed(keys[static_cast<size_t>(i)], pub_i), SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(coinbase::crypto::curve_secp256k1, pub0), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);

  // Change the party ordering ("role" indices) between protocols.
  // Example: a party that was at index 1 ("p1") moves to index 2.
  const std::vector<std::string_view> name_views2 = {names[0], names[2], names[1], names[3]};
  // Map new role index -> old role index (DKG) for the same party name.
  const int perm[n] = {0, 2, 1, 3};

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views2, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::sign_additive(job, keys[static_cast<size_t>(perm[i])], msg_hash,
                                                      /*sig_receiver=*/2, sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  EXPECT_GT(sigs[2].size(), 0);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    EXPECT_EQ(sigs[static_cast<size_t>(i)].size(), 0);
  }
  ASSERT_EQ(verify_key.verify(msg_hash, sigs[2]), SUCCESS);

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views2, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::refresh_additive(job, sids[static_cast<size_t>(perm[i])],
                                                         keys[static_cast<size_t>(perm[i])],
                                                         new_keys[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);

  for (int i = 0; i < n; i++) {
    buf_t pub_i;
    ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_key_compressed(new_keys[static_cast<size_t>(i)], pub_i), SUCCESS);
    EXPECT_EQ(pub_i, pub0);
  }

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views2, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::sign_additive(job, new_keys[static_cast<size_t>(i)], msg_hash,
                                                      /*sig_receiver=*/2, new_sigs[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  EXPECT_GT(new_sigs[2].size(), 0);
  for (int i = 0; i < n; i++) {
    if (i == 2) continue;
    EXPECT_EQ(new_sigs[static_cast<size_t>(i)].size(), 0);
  }
  ASSERT_EQ(verify_key.verify(msg_hash, new_sigs[2]), SUCCESS);
}

class dummy_transport_t final : public data_transport_i {
 public:
  error_t send(party_idx_t /*receiver*/, mem_t /*msg*/) override { return E_GENERAL; }
  error_t receive(party_idx_t /*sender*/, buf_t& /*msg*/) override { return E_GENERAL; }
  error_t receive_all(const std::vector<party_idx_t>& /*senders*/, std::vector<buf_t>& /*msgs*/) override {
    return E_GENERAL;
  }
};

}  // namespace

TEST(ApiEcdsaMp, DkgSignRefreshSign4p) { exercise_4p(); }

TEST(ApiEcdsaMp, RejectsInvalidJobSelf) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t bad_job{/*self=*/3, names, t};

  buf_t key;
  buf_t sid;
  EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_additive(bad_job, curve_id::secp256k1, key, sid), E_BADARG);
}

TEST(ApiEcdsaMp, RejectsDuplicatePartyNames) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"dup", "dup"};
  job_mp_t bad_job{/*self=*/0, names, t};

  buf_t key;
  buf_t sid;
  EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_additive(bad_job, curve_id::secp256k1, key, sid), E_BADARG);
}

TEST(ApiEcdsaMp, RejectsInvalidSigReceiver) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t sig;
  EXPECT_EQ(coinbase::api::ecdsa_mp::sign_additive(job, mem_t(), mem_t(), /*sig_receiver=*/5, sig), E_BADARG);
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------
//
TEST(ApiEcdsaMp, NegDkgEdwardsCurve) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_additive(job, curve_id::ed25519, key, sid), E_BADARG);
}

TEST(ApiEcdsaMp, NegDkgCurveZero) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_NE(coinbase::api::ecdsa_mp::dkg_additive(job, static_cast<curve_id>(0), key, sid), SUCCESS);
}

TEST(ApiEcdsaMp, NegDkgCurveFour) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_NE(coinbase::api::ecdsa_mp::dkg_additive(job, static_cast<curve_id>(4), key, sid), SUCCESS);
}

TEST(ApiEcdsaMp, NegDkgCurve255) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_NE(coinbase::api::ecdsa_mp::dkg_additive(job, static_cast<curve_id>(255), key, sid), SUCCESS);
}

TEST(ApiEcdsaMp, NegDkgSingleParty) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_additive(job, curve_id::secp256k1, key, sid), E_BADARG);
}

TEST(ApiEcdsaMp, NegDkgEmptyPartyNames) {
  dummy_transport_t t;
  std::vector<std::string_view> names;
  job_mp_t job{/*self=*/0, names, t};

  buf_t key, sid;
  EXPECT_EQ(coinbase::api::ecdsa_mp::dkg_additive(job, curve_id::secp256k1, key, sid), E_BADARG);
}

TEST(ApiEcdsaMp, NegGetPubKeyEmpty) {
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_mp::get_public_key_compressed(mem_t(), pub), SUCCESS);
}

TEST(ApiEcdsaMp, NegGetPubKeyGarbage) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_mp::get_public_key_compressed(mem_t(garbage, sizeof(garbage)), pub), SUCCESS);
}

TEST(ApiEcdsaMp, NegGetPubKeyAllZero) {
  uint8_t zeros[64] = {};
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_mp::get_public_key_compressed(mem_t(zeros, sizeof(zeros)), pub), SUCCESS);
}

TEST(ApiEcdsaMp, NegGetPubKeyOneByte) {
  uint8_t one = 0x00;
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_mp::get_public_key_compressed(mem_t(&one, 1), pub), SUCCESS);
}

TEST(ApiEcdsaMp, NegGetPubKeyOversized) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t pub;
  EXPECT_NE(coinbase::api::ecdsa_mp::get_public_key_compressed(big, pub), SUCCESS);
}

TEST(ApiEcdsaMp, NegGetPubShareEmpty) {
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::get_public_share_compressed(mem_t(), out), SUCCESS);
}

TEST(ApiEcdsaMp, NegGetPubShareGarbage) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::get_public_share_compressed(mem_t(garbage, sizeof(garbage)), out), SUCCESS);
}

TEST(ApiEcdsaMp, NegGetPubShareAllZero) {
  uint8_t zeros[64] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::get_public_share_compressed(mem_t(zeros, sizeof(zeros)), out), SUCCESS);
}

TEST(ApiEcdsaMp, NegGetPubShareOneByte) {
  uint8_t one = 0x00;
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::get_public_share_compressed(mem_t(&one, 1), out), SUCCESS);
}

TEST(ApiEcdsaMp, NegGetPubShareOversized) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::get_public_share_compressed(big, out), SUCCESS);
}

TEST(ApiEcdsaMp, NegDetachEmpty) {
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::ecdsa_mp::detach_private_scalar(mem_t(), pub_blob, scalar), SUCCESS);
}

TEST(ApiEcdsaMp, NegDetachGarbage) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::ecdsa_mp::detach_private_scalar(mem_t(garbage, sizeof(garbage)), pub_blob, scalar), SUCCESS);
}

TEST(ApiEcdsaMp, NegDetachAllZero) {
  uint8_t zeros[64] = {};
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::ecdsa_mp::detach_private_scalar(mem_t(zeros, sizeof(zeros)), pub_blob, scalar), SUCCESS);
}

TEST(ApiEcdsaMp, NegDetachOneByte) {
  uint8_t one = 0x00;
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::ecdsa_mp::detach_private_scalar(mem_t(&one, 1), pub_blob, scalar), SUCCESS);
}

TEST(ApiEcdsaMp, NegDetachOversized) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t pub_blob, scalar;
  EXPECT_NE(coinbase::api::ecdsa_mp::detach_private_scalar(big, pub_blob, scalar), SUCCESS);
}

TEST(ApiEcdsaMp, NegAttachEmptyPublicKeyBlob) {
  uint8_t scalar[32] = {0x01};
  uint8_t point[33] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(mem_t(), mem_t(scalar, 32), mem_t(point, 33), out), SUCCESS);
}

TEST(ApiEcdsaMp, NegAttachGarbagePublicKeyBlob) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  uint8_t scalar[32] = {0x01};
  uint8_t point[33] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(mem_t(garbage, sizeof(garbage)), mem_t(scalar, 32),
                                                           mem_t(point, 33), out),
            SUCCESS);
}

TEST(ApiEcdsaMp, NegAttachOversizedPublicKeyBlob) {
  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  uint8_t scalar[32] = {0x01};
  uint8_t point[33] = {};
  point[0] = 0x02;
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(big, mem_t(scalar, 32), mem_t(point, 33), out), SUCCESS);
}

TEST(ApiEcdsaMp, NegRefreshEmptyKeyBlob) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t sid, new_blob;
  EXPECT_NE(coinbase::api::ecdsa_mp::refresh_additive(job, sid, mem_t(), new_blob), SUCCESS);
}

TEST(ApiEcdsaMp, NegRefreshGarbageKeyBlob) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t sid, new_blob;
  EXPECT_NE(coinbase::api::ecdsa_mp::refresh_additive(job, sid, mem_t(garbage, sizeof(garbage)), new_blob), SUCCESS);
}

TEST(ApiEcdsaMp, NegRefreshAllZeroKeyBlob) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t zeros[64] = {};
  buf_t sid, new_blob;
  EXPECT_NE(coinbase::api::ecdsa_mp::refresh_additive(job, sid, mem_t(zeros, sizeof(zeros)), new_blob), SUCCESS);
}

TEST(ApiEcdsaMp, NegRefreshOneByteKeyBlob) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t one = 0x00;
  buf_t sid, new_blob;
  EXPECT_NE(coinbase::api::ecdsa_mp::refresh_additive(job, sid, mem_t(&one, 1), new_blob), SUCCESS);
}

TEST(ApiEcdsaMp, NegRefreshOversizedKeyBlob) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t sid, new_blob;
  EXPECT_NE(coinbase::api::ecdsa_mp::refresh_additive(job, sid, big, new_blob), SUCCESS);
}

TEST(ApiEcdsaMp, NegSignEmptyKeyBlob) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t msg_hash(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::ecdsa_mp::sign_additive(job, mem_t(), msg_hash, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiEcdsaMp, NegSignGarbageKeyBlob) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t msg_hash(32);
  buf_t sig;
  EXPECT_NE(
      coinbase::api::ecdsa_mp::sign_additive(job, mem_t(garbage, sizeof(garbage)), msg_hash, /*sig_receiver=*/0, sig),
      SUCCESS);
}

TEST(ApiEcdsaMp, NegSignAllZeroKeyBlob) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t zeros[64] = {};
  buf_t msg_hash(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::ecdsa_mp::sign_additive(job, mem_t(zeros, sizeof(zeros)), msg_hash, /*sig_receiver=*/0, sig),
            SUCCESS);
}

TEST(ApiEcdsaMp, NegSignOneByteKeyBlob) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  uint8_t one = 0x00;
  buf_t msg_hash(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::ecdsa_mp::sign_additive(job, mem_t(&one, 1), msg_hash, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiEcdsaMp, NegSignOversizedKeyBlob) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t big(1024 * 1024 + 1);
  std::memset(big.data(), 0xAB, static_cast<size_t>(big.size()));
  buf_t msg_hash(32);
  buf_t sig;
  EXPECT_NE(coinbase::api::ecdsa_mp::sign_additive(job, big, msg_hash, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST(ApiEcdsaMp, NegSignSigReceiverNegative) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t sig;
  EXPECT_EQ(coinbase::api::ecdsa_mp::sign_additive(job, mem_t(), mem_t(), /*sig_receiver=*/-1, sig), E_BADARG);
}

namespace {
static void generate_mp_key_blobs(curve_id curve, int n, std::vector<buf_t>& blobs) {
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  std::vector<std::shared_ptr<local_api_transport_t>> transports;
  transports.reserve(n);
  for (const auto& p : peers) transports.push_back(std::make_shared<local_api_transport_t>(p));

  std::vector<std::string> names;
  std::vector<std::string_view> name_views;
  for (int i = 0; i < n; i++) names.push_back("p" + std::to_string(i));
  for (const auto& nm : names) name_views.emplace_back(nm);

  blobs.resize(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::ecdsa_mp::dkg_additive(job, curve, blobs[static_cast<size_t>(i)],
                                                     sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
}
}  // namespace

class ApiEcdsaMpNegWithBlobs : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { generate_mp_key_blobs(curve_id::secp256k1, 3, blobs_); }
  static std::vector<buf_t> blobs_;
};
std::vector<buf_t> ApiEcdsaMpNegWithBlobs::blobs_;

TEST_F(ApiEcdsaMpNegWithBlobs, NegSignEmptyMsgHash) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t sig;
  EXPECT_NE(coinbase::api::ecdsa_mp::sign_additive(job, blobs_[0], mem_t(), /*sig_receiver=*/0, sig), SUCCESS);
}

TEST_F(ApiEcdsaMpNegWithBlobs, NegSignOversizedMsgHash) {
  dummy_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t huge_hash(65);
  std::memset(huge_hash.data(), 0x42, static_cast<size_t>(huge_hash.size()));
  buf_t sig;
  EXPECT_NE(coinbase::api::ecdsa_mp::sign_additive(job, blobs_[0], huge_hash, /*sig_receiver=*/0, sig), SUCCESS);
}

TEST_F(ApiEcdsaMpNegWithBlobs, NegAttachEmptyPrivateScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(pub_blob, mem_t(), Qi, out), SUCCESS);
}

TEST_F(ApiEcdsaMpNegWithBlobs, NegAttachGarbagePrivateScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  uint8_t garbage[512];
  std::memset(garbage, 0xFF, sizeof(garbage));
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(pub_blob, mem_t(garbage, sizeof(garbage)), Qi, out),
            SUCCESS);
}

TEST_F(ApiEcdsaMpNegWithBlobs, NegAttachWrongSizeScalar31) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  uint8_t short_scalar[31];
  std::memset(short_scalar, 0x01, sizeof(short_scalar));
  buf_t out;
  EXPECT_NE(
      coinbase::api::ecdsa_mp::attach_private_scalar(pub_blob, mem_t(short_scalar, sizeof(short_scalar)), Qi, out),
      SUCCESS);
}

TEST_F(ApiEcdsaMpNegWithBlobs, NegAttachWrongSizeScalar33) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  uint8_t long_scalar[33];
  std::memset(long_scalar, 0x01, sizeof(long_scalar));
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(pub_blob, mem_t(long_scalar, sizeof(long_scalar)), Qi, out),
            SUCCESS);
}

TEST_F(ApiEcdsaMpNegWithBlobs, NegAttachZeroScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  uint8_t zero[32] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(pub_blob, mem_t(zero, 32), Qi, out), SUCCESS);
}

TEST_F(ApiEcdsaMpNegWithBlobs, NegAttachSingleByteScalar) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t Qi;
  ASSERT_EQ(coinbase::api::ecdsa_mp::get_public_share_compressed(blobs_[0], Qi), SUCCESS);

  uint8_t one_byte = 0x01;
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(pub_blob, mem_t(&one_byte, 1), Qi, out), SUCCESS);
}

TEST_F(ApiEcdsaMpNegWithBlobs, NegAttachEmptyPublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(pub_blob, x, mem_t(), out), SUCCESS);
}

TEST_F(ApiEcdsaMpNegWithBlobs, NegAttachAllZeroPublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  uint8_t zero_point[33] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(pub_blob, x, mem_t(zero_point, 33), out), SUCCESS);
}

TEST_F(ApiEcdsaMpNegWithBlobs, NegAttachGarbagePublicShare) {
  buf_t pub_blob, x;
  ASSERT_EQ(coinbase::api::ecdsa_mp::detach_private_scalar(blobs_[0], pub_blob, x), SUCCESS);

  uint8_t bad_point[33];
  bad_point[0] = 0x05;
  std::memset(bad_point + 1, 0xAB, 32);
  buf_t out;
  EXPECT_NE(coinbase::api::ecdsa_mp::attach_private_scalar(pub_blob, x, mem_t(bad_point, 33), out), SUCCESS);
}
