package curve

import "github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"

// newFromNativeRef constructs a Curve implementation from a native ECurveRef.
// It is intentionally unexported and should only be accessed via the
// api/internal/curveref bridge using go:linkname. The caller is responsible
// for eventually releasing the Curve via Curve.Free().
//
// The function inspects the numeric NID of the curve to decide which concrete
// implementation wrapper to instantiate. While we still rely on the numeric
// code internally, this is now confined to the curve package and hidden from
// higher layers so callers no longer need to depend on curvemap.CurveForCode.
func newFromNativeRef(ref cgobinding.ECurveRef) Curve {
	switch cgobinding.ECurveGetCurveCode(ref) {
	case secp256k1Code:
		return &secp256k1Curve{baseCurve: &baseCurve{cCurve: ref}}
	case p256Code:
		return &p256Curve{baseCurve: &baseCurve{cCurve: ref}}
	case ed25519Code:
		return &ed25519Curve{baseCurve: &baseCurve{cCurve: ref}}
	default:
		// Fallback to the generic baseCurve wrapper if the code is unknown.
		return &baseCurve{cCurve: ref}
	}
}
