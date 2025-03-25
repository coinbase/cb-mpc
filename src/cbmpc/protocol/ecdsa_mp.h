#pragma once

#include <stdint.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/ec_dkg.h>
#include <cbmpc/protocol/mpc_job.h>

namespace coinbase::mpc::ecdsampc {

// 256 provides 64-bit statistical security due to OT Multiplication
constexpr int kappa = 256;

enum {
  ot_no_role = -1,
  ot_sender = 0,
  ot_receiver = 1,
};

typedef eckey::key_share_mp_t key_t;

/**
 * @specs:
 * - ecdsa-mpc-spec | ECDSA-MPC-KeyGen-MP
 */
error_t dkg(job_mp_t& job, ecurve_t curve, key_t& key, buf_t& sid);

/**
 * @specs:
 * - ecdsa-mpc-spec | ECDSA-MPC-Refresh-MP
 */
error_t refresh(job_mp_t& job, buf_t& sid, key_t& key, key_t& new_key);

/**
 * @specs:
 * - ecdsa-mpc-spec | ECDSA-MPC-Sign-MP
 * @notes:
 * - This function runs base OT internally which is not efficient and is only done for ease of use.
 *   The proper more efficient way is to generate Base OTs one outside this function, then during the run of the
 *   protocol, use OT Extension to generate extra values and output them to be used as base OT for the next execution of
 *   the protocol.
 */
error_t sign(job_mp_t& job, key_t& key, mem_t msg, const party_idx_t sig_receiver,
             const std::vector<std::vector<int>>& ot_role_map, buf_t& sig);

/**
 * @specs:
 * - ecdsa-mpc-spec | ECDSA-MPC-Sign-MP
 * @notes:
 * - The difference between this function and the one above is that this function does not take `ot_role_map` as an
 * argument.
 * - This function runs base OT internally which is not efficient and is only done for ease of use.
 *   The proper more efficient way is to generate Base OTs one outside this function, then during the run of the
 *   protocol, use OT Extension to generate extra values and output them to be used as base OT for the next execution of
 *   the protocol.
 */
error_t sign(job_mp_t& job, key_t& key, mem_t msg, const party_idx_t sig_receiver, buf_t& sig);

static party_set_t ot_senders_for(int i, int peers_count, std::vector<std::vector<int>> ot_role_map) {
  party_set_t s;
  for (int j = 0; j < peers_count; j++) {
    if (ot_role_map[i][j] == ot_receiver) s.add(j);
  }
  return s;
}

static party_set_t ot_receivers_for(int i, int peers_count, std::vector<std::vector<int>> ot_role_map) {
  party_set_t s;
  for (int j = 0; j < peers_count; j++) {
    if (ot_role_map[i][j] == ot_sender) s.add(j);
  }
  return s;
}

/**
 * This is a essentially a set intersection that returns [1, ..., n] - self - receivers
 */
static party_set_t get_senders_from_receivers(const job_mp_t& job, party_set_t receivers) {
  party_set_t senders = party_set_t(0);
  for (int i = 0; i < job.get_n_parties(); i++) {
    if (i == job.get_party_idx()) continue;
    if (receivers.has(i)) continue;
    senders.add(i);
  }
  return senders;
}

/**
 * Receivers get pairwise_msg and everyone sends and receives to_all_msgs which is like
 * a broadcast message communication
 */
template <typename OT_MSG, typename... TO_ALL_MSG>
error_t plain_broadcast_and_pairwise_message(job_mp_t& job, party_set_t receivers, OT_MSG& pairwise_msg,
                                             TO_ALL_MSG&... to_all_msgs) {
  error_t rv = UNINITIALIZED_ERROR;
  party_set_t senders = get_senders_from_receivers(job, receivers);

  if constexpr (sizeof...(to_all_msgs) == 0) {
    if (rv = job.group_message(receivers, senders, pairwise_msg)) return rv;
  } else {
    auto to_all_msg = job_mp_t::tie_msgs(to_all_msgs...);
    auto all_parties = party_set_t::all();

    if (rv = job.group_message(                          //
            std::tie(receivers, senders, pairwise_msg),  //
            std::tie(all_parties, all_parties, to_all_msg)))
      return rv;
  }

  return SUCCESS;
}

}  // namespace coinbase::mpc::ecdsampc