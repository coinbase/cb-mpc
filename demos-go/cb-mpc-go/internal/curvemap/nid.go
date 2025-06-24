package curvemap

import (
	"fmt"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
)

// Numeric OpenSSL NIDs for curves supported by cb-mpc-go.
const (
	Secp256k1 = 714  // NID_secp256k1
	P256      = 415  // NID_X9_62_prime256v1
	Ed25519   = 1087 // NID_ED25519
)

// CurveForCode converts an OpenSSL NID into a curve.Curve instance.
// Only for internal consumption.
func CurveForCode(code int) (curve.Curve, error) {
	switch code {
	case Secp256k1:
		return curve.NewSecp256k1()
	case P256:
		return curve.NewP256()
	case Ed25519:
		return curve.NewEd25519()
	default:
		return nil, fmt.Errorf("unsupported curve code %d", code)
	}
}
