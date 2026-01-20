#include "base_ecc_secp256k1.h"

// clang-format off
#include "secp256k1/src/assumptions.h"
#include "secp256k1/src/field_impl.h"
#include "secp256k1/src/scalar_impl.h"
#include "secp256k1/src/scratch_impl.h"
#include "secp256k1/src/group_impl.h"
#include "secp256k1/src/ecmult_impl.h"
#include "secp256k1/src/ecmult_const_impl.h"
#include "secp256k1/src/ecmult_gen_impl.h"
#include "secp256k1/src/eckey_impl.h"
#include "secp256k1/src/int128_impl.h"
#include "secp256k1/src/precomputed_ecmult.c"
#include "secp256k1/src/precomputed_ecmult_gen.c"
// clang-format on

namespace coinbase::crypto {

bool curve_x_to_y(const bn_t& P, const bn_t& A, const bn_t& B, const bn_t& x, bn_t& y);
error_t ossl_ecdsa_verify(const EC_GROUP* group, EC_POINT* point, mem_t hash, mem_t signature);
buf_t ossl_ecdsa_sign(const EC_GROUP* group, BIGNUM* x, mem_t hash);
buf_t ossl_pub_to_x509(const EC_GROUP* group, const EC_POINT* point);
buf_t ossl_prv_to_pkcs8(const EC_GROUP* group, const BIGNUM* x);
EC_GROUP* ossl_get_optimized_curve(int type);
bool ossl_equ_groups(const EC_GROUP* g1, const EC_GROUP* g2);
bn_t ossl_get_p(const EC_GROUP* group);

static secp256k1_gej G;
static secp256k1_ecmult_gen_context secp256k1_ecmult_gen_ctx = {0};

namespace secp256k1 {
point_ptr_t new_point(const point_ptr_t src) { return point_ptr_t(new secp256k1_gej(*(const secp256k1_gej*)src)); }
}  // namespace secp256k1

// Constant-time addition for secp256k1_gej points for all inputs (including infinity).
// Implementation strategy:
// - Use libsecp256k1's `secp256k1_gej_add_ge` (side-channel resistant, handles degeneracy),
//   which requires the second operand in affine form and non-infinity.
// - If the second operand is infinity, cmov it to generator (any fixed non-infinity point),
//   perform the addition, then cmov the result back to `a`.
static void secp256k1_gej_add_consttime_full(secp256k1_gej* r, const secp256k1_gej* a, const secp256k1_gej* b) {
  SECP256K1_GEJ_VERIFY(a);
  SECP256K1_GEJ_VERIFY(b);

  const int b_is_inf = b->infinity;

  secp256k1_gej b_noninf = *b;
  secp256k1_gej_cmov(&b_noninf, &G, b_is_inf);  // ensure non-infinity for affine conversion

  // Convert a copy of b_noninf to affine (constant-time inversion via secp256k1_fe_inv).
  secp256k1_ge b_ge;
  secp256k1_gej b_tmp = b_noninf;
  secp256k1_ge_set_gej(&b_ge, &b_tmp);
  VERIFY_CHECK(!b_ge.infinity);

  // Add in (side-channel resistant) constant-time.
  secp256k1_gej_add_ge(r, a, &b_ge);

  // If original `b` was infinity, the result should be `a`.
  secp256k1_gej_cmov(r, a, b_is_inf);

  SECP256K1_GEJ_VERIFY(r);
}

ecurve_secp256k1_t::ecurve_secp256k1_t() noexcept {
  name = "SECP256K1";
  type = ecurve_type_e::bitcoin;
  openssl_code = NID_secp256k1;
  bits = 256;

  group = ossl_get_optimized_curve(openssl_code);

  bn_t q_value;
  int res = EC_GROUP_get_order(group, q_value, NULL);
  cb_assert(res);
  q = mod_t(q_value, /* multiplicative_dense */ true);

  secp256k1_gej_set_ge(&G, &secp256k1_ge_const_g);
  secp256k1_ecmult_gen_context_build(&secp256k1_ecmult_gen_ctx);
}

const mod_t& ecurve_secp256k1_t::order() const { return q; }

const mod_t& ecurve_secp256k1_t::p() const {
  static const mod_t mod_p(ossl_get_p(group), /* multiplicative_dense */ true);
  return mod_p;
}

const ecc_generator_point_t& ecurve_secp256k1_t::generator() const {
  static ecc_generator_point_t gen = ecc_point_t(secp256k1::point_ptr_t(&G));
  return gen;
}

void ecurve_secp256k1_t::get_params(bn_t& p, bn_t& a, bn_t& b) const {
  cb_assert(group);
  EC_GROUP_get_curve(group, p, a, b, bn_t::thread_local_storage_bn_ctx());
}

static void bzero(secp256k1_scalar& scalar) { coinbase::bzero(byte_ptr(&scalar), sizeof(secp256k1_scalar)); }

void ecurve_secp256k1_t::init_point(ecc_point_t& P) const {
  auto ptr = new secp256k1_gej;
  secp256k1_gej_set_infinity(ptr);
  P.secp256k1 = secp256k1::point_ptr_t(ptr);
}

void ecurve_secp256k1_t::free_point(ecc_point_t& P) const { delete (secp256k1_gej*)P.secp256k1; }

void ecurve_secp256k1_t::copy_point(ecc_point_t& Dst, const ecc_point_t& Src) const {
  Dst.secp256k1 = secp256k1::new_point(Src.secp256k1);
}

bool ecurve_secp256k1_t::cnd_copy_point(bool flag, const ecc_point_t& Src, ecc_point_t& Dst) const {
  secp256k1_gej_cmov((secp256k1_gej*)Dst.secp256k1, (const secp256k1_gej*)Src.secp256k1, static_cast<int>(flag));
  return true;
}

bool ecurve_secp256k1_t::is_on_curve(const ecc_point_t& P) const {
  secp256k1_ge ge;
  secp256k1_ge_set_gej(&ge, (secp256k1_gej*)P.secp256k1);
  return 0 != secp256k1_ge_is_valid_var(&ge);
}

bool ecurve_secp256k1_t::is_in_subgroup(const ecc_point_t& P) const { return is_on_curve(P); }

bool ecurve_secp256k1_t::is_infinity(const ecc_point_t& P) const {
  return 0 != secp256k1_gej_is_infinity((const secp256k1_gej*)P.secp256k1);
}

void ecurve_secp256k1_t::set_infinity(ecc_point_t& P) const { secp256k1_gej_set_infinity((secp256k1_gej*)P.secp256k1); }

bool ecurve_secp256k1_t::equ_points(const ecc_point_t& P1, const ecc_point_t& P2) const {
  if (is_infinity(P1)) return is_infinity(P2);
  if (is_infinity(P2)) return is_infinity(P1);

  secp256k1_ge ge1, ge2;
  secp256k1_ge_set_gej(&ge1, (secp256k1_gej*)P1.secp256k1);
  secp256k1_ge_set_gej(&ge2, (secp256k1_gej*)P2.secp256k1);
  return secp256k1_fe_equal(&ge1.x, &ge2.x) && secp256k1_fe_equal(&ge1.y, &ge2.y);
}

void ecurve_secp256k1_t::invert_point(ecc_point_t& P) const {
  secp256k1_gej_neg((secp256k1_gej*)P.secp256k1, (const secp256k1_gej*)P.secp256k1);
}

void ecurve_secp256k1_t::add(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const {
  secp256k1_gej_add_var((secp256k1_gej*)R.secp256k1, (const secp256k1_gej*)P1.secp256k1,
                        (const secp256k1_gej*)P2.secp256k1, nullptr);
}

void ecurve_secp256k1_t::add_consttime(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const {
  secp256k1_gej_add_consttime_full((secp256k1_gej*)R.secp256k1, (const secp256k1_gej*)P1.secp256k1,
                                   (const secp256k1_gej*)P2.secp256k1);
}

void ecurve_secp256k1_t::mul_vartime(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const {
  bn_t xx = q.mod(x);

  buf_t bin = xx.to_bin(32);
  secp256k1_scalar scalar_x;
  secp256k1_scalar_set_b32(&scalar_x, bin.data(), nullptr);

  secp256k1_ecmult((secp256k1_gej*)R.secp256k1, (const secp256k1_gej*)P.secp256k1, &scalar_x, nullptr);

  secp256k1_ge a;
  secp256k1_ge_set_gej_var(&a, (secp256k1_gej*)P.secp256k1);

  bzero(scalar_x);
}

void ecurve_secp256k1_t::mul(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const {
  bn_t xx = q.mod(x);

  buf_t bin = xx.to_bin(32);
  secp256k1_scalar scalar_x;
  secp256k1_scalar_set_b32(&scalar_x, bin.data(), nullptr);

  secp256k1_ge a;
  secp256k1_ge_set_gej(&a, (secp256k1_gej*)P.secp256k1);

  secp256k1_ecmult_const((secp256k1_gej*)R.secp256k1, &a, &scalar_x);
  bzero(scalar_x);
}

void ecurve_secp256k1_t::mul_add(const bn_t& n, const ecc_point_t& P, const bn_t& m, ecc_point_t& R) const {
  bn_t nn;
  MODULO(q) nn = n + 0;
  bn_t mm;
  MODULO(q) mm = m + 0;

  buf_t bin_n = nn.to_bin(32);
  buf_t bin_m = mm.to_bin(32);
  secp256k1_scalar scalar_n;
  secp256k1_scalar scalar_m;
  secp256k1_scalar_set_b32(&scalar_n, bin_n.data(), nullptr);
  secp256k1_scalar_set_b32(&scalar_m, bin_m.data(), nullptr);

  if (is_vartime_scope()) {
    // Verifier code paths allow variable-time operations and require robustness for all inputs.
    secp256k1_ecmult((secp256k1_gej*)R.secp256k1, (const secp256k1_gej*)P.secp256k1, &scalar_m, &scalar_n);
    secp256k1_scalar_clear(&scalar_m);
    secp256k1_scalar_clear(&scalar_n);
    return;
  }

  secp256k1_gej Rn;
  secp256k1_ecmult_gen(&secp256k1_ecmult_gen_ctx, &Rn, &scalar_n);

  // Convert P to affine coordinates for secp256k1_ecmult_const.
  // Use the constant-time conversion (secp256k1_fe_inv via secp256k1_ge_set_gej).
  // Also ensure we never feed infinity into secp256k1_ge_set_gej by cmov'ing to generator.
  secp256k1_gej Pj = *(const secp256k1_gej*)P.secp256k1;
  const int P_is_inf = Pj.infinity;
  secp256k1_gej_cmov(&Pj, &G, P_is_inf);

  secp256k1_ge P_ge;
  secp256k1_ge_set_gej(&P_ge, &Pj);
  VERIFY_CHECK(!P_ge.infinity);

  // Compute Rm = mP in constant-time
  secp256k1_gej Rm;
  secp256k1_ecmult_const(&Rm, &P_ge, &scalar_m);

  // If original P was infinity, mP is infinity.
  secp256k1_gej inf;
  secp256k1_gej_set_infinity(&inf);
  secp256k1_gej_cmov(&Rm, &inf, P_is_inf);

  secp256k1_gej R_sum;
  secp256k1_gej_add_consttime_full(&R_sum, &Rm, &Rn);
  memcpy(R.secp256k1, &R_sum, sizeof(R_sum));

  secp256k1_scalar_clear(&scalar_m);
  secp256k1_scalar_clear(&scalar_n);
}

void ecurve_secp256k1_t::mul_to_generator_vartime(const bn_t& x, ecc_point_t& P) const { mul_to_generator(x, P); }

void ecurve_secp256k1_t::mul_to_generator(const bn_t& x, ecc_point_t& P) const {
  bn_t xx = x % q;

  buf_t bin = xx.to_bin(32);
  secp256k1_scalar scalar_x;
  secp256k1_scalar_set_b32(&scalar_x, bin.data(), nullptr);

  secp256k1_ecmult_gen(&secp256k1_ecmult_gen_ctx, (secp256k1_gej*)P.secp256k1, &scalar_x);
  bzero(scalar_x);
}

int ecurve_secp256k1_t::to_compressed_bin(const ecc_point_t& P, byte_ptr out) const {
  if (out) {
    size_t size = 0;
    secp256k1_ge ge;
    secp256k1_ge_set_gej(&ge, (secp256k1_gej*)P.secp256k1);
    secp256k1_eckey_pubkey_serialize(&ge, out, &size, 1);
  }
  return 33;
}

int ecurve_secp256k1_t::to_bin(const ecc_point_t& P, byte_ptr out) const {
  if (out) {
    size_t size = 0;
    secp256k1_ge ge;
    secp256k1_ge_set_gej(&ge, (secp256k1_gej*)P.secp256k1);
    secp256k1_eckey_pubkey_serialize(&ge, out, &size, 0);
  }
  return 65;
}

error_t ecurve_secp256k1_t::from_bin(ecc_point_t& P, mem_t bin) const {
  secp256k1_ge ge;
  if (0 == secp256k1_eckey_pubkey_parse(&ge, bin.data, bin.size))
    return coinbase::error(E_CRYPTO, "secp256k1_eckey_pubkey_parse failed");
  secp256k1_gej_set_ge((secp256k1_gej*)P.secp256k1, &ge);
  return SUCCESS;
}

void ecurve_secp256k1_t::get_coordinates(const ecc_point_t& P, bn_t& x, bn_t& y) const {
  if (is_infinity(P)) {
    x = y = 0;
    return;
  }

  buf_t buf(65);
  to_bin(P, buf.data());
  x = bn_t::from_bin(buf.range(1, 32));
  y = bn_t::from_bin(buf.range(33, 32));
}

bool ecurve_secp256k1_t::hash_to_point(mem_t bin, ecc_point_t& Q) const {
  if (bin.size != size()) return false;
  buf_t oct(1 + bin.size);
  memmove(oct.data() + 1, bin.data, bin.size);
  oct[0] = 2;
  if (0 == from_bin(Q, oct)) return true;
  return false;
}

static EC_POINT* to_ossl_point(const EC_GROUP* group, secp256k1::point_ptr_t ptr) {
  byte_t bin[65];

  size_t size = 0;
  secp256k1_ge ge;
  secp256k1_ge_set_gej(&ge, (secp256k1_gej*)ptr);
  secp256k1_eckey_pubkey_serialize(&ge, bin, &size, 0);

  EC_POINT* point = EC_POINT_new(group);
  EC_POINT_oct2point(group, point, bin, 65, bn_t::thread_local_storage_bn_ctx());
  return point;
}

error_t ecurve_secp256k1_t::verify(const ecc_pub_key_t& P, mem_t hash, mem_t sig) const {
  scoped_ptr_t<EC_POINT> point = to_ossl_point(group, P.secp256k1);
  return ossl_ecdsa_verify(group, point, hash, sig);
}

buf_t ecurve_secp256k1_t::sign(const ecc_prv_key_t& K, mem_t hash) const {
  return ossl_ecdsa_sign(group, K.value(), hash);
}

buf_t ecurve_secp256k1_t::pub_to_der(const ecc_pub_key_t& P) const {
  cb_assert("not implemented");
  return buf_t();
}

buf_t ecurve_secp256k1_t::prv_to_der(const ecc_prv_key_t& K) const {
  cb_assert("not implemented");
  return buf_t();
}

void ecurve_secp256k1_t::set_ossl_point(ecc_point_t& P, const EC_POINT* point) const {
  byte_t buf[65];
  EC_POINT_point2oct(group, point, POINT_CONVERSION_UNCOMPRESSED, buf, 65, bn_t::thread_local_storage_bn_ctx());
  from_bin(P, mem_t(buf, 65));
}

error_t ecurve_secp256k1_t::pub_from_der(ecc_pub_key_t& P, mem_t der) const {
  cb_assert("not implemented");
  return coinbase::error(E_NOT_SUPPORTED);
}

error_t ecurve_secp256k1_t::prv_from_der(ecc_prv_key_t& K, mem_t der) const {
  cb_assert("not implemented");
  return coinbase::error(E_NOT_SUPPORTED);
}

namespace bip340 {

template <typename... ARGS>
static buf_t hash(const std::string& tag, const ARGS&... args) {
  buf_t h = crypto::sha256_t::hash(tag);
  return crypto::sha256_t::hash(h, h, args...);
}

bn_t hash_message(const bn_t& rx, const ecc_point_t& pub_key, mem_t message) {
  return bn_t::from_bin(hash("BIP0340/challenge", rx.to_bin(32), pub_key.get_x().to_bin(32), message)) %
         crypto::curve_secp256k1.order();
}

error_t verify(const ecc_point_t& pub_key, mem_t m, mem_t sig) {
  error_t rv = UNINITIALIZED_ERROR;
  if (sig.size != 64) return coinbase::error(E_BADARG, "BIP340 verify: sig size != 64");

  ecurve_t curve = curve_secp256k1;
  const mod_t& q = curve.order();
  const ecc_generator_point_t& G = curve.generator();
  if (pub_key.get_curve() != curve) return coinbase::error(E_BADARG, "BIP340 verify: only secp256k1 supported");

  bn_t r = bn_t::from_bin(sig.take(32));
  if (r >= curve.p()) return coinbase::error(E_CRYPTO, "BIP340 verify: sig r not in the field");

  bn_t s = bn_t::from_bin(sig.skip(32));
  if (s >= q) return coinbase::error(E_CRYPTO, "BIP340 verify: sig s not in [0, q)");

  buf_t oct = pub_key.to_compressed_bin();
  oct[0] = 2;  // even only
  ecc_point_t Q;
  if (rv = Q.from_bin(curve, oct)) return rv;

  bn_t e = hash_message(r, Q, m);
  ecc_point_t R = s * G - e * Q;
  if (R.is_infinity()) return coinbase::error(E_CRYPTO, "BIP340 verify: R is infinity");
  if (R.get_y().is_odd()) return coinbase::error(E_CRYPTO, "BIP340 verify: R.y is odd");
  if (r != R.get_x()) return coinbase::error(E_CRYPTO, "BIP340 verify: r != R.x");

  return SUCCESS;
}

}  // namespace bip340

}  // namespace coinbase::crypto
