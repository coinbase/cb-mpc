#include <gtest/gtest.h>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/protocol/eddsa.h>
#include <cbmpc/internal/protocol/schnorr_2p.h>

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

  void check_verification_failure_clears_outputs(ecurve_t curve, schnorr2p::variant_e variant) {
    std::vector<schnorr2p::key_t> keys(2);
    std::vector<error_t> results(2, UNINITIALIZED_ERROR);
    mpc_runner->run_2pc([&](job_2p_t& job) {
      const int party_index = job.get_party_idx();
      buf_t sid;
      results[party_index] = eckey::key_share_2p_t::dkg(job, curve, keys[party_index], sid);
    });
    ASSERT_EQ(results[0], SUCCESS);
    ASSERT_EQ(results[1], SUCCESS);

    // Q is only used to construct and verify the signature. Giving P1 a
    // different Q forces its final verification to fail.
    keys[0].Q += curve.generator();
    const buf_t data = crypto::gen_random(32);
    const std::vector<mem_t> msgs = {data};

    std::vector<std::vector<buf_t>> batch_sigs(2, std::vector<buf_t>{buf_t(1)});
    mpc_runner->run_2pc([&](job_2p_t& job) {
      const int party_index = job.get_party_idx();
      dylog_disable_scope_t no_log_err;
      results[party_index] = schnorr2p::sign_batch(job, keys[party_index], msgs, batch_sigs[party_index], variant);
    });
    EXPECT_NE(results[0], SUCCESS);
    EXPECT_EQ(results[1], SUCCESS);
    EXPECT_TRUE(batch_sigs[0].empty());

    std::vector<buf_t> sigs(2, buf_t(1));
    mpc_runner->run_2pc([&](job_2p_t& job) {
      const int party_index = job.get_party_idx();
      dylog_disable_scope_t no_log_err;
      results[party_index] = schnorr2p::sign(job, keys[party_index], data, sigs[party_index], variant);
    });
    EXPECT_NE(results[0], SUCCESS);
    EXPECT_EQ(results[1], SUCCESS);
    EXPECT_TRUE(sigs[0].empty());
    EXPECT_TRUE(sigs[1].empty());
  }
};

using EdDSA2PC = MPC_EC_2PC;
using BIP340_2PC = MPC_EC_2PC;

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

TEST_F(EdDSA2PC, VerificationFailureClearsSignatureOutputs) {
  check_verification_failure_clears_outputs(crypto::curve_ed25519, schnorr2p::variant_e::EdDSA);
}

TEST_F(BIP340_2PC, VerificationFailureClearsSignatureOutputs) {
  check_verification_failure_clears_outputs(crypto::curve_secp256k1, schnorr2p::variant_e::BIP340);
}

}  // namespace