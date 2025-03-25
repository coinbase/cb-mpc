#include "lagrange.h"

namespace coinbase::crypto {

extern "C" BIGNUM* bn_wexpand(BIGNUM* a, int words);

/**
 * @notes:
 * - This is a more complicated version of the `lagrange_basis` defined below, since it is constant-time.
 */
void lagrange_basis(const bn_t& x, const std::vector<int>& pids, int current, const mod_t& q, bn_t& numerator,
                    bn_t& denominator) {
  int m = (int)pids.size();
  numerator = q.to_mont(1);
  denominator = 1;
  bn_wexpand(denominator, m / 2);

  bool neg = false;
  const bn_t& mod = q.value();

  bn_t num_delta = x;
  bn_t num_delta_mont;
  int old = 0;
  auto bn_ctx = bn_t::thread_local_storage_bn_ctx();
  auto mont_ctx = q.get_mont_ctx();

  for (int j = 0; j < m; j++) {
    if (current == pids[j]) continue;
    cb_assert(pids[j] > 0 && "pids must be positive");

    int denom_delta = current - pids[j];
    if (denom_delta < 0) {
      denom_delta = -denom_delta;
      neg = !neg;
    }

    BN_mul_word(denominator, denom_delta);  // denominator *= delta;

    BN_sub_word(num_delta, pids[j]);
    BN_add_word(num_delta, old);
    old = pids[j];

    BN_to_montgomery(num_delta_mont, num_delta, mont_ctx, bn_ctx);
    BN_mod_mul_montgomery(numerator, numerator, num_delta_mont, mont_ctx, bn_ctx);
  }

  BN_div(nullptr, denominator, denominator, mod, bn_ctx);
  if (neg) BN_sub(denominator, mod, denominator);

  BN_from_montgomery(numerator, numerator, mont_ctx, bn_ctx);
}

bn_t lagrange_basis(const bn_t& x, const std::vector<int>& pids, int current, const mod_t& q) {
  bn_t numerator, denominator;
  lagrange_basis(x, pids, current, q, numerator, denominator);

  auto bn_ctx = bn_t::thread_local_storage_bn_ctx();
  const bn_t& mod = q.value();
  BN_mod_inverse(denominator, denominator, mod,
                 bn_ctx);  // denominator = 1/denominator mod q;

  BN_mod_mul(numerator, numerator, denominator, mod,
             bn_ctx);  // numerator = numerator * denominator
  return numerator;
}

bn_t lagrange_basis(const bn_t& x, const std::vector<bn_t>& pids, const bn_t& current_pid, const mod_t& q) {
  vartime_scope_t vartime_scope;

  int m = (int)pids.size();
  bn_t numerator = 1;
  bn_t denominator = 1;

  for (int j = 0; j < m; j++) {
    const bn_t& pid = pids[j];
    cb_assert(pid > 0);

    if (current_pid == pid) continue;
    MODULO(q) numerator *= x - pid;
    MODULO(q) denominator *= current_pid - pid;
  }

  bn_t result;
  MODULO(q) result = numerator / denominator;
  return result;
}

bn_t lagrange_partial_interpolate(const bn_t& x, const std::vector<bn_t>& shares,
                                  const std::vector<bn_t>& pids_for_shares, const std::vector<bn_t>& all_pids,
                                  const mod_t& q) {
  cb_assert(pids_for_shares.size() == shares.size() && "shares and pids_for_shares must have the same size");
  cb_assert(all_pids.size() >= shares.size() && "all_pids must have at least as many elements as shares");
  int m = (int)shares.size();
  bn_t secret = 0;

  for (int i = 0; i < m; i++) {
    bn_t lambda = lagrange_basis(x, all_pids, pids_for_shares[i], q);
    MODULO(q) secret += lambda * shares[i];
  }

  return secret;
}

bn_t lagrange_interpolate(const bn_t& x, const std::vector<bn_t>& shares, const std::vector<bn_t>& pids,
                          const mod_t& q) {
  cb_assert(shares.size() == pids.size() && "shares and pids must have the same size");
  return lagrange_partial_interpolate(x, shares, pids, pids, q);
}

/* Lagrange in exponent */

ecc_point_t lagrange_partial_interpolate_exponent(const bn_t& x, const std::vector<ecc_point_t>& shares,
                                                  const std::vector<bn_t>& pids_for_shares,
                                                  const std::vector<bn_t>& all_pids) {
  cb_assert(shares.size() == pids_for_shares.size() && "shares and pids_for_shares must have the same size");
  cb_assert(all_pids.size() >= shares.size() && "all_pids must have at least as many elements as shares");
  int m = (int)shares.size();
  cb_assert(m > 0 && "shares must have at least one element");
  ecurve_t curve = shares[0].get_curve();
  const mod_t& q = curve.order();
  ecc_point_t R = curve.infinity();

  for (int i = 0; i < m; i++) {
    bn_t lambda = lagrange_basis(x, all_pids, pids_for_shares[i], q);
    R += lambda * shares[i];
  }

  return R;
}

ecc_point_t lagrange_interpolate_exponent(const bn_t& x, const std::vector<ecc_point_t>& shares,
                                          const std::vector<bn_t>& pids) {
  cb_assert(shares.size() == pids.size() && "shares and pids must have the same size");
  return lagrange_partial_interpolate_exponent(x, shares, pids, pids);
}

bn_t horner_poly(const mod_t& q, const std::vector<bn_t>& a, const bn_t& x) {
  int count = int(a.size());
  bn_t b = a[count - 1];
  for (int i = count - 2; i >= 0; i--) {
    MODULO(q) b = a[i] + b * x;
  }
  return b;
}

ecc_point_t horner_poly(const std::vector<ecc_point_t>& A, const bn_t& x) {
  int count = int(A.size());
  ecc_point_t B = A[count - 1];
  for (int i = count - 2; i >= 0; i--) {
    B = A[i] + x * B;
  }
  return B;
}

}  // namespace coinbase::crypto
