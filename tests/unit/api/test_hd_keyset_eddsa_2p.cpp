#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/eddsa_2p.h>
#include <cbmpc/api/hd_keyset_eddsa_2p.h>
#include <cbmpc/internal/crypto/base_ecc.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;

using coinbase::api::curve_id;
using coinbase::api::eddsa_2p::party_t;

using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::api_harness::failing_transport_t;
using coinbase::testutils::api_harness::local_api_transport_t;
using coinbase::testutils::api_harness::run_2pc;

static void exercise_ed25519() {
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
      c1, c2, [&] { return coinbase::api::hd_keyset_eddsa_2p::dkg(job1, curve_id::ed25519, keyset1); },
      [&] { return coinbase::api::hd_keyset_eddsa_2p::dkg(job2, curve_id::ed25519, keyset2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t root_pub1;
  buf_t root_pub2;
  ASSERT_EQ(coinbase::api::hd_keyset_eddsa_2p::extract_root_public_key_compressed(keyset1, root_pub1), SUCCESS);
  ASSERT_EQ(coinbase::api::hd_keyset_eddsa_2p::extract_root_public_key_compressed(keyset2, root_pub2), SUCCESS);
  EXPECT_EQ(root_pub1.size(), 32);
  EXPECT_EQ(root_pub1, root_pub2);

  // Deterministic 32-byte message for testing.
  buf_t msg(32);
  for (int i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(0x11 + i);

  // Derive two keys.
  coinbase::api::hd_keyset_eddsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};

  std::vector<coinbase::api::hd_keyset_eddsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 0}});
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 1}});

  std::vector<buf_t> derived1;
  std::vector<buf_t> derived2;
  buf_t sid1;
  buf_t sid2;
  run_2pc(
      c1, c2,
      [&] {
        return coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job1, keyset1, hard, non_hard, sid1, derived1);
      },
      [&] {
        return coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job2, keyset2, hard, non_hard, sid2, derived2);
      },
      rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(sid1, sid2);
  ASSERT_EQ(derived1.size(), non_hard.size());
  ASSERT_EQ(derived2.size(), non_hard.size());

  for (size_t i = 0; i < derived1.size(); i++) {
    buf_t pub_a;
    buf_t pub_b;
    ASSERT_EQ(coinbase::api::eddsa_2p::get_public_key_compressed(derived1[i], pub_a), SUCCESS);
    ASSERT_EQ(coinbase::api::eddsa_2p::get_public_key_compressed(derived2[i], pub_b), SUCCESS);
    EXPECT_EQ(pub_a.size(), 32);
    EXPECT_EQ(pub_a, pub_b);
  }

  // Sign with derived key #0.
  buf_t derived_pub;
  ASSERT_EQ(coinbase::api::eddsa_2p::get_public_key_compressed(derived1[0], derived_pub), SUCCESS);
  coinbase::crypto::ecc_point_t derived_Q;
  ASSERT_EQ(derived_Q.from_bin(coinbase::crypto::curve_ed25519, derived_pub), SUCCESS);
  const coinbase::crypto::ecc_pub_key_t derived_verify_key(derived_Q);

  buf_t sig1;
  buf_t sig2;
  run_2pc(
      c1, c2, [&] { return coinbase::api::eddsa_2p::sign(job1, derived1[0], msg, sig1); },
      [&] { return coinbase::api::eddsa_2p::sign(job2, derived2[0], msg, sig2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  EXPECT_EQ(sig1.size(), 64);
  EXPECT_EQ(sig2.size(), 0);
  ASSERT_EQ(derived_verify_key.verify(msg, sig1), SUCCESS);

  // Refresh shares and ensure root pub stays same and derived pub stays same.
  buf_t keyset1_ref;
  buf_t keyset2_ref;
  run_2pc(
      c1, c2, [&] { return coinbase::api::hd_keyset_eddsa_2p::refresh(job1, keyset1, keyset1_ref); },
      [&] { return coinbase::api::hd_keyset_eddsa_2p::refresh(job2, keyset2, keyset2_ref); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);

  buf_t root_pub1_ref;
  buf_t root_pub2_ref;
  ASSERT_EQ(coinbase::api::hd_keyset_eddsa_2p::extract_root_public_key_compressed(keyset1_ref, root_pub1_ref), SUCCESS);
  ASSERT_EQ(coinbase::api::hd_keyset_eddsa_2p::extract_root_public_key_compressed(keyset2_ref, root_pub2_ref), SUCCESS);
  EXPECT_EQ(root_pub1_ref, root_pub1);
  EXPECT_EQ(root_pub2_ref, root_pub2);

  std::vector<buf_t> derived1_ref;
  std::vector<buf_t> derived2_ref;
  buf_t sid3;
  buf_t sid4;
  run_2pc(
      c1, c2,
      [&] {
        return coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job1, keyset1_ref, hard, non_hard, sid3,
                                                                       derived1_ref);
      },
      [&] {
        return coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job2, keyset2_ref, hard, non_hard, sid4,
                                                                       derived2_ref);
      },
      rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
  ASSERT_EQ(derived1_ref.size(), derived1.size());
  ASSERT_EQ(derived2_ref.size(), derived2.size());

  for (size_t i = 0; i < derived1.size(); i++) {
    buf_t pub_old;
    buf_t pub_new;
    ASSERT_EQ(coinbase::api::eddsa_2p::get_public_key_compressed(derived1[i], pub_old), SUCCESS);
    ASSERT_EQ(coinbase::api::eddsa_2p::get_public_key_compressed(derived1_ref[i], pub_new), SUCCESS);
    EXPECT_EQ(pub_old, pub_new);
  }
}

}  // namespace

TEST(ApiHdKeysetEddsa2p, DkgDeriveSignRefreshDerive) { exercise_ed25519(); }

TEST(ApiHdKeysetEddsa2p, UnsupportedCurveRejected) {
  failing_transport_t t;
  buf_t keyset;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  EXPECT_EQ(coinbase::api::hd_keyset_eddsa_2p::dkg(job, curve_id::secp256k1, keyset), E_BADARG);
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

TEST(ApiHdKeysetEddsa2p, DkgInvalidCurveP256) {
  failing_transport_t t;
  buf_t keyset;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  EXPECT_EQ(coinbase::api::hd_keyset_eddsa_2p::dkg(job, curve_id::p256, keyset), E_BADARG);
}

TEST(ApiHdKeysetEddsa2p, DkgInvalidCurveZero) {
  failing_transport_t t;
  buf_t keyset;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  EXPECT_EQ(coinbase::api::hd_keyset_eddsa_2p::dkg(job, curve_id(0), keyset), E_BADARG);
}

TEST(ApiHdKeysetEddsa2p, DkgInvalidCurveFour) {
  failing_transport_t t;
  buf_t keyset;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  EXPECT_EQ(coinbase::api::hd_keyset_eddsa_2p::dkg(job, curve_id(4), keyset), E_BADARG);
}

TEST(ApiHdKeysetEddsa2p, DkgInvalidCurve255) {
  failing_transport_t t;
  buf_t keyset;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  EXPECT_EQ(coinbase::api::hd_keyset_eddsa_2p::dkg(job, curve_id(255), keyset), E_BADARG);
}

TEST(ApiHdKeysetEddsa2p, DkgEmptyP1Name) {
  failing_transport_t t;
  buf_t keyset;
  const coinbase::api::job_2p_t job{party_t::p1, "", "p2", t};
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::dkg(job, curve_id::ed25519, keyset), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, DkgEmptyP2Name) {
  failing_transport_t t;
  buf_t keyset;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "", t};
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::dkg(job, curve_id::ed25519, keyset), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, DkgSamePartyNames) {
  failing_transport_t t;
  buf_t keyset;
  const coinbase::api::job_2p_t job{party_t::p1, "same", "same", t};
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::dkg(job, curve_id::ed25519, keyset), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, ExtractRootPubKeyEmptyBlob) {
  buf_t out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::extract_root_public_key_compressed(mem_t(), out), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, ExtractRootPubKeyGarbage) {
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::extract_root_public_key_compressed(mem_t(garbage, sizeof(garbage)), out),
            SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, ExtractRootPubKeyAllZero) {
  uint8_t zeros[64] = {};
  buf_t out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::extract_root_public_key_compressed(mem_t(zeros, sizeof(zeros)), out),
            SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, ExtractRootPubKeyOneByte) {
  uint8_t one = 0;
  buf_t out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::extract_root_public_key_compressed(mem_t(&one, 1), out), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, ExtractRootPubKeyOversized) {
  buf_t big(1024 * 1024 + 1);
  buf_t out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::extract_root_public_key_compressed(big, out), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, RefreshEmptyBlob) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  buf_t new_keyset;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::refresh(job, mem_t(), new_keyset), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, RefreshGarbageBlob) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t new_keyset;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::refresh(job, mem_t(garbage, sizeof(garbage)), new_keyset), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, RefreshAllZeroBlob) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  uint8_t zeros[64] = {};
  buf_t new_keyset;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::refresh(job, mem_t(zeros, sizeof(zeros)), new_keyset), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, RefreshOneByteBlob) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  uint8_t one = 0;
  buf_t new_keyset;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::refresh(job, mem_t(&one, 1), new_keyset), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, RefreshOversizedBlob) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  buf_t big(1024 * 1024 + 1);
  buf_t new_keyset;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::refresh(job, big, new_keyset), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, DeriveEmptyBlob) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  coinbase::api::hd_keyset_eddsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};
  std::vector<coinbase::api::hd_keyset_eddsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 0}});
  buf_t sid;
  std::vector<buf_t> out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job, mem_t(), hard, non_hard, sid, out), SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, DeriveGarbageBlob) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  coinbase::api::hd_keyset_eddsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};
  std::vector<coinbase::api::hd_keyset_eddsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 0}});
  buf_t sid;
  std::vector<buf_t> out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job, mem_t(garbage, sizeof(garbage)), hard,
                                                                    non_hard, sid, out),
            SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, DeriveAllZeroBlob) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  uint8_t zeros[64] = {};
  coinbase::api::hd_keyset_eddsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};
  std::vector<coinbase::api::hd_keyset_eddsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 0}});
  buf_t sid;
  std::vector<buf_t> out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job, mem_t(zeros, sizeof(zeros)), hard, non_hard,
                                                                    sid, out),
            SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, DeriveOneByteBlob) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  uint8_t one = 0;
  coinbase::api::hd_keyset_eddsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};
  std::vector<coinbase::api::hd_keyset_eddsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 0}});
  buf_t sid;
  std::vector<buf_t> out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job, mem_t(&one, 1), hard, non_hard, sid, out),
            SUCCESS);
}

