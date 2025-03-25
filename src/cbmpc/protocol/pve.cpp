#include "pve.h"

#include <cbmpc/core/buf.h>
#include <cbmpc/crypto/base.h>

namespace coinbase::mpc {

// -------- Helper functions ----------

template <typename T>
static buf_t generateLabelWithPoint(mem_t label, const T& Q) {
  return buf_t(label) + "-" + strext::to_hex(crypto::sha256_t::hash(Q));
}

// -------- Basic Version ----------

template <class PKI_T>
void ec_pve_t<PKI_T>::encrypt(const PK_T& key, mem_t label, ecurve_t curve, const bn_t& _x) {
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  bn_t bn_x = _x % q;
  Q = bn_x * G;
  buf128_t r0[kappa];
  buf128_t r1[kappa];
  buf_t c0[kappa];
  buf_t c1[kappa];
  ecc_point_t X0[kappa];
  ecc_point_t X1[kappa];
  L = buf_t(label);
  buf_t inner_label = generateLabelWithPoint(label, Q);

  for (int i = 0; i < kappa; i++) {
    bn_t x0, x1;

    crypto::gen_random(r0[i]);
    crypto::gen_random(r1[i]);
    crypto::drbg_aes_ctr_t drbg0(r0[i]);
    crypto::drbg_aes_ctr_t drbg1(r1[i]);

    x0 = drbg0.gen_bn(q);
    buf_t rho0 = drbg0.gen(rho_size);

    MODULO(q) x1 = bn_x - x0;
    buf_t rho1 = drbg1.gen(rho_size);

    c0[i] = pve_base_encrypt<PKI_T>(key, inner_label, x0.to_bin(), rho0);
    X0[i] = x0 * G;
    c1[i] = pve_base_encrypt<PKI_T>(key, inner_label, x1.to_bin(), rho1);
    X1[i] = Q - X0[i];

    x[i] = x1;  // output. will be clear out if later on bi == 0
  }

  b = crypto::ro::hash_string(Q, label, c0, c1, X0, X1).bitlen(kappa);

  for (int i = 0; i < kappa; i++) {
    bool bi = b.get_bit(i);
    r[i] = bi ? r1[i] : r0[i];
    c[i] = bi ? c0[i] : c1[i];
    if (!bi) x[i] = 0;  // clear the output
  }
}

template <class PKI_T>
error_t ec_pve_t<PKI_T>::verify(const PK_T& key, const ecc_point_t& Q, mem_t label) const {
  error_t rv = UNINITIALIZED_ERROR;
  ecurve_t curve = Q.get_curve();
  if (rv = curve.check(Q)) return coinbase::error(rv, "ec_pve_t::verify: check Q failed");
  if (Q != this->Q) return coinbase::error(E_CRYPTO, "public key (Q) mismatch");
  if (label != L) return coinbase::error(E_CRYPTO, "label mismatch");
  buf_t inner_label = generateLabelWithPoint(label, Q);

  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  buf_t c0[kappa];
  buf_t c1[kappa];
  ecc_point_t X0[kappa];
  ecc_point_t X1[kappa];

  for (int i = 0; i < kappa; i++) {
    bool bi = b.get_bit(i);

    crypto::drbg_aes_ctr_t drbg(r[i]);

    bn_t xi = x[i];
    if (!bi) xi = drbg.gen_bn(q);
    buf_t rho = drbg.gen(rho_size);

    X0[i] = xi * G;
    X1[i] = Q - X0[i];
    c0[i] = pve_base_encrypt<PKI_T>(key, inner_label, xi.to_bin(), rho);
    c1[i] = c[i];

    if (bi) {
      std::swap(X0[i], X1[i]);
      std::swap(c0[i], c1[i]);
    }
  }

  buf_t b_tag = crypto::ro::hash_string(Q, label, c0, c1, X0, X1).bitlen(kappa);
  if (b_tag != b) return coinbase::error(E_CRYPTO, "b' != b");
  return SUCCESS;
}

template <class PKI_T>
error_t ec_pve_t<PKI_T>::restore_from_decrypted(int row_index, mem_t decrypted_x_buf, ecurve_t curve,
                                                bn_t& x_value) const {
  const mod_t& q = curve.order();
  const auto& G = curve.generator();

  bool bi = b.get_bit(row_index);
  bn_t x_bi_bar = bn_t::from_bin(decrypted_x_buf);
  bn_t x_bi = x[row_index];

  if (!bi) {
    crypto::drbg_aes_ctr_t drbg0(r[row_index]);
    x_bi = drbg0.gen_bn(q);
  }

  MODULO(q) x_value = x_bi_bar + x_bi;

  if (x_value * G != Q) {
    x_value = 0;
    return coinbase::error(E_CRYPTO);
  }
  return SUCCESS;
}

template <class PKI_T>
error_t ec_pve_t<PKI_T>::decrypt(const SK_T& key, mem_t label, ecurve_t curve, bn_t& x_out, bool skip_verify) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (!skip_verify && (rv = verify(key.pub(), Q, label))) return rv;

