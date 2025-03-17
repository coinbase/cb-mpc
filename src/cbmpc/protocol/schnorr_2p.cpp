#include "schnorr_2p.h"

#include <cbmpc/crypto/base_ecc_secp256k1.h>
#include <cbmpc/protocol/agree_random.h>
#include <cbmpc/protocol/ec_dkg.h>

#include "util.h"

namespace coinbase::mpc::schnorr2p {

error_t sign(job_2p_t& job, key_t& key, const mem_t& msg, buf_t& sig, variant_e variant) {
  error_t rv = UNINITIALIZED_ERROR;
  std::vector<mem_t> msgs(1, msg);
  std::vector<buf_t> sigs;
  if (rv = sign_batch(job, key, msgs, sigs, variant)) return rv;
  sig = sigs[0];
  return SUCCESS;
}

error_t sign_batch(job_2p_t& job, key_t& key, const std::vector<mem_t>& msgs, std::vector<buf_t>& sigs,
                   variant_e variant) {
  int n_sigs = msgs.size();
  sigs.resize(n_sigs);

  error_t rv = UNINITIALIZED_ERROR;
  ecurve_t curve = key.curve;

  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  std::vector<bn_t> k1(n_sigs), k2(n_sigs);
  std::vector<ecc_point_t> R1(n_sigs), R2(n_sigs);
  buf_t sid1, sid2, sid;
  coinbase::crypto::commitment_t com;
  zk::uc_batch_dl_t zk_dl1, zk_dl2;

  if (job.is_p1()) {
    sid1 = crypto::gen_random_bitlen(SEC_P_COM);
    for (int i = 0; i < n_sigs; i++) k1[i] = bn_t::rand(q);
    for (int i = 0; i < n_sigs; i++) R1[i] = k1[i] * G;
    com.id(sid1, job.get_pid(party_t::p1)).gen(R1);
  }
  if (rv = job.p1_to_p2(sid1, com.msg)) return rv;

  if (job.is_p2()) {
    sid2 = crypto::gen_random_bitlen(SEC_P_COM);
    for (int i = 0; i < n_sigs; i++) k2[i] = bn_t::rand(q);
    for (int i = 0; i < n_sigs; i++) R2[i] = k2[i] * G;
    sid = crypto::sha256_t::hash(sid1, sid2);
    zk_dl2.prove(R2, k2, sid, 2);
  }
  if (rv = job.p2_to_p1(R2, zk_dl2, sid2)) return rv;

  if (job.is_p1()) {
    // point checks are covered by the zk proof
    sid = crypto::sha256_t::hash(sid1, sid2);
    if (rv = zk_dl2.verify(R2, sid, 2)) return rv;
    zk_dl1.prove(R1, k1, sid, 1);
  }
  if (rv = job.p1_to_p2(zk_dl1, R1, com.rand)) return rv;

  if (job.is_p2()) {
    // point checks are covered by the zk proof
    if (rv = com.id(sid1, job.get_pid(party_t::p1)).open(R1)) return rv;
    if (rv = zk_dl1.verify(R1, sid, 1)) return rv;
  }

  std::vector<ecc_point_t> R(n_sigs);
  for (int i = 0; i < n_sigs; i++) R[i] = R1[i] + R2[i];

  std::vector<bn_t> e(n_sigs);
  if (variant == variant_e::BIP340) {
    if (curve != crypto::curve_secp256k1) return coinbase::error(E_BADARG, "BIP340 variant requires secp256k1 curve");
    for (int i = 0; i < n_sigs; i++) {
      bn_t rx, ry;
      R[i].get_coordinates(rx, ry);
      if (ry.is_odd()) {
        k1[i] = q - k1[i];
        k2[i] = q - k2[i];
      }
      e[i] = crypto::bip340::hash_message(rx, key.Q, msgs[i]);
      if (key.Q.get_y().is_odd()) e[i] = q - e[i];
    }
  } else if (variant == variant_e::EdDSA) {
    if (curve != crypto::curve_ed25519) return coinbase::error(E_BADARG, "EdDSA variant requires ed25519 curve");
    std::vector<buf_t> e_buf(n_sigs);
    for (int i = 0; i < n_sigs; i++) {
      e_buf[i] = crypto::sha512_t::hash(R[i], key.Q.to_compressed_bin(), msgs[i]);
      e[i] = bn_t::from_bin(e_buf[i].rev()) % q;
    }
  } else {
    cb_assert(false && "schnorr_2p: non-existing variant");
  }

  std::vector<bn_t> s2(n_sigs);
  if (job.is_p2()) {
    for (int i = 0; i < n_sigs; i++) {
      MODULO(q) s2[i] = e[i] * key.x_share + k2[i];
    }
  }

  if (rv = job.p2_to_p1(s2)) return rv;

  if (job.is_p1()) {
    for (int i = 0; i < n_sigs; i++) {
      bn_t s, s1;
      MODULO(q) {
        s1 = e[i] * key.x_share + k1[i];
        s = s1 + s2[i];
      }

      if (variant == variant_e::EdDSA) {
        sigs[i] = R[i].to_compressed_bin() + s.to_bin(crypto::ed25519::prv_bin_size()).rev();
        crypto::ecc_pub_key_t pub_key(key.Q);
        if (rv = pub_key.verify(msgs[i], sigs[i])) return coinbase::error(rv, "schnorr_2p: eddsa verify failed");
      } else if (variant == variant_e::BIP340) {
        bn_t rx, ry;
        R[i].get_coordinates(rx, ry);
        sigs[i] = rx.to_bin(32) + s.to_bin(32);
        crypto::ecc_pub_key_t pub_key(key.Q);
        if (rv = crypto::bip340::verify(pub_key, msgs[i], sigs[i]))
          return coinbase::error(rv, "schnorr_2p: secp256k1 verify failed");
      } else {
        cb_assert(false && "schnorr_2p: non-existing variant");
      }
    }
  }
  return SUCCESS;
}

}  // namespace coinbase::mpc::schnorr2p