#include "curve.h"

#include <memory>

#include <cbmpc/core/buf.h>
#include <cbmpc/crypto/base.h>

using namespace coinbase;
using namespace coinbase::crypto;

// ============ Curve Operations ================

ecurve_ref new_ecurve(int curve_code) {
  ecurve_t* curve = new ecurve_t(ecurve_t::find(curve_code));
  return ecurve_ref{curve};
}

void free_ecurve(ecurve_ref ref) {
  if (ref.opaque) {
    delete static_cast<ecurve_t*>(ref.opaque);
  }
}

void free_ecc_point(ecc_point_ref ref) {
  if (ref.opaque) {
    delete static_cast<ecc_point_t*>(ref.opaque);
  }
}

ecc_point_ref ecurve_generator(ecurve_ref* curve) {
  ecurve_t* curve_obj = static_cast<ecurve_t*>(curve->opaque);
  // Create a new point and copy the generator into it
  ecc_point_t* generator = new ecc_point_t();
  const ecc_generator_point_t& gen = curve_obj->generator();
  *generator = gen;  // This should copy the generator point
  return ecc_point_ref{generator};
}

cmem_t ecurve_order(ecurve_ref* curve) {
  ecurve_t* curve_obj = static_cast<ecurve_t*>(curve->opaque);
  bn_t order = curve_obj->order();
  buf_t order_buf = order.to_bin();
  return order_buf.to_cmem();
}

int ecurve_get_curve_code(ecurve_ref* curve) {
  ecurve_t* curve_obj = static_cast<ecurve_t*>(curve->opaque);
  return curve_obj->get_openssl_code();
}

ecc_point_ref ecc_point_from_bytes(cmem_t point_bytes) {
  ecc_point_t* point = new ecc_point_t();
  error_t err = coinbase::deser(mem_t(point_bytes), *point);
  if (err) {
    delete point;
    return ecc_point_ref{nullptr};
  }
  return ecc_point_ref{point};
}

cmem_t ecc_point_to_bytes(ecc_point_ref* point) {
  ecc_point_t* point_obj = static_cast<ecc_point_t*>(point->opaque);
  buf_t point_buf = coinbase::ser(*point_obj);
  return point_buf.to_cmem();
}

ecc_point_ref ecc_point_multiply(ecc_point_ref* point, cmem_t scalar) {
  ecc_point_t* point_obj = static_cast<ecc_point_t*>(point->opaque);
  // Use from_bin to convert raw bytes to bn_t
  bn_t scalar_bn = bn_t::from_bin(mem_t(scalar));

  ecc_point_t* result = new ecc_point_t(scalar_bn * (*point_obj));
  return ecc_point_ref{result};
}

ecc_point_ref ecc_point_add(ecc_point_ref* point1, ecc_point_ref* point2) {
  ecc_point_t* p1 = static_cast<ecc_point_t*>(point1->opaque);
  ecc_point_t* p2 = static_cast<ecc_point_t*>(point2->opaque);
  ecc_point_t* result = new ecc_point_t(*p1 + *p2);
  return ecc_point_ref{result};
}

ecc_point_ref ecc_point_subtract(ecc_point_ref* point1, ecc_point_ref* point2) {
  ecc_point_t* p1 = static_cast<ecc_point_t*>(point1->opaque);
  ecc_point_t* p2 = static_cast<ecc_point_t*>(point2->opaque);
  ecc_point_t* result = new ecc_point_t(*p1 - *p2);
  return ecc_point_ref{result};
}

cmem_t ecc_point_get_x(ecc_point_ref* point) {
  ecc_point_t* point_obj = static_cast<ecc_point_t*>(point->opaque);
  buf_t x_buf = point_obj->get_x().to_bin();
  return x_buf.to_cmem();
}

cmem_t ecc_point_get_y(ecc_point_ref* point) {
  ecc_point_t* point_obj = static_cast<ecc_point_t*>(point->opaque);
  buf_t y_buf = point_obj->get_y().to_bin();
  return y_buf.to_cmem();
}

int ecc_point_is_zero(ecc_point_ref* point) {
  ecc_point_t* point_obj = static_cast<ecc_point_t*>(point->opaque);
  // Use the built-in infinity check method
  return point_obj->is_infinity() ? 1 : 0;
}

int ecc_point_equals(ecc_point_ref* point1, ecc_point_ref* point2) {
  ecc_point_t* p1 = static_cast<ecc_point_t*>(point1->opaque);
  ecc_point_t* p2 = static_cast<ecc_point_t*>(point2->opaque);
  return (*p1 == *p2) ? 1 : 0;
}

// ============ Random Scalar Generation ================

cmem_t ecurve_random_scalar(ecurve_ref* curve) {
  ecurve_t* curve_obj = static_cast<ecurve_t*>(curve->opaque);
  bn_t k = curve_obj->get_random_value();
  buf_t k_buf = k.to_bin(curve_obj->order().get_bin_size());
  return k_buf.to_cmem();
}

// ============ Scalar Operations ================

// Adds two scalars represented as byte arrays (big-endian) and returns the
// resulting scalar as bytes. The addition is performed using the bn_t
// implementation from the core library to ensure constant-time behaviour.

cmem_t bn_add(cmem_t a, cmem_t b) {
  bn_t a_bn = bn_t::from_bin(mem_t(a));
  bn_t b_bn = bn_t::from_bin(mem_t(b));
  bn_t c_bn = a_bn + b_bn;
  buf_t c_buf = c_bn.to_bin();
  return c_buf.to_cmem();
}

// Adds two scalars modulo the curve order and returns the result as bytes.
cmem_t ec_mod_add(ecurve_ref* curve, cmem_t a, cmem_t b) {
  ecurve_t* curve_obj = static_cast<ecurve_t*>(curve->opaque);
  mod_t q = curve_obj->order();

  bn_t a_bn = bn_t::from_bin(mem_t(a));
  bn_t b_bn = bn_t::from_bin(mem_t(b));

  bn_t c_bn = (a_bn + b_bn) % q;

  buf_t c_buf = c_bn.to_bin(q.get_bin_size());
  return c_buf.to_cmem();
}

// Creates a bn_t from an int64 value and returns its byte representation.
cmem_t bn_from_int64(int64_t value) {
  bn_t bn;
  bn.set_int64(value);
  buf_t bn_buf = bn.to_bin();
  return bn_buf.to_cmem();
}

// ============ Generator Multiply ================

ecc_point_ref ecurve_mul_generator(ecurve_ref* curve, cmem_t scalar) {
  ecurve_t* curve_obj = static_cast<ecurve_t*>(curve->opaque);
  bn_t k = bn_t::from_bin(mem_t(scalar));
  ecc_point_t* result = new ecc_point_t(curve_obj->mul_to_generator(k));
  return ecc_point_ref{result};
}