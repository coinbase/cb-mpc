#include <cbmpc/core/log.h>
#include <cbmpc/crypto/base.h>

namespace coinbase::crypto {

void update_public();
void update_private();

static thread_local paillier_t::rerand_e tls_rerand_mode = paillier_t::rerand_e::off;
paillier_t::rerand_e paillier_t::rerand_scope_t::get() { return tls_rerand_mode; }
void paillier_t::rerand_scope_t::set(paillier_t::rerand_e mode) { tls_rerand_mode = mode; }

void paillier_t::convert(coinbase::converter_t& converter) {
  converter.convert(has_private);
  converter.convert(N);
  if (has_private) {
    converter.convert(p);
    converter.convert(q);
  }

  if (!converter.is_write()) {
    if (has_private)
      update_private();
    else
      update_public();
  }
}

void paillier_t::generate() {
  // NOTE: This generates Paillier without safe primes since safe primes are only needed for threshold Paillier
  rsa_prv_key_t key;
  key.generate(bit_size);
  p = key.get_p();
  q = key.get_q();

  N = mod_t(p * q, /* multiplicative_dense */ true);

  update_private();
  has_private = true;
}

void paillier_t::update_public() {
  // calculate N^2
  NN = mod_t(N * N, /* multiplicative_dense */ true);
}

void paillier_t::update_private() {
  update_public();

  if (p < q) std::swap(p, q);

  // calculating phi(N) = (p-1)(q-1)
  phi_N = (p - 1) * (q - 1);

  inv_phi_N = N.inv(phi_N);

  // p^2
  bn_t p_sqr = p * p;

  // q^2
  bn_t q_sqr = q * q;

  // (q^2)^-1 (p^2)
  bn_t q_sqr_inverse = mod_t(p_sqr, /* multiplicative_dense */ true).inv(q_sqr);

  // p^2 - p
  bn_t p_sqr_minus_p = p_sqr - p;

  // q^2 - q
  bn_t q_sqr_minus_q = q_sqr - q;

  bn_t phi_N_mod_p_sqr_minus_p;
  bn_t phi_N_mod_q_sqr_minus_q;
  bn_t N_mod_p_sqr_minus_p;
  bn_t N_mod_q_sqr_minus_q;

  MODULO(LARGEST_PRIME_MOD_2048) {
    // The below calculations are based on the fact that q < p < 2q
    bn_t p_minus_q = p - q;
    phi_N_mod_p_sqr_minus_p = phi_N;
    phi_N_mod_q_sqr_minus_q = (p_minus_q - 1) * (q - 1);
    N_mod_p_sqr_minus_p = N;
    N_mod_q_sqr_minus_q = (p_minus_q + 1) * q;
  }

  crt_dec.p = mod_t(p_sqr, /* multiplicative_dense */ true);
  crt_dec.q = mod_t(q_sqr, /* multiplicative_dense */ true);
  crt_dec.dp = phi_N_mod_p_sqr_minus_p;
  crt_dec.dq = phi_N_mod_q_sqr_minus_q;
  crt_dec.qinv = q_sqr_inverse;

  crt_enc.p = mod_t(p_sqr, /* multiplicative_dense */ true);
  crt_enc.q = mod_t(q_sqr, /* multiplicative_dense */ true);
  crt_enc.dp = N_mod_p_sqr_minus_p;
  crt_enc.dq = N_mod_q_sqr_minus_q;
  crt_enc.qinv = q_sqr_inverse;
}

void paillier_t::create_prv(const bn_t& theN, const bn_t& theP, const bn_t& theQ) {
  N = mod_t(theN, /* multiplicative_dense */ true);
  p = theP;
  q = theQ;
  has_private = true;
  update_private();
}

void paillier_t::create_pub(const bn_t& theN) {
  N = mod_t(theN, /* multiplicative_dense */ true);
  has_private = false;
  update_public();
}

bn_t paillier_t::add_ciphers(const bn_t& src1, const bn_t& src2, crypto::paillier_t::rerand_e rerand_mode) const {
  bn_t res;
  MODULO(NN) res = src1 * src2;
  if (rerand_mode == rerand_e::on) res = rerand(res);
  return res;
}

bn_t paillier_t::sub_ciphers(const bn_t& src1, const bn_t& src2, crypto::paillier_t::rerand_e rerand_mode) const {
  bn_t temp = NN.inv(src2);
  bn_t res;
  MODULO(NN) res = src1 * temp;
  if (rerand_mode == rerand_e::on) res = rerand(res);
  return res;
}

bn_t paillier_t::mul_scalar(const bn_t& cipher, const bn_t& scalar, crypto::paillier_t::rerand_e rerand_mode) const {
  bn_t res;
  MODULO(NN) res = cipher.pow(scalar);
  if (rerand_mode == rerand_e::on) res = rerand(res);
  return res;
}

bn_t paillier_t::add_scalar(const bn_t& cipher, const bn_t& scalar, crypto::paillier_t::rerand_e rerand_mode) const {
  bn_t res;
  MODULO(NN) res = cipher * (scalar * N + 1);
  if (rerand_mode == rerand_e::on) res = rerand(res);
  return res;
}

bn_t paillier_t::sub_scalar(const bn_t& cipher, const bn_t& scalar, crypto::paillier_t::rerand_e rerand_mode) const {
  bn_t temp, res;
  MODULO(NN) {
    temp = bn_t(1) - scalar * N;
    res = cipher * temp;
  }

  if (rerand_mode == rerand_e::on) res = rerand(res);
  return res;
}

bn_t paillier_t::sub_cipher_scalar(const bn_t& scalar, const bn_t& cipher,
                                   crypto::paillier_t::rerand_e rerand_mode) const {
  bn_t res;
  bn_t temp = NN.inv(cipher);

  MODULO(NN) { res = (scalar * N + 1) * temp; }

  if (rerand_mode == rerand_e::on) res = rerand(res);
  return res;
}

bn_t paillier_t::encrypt(const bn_t& src) const { return encrypt(src, bn_t::rand(N)); }

bn_t paillier_t::crt_t::compute_power(const bn_t& c, const mod_t& NN) const {
  bn_t c_mod_p = c % p;
  bn_t c_mod_q = c % q;

  bn_t mp, mq;
  MODULO(p) mp = c_mod_p.pow(dp);
  MODULO(q) mq = c_mod_q.pow(dq);

  bn_t h;
  MODULO(p) h = qinv * (mp - mq);
  bn_t dec;
  MODULO(NN) dec = mq + h * q;
  return dec;
}

bn_t paillier_t::encrypt(const bn_t& src, const bn_t& rand) const {
  bn_t rn;
  if (has_private) {
    rn = crt_enc.compute_power(rand, NN);
  } else {
    cb_assert(mod_t::coprime(rand, N) && "paillier_t::encrypt: rand and N are not coprime");
    MODULO(NN) rn = rand.pow(N);
  }

  MODULO(NN) rn *= src * N + 1;
  return rn;
}

bn_t paillier_t::decrypt(const bn_t& src) const {
  bn_t c1;

  if (has_private) {
    c1 = crt_dec.compute_power(src, NN);
  } else {
    cb_assert(false);
  }

  bn_t m1 = (c1 - 1) / N;
  MODULO(N) m1 *= inv_phi_N;
  return m1;
}

bn_t paillier_t::rerand(const bn_t& cipher) const {
  bn_t r = bn_t::rand(N);

  bn_t rn;
  if (has_private) {
    rn = crt_enc.compute_power(r, NN);
  } else {
    cb_assert(mod_t::coprime(r, N));
    MODULO(NN) rn = r.pow(N);
  }
  MODULO(NN) rn *= cipher;
  return rn;
}

bn_t paillier_t::get_cipher_randomness(const bn_t& plain, const bn_t& cipher) const {
  bn_t c;
  bn_t result;
  bn_t temp = mod_t::N_inv_mod_phiN_2048(N, phi_N);  // temp = 1 / N mod phi_N

  MODULO(NN) { c = cipher / (plain * N + 1); }

  MODULO(N) { result = c.pow(temp); }

  return result;
}

error_t paillier_t::verify_cipher(const mod_t& N, const mod_t& NN, const bn_t& cipher) {
  error_t rv = UNINITIALIZED_ERROR;
  if (rv = coinbase::crypto::check_open_range(0, cipher, NN)) return rv;
  if (!mod_t::coprime(cipher, N)) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

error_t paillier_t::batch_verify_ciphers(const bn_t* ciphers, int n) const {
  if (n == 0) return SUCCESS;
  error_t rv = UNINITIALIZED_ERROR;
  for (int i = 0; i < n; i++) {
    if (rv = coinbase::crypto::check_open_range(0, ciphers[i], NN)) return rv;
  }

  bn_t prod = ciphers[0];
  for (int i = 1; i < n; i++) {
    MODULO(N) prod *= ciphers[i];
  }
  if (!mod_t::coprime(prod, N)) return coinbase::error(E_CRYPTO);

  return SUCCESS;
}

}  // namespace coinbase::crypto