TEST(ApiHdKeysetEddsa2p, DeriveOversizedBlob) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  buf_t big(1024 * 1024 + 1);
  coinbase::api::hd_keyset_eddsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};
  std::vector<coinbase::api::hd_keyset_eddsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 0}});
  buf_t sid;
  std::vector<buf_t> out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job, big, hard, non_hard, sid, out), SUCCESS);
}

namespace {

static void generate_eddsa_hd_keyset_blobs(buf_t& blob1, buf_t& blob2) {
  auto c1 = std::make_shared<mpc_net_context_t>(0);
  auto c2 = std::make_shared<mpc_net_context_t>(1);
  std::vector<std::shared_ptr<mpc_net_context_t>> peers = {c1, c2};
  c1->init_with_peers(peers);
  c2->init_with_peers(peers);

  local_api_transport_t t1(c1);
  local_api_transport_t t2(c2);

  const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};

  error_t rv1 = UNINITIALIZED_ERROR, rv2 = UNINITIALIZED_ERROR;
  run_2pc(
      c1, c2, [&] { return coinbase::api::hd_keyset_eddsa_2p::dkg(job1, curve_id::ed25519, blob1); },
      [&] { return coinbase::api::hd_keyset_eddsa_2p::dkg(job2, curve_id::ed25519, blob2); }, rv1, rv2);
  ASSERT_EQ(rv1, SUCCESS);
  ASSERT_EQ(rv2, SUCCESS);
}

}  // namespace

