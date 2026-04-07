#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base_bn.h>
#include <cbmpc/internal/protocol/eddsa.h>
#include <cbmpc/internal/protocol/schnorr_2p.h>

#include "unit/api/test_transport_harness.h"
#include "utils/local_network/mpc_tester.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;
using namespace coinbase::testutils;

class MPC_EC_2PC : public Network2PC {
 protected:
  static void check_key_pair(const eckey::key_share_2p_t& k1, const eckey::key_share_2p_t& k2) {
    crypto::vartime_scope_t vartime_scope;
    EXPECT_EQ(k1.curve, k2.curve);
    const auto& G = k1.curve.generator();
    EXPECT_EQ(k1.Q, k2.Q);
    EXPECT_EQ(k1.x_share * G + k2.x_share * G, k1.Q);
  }
};

using EdDSA2PC = MPC_EC_2PC;
using BIP340_2PC = MPC_EC_2PC;

namespace {

using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::api_harness::local_api_transport_t;
using coinbase::testutils::api_harness::run_2pc;

class tampering_transport_t final : public coinbase::api::data_transport_i {
 public:
  tampering_transport_t(std::shared_ptr<mpc_net_context_t> ctx, bool tamper_second_send, buf_t replacement)
      : ctx_(std::move(ctx)), tamper_second_send_(tamper_second_send), replacement_(std::move(replacement)) {}

  error_t send(api::party_idx_t receiver, mem_t msg) override {
    ++send_count_;
    const mem_t to_send =
        (tamper_second_send_ && send_count_ == 2) ? mem_t(replacement_.data(), replacement_.size()) : msg;
    ctx_->send(static_cast<coinbase::mpc::party_idx_t>(receiver), to_send);
    return SUCCESS;
  }

  error_t receive(api::party_idx_t sender, buf_t& msg) override {
    return ctx_->receive(static_cast<coinbase::mpc::party_idx_t>(sender), msg);
  }

  error_t receive_all(const std::vector<api::party_idx_t>& senders, std::vector<buf_t>& msgs) override {
    std::vector<coinbase::mpc::party_idx_t> sender_roles;
    sender_roles.reserve(senders.size());
    for (const auto sender : senders) sender_roles.push_back(static_cast<coinbase::mpc::party_idx_t>(sender));
    return ctx_->receive_all(sender_roles, msgs);
  }

 private:
  std::shared_ptr<mpc_net_context_t> ctx_;
  bool tamper_second_send_ = false;
  int send_count_ = 0;
  buf_t replacement_;
};

std::vector<std::shared_ptr<mpc_net_context_t>> make_peers() {
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(2);
  for (int i = 0; i < 2; ++i) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& peer : peers) peer->init_with_peers(peers);
  return peers;
}

std::array<schnorr2p::key_t, 2> make_keys() {
  std::array<schnorr2p::key_t, 2> keys;
  auto peers = make_peers();
  auto t1 = std::make_shared<local_api_transport_t>(peers[0]);
  auto t2 = std::make_shared<local_api_transport_t>(peers[1]);

  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;
  run_2pc(
      peers[0], peers[1],
      [&] {
        job_2p_t job(party_t::p1, "p0", "p1", *t1);
        buf_t sid;
        return eckey::key_share_2p_t::dkg(job, crypto::curve_secp256k1, keys[0], sid);
      },
      [&] {
        job_2p_t job(party_t::p2, "p0", "p1", *t2);
        buf_t sid;
        return eckey::key_share_2p_t::dkg(job, crypto::curve_secp256k1, keys[1], sid);
      },
      rv1, rv2);

  EXPECT_EQ(rv1, SUCCESS);
  EXPECT_EQ(rv2, SUCCESS);
  return keys;
}

}  // namespace

TEST_F(EdDSA2PC, KeygenSignRefreshSign) {
  const int DATA_COUNT = 7;
  std::vector<buf_t> data_bufs(DATA_COUNT);
  std::vector<mem_t> data(DATA_COUNT);
  for (int i = 0; i < DATA_COUNT; i++) data[i] = data_bufs[i] = crypto::gen_random(32);
  std::vector<eddsa2pc::key_t> keys(2);
  std::vector<eddsa2pc::key_t> new_keys(2);

  mpc_runner->run_2pc([&data, &keys, &new_keys](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    ecurve_t curve = crypto::curve_ed25519;

    eddsa2pc::key_t& key = keys[party_index];
    buf_t sid;
    rv = eckey::key_share_2p_t::dkg(job, curve, key, sid);
    ASSERT_EQ(rv, 0);

    std::vector<buf_t> sig_bufs;
    rv = eddsa2pc::sign_batch(job, key, data, sig_bufs);
    ASSERT_EQ(rv, 0);

    eddsa2pc::key_t& new_key = new_keys[party_index];
    rv = eckey::key_share_2p_t::refresh(job, key, new_key);
    ASSERT_EQ(rv, 0);

    EXPECT_EQ(new_key.role, key.role);
    EXPECT_EQ(new_key.curve, key.curve);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    std::vector<buf_t> new_sig_bufs;
    rv = eddsa2pc::sign_batch(job, new_key, data, new_sig_bufs);
    ASSERT_EQ(rv, 0);
  });

  check_key_pair(keys[0], keys[1]);
  check_key_pair(new_keys[0], new_keys[1]);
}

