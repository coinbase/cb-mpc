#include <gtest/gtest.h>

#include <cbmpc/protocol/eddsa.h>
#include <cbmpc/protocol/schnorr_2p.h>

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

TEST_F(EdDSA2PC, ParallelKSRS8) {
  int parallel_count = 8;
  std::vector<std::vector<mem_t>> data(30);
  std::vector<std::vector<buf_t>> data_buf(30);
  for (int i = 0; i < parallel_count; i++) {
    int len = i + 1;
    data[i].resize(len);
    data_buf[i].resize(len);
    for (int j = 0; j < len; j++) data[i][j] = data_buf[i][j] = crypto::gen_random(32);
  }
  std::vector<std::vector<eddsa2pc::key_t>> keys(parallel_count, std::vector<eddsa2pc::key_t>(2));
  std::vector<std::vector<eddsa2pc::key_t>> new_keys(parallel_count, std::vector<eddsa2pc::key_t>(2));

  mpc_runner->run_2pc_parallel(parallel_count, [&data, &keys, &new_keys](job_parallel_2p_t& job, int th_i) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    ecurve_t curve = crypto::curve_ed25519;

    eddsa2pc::key_t& key = keys[th_i][party_index];
    buf_t sid;
    rv = eckey::key_share_2p_t::dkg(job, curve, key, sid);
    ASSERT_EQ(rv, 0);

    std::vector<buf_t> sig_bufs;
    rv = eddsa2pc::sign_batch(job, key, data[th_i], sig_bufs);
    ASSERT_EQ(rv, 0);

    eddsa2pc::key_t& new_key = new_keys[th_i][party_index];
    rv = eckey::key_share_2p_t::refresh(job, key, new_key);
    ASSERT_EQ(rv, 0);

    EXPECT_EQ(new_key.role, key.role);
    EXPECT_EQ(new_key.curve, key.curve);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    std::vector<buf_t> new_sig_bufs;
    rv = eddsa2pc::sign_batch(job, new_key, data[th_i], new_sig_bufs);
    ASSERT_EQ(rv, 0);
  });

  for (int i = 0; i < parallel_count; i++) {
    check_key_pair(keys[i][0], keys[i][1]);
    check_key_pair(new_keys[i][0], new_keys[i][1]);
  }
}

}  // namespace