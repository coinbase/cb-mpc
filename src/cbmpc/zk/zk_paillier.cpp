#include "zk_paillier.h"

#include <cbmpc/crypto/elgamal.h>

#include "small_primes.h"

namespace coinbase::zk {

void valid_paillier_t::prove(const crypto::paillier_t& paillier, mem_t session_id, uint64_t aux) {
  cb_assert(paillier.has_private_key());
  const mod_t& N = paillier.get_N();
  const bn_t& phi_N = paillier.get_phi_N();

  bn_t N_inv = mod_t::N_inv_mod_phiN_2048(N, phi_N);

  assert(SEC_P_COM == 128 && "security parameter changed, please update the code");
  buf128_t k = crypto::ro::hash_string(N, session_id, aux).bitlen128();
  crypto::drbg_aes_ctr_t drbg(k);

  for (int i = 0; i < param::t; i++) {
    bn_t rho = drbg.gen_bn(N);
    sigma[i] = rho.pow_mod(N_inv, N);
  }
}

error_t valid_paillier_t::verify(const crypto::paillier_t& paillier, mem_t session_id, uint64_t aux) {
  crypto::vartime_scope_t vartime_scope;
  error_t rv = UNINITIALIZED_ERROR;
  const mod_t& N = paillier.get_N();

  buf128_t k = crypto::ro::hash_string(N, session_id, aux).bitlen128();
  crypto::drbg_aes_ctr_t drbg(k);

  if (N <= 0) return coinbase::error(E_CRYPTO);
  if (paillier_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N, param::alpha)) return rv;
    paillier_no_small_factors = zk_flag::verified;
  }

  bn_t rho_prod = 1;
  for (int i = 0; i < param::t; i++) {
    bn_t rho = drbg.gen_bn(N);
    MODULO(N) rho_prod *= rho;
    if (sigma[i] < 0) return coinbase::error(E_CRYPTO);
    if (sigma[i].pow_mod(N, N) != rho) return coinbase::error(E_CRYPTO);
  }
  if (!mod_t::coprime(rho_prod, N)) return coinbase::error(E_CRYPTO);
  paillier_valid_key = zk_flag::verified;
  return SUCCESS;
}

void valid_paillier_interactive_t::challenge(challenge_msg_t& challenge_msg) {
  crypto::gen_random(kV);
  challenge_msg.kV = kV;
}

void valid_paillier_interactive_t::valid_paillier_interactive_t::prove(const crypto::paillier_t& paillier,
                                                                       const challenge_msg_t& challenge_msg,
                                                                       const crypto::mpc_pid_t& prover_pid,
                                                                       prover_msg_t& prover_msg) const {
  cb_assert(paillier.has_private_key());
  const mod_t& N = paillier.get_N();
  const bn_t& phi_N = paillier.get_phi_N();

  bn_t N_inv = mod_t::N_inv_mod_phiN_2048(N, phi_N);

  buf128_t k = crypto::ro::hash_string(challenge_msg.kV, N, prover_pid).bitlen128();
  crypto::drbg_aes_ctr_t drbg(k);

  for (int i = 0; i < param::t; i++) {
    bn_t rho = drbg.gen_bn(N);
    prover_msg.sigma[i] = rho.pow_mod(N_inv, N);
  }
}

error_t valid_paillier_interactive_t::verify(const crypto::paillier_t& paillier, const crypto::mpc_pid_t& prover_pid,
                                             const prover_msg_t& prover_msg) {
  crypto::vartime_scope_t vartime_scope;

  error_t rv = UNINITIALIZED_ERROR;
  const mod_t& N = paillier.get_N();
  buf128_t k = crypto::ro::hash_string(kV, N, prover_pid).bitlen128();
  crypto::drbg_aes_ctr_t drbg(k);

  if (N <= 0) return coinbase::error(E_CRYPTO);
  if (paillier_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N, param::alpha)) return rv;
    paillier_no_small_factors = zk_flag::verified;
  }

  bn_t rho_prod = 1;
  for (int i = 0; i < param::t; i++) {
    bn_t rho = drbg.gen_bn(N);
    MODULO(N) rho_prod *= rho;

    if (prover_msg.sigma[i] < 0) return coinbase::error(E_CRYPTO);
    if (prover_msg.sigma[i].pow_mod(N, N) != rho) return coinbase::error(E_CRYPTO);
  }

  if (!mod_t::coprime(rho_prod, N)) return coinbase::error(E_CRYPTO);
  paillier_valid_key = zk_flag::verified;
  return SUCCESS;
}

