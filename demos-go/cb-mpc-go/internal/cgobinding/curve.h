#pragma once

#include <stdint.h>

#include <cbmpc/core/cmem.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============ Curve Type Definitions =============

typedef struct ecc_point_ref {
  void* opaque;
} ecc_point_ref;

typedef struct ecurve_ref {
  void* opaque;
} ecurve_ref;

// ============ Curve Memory Management =============

void free_ecc_point(ecc_point_ref ref);
void free_ecurve(ecurve_ref ref);

// ============ Curve Operations =============

// Curve functions
ecurve_ref new_ecurve(int curve_code);
ecc_point_ref ecurve_generator(ecurve_ref* curve);
cmem_t ecurve_order(ecurve_ref* curve);
int ecurve_get_curve_code(ecurve_ref* curve);

// Point functions
ecc_point_ref ecc_point_from_bytes(cmem_t point_bytes);
cmem_t ecc_point_to_bytes(ecc_point_ref* point);
ecc_point_ref ecc_point_multiply(ecc_point_ref* point, cmem_t scalar);
ecc_point_ref ecc_point_add(ecc_point_ref* point1, ecc_point_ref* point2);
ecc_point_ref ecc_point_subtract(ecc_point_ref* point1, ecc_point_ref* point2);
cmem_t ecc_point_get_x(ecc_point_ref* point);
cmem_t ecc_point_get_y(ecc_point_ref* point);
int ecc_point_is_zero(ecc_point_ref* point);
int ecc_point_equals(ecc_point_ref* point1, ecc_point_ref* point2);
cmem_t ecurve_random_scalar(ecurve_ref* curve);

// ECDSA verification (DER-encoded signature). Returns 0 on success, non-zero on failure.
int ecc_verify_der(int curve_code, cmem_t pub_oct, cmem_t hash, cmem_t der_sig);

// Scalar operations
cmem_t bn_add(cmem_t a, cmem_t b);
cmem_t ec_mod_add(ecurve_ref* curve, cmem_t a, cmem_t b);
cmem_t bn_from_int64(int64_t value);
ecc_point_ref ecurve_mul_generator(ecurve_ref* curve, cmem_t scalar);

#ifdef __cplusplus
}  // extern "C"
#endif