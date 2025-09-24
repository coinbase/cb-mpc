package curve

import (
	"fmt"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// Bridging conventions between Go and native (cgo boundary):
//
//   - Curves are passed as integer codes (OpenSSL NIDs). The bindings resolve
//     native curve handles internally via ECurveFind(code) when needed.
//   - Points, scalars and other crypto artefacts are passed as canonical []byte
//     encodings. The bindings reconstruct native objects internally as needed.
//
// Rationale: keep the public Go API free of native pointers/handles and any
// linkname plumbing, while incurring only negligible overhead to reconstruct
// native structures inside the bindings.
//
// The helpers Code(c) and NewFromCode(code) are the only public surface needed
// by higher layers to follow this convention consistently.

// Curve is the public interface that represents an elliptic curve supported by the cb-mpc library.
//
// Concrete curves – secp256k1, P-256 and Ed25519 – implement this interface.
// Users should obtain a curve via the constructor helpers NewSecp256k1, NewP256 or NewEd25519
// rather than dealing with numeric curve codes directly.
//
// All implementations wrap native (C++) resources. Therefore each Curve must be released
// with a call to Free once it is no longer needed to avoid memory leaks.
// Alternatively, the caller can rely on the Go GC finalizer to invoke Free automatically
// (not implemented here to avoid hidden costs).
//
// The methods mirror the functionality that was previously available on the ECurve struct.
// They remain unchanged so existing call-sites require only minimal migration.
type Curve interface {
	// Generator returns the generator point of the curve.
	Generator() *Point
	// Order returns the (big-endian) order of the curve group.
	Order() []byte
	// Free releases the native resources associated with the curve.
	Free()
	// RandomScalar returns a uniformly random non-zero scalar in the interval
	// [1, Order()-1]. The random sampling is delegated to the native C++ layer.
	RandomScalar() (*Scalar, error)
	// MultiplyGenerator multiplies the curve generator by the given scalar
	// and returns the resulting point (k * G).
	MultiplyGenerator(k *Scalar) (*Point, error)
	// RandomKeyPair returns a uniformly random non-zero scalar in the interval
	// [1, Order()-1] and the corresponding point (k * G).
	RandomKeyPair() (*Scalar, *Point, error)
	// Add returns (a + b) mod Order() as a new Scalar.
	Add(a, b *Scalar) (*Scalar, error)
	// String returns a human friendly identifier (implements fmt.Stringer).
	fmt.Stringer
}

// Internal numeric identifiers – matching OpenSSL NIDs – used by the native library.
const (
	secp256k1Code = 714  // OpenSSL NID_secp256k1
	p256Code      = 415  // OpenSSL NID_X9_62_prime256v1
	ed25519Code   = 1087 // OpenSSL NID_ED25519
)

// ========================= common implementation =========================

type baseCurve struct {
	cCurve cgobinding.ECurveRef
}

func newBaseCurve(code int) (*baseCurve, error) {
	cCurve, err := cgobinding.ECurveFind(code)
	if err != nil {
		return nil, err
	}
	return &baseCurve{cCurve: cCurve}, nil
}

func (b *baseCurve) Generator() *Point {
	cPoint := cgobinding.ECurveGenerator(b.cCurve)
	return &Point{cPoint: cPoint}
}

func (b *baseCurve) Order() []byte {
	return cgobinding.ECurveOrderToMem(b.cCurve)
}

func (b *baseCurve) Free() {
	b.cCurve.Free()
}

func (b *baseCurve) RandomScalar() (*Scalar, error) {
	// Delegate sampling to the native library so we stay consistent with the
	// core C++ implementation.
	kBytes := cgobinding.ECurveRandomScalarToMem(b.cCurve)
	if len(kBytes) == 0 {
		return nil, fmt.Errorf("failed to generate random scalar")
	}
	return &Scalar{Bytes: kBytes}, nil
}

func (b *baseCurve) MultiplyGenerator(k *Scalar) (*Point, error) {
	if k == nil {
		return nil, fmt.Errorf("scalar is nil")
	}
	gen := b.Generator()
	defer gen.Free()
	return gen.Multiply(k)
}

func (b *baseCurve) RandomKeyPair() (*Scalar, *Point, error) {
	x, err := b.RandomScalar()
	if err != nil {
		return nil, nil, err
	}
	X, err := b.MultiplyGenerator(x)
	if err != nil {
		return nil, nil, err
	}
	return x, X, nil
}

func (b *baseCurve) Add(a, c *Scalar) (*Scalar, error) {
	if a == nil || c == nil {
		return nil, fmt.Errorf("nil scalar operand")
	}
	res := cgobinding.ScalarAddModOrder(b.cCurve, a.Bytes, c.Bytes)
	if len(res) == 0 {
		return nil, fmt.Errorf("scalar modular addition failed")
	}
	return &Scalar{Bytes: res}, nil
}

func (b *baseCurve) String() string {
	switch cgobinding.ECurveGetCurveCode(b.cCurve) {
	case secp256k1Code:
		return "secp256k1"
	case p256Code:
		return "P-256"
	case ed25519Code:
		return "Ed25519"
	default:
		return fmt.Sprintf("unknown curve (%d)", cgobinding.ECurveGetCurveCode(b.cCurve))
	}
}

// Code returns the internal numeric identifier for the curve (OpenSSL NID).
// This is the standard medium for crossing the Go↔native boundary for curves.
// Bindings will resolve a native handle on-demand using this code.
func Code(c Curve) int { return cgobinding.ECurveGetCurveCode(nativeRef(c)) }

// NewFromCode constructs a Curve instance for the provided internal code.
// It resolves a native curve under the hood and wraps it in the appropriate
// concrete curve type when recognised.
func NewFromCode(code int) (Curve, error) {
	bc, err := newBaseCurve(code)
	if err != nil {
		return nil, err
	}
	// Pick concrete wrapper for friendly String() output
	switch code {
	case secp256k1Code:
		return &secp256k1Curve{baseCurve: bc}, nil
	case p256Code:
		return &p256Curve{baseCurve: bc}, nil
	case ed25519Code:
		return &ed25519Curve{baseCurve: bc}, nil
	default:
		return bc, nil
	}
}

// nativeRef exposes the underlying native curve handle for a given Curve.
//
// It is unexported so it remains invisible to application code. Internal
// packages access it via go:linkname (see api/internal/curveref) to bridge
// between the high-level Go types and the low-level C++ pointers.
func nativeRef(c Curve) cgobinding.ECurveRef {
	switch v := c.(type) {
	case *secp256k1Curve:
		return v.cCurve
	case *p256Curve:
		return v.cCurve
	case *ed25519Curve:
		return v.cCurve
	default:
		panic(fmt.Sprintf("unsupported curve type %T", c))
	}
}

// ========================= concrete curve types =========================

// secp256k1Curve implements Curve for the secp256k1 group used by Bitcoin.
type secp256k1Curve struct{ *baseCurve }

// NewSecp256k1 returns a new instance of the secp256k1 curve.
func NewSecp256k1() (Curve, error) {
	bc, err := newBaseCurve(secp256k1Code)
	if err != nil {
		return nil, err
	}
	return &secp256k1Curve{baseCurve: bc}, nil
}

// p256Curve implements Curve for the NIST P-256 curve.
type p256Curve struct{ *baseCurve }

// NewP256 returns a new instance of the P-256 curve.
func NewP256() (Curve, error) {
	bc, err := newBaseCurve(p256Code)
	if err != nil {
		return nil, err
	}
	return &p256Curve{baseCurve: bc}, nil
}

// ed25519Curve implements Curve for the Ed25519 Edwards curve.
type ed25519Curve struct{ *baseCurve }

// NewEd25519 returns a new instance of the Ed25519 curve.
func NewEd25519() (Curve, error) {
	bc, err := newBaseCurve(ed25519Code)
	if err != nil {
		return nil, err
	}
	return &ed25519Curve{baseCurve: bc}, nil
}

// Compile-time guarantees that each concrete type satisfies the interface.
var _ Curve = (*secp256k1Curve)(nil)
var _ Curve = (*p256Curve)(nil)
var _ Curve = (*ed25519Curve)(nil)