//-------------------------------  paillier_zero_t -------------------------

// only 13 bits used from each 16 bits block (uint16_t).
// 16 bits are used for simpler splitting using uint16_t

void paillier_zero_t::prove(const crypto::paillier_t& paillier, const bn_t& c, const bn_t& r, mem_t session_id,
                            uint64_t aux) {
  // In our use cases, all our provers have private keys. If not, we would need to verify that gcd(rho_i,N) = 1
  cb_assert(paillier.has_private_key());
  const mod_t& N = paillier.get_N();
  const mod_t& NN = paillier.get_NN();
  // This is the statement that we want to prove. Adding it here as a sanity check.
  // If this causes efficiency issues, it can be removed and the caller must ensure that the ciphertext is valid.
  cb_assert(r.pow_mod(N, NN) == c);

  bn_t rho[param::t];
  bn_t a[param::t];

  for (int i = 0; i < param::t; i++) {
    rho[i] = bn_t::rand(N);
    a[i] = rho[i].pow_mod(N, NN);
  }

  e = crypto::ro::hash_string(N, c, a, session_id, aux)
          .bitlen(param::padded_log_alpha * param::t);  // use only 13 bits for each ei

  for (int i = 0; i < param::t; i++) {
    bn_t ei = param::get_13_bits(e, i);
    MODULO(N) z[i] = rho[i] * r.pow(ei);
  }
}

error_t paillier_zero_t::verify(const crypto::paillier_t& paillier, const bn_t& c, mem_t session_id, uint64_t aux) {
  crypto::vartime_scope_t vartime_scope;

  error_t rv = UNINITIALIZED_ERROR;
  if (paillier_valid_key == zk_flag::unverified) return coinbase::error(E_CRYPTO);

  const mod_t& N = paillier.get_N();
  const mod_t& NN = paillier.get_NN();

  if (paillier_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N, valid_paillier_t::param::alpha)) return rv;
    paillier_no_small_factors = zk_flag::verified;
  }

  if (paillier_valid_ciphertext == zk_flag::unverified) {
    if (rv = paillier.verify_cipher(c)) return rv;
    paillier_valid_ciphertext = zk_flag::verified;
  }
  if (e.size() != bits_to_bytes(param::padded_log_alpha * param::t))
    return coinbase::error(E_CRYPTO);  // use only 13 bits for each ei

  bn_t d = NN.inv(c);
  bn_t a[param::t];

  bn_t z_prod = 1;
  for (int i = 0; i < param::t; i++) {
    MODULO(N) z_prod *= z[i];
    bn_t ei = param::get_13_bits(e, i);
    MODULO(NN) a[i] = z[i].pow(N) * d.pow(ei);
  }
  if (z_prod == 0 || !mod_t::coprime(z_prod, N)) return coinbase::error(E_CRYPTO);

  buf_t e_tag = crypto::ro::hash_string(N, c, a, session_id, aux)
                    .bitlen(param::padded_log_alpha * param::t);  // use only 13 bits for each ei
  if (e != e_tag) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

void paillier_zero_interactive_t::prover_msg1(const crypto::paillier_t& paillier) {
  // In our use cases, all our provers have private keys. If not, we would need to verify that gcd(rho_i,N) = 1
  cb_assert(paillier.has_private_key());
  const mod_t& N = paillier.get_N();
  const mod_t& NN = paillier.get_NN();
  for (int i = 0; i < param::t; i++) {
    rho[i] = bn_t::rand(N);
    MODULO(NN) a[i] = rho[i].pow(N);
  }

  com.id(prover_pid).gen(a);
}

