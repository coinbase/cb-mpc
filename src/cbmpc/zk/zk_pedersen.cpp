#include "zk_pedersen.h"

#include "small_primes.h"

namespace coinbase::zk {

pedersen_commitment_params_t::pedersen_commitment_params_t() {
  // this was generated using https://www.openssl.org/docs/man1.1.1/man3/BN_generate_prime_ex.html
  // with parameter safe = 1

  static const byte_t PED_P_BIN[] = {
      0xd8, 0xf7, 0x9b, 0x66, 0xd2, 0xcf, 0x04, 0x46, 0xa7, 0x7b, 0x03, 0x8d, 0xec, 0xcf, 0x86, 0x32, 0x11, 0xe4, 0x29,
      0xe7, 0x18, 0x29, 0x77, 0x2c, 0x47, 0xd7, 0xe3, 0x92, 0x13, 0x4b, 0x92, 0x97, 0x22, 0x10, 0x56, 0x6d, 0x7d, 0xef,
      0xdc, 0x88, 0x4d, 0xce, 0xf3, 0x34, 0x9c, 0x67, 0x1d, 0x49, 0x81, 0xc5, 0x9d, 0x5e, 0x07, 0x75, 0x80, 0x56, 0x47,
      0x0f, 0x17, 0xd0, 0xa3, 0xbf, 0x0b, 0xf9, 0x5f, 0x6d, 0xc5, 0xc5, 0x2c, 0x9f, 0x52, 0x55, 0xdc, 0x52, 0x11, 0x50,
      0x40, 0x61, 0xb6, 0x50, 0x30, 0xc0, 0x7e, 0x75, 0xcf, 0x37, 0x86, 0xae, 0x1c, 0x7c, 0x4b, 0x87, 0xc5, 0xd8, 0xe9,
      0x22, 0xb6, 0xa4, 0xa9, 0x37, 0x14, 0x10, 0xa7, 0x9d, 0xd2, 0x5e, 0x9f, 0xa3, 0xf2, 0xd7, 0xb8, 0xc1, 0xf3, 0x04,
      0x07, 0x75, 0xe2, 0xb5, 0xac, 0xb7, 0x3f, 0x92, 0x47, 0xaa, 0x63, 0x4e, 0xa6, 0x1a, 0x78, 0x4a, 0x0f, 0x25, 0x53,
      0xd5, 0x16, 0x41, 0x9e, 0x3f, 0x16, 0x7f, 0x82, 0x94, 0x4f, 0x9d, 0x1a, 0xdd, 0x10, 0x1b, 0xc6, 0xa3, 0x9c, 0x63,
      0x1d, 0xe1, 0x4f, 0x3c, 0xa4, 0xcc, 0xb6, 0x85, 0xbf, 0xf1, 0x2e, 0x92, 0x0d, 0x01, 0x3c, 0xf0, 0x97, 0x8a, 0x46,
      0x4a, 0xb5, 0xba, 0x59, 0x82, 0x7a, 0x12, 0x1b, 0x01, 0x1f, 0x45, 0x75, 0x47, 0x9e, 0x88, 0xa9, 0xc7, 0x94, 0x31,
      0x25, 0xbf, 0xd5, 0x2e, 0x48, 0x97, 0x20, 0xc7, 0x01, 0x65, 0xa5, 0x02, 0xaa, 0xb7, 0xd5, 0x9b, 0x4d, 0x17, 0xde,
      0xc4, 0x05, 0xb0, 0x69, 0xfa, 0x8f, 0x62, 0xa4, 0x1d, 0xe3, 0xab, 0xba, 0xd5, 0xf6, 0x0b, 0xca, 0xe7, 0x46, 0xe6,
      0x4a, 0x52, 0xcb, 0xc1, 0x03, 0x3a, 0x24, 0xd3, 0x09, 0x5a, 0xef, 0x0e, 0x17, 0xb5, 0x0e, 0x23, 0xf2, 0xc5, 0x7d,
      0x8a, 0xd9, 0x7b, 0x7c, 0xac, 0xa9, 0xdc, 0xb9, 0x3f,
  };

  p = mod_t(bn_t::from_bin(mem_t(PED_P_BIN, sizeof(PED_P_BIN))), /* multiplicative_dense */ true);
  assert(bn_t(p).prime());
  p_tag = mod_t((bn_t(p) - 1) / 2, /* multiplicative_dense */ true);
  assert(bn_t(p_tag).prime());
  sqrt_g = 2;
  g = 4;

  std::string param_name = "Pedersen commitment parameter h";
  sqrt_h = crypto::ro::hash_number(param_name, p, p_tag, g).mod(p);
  h = (sqrt_h * sqrt_h) % p;
};

const pedersen_commitment_params_t& pedersen_commitment_params_t::get() {
  static pedersen_commitment_params_t params;
  return params;
}

error_t pedersen_commitment_params_t::check_safe_prime_subgroup(const bn_t& x) const {
  if (x.pow_mod(p_tag, p) != 1) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

void range_pedersen_t::prove(const bn_t& q, const bn_t& c, const bn_t& x, const bn_t& r, mem_t session_id,
                             uint64_t aux) {
  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const bn_t& g = params.g;
  const bn_t& h = params.h;
  prove(q, g, h, c, x, r, session_id, aux);
}

void range_pedersen_t::prove(const bn_t& q, const bn_t& g, const bn_t& h, const bn_t& c, const bn_t& x, const bn_t& r,
                             mem_t session_id, uint64_t aux) {
  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p_tag = params.p_tag;
  const mod_t& p = params.p;
  const bn_t& sqrt_h = params.sqrt_h;
  const bn_t& sqrt_g = params.sqrt_g;

  bn_t q_with_slack = q << SEC_P_STAT;

  cb_assert(check_right_open_range(0, x, q) == SUCCESS);
  cb_assert(p_tag > q_with_slack);

  bn_t xi[t];
  bn_t ri[t];
  for (int i = 0; i < t; i++) {
    xi[i] = bn_t::rand(q_with_slack);
    ri[i] = bn_t::rand(p_tag);
    MODULO(p) c_tilde[i] = sqrt_g.pow(xi[i]) * sqrt_h.pow(ri[i]);
  }

  e = crypto::ro::hash_string(p, q, g, h, c, c_tilde, session_id, aux).bitlen(t);

  for (int i = 0; i < t; i++) {
    d[i] = xi[i];
    f[i] = ri[i];
    bool ei = e.get_bit(i);
    if (ei) {
      d[i] += x;
      MODULO(p_tag) f[i] += r;
    }
  }
}

error_t range_pedersen_t::verify(const bn_t& q, const bn_t& c, mem_t session_id, uint64_t aux) const {
  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const bn_t& h = params.h;
  const bn_t& g = params.g;
  return verify(q, g, h, c, session_id, aux);
}

error_t range_pedersen_t::verify(const bn_t& q, const bn_t& g, const bn_t& h, const bn_t& c, mem_t session_id,
                                 uint64_t aux) const {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::vartime_scope_t vartime_scope;
  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p_tag = params.p_tag;
  const mod_t& p = params.p;

  bn_t q_with_slack = q << SEC_P_STAT;
  if (p <= (q_with_slack << 1)) return coinbase::error(E_CRYPTO);

  if (rv = params.check_safe_prime_subgroup(c)) return rv;
  // Subgroup checks for c_tildes are not done due to the optimizations described in the spec

  // If in the future t != 128, then the the hash function should change to produce the output of length t
  cb_assert(t == 128);
  buf128_t e_tag = crypto::ro::hash_string(p, q, g, h, c, c_tilde, session_id, aux).bitlen128();
  if (e != e_tag) return coinbase::error(E_CRYPTO);

  bn_t local_c_tilde[t];
  bn_t D = 0;
  bn_t F = 0;
  bn_t C = 1;

  bn_t c_tilde2[t];
  for (int i = 0; i < t; i++) {
    if (rv = coinbase::crypto::check_right_open_range(0, d[i], q_with_slack)) return rv;

    bool ei = e.get_bit(i);

    if (rv = coinbase::crypto::check_open_range(0, c_tilde[i], p)) return rv;

    // Related to the optimizations described in the spec
    MODULO(p) c_tilde2[i] = c_tilde[i] * c_tilde[i];

    bn_t rho_i = bn_t::rand_bitlen(SEC_P_STAT);

    MODULO(p_tag) {
      D += d[i] * rho_i;
      F += f[i] * rho_i;
    }

    bn_t c_tilde_c_ei = c_tilde2[i];
    MODULO(p) {
      if (ei) c_tilde_c_ei *= c;
      C *= c_tilde_c_ei.pow(rho_i);
    }
  }

  bn_t C_test;
  MODULO(p) C_test = g.pow(D) * h.pow(F);
  if (C != C_test) return coinbase::error(E_CRYPTO);

  return SUCCESS;
}

void range_pedersen_interactive_t::prover_msg1(const mod_t& q) {
  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p_tag = params.p_tag;
  const mod_t& p = params.p;
  bn_t sqrt_h = params.sqrt_h;
  bn_t sqrt_g = params.sqrt_g;

  // Type conversion used for assertions later on
  q_bn = q;

  bn_t q_with_slack = q << SEC_P_STAT;
  cb_assert(p_tag > q_with_slack);

  for (int i = 0; i < t; i++) {
    xi[i] = bn_t::rand(q_with_slack);
    ri[i] = bn_t::rand(p_tag);
    MODULO(p) c_tilde[i] = sqrt_g.pow(xi[i]) * sqrt_h.pow(ri[i]);
  }
  com.id(prover_pid).gen(c_tilde);
}

void range_pedersen_interactive_t::verifier_challenge() {
  static_assert(t <= 64, "t must be <= 64 for the type used for `mask`");
  uint64_t mask = (uint64_t(1) << t) - 1;
  e = crypto::gen_random_int<uint64_t>() & mask;
}

void range_pedersen_interactive_t::prover_msg2(const bn_t& x, const bn_t& r) {
  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p_tag = params.p_tag;

  cb_assert(x >= 0);
  cb_assert(x < q_bn);

  uint64_t e_temp = e;
  for (int i = 0; i < t; i++) {
    bool ei = (e_temp & 1) != 0;
    e_temp >>= 1;

    d[i] = xi[i];
    f[i] = ri[i];

    if (ei) {
      d[i] += x;
      MODULO(p_tag) f[i] += r;
    }
  }
}

error_t range_pedersen_interactive_t::verify(const bn_t& c, const mod_t& q) {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::vartime_scope_t vartime_scope;

  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p_tag = params.p_tag;
  const mod_t& p = params.p;
  const bn_t& h = params.h;
  const bn_t& g = params.g;

  if (rv = com.id(prover_pid).open(c_tilde)) return rv;

  bn_t q_with_slack = q << SEC_P_STAT;
  if (p <= (q_with_slack << 1)) return coinbase::error(E_CRYPTO);

  if (rv = params.check_safe_prime_subgroup(c)) return rv;
  // Same optimization as in the non-interactive version

  bn_t D = 0, F = 0, C = 1;
  uint64_t e_temp = e;
  for (int i = 0; i < t; i++) {
    // Related to the optimizations described in the spec
    MODULO(p) c_tilde[i] *= c_tilde[i];

    if (rv = coinbase::crypto::check_right_open_range(0, d[i], q_with_slack)) return rv;
    bn_t rho = bn_t::rand_bitlen(SEC_P_STAT);

    MODULO(p_tag) {
      D += d[i] * rho;
      F += f[i] * rho;
    }

    bool ei = (e_temp & 1) != 0;
    e_temp >>= 1;

    bn_t Ci = c_tilde[i];
    if (ei) MODULO(p) Ci *= c;
    MODULO(p) C *= Ci.pow(rho);
  }

  bn_t C_test;
  MODULO(p) C_test = g.pow(D) * h.pow(F);
  if (C_test != C) return coinbase::error(E_CRYPTO);

  return SUCCESS;
}

void paillier_pedersen_equal_t::prove(const crypto::paillier_t& paillier, const bn_t& c, const mod_t& q,
                                      const bn_t& Com, const bn_t& x, const bn_t& R, const bn_t& rho, mem_t session_id,
                                      uint64_t aux) {
  // In our use cases, all our provers have private keys. If not, we would need to verify that gcd(R_tilde[i],N) = 1
  cb_assert(paillier.has_private_key());

  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p_tag = params.p_tag;
  const mod_t& p = params.p;
  const bn_t& g = params.g;
  const bn_t& h = params.h;

  const mod_t& N = paillier.get_N();

  bn_t q_with_slack_N = q << (SEC_P_STAT + param::lambda + 2 * param::log_alpha + 1);
  cb_assert(N > q_with_slack_N);

  bn_t q_with_slack_p = q << (SEC_P_STAT + param::lambda + param::log_alpha + 2);
  cb_assert(p > q_with_slack_p);

  bn_t q_with_slack = q << (param::log_alpha + SEC_P_STAT);
  bn_t ri[param::t];
  bn_t R_tilde[param::t];
  crypto::paillier_t::elem_t c_tilde[param::t];

  bn_t mu = bn_t::rand(p_tag);
  bn_t r = 0;
  for (int i = 0; i < param::t; i++) {
    ri[i] = bn_t::rand(q_with_slack);
    R_tilde[i] = bn_t::rand(N);
    c_tilde[i] = paillier.enc(ri[i], R_tilde[i]);
    r += ri[i] << (i * param::log_alpha);
  }

  MODULO(p) Com_tilde = g.pow(r) * h.pow(mu);

  buf_t e_buf =
      crypto::ro::hash_string(N, c, p, q, g, h, Com, c_tilde, Com_tilde, session_id, aux).bitlen(param::lambda);
  e = bn_t::from_bin_bitlen(e_buf, param::lambda);

  bn_t e_temp = e;

  for (int i = 0; i < param::t; i++) {
    bn_t ei;
    {
      crypto::vartime_scope_t vartime_scope;
      ei = mod_t::mod(e_temp, param::alpha);
    }

    e_temp >>= param::log_alpha;

    di[i] = ei * x + ri[i];

    MODULO(N) D[i] = R.pow(ei) * R_tilde[i];
  }

  MODULO(p_tag) nu = e * rho + mu;
}

error_t paillier_pedersen_equal_t::verify(const crypto::paillier_t& paillier, const bn_t& c, const mod_t& q,
                                          const bn_t& Com, mem_t session_id, uint64_t aux) {
  crypto::vartime_scope_t vartime_scope;

  error_t rv = UNINITIALIZED_ERROR;
  if (paillier_valid_key == zk_flag::unverified) return coinbase::error(E_CRYPTO);

  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p = params.p;
  const bn_t& g = params.g;
  const bn_t& h = params.h;

  bn_t q_with_slack_p = q << (SEC_P_STAT + param::lambda + param::log_alpha + 2);
  if (p <= q_with_slack_p) return coinbase::error(E_CRYPTO);

  const mod_t& N = paillier.get_N();
  if (N <= 0) return coinbase::error(E_CRYPTO);

  bn_t q_with_slack_N = q << (SEC_P_STAT + param::lambda + 2 * param::log_alpha + 1);
  if (N <= q_with_slack_N) return coinbase::error(E_CRYPTO);

  if (rv = params.check_safe_prime_subgroup(Com)) return rv;
  if (rv = params.check_safe_prime_subgroup(Com_tilde)) return rv;

  if (paillier_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N, param::alpha)) return rv;
    paillier_no_small_factors = zk_flag::verified;
  }

