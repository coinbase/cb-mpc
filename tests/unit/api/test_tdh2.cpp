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

// ------------ Disclaimer: All the following tests have been generated by AI ------------

#include <cbmpc/internal/core/log.h>

TEST(ApiTdh2Neg, DkgAdditiveInvalidCurve0) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t public_key;
  std::vector<buf_t> public_shares;
  buf_t private_share;
  buf_t sid;
  EXPECT_NE(
      coinbase::api::tdh2::dkg_additive(job, static_cast<curve_id>(0), public_key, public_shares, private_share, sid),
      SUCCESS);
}

TEST(ApiTdh2Neg, DkgAdditiveInvalidCurve255) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t public_key;
  std::vector<buf_t> public_shares;
  buf_t private_share;
  buf_t sid;
  EXPECT_NE(
      coinbase::api::tdh2::dkg_additive(job, static_cast<curve_id>(255), public_key, public_shares, private_share, sid),
      SUCCESS);
}

TEST(ApiTdh2Neg, DkgAdditiveEd25519Rejected) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t public_key;
  std::vector<buf_t> public_shares;
  buf_t private_share;
  buf_t sid;
  EXPECT_NE(coinbase::api::tdh2::dkg_additive(job, curve_id::ed25519, public_key, public_shares, private_share, sid),
            SUCCESS);
}

TEST(ApiTdh2Neg, DkgAdditiveEmptyPartyList) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t public_key;
  std::vector<buf_t> public_shares;
  buf_t private_share;
  buf_t sid;
  EXPECT_NE(coinbase::api::tdh2::dkg_additive(job, curve_id::secp256k1, public_key, public_shares, private_share, sid),
            SUCCESS);
}

TEST(ApiTdh2Neg, DkgAdditiveSingleParty) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0"};
  job_mp_t job{/*self=*/0, names, ft};
  buf_t public_key;
  std::vector<buf_t> public_shares;
  buf_t private_share;
  buf_t sid;
  EXPECT_NE(coinbase::api::tdh2::dkg_additive(job, curve_id::secp256k1, public_key, public_shares, private_share, sid),
            SUCCESS);
}

TEST(ApiTdh2Neg, DkgAcInvalidCurve0) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  const std::vector<std::string_view> quorum = {"p0", "p1"};
  buf_t public_key;
  std::vector<buf_t> public_shares;
  buf_t private_share;
  buf_t sid;
  EXPECT_NE(coinbase::api::tdh2::dkg_ac(job, static_cast<curve_id>(0), sid, ac, quorum, public_key, public_shares,
                                        private_share),
            SUCCESS);
}

TEST(ApiTdh2Neg, DkgAcEd25519Rejected) {
  failing_transport_t ft;
  std::vector<std::string_view> names = {"p0", "p1", "p2"};
  job_mp_t job{/*self=*/0, names, ft};
  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  const std::vector<std::string_view> quorum = {"p0", "p1"};
  buf_t public_key;
  std::vector<buf_t> public_shares;
  buf_t private_share;
  buf_t sid;
  EXPECT_NE(
      coinbase::api::tdh2::dkg_ac(job, curve_id::ed25519, sid, ac, quorum, public_key, public_shares, private_share),
      SUCCESS);
}

TEST(ApiTdh2Neg, EncryptEmptyPublicKey) {
  dylog_disable_scope_t no_log;
  buf_t ct;
  buf_t pt(16);
  EXPECT_NE(coinbase::api::tdh2::encrypt(mem_t(), pt, mem_t("label"), ct), SUCCESS);
}

TEST(ApiTdh2Neg, EncryptGarbagePublicKey) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t ct;
  buf_t pt(16);
  EXPECT_NE(coinbase::api::tdh2::encrypt(mem_t(garbage, 4), pt, mem_t("label"), ct), SUCCESS);
}

TEST(ApiTdh2Neg, EncryptEmptyPlaintext) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t ct;
  EXPECT_NE(coinbase::api::tdh2::encrypt(mem_t(garbage, 4), mem_t(), mem_t("label"), ct), SUCCESS);
}

TEST(ApiTdh2Neg, EncryptEmptyLabel) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t ct;
  buf_t pt(16);
  EXPECT_NE(coinbase::api::tdh2::encrypt(mem_t(garbage, 4), pt, mem_t(), ct), SUCCESS);
}

TEST(ApiTdh2Neg, VerifyEmptyPublicKey) {
  dylog_disable_scope_t no_log;
  EXPECT_NE(coinbase::api::tdh2::verify(mem_t(), mem_t("ct"), mem_t("label")), SUCCESS);
}

TEST(ApiTdh2Neg, VerifyGarbagePublicKey) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  EXPECT_NE(coinbase::api::tdh2::verify(mem_t(garbage, 4), mem_t("ct"), mem_t("label")), SUCCESS);
}

TEST(ApiTdh2Neg, VerifyEmptyCiphertext) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  EXPECT_NE(coinbase::api::tdh2::verify(mem_t(garbage, 4), mem_t(), mem_t("label")), SUCCESS);
}

