#include <gtest/gtest.h>

#include <cbmpc/protocol/eddsa.h>

#include "utils/local_network/mpc_tester.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;
using namespace coinbase::mpc::eddsampc;
using namespace coinbase::testutils;

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

TEST_F(EdDSA_4PC, ParallelKSRS8) {
  int parallel_count = 8;
  std::vector<std::vector<buf_t>> data(parallel_count);
  for (int i = 0; i < parallel_count; i++) {
    int len = i + 1;
    data[i].resize(len);
    for (int j = 0; j < len; j++) data[i][j] = crypto::gen_random(32);
  }
  std::vector<std::vector<eddsampc::key_t>> keys(parallel_count, std::vector<eddsampc::key_t>(4));
  std::vector<std::vector<eddsampc::key_t>> new_keys(parallel_count, std::vector<eddsampc::key_t>(4));

  mpc_runner->run_mpc_parallel(parallel_count, [&keys, &new_keys, &data](job_parallel_mp_t& job, int th_i) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    eddsampc::key_t& key = keys[th_i][party_index];
    ecurve_t curve = crypto::curve_ed25519;

    buf_t sid;
    rv = eckey::key_share_mp_t::dkg(job, curve, key, sid);
    ASSERT_EQ(rv, 0);

    std::vector<buf_t> sig_buf;
    rv = eddsampc::sign_batch(job, key, buf_t::to_mems(data[th_i]), party_idx_t(0), sig_buf);
    ASSERT_EQ(rv, 0);

    eddsampc::key_t& new_key = new_keys[th_i][party_index];
    rv = eckey::key_share_mp_t::refresh(job, sid, key, new_key);
    ASSERT_EQ(rv, 0);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    std::vector<buf_t> new_sig_buf;
    rv = eddsampc::sign_batch(job, new_key, buf_t::to_mems(data[th_i]), party_idx_t(0), new_sig_buf);
    ASSERT_EQ(rv, 0);
  });

  for (int i = 0; i < parallel_count; i++) {
    check_keys(keys[i]);
    check_keys(new_keys[i]);
  }
}

}  // namespace
