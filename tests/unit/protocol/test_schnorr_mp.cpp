#include <gtest/gtest.h>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/protocol/eddsa.h>
#include <cbmpc/internal/protocol/schnorr_mp.h>

#include "utils/local_network/mpc_tester.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;
using namespace coinbase::mpc::eddsampc;
using namespace coinbase::testutils;

class mutate_schnorr_verification_key_job_t final : public job_mp_t {
 public:
  mutate_schnorr_verification_key_job_t(party_idx_t index, const std::vector<crypto::pname_t>& pnames,
                                        schnorrmp::key_t& key)
      : job_mp_t(index, pnames), key_(key) {}

  void arm(size_t expected_batch_size) {
    expected_batch_size_ = expected_batch_size;
    armed_ = true;
    mutated_ = false;
  }
  bool mutated() const { return mutated_; }

  error_t receive_many_impl(std::vector<party_idx_t> from_set, std::vector<buf_t>& outs) override {
    error_t rv = job_mp_t::receive_many_impl(from_set, outs);
    if (rv || !armed_ || mutated_ || outs.empty()) return rv;

    for (const auto& out : outs) {
      std::vector<bn_t> signature_shares;
      dylog_disable_scope_t no_log_err;
      if (coinbase::deser(out, signature_shares) || signature_shares.size() != expected_batch_size_) return SUCCESS;
    }

    // The final signature shares have been received. Changing Q now affects
    // only the final signature verification.
    key_.Q += key_.curve.generator();
    mutated_ = true;
    armed_ = false;
    return SUCCESS;
  }

 private:
  schnorrmp::key_t& key_;
  size_t expected_batch_size_ = 0;
  bool armed_ = false;
  bool mutated_ = false;
};

class MPC_EC_MP : public Network4PC {
 protected:
  static void check_keys(const std::vector<eckey::key_share_mp_t>& keys) {
    crypto::vartime_scope_t vartime_scope;
    auto Q = keys[0].Q;
    auto curve = keys[0].curve;
    for (int i = 1; i < keys.size(); i++) {
      EXPECT_EQ(Q, keys[i].Q);
      EXPECT_EQ(curve, keys[i].curve);
    }
    const auto& G = curve.generator();
    auto Q_from_x_shares = keys[0].x_share * G;
    for (int i = 1; i < keys.size(); i++) {
      Q_from_x_shares += keys[i].x_share * G;
    }
    EXPECT_EQ(Q, Q_from_x_shares);
  }

  static void check_verification_failure_clears_outputs(ecurve_t curve, schnorrmp::variant_e variant) {
    constexpr int n = 2;
    const std::vector<crypto::pname_t> pnames(mpc_runner_t::test_pnames.begin(), mpc_runner_t::test_pnames.begin() + n);
    std::vector<schnorrmp::key_t> keys(n);
    std::vector<std::shared_ptr<mutate_schnorr_verification_key_job_t>> mutating_jobs;
    std::vector<std::shared_ptr<job_mp_t>> jobs;
    mutating_jobs.reserve(n);
    jobs.reserve(n);
    for (int i = 0; i < n; i++) {
      auto job = std::make_shared<mutate_schnorr_verification_key_job_t>(party_idx_t(i), pnames, keys[i]);
      mutating_jobs.push_back(job);
      jobs.push_back(job);
    }
    mpc_runner_t runner(jobs);

    std::vector<error_t> results(n, UNINITIALIZED_ERROR);
    runner.run_mpc([&](job_mp_t& job) {
      const int party_index = job.get_party_idx();
      buf_t sid;
      results[party_index] = eckey::key_share_mp_t::dkg(job, curve, keys[party_index], sid);
    });
    ASSERT_EQ(results[0], SUCCESS);
    ASSERT_EQ(results[1], SUCCESS);

    constexpr party_idx_t sig_receiver = 0;
    const ecc_point_t original_Q = keys[sig_receiver].Q;
    const buf_t data = crypto::gen_random(32);
    const std::vector<mem_t> msgs = {data};

    mutating_jobs[sig_receiver]->arm(msgs.size());
    std::vector<std::vector<buf_t>> batch_sigs(2, std::vector<buf_t>{buf_t(1)});
    runner.run_mpc([&](job_mp_t& job) {
      const int party_index = job.get_party_idx();
      dylog_disable_scope_t no_log_err;
      results[party_index] =
          schnorrmp::sign_batch(job, keys[party_index], msgs, sig_receiver, batch_sigs[party_index], variant);
    });
    EXPECT_TRUE(mutating_jobs[sig_receiver]->mutated());
    EXPECT_NE(results[sig_receiver], SUCCESS);
    EXPECT_EQ(results[1], SUCCESS);
    EXPECT_TRUE(batch_sigs[sig_receiver].empty());

    keys[sig_receiver].Q = original_Q;
    mutating_jobs[sig_receiver]->arm(1);
    std::vector<buf_t> sigs(2, buf_t(1));
    runner.run_mpc([&](job_mp_t& job) {
      const int party_index = job.get_party_idx();
      dylog_disable_scope_t no_log_err;
      results[party_index] = schnorrmp::sign(job, keys[party_index], data, sig_receiver, sigs[party_index], variant);
    });
    EXPECT_TRUE(mutating_jobs[sig_receiver]->mutated());
    EXPECT_NE(results[sig_receiver], SUCCESS);
    EXPECT_EQ(results[1], SUCCESS);
    EXPECT_TRUE(sigs[0].empty());
    EXPECT_TRUE(sigs[1].empty());
  }
};

