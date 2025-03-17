#pragma once

#include <cbmpc/crypto/base.h>

namespace coinbase::crypto {

class ec_elgamal_commitment_t {
 public:
  ecc_point_t L, R;

  ec_elgamal_commitment_t() {}
  ec_elgamal_commitment_t(const ecc_point_t& _U, const ecc_point_t& _R) : L(_U), R(_R) {}

  /**
   * @specs:
   * - basic-primitives-spec | EC-ElGamal-Commit-1P
   */
  static ec_elgamal_commitment_t make_commitment(const ecc_point_t& P, const bn_t& m, const bn_t& r);

  /**
   * @specs:
   * - basic-primitives-spec | EC-ElGamal-Commit-Local-Keygen-1P
   */
  static std::tuple<ecc_point_t, bn_t> local_keygen(ecurve_t curve);
  static const mod_t& order(ecurve_t curve);

  void convert(coinbase::converter_t& converter) {
    converter.convert(L);
    converter.convert(R);
  }

  struct com_t {
    const ecc_point_t& P;
    const bn_t& m;
    ec_elgamal_commitment_t rand(const bn_t& r) const { return ec_elgamal_commitment_t::make_commitment(P, m, r); }
  };
  // Encryption without generating U, V yet for efficiency
  static com_t commit(const ecc_point_t& P, const bn_t& m) { return com_t{P, m}; }
  static ec_elgamal_commitment_t random_commit(const ecc_point_t& P, const bn_t& m) {
    return commit(P, m).rand(P.get_curve().get_random_value());
  }

  struct rerand_t {
    const ecc_point_t& P;
    const ec_elgamal_commitment_t& UV;
    ec_elgamal_commitment_t rand(const bn_t& r) const { return UV.rerand(P, r); }
  };

  /**
   * @specs:
   * - basic-primitives-spec | EC-ElGamal-Commit-ReRand-1P
   */
  static rerand_t rerand(const ecc_point_t& PK, const ec_elgamal_commitment_t& UV) { return rerand_t{PK, UV}; }

  ec_elgamal_commitment_t operator+(const ec_elgamal_commitment_t& E) const;
  ec_elgamal_commitment_t& operator+=(const ec_elgamal_commitment_t& E) { return *this = *this + E; }
  ec_elgamal_commitment_t operator-(const ec_elgamal_commitment_t& E) const;
  ec_elgamal_commitment_t& operator-=(const ec_elgamal_commitment_t& E) { return *this = *this - E; }
  ec_elgamal_commitment_t operator+(const bn_t& s) const;
  ec_elgamal_commitment_t& operator+=(const bn_t& s) { return *this = *this + s; }
  ec_elgamal_commitment_t operator-(const bn_t& s) const;
  ec_elgamal_commitment_t& operator-=(const bn_t& s) { return *this = *this - s; }
  ec_elgamal_commitment_t operator*(const bn_t& s) const;
  ec_elgamal_commitment_t& operator*=(const bn_t& s) { return *this = *this * s; }
  ec_elgamal_commitment_t operator/(const bn_t& s) const;
  ec_elgamal_commitment_t& operator/=(const bn_t& s) { return *this = *this * s; }

  bool operator==(const ec_elgamal_commitment_t& E) const { return L == E.L && R == E.R; }
  bool operator!=(const ec_elgamal_commitment_t& E) const { return L != E.L || R != E.R; }

  ec_elgamal_commitment_t rerand(const ecc_point_t& pub_key, const bn_t& r) const;

  /**
   * @specs:
   * - basic-primitives-spec | EC-ElGamal-Commit-ReRand-1P
   */
  void randomize(const ecc_point_t& pub_key);

  /**
   * @specs:
   * - basic-primitives-spec | EC-ElGamal-Commit-ReRand-1P
   */
  void randomize(const bn_t& r, const ecc_point_t& pub_key);
  bool check_zero(const bn_t& prv_key) const;
  static bool check_equ(const ec_elgamal_commitment_t& E1, const ec_elgamal_commitment_t& E2, const bn_t& d);
  template <class T>
  void update_state(T& state) const {
    state.update(L);
    state.update(R);
  }

  error_t check_curve(ecurve_t curve) const {
    error_t rv = UNINITIALIZED_ERROR;
    if (rv = curve.check(L)) return coinbase::error(rv, "ec_elgamal_commitment_t::check_curve: invalid L");
    if (rv = curve.check(R)) return coinbase::error(rv, "ec_elgamal_commitment_t::check_curve: invalid R");
    return SUCCESS;
  }
};

template <class T>
T& update_state(T& state, const ec_elgamal_commitment_t& v) {
  v.update_state(state);
  return state;
}

inline ec_elgamal_commitment_t operator*(const bn_t& a, const ec_elgamal_commitment_t& B) { return B * a; }

}  // namespace coinbase::crypto

typedef coinbase::crypto::ec_elgamal_commitment_t elg_com_t;
