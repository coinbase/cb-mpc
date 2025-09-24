#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/pve_base.h>
#include <cbmpc/zk/zk_ec.h>

namespace coinbase::mpc {

class ec_pve_t {
 public:
  // Default to unified PKE when not provided explicitly
  ec_pve_t();
  explicit ec_pve_t(const pve_base_pke_i& base_pke) : base_pke(base_pke) {}

  // Custom copy/move ctors bind the reference member correctly
  ec_pve_t(const ec_pve_t& other) : base_pke(other.base_pke), L(other.L), Q(other.Q), b(other.b) {
    for (int i = 0; i < kappa; ++i) {
      x_rows[i] = other.x_rows[i];
      r[i] = other.r[i];
      c[i] = other.c[i];
    }
  }
  ec_pve_t(ec_pve_t&& other) noexcept
      : base_pke(other.base_pke), L(std::move(other.L)), Q(std::move(other.Q)), b(other.b) {
    for (int i = 0; i < kappa; ++i) {
      x_rows[i] = std::move(other.x_rows[i]);
      r[i] = std::move(other.r[i]);
      c[i] = std::move(other.c[i]);
    }
  }
  // Assignment operators copy payload fields; reference member remains bound
  ec_pve_t& operator=(const ec_pve_t& other) {
    if (this == &other) return *this;
    L = other.L;
    Q = other.Q;
    b = other.b;
    for (int i = 0; i < kappa; ++i) {
      x_rows[i] = other.x_rows[i];
      r[i] = other.r[i];
      c[i] = other.c[i];
    }
    return *this;
  }
  ec_pve_t& operator=(ec_pve_t&& other) noexcept {
    if (this == &other) return *this;
    L = std::move(other.L);
    Q = std::move(other.Q);
    b = other.b;
    for (int i = 0; i < kappa; ++i) {
      x_rows[i] = std::move(other.x_rows[i]);
      r[i] = std::move(other.r[i]);
      c[i] = std::move(other.c[i]);
    }
    return *this;
  }

  const static int kappa = SEC_P_COM;
  const static int rho_size = 32;

  void encrypt(const void* ek, mem_t label, ecurve_t curve, const bn_t& x);
  error_t verify(const void* ek, const ecc_point_t& Q, mem_t label) const;
  error_t decrypt(const void* dk, const void* ek, mem_t label, ecurve_t curve, bn_t& x, bool skip_verify = false) const;

  const ecc_point_t& get_Q() const { return Q; }
  const buf_t& get_Label() const { return L; }

  void convert(coinbase::converter_t& converter) {
    converter.convert(Q, L, b);
    for (int i = 0; i < kappa; i++) {
      converter.convert(x_rows[i]);
      converter.convert(r[i]);
      converter.convert(c[i]);
    }
  }

 private:
  const pve_base_pke_i& base_pke;

  buf_t L;
  ecc_point_t Q;
  buf128_t b;

  bn_t x_rows[kappa];
  buf128_t r[kappa];
  buf_t c[kappa];

  error_t restore_from_decrypted(int row_index, mem_t decrypted_x_buf, ecurve_t curve, bn_t& x_value) const;
};

}  // namespace coinbase::mpc
