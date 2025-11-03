#include "pve_batch.h"

namespace coinbase::mpc {

ec_pve_batch_t::ec_pve_batch_t(int batch_count) : base_pke(pve_base_pke_unified()), n(batch_count), rows(kappa), Q(n) {}

void ec_pve_batch_t::encrypt(const void* ek, mem_t label, ecurve_t curve, const std::vector<bn_t>& _x) {
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
  buf_t inner_label = genPVELabelWithPoint(label, Q);

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

    base_pke.encrypt(ek, inner_label, r01[i], rho0, c0[i]);
    base_pke.encrypt(ek, inner_label, x1_bin, rho1, c1[i]);
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

error_t ec_pve_batch_t::verify(const void* ek, const std::vector<ecc_point_t>& Q, mem_t label) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (int(Q.size()) != n) return coinbase::error(E_BADARG);

  // This verifies that the input Q values are the same as backed up Q values (step 2 of spec)
  // and that the input Q values are on curve (step 1 of spec) assuming backed up one is on curve
  ecurve_t curve = Q[0].get_curve();
  for (int i = 0; i < n; i++) {
    if (rv = curve.check(Q[i])) return coinbase::error(rv, "ec_pve_batch_t::verify: check Q[i] failed");
  }
  if (Q != this->Q) return coinbase::error(E_CRYPTO, "public keys (Qs) mismatch");
  if (label != this->L) return coinbase::error(E_CRYPTO);
  buf_t inner_label = genPVELabelWithPoint(label, Q);

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

      if (rows[i].r.size() != 16) return coinbase::error(E_CRYPTO);
      crypto::drbg_aes_ctr_t drbg1(rows[i].r);
      buf_t rho1 = drbg1.gen(rho_size);

      base_pke.encrypt(ek, inner_label, bn_t::vector_to_bin(xi, curve_size), rho1, c1[i]);
    } else {
      c1[i] = rows[i].c;

      if (rows[i].r.size() != 32) return coinbase::error(E_CRYPTO);
      crypto::drbg_aes_ctr_t drbg01(rows[i].r.take(16));
      buf_t x0_source_bin = drbg01.gen(n * (curve_size + coinbase::bits_to_bytes(SEC_P_STAT)));
      xi = bn_t::vector_from_bin(x0_source_bin, n, curve_size + coinbase::bits_to_bytes(SEC_P_STAT), q);

      crypto::drbg_aes_ctr_t drbg02(rows[i].r.skip(16));
      buf_t rho0 = drbg02.gen(rho_size);

      base_pke.encrypt(ek, inner_label, rows[i].r.take(16), rho0, c0[i]);
    }

    X0[i].resize(n);
    X1[i].resize(n);
    for (int j = 0; j < n; j++) {
      X0[i][j] = xi[j] * G;
      X1[i][j] = Q[j] - X0[i][j];
    }

    if (bi) std::swap(X0[i], X1[i]);
  }

  buf_t b_tag = crypto::ro::hash_string(Q, label, c0, c1, X0, X1).bitlen(SEC_P_COM);
  if (b_tag != b) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

error_t ec_pve_batch_t::restore_from_decrypted(int row_index, mem_t decrypted_x_buf, ecurve_t curve,
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
    if (rows[row_index].r.size() != 32) return coinbase::error(E_CRYPTO);
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

error_t ec_pve_batch_t::decrypt(const void* dk, const void* ek, mem_t label, ecurve_t curve, std::vector<bn_t>& xs,
                                bool skip_verify) const {
  error_t rv = UNINITIALIZED_ERROR;
  xs.resize(n);
  if (!skip_verify && (rv = verify(ek, Q, label))) return rv;

  if (label != this->L) return coinbase::error(E_CRYPTO);
  buf_t inner_label = genPVELabelWithPoint(label, Q);

  for (int i = 0; i < kappa; i++) {
    buf_t x_buf;
    if (rv = base_pke.decrypt(dk, inner_label, rows[i].c, x_buf)) return rv;
    if (restore_from_decrypted(i, x_buf, curve, xs) == SUCCESS) return SUCCESS;
  }

  xs.clear();
  return coinbase::error(E_CRYPTO);
}

}  // namespace coinbase::mpc