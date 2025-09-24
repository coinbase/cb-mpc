#include <gtest/gtest.h>

#include <cbmpc/protocol/ecdsa_2p.h>
#include <cbmpc/protocol/hd_keyset_ecdsa_2p.h>

#include "utils/local_network/mpc_tester.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;
using namespace coinbase::testutils;

class HDMPC_ECDSA_2P : public Network2PC {
 protected:
  static void check_hd_key_pairs_diff(const key_share_ecdsa_hdmpc_2p_t k1, const key_share_ecdsa_hdmpc_2p_t k2,
                                      const key_share_ecdsa_hdmpc_2p_t new_k1,
                                      const key_share_ecdsa_hdmpc_2p_t new_k2) {
    EXPECT_EQ(k1.root.Q, new_k1.root.Q);
    EXPECT_EQ(k2.root.Q, new_k2.root.Q);
    EXPECT_EQ(k1.root.K, new_k1.root.K);
    EXPECT_EQ(k2.root.K, new_k2.root.K);

    EXPECT_NE(k1.root.x_share, new_k1.root.x_share);
    EXPECT_NE(k2.root.k_share, new_k2.root.k_share);

    EXPECT_NE(k1.c_key, new_k1.c_key);
    EXPECT_NE(k2.c_key, new_k2.c_key);

    EXPECT_NE(k1.paillier.get_N(), new_k1.paillier.get_N());
    EXPECT_NE(k2.paillier.get_N(), new_k2.paillier.get_N());
  }

  static void check_hd_key_pairs(const key_share_ecdsa_hdmpc_2p_t k1, const key_share_ecdsa_hdmpc_2p_t k2) {
    crypto::vartime_scope_t vartime_scope;
    EXPECT_EQ(k1.curve, k2.curve);
    const auto& G = k1.curve.generator();
    EXPECT_EQ(k1.root.Q, k2.root.Q);
    EXPECT_EQ(k1.root.K, k2.root.K);
    EXPECT_EQ(k1.root.x_share * G + k2.root.x_share * G, k1.root.Q);
    EXPECT_EQ(k1.root.k_share * G + k2.root.k_share * G, k1.root.K);

    EXPECT_EQ(k1.paillier.decrypt(k1.c_key), k1.root.x_share);
    EXPECT_EQ(k1.paillier.decrypt(k2.c_key), k1.root.x_share);
  }

  static void check_key_pair(const ecdsa2pc::key_t k1, const ecdsa2pc::key_t k2) {
    EXPECT_EQ(k1.curve, k2.curve);
    const auto& G = k1.curve.generator();
    EXPECT_EQ(k1.Q, k2.Q);
    EXPECT_EQ(k1.x_share * G + k2.x_share * G, k1.Q);
  }
};

TEST_F(HDMPC_ECDSA_2P, Keygen) {
  key_share_ecdsa_hdmpc_2p_t p1_key, p2_key;

  mpc_runner->run_2pc([&p1_key, &p2_key](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto role = job.get_party();
    ecurve_t curve = coinbase::crypto::curve_secp256k1;

    key_share_ecdsa_hdmpc_2p_t* key;
    if (role == party_t::p1)
      key = &p1_key;
    else
      key = &p2_key;

    rv = key_share_ecdsa_hdmpc_2p_t::dkg(job, curve, *key);
    ASSERT_EQ(rv, 0);
  });

  check_hd_key_pairs(p1_key, p2_key);
}

TEST_F(HDMPC_ECDSA_2P, KeygenDerive) {
  int DATA_COUNT = 2;
  key_share_ecdsa_hdmpc_2p_t p1_key, p2_key;
  std::vector<coinbase::mpc::ecdsa2pc::key_t> p1_derived_keys(DATA_COUNT);
  std::vector<coinbase::mpc::ecdsa2pc::key_t> p2_derived_keys(DATA_COUNT);

  buf_t session_id = coinbase::crypto::gen_random(32);
  bip32_path_t hardened_path;
  std::vector<bip32_path_t> non_hardened_paths(DATA_COUNT);

  hardened_path.append(1);
  hardened_path.append(2);
  hardened_path.append(3);

  for (int i = 0; i < DATA_COUNT; i++) {
    non_hardened_paths[i].append((i + 1) * 4 + 0);
    non_hardened_paths[i].append((i + 1) * 4 + 1);
  }

  mpc_runner->run_2pc([&p1_key, &p2_key, &p1_derived_keys, &p2_derived_keys, hardened_path, non_hardened_paths,
                       &session_id](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto role = job.get_party();
    ecurve_t curve = coinbase::crypto::curve_secp256k1;

    key_share_ecdsa_hdmpc_2p_t* key;
    std::vector<coinbase::mpc::ecdsa2pc::key_t>* derived_keys;
    if (role == party_t::p1) {
      key = &p1_key;
      derived_keys = &p1_derived_keys;
    } else {
      key = &p2_key;
      derived_keys = &p2_derived_keys;
    }

    rv = key_share_ecdsa_hdmpc_2p_t::dkg(job, curve, *key);
    ASSERT_EQ(rv, 0);

    int n_sigs = (int)non_hardened_paths.size();

    rv = key_share_ecdsa_hdmpc_2p_t::derive_keys(job, *key, hardened_path, non_hardened_paths, session_id,
                                                 *derived_keys);
    ASSERT_EQ(rv, 0);
  });

  check_hd_key_pairs(p1_key, p2_key);
  check_key_pair(p1_derived_keys[0], p2_derived_keys[0]);
}