void paillier_zero_interactive_t::verifier_challenge() {
  for (int i = 0; i < param::t; i++) {
    e[i] = crypto::gen_random_int<uint16_t>() & param::alpha_bits_mask;
  }
}

void paillier_zero_interactive_t::prover_msg2(const crypto::paillier_t& paillier, const bn_t& r) {
  const mod_t& N = paillier.get_N();
  for (int i = 0; i < param::t; i++) {
    MODULO(N) z[i] = rho[i] * r.pow(e[i] & param::alpha_bits_mask);
  }
}

error_t paillier_zero_interactive_t::verify(const crypto::paillier_t& paillier, const bn_t& c) {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::vartime_scope_t vartime_scope;

  if (paillier_valid_key == zk_flag::unverified) return coinbase::error(E_CRYPTO);

  const mod_t& N = paillier.get_N();
  const mod_t& NN = paillier.get_NN();

  if (paillier_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N, valid_paillier_t::param::alpha)) return rv;
    paillier_no_small_factors = zk_flag::verified;
  }

  if (paillier_valid_ciphertext == zk_flag::unverified) {
    if (rv = paillier.verify_cipher(c)) return rv;
    paillier_valid_ciphertext = zk_flag::verified;
  }

  if (rv = com.id(prover_pid).open(a)) return rv;

  bn_t AZ = 1;

  for (int i = 0; i < param::t; i++) {
    MODULO(NN) {
      if (z[i].pow(N) != a[i] * c.pow(e[i])) return coinbase::error(E_CRYPTO);
    }

    MODULO(N) AZ *= a[i] * z[i];
  }

  if (AZ == 0 || !mod_t::coprime(AZ, N)) return coinbase::error(E_CRYPTO);

  return SUCCESS;
}

void two_paillier_equal_t::prove(const mod_t& q, const crypto::paillier_t& P0, const bn_t& c0,
                                 const crypto::paillier_t& P1, const bn_t& c1, const bn_t& x, const bn_t& r0,
                                 const bn_t& r1, mem_t session_id, uint64_t aux) {
  cb_assert(P0.has_private_key());
  cb_assert(P1.has_private_key());

  const mod_t& N0 = P0.get_N();
  const mod_t& N1 = P1.get_N();

  int test_len = q.get_bits_count() + param::log_alpha + SEC_P_STAT + 1;
  cb_assert(N0.get_bits_count() >= std::max(2048, test_len));
  cb_assert(N1.get_bits_count() >= std::max(2048, test_len));

  bn_t q_with_slack = q << (param::log_alpha + SEC_P_STAT);

  bn_t tau[param::t];
  bn_t R0_tilde[param::t];
  bn_t R1_tilde[param::t];

  crypto::paillier_t::elem_t c0_tilde[param::t];
  crypto::paillier_t::elem_t c1_tilde[param::t];

  for (int i = 0; i < param::t; i++) {
    tau[i] = bn_t::rand(q_with_slack);

    R0_tilde[i] = bn_t::rand(N0);
    c0_tilde[i] = P0.enc(tau[i], R0_tilde[i]);

    R1_tilde[i] = bn_t::rand(N1);
    c1_tilde[i] = P1.enc(tau[i], R1_tilde[i]);
  }

  e = crypto::ro::hash_string(N0, c0, N1, c1, c0_tilde, c1_tilde, session_id, aux)
          .bitlen(param::t * param::padded_log_alpha);  // only 13 bits are used for each ei

  for (int i = 0; i < param::t; i++) {
    bn_t ei = param::get_13_bits(e, i);
    d[i] = ei * x + tau[i];
    MODULO(N0) r0_hat[i] = r0.pow(ei) * R0_tilde[i];
    MODULO(N1) r1_hat[i] = r1.pow(ei) * R1_tilde[i];
  }
}

