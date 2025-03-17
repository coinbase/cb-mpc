#pragma once
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/elgamal.h>
#include <cbmpc/crypto/ro.h>

namespace coinbase::crypto {

bn_t horner_poly(const mod_t& q, const std::vector<bn_t>& a, const bn_t& x);
ecc_point_t horner_poly(const std::vector<ecc_point_t>& A, const bn_t& x);

/* Lagrange */

/**
 * @specs:
 * - basic-primitives-spec | Lagrange-Basis-1P
 */
void lagrange_basis(const bn_t& x, const std::vector<int>& pids, int current_pid, const mod_t& q, bn_t& numerator,
                    bn_t& denominator);
bn_t lagrange_basis(const bn_t& x, const std::vector<int>& pids, int current_pid, const mod_t& q);
bn_t lagrange_basis(const bn_t& x, const std::vector<bn_t>& pids, const bn_t& current_pid, const mod_t& q);

/**
 * Note: shares and pids should have the same size. For non-existing shares, use 0.
 *
 * @specs:
 * - basic-primitives-spec | Lagrange-Partial-Interpolate-1P
 */
bn_t lagrange_partial_interpolate(const bn_t& x, const std::vector<bn_t>& shares,
                                  const std::vector<bn_t>& pids_for_shares, const std::vector<bn_t>& all_pids,
                                  const mod_t& q);

/**
 * @specs:
 * - basic-primitives-spec | Lagrange-Interpolate-1P
 */
bn_t lagrange_interpolate(const bn_t& x, const std::vector<bn_t>& shares, const std::vector<bn_t>& pids,
                          const mod_t& q);

/* Lagrange in exponent */

/**
 * @specs:
 * - basic-primitives-spec | Lagrange-Partial-Interpolate-Exponent-1P
 */
ecc_point_t lagrange_partial_interpolate_exponent(const bn_t& x, const std::vector<ecc_point_t>& shares,
                                                  const std::vector<bn_t>& pids_for_shares,
                                                  const std::vector<bn_t>& all_pids);

/**
 * @specs:
 * - basic-primitives-spec | Lagrange-Interpolate-Exponent-1P
 */
ecc_point_t lagrange_interpolate_exponent(const bn_t& x, const std::vector<ecc_point_t>& shares,
                                          const std::vector<bn_t>& pids);

}  // namespace coinbase::crypto
