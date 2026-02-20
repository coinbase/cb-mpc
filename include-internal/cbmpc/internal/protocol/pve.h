#pragma once

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/protocol/pve_base.h>
#include <cbmpc/internal/zk/zk_ec.h>

namespace coinbase::mpc {

class ec_pve_t {
 public:
  ec_pve_t() = default;

  const static int kappa = SEC_P_COM;
  const static int rho_size = 32;

  error_t encrypt(const pve_base_pke_i& base_pke, pve_keyref_t ek, mem_t label, ecurve_t curve, const bn_t& x);
  error_t verify(const pve_base_pke_i& base_pke, pve_keyref_t ek, const ecc_point_t& Q, mem_t label) const;
  error_t decrypt(const pve_base_pke_i& base_pke, pve_keyref_t dk, pve_keyref_t ek, mem_t label, ecurve_t curve,
                  bn_t& x, bool skip_verify = false) const;

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
  buf_t L;
  ecc_point_t Q;
  buf128_t b;

  bn_t x_rows[kappa];
  buf128_t r[kappa];
  buf_t c[kappa];

  error_t restore_from_decrypted(int row_index, mem_t decrypted_x_buf, ecurve_t curve, bn_t& x_value) const;
};

}  // namespace coinbase::mpc
