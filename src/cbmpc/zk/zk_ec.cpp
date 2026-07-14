#include <cbmpc/internal/crypto/base_mod.h>
#include <cbmpc/internal/crypto/lagrange.h>
#include <cbmpc/internal/crypto/ro.h>
#include <cbmpc/internal/zk/zk_ec.h>

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
  if (rv = params.check()) return rv;
  int rho = params.rho;
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
  std::vector<bn_t> sigmas(rho);

  for (int i = 0; i < rho; i++) {
    if (rv = crypto::check_right_open_range(0, z[i], q)) return rv;

    bn_t& sigma = sigmas[i];
    sigma = bn_t::rand_bitlen(SEC_P_STAT);
    MODULO(q) {
      z_sum += sigma * z[i];
      e_sum += sigma * bn_t(e[i]);
    }

    uint32_t h = hash32bit_for_zk_fischlin(common_hash, i, e[i], z[i]) & b_mask;
    if (h != 0) return coinbase::error(E_CRYPTO, "uc_dl_t::verify: zk_fischlin hash not equal zero");
  }

  A_sum = crypto::sum_mul(A, sigmas);
  if (A_sum != z_sum * G - e_sum * Q) return coinbase::error(E_CRYPTO, "uc_dl_t::verify: A != z * G - e * Q");
  return SUCCESS;
}

namespace {

bn_t to_bn(const bn_t& value) { return value; }
bn_t to_bn(const bn256_t& value) { return static_cast<bn_t>(value); }

void add_mod(bn_t& result, const bn_t& a, const bn_t& b, const mod_t& q) {
  int res = bn_mod_add_fixed_top(result, a, b, q.value());
  cb_assert(res && "modular addition failed");
}

void add_mod(bn256_t& result, const bn256_t& a, const bn256_t& b, const mod_t& q) {
  MODULO(q) { result = a + b; }
}

template <typename BN>
void prove_batch_dl(uc_batch_dl_finite_difference_impl_t& proof, const std::vector<ecc_point_t>& Q,
                    const std::vector<bn_t>& w, mem_t session_id, uint64_t aux) {
  const int n = int(w.size());
  std::vector<BN> r(proof.params.rho);
  const ecurve_t curve = Q[0].get_curve();
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  std::vector<BN> pw0 = {BN(0)};
  std::vector<BN> pw1;
  for (int j = 0; j < n; j++) {
    cb_assert(q.is_in_range(w[j]) && "w[j] < q");
    if ((j % 2) == 0)
      pw1.push_back(BN(w[j]));
    else
      pw0.push_back(BN(w[j]));
  }

  const int rho = proof.params.rho;
  proof.R.resize(rho);
  proof.e.resize(rho);
  proof.z.resize(rho);

  buf_t common_hash;
  BN ri, z_tag;

  const int n_half = (n + 1) / 2;
  typename uc_batch_dl_finite_difference_impl_t::matrix_sum_impl_t<BN> matrix_sum(n);
  typename uc_batch_dl_finite_difference_impl_t::vector_sum_impl_t<BN> sum(n, proof.params.t);

  for (int ei = 0; ei <= n_half; ei++) {
    const BN ei_value(ei);
    BN ei_square;
    MODULO(q) { ei_square = ei_value * ei_value; }
    const BN alpha = crypto::horner_poly(q, pw0, ei_square);
    const BN beta = crypto::horner_poly(q, pw1, ei_square);
    MODULO(q) {
      const BN beta_times_ei = beta * ei_value;
      sum[ei] = matrix_sum[ei][0] = alpha + beta_times_ei;
      sum[-ei] = matrix_sum[-ei][0] = alpha - beta_times_ei;
    }
  }

  int k = n_half;
  std::vector<BN>* last = &matrix_sum[n_half];
  std::vector<BN>* current = &matrix_sum[n_half + 1];

  for (int i = 1; i <= n; i++) {
    for (int j = n_half - i; j >= -n_half; j--)
      MODULO(q) matrix_sum[j][i] = matrix_sum[j + 1][i - 1] - matrix_sum[j][i - 1];
  }
  matrix_sum[-n_half + 1][n] = matrix_sum[-n_half][n];
  for (int j = -n_half + 2; j <= n_half; j++) {
    matrix_sum[j][n] = matrix_sum[j - 1][n];
    for (int i = n - 1; i >= n_half - j + 1; i--)
      add_mod(matrix_sum[j][i], matrix_sum[j - 1][i], matrix_sum[j - 1][i + 1], q);
  }

  fischlin_prove(
      proof.params,
      // initialize
      [&]() {
        for (int i = 0; i < rho; i++) {
          r[i] = BN::rand(q);
          proof.R[i] = to_bn(r[i]) * G;
        }
        common_hash = crypto::ro::hash_string(G, Q, proof.R, session_id, aux).bitlen(2 * SEC_P_COM);
      },

      // response_begin
      [&](int i) {
        ri = r[i];
        const int ei = 0 - n_half;
        MODULO(q) { z_tag = ri + matrix_sum[ei][0]; }
      },

      // hash
      [&](int i, int try_number) -> uint32_t {
        const int ei = try_number - n_half;
        return hash32bit_for_zk_fischlin(common_hash, i, ei, z_tag);
      },

      // save
      [&](int i, int try_number) {
        proof.e[i] = try_number - n_half;
        proof.z[i] = to_bn(z_tag);
      },

      // response_next
      [&](int try_number) {
        const int ei = try_number - n_half;
        if (ei > k) {
          current->at(n) = last->at(n);
          for (int i = n - 1; i >= 0; i--) add_mod(current->at(i), last->at(i), last->at(i + 1), q);
          sum[ei] = current->at(0);
          std::swap(current, last);
          k++;
        }
        add_mod(z_tag, ri, sum[ei], q);
      });
}

}  // namespace