TEST_F(HDMPC_ECDSA_2P, KeygenRefresh) {
  key_share_ecdsa_hdmpc_2p_t p1_key, p2_key;
  key_share_ecdsa_hdmpc_2p_t new_p1_key, new_p2_key;
  buf_t sid = coinbase::crypto::gen_random_bitlen(SEC_P_COM);

  mpc_runner->run_2pc([&new_p1_key, &new_p2_key, &p1_key, &p2_key, &sid](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto role = job.get_party();
    ecurve_t curve = coinbase::crypto::curve_secp256k1;

    key_share_ecdsa_hdmpc_2p_t* key;
    key_share_ecdsa_hdmpc_2p_t* new_key;
    if (role == party_t::p1) {
      key = &p1_key;
      new_key = &new_p1_key;
    } else {
      key = &p2_key;
      new_key = &new_p2_key;
    }
    rv = key_share_ecdsa_hdmpc_2p_t::dkg(job, curve, *key);
    ASSERT_EQ(rv, 0);

    rv = key_share_ecdsa_hdmpc_2p_t::refresh(job, *key, *new_key);
    ASSERT_EQ(rv, 0);
  });

  check_hd_key_pairs(p1_key, p2_key);
  check_hd_key_pairs(new_p1_key, new_p2_key);
  check_hd_key_pairs_diff(p1_key, p2_key, new_p1_key, new_p2_key);
}

TEST_F(HDMPC_ECDSA_2P, SignSequential) {
  int DATA_COUNT = 2;
  std::vector<buf_t> data(DATA_COUNT);
  for (int i = 0; i < data.size(); i++) data[i] = coinbase::crypto::gen_random(32);
  buf_t session_id = coinbase::crypto::gen_random(32);

  mpc_runner->run_2pc([&data, &session_id, DATA_COUNT](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto role = job.get_party();
    ecurve_t curve = coinbase::crypto::curve_secp256k1;

    key_share_ecdsa_hdmpc_2p_t key;
    rv = key_share_ecdsa_hdmpc_2p_t::dkg(job, curve, key);
    ASSERT_EQ(rv, 0);

    bip32_path_t hardened_path;
    std::vector<bip32_path_t> non_hardened_paths(DATA_COUNT);

    hardened_path.append(1);
    hardened_path.append(2);
    hardened_path.append(3);

    for (int i = 0; i < DATA_COUNT; i++) {
      non_hardened_paths[i].append((i + 1) * 4 + 0);
      non_hardened_paths[i].append((i + 1) * 4 + 1);
    }

    int n_sigs = (int)non_hardened_paths.size();
    std::vector<coinbase::mpc::ecdsa2pc::key_t> derived_keys(n_sigs);

    rv = key_share_ecdsa_hdmpc_2p_t::derive_keys(job, key, hardened_path, non_hardened_paths, session_id, derived_keys);
    ASSERT_EQ(rv, 0);

    buf_t empty_sid;  // empty session id -> sign will generate sid internally
    std::vector<buf_t> sigs(n_sigs);

    for (int i = 0; i < n_sigs; i++) {
      rv = ecdsa2pc::sign(job, empty_sid, derived_keys[i], data[i], sigs[i]);
      ASSERT_EQ(rv, 0);
    }
  });
}

TEST_F(HDMPC_ECDSA_2P, SignParallel) {
  int DATA_COUNT = 3;
  std::vector<buf_t> data(DATA_COUNT);
  for (int i = 0; i < data.size(); i++) data[i] = coinbase::crypto::gen_random(32);
  buf_t session_id = coinbase::crypto::gen_random(32);

  mpc_runner->run_2pc_parallel(1, [&data, &session_id, &DATA_COUNT](job_parallel_2p_t& job, int dummy) {
    error_t rv = UNINITIALIZED_ERROR;
    auto role = job.get_party();
    ecurve_t curve = coinbase::crypto::curve_secp256k1;

    key_share_ecdsa_hdmpc_2p_t key;
    rv = key_share_ecdsa_hdmpc_2p_t::dkg(job, curve, key);
    ASSERT_EQ(rv, 0);

    bip32_path_t hardened_path;
    std::vector<bip32_path_t> non_hardened_paths(DATA_COUNT);

    hardened_path.append(1);
    hardened_path.append(2);
    hardened_path.append(3);

    for (int i = 0; i < DATA_COUNT; i++) {
      non_hardened_paths[i].append((i + 1) * 4 + 0);
      non_hardened_paths[i].append((i + 1) * 4 + 1);
    }

    int n_sigs = (int)non_hardened_paths.size();
    std::vector<buf_t> sigs(n_sigs);
    std::vector<coinbase::mpc::ecdsa2pc::key_t> derived_keys(n_sigs);

    rv = key_share_ecdsa_hdmpc_2p_t::derive_keys(job, key, hardened_path, non_hardened_paths, session_id, derived_keys);

    ASSERT_EQ(rv, 0);

    std::vector<std::thread> threads;
    job.set_parallel_count(n_sigs);
    std::mutex update_sig_mtx;

    for (int i = 0; i < n_sigs; i++) {
      threads.emplace_back([i, &derived_keys, &data, &job, &sigs, &update_sig_mtx]() {
        auto _derived_key = derived_keys[i];
        auto _data = data[i];
        int parallel_count = sigs.size();
        job_parallel_2p_t parallel_job =
            job.get_parallel_job(parallel_count, parallel_id_t(i));  // create a new job from network
        buf_t _sig;
        buf_t empty_sid;  // empty session id -> sign will generate sid internally

        error_t rv = ecdsa2pc::sign(parallel_job, empty_sid, _derived_key, _data, _sig);

        {
          std::unique_lock<std::mutex> lk(update_sig_mtx, std::defer_lock);
          sigs[i] = _sig;
        }

        ASSERT_EQ(rv, 0);
      });
    }
    for (auto& th : threads) th.join();

    job.set_parallel_count(0);
  });
}

}  // namespace