TEST_F(BIP340_2PC, KeygenSignRefreshSign) {
  const int DATA_COUNT = 7;
  std::vector<buf_t> data_bufs(DATA_COUNT);
  std::vector<mem_t> data(DATA_COUNT);
  for (int i = 0; i < DATA_COUNT; i++) data[i] = data_bufs[i] = crypto::gen_random(32);
  std::vector<eddsa2pc::key_t> keys(2);
  std::vector<eddsa2pc::key_t> new_keys(2);

  mpc_runner->run_2pc([&data, &keys, &new_keys](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    ecurve_t curve = crypto::curve_secp256k1;

    eddsa2pc::key_t& key = keys[party_index];
    buf_t sid;
    rv = eckey::key_share_2p_t::dkg(job, curve, key, sid);
    ASSERT_EQ(rv, 0);

    std::vector<buf_t> sig_bufs;
    rv = schnorr2p::sign_batch(job, key, data, sig_bufs, schnorr2p::variant_e::BIP340);
    ASSERT_EQ(rv, 0);

    eddsa2pc::key_t& new_key = new_keys[party_index];
    rv = eckey::key_share_2p_t::refresh(job, key, new_key);
    ASSERT_EQ(rv, 0);

    EXPECT_EQ(new_key.role, key.role);
    EXPECT_EQ(new_key.curve, key.curve);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    std::vector<buf_t> new_sig_bufs;
    rv = schnorr2p::sign_batch(job, new_key, data, new_sig_bufs, schnorr2p::variant_e::BIP340);
    ASSERT_EQ(rv, 0);
  });

  check_key_pair(keys[0], keys[1]);
  check_key_pair(new_keys[0], new_keys[1]);
}

TEST_F(BIP340_2PC, RejectsOutOfRangeS2WithoutLeakingModuloState) {
  auto keys = make_keys();

  std::vector<bn_t> malicious_s2(1);
  malicious_s2[0] = -bn_t(1);
  const buf_t malicious_payload = coinbase::ser(malicious_s2);

  auto peers = make_peers();
  tampering_transport_t p1_transport(peers[0], /*tamper_second_send=*/false, buf_t());
  tampering_transport_t p2_transport(peers[1], /*tamper_second_send=*/true, malicious_payload);

  buf_t msg(32);
  for (int i = 0; i < msg.size(); ++i) msg[i] = 0x42;
  std::vector<mem_t> msgs = {msg};

  error_t p1_rv = UNINITIALIZED_ERROR;
  error_t p2_rv = UNINITIALIZED_ERROR;
  bool modulo_leaked = true;
  bool math_poisoned = true;

  std::thread p1([&] {
    job_2p_t job(party_t::p1, "p0", "p1", p1_transport);
    std::vector<buf_t> sigs;
    p1_rv = schnorr2p::sign_batch(job, keys[0], msgs, sigs, schnorr2p::variant_e::BIP340);
    modulo_leaked = (crypto::thread_local_storage_mod() != nullptr);

    try {
      bn_t q_as_bn = keys[0].curve.order();
      (void)(q_as_bn + bn_t(1));
      math_poisoned = false;
    } catch (...) {
      math_poisoned = true;
    }
  });

  std::thread p2([&] {
    job_2p_t job(party_t::p2, "p0", "p1", p2_transport);
    std::vector<buf_t> sigs;
    p2_rv = schnorr2p::sign_batch(job, keys[1], msgs, sigs, schnorr2p::variant_e::BIP340);
  });

  p1.join();
  p2.join();

  EXPECT_NE(p1_rv, SUCCESS);
  EXPECT_FALSE(modulo_leaked);
  EXPECT_FALSE(math_poisoned);
  EXPECT_EQ(crypto::thread_local_storage_mod(), nullptr);
  EXPECT_EQ(p2_rv, SUCCESS);
}

}  // namespace
