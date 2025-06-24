package curve

import (
	"fmt"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// Point represents a point on an elliptic curve
type Point struct {
	cPoint cgobinding.ECCPointRef
}

// NewPointFromBytes creates a new point from serialized bytes
func NewPointFromBytes(pointBytes []byte) (*Point, error) {
	if len(pointBytes) == 0 {
		return nil, fmt.Errorf("empty point bytes")
	}
	cPoint, err := cgobinding.ECCPointFromBytes(pointBytes)
	if err != nil {
		return nil, err
	}
	return &Point{cPoint: cPoint}, nil
}

// Free releases the memory associated with the point
func (p *Point) Free() {
	p.cPoint.Free()
}

// toCRef returns the underlying C++ point reference.
//
// INTERNAL USE ONLY – the function is intentionally unexported so that
// application code cannot depend on the native representation. A
// go:linkname directive (see api/internal/curveref) provides controlled
// access for other cb-mpc-go sub-packages that need to cross the cgo
// boundary.
func toCRef(p *Point) cgobinding.ECCPointRef {
	return p.cPoint
}

// Multiply multiplies the point by a scalar
func (p *Point) Multiply(scalar *Scalar) (*Point, error) {
	if scalar.Bytes == nil {
		return nil, fmt.Errorf("nil scalar")
	}
	cPoint, err := cgobinding.ECCPointMultiply(p.cPoint, scalar.Bytes)
	if err != nil {
		return nil, err
	}
	return &Point{cPoint: cPoint}, nil
}

// Add adds two points together
func (p *Point) Add(other *Point) *Point {
	cPoint := cgobinding.ECCPointAdd(p.cPoint, other.cPoint)
	return &Point{cPoint: cPoint}
}

// Subtract subtracts one point from another
func (p *Point) Subtract(other *Point) *Point {
	cPoint := cgobinding.ECCPointSubtract(p.cPoint, other.cPoint)
	return &Point{cPoint: cPoint}
}

// GetX returns the x coordinate of the point as bytes
func (p *Point) GetX() []byte {
	return cgobinding.ECCPointGetX(p.cPoint)
}

// GetY returns the y coordinate of the point as bytes
func (p *Point) GetY() []byte {
	return cgobinding.ECCPointGetY(p.cPoint)
}

// IsZero checks if the point is the point at infinity (zero point)
func (p *Point) IsZero() bool {
	return cgobinding.ECCPointIsZero(p.cPoint)
}

// Equals checks if two points are equal
func (p *Point) Equals(other *Point) bool {
	return cgobinding.ECCPointEquals(p.cPoint, other.cPoint)
}

// String returns a string representation of the point
func (p *Point) String() string {
	if p.IsZero() {
		return "Point(∞)"
	}
	x := p.GetX()
	y := p.GetY()
	return fmt.Sprintf("Point(x: %x, y: %x)", x, y)
}

// Bytes returns the canonical serialization of the point as produced by the
// underlying native library (SEC1 uncompressed format).
func (p *Point) Bytes() []byte {
	if p == nil {
		return nil
	}
	return cgobinding.ECCPointToBytes(p.cPoint)
}

// newPointFromCRef wraps an existing native reference into a *Point.
//
// It is unexported so that it disappears from the public API surface. The
// function is still linked to the internal helper package via go:linkname so
// that other cb-mpc-go sub-packages can construct Point values without
// direct access to the unexported cPoint field.
//
// **DO NOT** use this from application code; it is considered internal API.
func newPointFromCRef(ref cgobinding.ECCPointRef) *Point {
	return &Point{cPoint: ref}
}
