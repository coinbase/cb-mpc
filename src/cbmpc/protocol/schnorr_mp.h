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
 * - ec-dkg-spec | EC-DKG-MP
 */
error_t dkg(job_mp_t& job, ecurve_t curve, key_t& key, buf_t& sid);

/**
 * @specs:
 * - ec-dkg-spec | EC-Refresh-MP
 */
error_t refresh(job_mp_t& job, buf_t& sid, key_t& key, key_t& new_key);

/**
 * @specs:
 * - ec-dkg-spec | EC-DKG-Threshold-MP
 */
error_t threshold_dkg(job_mp_t& job, ecurve_t curve, buf_t& sid, const crypto::ss::ac_t ac,
                      const party_set_t& quorum_party_set, key_t& key);

/**
 * @specs:
 * - ec-dkg-spec | EC-Refresh-Threshold-MP
 */
error_t threshold_refresh(job_mp_t& job, ecurve_t curve, buf_t& sid, const crypto::ss::ac_t ac,
                          const party_set_t& quorum_party_set, key_t& key, key_t& new_key);

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
