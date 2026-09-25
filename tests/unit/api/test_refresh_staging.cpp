#include <algorithm>
#include <chrono>
#include <future>
#include <gtest/gtest.h>

#include <cbmpc/api/schnorr_mp.h>
#include <cbmpc/internal/crypto/base_ecc_secp256k1.h>
#include <cbmpc/internal/protocol/ec_dkg.h>

#include "test_transport_harness.h"

namespace {
using namespace coinbase;
using namespace coinbase::api;
using namespace coinbase::testutils;
using namespace coinbase::testutils::api_harness;

// A malicious participant's transport changes only the opening to one recipient.
// With a caller-provided SID, its third send to that peer carries the opening.
class selective_opening_transport_t final : public data_transport_i {
 public:
  explicit selective_opening_transport_t(data_transport_i& inner) : inner_(inner) {}
  bool mutated = false;
  error_t send(party_idx_t receiver, mem_t msg) override {
    if (receiver == 1 && ++victim_sends_ == 3) {
      buf256_t hash, rho;
      crypto::ss::ac_internal_pub_shares_t pubs;
      zk::uc_batch_dl_t proof;
      crypto::bn_t share;
      const auto rv = deser(msg, hash, pubs, proof, rho, share);
      if (!rv) {
        rho[0] ^= 1;
        const buf_t corrupt = ser(hash, pubs, proof, rho, share);
        mutated = true;
        return inner_.send(receiver, corrupt);
      }
      // Forward unchanged on an unexpected layout; the test's mutation assertion fails.
    }
    return inner_.send(receiver, msg);
  }
  error_t receive(party_idx_t sender, buf_t& msg) override { return inner_.receive(sender, msg); }
  error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& msgs) override {
    return inner_.receive_all(senders, msgs);
  }

 private:
  data_transport_i& inner_;
  int victim_sends_ = 0;
};

TEST(ApiSchnorrMpAc, RefreshSelectiveFailureDoesNotActivateCandidates) {
  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  for (int i = 0; i < 3; ++i) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);
  std::vector<std::shared_ptr<local_api_transport_t>> transports;
  for (const auto& p : peers) transports.push_back(std::make_shared<local_api_transport_t>(p));
  const std::vector<std::string_view> names = {"honest0", "honest1", "malicious"};
  const auto ac =
      access_structure_t::Threshold(2, {access_structure_t::leaf("honest0"), access_structure_t::leaf("honest1"),
                                        access_structure_t::leaf("malicious")});
  std::vector<buf_t> active(3), sids(3), pending(3);
  std::vector<error_t> results;
  run_mp(
      peers,
      [&](int i) {
        job_mp_t job{party_idx_t(i), names, *transports[i]};
        return schnorr_mp::dkg_ac(job, curve_id::secp256k1, sids[i], ac, names, active[i]);
      },
      results);
  for (auto rv : results) ASSERT_EQ(rv, SUCCESS);
  const auto original = active;
  for (auto& sid : sids) sid = buf_t("unique-refresh-session-for-test");
  selective_opening_transport_t faulty(*transports[2]);
  std::promise<void> completer_done;
  auto done = completer_done.get_future();
  // The generic harness aborts every peer on any error. Delay that notification until
  // the unaffected honest peer finishes, to observe the protocol's local outcomes.
  run_mp(
      peers,
      [&](int i) {
        data_transport_i& transport = i == 2 ? static_cast<data_transport_i&>(faulty) : *transports[i];
        job_mp_t job{party_idx_t(i), names, transport};
        const auto rv = schnorr_mp::refresh_ac(job, sids[i], active[i], ac, names, pending[i]);
        if (i == 0) completer_done.set_value();
        if (i == 1 && rv) done.wait_for(std::chrono::seconds(10));
        return rv;
      },
      results);
  ASSERT_TRUE(faulty.mutated);
  ASSERT_EQ(results[0], SUCCESS);
  ASSERT_EQ(results[1], E_CRYPTO);
  ASSERT_FALSE(pending[0].empty());
  ASSERT_TRUE(pending[1].empty());
  buf_t before_Q, after_Q;
  ASSERT_EQ(schnorr_mp::get_public_key_compressed(active[0], before_Q), SUCCESS);
  ASSERT_EQ(schnorr_mp::get_public_key_compressed(pending[0], after_Q), SUCCESS);
  EXPECT_EQ(before_Q, after_Q);  // Comparing Q alone would not detect partial success.
  EXPECT_NE(pending[0], active[0]);

  // Local orchestration example ONLY: do not activate on partial success. A real
  // distributed application additionally needs authenticated readiness and a durable
  // commit/recovery decision. This is not that distributed transaction implementation.
  const bool all_ready = std::all_of(results.begin(), results.end(), [](error_t rv) { return rv == SUCCESS; });
  ASSERT_FALSE(all_ready);
  if (all_ready)
    active = pending;
  else
    pending.clear();
  EXPECT_EQ(active, original);

  // Keeping the last committed epoch intact preserves the honest-only signing quorum.
  const std::vector<std::shared_ptr<mpc_net_context_t>> honest = {peers[0], peers[1]};
  const std::vector<std::string_view> signers = {names[0], names[1]};
  buf_t msg(32);
  msg.bzero();
  std::vector<buf_t> sigs(2);
  run_mp(
      honest,
      [&](int i) {
        job_mp_t job{party_idx_t(i), signers, *transports[i]};
        return schnorr_mp::sign_ac(job, active[i], ac, msg, 0, sigs[i]);
      },
      results);
  for (auto rv : results) ASSERT_EQ(rv, SUCCESS);
  crypto::ecc_point_t Q;
  ASSERT_EQ(Q.from_bin(crypto::curve_secp256k1, before_Q), SUCCESS);
  EXPECT_EQ(crypto::bip340::verify(Q, msg, sigs[0]), SUCCESS);
}
}  // namespace
