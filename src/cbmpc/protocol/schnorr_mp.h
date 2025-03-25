#pragma once

#include <stdint.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/ec_dkg.h>
#include <cbmpc/protocol/mpc_job.h>

namespace coinbase::mpc::schnorrmp {

using key_t = eckey::key_share_mp_t;

enum class variant_e {
  EdDSA,
  BIP340,
};

/**
 * @specs:
 * - schnorr-spec | Schnorr-MPC-Sign-MP
 */
error_t sign_batch(job_mp_t& job, key_t& key, const std::vector<mem_t>& msgs, party_idx_t sig_receiver,
                   std::vector<buf_t>& sigs, variant_e variant);

/**
 * @specs:
 * - schnorr-spec | Schnorr-MPC-Sign-MP
 */
error_t sign(job_mp_t& job, key_t& key, const mem_t& msg, party_idx_t sig_receiver, buf_t& sig, variant_e variant);
}  // namespace coinbase::mpc::schnorrmp