  buf_t inner_label = generateLabelWithPoint(label, Q);

  for (int i = 0; i < kappa; i++) {
    buf_t x_buf;
    if (rv = pve_base_decrypt<PKI_T>(key, inner_label, c[i], x_buf)) return rv;
    if (restore_from_decrypted(i, x_buf, curve, x_out) == SUCCESS) {
      return SUCCESS;
    }
  }

  x_out = 0;
  return coinbase::error(E_CRYPTO);
}

template class ec_pve_t<crypto::hybrid_cipher_t>;
template class ec_pve_t<crypto::rsa_kem_t>;
template class ec_pve_t<crypto::ecies_t>;

// -------- Batch Version ----------

template <class PKI_T>
void ec_pve_batch_t<PKI_T>::encrypt(const PK_T& key, mem_t label, ecurve_t curve, const std::vector<bn_t>& _x) {
  cb_assert(int(_x.size()) == n);

  const mod_t& q = curve.order();
  const auto& G = curve.generator();
  int curve_size = curve.size();
  std::vector<bn_t> x(n);

  for (int j = 0; j < n; j++) {
    x[j] = _x[j] % q;
    Q[j] = x[j] * G;
  }

  buf128_t r01[kappa], r02[kappa];
  buf128_t r1[kappa];
  buf_t c0[kappa];
  buf_t c1[kappa];
  std::vector<ecc_point_t> X0[kappa];
  std::vector<ecc_point_t> X1[kappa];
  L = buf_t(label);
  buf_t inner_label = generateLabelWithPoint(label, Q);

  for (int i = 0; i < kappa; i++) {
    X0[i].resize(n);
    X1[i].resize(n);

    crypto::gen_random(r01[i]);
    crypto::gen_random(r02[i]);
    crypto::gen_random(r1[i]);
    crypto::drbg_aes_ctr_t drbg01(r01[i]);
    crypto::drbg_aes_ctr_t drbg02(r02[i]);
    crypto::drbg_aes_ctr_t drbg1(r1[i]);

    buf_t x0_source_bin = drbg01.gen(n * (curve_size + coinbase::bits_to_bytes(SEC_P_STAT)));
    buf_t rho0 = drbg02.gen(rho_size);
    buf_t rho1 = drbg1.gen(rho_size);

    std::vector<bn_t> x0 = bn_t::vector_from_bin(x0_source_bin, n, curve_size + coinbase::bits_to_bytes(SEC_P_STAT), q);
    std::vector<bn_t> x1(n);
    for (int j = 0; j < n; j++) {
      MODULO(q) x1[j] = x[j] - x0[j];

      X0[i][j] = x0[j] * G;
      X1[i][j] = Q[j] - X0[i][j];
    }

    buf_t x1_bin = bn_t::vector_to_bin(x1, curve_size);

    c0[i] = pve_base_encrypt<PKI_T>(key, inner_label, r01[i], rho0);
    c1[i] = pve_base_encrypt<PKI_T>(key, inner_label, x1_bin, rho1);
    rows[i].x_bin = x1_bin;  // some of these will be reset to zero later based on `bi`
  }

  b = crypto::ro::hash_string(Q, label, c0, c1, X0, X1).bitlen(kappa);

  for (int i = 0; i < kappa; i++) {
    bool bi = b.get_bit(i);
    rows[i].r = bi ? r1[i] : (r01[i] + r02[i]);
    rows[i].c = bi ? c0[i] : c1[i];
    if (!bi) rows[i].x_bin.free();
  }
}

