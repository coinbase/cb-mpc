#include <gtest/gtest.h>

#include <cbmpc/protocol/ecdsa_2p.h>

#include "utils/local_network/mpc_tester.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;
using namespace coinbase::mpc::ecdsa2pc;
using namespace coinbase::testutils;

class ECDSA2PC : public Network2PC {
 protected:
  static void check_key_pair(const ecdsa2pc::key_t& k1, const ecdsa2pc::key_t& k2) {
    crypto::vartime_scope_t vartime_scope;
    EXPECT_EQ(k1.curve, k2.curve);
    const auto& G = k1.curve.generator();
    EXPECT_EQ(k1.Q, k2.Q);
    EXPECT_EQ(k1.x_share * G + k2.x_share * G, k1.Q);

    EXPECT_EQ(k1.paillier.decrypt(k1.c_key), k1.x_share);
    EXPECT_EQ(k1.paillier.decrypt(k2.c_key), k1.x_share);
  }
};

TEST_F(ECDSA2PC, Keygen) {
  ecdsa2pc::key_t p1_key, p2_key;

  mpc_runner->run_2pc([&p1_key, &p2_key](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto role = job.get_party();
    ecurve_t curve = coinbase::crypto::curve_secp256k1;

    ecdsa2pc::key_t* key;
    if (role == party_t::p1)
      key = &p1_key;
    else
      key = &p2_key;
    rv = ecdsa2pc::dkg(job, curve, *key);
    ASSERT_EQ(rv, 0);
  });

  check_key_pair(p1_key, p2_key);
}

TEST_F(ECDSA2PC, PaillierKeygenInteractive) {
  paillier_gen_interactive_t dkg(coinbase::crypto::pid_from_name("test"));
  ecurve_t curve = coinbase::crypto::curve_secp256k1;
  ecc_point_t G = curve.generator();
  bn_t x1 = bn_t::rand(curve.order());
  ecc_point_t Q1 = x1 * G;
  bn_t c_key;
  buf_t sid = coinbase::crypto::gen_random_bitlen(SEC_P_COM);
  coinbase::crypto::paillier_t p;
  crypto::mpc_pid_t prover_pid = coinbase::crypto::pid_from_name("test");

  dkg.step1_p1_to_p2(p, x1, curve.order(), c_key);
  dkg.step2_p2_to_p1();
  dkg.step3_p1_to_p2(p, x1, Q1, prover_pid, sid);
  error_t rv = dkg.step4_p2_output(p, Q1, c_key, prover_pid, sid);
  ASSERT_EQ(rv, 0);
}

TEST_F(ECDSA2PC, OptimizedKeygen) {
  std::unordered_map<party_t, ecdsa2pc::key_t> keys;
  keys[party_t::p1];
  keys[party_t::p2];

  mpc_runner->run_2pc([&keys](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto role = job.get_party();
    ecurve_t curve = coinbase::crypto::curve_secp256k1;

    ecdsa2pc::key_t& key = keys[role];
    rv = ecdsa2pc::dkg(job, curve, key);
    ASSERT_EQ(rv, 0);
  });

  check_key_pair(keys[party_t::p1], keys[party_t::p2]);
}

