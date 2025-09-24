package cgobinding

import (
	"fmt"
)

/*
#include <stdint.h>
#include "curve.h"
*/
import "C"

// =========== Curve and Point Types =====================

// Exported aliases so that other packages can reference these types.
// They are simple type aliases, so no extra conversion cost.
type ECurveRef C.ecurve_ref
type ECCPointRef C.ecc_point_ref

func (c *ECurveRef) Free() {
	C.free_ecurve(C.ecurve_ref(*c))
}

func (p *ECCPointRef) Free() {
	C.free_ecc_point(C.ecc_point_ref(*p))
}

// =========== Curve Operations =====================

// ECurveFind finds a curve by curve code
func ECurveFind(curveCode int) (ECurveRef, error) {
	cCurve := C.new_ecurve(C.int(curveCode))
	if cCurve.opaque == nil {
		return ECurveRef{}, fmt.Errorf("invalid curve code: %d", curveCode)
	}
	return ECurveRef(cCurve), nil
}

// ECurveGenerator returns the generator point of the curve
func ECurveGenerator(curve ECurveRef) ECCPointRef {
	cPoint := C.ecurve_generator((*C.ecurve_ref)(&curve))
	return ECCPointRef(cPoint)
}

// ECurveOrderToMem returns the order of the curve as bytes
func ECurveOrderToMem(curve ECurveRef) []byte {
	cMem := C.ecurve_order((*C.ecurve_ref)(&curve))
	return CMEMGet(cMem)
}

// ECurveGetCurveCode returns the curve code
func ECurveGetCurveCode(curve ECurveRef) int {
	code := C.ecurve_get_curve_code((*C.ecurve_ref)(&curve))
	return int(code)
}

// ECurveRandomScalarToMem returns a random scalar modulo the curve order
func ECurveRandomScalarToMem(curve ECurveRef) []byte {
	cMem := C.ecurve_random_scalar((*C.ecurve_ref)(&curve))
	return CMEMGet(cMem)
}

// ================= Scalar Operations ====================

// ScalarAdd returns the byte representation of a + b where the operands are
// interpreted as big-endian scalars (bn_t in the C++ layer).
// The computation is delegated to the native C++ implementation to leverage
// its constant-time big number arithmetic.
func ScalarAdd(a, b []byte) []byte {
	cMem := C.bn_add(cmem(a), cmem(b))
	return CMEMGet(cMem)
}

// ScalarAddModOrder returns (a+b) mod order(curve).
func ScalarAddModOrder(curve ECurveRef, a, b []byte) []byte {
	cMem := C.ec_mod_add((*C.ecurve_ref)(&curve), cmem(a), cmem(b))
	return CMEMGet(cMem)
}

// ScalarFromInt64 creates a scalar from an int64 value and returns its byte representation.
func ScalarFromInt64(value int64) []byte {
	cMem := C.bn_from_int64((C.int64_t)(value))
	return CMEMGet(cMem)
}

// ECurveMulGenerator multiplies the curve generator by a scalar and returns a
// new point reference.
func ECurveMulGenerator(curve ECurveRef, scalar []byte) ECCPointRef {
	cPoint := C.ecurve_mul_generator((*C.ecurve_ref)(&curve), cmem(scalar))
	return ECCPointRef(cPoint)
}

// =========== Point Operations =====================

// ECCPointFromBytes creates a point from bytes
func ECCPointFromBytes(pointBytes []byte) (ECCPointRef, error) {
	cPoint := C.ecc_point_from_bytes(cmem(pointBytes))
	if cPoint.opaque == nil {
		return ECCPointRef{}, fmt.Errorf("failed to create point from bytes")
	}
	return ECCPointRef(cPoint), nil
}

// ECCPointMultiply multiplies a point by a scalar
func ECCPointMultiply(point ECCPointRef, scalar []byte) (ECCPointRef, error) {
	cPoint := C.ecc_point_multiply((*C.ecc_point_ref)(&point), cmem(scalar))
	if cPoint.opaque == nil {
		return ECCPointRef{}, fmt.Errorf("failed to multiply point")
	}
	return ECCPointRef(cPoint), nil
}

// ECCPointAdd adds two points
func ECCPointAdd(point1, point2 ECCPointRef) ECCPointRef {
	cPoint := C.ecc_point_add((*C.ecc_point_ref)(&point1), (*C.ecc_point_ref)(&point2))
	return ECCPointRef(cPoint)
}

// ECCPointSubtract subtracts two points
func ECCPointSubtract(point1, point2 ECCPointRef) ECCPointRef {
	cPoint := C.ecc_point_subtract((*C.ecc_point_ref)(&point1), (*C.ecc_point_ref)(&point2))
	return ECCPointRef(cPoint)
}

// ECCPointGetX returns the X coordinate of the point
func ECCPointGetX(point ECCPointRef) []byte {
	cMem := C.ecc_point_get_x((*C.ecc_point_ref)(&point))
	return CMEMGet(cMem)
}

// ECCPointGetY returns the Y coordinate of the point
func ECCPointGetY(point ECCPointRef) []byte {
	cMem := C.ecc_point_get_y((*C.ecc_point_ref)(&point))
	return CMEMGet(cMem)
}

// ECCPointIsZero checks if the point is zero
func ECCPointIsZero(point ECCPointRef) bool {
	return C.ecc_point_is_zero((*C.ecc_point_ref)(&point)) != 0
}

// ECCPointEquals checks if two points are equal
func ECCPointEquals(point1, point2 ECCPointRef) bool {
	return C.ecc_point_equals((*C.ecc_point_ref)(&point1), (*C.ecc_point_ref)(&point2)) != 0
}

// ECCPointToBytes serializes a point to the library's canonical byte format.
func ECCPointToBytes(point ECCPointRef) []byte {
	cMem := C.ecc_point_to_bytes((*C.ecc_point_ref)(&point))
	return CMEMGet(cMem)
}

// ECCVerifyDER verifies a DER-encoded ECDSA signature.
// curveCode: OpenSSL NID for the curve (e.g., 714 for secp256k1)
// pubOct: SEC1 uncompressed public key bytes
// hash: 32-byte digest
// derSig: DER-encoded ECDSA signature
func ECCVerifyDER(curveCode int, pubOct []byte, hash []byte, derSig []byte) error {
	rv := C.ecc_verify_der(C.int(curveCode), cmem(pubOct), cmem(hash), cmem(derSig))
	if rv != 0 {
		return fmt.Errorf("ecdsa verify failed (%d)", int(rv))
	}
	return nil
}
