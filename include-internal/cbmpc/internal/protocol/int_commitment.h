#pragma once

#include <array>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/zk/zk_unknown_order.h>
#include <cbmpc/internal/zk/zk_util.h>

namespace coinbase::crypto {

struct unknown_order_pedersen_params_t {
  mod_t N;
  bn_t g, h;

  buf_t sid;
  std::string e_str;
  std::array<std::string, SEC_P_COM> z_str;

  static const unknown_order_pedersen_params_t& get();
  static unknown_order_pedersen_params_t generate();

 private:
  unknown_order_pedersen_params_t();
  unknown_order_pedersen_params_t(const mod_t& _N, const bn_t& _g, const bn_t& _h, const mem_t _sid, const mem_t e,
                                  const bn_t (&z)[SEC_P_COM])
      : N(_N), g(_g), h(_h), sid(_sid), e_str(bn_t(e).to_string()) {
    for (int i = 0; i < SEC_P_COM; i++) z_str[i] = z[i].to_string();
  }
};

}  // namespace coinbase::crypto