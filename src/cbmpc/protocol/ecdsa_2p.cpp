#include "ecdsa_2p.h"

#include <cbmpc/protocol/agree_random.h>
#include <cbmpc/protocol/ec_dkg.h>
#include <cbmpc/protocol/int_commitment.h>
#include <cbmpc/protocol/sid.h>
#include <cbmpc/zk/zk_pedersen.h>

#include "util.h"

namespace coinbase::mpc::ecdsa2pc {

enum sign_mode_e {
  SIGN_MODE_DEFAULT = 0,
  SIGN_MODE_GLOBAL_ABORT = 1,
};

void paillier_gen_interactive_t::step1_p1_to_p2(crypto::paillier_t& paillier, const bn_t& x1, const mod_t& q,
                                                bn_t& c_key) {
  // The length of the Paillier is hardcoded to 2048 bits, which is enough for the curves supported by the library.
  // If a larger curves are used (e.g., curves larger than P-521), then Paillier generate should be updated to use
  // larger bitlengths.
  if (!paillier.has_private_key()) paillier.generate();
  const mod_t& N = paillier.get_N();
  this->N = N;

  r_key = bn_t::rand(N);
  this->c_key = c_key = paillier.encrypt(x1, r_key);

  const zk::pedersen_commitment_params_t& params = zk::pedersen_commitment_params_t::get();
  const mod_t& p_tag = params.p_tag;
  const mod_t& p = params.p;
  const bn_t& g = params.g;
  const bn_t& h = params.h;

  rho = bn_t::rand(p_tag);
  MODULO(p) Com = g.pow(x1) * h.pow(rho);

  equal.prover_msg1(paillier, q);
  range.prover_msg1(q);
}

void paillier_gen_interactive_t::step2_p2_to_p1() {
  valid.challenge(valid_m1);
  equal.verifier_challenge();
  range.verifier_challenge();
}

void paillier_gen_interactive_t::step3_p1_to_p2(const crypto::paillier_t& paillier, const bn_t& x1,
                                                const ecc_point_t& Q1, const crypto::mpc_pid_t& prover_pid, mem_t sid) {
  valid.prove(paillier, valid_m1, prover_pid, valid_m2);
  equal.prover_msg2(paillier, x1, r_key, rho);
  range.prover_msg2(x1, rho);
  pdl.paillier_range_exp_slack_proof = zk::zk_flag::skip;
  pdl.prove(c_key, paillier, Q1, x1, r_key, sid, 0);
}

error_t paillier_gen_interactive_t::step4_p2_output(crypto::paillier_t& paillier, const ecc_point_t& Q1,
                                                    const bn_t& c_key, const crypto::mpc_pid_t& prover_pid, mem_t sid) {
  error_t rv = UNINITIALIZED_ERROR;
  ecurve_t curve = Q1.get_curve();
  const mod_t& q = curve.order();
  paillier.create_pub(N);

  if (N.get_bits_count() < crypto::paillier_t::bit_size) return coinbase::error(E_CRYPTO);
  if (N.get_bits_count() < 3 * q.get_bits_count() + 3 * SEC_P_STAT + SEC_P_COM + 1)
    return coinbase::error(E_CRYPTO, "length of N < 3lg q+ 3 stat-sec-param + com-sec-param + 1");

  // Potential optimization: both `verify_cipher` and pdl.verify perform GCDs. These can be merged into a single GCD by
  // multiplying them together. See the notes in the spec.
  if (rv = paillier.verify_cipher(c_key)) return rv;

  if (rv = valid.verify(paillier, prover_pid, valid_m2)) return rv;

  pdl.paillier_valid_key = valid.paillier_valid_key;
  pdl.paillier_no_small_factors = valid.paillier_no_small_factors;
  pdl.paillier_range_exp_slack_proof = zk::zk_flag::skip;
  if (rv = pdl.verify(c_key, paillier, Q1, sid, 0)) return rv;

  equal.paillier_valid_key = valid.paillier_valid_key;
  equal.paillier_no_small_factors = valid.paillier_no_small_factors;
  if (rv = equal.verify(paillier, c_key, q, Com)) return rv;
  if (rv = range.verify(Com, q)) return rv;
  return SUCCESS;
}

error_t dkg(job_2p_t& job, ecurve_t curve, key_t& key) {
  error_t rv = UNINITIALIZED_ERROR;

  key.curve = curve;
  const mod_t& q = curve.order();

  paillier_gen_interactive_t paillier_gen(job.get_pid(party_t::p1));
  eckey::dkg_2p_t ec_dkg(curve, job.get_pid(party_t::p1));

  key.x_share = bn_t::rand(q);
  key.role = job.get_party();

  if (job.is_p1()) {
    ec_dkg.step1_p1_to_p2(key.x_share);
    paillier_gen.step1_p1_to_p2(key.paillier, key.x_share, ec_dkg.curve.order(), key.c_key);
  }

  if (rv = job.p1_to_p2(ec_dkg.msg1, paillier_gen.msg1, key.c_key)) return rv;

  if (job.is_p2()) {
    ec_dkg.step2_p2_to_p1(key.x_share);
    paillier_gen.step2_p2_to_p1();
  }

  if (rv = job.p2_to_p1(ec_dkg.msg2, paillier_gen.msg2)) return rv;

  if (job.is_p1()) {
    if (rv = ec_dkg.step3_p1_to_p2(key.Q)) return rv;
    paillier_gen.step3_p1_to_p2(key.paillier, key.x_share, ec_dkg.Q1, job.get_pid(party_t::p1), ec_dkg.sid);
  }

  if (rv = job.p1_to_p2(ec_dkg.msg3, paillier_gen.msg3)) return rv;

  if (job.is_p2()) {
    if (rv = ec_dkg.step4_output_p2(key.Q)) return rv;
    if (rv = paillier_gen.step4_p2_output(key.paillier, ec_dkg.Q1, paillier_gen.c_key, job.get_pid(party_t::p1),
                                          ec_dkg.sid))
      return rv;
  }
  return SUCCESS;
}

error_t refresh(job_2p_t& job, const key_t& key, key_t& new_key) {
  error_t rv = UNINITIALIZED_ERROR;
  cb_assert(job.is_party(key.role));
  new_key.role = key.role;
  new_key.curve = key.curve;
  new_key.Q = key.Q;

  const mod_t& q = key.curve.order();

  bn_t N_tag;
  bn_t rho1, rho2;
  coinbase::crypto::commitment_t com(job.get_pid(party_t::p1));
  bn_t r_key, r_key_tag, c_key_tag;
  zk::two_paillier_equal_interactive_t zk_two_paillier_equal(job.get_pid(party_t::p1));
  zk::two_paillier_equal_interactive_t::prover_msg1_t pi1_P;

  if (job.is_p1()) {
    rho1 = bn_t::rand(q);
    com.gen(rho1);

    new_key.paillier.generate();
    N_tag = new_key.paillier.get_N();
    r_key_tag = bn_t::rand(N_tag);
    c_key_tag = new_key.paillier.encrypt(key.x_share, r_key_tag);
    r_key = key.paillier.get_cipher_randomness(key.x_share, key.c_key);
    zk_two_paillier_equal.prover_msg1(q, key.paillier, new_key.paillier, pi1_P);
  }

  if (rv = job.p1_to_p2(com.msg, N_tag, c_key_tag, pi1_P)) return rv;

  zk::two_paillier_equal_interactive_t::verifier_challenge_msg_t pi2_V;
  zk::valid_paillier_interactive_t zk_valid_paillier_interactive;
  zk::valid_paillier_interactive_t::challenge_msg_t pi1_V_tag;
  if (job.is_p2()) {
    if (N_tag <= 0) return rv = job.mpc_abort(E_CRYPTO, "N' < 0");
    if (N_tag.get_bits_count() < 3 * q.get_bits_count() + 3 * SEC_P_STAT + SEC_P_COM + 1)
      return coinbase::error(E_CRYPTO, "length of N < 3lg q+ 3 stat-sec-param + com-sec-param + 1");
    if (N_tag.get_bits_count() < crypto::paillier_t::bit_size) return rv = job.mpc_abort(E_CRYPTO, "N' < 2048");

    new_key.paillier.create_pub(N_tag);
    // This includes the GCD check.
    if (rv = new_key.paillier.verify_cipher(c_key_tag)) return rv;
    rho2 = bn_t::rand(q);

    zk_two_paillier_equal.verifier_challenge_msg(pi2_V);
    zk_valid_paillier_interactive.challenge(pi1_V_tag);
  }

  if (rv = job.p2_to_p1(rho2, pi1_V_tag, pi2_V)) return rv;

  zk::two_paillier_equal_interactive_t::prover_msg2_t pi3_P;
  zk::valid_paillier_interactive_t::prover_msg_t pi2_P_tag;
  if (job.is_p1()) {
    if (rv = zk_two_paillier_equal.prover_msg2(key.paillier, new_key.paillier, key.x_share, r_key, r_key_tag, pi2_V,
                                               pi3_P))
      return rv;
    zk_valid_paillier_interactive.prove(new_key.paillier, pi1_V_tag, job.get_pid(party_t::p1), pi2_P_tag);
  }

  if (rv = job.p1_to_p2(rho1, com.rand, pi3_P, pi2_P_tag)) return rv;

  if (job.is_p2()) {
    if (rv = zk_valid_paillier_interactive.verify(new_key.paillier, job.get_pid(party_t::p1), pi2_P_tag)) return rv;

    // old key (key.c_key)
    zk_two_paillier_equal.p0_valid_key = zk::zk_flag::verified;
    zk_two_paillier_equal.p0_no_small_factors = zk::zk_flag::verified;
    zk_two_paillier_equal.c0_plaintext_range = zk::zk_flag::verified;

    // new ciphertext (c_key_tag) will be checked inside the verify function
    zk_two_paillier_equal.p1_valid_key = zk::zk_flag::verified;
    zk_two_paillier_equal.p1_no_small_factors = zk::zk_flag::verified;

    if (rv = zk_two_paillier_equal.verify(q, key.paillier, key.c_key, new_key.paillier, c_key_tag, pi1_P, pi3_P))
      return rv;
    if (rv = com.open(rho1)) return rv;
  }

  bn_t rho;
  MODULO(q) { rho = rho1 + rho2; }
  new_key.c_key = new_key.paillier.add_scalar(c_key_tag, rho, crypto::paillier_t::rerand_e::off);

  if (job.is_p1()) {
    new_key.x_share = key.x_share + rho;
  } else {
    MODULO(q) new_key.x_share = key.x_share - rho;
  }

  return SUCCESS;
}

error_t sign_batch_impl(job_2p_t& job, buf_t& sid, const key_t& key, const std::vector<mem_t>& msgs, int sign_mode_flag,
                        std::vector<buf_t>& sigs) {
  error_t rv = UNINITIALIZED_ERROR;

  bool global_abort_mode = sign_mode_flag == SIGN_MODE_GLOBAL_ABORT;

  auto n_sigs = msgs.size();
  sigs.resize(n_sigs);
  const ecurve_t curve = key.curve;
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  std::vector<bn_t> m(n_sigs);
  for (int i = 0; i < n_sigs; i++) {
    mem_t bin = msgs[i];
    bin.size = std::min(bin.size, curve.size());
    m[i] = bn_t::from_bin(bin);
  }

  if (sid.empty())
    if (rv = generate_sid_fixed_2p(job, party_t::p2, sid)) return rv;

  std::vector<bn_t> k1;
  std::vector<bn_t> k2;
  zk::uc_batch_dl_t pi_1, pi_2;
  std::vector<ecc_point_t> R1;
  std::vector<ecc_point_t> R2(n_sigs);
  coinbase::crypto::commitment_t com(sid, job.get_pid(party_t::p1));

  if (job.is_p1()) {
    k1.resize(n_sigs);
    R1.resize(n_sigs);
    for (int i = 0; i < n_sigs; i++) {
      k1[i] = curve.get_random_value();
      R1[i] = k1[i] * G;
    }
    pi_1.prove(R1, k1, sid, 1);
    // Adding msgs here serves as a way of checking the consistency of the input messages.
    com.gen(msgs, R1, pi_1);
  }

  if (rv = job.p1_to_p2(com.msg)) return rv;

  if (job.is_p2()) {
    k2.resize(n_sigs);
    for (int i = 0; i < n_sigs; i++) {
      k2[i] = curve.get_random_value();
      R2[i] = k2[i] * G;
    }
    pi_2.prove(R2, k2, sid, 2);
  }

  if (rv = job.p2_to_p1(R2, pi_2)) return rv;

  std::vector<ecc_point_t> R(n_sigs);

  if (job.is_p1()) {
    // Checking that R2 values are valid is done in the verify function.
    if (rv = pi_2.verify(R2, sid, 2)) return rv;
    for (int i = 0; i < n_sigs; i++) {
      R[i] = k1[i] * R2[i];
    }
  }

  if (rv = job.p1_to_p2(com.rand, R1, pi_1)) return rv;

  std::vector<bn_t> c(n_sigs);
  std::vector<bn_t> r(n_sigs);
  std::vector<zk_ecdsa_sign_2pc_integer_commit_t> zk_ecdsa(n_sigs);

  // This is the step 4, taken from the section in the spec called ZK Proof of Correctness for Message 4 from P2 to P1
  if (job.is_p2()) {
    const mod_t& N = key.paillier.get_N();

    if (rv = com.open(msgs, R1, pi_1)) return rv;

    // Checking that R1 values are valid is done in the verify function.
    if (rv = pi_1.verify(R1, sid, 1)) return rv;
    for (int i = 0; i < n_sigs; i++) {
      R[i] = k2[i] * R1[i];
      r[i] = R[i].get_x() % q;
      bn_t rho = bn_t::rand((q * q) << (SEC_P_STAT * 2));
      bn_t rc = bn_t::rand(N);
      if (!mod_t::coprime(rc, N)) return coinbase::error(E_CRYPTO, "gcd(rc, N) != 1");

      bn_t k2_inv;
      bn_t temp;
      MODULO(q) {
        k2_inv = k2[i].inv();
        temp = k2_inv * key.x_share;
      }
      temp = k2_inv * m[i] + temp * r[i] + rho * q;
      auto c_tag = key.paillier.enc(temp, rc);

      // We turn off rerand for the paillier encryption and do not rerand the ciphertext at the end of the scope since
      // c_tag was generated with the fresh randomness rc.
      crypto::paillier_t::rerand_scope_t paillier_rerand(crypto::paillier_t::rerand_e::off);
      crypto::paillier_t::elem_t c_key_tag = key.paillier.elem(key.c_key) + (q << SEC_P_STAT);
      crypto::paillier_t::elem_t pai_c = (c_key_tag * (k2_inv * r[i])) + c_tag;

      c[i] = pai_c.to_bn();

      if (!global_abort_mode) {
        zk_ecdsa[i].prove(key.paillier, c_key_tag, pai_c, key.x_share * G, R2[i], m[i], r[i], k2[i], key.x_share, rho,
                          rc, sid, i);
      }
    }
  }

  if (!global_abort_mode) {
    if (rv = job.p2_to_p1(c, zk_ecdsa)) return rv;
  } else {
    if (rv = job.p2_to_p1(c)) return rv;
  }

  if (job.is_p1()) {
    for (int i = 0; i < n_sigs; i++) {
      r[i] = R[i].get_x() % q;

      if (!global_abort_mode) {
        crypto::paillier_t::rerand_scope_t paillier_rerand(crypto::paillier_t::rerand_e::off);
        crypto::paillier_t::elem_t c_key_tag = key.paillier.elem(key.c_key) + (q << SEC_P_STAT);
        crypto::paillier_t::elem_t pai_c = key.paillier.elem(c[i]);

        ecc_point_t Q_pub_share = key.x_share * G;
        ecc_point_t Q_minus_xG;
        Q_minus_xG = key.Q - Q_pub_share;
        if (rv = zk_ecdsa[i].verify(curve, key.paillier, c_key_tag, pai_c, Q_minus_xG, R2[i], m[i], r[i], sid, i))
          return coinbase::error(rv, "zk_ecdsa_sign_2pc_integer_commit_t::verify failed");
      }

      bn_t s = key.paillier.decrypt(c[i]);
      s = q.mod(s);

      MODULO(q) { s /= k1[i]; }

      bn_t q_minus_s = q - s;
      if (q_minus_s < s) s = q_minus_s;

      crypto::ecdsa_signature_t sig(curve, r[i], s);
      sigs[i] = sig.to_der();

      // verify
      crypto::ecc_pub_key_t ecc_verification_key(key.Q);
      if (rv = ecc_verification_key.verify(msgs[i], sigs[i]))
        if (global_abort_mode)
          return coinbase::error(E_ECDSA_2P_BIT_LEAK, "signature verification failed");
        else
          return coinbase::error(rv, "signature verification failed");
    }
  }

  return SUCCESS;
}

error_t sign_batch(job_2p_t& job, buf_t& sid, const key_t& key, const std::vector<mem_t>& msgs,
                   std::vector<buf_t>& sigs) {
  return sign_batch_impl(job, sid, key, msgs, SIGN_MODE_DEFAULT, sigs);
}

error_t sign(job_2p_t& job, buf_t& sid, const key_t& key, const mem_t msg, buf_t& sig) {
  error_t rv = UNINITIALIZED_ERROR;
  std::vector<mem_t> msgs(1, msg);
  std::vector<buf_t> sigs;
  if (rv = sign_batch(job, sid, key, msgs, sigs)) return rv;
  sig = sigs[0];
  return SUCCESS;
}

error_t sign_with_global_abort_batch(job_2p_t& job, buf_t& sid, const key_t& key, const std::vector<mem_t>& msgs,
                                     std::vector<buf_t>& sigs) {
  return sign_batch_impl(job, sid, key, msgs, SIGN_MODE_GLOBAL_ABORT, sigs);
}

error_t sign_with_global_abort(job_2p_t& job, buf_t& sid, const key_t& key, const mem_t msg, buf_t& sig) {
  error_t rv = UNINITIALIZED_ERROR;
  std::vector<mem_t> msgs(1, msg);
  std::vector<buf_t> sigs;
  if (rv = sign_with_global_abort_batch(job, sid, key, msgs, sigs)) return rv;
  sig = sigs[0];
  return SUCCESS;
}

void zk_ecdsa_sign_2pc_integer_commit_t::prove(const crypto::paillier_t& paillier,
                                               const crypto::paillier_t::elem_t& c_key,
                                               const crypto::paillier_t::elem_t& c, const ecc_point_t& Q2,
                                               const ecc_point_t& R2, const bn_t& m_tag, const bn_t& r, const bn_t& k2,
                                               const bn_t& x2, const bn_t& rho, const bn_t& rc, mem_t sid,
                                               uint64_t aux) {
  crypto::paillier_t::rerand_scope_t paillier_rerand(crypto::paillier_t::rerand_e::off);

  const mod_t& N = paillier.get_N();
  ecurve_t curve = Q2.get_curve();
  const mod_t& q = curve.order();

  const crypto::unknown_order_pedersen_params_t& params = crypto::unknown_order_pedersen_params_t::get();
  const mod_t& N_ped = params.N;
  const bn_t& g = params.g;
  const bn_t& h = params.h;

  // This has nothing to do with the c_key_tag in the signing.
  // The c_key input here is actually c_key + q << SEC_P_STAT as required in the spec
  crypto::paillier_t::elem_t c_key_tag = r * c_key;
  bn_t w1;
  bn_t w2;
  MODULO(q) {
    w1 = k2.inv();
    w2 = w1 * x2;
  }
  bn_t w3 = rho;
  bn_t w4 = rc;

  bn_t r1_w = bn_t::rand(N_ped << SEC_P_STAT);
  bn_t r2_w = bn_t::rand(N_ped << SEC_P_STAT);
  bn_t r3_w = bn_t::rand(N_ped << SEC_P_STAT);

  MODULO(N_ped) {
    // Integer commitments
    W1 = g.pow(w1) * h.pow(r1_w);
    W2 = g.pow(w2) * h.pow(r2_w);
    W3 = g.pow(w3) * h.pow(r3_w);
  }

  bn_t w1_tag = bn_t::rand(q << (SEC_P_STAT + SEC_P_COM));
  bn_t w2_tag = bn_t::rand(q << (SEC_P_STAT + SEC_P_COM));
  bn_t w3_tag = bn_t::rand((q * q) << (3 * SEC_P_STAT + SEC_P_COM));

  bn_t r1_w_tag = bn_t::rand(N_ped << (2 * SEC_P_STAT + SEC_P_COM));
  bn_t r2_w_tag = bn_t::rand(N_ped << (2 * SEC_P_STAT + SEC_P_COM));
  bn_t r3_w_tag = bn_t::rand(N_ped << (2 * SEC_P_STAT + SEC_P_COM));

  MODULO(N_ped) {
    // Integer commitments
    W1_tag = g.pow(w1_tag) * h.pow(r1_w_tag);
    W2_tag = g.pow(w2_tag) * h.pow(r2_w_tag);
    W3_tag = g.pow(w3_tag) * h.pow(r3_w_tag);
  }

  G_tag = w1_tag * R2;
  Q2_tag = w2_tag * R2;

  bn_t r_enc = bn_t::rand(N);
  cb_assert(mod_t::coprime(r_enc, N));

  bn_t temp = w1_tag * m_tag + w2_tag * r + w3_tag * q;
  crypto::paillier_t::elem_t C_enc_tag = paillier.enc(temp, r_enc) + (w1_tag * c_key_tag);
  this->C_enc_tag = C_enc_tag.to_bn();

  buf_t e_buf = crypto::ro::hash_string(N, c_key, c, Q2, R2, m_tag, r, W1, W2, W3, W1_tag, W2_tag, W3_tag, G_tag,
                                        Q2_tag, C_enc_tag, sid, aux)
                    .bitlen(SEC_P_COM);
  e = bn_t::from_bin(e_buf);

  w1_tag_tag = w1_tag + e * w1;
  w2_tag_tag = w2_tag + e * w2;
  w3_tag_tag = w3_tag + e * w3;

  r1_w_tag_tag = r1_w_tag + e * r1_w;
  r2_w_tag_tag = r2_w_tag + e * r2_w;
  r3_w_tag_tag = r3_w_tag + e * r3_w;

  MODULO(N) { r_enc_tag_tag = r_enc * w4.pow(e); }
}

error_t zk_ecdsa_sign_2pc_integer_commit_t::verify(const ecurve_t curve, const crypto::paillier_t& paillier,
                                                   const crypto::paillier_t::elem_t& c_key,
                                                   const crypto::paillier_t::elem_t& c, const ecc_point_t& Q2,
                                                   const ecc_point_t& R2, const bn_t& m_tag, const bn_t& r, mem_t sid,
                                                   uint64_t aux) const {
  crypto::vartime_scope_t vartime_scope;
  error_t rv = UNINITIALIZED_ERROR;
  crypto::paillier_t::rerand_scope_t paillier_rerand(crypto::paillier_t::rerand_e::off);

  const crypto::unknown_order_pedersen_params_t& params = crypto::unknown_order_pedersen_params_t::get();
  const mod_t& N_ped = params.N;
  const bn_t& g = params.g;
  const bn_t& h = params.h;

  const mod_t& N = paillier.get_N();
  const mod_t& NN = paillier.get_NN();

  const mod_t& q = curve.order();
  const auto& G = curve.generator();

  buf_t e_buf = crypto::ro::hash_string(N, c_key, c, Q2, R2, m_tag, r, W1, W2, W3, W1_tag, W2_tag, W3_tag, G_tag,
                                        Q2_tag, C_enc_tag, sid, aux)
                    .bitlen(SEC_P_COM);
  if (e != bn_t::from_bin(e_buf)) return coinbase::error(E_CRYPTO);

  crypto::paillier_t::elem_t C_enc_tag = paillier.elem(this->C_enc_tag);
  crypto::paillier_t::elem_t c_key_tag = r * c_key;

  if (rv = check_right_open_range(0, r1_w_tag_tag, N_ped << (2 * SEC_P_STAT + SEC_P_COM + 1))) return rv;
  if (rv = check_right_open_range(0, r2_w_tag_tag, N_ped << (2 * SEC_P_STAT + SEC_P_COM + 1))) return rv;
  if (rv = check_right_open_range(0, r3_w_tag_tag, N_ped << (2 * SEC_P_STAT + SEC_P_COM + 1))) return rv;

  if (rv = curve.check(Q2)) return coinbase::error(rv, "zk_ecdsa_sign_2pc_integer_commit_t::verify: check Q2 failed");
  if (rv = curve.check(R2)) return coinbase::error(rv, "zk_ecdsa_sign_2pc_integer_commit_t::verify: check R2 failed");
  if (rv = curve.check(G_tag))
    return coinbase::error(rv, "zk_ecdsa_sign_2pc_integer_commit_t::verify: check G_tag failed");
  if (rv = curve.check(Q2_tag))
    return coinbase::error(rv, "zk_ecdsa_sign_2pc_integer_commit_t::verify: check Q2_tag failed");

  if (rv = check_right_open_range(0, m_tag, q)) return rv;
  if (rv = check_right_open_range(0, r, q)) return rv;

  if (rv = check_open_range(0, W1, N_ped)) return rv;
  if (rv = check_open_range(0, W2, N_ped)) return rv;
  if (rv = check_open_range(0, W3, N_ped)) return rv;
  if (rv = check_open_range(0, W1_tag, N_ped)) return rv;
  if (rv = check_open_range(0, W2_tag, N_ped)) return rv;
  if (rv = check_open_range(0, W3_tag, N_ped)) return rv;

  if (rv = check_open_range(0, C_enc_tag.to_bn(), NN)) return rv;
  if (rv = check_open_range(0, c_key.to_bn(), NN)) return rv;
  if (rv = check_open_range(0, c.to_bn(), NN)) return rv;

  if (rv = check_right_open_range(0, w1_tag_tag, q << (SEC_P_STAT + SEC_P_COM + 1))) return rv;
  if (rv = check_right_open_range(0, w2_tag_tag, q << (SEC_P_STAT + SEC_P_COM + 1))) return rv;
  if (rv = check_right_open_range(0, w3_tag_tag, (q * q) << (3 * SEC_P_STAT + SEC_P_COM + 1))) return rv;

  if (rv = check_open_range(0, r_enc_tag_tag, N)) return rv;

  if (w1_tag_tag * R2 != e * G + G_tag) return coinbase::error(E_CRYPTO);
  if (w2_tag_tag * R2 != e * Q2 + Q2_tag) return coinbase::error(E_CRYPTO);

  MODULO(N_ped) {
    if (g.pow(w1_tag_tag) * h.pow(r1_w_tag_tag) != W1_tag * W1.pow(e)) return coinbase::error(E_CRYPTO);
    if (g.pow(w2_tag_tag) * h.pow(r2_w_tag_tag) != W2_tag * W2.pow(e)) return coinbase::error(E_CRYPTO);
    if (g.pow(w3_tag_tag) * h.pow(r3_w_tag_tag) != W3_tag * W3.pow(e)) return coinbase::error(E_CRYPTO);
  }

  bn_t temp = (w1_tag_tag * m_tag) + (w2_tag_tag * r) + (w3_tag_tag * q);
  auto left = paillier.enc(temp, r_enc_tag_tag) + (w1_tag_tag * c_key_tag);
  auto right = C_enc_tag + (e * c);

  if (left != right) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

}  // namespace coinbase::mpc::ecdsa2pc