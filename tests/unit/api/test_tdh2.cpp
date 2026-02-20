#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/tdh2.h>
#include <cbmpc/core/access_structure.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/job.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;

using coinbase::api::access_structure_t;
using coinbase::api::curve_id;
using coinbase::api::job_mp_t;
using coinbase::api::party_idx_t;

using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::api_harness::failing_transport_t;
using coinbase::testutils::api_harness::local_api_transport_t;
using coinbase::testutils::api_harness::run_mp;

static void exercise_dkg_round_trip(curve_id curve) {
  constexpr int n = 3;

  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  std::vector<std::shared_ptr<local_api_transport_t>> transports;
  transports.reserve(n);
  for (const auto& p : peers) transports.push_back(std::make_shared<local_api_transport_t>(p));

  std::vector<std::string> names = {"p0", "p1", "p2"};
  std::vector<std::string_view> name_views;
  name_views.reserve(names.size());
  for (const auto& name : names) name_views.emplace_back(name);

  std::vector<buf_t> public_keys(n);
  std::vector<std::vector<buf_t>> public_shares(n);
  std::vector<buf_t> private_shares(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::tdh2::dkg_additive(job, curve, public_keys[static_cast<size_t>(i)],
                                                 public_shares[static_cast<size_t>(i)],
                                                 private_shares[static_cast<size_t>(i)], sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) {
    EXPECT_EQ(public_keys[0], public_keys[static_cast<size_t>(i)]);
    EXPECT_EQ(public_shares[0], public_shares[static_cast<size_t>(i)]);
    EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);
  }
  ASSERT_EQ(public_shares[0].size(), static_cast<size_t>(n));

  buf_t plaintext(32);
  for (int i = 0; i < plaintext.size(); i++) plaintext[i] = static_cast<uint8_t>(0xA5 ^ i);
  const mem_t label("tdh2-label");

  buf_t ciphertext;
  ASSERT_EQ(coinbase::api::tdh2::encrypt(public_keys[0], plaintext, label, ciphertext), SUCCESS);
  ASSERT_GT(ciphertext.size(), 0);
  ASSERT_EQ(coinbase::api::tdh2::verify(public_keys[0], ciphertext, label), SUCCESS);

  std::vector<buf_t> partials(n);
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(coinbase::api::tdh2::partial_decrypt(private_shares[static_cast<size_t>(i)], ciphertext, label,
                                                   partials[static_cast<size_t>(i)]),
              SUCCESS);
  }

  const auto public_shares_mems = buf_t::to_mems(public_shares[0]);
  const auto partials_mems = buf_t::to_mems(partials);

  buf_t decrypted;
  ASSERT_EQ(coinbase::api::tdh2::combine_additive(public_keys[0], public_shares_mems, label, partials_mems, ciphertext,
                                                  decrypted),
            SUCCESS);
  EXPECT_EQ(mem_t(decrypted), mem_t(plaintext));

  const mem_t wrong_label("wrong-label");
  EXPECT_NE(coinbase::api::tdh2::verify(public_keys[0], ciphertext, wrong_label), SUCCESS);
}

static void exercise_dkg_ac_round_trip(curve_id curve) {
  constexpr int n = 3;

  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  std::vector<std::shared_ptr<local_api_transport_t>> transports;
  transports.reserve(n);
  for (const auto& p : peers) transports.push_back(std::make_shared<local_api_transport_t>(p));

  std::vector<std::string> names = {"p0", "p1", "p2"};
  std::vector<std::string_view> name_views;
  name_views.reserve(names.size());
  for (const auto& name : names) name_views.emplace_back(name);

  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  const std::vector<std::string_view> quorum = {"p0", "p1"};

  std::vector<buf_t> public_keys(n);
  std::vector<std::vector<buf_t>> public_shares(n);
  std::vector<buf_t> private_shares(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{static_cast<party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
        return coinbase::api::tdh2::dkg_ac(job, curve, sids[static_cast<size_t>(i)], ac, quorum,
                                           public_keys[static_cast<size_t>(i)], public_shares[static_cast<size_t>(i)],
                                           private_shares[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, SUCCESS);
  for (int i = 1; i < n; i++) {
    EXPECT_EQ(public_keys[0], public_keys[static_cast<size_t>(i)]);
    EXPECT_EQ(public_shares[0], public_shares[static_cast<size_t>(i)]);
    EXPECT_EQ(sids[0], sids[static_cast<size_t>(i)]);
  }

  buf_t plaintext(32);
  for (int i = 0; i < plaintext.size(); i++) plaintext[i] = static_cast<uint8_t>(0x3C ^ i);
  const mem_t label("tdh2-label");

  buf_t ciphertext;
  ASSERT_EQ(coinbase::api::tdh2::encrypt(public_keys[0], plaintext, label, ciphertext), SUCCESS);
  ASSERT_EQ(coinbase::api::tdh2::verify(public_keys[0], ciphertext, label), SUCCESS);

  // Collect partial decryptions from a 2-party quorum.
  buf_t partial0;
  buf_t partial1;
  ASSERT_EQ(coinbase::api::tdh2::partial_decrypt(private_shares[0], ciphertext, label, partial0), SUCCESS);
  ASSERT_EQ(coinbase::api::tdh2::partial_decrypt(private_shares[1], ciphertext, label, partial1), SUCCESS);

  const std::vector<std::string_view> partial_names = {"p0", "p1"};
  const std::vector<buf_t> partial_bufs = {partial0, partial1};

  const auto public_shares_mems = buf_t::to_mems(public_shares[0]);
  const auto partials_mems = buf_t::to_mems(partial_bufs);

  buf_t decrypted;
  ASSERT_EQ(coinbase::api::tdh2::combine_ac(ac, public_keys[0], name_views, public_shares_mems, label, partial_names,
                                            partials_mems, ciphertext, decrypted),
            SUCCESS);
  EXPECT_EQ(mem_t(decrypted), mem_t(plaintext));

  // Not enough shares should fail.
  buf_t decrypted2;
  const std::vector<std::string_view> one_name = {"p0"};
  const std::vector<buf_t> one_partial = {partial0};
  const auto one_partial_mems = buf_t::to_mems(one_partial);
  EXPECT_NE(coinbase::api::tdh2::combine_ac(ac, public_keys[0], name_views, public_shares_mems, label, one_name,
                                            one_partial_mems, ciphertext, decrypted2),
            SUCCESS);
}

}  // namespace

TEST(ApiTdh2, RoundTripEncryptDecrypt) {
  exercise_dkg_round_trip(coinbase::api::curve_id::secp256k1);
  exercise_dkg_round_trip(coinbase::api::curve_id::p256);
  exercise_dkg_ac_round_trip(coinbase::api::curve_id::secp256k1);
  exercise_dkg_ac_round_trip(coinbase::api::curve_id::p256);
}

TEST(ApiTdh2, InvalidCurveRejected) {
  failing_transport_t t;
  std::vector<std::string_view> names = {"p0", "p1"};
  job_mp_t job{/*self=*/0, names, t};

  buf_t public_key;
  std::vector<buf_t> public_shares;
  buf_t private_share;
  buf_t sid;
  EXPECT_EQ(
      coinbase::api::tdh2::dkg_additive(job, static_cast<curve_id>(42), public_key, public_shares, private_share, sid),
      E_BADARG);
}
