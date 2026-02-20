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
