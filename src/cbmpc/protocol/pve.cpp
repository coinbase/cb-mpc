#include "pve.h"

#include <cbmpc/core/buf.h>
#include <cbmpc/crypto/base.h>

namespace coinbase::mpc {

ec_pve_t::ec_pve_t() : base_pke(pve_base_pke_unified()) {}

void ec_pve_t::encrypt(const void* ek, mem_t label, ecurve_t curve, const bn_t& _x) {
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
  buf_t inner_label = genPVELabelWithPoint(label, Q);

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

    base_pke.encrypt(ek, inner_label, x0.to_bin(), rho0, c0[i]);
    X0[i] = x0 * G;
    base_pke.encrypt(ek, inner_label, x1.to_bin(), rho1, c1[i]);
    X1[i] = Q - X0[i];

    x_rows[i] = x1;  // output. will be cleared out if later bi == 0
  }

  b = crypto::ro::hash_string(Q, label, c0, c1, X0, X1).bitlen(kappa);

  for (int i = 0; i < kappa; i++) {
    bool bi = b.get_bit(i);
    r[i] = bi ? r1[i] : r0[i];
    c[i] = bi ? c0[i] : c1[i];
    if (!bi) x_rows[i] = 0;  // clear the output
  }
}

error_t ec_pve_t::verify(const void* ek, const ecc_point_t& Q, mem_t label) const {
  error_t rv = UNINITIALIZED_ERROR;
  ecurve_t curve = Q.get_curve();
  if (rv = curve.check(Q)) return coinbase::error(rv, "ec_pve_t::verify: check Q failed");
  if (Q != this->Q) return coinbase::error(E_CRYPTO, "public key (Q) mismatch");
  if (label != L) return coinbase::error(E_CRYPTO, "label mismatch");
  buf_t inner_label = genPVELabelWithPoint(label, Q);

  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  buf_t c0[kappa];
  buf_t c1[kappa];
  ecc_point_t X0[kappa];
  ecc_point_t X1[kappa];

  for (int i = 0; i < kappa; i++) {
    bool bi = b.get_bit(i);

    crypto::drbg_aes_ctr_t drbg(r[i]);

    bn_t xi = x_rows[i];
    if (!bi) xi = drbg.gen_bn(q);
    buf_t rho = drbg.gen(rho_size);

    X0[i] = xi * G;
    X1[i] = Q - X0[i];
    base_pke.encrypt(ek, inner_label, xi.to_bin(), rho, c0[i]);
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

error_t ec_pve_t::restore_from_decrypted(int row_index, mem_t decrypted_x_buf, ecurve_t curve, bn_t& x_value) const {
  const mod_t& q = curve.order();
  const auto& G = curve.generator();

  bool bi = b.get_bit(row_index);
  bn_t x_bi_bar = bn_t::from_bin(decrypted_x_buf);
  bn_t x_bi = x_rows[row_index];

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

error_t ec_pve_t::decrypt(const void* dk, const void* ek, mem_t label, ecurve_t curve, bn_t& x_out,
                          bool skip_verify) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (!skip_verify && (rv = verify(ek, Q, label))) return rv;

  buf_t inner_label = genPVELabelWithPoint(label, Q);

  for (int i = 0; i < kappa; i++) {
    buf_t x_buf;
    if (rv = base_pke.decrypt(dk, inner_label, c[i], x_buf)) return rv;
    if (restore_from_decrypted(i, x_buf, curve, x_out) == SUCCESS) {
      return SUCCESS;
    }
  }

  x_out = 0;
  return coinbase::error(E_CRYPTO);
}

}  // namespace coinbase::mpc
