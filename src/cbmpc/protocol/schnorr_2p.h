#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/ec_dkg.h>
#include <cbmpc/protocol/mpc_job.h>
#include <cbmpc/zk/zk_ec.h>
#include <cbmpc/zk/zk_paillier.h>

namespace coinbase::mpc::schnorr2p {

using key_t = eckey::key_share_2p_t;

enum class variant_e {
  EdDSA,
  BIP340,
};

/**
 * @specs:
 * - schnorr-spec | Schnorr-2PC-Sign-2P
 */
error_t sign_batch(job_2p_t& job, key_t& key, const std::vector<mem_t>& msgs, std::vector<buf_t>& sigs,
                   variant_e variant);

/**
 * @specs:
 * - schnorr-spec | Schnorr-2PC-Sign-2P
 */
error_t sign(job_2p_t& job, key_t& key, const mem_t& msg, buf_t& sig, variant_e variant);

}  // namespace coinbase::mpc::schnorr2p