error_t two_paillier_equal_t::verify(const mod_t& q, const crypto::paillier_t& P0, const bn_t& c0,  // old
                                     const crypto::paillier_t& P1, const bn_t& c1,                  // new
                                     mem_t session_id, uint64_t aux) {
  crypto::vartime_scope_t vartime_scope;
  error_t rv = UNINITIALIZED_ERROR;

  const mod_t& N0 = P0.get_N();
  const mod_t& N1 = P1.get_N();

  if (p0_valid_key == zk_flag::unverified) return coinbase::error(E_CRYPTO);
  if (p1_valid_key == zk_flag::unverified) return coinbase::error(E_CRYPTO);

  if (c0_plaintext_range == zk_flag::unverified) return coinbase::error(E_CRYPTO);

  if (p0_valid_ciphertext == zk_flag::unverified) {
    if (rv = P0.verify_cipher(c0)) return rv;
    p0_valid_ciphertext = zk_flag::verified;
  }
  if (p1_valid_ciphertext == zk_flag::unverified) {
    if (rv = P1.verify_cipher(c1)) return rv;
    p1_valid_ciphertext = zk_flag::verified;
  }
  if (p0_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N0, valid_paillier_t::param::alpha)) return rv;
    p0_no_small_factors = zk_flag::verified;
  }
  if (p1_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N1, valid_paillier_t::param::alpha)) return rv;
    p1_no_small_factors = zk_flag::verified;
  }

  if (bn_t(N0) <= 0) return coinbase::error(E_CRYPTO);
  if (bn_t(N1) <= 0) return coinbase::error(E_CRYPTO);
  int test_len = q.get_bits_count() + param::log_alpha + SEC_P_STAT + 1;
  if (N0.get_bits_count() < std::max(2048, test_len)) return coinbase::error(E_CRYPTO);
  if (N1.get_bits_count() < std::max(2048, test_len)) return coinbase::error(E_CRYPTO);

  if (e.size() != coinbase::bits_to_bytes(param::t * param::padded_log_alpha))
    return coinbase::error(E_CRYPTO);  // only 13 bits are used for each ei

  const mod_t& NN0 = P0.get_NN();
  const mod_t& NN1 = P1.get_NN();
  bn_t q_with_slack = q << (param::log_alpha + SEC_P_STAT);

  crypto::paillier_t::elem_t c0_tilde[param::t];
  crypto::paillier_t::elem_t c1_tilde[param::t];
  bn_t c0_inv = NN0.inv(c0);
  bn_t c1_inv = NN1.inv(c1);

  bn_t r0_hat_prod = 1;
  bn_t r1_hat_prod = 1;
  for (int i = 0; i < param::t; i++) {
    if (rv = coinbase::crypto::check_right_open_range(0, d[i], q_with_slack)) return rv;

    bn_t ei = param::get_13_bits(e, i);

    if (r0_hat[i] <= 0) return coinbase::error(E_CRYPTO);
    if (r1_hat[i] <= 0) return coinbase::error(E_CRYPTO);

    MODULO(N0) r0_hat_prod *= r0_hat[i];
    MODULO(N1) r1_hat_prod *= r1_hat[i];

    crypto::paillier_t::rerand_scope_t paillier_rerand(crypto::paillier_t::rerand_e::off);
    c0_tilde[i] = P0.enc(d[i], r0_hat[i]) + P0.elem(c0_inv.pow_mod(ei, NN0));
    c1_tilde[i] = P1.enc(d[i], r1_hat[i]) + P1.elem(c1_inv.pow_mod(ei, NN1));
  }
  if (!mod_t::coprime(r0_hat_prod, N0)) return coinbase::error(E_CRYPTO);
  if (!mod_t::coprime(r1_hat_prod, N1)) return coinbase::error(E_CRYPTO);

  buf_t e_tag = crypto::ro::hash_string(N0, c0, N1, c1, c0_tilde, c1_tilde, session_id, aux)
                    .bitlen(param::t * param::padded_log_alpha);  // only 13 bits are used for each ei
  if (e_tag != e) return coinbase::error(E_CRYPTO);
  c1_plaintext_range = zk_flag::verified;
  return SUCCESS;
}

