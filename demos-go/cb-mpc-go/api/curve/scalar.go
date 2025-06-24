package curve

import (
	"bytes"
	"fmt"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// Scalar represents a field element (mod the curve order).
//
// Bytes is a fixed-length, big-endian encoding with the same byte length as
// the curve order.
//
// For now the type only supports random generation, but it lays the
// foundation for future arithmetic helpers.
type Scalar struct {
	// Bytes holds the big-endian representation of the scalar. The length of
	// the slice matches the order of the underlying curve (but the scalar
	// itself no longer embeds that information).
	Bytes []byte
}

// NewScalarFromInt64 creates a new Scalar from an int64 value.
// The int64 value is converted to a big number using the native C++ layer's
// set_int64 function to ensure consistent representation.
func NewScalarFromInt64(value int64) *Scalar {
	bytes := cgobinding.ScalarFromInt64(value)
	return &Scalar{Bytes: bytes}
}

// Add returns s + other as a new Scalar. The addition is performed by the
// native C++ layer (bn_t addition) to leverage its constant-time
// implementation and to stay consistent with the rest of the library.
func (s *Scalar) Add(other *Scalar) (*Scalar, error) {
	if s == nil || other == nil {
		return nil, fmt.Errorf("nil scalar operand")
	}
	res := cgobinding.ScalarAdd(s.Bytes, other.Bytes)
	if len(res) == 0 {
		return nil, fmt.Errorf("scalar addition failed")
	}
	return &Scalar{Bytes: res}, nil
}

// Equal returns true if s and other represent the same scalar value.
// Returns false if either scalar is nil.
func (s *Scalar) Equal(other *Scalar) bool {
	if s == nil || other == nil {
		return false
	}
	return bytes.Equal(s.Bytes, other.Bytes)
}

func (s *Scalar) String() string {
	if s == nil {
		return "<nil scalar>"
	}
	return fmt.Sprintf("Scalar(%x)", s.Bytes)
}
