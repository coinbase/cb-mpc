#include <cbmpc/core/precompiled.h>

#include "hd_keyset_eddsa_2p.h"

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/commitment.h>
#include <cbmpc/protocol/agree_random.h>
#include <cbmpc/protocol/sid.h>
#include <cbmpc/zk/zk_ec.h>
#include <cbmpc/zk/zk_paillier.h>

#include "ec_dkg.h"

using namespace coinbase;

namespace coinbase::mpc {

error_t key_share_eddsa_hdmpc_2p_t::dkg(job_2p_t& job, ecurve_t curve, key_share_eddsa_hdmpc_2p_t& key) {
  error_t rv = UNINITIALIZED_ERROR;

  key.curve = curve;
  const mod_t& q = curve.order();

  key.root.x_share = bn_t::rand(q);
  key.root.k_share = bn_t::rand(q);
  key.party_index = job.get_party_idx();

  eckey::dkg_2p_t x_dkg(curve, job.get_pid(party_t::p1)), k_dkg(curve, job.get_pid(party_t::p1));

  if (job.is_p1()) {
    x_dkg.step1_p1_to_p2(key.root.x_share);
    k_dkg.step1_p1_to_p2(key.root.k_share);
  }

  if (rv = job.p1_to_p2(x_dkg.msg1, k_dkg.msg1)) return rv;

  if (job.is_p2()) {
    x_dkg.step2_p2_to_p1(key.root.x_share);
    k_dkg.step2_p2_to_p1(key.root.k_share);
  }

  if (rv = job.p2_to_p1(x_dkg.msg2, k_dkg.msg2)) return rv;

  if (job.is_p1()) {
    if (rv = x_dkg.step3_p1_to_p2(key.root.Q)) return rv;
    if (rv = k_dkg.step3_p1_to_p2(key.root.K)) return rv;
  }

  if (rv = job.p1_to_p2(x_dkg.msg3, k_dkg.msg3)) return rv;

  if (job.is_p2()) {
    if (rv = x_dkg.step4_output_p2(key.root.Q)) return rv;
    if (rv = k_dkg.step4_output_p2(key.root.K)) return rv;
  }

  return SUCCESS;
}

error_t key_share_eddsa_hdmpc_2p_t::refresh(job_2p_t& job, key_share_eddsa_hdmpc_2p_t& key,
                                            key_share_eddsa_hdmpc_2p_t& new_key) {
  error_t rv = UNINITIALIZED_ERROR;
  new_key.party_index = key.party_index;
  new_key.curve = key.curve;
  new_key.root.Q = key.root.Q;
  new_key.root.K = key.root.K;

  const mod_t& q = key.curve.order();
  buf_t rand;
  int rand_bitlen = q.get_bits_count() + SEC_P_STAT;
  int rand_size = bits_to_bytes_floor(rand_bitlen);
  if (rv = agree_random(job, 2 * rand_bitlen, rand)) return rv;

  bn_t r_x = bn_t::from_bin(rand.take(rand_size)) % q;
  bn_t r_k = bn_t::from_bin(rand.skip(rand_size).take(rand_size)) % q;

  if (job.is_p1()) {
    MODULO(q) {
      new_key.root.x_share = key.root.x_share + r_x;
      new_key.root.k_share = key.root.k_share + r_k;
    }
  }

  if (job.is_p2()) {
    MODULO(q) {
      new_key.root.x_share = key.root.x_share - r_x;
      new_key.root.k_share = key.root.k_share - r_k;
    }
  }

  return SUCCESS;
}

error_t key_share_eddsa_hdmpc_2p_t::derive_keys(job_2p_t& job, const key_share_eddsa_hdmpc_2p_t& key,
                                                const bip32_path_t& hardened_path,
                                                const std::vector<bip32_path_t>& non_hardened_paths, buf_t& sid,
                                                std::vector<eddsa2pc::key_t>& derived_keys) {
  // The begining of this function is the same as its ecdsa counterpart.
  error_t rv = UNINITIALIZED_ERROR;

  if (sid.empty()) {
    if (rv = generate_sid_fixed_2p(job, party_t::p2, sid)) return rv;
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
  ecc_point_t Z;
  {
    crypto::vartime_scope_t vartime_scope;
    Z = Z1 + Z2;
  }
  // The rest of Hard-Derive-2P
  buf_t y = crypto::ro::hash_string(Z).bitlen(bytes_to_bits(delta_size) + 256);
  bn_t delta = bn_t::from_bin(y.take(delta_size)) % q;
  buf_t chain_code = y.skip(delta_size);

  int n_hd_paths = (int)non_hardened_paths.size();

  ecc_point_t delta_G = delta * G;
  ecc_point_t Q_derived;
  {
    crypto::vartime_scope_t vartime_scope;
    Q_derived = Q + delta_G;
  }
  std::vector<bn_t> non_hard_delta = non_hard_derive(Q_derived, chain_code, non_hardened_paths);
  std::vector<bn_t> derived_xs(n_hd_paths);
  std::vector<ecc_point_t> derived_Qs(n_hd_paths);

  // Start of the difference with the ecdsa-2pc version.
  for (int i = 0; i < n_hd_paths; i++) {
    derived_keys[i].role = party_t(key.party_index);
    derived_keys[i].curve = curve;
    derived_keys[i].Q = Q_derived + non_hard_delta[i] * G;
    if (job.get_party() == party_t::p1)
      MODULO(q) derived_keys[i].x_share = x_share + delta + non_hard_delta[i];
    else
      derived_keys[i].x_share = x_share;
  }

  return SUCCESS;
}

}  // namespace coinbase::mpc