template <class PKI_T>
error_t ec_pve_batch_t<PKI_T>::verify(const PK_T& key, const std::vector<ecc_point_t>& Q, mem_t label) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (int(Q.size()) != n) return coinbase::error(E_BADARG);

  // This verifies that the input Q values are the same as backed up Q values (step 2 of spec)
  // and that the input Q values are on curve (step 1 of spec) assuming backed up one is on curve
  if (Q != this->Q) return coinbase::error(E_CRYPTO, "public keys (Qs) mismatch");
  ecurve_t curve = Q[0].get_curve();
  for (int i = 0; i < n; i++) {
    if (rv = curve.check(Q[i])) return coinbase::error(rv, "ec_pve_t::verify: check Q[i] failed");
  }
  if (label != this->L) return coinbase::error(E_CRYPTO);
  buf_t inner_label = generateLabelWithPoint(label, Q);

  const auto& G = curve.generator();
  const mod_t& q = curve.order();
  int curve_size = curve.size();

  buf_t c0[kappa];
  buf_t c1[kappa];
  std::vector<ecc_point_t> X0[kappa];
  std::vector<ecc_point_t> X1[kappa];

  for (int i = 0; i < kappa; i++) {
    bool bi = b.get_bit(i);
    // xi is x^0_i or x^1_i depends on bi == 1 or 0.
    // Note that we always have X[0][i] = xi * G, then X[0] and X[1] if xi is x^1_i.
    std::vector<bn_t> xi;
    if (bi) {
      c0[i] = rows[i].c;

      xi = bn_t::vector_from_bin(rows[i].x_bin, n, curve_size, q);

      crypto::drbg_aes_ctr_t drbg1(rows[i].r);
      buf_t rho1 = drbg1.gen(rho_size);

      c1[i] = pve_base_encrypt<PKI_T>(key, inner_label, bn_t::vector_to_bin(xi, curve_size), rho1);
    } else {
      c1[i] = rows[i].c;

      crypto::drbg_aes_ctr_t drbg01(rows[i].r.take(16));
      buf_t x0_source_bin = drbg01.gen(n * (curve_size + coinbase::bits_to_bytes(SEC_P_STAT)));
      xi = bn_t::vector_from_bin(x0_source_bin, n, curve_size + coinbase::bits_to_bytes(SEC_P_STAT), q);

      crypto::drbg_aes_ctr_t drbg02(rows[i].r.skip(16));
      buf_t rho0 = drbg02.gen(rho_size);

      c0[i] = pve_base_encrypt<PKI_T>(key, inner_label, rows[i].r.take(16), rho0);
    }

    X0[i].resize(n);
    X1[i].resize(n);
    for (int j = 0; j < n; j++) {
      X0[i][j] = xi[j] * G;
      X1[i][j] = Q[j] - X0[i][j];
    }

    if (bi) std::swap(X0[i], X1[i]);
  }

  // If a different SEC_P_COM is used, change `bitlen128` to `bitlen(SEC_P_COM)`
  cb_assert(SEC_P_COM == 128);
  buf128_t b_tag = crypto::ro::hash_string(Q, label, c0, c1, X0, X1).bitlen128();
  if (b_tag != b) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

template <class PKI_T>
error_t ec_pve_batch_t<PKI_T>::restore_from_decrypted(int row_index, mem_t decrypted_x_buf, ecurve_t curve,
                                                      std::vector<bn_t>& x) const {
  if (row_index > kappa) return coinbase::error(E_BADARG);

  const mod_t& q = curve.order();
  const auto& G = curve.generator();
  int curve_size = curve.size();

  buf_t r01, x1_bin;
  bool bi = b.get_bit(row_index);
  if (bi) {
    x1_bin = rows[row_index].x_bin;
    r01 = decrypted_x_buf;
  } else {
    x1_bin = decrypted_x_buf;
    r01 = rows[row_index].r.take(16);
  }

  crypto::drbg_aes_ctr_t drbg01(r01);  // decrypted_x_buf = r01
  buf_t x0_source_bin = drbg01.gen(n * (curve_size + coinbase::bits_to_bytes(SEC_P_STAT)));
  std::vector<bn_t> x0 = bn_t::vector_from_bin(x0_source_bin, n, curve_size + coinbase::bits_to_bytes(SEC_P_STAT), q);

  std::vector<bn_t> x1 = bn_t::vector_from_bin(x1_bin, n, curve_size, q);

  for (int i = 0; i < n; i++) {
    MODULO(q) x[i] = x0[i] + x1[i];
    if (Q[i] != x[i] * G) return coinbase::error(E_CRYPTO);
  }

  return SUCCESS;
}

template <class PKI_T>
error_t ec_pve_batch_t<PKI_T>::decrypt(const SK_T& key, mem_t label, ecurve_t curve, std::vector<bn_t>& xs,
                                       bool skip_verify) const {
  error_t rv = UNINITIALIZED_ERROR;
  xs.resize(n);
  if (!skip_verify && (rv = verify(key.pub(), Q, label))) return rv;

  if (label != this->L) return coinbase::error(E_CRYPTO);
  buf_t inner_label = generateLabelWithPoint(label, Q);

  for (int i = 0; i < kappa; i++) {
    buf_t x_buf;
    if (rv = pve_base_decrypt<PKI_T>(key, inner_label, rows[i].c, x_buf)) return rv;
    if (restore_from_decrypted(i, x_buf, curve, xs) == SUCCESS) return SUCCESS;
  }

  xs.clear();
  return coinbase::error(E_CRYPTO);
}

template class ec_pve_batch_t<crypto::hybrid_cipher_t>;
template class ec_pve_batch_t<crypto::rsa_kem_t>;
template class ec_pve_batch_t<crypto::ecies_t>;

}  // namespace coinbase::mpc
