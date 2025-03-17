#include "schnorr_mp.h"

#include <iostream>
#include <vector>

#include <cbmpc/crypto/base_ecc_secp256k1.h>
#include <cbmpc/crypto/ro.h>
#include <cbmpc/zk/zk_elgamal_com.h>
#include <cbmpc/zk/zk_pedersen.h>

#include "util.h"

#define _i msg
#define _j received(j)
#define _js all_received_refs()

using namespace coinbase::mpc;

namespace coinbase::mpc::schnorrmp {

static bn_t calc_eddsa_HRAM(const ecc_point_t& R, const ecc_point_t& Q, mem_t in) {
  buf_t HRAM_buf = crypto::sha512_t::hash(R, Q.to_compressed_bin(), in);
  bn_t HRAM = bn_t::from_bin(HRAM_buf.rev()) % crypto::curve_ed25519.order();
  return HRAM;
}

error_t sign(job_mp_t& job, key_t& key, const mem_t& msg, party_idx_t sig_receiver, buf_t& sig, variant_e variant) {
  error_t rv = UNINITIALIZED_ERROR;
  std::vector<mem_t> msgs(1, msg);
  std::vector<buf_t> sigs;
  if (rv = sign_batch(job, key, msgs, sig_receiver, sigs, variant)) return rv;
  sig = sigs[0];
  return SUCCESS;
}

error_t sign_batch(job_mp_t& job, key_t& key, const std::vector<mem_t>& msgs, party_idx_t sig_receiver,
                   std::vector<buf_t>& sigs, variant_e variant) {
  error_t rv = UNINITIALIZED_ERROR;

  int n = job.get_n_parties();
  int i = job.get_party_idx();
  sigs.resize(msgs.size());

  ecurve_t curve = key.curve;
  const mod_t& q = curve.order();
  const auto& G = curve.generator();

  if (key.party_index != i) return coinbase::error(E_BADARG, "Wrong role");
  if (key.Qis.size() != n) return coinbase::error(E_BADARG, "Wrong number of peers");
  if (key.x_share * G != key.Qis[i]) return coinbase::error(E_BADARG, "x_share does not match Qi");
  if (SUM(key.Qis) != key.Q) return coinbase::error(E_BADARG, "Q does not match the sum of Qis");
  auto h_consistency = job.uniform_msg<buf256_t>();
  h_consistency._i = crypto::sha256_t::hash(msgs, key.Q, key.Qis);

  auto sid_i = job.uniform_msg<buf_t>(crypto::gen_random_bitlen(SEC_P_COM));
  auto ki = job.uniform_msg<std::vector<bn_t>>(std::vector<bn_t>(msgs.size()));
  auto Ri = job.uniform_msg<std::vector<ecc_point_t>>(std::vector<ecc_point_t>(msgs.size()));
  for (int l = 0; l < msgs.size(); l++) {
    ki[l] = bn_t::rand(q);
    Ri[l] = ki[l] * G;
  }

  coinbase::crypto::commitment_t com(sid_i, job.get_pid(i));
  com.gen(Ri._i);
  auto c = job.uniform_msg<buf_t>(com.msg);
  auto rho = job.uniform_msg<buf256_t>(com.rand);

  if (rv = job.plain_broadcast(c, sid_i, h_consistency)) return rv;

  for (int j = 0; j < n; j++) {
    if (j == i) continue;
    if (h_consistency._j != h_consistency) return coinbase::error(E_CRYPTO);
  }
  auto sid = job.uniform_msg<buf_t>();
  sid._i = crypto::sha256_t::hash(sid_i._js);

  auto h = job.uniform_msg<buf256_t>();
  h._i = crypto::sha256_t::hash(c._js);
  auto pi = job.uniform_msg<zk::uc_batch_dl_t>();
  pi.prove(Ri, ki, sid, i);
  if (rv = job.plain_broadcast(sid, h, Ri, rho, pi)) return rv;

  for (int j = 0; j < n; j++) {
    if (job.is_party_idx(j)) continue;

    if (sid._j != sid._i) return coinbase::error(E_CRYPTO);
    if (h._j != h._i) return coinbase::error(E_CRYPTO);
    // Verification of Ri._j is done in the zk verify function
    if (rv = pi._j.verify(Ri._j, sid._i, j)) return coinbase::error(rv, "schnorr_mp_t::sign_batch: verify pi failed");

    if (rv = coinbase::crypto::commitment_t(sid_i._j, job.get_pid(j)).set(rho._j, c._j).open(Ri._j)) return rv;
  }

  std::vector<ecc_point_t> R(msgs.size());
  for (int l = 0; l < msgs.size(); l++) {
    R[l] = Ri._i[l];
    for (int j = 0; j < n; j++) {
      if (j == i) continue;
      R[l] += Ri._j[l];
    }
  }

  std::vector<bn_t> e(msgs.size());

  if (variant == variant_e::EdDSA) {
    if (key.curve != crypto::curve_ed25519) return coinbase::error(E_BADARG, "EdDSA variant requires EdDSA curve");
    for (int l = 0; l < msgs.size(); l++) {
      e[l] = calc_eddsa_HRAM(R[l], key.Q, msgs[l]);
    }
  } else if (variant == variant_e::BIP340) {
    if (key.curve != crypto::curve_secp256k1)
      return coinbase::error(E_BADARG, "BIP340 variant requires secp256k1 curve");
    bn_t rx, ry;
    for (int l = 0; l < msgs.size(); l++) {
      R[l].get_coordinates(rx, ry);
      if (ry.is_odd()) ki[l] = q - ki[l];
      e[l] = crypto::bip340::hash_message(rx, key.Q, msgs[l]);
      if (key.Q.get_y().is_odd()) e[l] = q - e[l];
    }
  } else {
    cb_assert(false && "schnorr_mp: non-existing variant");
  }

  auto ssi = job.uniform_msg<std::vector<bn_t>>();
  ssi.msg.resize(msgs.size());
  for (int l = 0; l < msgs.size(); l++) {
    ssi.msg[l] = (e[l] * key.x_share + ki[l]) % q;
  }

  if (rv = job.send_message_all_to_one(sig_receiver, ssi)) return rv;

  if (job.is_party_idx(sig_receiver)) {
    std::vector<bn_t> ss(msgs.size(), 0);
    for (int j = 0; j < n; j++) {
      for (int l = 0; l < msgs.size(); l++) MODULO(q) ss[l] += ssi._j[l];
    }

    crypto::ecc_pub_key_t verify_key(key.Q);
    if (key.curve == crypto::curve_ed25519) {
      for (int l = 0; l < msgs.size(); l++) {
        sigs[l] = R[l].to_compressed_bin() + ss[l].to_bin(crypto::ed25519::prv_bin_size()).rev();
        if (rv = verify_key.verify(msgs[l], sigs[l])) return coinbase::error(rv, "ed25519 verify failed");
      }
    } else if (key.curve == crypto::curve_secp256k1) {
      bn_t rx, ry;
      for (int l = 0; l < msgs.size(); l++) {
        R[l].get_coordinates(rx, ry);
        sigs[l] = rx.to_bin(32) + ss[l].to_bin(32);
        if (rv = crypto::bip340::verify(verify_key, msgs[l], sigs[l])) {
          return coinbase::error(rv, "bip340 verify failed");
        }
      }
    } else {
      cb_assert(false && "schnorr_mp: non-existing variant");
    }
  }
  return SUCCESS;
}

}  // namespace coinbase::mpc::schnorrmp