TEST(ApiTdh2Neg, VerifyGarbageCiphertext) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  EXPECT_NE(coinbase::api::tdh2::verify(mem_t(garbage, 4), mem_t(garbage, 4), mem_t("label")), SUCCESS);
}

TEST(ApiTdh2Neg, VerifyEmptyLabel) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  EXPECT_NE(coinbase::api::tdh2::verify(mem_t(garbage, 4), mem_t("ct"), mem_t()), SUCCESS);
}

TEST(ApiTdh2Neg, PartialDecryptEmptyPrivateShare) {
  dylog_disable_scope_t no_log;
  buf_t pd;
  EXPECT_NE(coinbase::api::tdh2::partial_decrypt(mem_t(), mem_t("ct"), mem_t("label"), pd), SUCCESS);
}

TEST(ApiTdh2Neg, PartialDecryptGarbagePrivateShare) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t pd;
  EXPECT_NE(coinbase::api::tdh2::partial_decrypt(mem_t(garbage, 4), mem_t("ct"), mem_t("label"), pd), SUCCESS);
}

TEST(ApiTdh2Neg, PartialDecryptEmptyCiphertext) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t pd;
  EXPECT_NE(coinbase::api::tdh2::partial_decrypt(mem_t(garbage, 4), mem_t(), mem_t("label"), pd), SUCCESS);
}

TEST(ApiTdh2Neg, PartialDecryptGarbageCiphertext) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t pd;
  EXPECT_NE(coinbase::api::tdh2::partial_decrypt(mem_t(garbage, 4), mem_t(garbage, 4), mem_t("label"), pd), SUCCESS);
}

TEST(ApiTdh2Neg, PartialDecryptEmptyLabel) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t pd;
  EXPECT_NE(coinbase::api::tdh2::partial_decrypt(mem_t(garbage, 4), mem_t("ct"), mem_t(), pd), SUCCESS);
}

TEST(ApiTdh2Neg, CombineAdditiveEmptyPublicKey) {
  dylog_disable_scope_t no_log;
  std::vector<mem_t> ps = {mem_t("s1")};
  std::vector<mem_t> pd = {mem_t("d1")};
  buf_t pt;
  EXPECT_NE(coinbase::api::tdh2::combine_additive(mem_t(), ps, mem_t("label"), pd, mem_t("ct"), pt), SUCCESS);
}

TEST(ApiTdh2Neg, CombineAdditiveGarbagePublicKey) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  std::vector<mem_t> ps = {mem_t("s1")};
  std::vector<mem_t> pd = {mem_t("d1")};
  buf_t pt;
  EXPECT_NE(coinbase::api::tdh2::combine_additive(mem_t(garbage, 4), ps, mem_t("label"), pd, mem_t("ct"), pt), SUCCESS);
}

TEST(ApiTdh2Neg, CombineAdditiveEmptyCiphertext) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  std::vector<mem_t> ps = {mem_t("s1")};
  std::vector<mem_t> pd = {mem_t("d1")};
  buf_t pt;
  EXPECT_NE(coinbase::api::tdh2::combine_additive(mem_t(garbage, 4), ps, mem_t("label"), pd, mem_t(), pt), SUCCESS);
}

TEST(ApiTdh2Neg, CombineAdditiveGarbageCiphertext) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  std::vector<mem_t> ps = {mem_t("s1")};
  std::vector<mem_t> pd = {mem_t("d1")};
  buf_t pt;
  EXPECT_NE(coinbase::api::tdh2::combine_additive(mem_t(garbage, 4), ps, mem_t("label"), pd, mem_t(garbage, 4), pt),
            SUCCESS);
}

TEST(ApiTdh2Neg, CombineAdditiveEmptyLabel) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  std::vector<mem_t> ps = {mem_t("s1")};
  std::vector<mem_t> pd = {mem_t("d1")};
  buf_t pt;
  EXPECT_NE(coinbase::api::tdh2::combine_additive(mem_t(garbage, 4), ps, mem_t(), pd, mem_t("ct"), pt), SUCCESS);
}

TEST(ApiTdh2Neg, CombineAdditiveSizeMismatch) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  std::vector<mem_t> ps = {mem_t("s1"), mem_t("s2")};
  std::vector<mem_t> pd = {mem_t("d1")};
  buf_t pt;
  EXPECT_NE(coinbase::api::tdh2::combine_additive(mem_t(garbage, 4), ps, mem_t("label"), pd, mem_t("ct"), pt), SUCCESS);
}

TEST(ApiTdh2Neg, CombineAdditiveEmptyVectors) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  std::vector<mem_t> ps;
  std::vector<mem_t> pd;
  buf_t pt;
  EXPECT_NE(coinbase::api::tdh2::combine_additive(mem_t(garbage, 4), ps, mem_t("label"), pd, mem_t("ct"), pt), SUCCESS);
}

TEST(ApiTdh2Neg, CombineAcEmptyPublicKey) {
  dylog_disable_scope_t no_log;
  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  std::vector<std::string_view> party_names = {"p0", "p1", "p2"};
  std::vector<mem_t> ps = {mem_t("s0"), mem_t("s1"), mem_t("s2")};
  std::vector<std::string_view> pn = {"p0", "p1"};
  std::vector<mem_t> pd = {mem_t("d0"), mem_t("d1")};
  buf_t pt;
  EXPECT_NE(coinbase::api::tdh2::combine_ac(ac, mem_t(), party_names, ps, mem_t("label"), pn, pd, mem_t("ct"), pt),
            SUCCESS);
}