void uc_batch_dl_finite_difference_impl_t::prove(const std::vector<ecc_point_t>& Q, const std::vector<bn_t>& w,
                                                 mem_t session_id, uint64_t aux) {
  cb_assert(!Q.empty() && Q.size() == w.size());
  const int n = int(w.size());
  if (n <= 28) {
    params.rho = 43;
    params.b = 3 + int_log2(n);
  } else {
    params.rho = 64;
    params.b = 2 + int_log2(n);
  }
  params.t = params.b + 5;

  if (Q[0].get_curve().order().get_bin_size() == 32)
    prove_batch_dl<bn256_t>(*this, Q, w, session_id, aux);
  else
    prove_batch_dl<bn_t>(*this, Q, w, session_id, aux);
}

error_t uc_batch_dl_finite_difference_impl_t::verify(const std::vector<ecc_point_t>& Q, mem_t session_id,
                                                     uint64_t aux) const {
  error_t rv = UNINITIALIZED_ERROR;
  int n = int(Q.size());
  if (n <= 0) return coinbase::error(E_CRYPTO, "uc_batch_dl_t::verify: Q.size() <= 0");
  crypto::vartime_scope_t vartime_scope;
  const int b_minus_log2n = params.b - int_log2(n);
  if (rv = params.check_with_effective_b(b_minus_log2n)) return rv;
  int rho = params.rho;
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
  for (int i = 0; i < rho; i++) {
    if (rv = curve.check(R[i]))
      return coinbase::error(rv, "uc_batch_dl_finite_difference_impl_t::verify: R[i] is not on the curve");
  }
  buf_t common_hash = crypto::ro::hash_string(G, Q, R, session_id, aux).bitlen(2 * SEC_P_COM);

  std::vector<ecc_point_t> PQ(n + 1);
  PQ[0] = curve.infinity();
  for (int i = 0; i < n; i++) PQ[i + 1] = Q[i];

  for (int i = 0; i < rho; i++) {
    if (rv = crypto::check_right_open_range(0, z[i], q)) return rv;

    ecc_point_t R_test = z[i] * G - crypto::horner_poly_small_vartime(PQ, e[i]);
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

  if (rv = crypto::check_right_open_range(0, e, q)) return rv;
  if (rv = crypto::check_right_open_range(0, z, q)) return rv;

  ecc_point_t X = z * G - e * A;
  ecc_point_t Y = z * Q - e * B;

  bn_t e_tag = crypto::ro::hash_number(G, Q, A, B, X, Y, session_id, aux).mod(q);
  if (e_tag != e) return coinbase::error(E_CRYPTO, "dh_t::verify: e does not match");
  return SUCCESS;
}

}  // namespace coinbase::zk