void two_paillier_equal_interactive_t::prover_msg1(const mod_t& q, const crypto::paillier_t& P0,
                                                   const crypto::paillier_t& P1, prover_msg1_t& msg1) {
  // In our use cases, all our provers have private keys. If not, we would need to verify that gcd(rho_i,N) = 1
  cb_assert(P0.has_private_key());
  cb_assert(P1.has_private_key());

  const mod_t& N0 = P0.get_N();
  const mod_t& N1 = P1.get_N();

  int test_len = q.get_bits_count() + param::log_alpha + SEC_P_STAT + 1;
  cb_assert(N0.get_bits_count() >= std::max(2048, test_len));
  cb_assert(N1.get_bits_count() >= std::max(2048, test_len));

  bn_t q_with_slack = q << (param::log_alpha + SEC_P_STAT);

  for (int i = 0; i < param::t; i++) {
    tau[i] = bn_t::rand(q_with_slack);

    R0_tilde[i] = bn_t::rand(N0);
    c0_tilde[i] = P0.encrypt(tau[i], R0_tilde[i]);

    R1_tilde[i] = bn_t::rand(N1);
    c1_tilde[i] = P1.encrypt(tau[i], R1_tilde[i]);
  }

  crypto::commitment_t com(prover_pid);

  com.gen(c0_tilde, c1_tilde);
  msg1.com_msg = com.msg;
  com_rand = com.rand;
}

void two_paillier_equal_interactive_t::verifier_challenge_msg(verifier_challenge_msg_t& msg) {
  e = msg.e = crypto::gen_random_bits(param::t * param::padded_log_alpha);  // only 13 bits are used for each ei
}

error_t two_paillier_equal_interactive_t::prover_msg2(const crypto::paillier_t& P0, const crypto::paillier_t& P1,
                                                      const bn_t& x, const bn_t& r0, const bn_t& r1,
                                                      const verifier_challenge_msg_t& challenge_msg,
                                                      prover_msg2_t& msg2) const {
  if (coinbase::bits_to_bytes(param::t * param::padded_log_alpha) != challenge_msg.e.size())
    return coinbase::error(E_FORMAT);  // only 13 bits are used for each ei
  const mod_t& N0 = P0.get_N();
  const mod_t& N1 = P1.get_N();

  for (int i = 0; i < param::t; i++) {
    bn_t ei = param::get_13_bits(challenge_msg.e, i);
    msg2.d[i] = ei * x + tau[i];
    MODULO(N0) msg2.r0_hat[i] = r0.pow(ei) * R0_tilde[i];
    MODULO(N1) msg2.r1_hat[i] = r1.pow(ei) * R1_tilde[i];
    msg2.c0_tilde[i] = c0_tilde[i];
    msg2.c1_tilde[i] = c1_tilde[i];
  }
  msg2.com_rand = com_rand;
  return SUCCESS;
}