using EdDSA_4PC = MPC_EC_MP;
using BIP340_4PC = MPC_EC_MP;

TEST_F(EdDSA_4PC, KeygenSignRefreshSign) {
  const int DATA_COUNT = 20;

  std::vector<buf_t> data(DATA_COUNT);
  for (int i = 0; i < data.size(); i++) data[i] = crypto::gen_random(32);

  std::vector<eddsampc::key_t> keys(4);
  std::vector<eddsampc::key_t> new_keys(4);

  mpc_runner->run_mpc([&keys, &new_keys, &data](job_mp_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    eddsampc::key_t& key = keys[party_index];
    ecurve_t curve = crypto::curve_ed25519;

    buf_t sid;
    rv = eckey::key_share_mp_t::dkg(job, curve, key, sid);
    ASSERT_EQ(rv, 0);

    std::vector<buf_t> sig_buf;
    rv = eddsampc::sign_batch(job, key, buf_t::to_mems(data), party_idx_t(0), sig_buf);
    ASSERT_EQ(rv, 0);

    eddsampc::key_t& new_key = new_keys[party_index];
    rv = eckey::key_share_mp_t::refresh(job, sid, key, new_key);
    ASSERT_EQ(rv, 0);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    std::vector<buf_t> new_sig_buf;
    rv = eddsampc::sign_batch(job, new_key, buf_t::to_mems(data), party_idx_t(0), new_sig_buf);
    ASSERT_EQ(rv, 0);
  });

  check_keys(keys);
  check_keys(new_keys);
}

TEST_F(BIP340_4PC, KeygenSignRefreshSign) {
  const int DATA_COUNT = 20;

  std::vector<buf_t> data(DATA_COUNT);
  for (int i = 0; i < data.size(); i++) data[i] = crypto::gen_random(32);

  std::vector<eddsampc::key_t> keys(4);
  std::vector<eddsampc::key_t> new_keys(4);

  mpc_runner->run_mpc([&keys, &new_keys, &data](job_mp_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    eddsampc::key_t& key = keys[party_index];
    ecurve_t curve = crypto::curve_secp256k1;

    buf_t sid;
    rv = eckey::key_share_mp_t::dkg(job, curve, key, sid);
    ASSERT_EQ(rv, 0);

    std::vector<buf_t> sig_buf;
    rv = schnorrmp::sign_batch(job, key, buf_t::to_mems(data), party_idx_t(0), sig_buf, schnorrmp::variant_e::BIP340);
    ASSERT_EQ(rv, 0);

    eddsampc::key_t& new_key = new_keys[party_index];
    rv = eckey::key_share_mp_t::refresh(job, sid, key, new_key);
    ASSERT_EQ(rv, 0);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    std::vector<buf_t> new_sig_buf;
    rv = schnorrmp::sign_batch(job, new_key, buf_t::to_mems(data), party_idx_t(0), new_sig_buf,
                               schnorrmp::variant_e::BIP340);
    ASSERT_EQ(rv, 0);
  });

  check_keys(keys);
  check_keys(new_keys);
}

TEST_F(EdDSA_4PC, VerificationFailureClearsSignatureOutputs) {
  check_verification_failure_clears_outputs(crypto::curve_ed25519, schnorrmp::variant_e::EdDSA);
}

TEST_F(BIP340_4PC, VerificationFailureClearsSignatureOutputs) {
  check_verification_failure_clears_outputs(crypto::curve_secp256k1, schnorrmp::variant_e::BIP340);
}

}  // namespace
