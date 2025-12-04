#include <cbmpc/crypto/base.h>

namespace coinbase::crypto {

/**
 * @notes:
 * - Note: this must be followed by a call to seed
 */
void drbg_aes_ctr_t::init() {
  byte_t k[16] = {0};
  byte_t iv[16] = {0};

  ctr.init(mem_t(k, 16), iv);
}

drbg_aes_ctr_t::drbg_aes_ctr_t(mem_t s) { init(s); }

void drbg_aes_ctr_t::init(mem_t s) {
  cb_assert(coinbase::bytes_to_bits(s.size) >= SEC_P_COM && "DRBG requires SEC_P_COM bits of entropy");
  if (s.size == 32) {
    ctr.init(s.take(16), s.data + 16);
  } else {
    init();
    seed(s);
  }
}

void drbg_aes_ctr_t::seed(mem_t in) {
  buf128_t old = gen_buf128();
  buf256_t hash = buf256_t(crypto::sha256_t::hash(old, in));
  ctr.init(hash.lo, byte_ptr(&hash.hi));
}

void drbg_aes_ctr_t::gen(mem_t out) {
  out.bzero();
  ctr.update(out, out.data);
}

bn_t drbg_aes_ctr_t::gen_bn(const mod_t& mod) { return gen_bn(mod.get_bits_count() + SEC_P_STAT) % mod; }

bn_t drbg_aes_ctr_t::gen_bn(const bn_t& mod) { return gen_bn(mod.get_bits_count() + SEC_P_STAT) % mod; }

bn_t drbg_aes_ctr_t::gen_bn(int bits) {
  int n = coinbase::bits_to_bytes(bits);
  buf_t bin = gen(n);
  return bn_t::from_bin_bitlen(bin, bits);
}

}  // namespace coinbase::crypto
