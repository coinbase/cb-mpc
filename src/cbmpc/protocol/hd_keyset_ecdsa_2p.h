#pragma once
#include <cbmpc/protocol/ec_dkg.h>
#include <cbmpc/protocol/ecdsa_2p.h>
#include <cbmpc/protocol/hd_tree_bip32.h>
#include <cbmpc/protocol/mpc_job.h>

namespace coinbase::mpc {

struct key_share_ecdsa_hdmpc_2p_t {
  hd_root_t root;
  coinbase::crypto::paillier_t paillier;
  bn_t c_key;

  ecurve_t curve;
  party_idx_t party_index;

  /**
   * @specs:
   * - mpc-friendly-derivation-spec | Init-Derive-2P
   */
  static error_t dkg(job_2p_t& job, ecurve_t curve, key_share_ecdsa_hdmpc_2p_t& key);

  /**
   * @specs:
   * - mpc-friendly-derivation-spec | Hard-Derive-2P
   */
  static error_t derive_keys(job_2p_t& job, const key_share_ecdsa_hdmpc_2p_t& key, const bip32_path_t& hardened_path,
                             const std::vector<bip32_path_t>& non_hardened_paths, buf_t& sid,
                             std::vector<ecdsa2pc::key_t>& derived_keys);

  /**
   * @specs:
   * - mpc-friendly-derivation-spec | VRF-Refresh-2P
   * @notes:
   * - The initial part of this function is exactly the same as the ecdsa-2pc key refresh.
   *   Its only deviation is generating "two" delta values instead of one so that it can be used
   *   for refreshing both x_share and k_share as opposed to refreshing a single x.
   */
  static error_t refresh(job_2p_t& job, key_share_ecdsa_hdmpc_2p_t& current_keyset,
                         key_share_ecdsa_hdmpc_2p_t& new_keyset);
};

};  // namespace coinbase::mpc