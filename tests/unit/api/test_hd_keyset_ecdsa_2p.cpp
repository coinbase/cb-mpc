#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/ecdsa_2p.h>
#include <cbmpc/api/hd_keyset_ecdsa_2p.h>
#include <cbmpc/internal/crypto/base_ecc.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;

using coinbase::api::curve_id;

using coinbase::api::ecdsa_2p::party_t;

using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::api_harness::failing_transport_t;
using coinbase::testutils::api_harness::local_api_transport_t;
using coinbase::testutils::api_harness::run_2pc;

static void exercise_curve(curve_id curve, const coinbase::crypto::ecurve_t& verify_curve) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  local_api_transport_t t1(c1);
  local_api_transport_t t2(c2);

  const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};

  buf_t keyset1;
  buf_t keyset2;
  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;

  run_2pc(
      c1, c2, [&] { return coinbase::api::hd_keyset_ecdsa_2p::dkg(job1, curve, keyset1); },
      [&] { return coinbase::api::hd_keyset_ecdsa_2p::dkg(job2, curve, keyset2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t root_pub1;
  buf_t root_pub2;
  ASSERT_EQ(coinbase::api::hd_keyset_ecdsa_2p::extract_root_public_key_compressed(keyset1, root_pub1), SUCCESS);
  ASSERT_EQ(coinbase::api::hd_keyset_ecdsa_2p::extract_root_public_key_compressed(keyset2, root_pub2), SUCCESS);
  ASSERT_EQ(root_pub1, root_pub2);

  // Deterministic 32-byte "hash" for testing.
  buf_t msg_hash(32);
  for (int i = 0; i < msg_hash.size(); i++) msg_hash[i] = static_cast<uint8_t>(0xA0 + i);

  // Derive 2 keys.
  coinbase::api::hd_keyset_ecdsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};  // demo-only

  std::vector<coinbase::api::hd_keyset_ecdsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_ecdsa_2p::bip32_path_t{{0, 0}});
  non_hard.push_back(coinbase::api::hd_keyset_ecdsa_2p::bip32_path_t{{0, 1}});

  std::vector<buf_t> derived1;
  std::vector<buf_t> derived2;
  buf_t sid1;
  buf_t sid2;

  run_2pc(
      c1, c2,
      [&] {
        return coinbase::api::hd_keyset_ecdsa_2p::derive_ecdsa_2p_keys(job1, keyset1, hard, non_hard, sid1, derived1);
      },
      [&] {
        return coinbase::api::hd_keyset_ecdsa_2p::derive_ecdsa_2p_keys(job2, keyset2, hard, non_hard, sid2, derived2);
      },
      rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  ASSERT_EQ(sid1, sid2);
  ASSERT_EQ(derived1.size(), non_hard.size());
  ASSERT_EQ(derived2.size(), non_hard.size());

  // Derived pubkeys must match across parties.
  for (size_t i = 0; i < non_hard.size(); i++) {
    buf_t pub_a;
    buf_t pub_b;
    ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(derived1[i], pub_a), SUCCESS);
    ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(derived2[i], pub_b), SUCCESS);
    EXPECT_EQ(pub_a, pub_b);
  }

  // Sign using the first derived key.
  buf_t sig1;
  buf_t sig2;
  buf_t sid3;
  buf_t sid4;
  run_2pc(
      c1, c2, [&] { return coinbase::api::ecdsa_2p::sign(job1, derived1[0], msg_hash, sid3, sig1); },
      [&] { return coinbase::api::ecdsa_2p::sign(job2, derived2[0], msg_hash, sid4, sig2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(sid3, sid4);
  EXPECT_GT(sig1.size(), 0);
  EXPECT_EQ(sig2.size(), 0);

  buf_t derived_pub;
  ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(derived1[0], derived_pub), SUCCESS);
  coinbase::crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(verify_curve, derived_pub), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t verify_key(Q);
  ASSERT_EQ(verify_key.verify(msg_hash, sig1), SUCCESS);

  // Refresh keyset shares.
  buf_t keyset1_ref;
  buf_t keyset2_ref;
  run_2pc(
      c1, c2, [&] { return coinbase::api::hd_keyset_ecdsa_2p::refresh(job1, keyset1, keyset1_ref); },
      [&] { return coinbase::api::hd_keyset_ecdsa_2p::refresh(job2, keyset2, keyset2_ref); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t root_pub1_ref;
  buf_t root_pub2_ref;
  ASSERT_EQ(coinbase::api::hd_keyset_ecdsa_2p::extract_root_public_key_compressed(keyset1_ref, root_pub1_ref), SUCCESS);
  ASSERT_EQ(coinbase::api::hd_keyset_ecdsa_2p::extract_root_public_key_compressed(keyset2_ref, root_pub2_ref), SUCCESS);
  EXPECT_EQ(root_pub1_ref, root_pub1);
  EXPECT_EQ(root_pub2_ref, root_pub2);

  // Derive again; derived pubkeys should remain stable.
  std::vector<buf_t> derived1_ref;
  std::vector<buf_t> derived2_ref;
  buf_t sid5;
  buf_t sid6;
  run_2pc(
      c1, c2,
      [&] {
        return coinbase::api::hd_keyset_ecdsa_2p::derive_ecdsa_2p_keys(job1, keyset1_ref, hard, non_hard, sid5,
                                                                       derived1_ref);
      },
      [&] {
        return coinbase::api::hd_keyset_ecdsa_2p::derive_ecdsa_2p_keys(job2, keyset2_ref, hard, non_hard, sid6,
                                                                       derived2_ref);
      },
      rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(derived1_ref.size(), derived1.size());
  EXPECT_EQ(derived2_ref.size(), derived2.size());

  for (size_t i = 0; i < derived1.size(); i++) {
    buf_t pub_old;
    buf_t pub_new;
    ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(derived1[i], pub_old), SUCCESS);
    ASSERT_EQ(coinbase::api::ecdsa_2p::get_public_key_compressed(derived1_ref[i], pub_new), SUCCESS);
    EXPECT_EQ(pub_old, pub_new);
  }
}

}  // namespace

TEST(ApiHdKeysetEcdsa2p, DkgDeriveSignRefreshDerive) {
  exercise_curve(curve_id::secp256k1, coinbase::crypto::curve_secp256k1);
  exercise_curve(curve_id::p256, coinbase::crypto::curve_p256);
}

TEST(ApiHdKeysetEcdsa2p, UnsupportedCurveRejected) {
  failing_transport_t t;
  buf_t keyset;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  EXPECT_EQ(coinbase::api::hd_keyset_ecdsa_2p::dkg(job, curve_id::ed25519, keyset), E_BADARG);
}
