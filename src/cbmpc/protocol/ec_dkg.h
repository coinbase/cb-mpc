#pragma once

#include <stdint.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/lagrange.h>
#include <cbmpc/crypto/secret_sharing.h>
#include <cbmpc/protocol/mpc_job.h>
#include <cbmpc/zk/zk_ec.h>
#include <cbmpc/zk/zk_paillier.h>

#include "util.h"

namespace coinbase::mpc::eckey {
struct dkg_2p_t {
  const crypto::mpc_pid_t& p1_pid;
  dkg_2p_t(ecurve_t _curve, const crypto::mpc_pid_t& pid1) : curve(_curve), p1_pid(pid1) {}

  ecurve_t curve;
  buf_t sid;
  buf_t sid1, sid2;
  bn_t x1, x2;
  zk::uc_dl_t pi_1, pi_2;
  ecc_point_t Q1, Q2;
  coinbase::crypto::commitment_t com;

  void step1_p1_to_p2(const bn_t& x1);
  void step2_p2_to_p1(const bn_t& x2);
  error_t step3_p1_to_p2(ecc_point_t& Q);
  error_t step4_output_p2(ecc_point_t& Q);

  AUTO(msg1, std::tie(sid1, com.msg));
  AUTO(msg2, std::tie(sid2, pi_2, Q2));
  AUTO(msg3, std::tie(com.rand, pi_1, Q1));
};

struct key_share_2p_t {
  party_t role;
  ecurve_t curve;
  ecc_point_t Q;
  bn_t x_share;

  /**
   * @specs:
   * - ec-dkg-spec | EC-DKG-2P
   */
  static error_t dkg(job_2p_t& job, ecurve_t curve, key_share_2p_t& key, buf_t& sid);

  /**
   * @specs:
   * - ec-dkg-spec | EC-Refresh-2P
   */
  static error_t refresh(job_2p_t& job, const key_share_2p_t& key, key_share_2p_t& new_key);
};

struct key_share_mp_t {
  bn_t x_share;
  ecc_point_t Q;
  std::vector<crypto::ecc_point_t> Qis;
  ecurve_t curve;
  party_idx_t party_index;

  /**
   * @specs:
   * - ec-dkg-spec | EC-Refresh-MP
   */
  static error_t dkg(job_mp_t& job, ecurve_t curve, key_share_mp_t& key, buf_t& sid);

  /**
   * @specs:
   * - ec-dkg-spec | EC-Refresh-MP
   */
  static error_t refresh(job_mp_t& job, buf_t& sid, const key_share_mp_t& current_key, key_share_mp_t& new_key);

  error_t to_additive_share(const party_idx_t& party_new_index, const crypto::ss::ac_t ac, const int active_party_count,
                            const crypto::ss::party_map_t<party_idx_t>& name_to_idx, key_share_mp_t& additive_share);

 private:
  error_t reconstruct_additive_share(const mod_t& q, const crypto::ss::node_t* node,
                                     const crypto::ss::party_map_t<party_idx_t>& name_to_idx,
                                     bn_t& additive_share) const;
  error_t reconstruct_pub_additive_shares(const crypto::ss::node_t* node,
                                          const crypto::ss::party_map_t<party_idx_t>& name_to_idx, party_idx_t target,
                                          ecc_point_t& pub_additive_shares) const;
};

struct dkg_mp_threshold_t {
  /**
   * @specs:
   * - ec-dkg-spec | EC-DKG-Threshold-MP
   * @notes:
   * - This threshold DKG is not optimal in the sense that all n parties need to be connected
   * throughout, even though only t are active. In practice (and how we work in reality), it makes more sense for the t
   * parties to run the protocol, and then have the rest separately download the output message. This requires
   * additional infrastructure beyond what is in the scope of this library (like a PKI for the t parties to
   * encrypt-and-sign the output messages for the n parties), and therefore we implement this simpler demo DKG here.
   * In the future, we are planning on adding a VSS implementation that will make it easier to implement a threshold DKG
   * with only a subset of the parties online.
   */
  static error_t dkg(job_mp_t& job, const ecurve_t& curve, buf_t& sid, const crypto::ss::ac_t,
                     const party_set_t& quorum_party_set, key_share_mp_t& key);
  /**
   * @specs:
   * - ec-dkg-spec | EC-Refresh-Threshold-MP
   * @notes:
   * - See `dkg` for notes.
   */
  static error_t refresh(job_mp_t& job, const ecurve_t& curve, buf_t& sid, const crypto::ss::ac_t,
                         const party_set_t& quorum_party_set, key_share_mp_t& key, key_share_mp_t& new_key);

 private:
  static error_t dkg_or_refresh(job_mp_t& job, const ecurve_t& curve, buf_t& sid, const crypto::ss::ac_t,
                                const party_set_t& quorum_party_set, key_share_mp_t& key, key_share_mp_t& new_key,
                                bool is_refresh);
};

}  // namespace coinbase::mpc::eckey