#pragma once
#include <cbmpc/protocol/mpc_job.h>

namespace coinbase::mpc {

/**
 * @specs:
 * - basic-primitives-spec | AgreeRandom-2P
 */
error_t agree_random(job_2p_t& job, int bitlen, buf_t& out);

/**
 * @specs:
 * - basic-primitives-spec | WeakAgreeRandom-2P
 *
 * @notes:
 * - This is used to save a round when this is called in another protocol in which
 *   P1 is the first to send.
 */
error_t weak_agree_random_p1_first(job_2p_t& job, int bitlen, buf_t& out);

/**
 * @specs:
 * - basic-primitives-spec | WeakAgreeRandom-2P
 *
 * @notes:
 * - This is used to save a round when this is called in another protocol in which
 *   P2 is the first to send.
 */
error_t weak_agree_random_p2_first(job_2p_t& job, int bitlen, buf_t& out);

/**
 * @specs:
 * - basic-primitives-spec | MultiAgreeRandom-MP
 */
error_t multi_agree_random(job_mp_t& job, int bitlen, buf_t& out);

/**
 * @specs:
 * - basic-primitives-spec | WeakMultiAgreeRandom-MP
 */
error_t weak_multi_agree_random(job_mp_t& job, int bitlen, buf_t& out);

/**
 * @specs:
 * - basic-primitives-spec | MultiPairwiseAgreeRandom-MP
 */
error_t multi_pairwise_agree_random(job_mp_t& job, int bitlen, std::vector<buf_t>& out);

}  // namespace coinbase::mpc