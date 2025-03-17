#pragma once
#include <cbmpc/crypto/base.h>

namespace coinbase::zk {

struct unknown_order_dl_t {
  buf_t e;
  bn_t z[SEC_P_COM];

  void convert(coinbase::converter_t& c) { c.convert(e, z); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-ZK-Unknown-Order-DL-1P
   */
  void prove(const bn_t& a, const bn_t& b, const mod_t& N, const int l, const bn_t& w, mem_t sid, const uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-ZK-Unknown-Order-DL-1P
   */
  error_t verify(const bn_t& a, const bn_t& b, const mod_t& N, const int l, mem_t sid, uint64_t aux) const;
};

}  // namespace coinbase::zk