class ApiHdKeysetEddsa2pNegWithBlobs : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { generate_eddsa_hd_keyset_blobs(blob1_, blob2_); }
  static buf_t blob1_;
  static buf_t blob2_;
};
buf_t ApiHdKeysetEddsa2pNegWithBlobs::blob1_;
buf_t ApiHdKeysetEddsa2pNegWithBlobs::blob2_;

TEST_F(ApiHdKeysetEddsa2pNegWithBlobs, RefreshRoleMismatchP1BlobP2Job) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p2, "p1", "p2", t};
  buf_t new_keyset;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::refresh(job, blob1_, new_keyset), SUCCESS);
}

TEST_F(ApiHdKeysetEddsa2pNegWithBlobs, RefreshRoleMismatchP2BlobP1Job) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  buf_t new_keyset;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::refresh(job, blob2_, new_keyset), SUCCESS);
}

TEST_F(ApiHdKeysetEddsa2pNegWithBlobs, DeriveRoleMismatchP1BlobP2Job) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p2, "p1", "p2", t};
  coinbase::api::hd_keyset_eddsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};
  std::vector<coinbase::api::hd_keyset_eddsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 0}});
  buf_t sid;
  std::vector<buf_t> out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job, blob1_, hard, non_hard, sid, out), SUCCESS);
}

TEST_F(ApiHdKeysetEddsa2pNegWithBlobs, DeriveDuplicateNonHardenedPaths) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  coinbase::api::hd_keyset_eddsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};
  std::vector<coinbase::api::hd_keyset_eddsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 0}});
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 0}});
  buf_t sid;
  std::vector<buf_t> out;
  EXPECT_EQ(coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job, blob1_, hard, non_hard, sid, out), E_BADARG);
}

TEST_F(ApiHdKeysetEddsa2pNegWithBlobs, DeriveEmptyNonHardenedPaths) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  coinbase::api::hd_keyset_eddsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};
  std::vector<coinbase::api::hd_keyset_eddsa_2p::bip32_path_t> empty_paths;
  buf_t sid;
  std::vector<buf_t> out;
  error_t rv = coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job, blob1_, hard, empty_paths, sid, out);
  if (rv == SUCCESS) {
    EXPECT_TRUE(out.empty());
  }
}

TEST_F(ApiHdKeysetEddsa2pNegWithBlobs, DeriveEmptyHardenedPath) {
  failing_transport_t t;
  const coinbase::api::job_2p_t job{party_t::p1, "p1", "p2", t};
  coinbase::api::hd_keyset_eddsa_2p::bip32_path_t empty_hard;
  std::vector<coinbase::api::hd_keyset_eddsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_eddsa_2p::bip32_path_t{{0, 0}});
  buf_t sid;
  std::vector<buf_t> out;
  EXPECT_NE(coinbase::api::hd_keyset_eddsa_2p::derive_eddsa_2p_keys(job, blob1_, empty_hard, non_hard, sid, out),
            SUCCESS);
}