error_t two_paillier_equal_interactive_t::verify(const mod_t& q, const crypto::paillier_t& P0,
                                                 const bn_t& c0,                                // old
                                                 const crypto::paillier_t& P1, const bn_t& c1,  // new
                                                 const prover_msg1_t& msg1, const prover_msg2_t& msg2) {
  crypto::vartime_scope_t vartime_scope;
  error_t rv = UNINITIALIZED_ERROR;

  const mod_t& N0 = P0.get_N();
  const mod_t& N1 = P1.get_N();

  const mod_t& NN0 = P0.get_NN();
  const mod_t& NN1 = P1.get_NN();

  crypto::commitment_t com(prover_pid);
  if (rv = com.set(msg2.com_rand, msg1.com_msg).open(msg2.c0_tilde, msg2.c1_tilde)) return rv;

  if (p0_valid_key == zk_flag::unverified) return coinbase::error(E_CRYPTO);
  if (p1_valid_key == zk_flag::unverified) return coinbase::error(E_CRYPTO);

  if (c0_plaintext_range == zk_flag::unverified) return coinbase::error(E_CRYPTO);

  if (p0_valid_ciphertext == zk_flag::unverified) {
    if (rv = P0.verify_cipher(c0)) return rv;
    p0_valid_ciphertext = zk_flag::verified;
  }

  if (p1_valid_ciphertext == zk_flag::unverified) {
    if (rv = P1.verify_cipher(c1)) return rv;
    p1_valid_ciphertext = zk_flag::verified;
  }

  if (p0_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N0, valid_paillier_t::param::alpha)) return rv;
    p0_no_small_factors = zk_flag::verified;
  }

  if (p1_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N1, valid_paillier_t::param::alpha)) return rv;
    p1_no_small_factors = zk_flag::verified;
  }

  if (bn_t(N0) <= 0) return coinbase::error(E_CRYPTO);
  if (bn_t(N1) <= 0) return coinbase::error(E_CRYPTO);
  int test_len = q.get_bits_count() + param::log_alpha + SEC_P_STAT + 1;
  if (N0.get_bits_count() < std::max(2048, test_len)) return coinbase::error(E_CRYPTO);
  if (N1.get_bits_count() < std::max(2048, test_len)) return coinbase::error(E_CRYPTO);

  bn_t q_with_slack = q << (param::log_alpha + SEC_P_STAT);

  if (coinbase::bits_to_bytes(param::t * param::padded_log_alpha) != e.size())
    return coinbase::error(E_FORMAT);  // only 13 bits are used for each ei

  bn_t H0_test = c0, H1_test = c1;
  for (int i = 0; i < param::t; i++) {
    if (rv = coinbase::crypto::check_right_open_range(0, msg2.d[i], q_with_slack)) return rv;

    if (msg2.r0_hat[i] <= 0) return coinbase::error(E_CRYPTO);
    if (msg2.r1_hat[i] <= 0) return coinbase::error(E_CRYPTO);

    MODULO(N0) H0_test *= msg2.r0_hat[i] * msg2.c0_tilde[i];
    MODULO(N1) H1_test *= msg2.r1_hat[i] * msg2.c1_tilde[i];

    bn_t ei = param::get_13_bits(e, i);
    bn_t t0, t1;
    MODULO(NN0) t0 = c0.pow(ei) * msg2.c0_tilde[i];
    MODULO(NN1) t1 = c1.pow(ei) * msg2.c1_tilde[i];

    if (t0 != P0.encrypt(msg2.d[i], msg2.r0_hat[i])) return coinbase::error(E_CRYPTO);
    if (t1 != P1.encrypt(msg2.d[i], msg2.r1_hat[i])) return coinbase::error(E_CRYPTO);
  }

  if (H0_test == 0) return coinbase::error(E_CRYPTO);
  if (H1_test == 0) return coinbase::error(E_CRYPTO);
  if (!mod_t::coprime(H0_test, N0)) return coinbase::error(E_CRYPTO);
  if (!mod_t::coprime(H1_test, N1)) return coinbase::error(E_CRYPTO);

  c1_plaintext_range = zk_flag::verified;
  return SUCCESS;
}

void pdl_t::prove(const bn_t& c_key, const crypto::paillier_t& paillier, const ecc_point_t& Q1, const bn_t& x1,
                  const bn_t& r_key, mem_t sid, uint64_t aux) {
  // In our use cases, all our provers have private keys. If not, we would need to verify that gcd(r_rand,N) = 1
  cb_assert(paillier.has_private_key());

  ecurve_t curve = Q1.get_curve();
  const mod_t& q = curve.order();
  const auto& G = curve.generator();
  const mod_t& N = paillier.get_N();

  bn_t qq = q * q;
  cb_assert(N.get_bits_count() >= 2048 && N >= ((qq << (SEC_P_STAT + 1)) + (qq << 1)));

  bn_t r_rand = bn_t::rand(N);

  // We sample r from Z_{q^2*2^kappa} in the indirect way to avoid non-constant time mod when calculating r * G
  bn_t r_mod_q = bn_t::rand(q);
  bn_t r = bn_t::rand(q << SEC_P_STAT) * q + r_mod_q;
  c_r = paillier.encrypt(r, r_rand);
  R = r_mod_q * G;

  bn_t e = crypto::ro::hash_number(c_key, N, Q1, c_r, R, sid, aux).mod(q);
  z = r + e * x1;
  MODULO(N) r_z = r_rand * r_key.pow_mod(e, N);

  if (paillier_range_exp_slack_proof != zk_flag::skip) {
    zk_paillier_range_exp_slack.prove(paillier, q, c_key, x1, r_key, sid, aux);
  }
}