  // The following verification of `paillier.verify_cipher(c))` is removed and instead done with `D_prod`
  // later on to increase efficiency and save a GCD operation.

  bn_t q_with_slack = q << (param::log_alpha + SEC_P_STAT);
  const mod_t& NN = paillier.get_NN();

  crypto::paillier_t::elem_t c_tilde[param::t];
  bn_t c_inv = NN.inv(c);

  bn_t e_temp = e;
  bn_t radix = 1 << param::log_alpha;

  bn_t D_prod = c;
  bn_t d = 0;
  for (int i = 0; i < param::t; i++) {
    MODULO(N) D_prod *= D[i];

    if (rv = coinbase::crypto::check_open_range(0, di[i], q_with_slack)) return rv;
    d += di[i] << (i * param::log_alpha);

    bn_t ei = mod_t::mod(e_temp, radix);
    e_temp >>= param::log_alpha;

    crypto::paillier_t::elem_t c_tag(paillier, c_inv.pow_mod(ei, NN));
    c_tilde[i] = c_tag + paillier.enc(di[i], D[i]);
  }
  if (D_prod == 0) return coinbase::error(E_CRYPTO);
  if (!mod_t::coprime(D_prod, N)) return coinbase::error(E_CRYPTO);

  buf_t e_buf =
      crypto::ro::hash_string(N, c, p, q, g, h, Com, c_tilde, Com_tilde, session_id, aux).bitlen(param::lambda);
  bn_t e_tag = bn_t::from_bin_bitlen(e_buf, param::lambda);
  if (e != e_tag) return coinbase::error(E_CRYPTO, "e' != e");

