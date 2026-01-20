#include "elgamal.h"

#include <cbmpc/crypto/base_ecc.h>
#include <cbmpc/crypto/base_mod.h>

namespace coinbase::crypto {

namespace {

static inline void cb_assert_elgamal_ct_curve(ecurve_t curve) {
  // ElGamal operations are used with secret scalars in many protocols (e.g., ZK proving).
  // This repo only provides constant-time point addition on some curve backends; for OpenSSL-backed
  // curves (P-256/P-384/P-521) point addition is explicitly not constant-time.
  //
  // Allow bypassing this in explicitly marked variable-time regions (typically verification).
  if (!is_vartime_scope()) {
    cb_assert(curve.ct_add_support() != ct_add_support_e::None);
  }
}

}  // namespace

const mod_t& ec_elgamal_commitment_t::order(ecurve_t curve) { return curve.order(); }

std::tuple<ecc_point_t, bn_t> ec_elgamal_commitment_t::local_keygen(ecurve_t curve) {
  cb_assert_elgamal_ct_curve(curve);
  bn_t k = curve.get_random_value();
  ecc_point_t P = curve.mul_to_generator(k);
  return std::make_tuple(P, k);
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::make_commitment(const ecc_point_t& P, const bn_t& m,
                                                                 const bn_t& r)  // m - scalar, P - public key
{
  ecurve_t curve = P.get_curve();
  cb_assert_elgamal_ct_curve(curve);
  const auto& G = curve.generator();
  const mod_t& q = curve.order();
  const bn_t mm = q.mod(m);
  const bn_t rr = q.mod(r);
  return ec_elgamal_commitment_t(rr * G, curve.mul_add(mm, P, rr));  // m*G + r*P
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator+(const ec_elgamal_commitment_t& E) const {
  cb_assert_elgamal_ct_curve(L.get_curve());
  crypto::consttime_point_add_scope_t consttime_point_add_scope;
  return ec_elgamal_commitment_t(L + E.L, R + E.R);
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator-(const ec_elgamal_commitment_t& E) const {
  cb_assert_elgamal_ct_curve(L.get_curve());
  crypto::consttime_point_add_scope_t consttime_point_add_scope;
  return ec_elgamal_commitment_t(L - E.L, R - E.R);
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator+(const bn_t& s) const {
  ecurve_t curve = L.get_curve();
  cb_assert_elgamal_ct_curve(curve);
  const auto& G = curve.generator();
  const bn_t ss = curve.order().mod(s);
  crypto::consttime_point_add_scope_t consttime_point_add_scope;
  return ec_elgamal_commitment_t(L, R + ss * G);
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator-(const bn_t& s) const {
  ecurve_t curve = L.get_curve();
  cb_assert_elgamal_ct_curve(curve);
  const mod_t& q = order(curve);

  bn_t minus_s;
  const bn_t ss = q.mod(s);
  MODULO(q) minus_s = bn_t(0) - ss;

  return *this + minus_s;
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator*(const bn_t& s) const {
  cb_assert_elgamal_ct_curve(L.get_curve());
  return ec_elgamal_commitment_t(s * L, s * R);
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator/(const bn_t& s) const {
  ecurve_t curve = L.get_curve();
  cb_assert_elgamal_ct_curve(curve);
  const mod_t& q = order(curve);
  bn_t s_inv = q.inv(s);
  return *this * s_inv;
}

void ec_elgamal_commitment_t::randomize(const bn_t& r, const ecc_point_t& P) {
  ecurve_t curve = L.get_curve();
  cb_assert_elgamal_ct_curve(curve);
  const auto& G = curve.generator();
  const bn_t rr = curve.order().mod(r);
  crypto::consttime_point_add_scope_t consttime_point_add_scope;
  L += rr * G;
  R += rr * P;
}

void ec_elgamal_commitment_t::randomize(const ecc_point_t& P)  // P is the public key
{
  ecurve_t curve = L.get_curve();
  cb_assert_elgamal_ct_curve(curve);
  bn_t r = curve.get_random_value();
  randomize(r, P);
}

/**
 * @notes:
 * - This is the same as `randomize(r, pub_key)` except that it does not change the state of the object and instead
 * returns the rerandomized commitment as output.
 */
ec_elgamal_commitment_t ec_elgamal_commitment_t::rerand(const ecc_point_t& pub_key, const bn_t& r) const {
  ec_elgamal_commitment_t UV = *this;
  UV.randomize(r, pub_key);
  return UV;
}

}  // namespace coinbase::crypto
