#include "zk_elgamal_com.h"

#include <cbmpc/crypto/ro.h>
#include <cbmpc/zk/zk_pedersen.h>

namespace coinbase::zk {

void uc_elgamal_com_t::prove(const ecc_point_t& Q, elg_com_t UV, const bn_t& x, const bn_t& r, mem_t session_id,
                             uint64_t aux) {
  std::vector<bn_t> r1(params.rho);
  std::vector<bn_t> r2(params.rho);
  ecurve_t curve = Q.get_curve();
  const auto& G = curve.generator();
  const mod_t& q = curve.order();
  int rho = params.rho;

  AB.resize(rho);
  e.resize(rho);
  z1.resize(rho);
  z2.resize(rho);

  bn_t z1_tag, z2_tag;
  bn_t q_value = bn_t(q);
  buf_t common_hash;

  fischlin_prove(
      params,
      // initialize
      [&]() {
        for (int i = 0; i < rho; i++) {
          r1[i] = bn_t::rand(q);
          r2[i] = bn_t::rand(q);
          AB[i] = elg_com_t::commit(Q, r1[i]).rand(r2[i]);
        }
        common_hash = crypto::ro::hash_string(G, Q, UV, AB, session_id, aux).bitlen(2 * SEC_P_COM);
      },

      // response_begin
      [&](int i) {
        z1_tag = r1[i];
        z2_tag = r2[i];
      },

      // hash
      [&](int i, int e_tag) -> uint32_t { return hash32bit_for_zk_fischlin(common_hash, i, e_tag, z1_tag, z2_tag); },

      // save
      [&](int i, int e_tag) {
        e[i] = e_tag;
        z1[i] = z1_tag;
        z2[i] = z2_tag;
      },

      // response_next
      [&](int e_tag) {
        int res = bn_mod_add_fixed_top(z1_tag, z1_tag, x, q_value);
        cb_assert(res);
        res = bn_mod_add_fixed_top(z2_tag, z2_tag, r, q_value);
        cb_assert(res);
      });
}

error_t uc_elgamal_com_t::verify(const ecc_point_t& Q, const elg_com_t& UV, mem_t session_id, uint64_t aux) const {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::vartime_scope_t vartime_scope;
  int rho = params.rho;
  if (params.b * rho < SEC_P_COM) return coinbase::error(E_CRYPTO);
  if (int(AB.size()) != rho) return coinbase::error(E_CRYPTO);
  if (int(e.size()) != rho) return coinbase::error(E_CRYPTO);
  if (int(z1.size()) != rho) return coinbase::error(E_CRYPTO);
  if (int(z2.size()) != rho) return coinbase::error(E_CRYPTO);

  ecurve_t curve = Q.get_curve();
  if (rv = curve.check(Q)) return coinbase::error(rv, "uc_elgamal_com_t::verify: check Q failed");
  if (rv = UV.check_curve(curve)) return coinbase::error(rv, "uc_elgamal_com_t::verify: check UV failed");
  for (int i = 0; i < rho; i++) {
    if (rv = AB[i].check_curve(curve)) return coinbase::error(rv, "uc_elgamal_com_t::verify: check AB failed");
  }

  const mod_t& q = curve.order();
  const auto& G = curve.generator();
  uint32_t b_mask = params.b_mask();
  buf_t common_hash = crypto::ro::hash_string(G, Q, UV, AB, session_id, aux).bitlen(2 * SEC_P_COM);

  bn_t z1_sum = 0;
  bn_t z2_sum = 0;
  bn_t e_sum = 0;
  ecc_point_t A_sum = curve.infinity();
  ecc_point_t B_sum = curve.infinity();

  for (int i = 0; i < rho; i++) {
    bn_t sigma = bn_t::rand_bitlen(SEC_P_STAT);
    MODULO(q) {
      z1_sum += sigma * z1[i];
      z2_sum += sigma * z2[i];
      e_sum += sigma * bn_t(e[i]);
    }
    A_sum += sigma * AB[i].L;
    B_sum += sigma * AB[i].R;

    uint32_t h = hash32bit_for_zk_fischlin(common_hash, i, e[i], z1[i], z2[i]) & b_mask;
    if (h != 0) return coinbase::error(E_CRYPTO);
  }

  const ecc_point_t& U = UV.L;
  const ecc_point_t& V = UV.R;
  if (A_sum != z2_sum * G - e_sum * U) return coinbase::error(E_CRYPTO);
  if (B_sum != z2_sum * Q + z1_sum * G - e_sum * V) return coinbase::error(E_CRYPTO);

  return SUCCESS;
}

void elgamal_com_pub_share_equ_t::prove(const ecc_point_t& Q, const ecc_point_t& A, const elg_com_t eA, const bn_t& r,
                                        mem_t session_id, uint64_t aux) {
  ecc_point_t eaR_minus_A;
  eaR_minus_A = eA.R - A;
  return zk_dh.prove(Q, eA.L, eaR_minus_A, r, session_id, aux);
}

error_t elgamal_com_pub_share_equ_t::verify(const ecc_point_t& Q, const ecc_point_t& A, const elg_com_t B,
                                            mem_t session_id, uint64_t aux) const {
  crypto::vartime_scope_t vartime_scope;
  error_t rv = UNINITIALIZED_ERROR;
  ecurve_t curve = Q.get_curve();
  if (rv = curve.check(B.R)) return coinbase::error(rv, "elgamal_com_pub_share_equ_t::verify: check B.R failed");
  if (rv = curve.check(A)) return coinbase::error(rv, "elgamal_com_pub_share_equ_t::verify: check A failed");

  return rv = zk_dh.verify(Q, B.L, B.R - A, session_id, aux);
}

void elgamal_com_mult_t::prove(const ecc_point_t& Q, const elg_com_t& A, const elg_com_t& B, const elg_com_t& C,
                               const bn_t& r_B, const bn_t& r_C, const bn_t& b, mem_t session_id, uint64_t aux) {
  ecurve_t curve = Q.get_curve();
  const mod_t& q = curve.order();

  bn_t r1 = bn_t::rand(q);
  bn_t r2 = bn_t::rand(q);
  bn_t r3 = bn_t::rand(q);
  auto R = crypto::ec_elgamal_commitment_t::commit(Q, r1).rand(r2);
  auto A_tag = (r1 * A).rerand(Q, r3);
  e = crypto::ro::hash_number(Q, R, A_tag, A, B, C, session_id, aux).mod(q);

  MODULO(q) {
    z1 = r1 + e * b;
    z2 = r2 + e * r_B;
    z3 = r3 + e * r_C;
  }
}

error_t elgamal_com_mult_t::verify(const ecc_point_t& Q, const elg_com_t& A, const elg_com_t& B, const elg_com_t& C,
                                   mem_t session_id, uint64_t aux) const {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::vartime_scope_t vartime_scope;

  ecurve_t curve = Q.get_curve();
  if (rv = curve.check(Q)) return coinbase::error(rv, "elgamal_com_mult_t::verify: check Q failed");
  if (rv = A.check_curve(curve)) return coinbase::error(rv, "elgamal_com_mult_t::verify: check A failed");
  if (rv = B.check_curve(curve)) return coinbase::error(rv, "elgamal_com_mult_t::verify: check B failed");
  if (rv = C.check_curve(curve)) return coinbase::error(rv, "elgamal_com_mult_t::verify: check C failed");

  const mod_t& q = curve.order();

  auto R = crypto::ec_elgamal_commitment_t::commit(Q, z1).rand(z2) - e * B;
  auto A_tag = (z1 * A).rerand(Q, z3) - e * C;
  bn_t e_tag = crypto::ro::hash_number(Q, R, A_tag, A, B, C, session_id, aux).mod(q);
  if (e != e_tag) return coinbase::error(E_CRYPTO, "e != e'");
  return SUCCESS;
}

void uc_elgamal_com_mult_private_scalar_t::prove(const ecc_point_t& Q, const elg_com_t& A, const elg_com_t& B,
                                                 const bn_t& r, const bn_t& c, mem_t session_id, uint64_t aux) {
  std::vector<bn_t> r1(params.rho);
  std::vector<bn_t> r2(params.rho);
  ecurve_t curve = Q.get_curve();
  const mod_t& q = curve.order();
  int rho = params.rho;

  A1_tag.resize(rho);
  A2_tag.resize(rho);
  e.resize(rho);
  z1.resize(rho);
  z2.resize(rho);

  bn_t z1_tag, z2_tag;
  bn_t q_value = bn_t(q);
  buf_t common_hash;

  fischlin_prove(
      params,
      // initialize
      [&]() {
        const ecc_point_t& A1 = A.L;
        const ecc_point_t& A2 = A.R;
        for (int i = 0; i < rho; i++) {
          r1[i] = bn_t::rand(q);
          r2[i] = bn_t::rand(q);
          A1_tag[i] = curve.mul_add(r2[i], A1, r1[i]);
          A2_tag[i] = crypto::extended_ec_mul_add_ct(r1[i], A2, r2[i], Q);
        }
        common_hash = crypto::ro::hash_string(Q, A, B, A1_tag, A2_tag, session_id, aux).bitlen(2 * SEC_P_COM);
      },

      // response_begin
      [&](int i) {
        z1_tag = r1[i];
        z2_tag = r2[i];
      },

      // hash
      [&](uint16_t i, uint16_t e_tag) -> uint16_t {
        return hash32bit_for_zk_fischlin(common_hash, i, e_tag, z1_tag, z2_tag);
      },

      // save
      [&](int i, uint16_t e_tag) {
        e[i] = e_tag;
        z1[i] = z1_tag;
        z2[i] = z2_tag;
      },

      // response_next
      [&](uint16_t e_tag) {
        int res = bn_mod_add_fixed_top(z1_tag, z1_tag, c, q_value);
        cb_assert(res);
        res = bn_mod_add_fixed_top(z2_tag, z2_tag, r, q_value);
        cb_assert(res);
      });
}

error_t uc_elgamal_com_mult_private_scalar_t::verify(const ecc_point_t& Q, const elg_com_t& A, const elg_com_t& B,
                                                     mem_t session_id, uint64_t aux) {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::vartime_scope_t vartime_scope;
  int rho = params.rho;
  if (params.b * rho < SEC_P_COM) return coinbase::error(E_CRYPTO);
  if (int(A1_tag.size()) != rho) return coinbase::error(E_CRYPTO);
  if (int(A2_tag.size()) != rho) return coinbase::error(E_CRYPTO);
  if (int(e.size()) != rho) return coinbase::error(E_CRYPTO);
  if (int(z1.size()) != rho) return coinbase::error(E_CRYPTO);
  if (int(z2.size()) != rho) return coinbase::error(E_CRYPTO);

  ecurve_t curve = Q.get_curve();
  if (rv = curve.check(Q)) return coinbase::error(rv, "uc_elgamal_com_mult_private_scalar_t::verify: check Q failed");
  if (rv = A.check_curve(curve))
    return coinbase::error(rv, "uc_elgamal_com_mult_private_scalar_t::verify: check A failed");
  if (rv = B.check_curve(curve))
    return coinbase::error(rv, "uc_elgamal_com_mult_private_scalar_t::verify: check B failed");

  const mod_t& q = curve.order();
  const auto& G = curve.generator();
  uint16_t b_mask = params.b_mask();
  buf_t common_hash = crypto::ro::hash_string(Q, A, B, A1_tag, A2_tag, session_id, aux).bitlen(2 * SEC_P_COM);

  bn_t z1_sum = 0;
  bn_t z2_sum = 0;
  bn_t e_sum = 0;
  ecc_point_t A1_sum = curve.infinity();
  ecc_point_t A2_sum = curve.infinity();

  for (int i = 0; i < rho; i++) {
    if (rv = curve.check(A1_tag[i]))
      return coinbase::error(rv, "uc_elgamal_com_mult_private_scalar_t::verify: check A1_tag failed");
    if (rv = curve.check(A2_tag[i]))
      return coinbase::error(rv, "uc_elgamal_com_mult_private_scalar_t::verify: check A2_tag failed");

    bn_t sigma = bn_t::rand_bitlen(SEC_P_STAT);
    MODULO(q) {
      z1_sum += sigma * z1[i];
      z2_sum += sigma * z2[i];
      e_sum += sigma * bn_t(e[i]);
    }
    A1_sum += sigma * A1_tag[i];
    A2_sum += sigma * A2_tag[i];

    uint16_t h = hash32bit_for_zk_fischlin(common_hash, uint16_t(i), e[i], z1[i], z2[i]) & b_mask;
    if (h != 0) return coinbase::error(E_CRYPTO);
  }

  const ecc_point_t& A1 = A.L;
  const ecc_point_t& A2 = A.R;
  const ecc_point_t& B1 = B.L;
  const ecc_point_t& B2 = B.R;

  if (A1_sum != z1_sum * A1 + z2_sum * G - e_sum * B1) return coinbase::error(E_CRYPTO);
  if (A2_sum != z1_sum * A2 + z2_sum * Q - e_sum * B2) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

}  // namespace coinbase::zk