TEST(ApiTdh2Neg, CombineAcGarbagePublicKey) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  std::vector<std::string_view> party_names = {"p0", "p1", "p2"};
  std::vector<mem_t> ps = {mem_t("s0"), mem_t("s1"), mem_t("s2")};
  std::vector<std::string_view> pn = {"p0", "p1"};
  std::vector<mem_t> pd = {mem_t("d0"), mem_t("d1")};
  buf_t pt;
  EXPECT_NE(
      coinbase::api::tdh2::combine_ac(ac, mem_t(garbage, 4), party_names, ps, mem_t("label"), pn, pd, mem_t("ct"), pt),
      SUCCESS);
}

TEST(ApiTdh2Neg, CombineAcEmptyCiphertext) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  std::vector<std::string_view> party_names = {"p0", "p1", "p2"};
  std::vector<mem_t> ps = {mem_t("s0"), mem_t("s1"), mem_t("s2")};
  std::vector<std::string_view> pn = {"p0", "p1"};
  std::vector<mem_t> pd = {mem_t("d0"), mem_t("d1")};
  buf_t pt;
  EXPECT_NE(
      coinbase::api::tdh2::combine_ac(ac, mem_t(garbage, 4), party_names, ps, mem_t("label"), pn, pd, mem_t(), pt),
      SUCCESS);
}

TEST(ApiTdh2Neg, CombineAcEmptyLabel) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  std::vector<std::string_view> party_names = {"p0", "p1", "p2"};
  std::vector<mem_t> ps = {mem_t("s0"), mem_t("s1"), mem_t("s2")};
  std::vector<std::string_view> pn = {"p0", "p1"};
  std::vector<mem_t> pd = {mem_t("d0"), mem_t("d1")};
  buf_t pt;
  EXPECT_NE(coinbase::api::tdh2::combine_ac(ac, mem_t(garbage, 4), party_names, ps, mem_t(), pn, pd, mem_t("ct"), pt),
            SUCCESS);
}

TEST(ApiTdh2Neg, CombineAcPartySharesSizeMismatch) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  std::vector<std::string_view> party_names = {"p0", "p1", "p2"};
  std::vector<mem_t> ps = {mem_t("s0"), mem_t("s1")};
  std::vector<std::string_view> pn = {"p0", "p1"};
  std::vector<mem_t> pd = {mem_t("d0"), mem_t("d1")};
  buf_t pt;
  EXPECT_NE(
      coinbase::api::tdh2::combine_ac(ac, mem_t(garbage, 4), party_names, ps, mem_t("label"), pn, pd, mem_t("ct"), pt),
      SUCCESS);
}

TEST(ApiTdh2Neg, CombineAcPartialsSizeMismatch) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  std::vector<std::string_view> party_names = {"p0", "p1", "p2"};
  std::vector<mem_t> ps = {mem_t("s0"), mem_t("s1"), mem_t("s2")};
  std::vector<std::string_view> pn = {"p0", "p1"};
  std::vector<mem_t> pd = {mem_t("d0")};
  buf_t pt;
  EXPECT_NE(
      coinbase::api::tdh2::combine_ac(ac, mem_t(garbage, 4), party_names, ps, mem_t("label"), pn, pd, mem_t("ct"), pt),
      SUCCESS);
}

TEST(ApiTdh2Neg, CombineAcEmptyPartyName) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  std::vector<std::string_view> party_names = {"p0", "", "p2"};
  std::vector<mem_t> ps = {mem_t("s0"), mem_t("s1"), mem_t("s2")};
  std::vector<std::string_view> pn = {"p0", "p1"};
  std::vector<mem_t> pd = {mem_t("d0"), mem_t("d1")};
  buf_t pt;
  EXPECT_NE(
      coinbase::api::tdh2::combine_ac(ac, mem_t(garbage, 4), party_names, ps, mem_t("label"), pn, pd, mem_t("ct"), pt),
      SUCCESS);
}

TEST(ApiTdh2Neg, CombineAcDuplicatePartyName) {
  dylog_disable_scope_t no_log;
  uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const access_structure_t ac = access_structure_t::Threshold(
      2, {access_structure_t::leaf("p0"), access_structure_t::leaf("p1"), access_structure_t::leaf("p2")});
  std::vector<std::string_view> party_names = {"p0", "p0", "p2"};
  std::vector<mem_t> ps = {mem_t("s0"), mem_t("s1"), mem_t("s2")};
  std::vector<std::string_view> pn = {"p0", "p1"};
  std::vector<mem_t> pd = {mem_t("d0"), mem_t("d1")};
  buf_t pt;
  EXPECT_NE(
      coinbase::api::tdh2::combine_ac(ac, mem_t(garbage, 4), party_names, ps, mem_t("label"), pn, pd, mem_t("ct"), pt),
      SUCCESS);
}
