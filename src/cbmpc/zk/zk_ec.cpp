#include "zk_ec.h"

#include <cbmpc/crypto/base_mod.h>
#include <cbmpc/crypto/lagrange.h>
#include <cbmpc/crypto/ro.h>

namespace coinbase::zk {

void uc_dl_t::prove(const ecc_point_t& Q, const bn_t& w, mem_t session_id, uint64_t aux) {
  std::vector<bn_t> r(params.rho);
  ecurve_t curve = Q.get_curve();
  const auto& G = curve.generator();
  const mod_t& q = curve.order();
  int rho = params.rho;

  cb_assert(w < q && "w exceeds the order of the curve");

  A.resize(rho);
  e.resize(rho);
  z.resize(rho);

  bn_t z_tag;
  bn_t q_value = bn_t(q);
  buf_t common_hash;

  fischlin_prove(
      params,
      // initialize
      [&]() {
        for (int i = 0; i < rho; i++) {
          r[i] = bn_t::rand(q);
          A[i] = r[i] * G;
        }
        common_hash = crypto::ro::hash_string(G, Q, A, session_id, aux).bitlen(2 * SEC_P_COM);
      },

      // response_begin
      [&](int i) { z_tag = r[i]; },

      // hash
      [&](int i, int e_tag) -> uint32_t { return hash32bit_for_zk_fischlin(common_hash, i, e_tag, z_tag); },

      // save
      [&](int i, int e_tag) {
        e[i] = e_tag;
        z[i] = z_tag;
      },

      // response_next
      [&](int e_tag) {
        int res = bn_mod_add_fixed_top(z_tag, z_tag, w, q_value);
        cb_assert(res && "z' = z' + w (mod q) failed");
      });
}

error_t uc_dl_t::verify(const ecc_point_t& Q, mem_t session_id, uint64_t aux) const {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::vartime_scope_t vartime_scope;
  int rho = params.rho;
  if (params.b * rho < SEC_P_COM) return coinbase::error(E_CRYPTO, "uc_dl_t::verify: b * rho < SEC_P_COM");
  if (int(A.size()) != rho) return coinbase::error(E_CRYPTO, "uc_dl_t::verify: A.size() != rho");
  if (int(e.size()) != rho) return coinbase::error(E_CRYPTO, "uc_dl_t::verify: e.size() != rho");
  if (int(z.size()) != rho) return coinbase::error(E_CRYPTO, "uc_dl_t::verify: z.size() != rho");

  ecurve_t curve = Q.get_curve();
  const mod_t& q = curve.order();
  if (rv = curve.check(Q)) return coinbase::error(rv, "uc_dl_t::verify: Q is not on the curve");
  for (int i = 0; i < rho; i++) {
    if (rv = curve.check(A[i])) return coinbase::error(rv, "uc_dl_t::verify: A[i] is not on the curve");
  }

  const auto& G = curve.generator();
  uint32_t b_mask = params.b_mask();
  buf_t common_hash = crypto::ro::hash_string(G, Q, A, session_id, aux).bitlen(2 * SEC_P_COM);

  bn_t z_sum = 0;
  bn_t e_sum = 0;
  ecc_point_t A_sum = curve.infinity();

  for (int i = 0; i < rho; i++) {
    bn_t sigma = bn_t::rand_bitlen(SEC_P_STAT);
    MODULO(q) {
      z_sum += sigma * z[i];
      e_sum += sigma * bn_t(e[i]);
    }
    A_sum += sigma * A[i];

    uint32_t h = hash32bit_for_zk_fischlin(common_hash, i, e[i], z[i]) & b_mask;
    if (h != 0) return coinbase::error(E_CRYPTO, "uc_dl_t::verify: zk_fischlin hash not equal zero");
  }

  if (A_sum != z_sum * G - e_sum * Q) return coinbase::error(E_CRYPTO, "uc_dl_t::verify: A != z * G - e * Q");
  return SUCCESS;
}

void uc_batch_dl_finite_difference_impl_t::prove(const std::vector<ecc_point_t>& Q, const std::vector<bn_t>& w,
                                                 mem_t session_id, uint64_t aux) {
  int n = int(w.size());
  if (n <= 28) {
    params.rho = 43;
    params.b = 3 + int_log2(n);
  } else {
    params.rho = 64;
    params.b = 2 + int_log2(n);
  }
  params.t = params.b + 5;

  std::vector<bn_t> r(params.rho);
  ecurve_t curve = Q[0].get_curve();
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  std::vector<bn_t> pw0;
  pw0.push_back(0);
  std::vector<bn_t> pw1;
  for (int j = 0; j < n; j++) {
    cb_assert(w[j] < q && "w[j] exceeds the order of the curve");
    if ((j % 2) == 0)
      pw1.push_back(w[j]);
    else
      pw0.push_back(w[j]);
  }

  int rho = params.rho;
  R.resize(rho);
  e.resize(rho);
  z.resize(rho);

  bn_t q_value = bn_t(q);
  buf_t common_hash;
  bn_t ri, z_tag;

  int n_half = (n + 1) / 2;
  matrix_sum_t matrix_sum(n);
  vector_sum_t sum(n, params.t);

  for (int ei = 0; ei <= n_half; ei++) {
    bn_t ei_square = ei * ei;
    bn_t alpha = crypto::horner_poly(q, pw0, ei_square);
    bn_t beta = crypto::horner_poly(q, pw1, ei_square);
    MODULO(q) {
      sum[ei] = matrix_sum[ei][0] = alpha + beta * ei;    // for positive
      sum[-ei] = matrix_sum[-ei][0] = alpha - beta * ei;  // for negative
    }
  }
  int k = n_half;
  std::vector<bn_t>* last = &matrix_sum[n_half];
  std::vector<bn_t>* current = &matrix_sum[n_half + 1];

  for (int i = 1; i <= n; i++) {
    for (int j = n_half - i; j >= -n_half; j--)
      MODULO(q) matrix_sum[j][i] = matrix_sum[j + 1][i - 1] - matrix_sum[j][i - 1];
  }
  matrix_sum[-n_half + 1][n] = matrix_sum[-n_half][n];
  for (int j = -n_half + 2; j <= n_half; j++) {
    matrix_sum[j][n] = matrix_sum[j - 1][n];
    for (int i = n - 1; i >= n_half - j + 1; i--) {
      int res = bn_mod_add_fixed_top(matrix_sum[j][i], matrix_sum[j - 1][i], matrix_sum[j - 1][i + 1], q_value);
      cb_assert(res && "matrix_sum[j][i] = matrix_sum[j - 1][i] + matrix_sum[j - 1][i + 1] (mod q) failed");
    }
  }

  fischlin_prove(
      params,
      // initialize
      [&]() {
        for (int i = 0; i < rho; i++) {
          r[i] = bn_t::rand(q);
          R[i] = r[i] * G;
        }
        common_hash = crypto::ro::hash_string(G, Q, R, session_id, aux).bitlen(2 * SEC_P_COM);
      },

      // response_begin
      [&](int i) {
        ri = r[i];
        int32_t ei = 0 - n_half;
        MODULO(q) { z_tag = ri + matrix_sum[ei][0]; }
      },

      // hash
      [&](int i, int try_number) -> uint32_t {
        int ei = try_number - n_half;
        return hash32bit_for_zk_fischlin(common_hash, i, ei, z_tag);
      },

      // save
      [&](int i, int try_number) {
        int ei = try_number - n_half;
        e[i] = ei;
        z[i] = z_tag;
      },

      // response_next
      [&](int try_number) {
        int ei = try_number - n_half;
        if (ei > k) {
          current->at(n) = last->at(n);
          for (int i = n - 1; i >= 0; i--) {
            int res = bn_mod_add_fixed_top(current->at(i), last->at(i), last->at(i + 1), q_value);
            cb_assert(res);
          }
          sum[ei] = current->at(0);
          std::swap(current, last);
          k++;
        }

        int res = bn_mod_add_fixed_top(z_tag, ri, sum[ei], q_value);
        cb_assert(res && "z' = z' + sum[ei] (mod q) failed");
      });
}

error_t uc_batch_dl_finite_difference_impl_t::verify(const std::vector<ecc_point_t>& Q, mem_t session_id,
                                                     uint64_t aux) const {
  error_t rv = UNINITIALIZED_ERROR;
  int n = int(Q.size());
  crypto::vartime_scope_t vartime_scope;
  int rho = params.rho;
  if (rho * (params.b - int_log2(n)) < SEC_P_COM)
    return coinbase::error(E_CRYPTO,
                           "uc_batch_dl_finite_difference_impl_t::verify: rho * (params.b - int_log2(n)) < SEC_P_COM");
  if (int(R.size()) != rho)
    return coinbase::error(E_CRYPTO, "uc_batch_dl_finite_difference_impl_t::verify: R.size() != rho");
  if (int(e.size()) != rho)
    return coinbase::error(E_CRYPTO, "uc_batch_dl_finite_difference_impl_t::verify: e.size() != rho");
  if (int(z.size()) != rho)
    return coinbase::error(E_CRYPTO, "uc_batch_dl_finite_difference_impl_t::verify: z.size() != rho");

  ecurve_t curve = Q[0].get_curve();
  const mod_t& q = curve.order();

  for (int j = 0; j < n; j++) {
    if (rv = curve.check(Q[j]))
      return coinbase::error(rv, "uc_batch_dl_finite_difference_impl_t::verify: Q[j] is not on the curve");
  }

  const auto& G = curve.generator();
  uint32_t b_mask = params.b_mask();
  buf_t common_hash = crypto::ro::hash_string(G, Q, R, session_id, aux).bitlen(2 * SEC_P_COM);

  std::vector<ecc_point_t> PQ(n + 1);
  PQ[0] = curve.infinity();
  for (int i = 0; i < n; i++) PQ[i + 1] = Q[i];

  for (int i = 0; i < rho; i++) {
    if (rv = curve.check(R[i]))
      return coinbase::error(rv, "uc_batch_dl_finite_difference_impl_t::verify: R[i] is not on the curve");

    bn_t ei = e[i];
    if (ei < 0) ei += q;

    ecc_point_t R_test = z[i] * G - crypto::horner_poly(PQ, ei);
    if (R[i] != R_test)
      return coinbase::error(E_CRYPTO, "uc_batch_dl_finite_difference_impl_t::verify: R[i] does not match");

    uint32_t h = hash32bit_for_zk_fischlin(common_hash, i, e[i], z[i]) & b_mask;
    if (h != 0)
      return coinbase::error(E_CRYPTO, "uc_batch_dl_finite_difference_impl_t::verify: zk_fischlin hash not equal zero");
  }

  return SUCCESS;
}

void dh_t::prove(const ecc_point_t& Q, const ecc_point_t& A, const ecc_point_t& B, const bn_t& w, mem_t session_id,
                 uint64_t aux) {
  ecurve_t curve = Q.get_curve();
  const auto& G = curve.generator();
  const mod_t& q = curve.order();
  bn_t r = curve.get_random_value();

  cb_assert(w < q && "w exceeds the order of the curve");

  ecc_point_t X = r * G;
  ecc_point_t Y = r * Q;

  e = crypto::ro::hash_number(G, Q, A, B, X, Y, session_id, aux).mod(q);

  MODULO(q) { z = r + e * w; }
}

error_t dh_t::verify(const ecc_point_t& Q, const ecc_point_t& A, const ecc_point_t& B, mem_t session_id,
                     uint64_t aux) const {
  error_t rv = UNINITIALIZED_ERROR;

  crypto::vartime_scope_t vartime_scope;
  ecurve_t curve = Q.get_curve();
  if (rv = curve.check(Q)) return coinbase::error(rv, "dh_t::verify: Q is not on the curve");
  if (rv = curve.check(A)) return coinbase::error(rv, "dh_t::verify: A is not on the curve");
  if (rv = curve.check(B)) return coinbase::error(rv, "dh_t::verify: B is not on the curve");

  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  ecc_point_t X = z * G - e * A;
  ecc_point_t Y = z * Q - e * B;

  bn_t e_tag = crypto::ro::hash_number(G, Q, A, B, X, Y, session_id, aux).mod(q);
  if (e_tag != e) return coinbase::error(E_CRYPTO, "dh_t::verify: e does not match");
  return SUCCESS;
}

}  // namespace coinbase::zk
