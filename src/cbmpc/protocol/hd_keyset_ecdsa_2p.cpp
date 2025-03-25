#include <cbmpc/core/precompiled.h>

#include "hd_keyset_ecdsa_2p.h"

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/commitment.h>
#include <cbmpc/protocol/sid.h>
#include <cbmpc/zk/zk_ec.h>
#include <cbmpc/zk/zk_paillier.h>

#include "ec_dkg.h"

using namespace coinbase;

namespace coinbase::mpc {

error_t key_share_ecdsa_hdmpc_2p_t::dkg(job_2p_t& job, ecurve_t curve, key_share_ecdsa_hdmpc_2p_t& key) {
  error_t rv = UNINITIALIZED_ERROR;

  key.curve = curve;
  const mod_t& q = curve.order();

  key.root.x_share = bn_t::rand(q);
  key.root.k_share = bn_t::rand(q);
  key.party_index = job.get_party_idx();

  const crypto::mpc_pid_t& p1_pid = job.get_pid(party_t::p1);
  ecdsa2pc::paillier_gen_interactive_t pg(p1_pid);
  eckey::dkg_2p_t x_dkg(curve, p1_pid), k_dkg(curve, p1_pid);

  if (job.is_p1()) {
    x_dkg.step1_p1_to_p2(key.root.x_share);
    k_dkg.step1_p1_to_p2(key.root.k_share);
    pg.step1_p1_to_p2(key.paillier, key.root.x_share, x_dkg.curve.order(), key.c_key);
  }

  if (rv = job.p1_to_p2(x_dkg.msg1, k_dkg.msg1, pg.msg1)) return rv;

  if (job.is_p2()) {
    x_dkg.step2_p2_to_p1(key.root.x_share);
    k_dkg.step2_p2_to_p1(key.root.k_share);
    pg.step2_p2_to_p1();
  }

  if (rv = job.p2_to_p1(x_dkg.msg2, k_dkg.msg2, pg.msg2)) return rv;

  if (job.is_p1()) {
    if (rv = x_dkg.step3_p1_to_p2(key.root.Q)) return rv;
    if (rv = k_dkg.step3_p1_to_p2(key.root.K)) return rv;
    pg.step3_p1_to_p2(key.paillier, key.root.x_share, x_dkg.Q1, job.get_pid(party_t::p1), x_dkg.sid);
  }

  if (rv = job.p1_to_p2(x_dkg.msg3, k_dkg.msg3, pg.msg3)) return rv;

  if (job.is_p2()) {
    if (rv = x_dkg.step4_output_p2(key.root.Q)) return rv;
    if (rv = k_dkg.step4_output_p2(key.root.K)) return rv;
    key.c_key = pg.c_key;
    if (rv = pg.step4_p2_output(key.paillier, x_dkg.Q1, key.c_key, job.get_pid(party_t::p1), x_dkg.sid)) return rv;
  }

  return SUCCESS;
}

error_t key_share_ecdsa_hdmpc_2p_t::refresh(job_2p_t& job, key_share_ecdsa_hdmpc_2p_t& key,
                                            key_share_ecdsa_hdmpc_2p_t& new_key) {
  // NOTE: this is not an optimized version of the refresh function and has twice as many rounds as needed since
  //       the refresh operations are performed sequentially. It can be made more optimized by copy-pasting the
  //       two refresh code and interleaving the operations.
  //       We are using this less optimized version for simplicity.
  error_t rv = UNINITIALIZED_ERROR;
  ecdsa2pc::key_t ecdsa_key =
      ecdsa2pc::key_t{party_t(key.party_index), key.curve, key.root.Q, key.root.x_share, key.c_key, key.paillier};
  eckey::key_share_2p_t root_key =
      eckey::key_share_2p_t{party_t(key.party_index), key.curve, key.root.K, key.root.k_share};

  ecdsa2pc::key_t new_ecdsa_key;
  eckey::key_share_2p_t new_root_key;

  if (rv = ecdsa2pc::refresh(job, ecdsa_key, new_ecdsa_key)) return rv;
  if (rv = root_key.refresh(job, root_key, new_root_key)) return rv;

  new_key.party_index = key.party_index;
  new_key.curve = key.curve;

  new_key.root.Q = new_ecdsa_key.Q;
  new_key.root.x_share = new_ecdsa_key.x_share;
  new_key.c_key = new_ecdsa_key.c_key;
  new_key.paillier = new_ecdsa_key.paillier;

  new_key.root.k_share = new_root_key.x_share;
  new_key.root.K = new_root_key.Q;

  return SUCCESS;
}

error_t key_share_ecdsa_hdmpc_2p_t::derive_keys(job_2p_t& job, const key_share_ecdsa_hdmpc_2p_t& key,
                                                const bip32_path_t& hardened_path,
                                                const std::vector<bip32_path_t>& non_hardened_paths, buf_t& sid,
                                                std::vector<ecdsa2pc::key_t>& derived_keys) {
  // The begining of this function is the same as its eddsa counterpart.
  error_t rv = UNINITIALIZED_ERROR;

  if (sid.empty()) {
    if (rv = generate_sid_fixed_mp(job, sid)) return rv;
  }

  ecurve_t curve = key.curve;
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  bn_t x_share = key.root.x_share;
  bn_t k_share = key.root.k_share;
  ecc_point_t K_share = key.root.K_share();
  ecc_point_t other_K_share = key.root.other_K_share();
  ecc_point_t Q = key.root.Q;

  // This is VRF-Compute-2P in the spec
  const int delta_size = curve.size() + 16;  // 256 + 128 bits
  ecc_point_t Z1, Z2;
  ecc_point_t P = crypto::ro::hash_curve(hardened_path.get()).curve(curve);
  ecc_point_t Z_share = k_share * P;
  if (job.is_p1())
    Z1 = Z_share;
  else
    Z2 = Z_share;
  zk::dh_t zk_dh1, zk_dh2;

  if (job.is_p1()) {
    zk_dh1.prove(P, K_share, Z1, k_share, sid, 1);
  }

  if (rv = job.p1_to_p2(Z1, zk_dh1)) return rv;

  if (job.is_p2()) {
    // Verification that Z1 is valid is done in the verify function
    if (rv = zk_dh1.verify(P, other_K_share, Z1, sid, 1)) return rv;
    zk_dh2.prove(P, K_share, Z2, k_share, sid, 2);
  }

  if (rv = job.p2_to_p1(Z2, zk_dh2)) return rv;

  if (job.is_p1()) {
    if (rv = zk_dh2.verify(P, other_K_share, Z2, sid, 2)) return rv;
  }
  ecc_point_t Z = Z1 + Z2;
  // The rest of Hard-Derive-2P
  // The rest of Hard-Derive-2P
  buf_t y = crypto::ro::hash_string(Z).bitlen(bytes_to_bits(delta_size) + 256);
  bn_t delta = bn_t::from_bin(y.take(delta_size)) % q;
  buf_t chain_code = y.skip(delta_size);

  int n_hd_paths = (int)non_hardened_paths.size();

  ecc_point_t delta_G = delta * G;
  ecc_point_t Q_derived = Q + delta_G;
  std::vector<bn_t> non_hard_delta = non_hard_derive(Q_derived, chain_code, non_hardened_paths);
  std::vector<bn_t> derived_xs(n_hd_paths);
  std::vector<ecc_point_t> derived_Qs(n_hd_paths);

  // Start of the difference with the eddsa-2pc version.
  for (int i = 0; i < n_hd_paths; i++) {
    derived_keys[i].curve = curve;
    derived_keys[i].paillier = coinbase::crypto::paillier_t(key.paillier);
    derived_keys[i].role = party_t(key.party_index);
    derived_keys[i].c_key = bn_t(key.c_key);

    ecc_point_t non_hard_delta_G = non_hard_delta[i] * G;
    derived_keys[i].Q = Q_derived + non_hard_delta_G;
    if (job.get_party() == party_t::p2)
      MODULO(q) derived_keys[i].x_share = x_share + delta + non_hard_delta[i];
    else
      derived_keys[i].x_share = x_share;
  }

  return SUCCESS;
}

}  // namespace coinbase::mpc