#include "elgamal.h"

namespace coinbase::crypto {

const mod_t& ec_elgamal_commitment_t::order(ecurve_t curve) { return curve.order(); }

std::tuple<ecc_point_t, bn_t> ec_elgamal_commitment_t::local_keygen(ecurve_t curve) {
  bn_t k = curve.get_random_value();
  ecc_point_t P = curve.mul_to_generator(k);
  return std::make_tuple(P, k);
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::make_commitment(const ecc_point_t& P, const bn_t& m,
                                                                 const bn_t& r)  // m - scalar, P - public key
{
  ecurve_t curve = P.get_curve();
  const auto& G = curve.generator();
  return ec_elgamal_commitment_t(r * G, curve.mul_add(m, P, r));  // m * G + P * r;
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator+(const ec_elgamal_commitment_t& E) const {
  return ec_elgamal_commitment_t(L + E.L, R + E.R);
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator-(const ec_elgamal_commitment_t& E) const {
  return ec_elgamal_commitment_t(L - E.L, R - E.R);
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator+(const bn_t& s) const {
  ecurve_t curve = L.get_curve();
  const auto& G = curve.generator();
  return ec_elgamal_commitment_t(L, R + s * G);
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator-(const bn_t& s) const {
  ecurve_t curve = L.get_curve();
  const mod_t& q = order(curve);

  bn_t minus_s;
  MODULO(q) minus_s = bn_t(0) - s;

  return *this + minus_s;
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator*(const bn_t& s) const {
  return ec_elgamal_commitment_t(s * L, s * R);
}

ec_elgamal_commitment_t ec_elgamal_commitment_t::operator/(const bn_t& s) const {
  ecurve_t curve = L.get_curve();
  const mod_t& q = order(curve);
  bn_t s_inv = q.inv(s);
  return *this * s_inv;
}

void ec_elgamal_commitment_t::randomize(const bn_t& r, const ecc_point_t& P) {
  ecurve_t curve = L.get_curve();
  const auto& G = curve.generator();
  *this += ec_elgamal_commitment_t(r * G, r * P);
}

void ec_elgamal_commitment_t::randomize(const ecc_point_t& P)  // P is the public key
{
  ecurve_t curve = L.get_curve();
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

bool ec_elgamal_commitment_t::check_zero(const bn_t& d) const  // d is the private key
{
  return R == d * L;
}

bool ec_elgamal_commitment_t::check_equ(const ec_elgamal_commitment_t& E1, const ec_elgamal_commitment_t& E2,
                                        const bn_t& d) {
  return (E1 - E2).check_zero(d);
}

}  // namespace coinbase::crypto
