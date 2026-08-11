#include <gtest/gtest.h>

#include <cbmpc/api/ecdsa_2p.h>
#include <cbmpc/api/ecdsa_mp.h>
#include <cbmpc/api/eddsa_2p.h>
#include <cbmpc/api/eddsa_mp.h>
#include <cbmpc/api/schnorr_2p.h>
#include <cbmpc/api/schnorr_mp.h>

#include "test_transport_harness.h"

namespace {

using coinbase::buf_t;
using coinbase::mem_t;
using coinbase::api::access_structure_t;
using coinbase::api::job_2p_t;
using coinbase::api::job_mp_t;
using coinbase::api::party_2p_t;
using coinbase::testutils::api_harness::failing_transport_t;

buf_t stale_signature() {
  constexpr uint8_t stale[] = {0x53, 0x49, 0x47};
  return buf_t(stale, sizeof(stale));
}

access_structure_t two_of_two_access_structure() {
  return access_structure_t::And({
      access_structure_t::leaf("p0"),
      access_structure_t::leaf("p1"),
  });
}

TEST(ApiSigningOutput, TwoPartyWrappersClearOutputBeforeJobValidationFailure) {
  failing_transport_t transport;
  const job_2p_t invalid_job{party_2p_t::p1, "", "p2", transport};
  const uint8_t key_blob_bytes[] = {0x01};
  const mem_t key_blob(key_blob_bytes, sizeof(key_blob_bytes));
  const buf_t msg(32);
  buf_t sid;

  buf_t ecdsa_sig = stale_signature();
  EXPECT_NE(coinbase::api::ecdsa_2p::sign(invalid_job, key_blob, msg, sid, ecdsa_sig), SUCCESS);
  EXPECT_TRUE(ecdsa_sig.empty());

  buf_t eddsa_sig = stale_signature();
  EXPECT_NE(coinbase::api::eddsa_2p::sign(invalid_job, key_blob, msg, eddsa_sig), SUCCESS);
  EXPECT_TRUE(eddsa_sig.empty());

  buf_t schnorr_sig = stale_signature();
  EXPECT_NE(coinbase::api::schnorr_2p::sign(invalid_job, key_blob, msg, schnorr_sig), SUCCESS);
  EXPECT_TRUE(schnorr_sig.empty());
}

TEST(ApiSigningOutput, AdditiveWrappersClearOutputBeforeJobValidationFailure) {
  failing_transport_t transport;
  const job_mp_t invalid_job{/*self=*/0, {"p0"}, transport};
  const uint8_t key_blob_bytes[] = {0x01};
  const mem_t key_blob(key_blob_bytes, sizeof(key_blob_bytes));
  const buf_t msg(32);

  buf_t ecdsa_sig = stale_signature();
  EXPECT_NE(coinbase::api::ecdsa_mp::sign_additive(invalid_job, key_blob, msg, /*sig_receiver=*/0, ecdsa_sig), SUCCESS);
  EXPECT_TRUE(ecdsa_sig.empty());

  buf_t eddsa_sig = stale_signature();
  EXPECT_NE(coinbase::api::eddsa_mp::sign_additive(invalid_job, key_blob, msg, /*sig_receiver=*/0, eddsa_sig), SUCCESS);
  EXPECT_TRUE(eddsa_sig.empty());

  buf_t schnorr_sig = stale_signature();
  EXPECT_NE(coinbase::api::schnorr_mp::sign_additive(invalid_job, key_blob, msg, /*sig_receiver=*/0, schnorr_sig),
            SUCCESS);
  EXPECT_TRUE(schnorr_sig.empty());
}

TEST(ApiSigningOutput, AccessStructureWrappersClearOutputBeforeJobValidationFailure) {
  failing_transport_t transport;
  const job_mp_t invalid_job{/*self=*/0, {"p0"}, transport};
  const uint8_t key_blob_bytes[] = {0x01};
  const mem_t key_blob(key_blob_bytes, sizeof(key_blob_bytes));
  const buf_t msg(32);
  const access_structure_t access_structure = two_of_two_access_structure();

  buf_t ecdsa_sig = stale_signature();
  EXPECT_NE(
      coinbase::api::ecdsa_mp::sign_ac(invalid_job, key_blob, access_structure, msg, /*sig_receiver=*/0, ecdsa_sig),
      SUCCESS);
  EXPECT_TRUE(ecdsa_sig.empty());

  buf_t eddsa_sig = stale_signature();
  EXPECT_NE(
      coinbase::api::eddsa_mp::sign_ac(invalid_job, key_blob, access_structure, msg, /*sig_receiver=*/0, eddsa_sig),
      SUCCESS);
  EXPECT_TRUE(eddsa_sig.empty());

  buf_t schnorr_sig = stale_signature();
  EXPECT_NE(
      coinbase::api::schnorr_mp::sign_ac(invalid_job, key_blob, access_structure, msg, /*sig_receiver=*/0, schnorr_sig),
      SUCCESS);
  EXPECT_TRUE(schnorr_sig.empty());
}

TEST(ApiSigningOutput, WrappersClearOutputForOtherEarlyValidationFailures) {
  failing_transport_t transport;
  const job_2p_t job_2p{party_2p_t::p1, "p1", "p2", transport};
  const job_mp_t job_mp{/*self=*/0, {"p0", "p1"}, transport};
  const uint8_t invalid_key_blob_bytes[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const mem_t invalid_key_blob(invalid_key_blob_bytes, sizeof(invalid_key_blob_bytes));
  const buf_t msg(32);
  const buf_t invalid_schnorr_msg(31);
  const access_structure_t access_structure = two_of_two_access_structure();
  buf_t sid;

  buf_t ecdsa_2p_sig = stale_signature();
  EXPECT_NE(coinbase::api::ecdsa_2p::sign(job_2p, invalid_key_blob, msg, sid, ecdsa_2p_sig), SUCCESS);
  EXPECT_TRUE(ecdsa_2p_sig.empty());

  buf_t eddsa_2p_sig = stale_signature();
  EXPECT_NE(coinbase::api::eddsa_2p::sign(job_2p, invalid_key_blob, mem_t(), eddsa_2p_sig), SUCCESS);
  EXPECT_TRUE(eddsa_2p_sig.empty());

  buf_t schnorr_2p_sig = stale_signature();
  EXPECT_NE(coinbase::api::schnorr_2p::sign(job_2p, invalid_key_blob, invalid_schnorr_msg, schnorr_2p_sig), SUCCESS);
  EXPECT_TRUE(schnorr_2p_sig.empty());

  buf_t ecdsa_additive_sig = stale_signature();
  EXPECT_NE(
      coinbase::api::ecdsa_mp::sign_additive(job_mp, invalid_key_blob, msg, /*sig_receiver=*/-1, ecdsa_additive_sig),
      SUCCESS);
  EXPECT_TRUE(ecdsa_additive_sig.empty());

  buf_t eddsa_additive_sig = stale_signature();
  EXPECT_NE(
      coinbase::api::eddsa_mp::sign_additive(job_mp, invalid_key_blob, msg, /*sig_receiver=*/0, eddsa_additive_sig),
      SUCCESS);
  EXPECT_TRUE(eddsa_additive_sig.empty());

  buf_t schnorr_additive_sig = stale_signature();
  EXPECT_NE(coinbase::api::schnorr_mp::sign_additive(job_mp, invalid_key_blob, invalid_schnorr_msg,
                                                     /*sig_receiver=*/0, schnorr_additive_sig),
            SUCCESS);
  EXPECT_TRUE(schnorr_additive_sig.empty());

  buf_t ecdsa_ac_sig = stale_signature();
  EXPECT_NE(coinbase::api::ecdsa_mp::sign_ac(job_mp, invalid_key_blob, access_structure, msg, /*sig_receiver=*/-1,
                                             ecdsa_ac_sig),
            SUCCESS);
  EXPECT_TRUE(ecdsa_ac_sig.empty());

  buf_t eddsa_ac_sig = stale_signature();
  EXPECT_NE(coinbase::api::eddsa_mp::sign_ac(job_mp, invalid_key_blob, access_structure, msg, /*sig_receiver=*/0,
                                             eddsa_ac_sig),
            SUCCESS);
  EXPECT_TRUE(eddsa_ac_sig.empty());

  buf_t schnorr_ac_sig = stale_signature();
  EXPECT_NE(coinbase::api::schnorr_mp::sign_ac(job_mp, invalid_key_blob, access_structure, invalid_schnorr_msg,
                                               /*sig_receiver=*/0, schnorr_ac_sig),
            SUCCESS);
  EXPECT_TRUE(schnorr_ac_sig.empty());
}

}  // namespace
