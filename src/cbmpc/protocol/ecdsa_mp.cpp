#include "ecdsa_mp.h"

#include <cbmpc/crypto/ro.h>
#include <cbmpc/protocol/ot.h>
#include <cbmpc/protocol/sid.h>
#include <cbmpc/zk/zk_elgamal_com.h>
#include <cbmpc/zk/zk_pedersen.h>

#include "util.h"

using namespace coinbase::mpc;

namespace coinbase::mpc::ecdsampc {

// These macros help with the readability of the code to make it easier to match the spec and the code
#define _ij msgs[j]
#define _i msg
#define _j received(j)
#define _js all_received_refs()

error_t dkg(job_mp_t& job, ecurve_t curve, key_t& key, buf_t& sid) {
  return eckey::key_share_mp_t::dkg(job, curve, key, sid);
}

error_t refresh(job_mp_t& job, buf_t& sid, key_t& key, key_t& new_key) {
  return eckey::key_share_mp_t::refresh(job, sid, key, new_key);
}

error_t sign(job_mp_t& job, key_t& key, mem_t msg, const party_idx_t sig_receiver,
             const std::vector<std::vector<int>>& ot_role_map, buf_t& sig) {
  error_t rv = UNINITIALIZED_ERROR;

  int peers_count = job.get_n_parties();
  int peer_index = job.get_party_idx();
  int i = peer_index;
  int n = peers_count;
  auto sid_i = job.uniform_msg<buf_t>(crypto::gen_random_bitlen(SEC_P_COM));

  ecurve_t curve = key.curve;
  const mod_t& q = curve.order();
  const auto& G = curve.generator();
  int theta = q.get_bits_count() + kappa;

  if (key.x_share * G != key.Qis[i]) return coinbase::error(E_BADARG, "x_share does not match Qi");
  if (SUM(key.Qis) != key.Q) return coinbase::error(E_BADARG, "Q does not match the sum of Qis");
  auto h_consistency = job.uniform_msg<buf256_t>();
  h_consistency._i = crypto::sha256_t::hash(msg, key.Q, key.Qis);

  // --------------------- Start of the 1st round of Pre-message section

  // s_i and Ei are related to 1st round of the ec-dkg for ElGamal commitment key.
  bn_t s_i = bn_t::rand(q);
  auto Ei_gen = job.uniform_msg<ecc_point_t>(s_i * G);
  coinbase::crypto::commitment_t com(sid_i.msg, job.get_pid(i));
  com.gen(Ei_gen.msg, peer_index);
  auto c = job.uniform_msg<buf_t>(com.msg);

  if (rv = job.plain_broadcast(sid_i, c, h_consistency)) return rv;

  // ---------------------- Start of 2nd round of Pre-message section and first round of the signing protocol
  // ---------------------- and the 2nd round of DKG and completion of the input consistency check

  for (int j = 0; j < n; j++) {
    if (j == i) continue;
    if (h_consistency._j != h_consistency) return coinbase::error(E_CRYPTO);
  }

  // This is added to adhere to `GenerateSID-Dynamic-MP` api in the spec
  // This overlaps with the computation of sid for ec-dkg as well
  auto pids = job.get_pids();
  std::sort(pids.begin(), pids.end());
  buf_t sid = crypto::sha256_t::hash(sid_i._js, pids);

  // This is for the 2nd round of dkg for the ElGamal commitment key
  auto h_gen = job.uniform_msg<buf256_t>(crypto::sha256_t::hash(c._js));
  auto rho = job.uniform_msg<buf256_t>(com.rand);
  auto pi_s = job.uniform_msg<zk::uc_dl_t>();
  pi_s.prove(Ei_gen, s_i, sid, peers_count + peer_index);

  // Proceed with the signing protocol
  std::vector<mpc::ot_protocol_pvw_ctx_t> ot(n, mpc::ot_protocol_pvw_ctx_t(curve));
  std::vector<coinbase::bits_t> R_bits_i(n);
  std::vector<std::array<bool, 4>> R[theta];
  for (int l = 0; l < theta; l++) R[l].resize(n);

  // This is step 5.(a): generating the pairwise sids for OTs
  for (int j = 0; j < n; j++) {
    int rid_s, rid_r;
    if (ot_role_map[i][j] == ot_sender) {
      rid_s = i;
      rid_r = j;
    } else {
      rid_s = j;
      rid_r = i;
    }
    ot[j].base.sid = crypto::sha256_t::hash(sid, rid_s, rid_r);
  }

  for (int j = 0; j < n; j++) {
    if (ot_role_map[i][j] != ot_sender) continue;
    if (rv = ot[j].step1_S2R()) return rv;
  }

  party_set_t ot_senders = ot_senders_for(i, n, ot_role_map);
  party_set_t ot_receivers = ot_receivers_for(i, n, ot_role_map);

  auto ot_msg1 = job.inplace_msg<mpc::ot_protocol_pvw_ctx_t::msg1_t>([&ot](int j) -> auto{ return ot[j].msg1(); });
  if (rv = plain_broadcast_and_pairwise_message(job, ot_receivers, ot_msg1, h_gen, Ei_gen, rho, pi_s)) return rv;

  // ---------------------- Start of the 2nd round of the signing protocol

  // Output generation from DKG for the ElGamal commitment key
  for (int j = 0; j < n; j++) {
    if (i == j) continue;
    if (h_gen._j != h_gen.msg) return coinbase::error(E_CRYPTO);
    if (rv = coinbase::crypto::commitment_t(sid_i._j, job.get_pid(j)).set(rho._j, c._j).open(Ei_gen._j, j)) return rv;
    // Verifying that Ei_gen values are valid is done in the following verification function
    if (rv = pi_s._j.verify(Ei_gen._j, sid, peers_count + j)) return rv;
  }
  const std::vector<ecc_point_t>& E_i = Ei_gen.all_received_values();
  ecc_point_t E = SUM(E_i);

  // Proceed with the signing protocol
  for (int j = 0; j < n; j++) {
    if (ot_role_map[i][j] != ot_receiver) continue;
    R_bits_i[j] = crypto::gen_random_bits(4 * theta);
    for (int l = 0; l < theta; l++) {
      for (int t = 0; t < 4; t++) R[l][j][t] = R_bits_i[j][l * 4 + t];
    }
  }

  for (int j = 0; j < n; j++) {
    if (ot_role_map[i][j] != ot_receiver) continue;
    if (rv = ot[j].step2_R2S(R_bits_i[j], q.get_bits_count())) return rv;
  }

  auto ot_msg2 = job.inplace_msg<mpc::ot_protocol_pvw_ctx_t::msg2_t>([&ot](int j) -> auto{ return ot[j].msg2(); });
  if (rv = plain_broadcast_and_pairwise_message(job, ot_senders, ot_msg2)) return rv;

  // ---------------------- Start of the 3rd round of the signing protocol
  bn_t k_i = bn_t::rand(q);
  bn_t rho_i = bn_t::rand(q);
  bn_t r_eK_i = bn_t::rand(q);
  bn_t r_eRHO_i = bn_t::rand(q);

  auto eK_i = job.uniform_msg<elg_com_t>(elg_com_t::commit(E, k_i).rand(r_eK_i));
  auto eRHO_i = job.uniform_msg<elg_com_t>(elg_com_t::commit(E, rho_i).rand(r_eRHO_i));

  // The steps in Message 3 (continued)
  const int n_uc_elgamal_com_proofs = 4;
  auto pi_eK = job.uniform_msg<zk::uc_elgamal_com_t>();
  auto pi_eRHO = job.uniform_msg<zk::uc_elgamal_com_t>();
  pi_eK._i.prove(E, eK_i, k_i, r_eK_i, sid, n_uc_elgamal_com_proofs * i + 0);
  pi_eRHO._i.prove(E, eRHO_i, rho_i, r_eRHO_i, sid, n_uc_elgamal_com_proofs * i + 1);

  bn_t x_i = key.x_share;

  // The other steps related to OT
  std::vector<std::array<bn_t, 4>> delta[theta];
  for (int l = 0; l < theta; l++) delta[l].resize(n);
  for (int j = 0; j < n; j++) {
    if (ot_role_map[i][j] != ot_sender) continue;

    bn_t a[] = {rho_i, k_i, rho_i, x_i};

    std::vector<bn_t> D(4 * theta);
    bn_t Delta[4];
    for (int t = 0; t < 4; t++) MODULO(q) Delta[t] = a[t] + a[t];

    for (int l = 0; l < theta; l++)
      for (int t = 0; t < 4; t++) D[l * 4 + t] = Delta[t];

    std::vector<bn_t> X0, _X1;
    if (rv = ot[j].step3_S2R(D, q, X0, _X1)) return rv;

    for (int l = 0; l < theta; l++)
      for (int t = 0; t < 4; t++) MODULO(q) delta[l][j][t] = X0[l * 4 + t] + a[t];
  }

  auto ot_msg3 =
      job.inplace_msg<mpc::ot_protocol_pvw_ctx_t::msg3_delta_t>([&ot](int j) -> auto{ return ot[j].msg3_delta(); });
  if (rv = plain_broadcast_and_pairwise_message(job, ot_receivers, ot_msg3, eK_i, eRHO_i, pi_eK, pi_eRHO)) return rv;

  // ---------------------- Start of the 4th round of the signing protocol

  // Generate OT output
  std::vector<std::array<bn_t, 4>> X[theta];
  for (int l = 0; l < theta; l++) X[l].resize(n);
  for (int j = 0; j < n; j++) {
    if (ot_role_map[i][j] != ot_receiver) continue;

    std::vector<buf_t> X_bin;
    if (rv = ot[j].output_R(4 * theta, X_bin)) return rv;

    for (int l = 0; l < theta; l++)
      for (int t = 0; t < 4; t++) X[l][j][t] = bn_t::from_bin(X_bin[l * 4 + t]);
  }

  // Initialize the view
  crypto::sha256_t view;
  view.update(E_i, eK_i._js, eRHO_i._js, pi_eK._js, pi_eRHO._js);

  // Proceed with message 4 of the signing protocol
  for (int j = 0; j < n; j++) {
    if (i == j) continue;
    // The check for validating eK_i and eRHO_i is done in the verify function
    if (rv = pi_eK._j.verify(E, eK_i._j, sid, n_uc_elgamal_com_proofs * j + 0)) return rv;
    if (rv = pi_eRHO._j.verify(E, eRHO_i._j, sid, n_uc_elgamal_com_proofs * j + 1)) return rv;
  }

  auto seed = job.nonuniform_msg<buf256_t>();
  auto v_theta = job.nonuniform_msg<std::array<bn_t, 4>>();

  std::vector<std::array<bn_t, 4>> s[2];
  s[ot_sender].resize(n);
  s[ot_receiver].resize(n);

  for (int j = 0; j < n; j++) {
    if (ot_role_map[i][j] != ot_receiver) continue;
    crypto::gen_random(seed._ij);
    crypto::drbg_aes_ctr_t drbg(seed._ij);

    bn_t a[] = {k_i, rho_i, x_i, rho_i};

    std::array<bn_t, 4> v[theta];
    for (int t = 0; t < 4; t++) {
      for (int l = 0; l < theta - 1; l++) v[l][t] = drbg.gen_bn(q);

      bn_t temp = 0;
      MODULO(q) {
        for (int l = 0; l < theta - 1; l++) {
          if (R[l][j][t])
            temp += v[l][t];
          else
            temp -= v[l][t];
        }
      }
      MODULO(q) v[theta - 1][t] = R[theta - 1][j][t] ? a[t] - temp : temp - a[t];

      bn_t sigma = drbg.gen_bn(q);
      bn_t sum = 0;
      MODULO(q) {
        for (int l = 0; l < theta; l++) sum += v[l][t] * X[l][j][t];
        s[ot_receiver][j][t] = sigma + sum;
      }
    }
    v_theta._ij = v[theta - 1];
  }

  auto ot_part = job_mp_t::tie_msgs(seed, v_theta);
  if (rv = plain_broadcast_and_pairwise_message(job, ot_senders, ot_part)) return rv;

  // ---------------------- Start of the 5th round of the signing protocol
  for (int j = 0; j < n; j++) {
    if (ot_role_map[i][j] != ot_sender) continue;

    bn_t a[4] = {k_i, rho_i, x_i, rho_i};

    std::array<bn_t, 4> v[theta];
    v[theta - 1] = v_theta._j;
    crypto::drbg_aes_ctr_t drbg(seed._j);

    for (int t = 0; t < 4; t++) {
      for (int l = 0; l < theta - 1; l++) v[l][t] = drbg.gen_bn(q);
      bn_t sigma = drbg.gen_bn(q);

      bn_t sum = 0;
      MODULO(q) {
        for (int l = 0; l < theta; l++) sum -= v[l][t] * delta[l][j][t];
        s[ot_sender][j][t] = sum - sigma;
      }
    }
  }

  bn_t rho_k_i;
  bn_t rho_x_i;
  MODULO(q) {
    rho_k_i = rho_i * k_i + SUM<bn_t>(n, [=](bn_t& sum, int j) {
                if (i == j) return;
                int role = ot_role_map[i][j];
                sum += s[role][j][0] + s[role][j][1];
              });
    rho_x_i = rho_i * x_i + SUM<bn_t>(n, [=](bn_t& sum, int j) {
                if (i == j) return;
                int role = ot_role_map[i][j];
                sum += s[role][j][2] + s[role][j][3];
              });
  }

  bn_t r_eRHO_K = bn_t::rand(q);
  bn_t r_eRHO_X = bn_t::rand(q);

  auto eRHO_K = job.uniform_msg<elg_com_t>(elg_com_t::commit(E, rho_k_i).rand(r_eRHO_K));
  auto eRHO_X = job.uniform_msg<elg_com_t>(elg_com_t::commit(E, rho_x_i).rand(r_eRHO_X));
  auto pi_eRHO_K = job.uniform_msg<zk::uc_elgamal_com_t>();
  auto pi_eRHO_X = job.uniform_msg<zk::uc_elgamal_com_t>();
  pi_eRHO_K.prove(E, eRHO_K, rho_k_i, r_eRHO_K, sid, n_uc_elgamal_com_proofs * i + 2);
  pi_eRHO_X.prove(E, eRHO_X, rho_x_i, r_eRHO_X, sid, n_uc_elgamal_com_proofs * i + 3);

  elg_com_t eK = SUM(eK_i._js);
  elg_com_t eX = elg_com_t(G, E + key.Q);

  bn_t r_F_eRHO_K = bn_t::rand(q);
  bn_t r_F_eRHO_X = bn_t::rand(q);
  auto F_eRHO_K = job.uniform_msg<elg_com_t>(elg_com_t::rerand(E, rho_i * eK).rand(r_F_eRHO_K));
  auto F_eRHO_X = job.uniform_msg<elg_com_t>(elg_com_t::rerand(E, rho_i * eX).rand(r_F_eRHO_X));

  int n_elgamal_com_mult_proofs = 2;
  auto pi_F_eRHO_K = job.uniform_msg<zk::elgamal_com_mult_t>();
  auto pi_F_eRHO_X = job.uniform_msg<zk::elgamal_com_mult_t>();
  pi_F_eRHO_K.prove(E, eK, eRHO_i, F_eRHO_K, r_eRHO_i, r_F_eRHO_K, rho_i, sid, n_elgamal_com_mult_proofs * i + 0);
  pi_F_eRHO_X.prove(E, eX, eRHO_i, F_eRHO_X, r_eRHO_i, r_F_eRHO_X, rho_i, sid, n_elgamal_com_mult_proofs * i + 1);

  if (rv = job.plain_broadcast(eRHO_K, pi_eRHO_K, eRHO_X, pi_eRHO_X, F_eRHO_K, pi_F_eRHO_K, F_eRHO_X, pi_F_eRHO_X))
    return rv;

  // ---------------------- Start of the 6th round of the signing protocol
  view.update(eRHO_K._js, pi_eRHO_K._js, eRHO_X._js, pi_eRHO_X._js, F_eRHO_K._js, pi_F_eRHO_K._js, F_eRHO_X._js,
              pi_F_eRHO_X._js);
  auto h = job.uniform_msg<buf256_t>(view.final());

  for (int j = 0; j < n; j++) {
    if (i == j) continue;
    // Curve checks are done inside the verify function
    if (rv = pi_F_eRHO_K._j.verify(E, eK, eRHO_i._j, F_eRHO_K._j, sid, n_elgamal_com_mult_proofs * j + 0)) return rv;
    if (rv = pi_F_eRHO_X._j.verify(E, eX, eRHO_i._j, F_eRHO_X._j, sid, n_elgamal_com_mult_proofs * j + 1)) return rv;
    if (rv = pi_eRHO_K._j.verify(E, eRHO_K._j, sid, n_uc_elgamal_com_proofs * j + 2)) return rv;
    if (rv = pi_eRHO_X._j.verify(E, eRHO_X._j, sid, n_uc_elgamal_com_proofs * j + 3)) return rv;
  }

  elg_com_t Y_eRHO_K = SUM(F_eRHO_K._js) - SUM(eRHO_K._js);
  elg_com_t Y_eRHO_X = SUM(F_eRHO_X._js) - SUM(eRHO_X._js);

  bn_t r_Z_eRHO_K = bn_t::rand(q);
  bn_t r_Z_eRHO_X = bn_t::rand(q);
  bn_t o_Z_eRHO_K = bn_t::rand(q);
  bn_t o_Z_eRHO_X = bn_t::rand(q);

  auto Z_eRHO_K_i = job.uniform_msg<elg_com_t>(elg_com_t::rerand(E, o_Z_eRHO_K * Y_eRHO_K).rand(r_Z_eRHO_K));
  auto Z_eRHO_X_i = job.uniform_msg<elg_com_t>(elg_com_t::rerand(E, o_Z_eRHO_X * Y_eRHO_X).rand(r_Z_eRHO_X));

  int n_elgamal_mult_private_scalar_proofs = 2;
  auto pi_Z_eRHO_K = job.uniform_msg<zk::uc_elgamal_com_mult_private_scalar_t>();
  auto pi_Z_eRHO_X = job.uniform_msg<zk::uc_elgamal_com_mult_private_scalar_t>();
  pi_Z_eRHO_K.prove(E, Y_eRHO_K, Z_eRHO_K_i, r_Z_eRHO_K, o_Z_eRHO_K, sid, n_elgamal_mult_private_scalar_proofs * i + 0);
  pi_Z_eRHO_X.prove(E, Y_eRHO_X, Z_eRHO_X_i, r_Z_eRHO_X, o_Z_eRHO_X, sid, n_elgamal_mult_private_scalar_proofs * i + 1);

  if (rv = job.plain_broadcast(h, Z_eRHO_K_i, pi_Z_eRHO_K, Z_eRHO_X_i, pi_Z_eRHO_X)) return rv;

  // ---------------------- Start of the 7th round of the signing protocol
  for (int j = 0; j < n; j++) {
    if (i == j) continue;
    if (h != h._j) return coinbase::error(E_CRYPTO);

    if (rv = pi_Z_eRHO_K._j.verify(E, Y_eRHO_K, Z_eRHO_K_i._j, sid, n_elgamal_mult_private_scalar_proofs * j + 0))
      return rv;
    if (rv = pi_Z_eRHO_X._j.verify(E, Y_eRHO_X, Z_eRHO_X_i._j, sid, n_elgamal_mult_private_scalar_proofs * j + 1))
      return rv;
  }

  auto h2 = job.uniform_msg<buf256_t>(
      crypto::sha256_t::hash(Z_eRHO_K_i._js, pi_Z_eRHO_K._js, Z_eRHO_X_i._js, pi_Z_eRHO_X._js, h.msg));

  elg_com_t Z_eRHO_K = SUM(Z_eRHO_K_i._js);
  elg_com_t Z_eRHO_X = SUM(Z_eRHO_X_i._js);

  auto W_eRHO_K_i = job.uniform_msg<ecc_point_t>(s_i * Z_eRHO_K.L);
  auto W_eRHO_X_i = job.uniform_msg<ecc_point_t>(s_i * Z_eRHO_X.L);
  int n_dh_proofs = 2;
  auto pi_W_eRHO_K = job.uniform_msg<zk::dh_t>();
  auto pi_W_eRHO_X = job.uniform_msg<zk::dh_t>();
  pi_W_eRHO_K.prove(Z_eRHO_K.L, E_i[i], W_eRHO_K_i, s_i, sid, n_dh_proofs * i + 0);
  pi_W_eRHO_X.prove(Z_eRHO_X.L, E_i[i], W_eRHO_X_i, s_i, sid, n_dh_proofs * i + 1);

  auto K_i = job.uniform_msg<ecc_point_t>(k_i * G);
  int n_elgamal_com_pub_share_equ_proofs = 3;
  auto pi_K = job.uniform_msg<zk::elgamal_com_pub_share_equ_t>();
  pi_K.prove(E, K_i, eK_i, r_eK_i, sid, n_elgamal_com_pub_share_equ_proofs * i + 0);

  if (rv = job.plain_broadcast(W_eRHO_K_i, pi_W_eRHO_K, W_eRHO_X_i, pi_W_eRHO_X, K_i, pi_K, h2)) return rv;

  // ---------------------- Start of the 8th round of the signing protocol
  for (int j = 0; j < n; j++) {
    if (i == j) continue;
    if (h2 != h2._j) return coinbase::error(E_CRYPTO);
    if (rv = pi_W_eRHO_K._j.verify(Z_eRHO_K.L, E_i[j], W_eRHO_K_i._j, sid, n_dh_proofs * j + 0)) return rv;
    if (rv = pi_W_eRHO_X._j.verify(Z_eRHO_X.L, E_i[j], W_eRHO_X_i._j, sid, n_dh_proofs * j + 1)) return rv;
    if (rv = pi_K._j.verify(E, K_i._j, eK_i._j, sid, n_elgamal_com_pub_share_equ_proofs * j + 0)) return rv;
  }

  ecc_point_t K = SUM(K_i._js);
  bn_t r_tag = K.get_x();
  bn_t r = r_tag % q;

  ecc_point_t W_eRHO_K = SUM(W_eRHO_K_i._js);
  ecc_point_t W_eRHO_X = SUM(W_eRHO_X_i._js);

  if (W_eRHO_K != Z_eRHO_K.R) return coinbase::error(E_CRYPTO);
  if (W_eRHO_X != Z_eRHO_X.R) return coinbase::error(E_CRYPTO);

  mem_t data_to_sign = msg;
  if (data_to_sign.size > curve.size()) data_to_sign.size = curve.size();
  bn_t m = bn_t::from_bin(data_to_sign);
  bn_t r_rho_x, rho_m, r_eR_RHO_X, r_eR_RHO_M, r_eB;
  auto beta = job.uniform_msg<bn_t>();
  MODULO(q) {
    r_rho_x = r * rho_x_i;
    rho_m = m * rho_i;
    beta.msg = r_rho_x + rho_m;
    r_eR_RHO_X = r * r_eRHO_X;
    r_eR_RHO_M = m * r_eRHO_i;
    r_eB = r_eR_RHO_X + r_eR_RHO_M;
  }

  std::vector<elg_com_t> eB(n);
  for (int j = 0; j < n; j++) {
    elg_com_t eR_RHO_X = r * eRHO_X._j;
    elg_com_t eRHO_M = m * eRHO_i._j;
    eB[j] = eR_RHO_X + eRHO_M;
  }
  ecc_point_t RHO_K = rho_k_i * G;
  ecc_point_t B = beta * G;
  auto pi_R_eRHO_K = job.uniform_msg<zk::elgamal_com_pub_share_equ_t>();
  auto pi_R_eB = job.uniform_msg<zk::elgamal_com_pub_share_equ_t>();
  pi_R_eRHO_K.prove(E, RHO_K, eRHO_K, r_eRHO_K, sid, n_elgamal_com_pub_share_equ_proofs * i + 1);
  pi_R_eB.prove(E, B, eB[i], r_eB, sid, n_elgamal_com_pub_share_equ_proofs * i + 2);

  auto rho_k = job.uniform_msg<bn_t>(rho_k_i);

  if (rv = job.send_message_all_to_one(sig_receiver, rho_k, pi_R_eRHO_K, beta, pi_R_eB)) return rv;

  // ---------------------- Start of output generation
  if (job.is_party_idx(sig_receiver)) {
    for (int j = 0; j < n; j++) {
      if (i == j) continue;
      ecc_point_t RHO_K = rho_k._j * G;
      ecc_point_t B = beta._j * G;
      if (rv = pi_R_eRHO_K._j.verify(E, RHO_K, eRHO_K._j, sid, n_elgamal_com_pub_share_equ_proofs * j + 1)) return rv;
      if (rv = pi_R_eB._j.verify(E, B, eB[j], sid, n_elgamal_com_pub_share_equ_proofs * j + 2)) return rv;
    }

    bn_t sum_rho_k = SUM(rho_k._js, q);
    bn_t sum_beta = SUM(beta._js, q);
    bn_t s;
    MODULO(q) s = sum_beta / sum_rho_k;

    bn_t s_reduced = q - s;
    if (s_reduced < s) s = s_reduced;
    sig = crypto::ecdsa_signature_t(curve, r, s).to_der();
    crypto::ecc_pub_key_t pub(key.Q);
    if (rv = pub.verify(msg, sig)) return rv;
  }

  return SUCCESS;
}

error_t sign(job_mp_t& job, key_t& key, mem_t msg, const party_idx_t sig_receiver, buf_t& sig) {
  int n = job.get_n_parties();
  std::vector<std::vector<int>> ot_role_map(n, std::vector<int>(n));
  for (int i = 0; i < n; i++) {
    ot_role_map[i][i] = ot_no_role;
  }

  for (int i = 0; i <= n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
      ot_role_map[i][j] = ot_sender;
      ot_role_map[j][i] = ot_receiver;
    }
  }
  return sign(job, key, msg, sig_receiver, ot_role_map, sig);
}

}  // namespace coinbase::mpc::ecdsampc
