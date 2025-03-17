#include <cbmpc/crypto/base.h>

namespace coinbase::crypto {

namespace ed25519 {

static const uint8_t ed25519_prv_prefix[] = {0x30, 0x2E, 0x02, 0x01, 0x00, 0x30, 0x05, 0x06,
                                             0x03, 0x2B, 0x65, 0x70, 0x04, 0x22, 0x04, 0x20};
static const uint8_t ed25519_pub_prefix[] = {0x30, 0x2A, 0x30, 0x05, 0x06, 0x03, 0x2B, 0x65, 0x70, 0x03, 0x21, 0x00};
static const mem_t pkcs8_prefix = mem_t(ed25519_prv_prefix, sizeof(ed25519_prv_prefix));
static const mem_t x509_prefix = mem_t(ed25519_pub_prefix, sizeof(ed25519_pub_prefix));

int signature_size() { return 64; }
int pub_compressed_bin_size() { return 32; }
int prv_bin_size() { return 32; }
int bits() { return 256; }

static const bn_t& a() {
  static const bn_t a_value = bn_t::from_string("486662");
  return a_value;
}

static const bn_t& b() {
  static const bn_t b_value = bn_t::from_string("1");
  return b_value;
}

static const bn_t& p() {
  static const bn_t p_value =
      bn_t::from_string("57896044618658097711785492504343953926634992332820282019728792003956564819949");
  return p_value;
}

static const mod_t& order() {
  static const mod_t order_value =
      mod_t(bn_t::from_string("7237005577332262213973186563042994240857116359379907606001950938285454250989"),
            /* multiplicative_dense */ true);
  return order_value;
}

// see https://www.rfc-editor.org/rfc/rfc8032#section-3.2
bn_t prv_key_to_scalar(mem_t bin) {
  if (bin.size != 32) return bn_t(0);

  buf_t az = crypto::hash_t(crypto::hash_e::sha512).init().update(bin).final();

  az[0] &= 248;
  az[31] &= 63;
  az[31] |= 64;

  az.resize(32);
  az.reverse();
  return bn_t::from_bin(az) % order();
}

}  // namespace ed25519

// ------------------------- ecurve_ed_t ------------------

ecurve_ed_t::ecurve_ed_t() noexcept {
  type = ecurve_type_e::ed25519;
  name = "ED25519";
  bits = 256;
  openssl_code = NID_ED25519;
}

void ecurve_ed_t::get_params(bn_t& p, bn_t& a, bn_t& b) const {
  a = ed25519::a();
  b = ed25519::b();
  p = ed25519::p();
}

const mod_t& ecurve_ed_t::p() const {
  static const mod_t p_value(ed25519::p(), /* multiplicative_dense */ true);
  return p_value;
}

const mod_t& ecurve_ed_t::order() const { return ed25519::order(); }

const ecc_generator_point_t& ecurve_ed_t::generator() const {
  static const ecc_generator_point_t gen = ecc_point_t(ec25519_core::get_generator());
  return gen;
}

void ecurve_ed_t::mul_to_generator_vartime(const bn_t& val, ecc_point_t& P) const { mul_to_generator(val, P); }

void ecurve_ed_t::mul_to_generator(const bn_t& val, ecc_point_t& P) const {
  ec25519_core::mul_to_generator(P.ed, val % order());
}

void ecurve_ed_t::init_point(ecc_point_t& P) const { P.ed = ec25519_core::new_point(); }

void ecurve_ed_t::free_point(ecc_point_t& P) const { ec25519_core::free_point(P.ed); }

void ecurve_ed_t::copy_point(ecc_point_t& Dst, const ecc_point_t& Src) const {
  Dst.ed = ec25519_core::new_point(Src.ed);
}

bool ecurve_ed_t::is_on_curve(const ecc_point_t& P) const { return ec25519_core::is_on_curve(P.ed); }

bool ecurve_ed_t::is_in_subgroup(const ecc_point_t& P) const {
  // NOTE: There is a more efficient way to check: https://eprint.iacr.org/2022/1164.pdf
  if (!is_on_curve(P)) return false;
  return ec25519_core::is_in_subgroup(P.ed);
}

bool ecurve_ed_t::is_infinity(const ecc_point_t& P) const { return ec25519_core::is_infinity(P.ed); }

void ecurve_ed_t::set_infinity(ecc_point_t& P) const { ec25519_core::set_infinity(P.ed); }

void ecurve_ed_t::invert_point(ecc_point_t& P) const { ec25519_core::neg(P.ed, P.ed); }

bool ecurve_ed_t::equ_points(const ecc_point_t& P1, const ecc_point_t& P2) const {
  return ec25519_core::equ(P1.ed, P2.ed);
}

void ecurve_ed_t::add(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const {
  ec25519_core::add(R.ed, P1.ed, P2.ed);
}

void ecurve_ed_t::add_consttime(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const {
  ec25519_core::add(R.ed, P1.ed, P2.ed);
}

void ecurve_ed_t::mul_vartime(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const { mul(P, x, R); }

void ecurve_ed_t::mul(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const { ec25519_core::mul(R.ed, P.ed, x); }

void ecurve_ed_t::mul_add(const bn_t& n, const ecc_point_t& P, const bn_t& m, ecc_point_t& R) const  // R = G*n + P*m
{
  ec25519_core::mul_add(R.ed, P.ed, m, n);
}

int ecurve_ed_t::to_compressed_bin(const ecc_point_t& P, byte_ptr out) const {
  if (out) ec25519_core::to_bin(P.ed, out);
  return 32;
}

void ecurve_ed_t::get_coordinates(const ecc_point_t& P, bn_t& x, bn_t& y) const { ec25519_core::get_xy(P.ed, x, y); }

void ecurve_ed_t::set_coordinates(ecc_point_t& P, const bn_t& x, const bn_t& y) const {
  ec25519_core::set_xy(P.ed, x, y);
}

bool ecurve_ed_t::hash_to_point(mem_t bin, ecc_point_t& P) const {
  if (bin.size != ed25519::pub_compressed_bin_size()) return false;
  if (0 != from_bin(P, bin)) return false;

  P *= 8;  // clear co-factor
  return true;
}

error_t ecurve_ed_t::from_bin(ecc_point_t& P, mem_t bin) const {
  error_t rv = ec25519_core::from_bin(P.ed, bin);
  if (rv != 0) {
    set_infinity(P);
    return rv;
  }
  return SUCCESS;
}

buf_t ecurve_ed_t::pub_to_der(const ecc_pub_key_t& P) const {
  buf_t out(ed25519::x509_prefix.size + ed25519::pub_compressed_bin_size());
  memmove(out.data(), ed25519::x509_prefix.data, ed25519::x509_prefix.size);
  to_compressed_bin(P, out.data() + ed25519::x509_prefix.size);
  return out;
}

buf_t ecurve_ed_t::prv_to_der(const ecc_prv_key_t& K) const {
  cb_assert(K.ed_bin.size() == ed25519::prv_bin_size());
  buf_t out(ed25519::pkcs8_prefix.size + ed25519::prv_bin_size());
  memmove(out.data(), ed25519::pkcs8_prefix.data, ed25519::pkcs8_prefix.size);
  memmove(out.data() + ed25519::x509_prefix.size, K.ed_bin.data(), ed25519::prv_bin_size());
  return out;
}

error_t ecurve_ed_t::pub_from_der(ecc_pub_key_t& P, mem_t der) const {
  if (der.size != ed25519::x509_prefix.size + ed25519::pub_compressed_bin_size()) return coinbase::error(E_FORMAT);
  if (0 != memcmp(ed25519::x509_prefix.data, der.data, ed25519::x509_prefix.size)) return coinbase::error(E_FORMAT);
  return from_bin(P, der.skip(ed25519::x509_prefix.size));
}

error_t ecurve_ed_t::prv_from_der(ecc_prv_key_t& K, mem_t der) const {
  if (der.size != ed25519::pkcs8_prefix.size + ed25519::prv_bin_size()) return coinbase::error(E_FORMAT);
  if (0 != memcmp(ed25519::pkcs8_prefix.data, der.data, ed25519::pkcs8_prefix.size)) return coinbase::error(E_FORMAT);
  K.ed_bin = der.skip(ed25519::pkcs8_prefix.size);
  return SUCCESS;
}

error_t ecurve_ed_t::verify(const ecc_pub_key_t& P, mem_t hash, mem_t sig) const {
  byte_t pub_bin[32];
  to_compressed_bin(P, pub_bin);
  if (sig.size != ed25519::signature_size()) return coinbase::error(E_FORMAT);
  if (!ED25519_verify(hash.data, hash.size, sig.data, pub_bin)) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

buf_t ecurve_ed_t::sign(const ecc_prv_key_t& K, mem_t hash) const {
  buf_t sig(ed25519::signature_size());
  ecc_point_t P = K.pub();
  buf_t pub_bin = P.to_compressed_bin();

  if (K.ed_bin.empty()) {
    buf_t scalar = K.value().to_bin(ed25519::prv_bin_size());
    ED25519_sign_with_scalar(sig.data(), hash.data, hash.size, pub_bin.data(), scalar.data());
  } else {
    ED25519_sign(sig.data(), hash.data, hash.size, pub_bin.data(), K.ed_bin.data());
  }
  return sig;
}

}  // namespace coinbase::crypto
