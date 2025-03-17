#include "ro.h"

#include <cbmpc/core/log.h>

namespace coinbase::crypto::ro {  // random oracle

/**
 * @specs:
 * - basic-primitives-spec | drbg-sample-string-1P
 */
buf_t drbg_sample_string(mem_t seed, int bits) {
  crypto::drbg_aes_ctr_t drbg(seed);
  return drbg.gen(coinbase::bits_to_bytes(bits));
}

/**
 * @specs:
 * - basic-primitives-spec | drbg-sample-number-1P
 */
bn_t drbg_sample_number(mem_t seed, const mod_t& p)  // modulo p
{
  buf_t r = drbg_sample_string(seed, p.get_bits_count() + SEC_P_STAT);
  return bn_t::from_bin(r) % p;
}

/**
 * @specs:
 * - basic-primitives-spec | drbg-sample-curve-point-1P
 */
ecc_point_t drbg_sample_curve(mem_t seed, const crypto::ecurve_t& curve) {
  mod_t q = curve.order();
  bn_t r = drbg_sample_number(seed, q);
  return r * curve.generator();
}

/**
 * In the basic-primitives-spec.pdf, section 4.2, we describe how we create a "Random Oracle"
 * using an HMAC with a hardwired key.
 * It is important that this is NOT used as the seed.
 * Rather seeds are random values and are passed as input to an hmac keyed by this hardwired value.
 */
// The following is the first 16 bytes of the result of SHA256("Coinbase Random Oracle Key")
static const byte_t global_key[16] = {0xe5, 0xef, 0x49, 0x37, 0x19, 0x89, 0x88, 0x83,
                                      0x50, 0xc4, 0x56, 0x5c, 0xca, 0x19, 0x08, 0x4a};

hmac_state_t::hmac_state_t() : hmac(mem_t(global_key, sizeof(global_key))) {}

buf_t hmac_state_t::final() { return hmac.final(); }

buf128_t hash_string_t::bitlen128() {
  buf_t h = final();
  return buf128_t::load(h.data());
}

buf256_t hash_string_t::bitlen256() {
  buf_t h = final();
  return buf256_t::load(h.data());
}

buf_t hash_string_t::bitlen(int bits) {
  buf_t h = final();
  int bytes = coinbase::bits_to_bytes(bits);

  if (bytes <= 32) {
    buf_t out = h.take(bytes);
    return out;
  }

  return drbg_sample_string(h, bits);
}

bn_t hash_number_t::mod(const mod_t& p) {
  buf_t h = final();
  return drbg_sample_number(h, p);
}

std::vector<bn_t> hash_numbers_t::mod(const mod_t& p) {
  buf_t h = final();

  int bits_per_value = p.get_bits_count() + SEC_P_STAT;
  int bytes_per_value = bits_to_bytes(bits_per_value);
  buf_t t = drbg_sample_string(h, bytes_to_bits(bytes_per_value) * l);

  std::vector<bn_t> r(l);
  for (int i = 0; i < l; i++) {
    mem_t bin = t.range(i * bytes_per_value, bytes_per_value);
    r[i] = bn_t::from_bin(bin) % p;
  }

  return r;
}

ecc_point_t hash_curve_t::curve(ecurve_t curve) {
  dylog_disable_scope_t dylog_disable_scope;

  ecc_point_t Q(curve);
  for (int i = 0; true; i++) {
    ro::hash_string_t alt;
    hmac.copy_state(alt.hmac);
    alt.encode_and_update(i);

    buf_t bin = alt.bitlen(curve.bits());
    if (curve.hash_to_point(bin, Q)) break;
  }

  return Q;
}

}  // namespace coinbase::crypto::ro