  bn_t temp1, temp2;
  MODULO(p) {
    temp1 = Com.pow(e) * Com_tilde;
    temp2 = g.pow(d) * h.pow(nu);
  }

  if (temp1 != temp2) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

void paillier_pedersen_equal_interactive_t::prover_msg1(const crypto::paillier_t& paillier, const mod_t& q) {
  // In our use cases, all our provers have private keys. If not, we would need to verify that gcd(R_tilde[i],N) = 1
  cb_assert(paillier.has_private_key());

  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p = params.p;
  const mod_t& p_tag = params.p_tag;
  const bn_t& g = params.g;
  const bn_t& h = params.h;

  const mod_t& N = paillier.get_N();
  bn_t q_with_slack_p = q << (SEC_P_STAT + param::lambda + param::log_alpha + 2);
  cb_assert(p > q_with_slack_p);

  bn_t q_with_slack_N = q << (SEC_P_STAT + param::lambda + 2 * param::log_alpha + 1);
  cb_assert(N > q_with_slack_N);

  bn_t q_with_slack = q << (param::log_alpha + SEC_P_STAT);

  mu = bn_t::rand(p_tag);
  r = 0;
  for (int i = 0; i < param::t; i++) {
    ri[i] = bn_t::rand(q_with_slack);
    R_tilde[i] = bn_t::rand(N);
    c_tilde[i] = paillier.encrypt(ri[i], R_tilde[i]);
    r += ri[i] << (i * param::log_alpha);
  }

  MODULO(p) Com_tilde = g.pow(r) * h.pow(mu);

  com.id(prover_pid).gen(c_tilde, Com_tilde);
}

void paillier_pedersen_equal_interactive_t::verifier_challenge() { e = bn_t::rand_bitlen(param::t * param::log_alpha); }

void paillier_pedersen_equal_interactive_t::prover_msg2(const crypto::paillier_t& paillier, const bn_t& x,
                                                        const bn_t& R, const bn_t& rho) {
  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p_tag = params.p_tag;
  const mod_t& N = paillier.get_N();

  bn_t e_temp = e;

  for (int i = 0; i < param::t; i++) {
    bn_t ei;
    {
      crypto::vartime_scope_t vartime_scope;
      ei = mod_t::mod(e_temp, param::alpha);
      e_temp >>= (param::log_alpha);
    }

    di[i] = x * ei + ri[i];
    MODULO(N) Di[i] = R.pow(ei) * R_tilde[i];
  }
  MODULO(p_tag) nu = e * rho + mu;
}

error_t paillier_pedersen_equal_interactive_t::verify(const crypto::paillier_t& paillier, const bn_t& c, const mod_t& q,
                                                      const bn_t& Com) {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::vartime_scope_t vartime_scope;

  const mod_t& N = paillier.get_N();
  const mod_t& NN = paillier.get_NN();

  if (paillier_valid_key == zk_flag::unverified) return coinbase::error(E_CRYPTO);
  if (paillier_no_small_factors == zk_flag::unverified) {
    if (rv = check_integer_with_small_primes(N, param::alpha)) return rv;
    paillier_no_small_factors = zk_flag::verified;
  }

  const pedersen_commitment_params_t& params = pedersen_commitment_params_t::get();
  const mod_t& p = params.p;
  const bn_t& g = params.g;
  const bn_t& h = params.h;

  if (N <= 0) return coinbase::error(E_CRYPTO);

  bn_t q_with_slack_p = q << (SEC_P_STAT + param::lambda + param::log_alpha + 2);
  if (p <= q_with_slack_p) return coinbase::error(E_CRYPTO);

  bn_t q_with_slack_N = q << (SEC_P_STAT + param::lambda + 2 * param::log_alpha + 1);
  if (N <= q_with_slack_N) return coinbase::error(E_CRYPTO);

  // Similar to the non-interactive version, we do not verify the ciphertext here and include it in `CD`

  if (rv = com.id(prover_pid).open(c_tilde, Com_tilde)) return rv;

  if (rv = params.check_safe_prime_subgroup(Com)) return rv;
  if (rv = params.check_safe_prime_subgroup(Com_tilde)) return rv;

  bn_t d = 0;
  bn_t CD = c;
  bn_t e_temp = e;
  bn_t radix = 1 << param::log_alpha;
  bn_t q_with_slack = q << (param::log_alpha + SEC_P_STAT);

  for (int i = 0; i < param::t; i++) {
    MODULO(N) CD *= Di[i] * c_tilde[i];

    if (rv = coinbase::crypto::check_right_open_range(0, di[i], q_with_slack)) return rv;

    bn_t ei = mod_t::mod(e_temp, radix);
    e_temp >>= param::log_alpha;

    bn_t C;
    MODULO(NN) { C = (c_tilde[i] * c.pow(ei)); }
    if (C != paillier.encrypt(di[i], Di[i])) return coinbase::error(E_CRYPTO);

    d += di[i] << (i * param::log_alpha);
  }

  if (CD == 0) return coinbase::error(E_CRYPTO);
  if (!mod_t::coprime(CD, N)) return coinbase::error(E_CRYPTO);

  bn_t C1, C2;
  MODULO(p) {
    C1 = Com.pow(e) * Com_tilde;
    C2 = g.pow(d) * h.pow(nu);
  }
  if (C1 != C2) return coinbase::error(E_CRYPTO);

  return SUCCESS;
}

}  // namespace coinbase::zk
