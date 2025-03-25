#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/zk/zk_unknown_order.h>
#include <cbmpc/zk/zk_util.h>

namespace coinbase::crypto {

struct unknown_order_pedersen_params_t {
  mod_t N;
  bn_t g, h;

  buf_t sid;
  char* e_str;
  char* z_str[SEC_P_COM];

  static const unknown_order_pedersen_params_t& get();
  static const unknown_order_pedersen_params_t generate();

  ~unknown_order_pedersen_params_t() {
    free(e_str);
    for (int i = 0; i < SEC_P_COM; i++) free(z_str[i]);
  }

 private:
  unknown_order_pedersen_params_t();
  unknown_order_pedersen_params_t(const mod_t& _N, const bn_t& _g, const bn_t& _h, const mem_t _sid, const mem_t e,
                                  const bn_t (&z)[SEC_P_COM])
      : N(_N), g(_g), h(_h), sid(_sid) {
    e_str = strdup(bn_t(e).to_string().c_str());
    for (int i = 0; i < SEC_P_COM; i++) z_str[i] = strdup(z[i].to_string().c_str());
  }
};

}  // namespace coinbase::crypto