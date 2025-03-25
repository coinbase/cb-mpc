#include <cbmpc/core/log.h>
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>
#include <cbmpc/crypto/scope.h>

#include "base_ecc_secp256k1.h"
#include "base_eddsa.h"
#include "ec25519_core.h"

namespace coinbase::crypto {

class ecurve_ossl_t final : public ecurve_interface_t {
 public:
  ecurve_ossl_t(int code) noexcept;
  const mod_t& order() const override;
  const mod_t& p() const override;
  const ecc_generator_point_t& generator() const override;
  void get_params(bn_t& p, bn_t& a, bn_t& b) const override;
  void mul_to_generator(const bn_t& val, ecc_point_t& P) const override;
  void mul_to_generator_vartime(const bn_t& val, ecc_point_t& P) const override;
  void init_point(ecc_point_t& P) const override;
  void set_ossl_point(ecc_point_t& P, const EC_POINT* point) const override;
  void free_point(ecc_point_t& P) const override;
  void copy_point(ecc_point_t& Dst, const ecc_point_t& Src) const override;
  bool is_on_curve(const ecc_point_t& P) const override;
  bool is_in_subgroup(const ecc_point_t& P) const override;
  bool is_infinity(const ecc_point_t& P) const override;
  void set_infinity(ecc_point_t& P) const override;
  void invert_point(ecc_point_t& P) const override;
  bool equ_points(const ecc_point_t& P1, const ecc_point_t& P2) const override;
  void add(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const override;
  void add_consttime(const ecc_point_t& P, const ecc_point_t& x, ecc_point_t& R) const override;
  void mul(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const override;
  void mul_vartime(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const override;
  void mul_add(const bn_t& n, const ecc_point_t& P, const bn_t& m, ecc_point_t& R) const override;  // G*n + P*m
  int to_compressed_bin(const ecc_point_t& P, byte_ptr out) const override;
  int to_bin(const ecc_point_t& P, byte_ptr out) const override;
  error_t from_bin(ecc_point_t& P, mem_t bin) const override;
  void get_coordinates(const ecc_point_t& P, bn_t& x, bn_t& y) const override;
  void set_coordinates(ecc_point_t& P, const bn_t& x, const bn_t& y) const override;
  bool hash_to_point(mem_t bin, ecc_point_t& Q) const override;
  buf_t pub_to_der(const ecc_pub_key_t& P) const override;
  buf_t prv_to_der(const ecc_prv_key_t& K) const override;
  error_t pub_from_der(ecc_pub_key_t& P, mem_t der) const override;
  error_t prv_from_der(ecc_prv_key_t& K, mem_t der) const override;
  buf_t sign(const ecc_prv_key_t& K, mem_t hash) const override;
  error_t verify(const ecc_pub_key_t& P, mem_t hash, mem_t sig) const override;

 private:
  mod_t q;
  mod_t _p;
  ecc_generator_point_t gen;
};

static const ecurve_ossl_t p256_info(NID_X9_62_prime256v1);
static const ecurve_ossl_t p384_info(NID_secp384r1);
static const ecurve_ossl_t p521_info(NID_secp521r1);
static const ecurve_secp256k1_t secp256k1_info;
static const ecurve_ed_t ed25519_info;

static const ecurve_interface_t* g_curves[] = {&p256_info, &p384_info, &p521_info, &secp256k1_info, &ed25519_info};

ecurve_t const curve_p256 = ecurve_t(g_curves[0]);
ecurve_t const curve_p384 = ecurve_t(g_curves[1]);
ecurve_t const curve_p521 = ecurve_t(g_curves[2]);
ecurve_t const curve_secp256k1 = ecurve_t(g_curves[3]);
ecurve_t const curve_ed25519 = ecurve_t(g_curves[4]);

// -------------------- ossl_ -------------------------------
bool ossl_equ_groups(const EC_GROUP* g1, const EC_GROUP* g2) {
  const EC_POINT* p1 = EC_GROUP_get0_generator(g1);
  const EC_POINT* p2 = EC_GROUP_get0_generator(g2);
  bn_t x1, y1, x2, y2;
  EC_POINT_get_affine_coordinates(g1, p1, x1, y1, NULL);
  EC_POINT_get_affine_coordinates(g2, p2, x2, y2, NULL);
  return x1 == x2 && y1 == y2;
}

static const char* crypto_ec_group_2_name(const EC_GROUP* group) {
  int id = EC_GROUP_get_curve_name(group);
  switch (id) {
    case NID_X9_62_prime256v1:
      return SN_X9_62_prime256v1;
    case NID_secp256k1:
      return SN_secp256k1;
  }
  return nullptr;
}

error_t ossl_ecdsa_verify(const EC_GROUP* group, EC_POINT* point, mem_t hash, mem_t signature) {
  uint8_t oct[65];
  cb_assert(EC_POINT_point2oct(group, point, POINT_CONVERSION_UNCOMPRESSED, oct, 65,
                               bn_t::thread_local_storage_bn_ctx()) > 0);

  OSSL_PARAM_BLD* param_bld = OSSL_PARAM_BLD_new();
  cb_assert(param_bld);
  cb_assert(OSSL_PARAM_BLD_push_utf8_string(param_bld, "group", crypto_ec_group_2_name(group), 0) > 0);
  cb_assert(OSSL_PARAM_BLD_push_octet_string(param_bld, "pub", oct, 65) > 0);
  OSSL_PARAM* params = OSSL_PARAM_BLD_to_param(param_bld);
  cb_assert(params);
  EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
  cb_assert(ctx);
  cb_assert(EVP_PKEY_fromdata_init(ctx) > 0);
  EVP_PKEY* pkey = NULL;
  cb_assert(EVP_PKEY_fromdata(ctx, &pkey, EVP_PKEY_PUBLIC_KEY, params) > 0);
  EVP_PKEY_CTX_free(ctx);
  OSSL_PARAM_BLD_free(param_bld);
  OSSL_PARAM_free(params);

  ctx = EVP_PKEY_CTX_new(pkey, NULL);
  cb_assert(ctx);
  cb_assert(EVP_PKEY_verify_init(ctx) > 0);
  int res = EVP_PKEY_verify(ctx, signature.data, signature.size, hash.data, hash.size);
  EVP_PKEY_CTX_free(ctx);

  EVP_PKEY_free(pkey);
  if (res != 1) return coinbase::error(E_CRYPTO, "EVP_PKEY_verify failed in ossl_ecdsa_verify");
  return SUCCESS;
}

buf_t ossl_ecdsa_sign(const EC_GROUP* group, BIGNUM* x, mem_t hash) {
  OSSL_PARAM_BLD* param_bld = OSSL_PARAM_BLD_new();
  cb_assert(param_bld);
  cb_assert(OSSL_PARAM_BLD_push_utf8_string(param_bld, "group", crypto_ec_group_2_name(group), 0) > 0);
  cb_assert(OSSL_PARAM_BLD_push_BN(param_bld, "priv", x) > 0);
  OSSL_PARAM* params = OSSL_PARAM_BLD_to_param(param_bld);
  cb_assert(params);
  EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
  cb_assert(ctx);
  cb_assert(EVP_PKEY_fromdata_init(ctx) > 0);
  EVP_PKEY* pkey = NULL;
  cb_assert(EVP_PKEY_fromdata(ctx, &pkey, EVP_PKEY_KEYPAIR, params) > 0);
  EVP_PKEY_CTX_free(ctx);
  OSSL_PARAM_BLD_free(param_bld);
  OSSL_PARAM_free(params);

  ctx = EVP_PKEY_CTX_new(pkey, NULL);
  cb_assert(ctx);
  cb_assert(EVP_PKEY_sign_init(ctx) > 0);
  size_t siglen = 0;
  cb_assert(EVP_PKEY_sign(ctx, nullptr, &siglen, hash.data, hash.size) > 0);
  buf_t der((int)siglen);
  cb_assert(EVP_PKEY_sign(ctx, der.data(), &siglen, hash.data, hash.size) > 0);
  EVP_PKEY_CTX_free(ctx);

  EVP_PKEY_free(pkey);
  return der;
}

// --------------------- ecurve_ossl_t -----------------------

EC_GROUP* ossl_get_optimized_curve(int type) {
  EC_GROUP* group = EC_GROUP_new_by_curve_name(type);
  cb_assert(group);

  EC_GROUP_set_asn1_flag(group, OPENSSL_EC_NAMED_CURVE);
  return group;
}

bn_t ossl_get_p(const EC_GROUP* group) {
  cb_assert(group);
  bn_t p;
  int res = EC_GROUP_get_curve(group, p, nullptr, nullptr, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
  return p;
}

ecurve_ossl_t::ecurve_ossl_t(int code) noexcept {
  type = ecurve_type_e::ossl;
  openssl_code = code;
  switch (code) {
    case NID_X9_62_prime256v1:
      name = "P256";
      bits = 256;
      break;
    case NID_secp384r1:
      name = "P384";
      bits = 384;
      break;
    case NID_secp521r1:
      name = "P521";
      bits = 521;
      break;
    case NID_secp256k1:
      name = "SECP256K1";
      bits = 256;
      break;
  }

  group = ossl_get_optimized_curve(openssl_code);
  gen.curve = ecurve_t(this);
  gen.ptr = (EC_POINT*)EC_GROUP_get0_generator(group);  // never destroyed

  bn_t q_value;
  int res = EC_GROUP_get_order(group, q_value, NULL);
  q = mod_t(q_value, /* multiplicative_dense */ true);
  _p = mod_t(ossl_get_p(group), /* multiplicative_dense */ true);
  cb_assert(res);
}

void ecurve_ossl_t::get_params(bn_t& p, bn_t& a, bn_t& b) const {
  cb_assert(group);
  int res = EC_GROUP_get_curve(group, p, a, b, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
}

const mod_t& ecurve_ossl_t::p() const { return _p; }

const mod_t& ecurve_ossl_t::order() const { return q; }
const ecc_generator_point_t& ecurve_ossl_t::generator() const { return gen; }

void ecurve_ossl_t::mul_to_generator_vartime(const bn_t& val, ecc_point_t& P) const { mul_to_generator(val, P); }

void ecurve_ossl_t::mul_to_generator(const bn_t& val, ecc_point_t& P) const {
  int res = EC_POINT_mul(group, P, val, nullptr, nullptr, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
}

void ecurve_ossl_t::init_point(ecc_point_t& P) const {
  P.ptr = EC_POINT_new(group);
  if (!P.ptr) throw std::bad_alloc();
}

void ecurve_ossl_t::free_point(ecc_point_t& P) const { EC_POINT_free(P.ptr); }

void ecurve_ossl_t::copy_point(ecc_point_t& Dst, const ecc_point_t& Src) const {
  Dst.ptr = EC_POINT_dup(Src.ptr, group);
  if (!Dst.ptr) throw std::bad_alloc();
}

bool ecurve_ossl_t::is_on_curve(const ecc_point_t& P) const {
  return 0 < EC_POINT_is_on_curve(group, P, bn_t::thread_local_storage_bn_ctx());
}
bool ecurve_ossl_t::is_in_subgroup(const ecc_point_t& P) const { return is_on_curve(P); }
bool ecurve_ossl_t::is_infinity(const ecc_point_t& P) const { return 0 < EC_POINT_is_at_infinity(group, P); }
void ecurve_ossl_t::set_infinity(ecc_point_t& P) const { EC_POINT_set_to_infinity(group, P); }

void ecurve_ossl_t::invert_point(ecc_point_t& P) const {
  int res = EC_POINT_invert(group, P, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
}

bool ecurve_ossl_t::equ_points(const ecc_point_t& P1, const ecc_point_t& P2) const {
  return 0 == EC_POINT_cmp(group, P1, P2, bn_t::thread_local_storage_bn_ctx());
}

void ecurve_ossl_t::add(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const {
  int res = EC_POINT_add(group, R, P1, P2, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
}

void ecurve_ossl_t::add_consttime(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const {
  cb_assert(!P1.is_infinity());
  cb_assert(!P2.is_infinity());

  bn_t x1, y1, x2, y2;
  get_coordinates(P1, x1, y1);
  get_coordinates(P2, x2, y2);
  cb_assert(!p().sub(x2, x1).is_zero() && "Degenerate addition: Δx = 0");
  cb_assert(!p().sub(y2, y1).is_zero() && "Degenerate addition: Δy = 0");

  int res = EC_POINT_add(group, R, P1, P2, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
}

void ecurve_ossl_t::mul_vartime(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const { mul(P, x, R); }

void ecurve_ossl_t::mul(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const {
  int res = EC_POINT_mul(group, R, nullptr, P, x, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
}

void ecurve_ossl_t::get_coordinates(const ecc_point_t& P, bn_t& x, bn_t& y) const {
  int res = EC_POINT_get_affine_coordinates(group, P, x, y, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
}

void ecurve_ossl_t::set_coordinates(ecc_point_t& P, const bn_t& x, const bn_t& y) const {
  int res = EC_POINT_set_affine_coordinates(group, P, x, y, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
}

void ecurve_ossl_t::set_ossl_point(ecc_point_t& P, const EC_POINT* point) const {
  int res = EC_POINT_copy(P.ptr, point);
  cb_assert(res);
}

void ecurve_ossl_t::mul_add(const bn_t& n, const ecc_point_t& P, const bn_t& m, ecc_point_t& R) const {
  int res = EC_POINT_mul(group, R, n, P, m, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
}

int ecurve_ossl_t::to_compressed_bin(const ecc_point_t& P, byte_ptr out) const {
  int n = 1 + size();
  if (out) {
    int res =
        (int)EC_POINT_point2oct(group, P, POINT_CONVERSION_COMPRESSED, out, n, bn_t::thread_local_storage_bn_ctx());
    cb_assert(res);
    if (res == 1) memset(out + 1, 0, n - 1);
  }
  return n;
}

int ecurve_ossl_t::to_bin(const ecc_point_t& P, byte_ptr out) const {
  int n = 1 + size() * 2;
  if (out) {
    int res =
        (int)EC_POINT_point2oct(group, P, POINT_CONVERSION_UNCOMPRESSED, out, n, bn_t::thread_local_storage_bn_ctx());
    cb_assert(res);
    if (res == 1) memset(out + 1, 0, n - 1);
  }
  return n;
}

error_t ecurve_ossl_t::from_bin(ecc_point_t& P, mem_t bin) const {
  if (bin.size > 0 && bin[0] == 0)  // infinity
  {
    if (bin.size != 1 + size() && bin.size != 1 + size() * 2) return coinbase::error(E_FORMAT);
    for (int i = 0; i < bin.size; i++)
      if (bin[i]) return coinbase::error(E_CRYPTO);
    bin.size = 1;
  }

  if (0 >= EC_POINT_oct2point(group, P, bin.data, bin.size, bn_t::thread_local_storage_bn_ctx())) {
    return openssl_error("EC_POINT_oct2point error, data-size=" + strext::itoa(bin.size));
  }
  return SUCCESS;
}

bool ecurve_ossl_t::hash_to_point(mem_t bin, ecc_point_t& Q) const {
  if (bin.size != size()) return false;
  buf_t oct(1 + bin.size);
  memmove(oct.data() + 1, bin.data, bin.size);
  oct[0] = 2;
  if (0 == from_bin(Q, oct)) return true;
  return false;
}

buf_t ecurve_ossl_t::pub_to_der(const ecc_pub_key_t& P) const {
  cb_assert("not-implemented");
  return buf_t();
}

buf_t ecurve_ossl_t::prv_to_der(const ecc_prv_key_t& K) const {
  cb_assert("not-implemented");
  return buf_t();
}

error_t ecurve_ossl_t::verify(const ecc_pub_key_t& P, mem_t hash, mem_t sig) const {
  return ossl_ecdsa_verify(group, P.ptr, hash, sig);
}
buf_t ecurve_ossl_t::sign(const ecc_prv_key_t& K, mem_t hash) const { return ossl_ecdsa_sign(group, K.value(), hash); }

error_t ecurve_ossl_t::pub_from_der(ecc_pub_key_t& P, mem_t der) const {
  cb_assert("not-implemented");
  return coinbase::error(E_NOT_SUPPORTED);
}

error_t ecurve_ossl_t::prv_from_der(ecc_prv_key_t& K, mem_t der) const {
  cb_assert("not-implemented");
  return coinbase::error(E_NOT_SUPPORTED);
}

// ----------------------- ecc_pub_key_t --------------------
error_t ecc_pub_key_t::verify(mem_t hash, mem_t signature) const { return curve.ptr->verify(*this, hash, signature); }

buf_t ecc_pub_key_t::to_der() const {
  cb_assert(curve);
  buf_t der = curve.ptr->pub_to_der(*this);
  return der;
}

error_t ecc_pub_key_t::verify_schnorr(mem_t message, mem_t signature) const {
  if (signature.size != curve.size() * 2) return coinbase::error(E_FORMAT);

  const auto& G = curve.generator();
  const mod_t& q = curve.order();
  ecc_point_t Q = *this;

  bn_t e = bn_t::from_bin(signature.take(curve.size()));
  bn_t s = bn_t::from_bin(signature.skip(curve.size()));

  if (e <= 0 || e >= q) return coinbase::error(E_CRYPTO);
  if (s <= 0 || s >= q) return coinbase::error(E_CRYPTO);

  ecc_point_t R = s * G + e * Q;  // s * G + e * Q = (k - e * x) * G + e * x * G = (k - e * x + e * x) * G = k * G = R

  bn_t e_tag = bn_t(crypto::sha256_t::hash(Q, R, message)) % q;
  if (e_tag != e) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

// ----------------------- ecc_prv_key_t --------------------

void ecc_prv_key_t::convert(coinbase::converter_t& c) {
  c.convert(curve);
  c.convert(val);
  c.convert(ed_bin);
}

void ecc_prv_key_t::generate(ecurve_t curve) {
  this->curve = curve;
  if (curve == curve_ed25519)
    ed_bin = gen_random(32);
  else
    val = curve.get_random_value();
}

ecc_pub_key_t ecc_prv_key_t::pub() const {
  cb_assert(curve);
  return curve.mul_to_generator(value());
}

void ecc_prv_key_t::set(ecurve_t curve, const bn_t& val) {
  this->curve = curve;
  this->val = val;
}

void ecc_prv_key_t::set_ed_bin(mem_t ed_bin) {
  this->curve = curve_ed25519;
  this->ed_bin = ed_bin;
}

bn_t ecc_prv_key_t::value() const {
  cb_assert(curve);
  bn_t x = val;
  if (!ed_bin.empty()) x = ed25519::prv_key_to_scalar(ed_bin);
  return x % curve.order();
}

buf_t ecc_prv_key_t::sign(mem_t hash) const {
  cb_assert(curve);
  return curve.ptr->sign(*this, hash);
}

sig_with_pub_key_t ecc_prv_key_t::sign_and_output_pub_key(mem_t hash) const {
  sig_with_pub_key_t sig;
  sig.Q = this->pub();
  sig.sig = sign(hash);
  return sig;
}

error_t sig_with_pub_key_t::verify(mem_t hash) const {
  error_t rv = UNINITIALIZED_ERROR;
  const auto& curve = Q.get_curve();
  if (rv = curve.check(Q)) return coinbase::error(rv, "sig_with_pub_key_t::verify: invalid public key");
  crypto::ecc_pub_key_t ecc(Q);
  if (rv = ecc.verify(hash, sig)) return coinbase::error(E_CRYPTO, "sig_with_pub_key_t::verify: invalid signature");
  return SUCCESS;
}

error_t sig_with_pub_key_t::verify_all(const ecc_point_t& Q, mem_t hash,
                                       const std::vector<sig_with_pub_key_t>& sigs)  // static
{
  error_t rv = UNINITIALIZED_ERROR;
  ecc_point_t QSum = crypto::curve_p256.infinity();
  for (const auto& s : sigs) {
    if (rv = s.verify(hash)) return rv;
    QSum += s.Q;
  }
  if (Q != QSum) return coinbase::error(E_CRYPTO, "sig_with_pub_key_t::verify_all: Schnorr public key mismatch");
  return SUCCESS;
}

buf_t ecc_prv_key_t::ecdh(const ecc_point_t& P) const {
  cb_assert(curve);
  cb_assert(curve != curve_ed25519);
  return (value() * P).get_x().to_bin(curve.size());
}

error_t ecc_prv_key_t::execute(mem_t enc_info, buf_t& dec_info) const {
  dec_info.alloc(curve_p256.size());
  return ecdh_t::execute((void*)this, enc_info, dec_info);
}

buf_t ecc_prv_key_t::sign_schnorr(mem_t message) const {
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  bn_t x = value();
  ecc_point_t Q = x * G;
  bn_t k = bn_t::rand(q);
  ecc_point_t R = k * G;
  bn_t e = bn_t(crypto::sha256_t::hash(Q, R, message)) % q;
  bn_t s;
  MODULO(q) s = k - e * x;

  return e.to_bin(curve.size()) + s.to_bin(curve.size());
}

// ------------------ ecurve_t --------------------

ecurve_t ecurve_t::find(int openssl_id) {
  if (openssl_id == 0) return nullptr;

  for (int i = 0; i < _countof(g_curves); i++) {
    ecurve_t curve = ecurve_t(g_curves[i]);
    if (curve.type() == ecurve_type_e::ossl && !curve.get_group()) continue;
    if (openssl_id == curve.get_openssl_code()) return curve;
  }
  crypto::error("Curve not found, openssl-code=" + strext::itoa(openssl_id));
  return nullptr;
}

ecurve_t ecurve_t::find(const EC_GROUP* group) {
  int name_id = EC_GROUP_get_curve_name(group);
  if (name_id) return find(name_id);

  for (int i = 0; i < _countof(g_curves); i++) {
    ecurve_t curve = ecurve_t(g_curves[i]);
    const EC_GROUP* curve_group = curve.get_group();
    if (!curve_group) continue;

    if (ossl_equ_groups(group, curve_group)) return curve;
  }
  crypto::error("Curve not found by GROUP");
  return nullptr;
}

int ecurve_t::point_bin_size() const {
  ecc_point_t dummy;
  return ptr->to_bin(dummy, nullptr);
}

int ecurve_t::compressed_point_bin_size() const {
  ecc_point_t dummy;
  return ptr->to_compressed_bin(dummy, nullptr);
}

void ecurve_t::get_params(bn_t& p, bn_t& a, bn_t& b) const { ptr->get_params(p, a, b); }

const mod_t& ecurve_t::p() const { return ptr->p(); }

bool ecurve_t::hash_to_point(mem_t bin, ecc_point_t& Q) const { return ptr->hash_to_point(bin, Q); }

ecc_point_t ecurve_t::mul_to_generator(const bn_t& val) const {
  ecc_point_t P(*this);
  if (is_vartime_scope())
    ptr->mul_to_generator_vartime(val, P);
  else
    ptr->mul_to_generator(val, P);
  return P;
}

void ecurve_interface_t::mul_add(const bn_t& n, const ecc_point_t& P, const bn_t& m, ecc_point_t& R) const {
  R = ecurve_t(this).mul_to_generator(n) + m * P;
}

ecc_point_t ecurve_t::mul_add(const bn_t& n, const ecc_point_t& P, const bn_t& m) const  // n*G + m*P
{
  ecc_point_t R(*this);
  ptr->mul_add(n, P, m, R);
  return R;
}

int ecurve_t::size() const { return coinbase::bits_to_bytes(ptr->bits); }
int ecurve_t::get_openssl_code() const { return ptr->openssl_code; }
int ecurve_t::bits() const { return ptr->bits; }
const_char_ptr ecurve_t::get_name() const { return ptr->name; }
ecurve_type_e ecurve_t::type() const { return ptr->type; }

std::ostream& operator<<(std::ostream& os, ecurve_t curve) {
  os << curve.get_name();
  return os;
}

const EC_GROUP* ecurve_t::get_group() const {
  cb_assert(ptr);
  return ptr->group;
}

const mod_t& ecurve_t::order() const { return ptr->order(); }

const ecc_generator_point_t& ecurve_t::generator() const { return ptr->generator(); }

bn_t ecurve_t::get_random_value() const { return bn_t::rand(order()); }

static thread_local int thread_local_store_allow_ecc_infinity = 0;

allow_ecc_infinity_t::allow_ecc_infinity_t() { thread_local_store_allow_ecc_infinity++; }
allow_ecc_infinity_t::~allow_ecc_infinity_t() { thread_local_store_allow_ecc_infinity--; }

error_t ecurve_t::check(const ecc_point_t& point) const {
  if (!point.valid()) return crypto::error("EC-point invalid");
  if (point.get_curve() != *this) return crypto::error("EC-point of wrong curve");
  if (!point.is_in_subgroup()) return crypto::error("EC-point is not on curve");

  if (!thread_local_store_allow_ecc_infinity) {
    if (point.is_infinity()) return crypto::error("EC-point is infinity");
  }
  return SUCCESS;
}

void ecurve_t::convert(coinbase::converter_t& converter) {
  uint16_t curve_code = ptr ? ptr->openssl_code : 0;
  converter.convert(curve_code);
  if (curve_code) {
    ecurve_t curve = ecurve_t::find(curve_code);
    if (!curve) {
      converter.set_error();
      return;
    }
    ptr = curve.ptr;
  } else
    ptr = nullptr;
}

ecc_point_t ecurve_t::infinity() const {
  ecc_point_t P(*this);
  ptr->set_infinity(P);
  return P;
}

// --------------------- ecc_point_t ------------------------

ecc_point_t::ecc_point_t(ecurve_t _curve)
    : curve(_curve),
      ptr(nullptr)  // NOLINT:cppcoreguidelines-pro-type-member-init
{
  cb_assert(curve);
  curve.ptr->init_point(*this);
}

ecc_point_t& ecc_point_t::operator=(const ecc_point_t& src) {
  if (&src != this) {
    free();
    curve = src.curve;
    if (curve) curve.ptr->copy_point(*this, src);
  }
  return *this;
}

ecc_point_t& ecc_point_t::operator=(ecc_point_t&& src)  // move assignment
{
  if (&src != this) {
    free();
    curve = src.curve;
    ptr = src.ptr;
    src.ptr = nullptr;
    src.curve = nullptr;
  }
  return *this;
}

ecc_point_t::ecc_point_t(ecurve_t _curve, const EC_POINT* _ptr)
    : curve(_curve),
      ptr(nullptr)  // NOLINT:cppcoreguidelines-pro-type-member-init
{
  cb_assert(curve.type() == ecurve_type_e::ossl);
  ptr = EC_POINT_dup(_ptr, curve.get_group());
}

ecc_point_t::ecc_point_t(const ec25519_core::point_t& ed_point)
    : curve(curve_ed25519)  // NOLINT:cppcoreguidelines-pro-type-member-init
{
  ed = ec25519_core::new_point(&ed_point);
}

namespace secp256k1 {
point_ptr_t new_point(const point_ptr_t);
}

ecc_point_t::ecc_point_t(const secp256k1::point_ptr_t p)
    : curve(curve_secp256k1)  // NOLINT:cppcoreguidelines-pro-type-member-init
{
  secp256k1 = secp256k1::new_point(p);
}

void ecc_point_t::free() {
  if (!ptr) return;
  if (!curve) return;
  curve.ptr->free_point(*this);
  curve = nullptr;
  ptr = nullptr;
}

ecc_point_t::ecc_point_t(const ecc_point_t& src)
    : curve(nullptr),
      ptr(nullptr)  // NOLINT:cppcoreguidelines-pro-type-member-init
{
  if (!src.valid()) return;
  curve = src.curve;
  if (curve) curve.ptr->copy_point(*this, src);
}

ecc_point_t::ecc_point_t(ecc_point_t&& src)  // NOLINT:cppcoreguidelines-pro-type-member-init
{
  curve = src.curve;
  ptr = src.ptr;
  src.ptr = nullptr;
}

void ecc_point_t::attach(ecurve_t _curve, EC_POINT* value) {
  cb_assert(_curve.type() == ecurve_type_e::ossl);
  free();
  curve = _curve;
  ptr = value;
}

int ecc_point_t::to_bin(byte_ptr out) const { return curve.ptr->to_bin(*this, out); }

error_t ecc_point_t::from_bin(ecurve_t curve, mem_t in) {
  error_t rv = UNINITIALIZED_ERROR;
  free();
  this->curve = curve;
  curve.ptr->init_point(*this);
  if (rv = curve.ptr->from_bin(*this, in)) return rv;
  return SUCCESS;
}

int ecc_point_t::to_compressed_bin(byte_ptr bin) const { return curve.ptr->to_compressed_bin(*this, bin); }

buf_t ecc_point_t::to_compressed_bin() const {
  int s = to_compressed_bin(nullptr);
  buf_t out(s);
  to_compressed_bin(out.data());
  return out;
}

buf_t ecc_point_t::to_bin() const {
  int s = to_bin(nullptr);
  buf_t out(s);
  to_bin(out.data());
  return out;
}

void ecc_point_t::convert(coinbase::converter_t& converter) {
  ecurve_t c = curve;
  c.convert(converter);
  if (!c) return;
  convert_fixed_curve(converter, c);
}

void ecc_point_t::convert_fixed_curve(coinbase::converter_t& converter, ecurve_t curve) {
  int n = curve.compressed_point_bin_size();

  if (converter.is_write()) {
    if (!converter.is_calc_size()) {
      cb_assert(get_curve() == curve);
      to_compressed_bin(converter.current());
    }
  } else {
    error_t rv = UNINITIALIZED_ERROR;
    if (converter.is_error() || !converter.at_least(n)) {
      converter.set_error();
      return;
    }
    if (rv = from_bin(curve, mem_t(converter.current(), n))) {
      converter.set_error(rv);
      return;
    }
    if (rv = curve.check(*this)) {
      converter.set_error(rv);
      return;
    }
  }

  converter.forward(n);
}

void ecc_point_t::get_coordinates(bn_t& x, bn_t& y) const { curve.ptr->get_coordinates(*this, x, y); }

bn_t ecc_point_t::get_x() const {
  bn_t x;
  get_x(x);
  return x;
}
bn_t ecc_point_t::get_y() const {
  bn_t y;
  get_y(y);
  return y;
}

void ecc_point_t::get_x(bn_t& x) const {
  bn_t y;
  get_coordinates(x, y);
}
void ecc_point_t::get_y(bn_t& y) const {
  bn_t x;
  get_coordinates(x, y);
}

void ecc_point_t::set_coordinates(const bn_t& x, const bn_t& y) { curve.ptr->set_coordinates(*this, x, y); }

bool ecc_point_t::is_on_curve() const {
  if (!curve) return false;
  if (!ptr) return false;
  return curve.ptr->is_on_curve(*this);
}

bool ecc_point_t::is_in_subgroup() const {
  if (!curve) return false;
  if (!ptr) return false;
  return curve.ptr->is_in_subgroup(*this);
}

bool ecc_point_t::is_infinity() const {
  if (!curve) return false;
  if (!ptr) return false;
  return curve.ptr->is_infinity(*this);
}

ecc_point_t ecc_point_t::add(const ecc_point_t& val1, const ecc_point_t& val2)  // static
{
  ecc_point_t result(val1.curve);
  val1.curve.ptr->add(val1, val2, result);
  return result;
}

ecc_point_t ecc_point_t::add_consttime(const ecc_point_t& val1, const ecc_point_t& val2)  // static
{
  ecc_point_t result(val1.curve);
  val1.curve.ptr->add_consttime(val1, val2, result);
  return result;
}

ecc_point_t ecc_point_t::sub(const ecc_point_t& val1, const ecc_point_t& val2)  // static
{
  ecc_point_t temp = val2;
  temp.invert();
  return add(val1, temp);
}

ecc_point_t ecc_point_t::mul(const ecc_point_t& val1, const bn_t& val2)  // static
{
  ecc_point_t result(val1.curve);
  if (is_vartime_scope())
    val1.curve.ptr->mul_vartime(val1, val2, result);
  else
    val1.curve.ptr->mul(val1, val2, result);
  return result;
}

ecc_point_t operator+(const ecc_point_t& val1, const ecc_point_t& val2) { return ecc_point_t::add(val1, val2); }
ecc_point_t operator-(const ecc_point_t& val1, const ecc_point_t& val2) { return ecc_point_t::sub(val1, val2); }

ecc_point_t operator*(const bn_t& val1, const ecc_point_t& val2) { return ecc_point_t::mul(val2, val1); }

ecc_point_t operator*(const bn_t& val1, const ecc_generator_point_t& val2) { return val2.curve.mul_to_generator(val1); }

ecc_point_t& ecc_point_t::operator+=(const ecc_point_t& val) {
  curve.ptr->add(val, *this, *this);
  return *this;
}

ecc_point_t& ecc_point_t::operator-=(const ecc_point_t& val) {
  ecc_point_t temp = val;
  temp.invert();
  curve.ptr->add(temp, *this, *this);
  return *this;
}

ecc_point_t& ecc_point_t::operator*=(const bn_t& val) {
  curve.ptr->mul(*this, val, *this);
  return *this;
}

void ecc_point_t::invert() {
  cb_assert(curve);
  curve.ptr->invert_point(*this);
}

ecc_point_t ecc_point_t::operator-() const {
  ecc_point_t R = *this;
  R.invert();
  return R;
}

bool ecc_point_t::operator==(const ecc_point_t& val) const {
  if (!ptr) return val.ptr == nullptr;
  if (!val.ptr) return ptr != nullptr;
  if (!curve) return false;
  if (curve != val.curve) return false;
  return curve.ptr->equ_points(*this, val);
}

bool ecc_point_t::operator!=(const ecc_point_t& val) const { return !(*this == val); }

static ECDSA_SIG* make_ecdsa_sig(const bn_t& r, const bn_t& s) {
  ECDSA_SIG* sig = ECDSA_SIG_new();
  ECDSA_SIG_set0(sig, BN_dup(r), BN_dup(s));
  return sig;
}

std::ostream& operator<<(std::ostream& os, const ecc_point_t& p) {
  if (p.is_infinity())
    os << "infinity";
  else
    os << "(" << strext::to_hex(p.get_x().to_bin().range(0, 4)) << "..., "
       << strext::to_hex(p.get_y().to_bin().range(0, 4)) << "...)";
  return os;
}

error_t ecdsa_signature_t::from_der(ecurve_t curve, mem_t in) {
  const_byte_ptr in_ptr = in.data;
  ECDSA_SIG* sig_ptr = d2i_ECDSA_SIG(NULL, &in_ptr, in.size);
  if (!sig_ptr) return coinbase::error(E_FORMAT);

  const BIGNUM* r_ptr = nullptr;
  const BIGNUM* s_ptr = nullptr;

  ECDSA_SIG_get0(sig_ptr, &r_ptr, &s_ptr);
  r = bn_t(r_ptr);
  s = bn_t(s_ptr);
  ECDSA_SIG_free(sig_ptr);

  this->curve = curve;
  return SUCCESS;
}

int ecdsa_signature_t::to_der(byte_ptr out) const {
  ECDSA_SIG* sig_ptr = make_ecdsa_sig(r, s);

  int out_size = i2d_ECDSA_SIG(sig_ptr, NULL);
  if (out && out_size > 0) i2d_ECDSA_SIG(sig_ptr, &out);

  ECDSA_SIG_free(sig_ptr);

  if (out_size <= 0) return -1;
  return out_size;
}

buf_t ecdsa_signature_t::to_der() const {
  int out_size = to_der(nullptr);
  if (out_size <= 0) return buf_t();
  buf_t out(out_size);
  to_der(out.data());
  return out;
}

void ecdsa_signature_t::convert(coinbase::converter_t& converter) {
  converter.convert(curve);
  converter.convert(r);
  converter.convert(s);
}

error_t ecdsa_signature_t::get_recovery_code(mem_t in, const ecc_point_t& pub_key, int& recovery_code) {
  error_t rv = UNINITIALIZED_ERROR;
  int curve_size = curve.size();
  if (in.size >= curve_size) in.size = curve.size();
  bn_t e = bn_t::from_bin(in);

  buf_t oct(1 + curve_size);
  oct[0] = 2;
  r.to_bin(oct.data() + 1, curve_size);
  ecc_point_t R;
  if (rv = R.from_oct(curve, oct)) return rv;
  if (rv = curve.check(R)) return coinbase::error(rv, "ecdsa_signature_t::get_recovery_code: invalid R");

  const mod_t& q = curve.order();
  const auto& G = curve.generator();

  bn_t r_inv = q.inv(r);

  ecc_point_t Q = r_inv * (s * R - e * G);
  if (Q == pub_key) {
    recovery_code = 0;
    return SUCCESS;
  }

  R.invert();
  Q = r_inv * (s * R - e * G);
  if (Q == pub_key) {
    recovery_code = 1;
    return SUCCESS;
  }

  return coinbase::error(E_CRYPTO);
}

error_t ecdsa_signature_t::recover_pub_key(mem_t in, int recovery_code, ecc_point_t& pub_key) {
  error_t rv = UNINITIALIZED_ERROR;
  if (recovery_code != 0 && recovery_code != 1) return coinbase::error(E_CRYPTO);

  int curve_size = curve.size();
  if (in.size >= curve_size) in.size = curve.size();
  bn_t e = bn_t::from_bin(in);

  buf_t oct(1 + curve_size);
  oct[0] = 2 + recovery_code;
  r.to_bin(oct.data() + 1, curve_size);
  ecc_point_t R;
  if (rv = R.from_oct(curve, oct)) return rv;
  if (rv = curve.check(R)) return coinbase::error(rv, "ecdsa_signature_t::recover_pub_key: invalid R");

  const mod_t& q = curve.order();
  const auto& G = curve.generator();
  bn_t r_inv = q.inv(r);
  pub_key = r_inv * (s * R - e * G);
  return SUCCESS;
}

// --------------------------- ecies -----------------------
void ecies_ciphertext_t::convert(coinbase::converter_t& converter) {
  E.convert_fixed_curve(converter, curve_p256);
  converter.convert(iv);
  converter.convert(encrypted);
}

error_t ecies_ciphertext_t::from_bin(mem_t mem) const {
  error_t rv = UNINITIALIZED_ERROR;
  coinbase::converter_t converter(mem);
  const_cast<ecies_ciphertext_t*>(this)->convert(converter);
  if (rv = converter.get_rv()) return rv;
  if (converter.get_offset() != mem.size) {
    return coinbase::error(E_FORMAT);
  }
  return SUCCESS;
}

int ecies_ciphertext_t::get_bin_size(int plaintext_size)  // static
{
  return curve_p256.compressed_point_bin_size() + iv_size + buf_t::get_convert_size(plaintext_size + tag_size);
}

error_t ecies_ciphertext_t::encrypt(const ecc_point_t& pub_key, mem_t aad, mem_t plain, drbg_aes_ctr_t* drbg) {
  const mod_t& q = curve_p256.order();
  bn_t e = drbg ? drbg->gen_bn(curve_p256.order()) : bn_t::rand(q);
  buf_t iv = drbg ? drbg->gen(ecies_ciphertext_t::iv_size) : gen_random(iv_size);
  return encrypt(pub_key, aad, e, iv, plain);
}

error_t ecies_ciphertext_t::encrypt(const ecc_point_t& pub_key, mem_t aad, const bn_t& e, mem_t _iv, mem_t plain) {
  cb_assert(_iv.size == iv_size);
  memmove(iv, _iv.data, iv_size);

  const auto& G = curve_p256.generator();
  E = e * G;

  buf_t secret = (e * pub_key).get_x().to_bin(32);
  buf_t aes_key = crypto::sha256_t::hash(secret);
  aes_gcm_t::encrypt(aes_key, _iv, aad, tag_size, plain, encrypted);
  return SUCCESS;
}

error_t ecies_ciphertext_t::decrypt(const ecdh_t& ecdh, mem_t encrypted, mem_t aad, buf_t& decrypted)  // static
{
  error_t rv = UNINITIALIZED_ERROR;
  ecies_ciphertext_t ecies;
  if (rv = coinbase::convert(ecies, encrypted)) return rv;
  return ecies.decrypt(ecdh, aad, decrypted);
}

error_t ecies_ciphertext_t::decrypt(const ecdh_t& ecdh, mem_t aad, buf_t& decrypted) {
  buf_t secret;
  error_t rv = ecdh.execute(E, secret);
  if (rv) return rv;
  if (rv = decrypt_end(aad, secret, decrypted)) return rv;
  return SUCCESS;
}

error_t ecies_ciphertext_t::decrypt_begin(buf_t& enc_info) const {
  enc_info = E.to_oct();
  return SUCCESS;
}

error_t ecies_ciphertext_t::decrypt_end(mem_t aad, mem_t shared_secret, buf_t& out) const {
  if (shared_secret.size != 32) return coinbase::error(E_BADARG);
  buf_t aes_key = crypto::sha256_t::hash(shared_secret);
  return aes_gcm_t::decrypt(aes_key, mem_t(iv, iv_size), aad, tag_size, encrypted, out);
}

// -------------------------------- ecdh_t ----------------------

error_t ecdh_t::execute(const ecc_point_t& P, buf_t& out) const {
  if (key) {
    out = key->ecdh(P);
    return SUCCESS;
  } else {
    buf_t pub_oct = P.to_oct();
    int size = P.get_curve().size();
    return exec(ctx, cmem_t(pub_oct), cmem_t{out.alloc(size), size});
  }
}

error_t ecdh_t::execute(void* ctx, cmem_t pub_key, cmem_t out_secret)  // static
{
  error_t rv = UNINITIALIZED_ERROR;
  const ecc_prv_key_t* key = (const ecc_prv_key_t*)ctx;
  ecurve_t curve = key->get_curve();
  if (out_secret.size != curve.size()) return coinbase::error(E_BADARG, "Bad ECDH size");

  ecc_point_t P;
  {
    dylog_disable_scope_t dylog_disable_scope;
    if (rv = P.from_oct(curve, pub_key)) return rv;
  }

  buf_t out = key->ecdh(P);
  memmove(out_secret.data, out.data(), out_secret.size);
  return SUCCESS;
}

ecc_point_t extended_ec_mul_add_ct(const bn_t& x0, const ecc_point_t& P0, const bn_t& x1, const ecc_point_t& P1) {
  if (is_vartime_scope()) {
    return x0 * P0 + x1 * P1;
  } else {
    return ecc_point_t::add_consttime(x0 * P0, x1 * P1);
  }
}

}  // namespace coinbase::crypto
