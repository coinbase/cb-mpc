package curveref

import (
	_ "unsafe"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

//go:linkname curveNativeRef github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve.nativeRef
func curveNativeRef(c curve.Curve) cgobinding.ECurveRef

//go:linkname curveNewFromNativeRef github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve.newFromNativeRef
func curveNewFromNativeRef(ref cgobinding.ECurveRef) curve.Curve

// Ref returns the underlying native curve reference for the provided Curve.
//
// INTERNAL USE ONLY – application code must not import this package. It exists
// so that other sub-packages within cb-mpc-go that need to bridge to the C++
// layer can do so without exposing the raw handle in the public API.
func Ref(c curve.Curve) cgobinding.ECurveRef {
	return curveNativeRef(c)
}

// CurveFromCRef converts a native curve reference back into the high-level
// curve.Curve abstraction.
//
// INTERNAL USE ONLY – application code must not import this package.
func CurveFromCRef(ref cgobinding.ECurveRef) curve.Curve {
	return curveNewFromNativeRef(ref)
}
