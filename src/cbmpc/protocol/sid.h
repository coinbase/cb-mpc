#pragma once

#include <cbmpc/protocol/agree_random.h>
#include <cbmpc/protocol/mpc_job.h>

namespace coinbase::mpc {

/**
 * @specs:
 * - basic-primitives-spec | GenerateSID-Fixed-2P
 */
inline error_t generate_sid_fixed_2p(mpc::job_2p_t& job, party_t first_sender, buf_t& sid) {
  if (first_sender == party_t::p1)
    return weak_agree_random_p1_first(job, SEC_P_COM, sid);
  else
    return weak_agree_random_p2_first(job, SEC_P_COM, sid);
}

/**
 * @specs:
 * - basic-primitives-spec | GenerateSID-Dynamic-2P
 */
inline error_t generate_sid_dynamic_2p(mpc::job_2p_t& job, party_t first_sender, crypto::mpc_pid_t pid1,
                                       crypto::mpc_pid_t pid2, buf_t& sid) {
  error_t rv = UNINITIALIZED_ERROR;
  buf_t sid_tag;
  if (rv = generate_sid_fixed_2p(job, first_sender, sid_tag)) return rv;

  if (pid1 < pid2)
    sid = crypto::sha256_t::hash(sid_tag, pid1, pid2);
  else
    sid = crypto::sha256_t::hash(sid_tag, pid2, pid1);

  return SUCCESS;
}

/**
 * @specs:
 * - basic-primitives-spec | GenerateSID-Fixed-MP
 */
inline error_t generate_sid_fixed_mp(mpc::job_mp_t& job, buf_t& sid) {
  error_t rv = UNINITIALIZED_ERROR;
  auto sid_msg = job.uniform_msg<buf_t>(crypto::gen_random_bitlen(SEC_P_COM));
  if (rv = job.plain_broadcast(sid_msg)) return rv;
  sid = buf_t(crypto::sha256_t::hash(sid_msg.all_received_refs())).take(bits_to_bytes(SEC_P_COM));
  return SUCCESS;
}

/**
 * @specs:
 * - basic-primitives-spec | GenerateSID-Dynamic-MP
 */
inline error_t generate_sid_dynamic_mp(mpc::job_mp_t& job, std::vector<crypto::mpc_pid_t> pids, buf_t& sid) {
  error_t rv = UNINITIALIZED_ERROR;
  buf_t sid_tag;
  if (rv = generate_sid_fixed_mp(job, sid_tag)) return rv;

  std::sort(pids.begin(), pids.end());
  sid = crypto::sha256_t::hash(sid_tag, pids);

  return SUCCESS;
}

}  // namespace coinbase::mpc
