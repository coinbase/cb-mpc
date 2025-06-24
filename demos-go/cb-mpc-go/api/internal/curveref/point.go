package curveref

import (
	_ "unsafe"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

//go:linkname curveNewPointFromCRef github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve.newPointFromCRef
func curveNewPointFromCRef(ref cgobinding.ECCPointRef) *curve.Point

//go:linkname curvePointToCRef github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve.toCRef
func curvePointToCRef(p *curve.Point) cgobinding.ECCPointRef

// PointFromCRef converts a native point reference into the higher-level
// curve.Point type.
//
// INTERNAL USE ONLY – application code must not import this package.
func PointFromCRef(ref cgobinding.ECCPointRef) *curve.Point {
	return curveNewPointFromCRef(ref)
}

// PointToCRef converts a high-level curve.Point into its native
// cgobinding.ECCPointRef representation.
//
// INTERNAL USE ONLY – application code must not import this package.
func PointToCRef(p *curve.Point) cgobinding.ECCPointRef {
	return curvePointToCRef(p)
}
