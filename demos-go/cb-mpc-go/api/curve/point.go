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
