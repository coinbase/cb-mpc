#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_bn256.h>

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
  ctr.update(out, const_cast<byte_ptr>(out.data));
}

bn_t drbg_aes_ctr_t::gen_bn(const mod_t& mod) { return gen_bn(mod.get_bits_count() + SEC_P_STAT) % mod; }

bn_t drbg_aes_ctr_t::gen_bn(const bn_t& mod) { return gen_bn(mod.get_bits_count() + SEC_P_STAT) % mod; }

bn_t drbg_aes_ctr_t::gen_bn(int bits) {
  int n = coinbase::bits_to_bytes(bits);
  buf_t bin = gen(n);
  return bn_t::from_bin_bitlen(bin, bits);
}

bits_t drbg_aes_ctr_t::gen_bits(int bitlen) {
  bits_t bits(bitlen);
  gen(mem_t(bits));
  return bits;
}

bn256_t drbg_aes_ctr_t::gen_bn256(const mod_t& mod) {
  bn256_t r;

  const bn_t& m = mod;
  const BIGNUM* mbn = m;
  if (mbn->top == 4 && mbn->d[3] == 0xffffffffffffffff && mbn->d[2] >= 0xffffffffffff0000) {
    // Fast path for near-2^256 moduli. This intentionally skips reduction for
    // speed; callers of gen_bn256 must tolerate the negligible chance that the
    // result is >= mod.
    byte_t rnd[32];
    gen(byte_ptr(rnd), sizeof(rnd));
    r = bn256_t::from_bin(mem_t(rnd, sizeof(rnd)));
    return r;
  }

  byte_t rnd[40];
  gen(byte_ptr(rnd), sizeof(rnd));

  uint64_t temp[8] = {0};
  temp[0] = coinbase::be_get_8(rnd + 32);
  temp[1] = coinbase::be_get_8(rnd + 24);
  temp[2] = coinbase::be_get_8(rnd + 16);
  temp[3] = coinbase::be_get_8(rnd + 8);
  temp[4] = coinbase::be_get_8(rnd + 0);
  r = bn256_t::reduce(temp, mod);
  return r;
}

}  // namespace coinbase::crypto
