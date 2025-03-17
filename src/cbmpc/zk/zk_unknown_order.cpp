#include "zk_unknown_order.h"

#include <cbmpc/crypto/ro.h>
#include <cbmpc/zk/zk_paillier.h>
namespace coinbase::zk {

void unknown_order_dl_t::prove(const bn_t& a, const bn_t& b, const mod_t& N, const int l, const bn_t& w, mem_t sid,
                               const uint64_t aux) {
  cb_assert(w.get_bits_count() <= l);
  int r_size = l + SEC_P_STAT + 1;

  bn_t gcd_test;
  MODULO(N) gcd_test = a * b;
  cb_assert(mod_t::coprime(gcd_test, N) && "unknown_order_dl_t::prove: gcd(a*b, N) != 1");

  bn_t R[SEC_P_COM];
  for (int i = 0; i < SEC_P_COM; i++) {
    z[i] = bn_t::rand_bitlen(r_size);
    MODULO(N) R[i] = a.pow(z[i]);
  }

  e = crypto::ro::hash_string(a, b, N, l, R, sid, aux).bitlen(SEC_P_COM);

  for (int i = 0; i < SEC_P_COM; i++) {
    if (coinbase::bits_t::get(e.data(), i)) z[i] = z[i] + w;
  }
}

error_t unknown_order_dl_t::verify(const bn_t& a, const bn_t& b, const mod_t& N, const int l, mem_t sid,
                                   uint64_t aux) const {
  crypto::vartime_scope_t vartime_scope;
  bn_t b_inv = N.inv(b);

  bn_t R_tag;
  MODULO(N) R_tag = a * b;

  bn_t R[SEC_P_COM];
  for (int i = 0; i < SEC_P_COM; i++) {
    MODULO(N) R[i] = a.pow(z[i]);
    if (coinbase::bits_t::get(e.data(), i)) MODULO(N) R[i] = R[i] * b_inv;

    MODULO(N) R_tag *= R[i];
  }

  buf_t e_tag = crypto::ro::hash_string(a, b, N, l, R, sid, aux).bitlen(SEC_P_COM);
  if (e != e_tag) {
    return coinbase::error(E_CRYPTO);
  }

  if (!mod_t::coprime(R_tag, N)) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

}  // namespace coinbase::zk