TEST_F(ECDSA2PC, KeygenBatchSignRefreshBatchSign) {
  const int DATA_COUNT = 3;
  std::vector<buf_t> data(DATA_COUNT);
  for (int i = 0; i < DATA_COUNT; i++) data[i] = coinbase::crypto::gen_random(32);
  std::vector<ecdsa2pc::key_t> keys(2);
  std::vector<ecdsa2pc::key_t> new_keys(2);

  mpc_runner->run_2pc([&DATA_COUNT, &data, &keys, &new_keys](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    ecurve_t curve = coinbase::crypto::curve_secp256k1;

    ecdsa2pc::key_t& key = keys[party_index];
    rv = ecdsa2pc::dkg(job, curve, key);
    ASSERT_EQ(rv, 0);

    std::vector<buf_t> sig_bufs(DATA_COUNT);
    buf_t session_id;
    rv = sign_batch(job, session_id, key, buf_t::to_mems(data), sig_bufs);
    ASSERT_EQ(rv, 0);
    EXPECT_EQ(session_id.size(), SEC_P_COM / 8);

    ecdsa2pc::key_t& new_key = new_keys[party_index];
    rv = ecdsa2pc::refresh(job, key, new_key);
    ASSERT_EQ(rv, 0);

    EXPECT_EQ(new_key.role, key.role);
    EXPECT_EQ(new_key.curve, key.curve);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    std::vector<buf_t> new_sig_bufs(DATA_COUNT);
    rv = sign_batch(job, session_id, new_key, buf_t::to_mems(data), new_sig_bufs);
    ASSERT_EQ(rv, 0);

    rv = sign_with_global_abort_batch(job, session_id, new_key, buf_t::to_mems(data), new_sig_bufs);
    ASSERT_EQ(rv, 0);
  });

  check_key_pair(keys[0], keys[1]);
  check_key_pair(new_keys[0], new_keys[1]);
}

TEST_F(ECDSA2PC, Sign) {
  buf_t data = coinbase::crypto::gen_random(32);
  std::vector<ecdsa2pc::key_t> keys(2);
  auto curve = coinbase::crypto::curve_secp256k1;
  auto q = curve.order();
  auto G = curve.generator();
  keys[0].role = party_t::p1;
  keys[1].role = party_t::p2;
  keys[0].curve = keys[1].curve = curve;
  keys[0].x_share = bn_t::rand(q);
  keys[1].x_share = bn_t::rand(q);
  ecc_point_t Q;
  Q = keys[0].x_share * G + keys[1].x_share * G;
  keys[0].Q = keys[1].Q = Q;
  keys[0].paillier.generate();
  keys[1].paillier.create_pub(keys[0].paillier.get_N());
  keys[0].c_key = keys[0].paillier.encrypt(keys[0].x_share);
  keys[1].c_key = keys[0].c_key;

  check_key_pair(keys[0], keys[1]);
  mpc_runner->run_2pc([&data, &keys, this](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    ecdsa2pc::key_t& key = keys[party_index];
    buf_t sig;
    buf_t session_id;
    rv = sign(job, session_id, key, data, sig);
    if (rv) mpc_runner->abort();
    ASSERT_EQ(rv, 0);
    rv = sign_with_global_abort(job, session_id, key, data, sig);
    if (rv) mpc_runner->abort();
    ASSERT_EQ(rv, 0);
  });
}

TEST_F(ECDSA2PC, KeygenSign) {
  buf_t data = coinbase::crypto::gen_random(32);
  std::vector<ecdsa2pc::key_t> keys(2);

  mpc_runner->run_2pc([&data, &keys](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    ecurve_t curve = coinbase::crypto::curve_secp256k1;

    ecdsa2pc::key_t& key = keys[party_index];
    rv = ecdsa2pc::dkg(job, curve, key);
    ASSERT_EQ(rv, 0);

    buf_t sig;
    buf_t session_id;
    rv = sign(job, session_id, key, data, sig);
    ASSERT_EQ(rv, 0);
    rv = sign_with_global_abort(job, session_id, key, data, sig);
    ASSERT_EQ(rv, 0);
  });

  check_key_pair(keys[0], keys[1]);
}