error_t pdl_t::verify(const bn_t& c_key, const crypto::paillier_t& paillier, const ecc_point_t& Q1, mem_t sid,
                      uint64_t aux) {
  crypto::paillier_t::rerand_scope_t paillier_rerand(crypto::paillier_t::rerand_e::off);
  crypto::vartime_scope_t vartime_scope;
  error_t rv = UNINITIALIZED_ERROR;

  const mod_t& N = paillier.get_N();
  ecurve_t curve = Q1.get_curve();
  const mod_t& q = curve.order();
  const auto& G = curve.generator();

  bn_t e = crypto::ro::hash_number(c_key, N, Q1, c_r, R, sid, aux).mod(q);

  if (paillier_valid_key == zk_flag::unverified) return coinbase::error(E_CRYPTO);
  if (paillier_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N, valid_paillier_t::param::alpha)) return rv;
    paillier_no_small_factors = zk_flag::verified;
  }
  if (paillier_valid_ciphertext == zk_flag::unverified) {
    if (rv = paillier.verify_cipher(c_key)) return rv;
    paillier_valid_ciphertext = zk_flag::verified;
  }

  bn_t qq = q * q;
  if (N.get_bits_count() < 2048 || N < ((qq << (SEC_P_STAT + 1)) + (qq << 1))) return coinbase::error(E_CRYPTO);

  const mod_t& NN = paillier.get_NN();
  if (rv = coinbase::crypto::check_open_range(0, c_r, NN)) return rv;

  bn_t gcd_test;
  MODULO(N) gcd_test = c_r * e * r_z;
  if (!mod_t::coprime(gcd_test, N)) return coinbase::error(E_CRYPTO);

  if (z * G != R + e * Q1) return coinbase::error(E_CRYPTO);

  if (z >= (qq + 1) << SEC_P_STAT) return coinbase::error(E_CRYPTO);

  crypto::paillier_t::elem_t c_z = paillier.elem(c_r) + (paillier.elem(c_key) * e);
  if (paillier.encrypt(z, r_z) != c_z.to_bn()) return coinbase::error(E_CRYPTO);

  if (paillier_range_exp_slack_proof != zk_flag::skip) {
    zk_paillier_range_exp_slack.paillier_valid_key = paillier_valid_key;
    zk_paillier_range_exp_slack.paillier_no_small_factors = paillier_no_small_factors;
    if (rv = zk_paillier_range_exp_slack.verify(paillier, q, c_key, sid, aux)) return rv;
  }

  return SUCCESS;
}

void paillier_range_exp_slack_t::prove(const crypto::paillier_t& paillier, const mod_t& q, const bn_t& c, const bn_t& x,
                                       const bn_t& r, mem_t session_id, uint64_t aux) {
  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p_tag = params.p_tag;
  const mod_t& p = params.p;
  const bn_t& g = params.g;
  const bn_t& h = params.h;

  bn_t rho = bn_t::rand(p_tag);
  MODULO(p) Com = g.pow(x) * h.pow(rho);

  zk_paillier_pedersen_equal.prove(paillier, c, q, Com, x, r, rho, session_id, aux);
  zk_range_pedersen.prove(q, Com, x, rho, session_id, aux);
}

error_t paillier_range_exp_slack_t::verify(const crypto::paillier_t& paillier, const mod_t& q, const bn_t& c,
                                           mem_t session_id, uint64_t aux) {
  error_t rv = UNINITIALIZED_ERROR;

  zk_paillier_pedersen_equal.paillier_valid_key = paillier_valid_key;
  zk_paillier_pedersen_equal.paillier_no_small_factors = paillier_no_small_factors;

  if (rv = zk_paillier_pedersen_equal.verify(paillier, c, q, Com, session_id, aux)) return rv;
  if (rv = zk_range_pedersen.verify(q, Com, session_id, aux)) return rv;
  return SUCCESS;
}

}  // namespace coinbase::zk