TEST_F(ECDSA2PC, ParallelKSRS8) {
  int parallel_count = 4;
  std::vector<std::vector<buf_t>> data(parallel_count);
  for (int i = 0; i < parallel_count; i++) {
    int len = i + 1;
    data[i].resize(len);
    for (int j = 0; j < len; j++) data[i][j] = coinbase::crypto::gen_random(32);
  }
  std::vector<std::vector<ecdsa2pc::key_t>> keys(parallel_count, std::vector<ecdsa2pc::key_t>(2));
  std::vector<std::vector<ecdsa2pc::key_t>> new_keys(parallel_count, std::vector<ecdsa2pc::key_t>(2));
  buf_t sid = coinbase::crypto::gen_random_bitlen(SEC_P_COM);

  mpc_runner->run_2pc_parallel(parallel_count, [&data, &keys, &new_keys, &sid](job_parallel_2p_t& job, int th_i) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    ecurve_t curve = coinbase::crypto::curve_secp256k1;

    ecdsa2pc::key_t& key = keys[th_i][party_index];
    rv = ecdsa2pc::dkg(job, curve, key);
    ASSERT_EQ(rv, 0);

    std::vector<buf_t> sig_bufs;
    buf_t session_id;
    rv = sign_batch(job, session_id, key, buf_t::to_mems(data[th_i]), sig_bufs);
    ASSERT_EQ(rv, 0);

    ecdsa2pc::key_t& new_key = new_keys[th_i][party_index];
    rv = ecdsa2pc::refresh(job, key, new_key);
    ASSERT_EQ(rv, 0);

    EXPECT_EQ(new_key.role, key.role);
    EXPECT_EQ(new_key.curve, key.curve);
    EXPECT_EQ(new_key.Q, key.Q);
    EXPECT_NE(new_key.x_share, key.x_share);

    std::vector<buf_t> new_sig_bufs;
    rv = sign_batch(job, session_id, key, buf_t::to_mems(data[th_i]), new_sig_bufs);
    ASSERT_EQ(rv, 0);
    rv = sign_with_global_abort_batch(job, session_id, key, buf_t::to_mems(data[th_i]), new_sig_bufs);
    ASSERT_EQ(rv, 0);
  });

  for (int i = 0; i < parallel_count; i++) {
    check_key_pair(keys[i][0], keys[i][1]);
    check_key_pair(new_keys[i][0], new_keys[i][1]);
  }
}

TEST_F(ECDSA2PC, Integer_Commit) {
  error_t rv = UNINITIALIZED_ERROR;
  ecurve_t curve = coinbase::crypto::curve_secp256k1;
  ecc_point_t G = curve.generator();
  const mod_t& q = curve.order();

  bn_t m = bn_t::rand(q);

  crypto::paillier_t paillier;
  paillier.generate();
  const mod_t& N = paillier.get_N();

  bn_t x1 = bn_t::rand(q);
  bn_t x2 = bn_t::rand(q);

  bn_t k1 = curve.get_random_value();
  bn_t k2 = curve.get_random_value();
  bn_t k2_inv = q.inv(k2, mod_t::inv_algo_e::RandomMasking);

  ecc_point_t Q1 = x1 * G;
  ecc_point_t Q2 = x2 * G;

  ecc_point_t R1 = k1 * G;
  ecc_point_t R2 = k2 * G;

  ecc_point_t R = R1 + R2;
  bn_t r = R.get_x() % q;

  bn_t r_key = bn_t::rand(N);
  bn_t c_key = paillier.encrypt(x1, r_key);

  bn_t rho = bn_t::rand((q * q) << (SEC_P_STAT * 2));

  bn_t temp;
  MODULO(q) temp = k2_inv * x2;
  temp = k2_inv * m + temp * r + rho * q;
  bn_t rc = bn_t::rand(N);
  ASSERT_EQ(mod_t::coprime(rc, N), 1);
  auto c_tag = paillier.enc(temp, rc);

  crypto::paillier_t::rerand_scope_t paillier_rerand(crypto::paillier_t::rerand_e::off);
  crypto::paillier_t::elem_t c_key_tag = paillier.elem(c_key) + (q << SEC_P_STAT);
  crypto::paillier_t::elem_t pai_c = c_key_tag * (k2_inv * r) + c_tag;

  buf_t sid = coinbase::crypto::gen_random_bitlen(SEC_P_COM);

  zk_ecdsa_sign_2pc_integer_commit_t zk;
  zk.prove(paillier, c_key_tag, pai_c, Q2, R2, m, r, k2, x2, rho, rc, sid, 0);
  rv = zk.verify(curve, paillier, c_key_tag, pai_c, Q2, R2, m, r, sid, 0);
  ASSERT_EQ(rv, 0);
}

}  // namespace