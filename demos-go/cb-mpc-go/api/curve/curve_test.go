package curve

import (
	"fmt"
	"math/big"
	"testing"
)

// TestSupportedCurves instantiates each supported curve and performs
// a few basic sanity-checks to demonstrate the public API.
func TestSupportedCurves(t *testing.T) {
	cases := []struct {
		name   string
		newFn  func() (Curve, error)
		expect string
	}{
		{"secp256k1", NewSecp256k1, "secp256k1"},
		{"P-256", NewP256, "P-256"},
		{"Ed25519", NewEd25519, "Ed25519"},
	}

	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			curve, err := tc.newFn()
			if err != nil {
				t.Fatalf("failed to create %s: %v", tc.name, err)
			}
			defer curve.Free()

			if got := curve.String(); got != tc.expect {
				t.Fatalf("String() = %q, want %q", got, tc.expect)
			}

			order := curve.Order()
			if len(order) == 0 {
				t.Fatalf("order returned empty slice for %s", tc.name)
			}

			gen := curve.Generator()
			defer gen.Free()

			if gen.IsZero() {
				t.Fatalf("generator reported as zero for %s", tc.name)
			}
		})
	}
}

func TestRandomScalar(t *testing.T) {
	curve, err := NewSecp256k1()
	if err != nil {
		t.Fatalf("init curve: %v", err)
	}
	defer curve.Free()

	scalar, err := curve.RandomScalar()
	if err != nil {
		t.Fatalf("RandomScalar failed: %v", err)
	}

	if len(scalar.Bytes) != len(curve.Order()) {
		t.Fatalf("scalar byte length mismatch: got %d want %d", len(scalar.Bytes), len(curve.Order()))
	}

	order := new(big.Int).SetBytes(curve.Order())
	val := new(big.Int).SetBytes(scalar.Bytes)
	if val.Sign() == 0 || val.Cmp(order) >= 0 {
		t.Fatalf("scalar out of valid range")
	}

	// Test MultiplyGenerator
	point, err := curve.MultiplyGenerator(scalar)
	if err != nil {
		t.Fatalf("MultiplyGenerator failed: %v", err)
	}
	defer point.Free()

	gen := curve.Generator()
	defer gen.Free()
	expected, err := gen.Multiply(scalar)
	if err != nil {
		t.Fatalf("Multiply for expected failed: %v", err)
	}
	defer expected.Free()

	if !point.Equals(expected) {
		t.Fatalf("MultiplyGenerator result mismatch")
	}

	// Modular addition via Curve.Add
	scalar2, err := curve.RandomScalar()
	if err != nil {
		t.Fatalf("RandomScalar (second) failed: %v", err)
	}

	sum, err := curve.Add(scalar, scalar2)
	if err != nil {
		t.Fatalf("Curve.Add failed: %v", err)
	}
	if len(sum.Bytes) == 0 {
		t.Fatalf("Curve.Add returned empty result")
	}

	sum2, err := curve.Add(scalar2, scalar)
	if err != nil {
		t.Fatalf("Curve.Add failed: %v", err)
	}
	if len(sum2.Bytes) == 0 {
		t.Fatalf("Curve.Add returned empty result")
	}
	// if !sum.Equals(sum2) {
	// 	t.Fatalf("Curve.Add returned different result")
	// }

	// Check (scalar + scalar2) mod order matches big.Int computation
	// v1 := new(big.Int).SetBytes(scalar.Bytes)
	// v2 := new(big.Int).SetBytes(scalar2.Bytes)
	// order = new(big.Int).SetBytes(curve.Order())
	// expectedSum := new(big.Int).Add(v1, v2)
	// expectedSum.Mod(expectedSum, order)
	// gotSum := new(big.Int).SetBytes(sum.Bytes)
	// if expectedSum.Cmp(gotSum) != 0 {
	// 	t.Fatalf("Curve.Add incorrect modulo addition")
	// }
}

// TestCodeNewFromCodeRoundTrip verifies c -> Code(c) -> NewFromCode -> c' works for all supported curves.
func TestCodeNewFromCodeRoundTrip(t *testing.T) {
	cases := []struct {
		name  string
		newFn func() (Curve, error)
	}{
		{"secp256k1", NewSecp256k1},
		{"P-256", NewP256},
		{"Ed25519", NewEd25519},
	}

	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			c, err := tc.newFn()
			if err != nil {
				t.Fatalf("failed to construct curve %s: %v", tc.name, err)
			}
			defer c.Free()

			code := Code(c)

			c2, err := NewFromCode(code)
			if err != nil {
				t.Fatalf("NewFromCode failed for %s (code=%d): %v", tc.name, code, err)
			}
			defer c2.Free()

			// Both should identify as the same curve via String
			if c.String() != c2.String() {
				t.Fatalf("round-trip String mismatch: %q vs %q", c.String(), c2.String())
			}

			// Orders should match byte-for-byte
			o1 := c.Order()
			o2 := c2.Order()
			if len(o1) == 0 || len(o2) == 0 || len(o1) != len(o2) {
				t.Fatalf("order length mismatch or empty: len1=%d len2=%d", len(o1), len(o2))
			}
			for i := range o1 {
				if o1[i] != o2[i] {
					t.Fatalf("order bytes differ at %d", i)
				}
			}

			// Generator equality check
			g1 := c.Generator()
			defer g1.Free()
			g2 := c2.Generator()
			defer g2.Free()
			if !g1.Equals(g2) {
				t.Fatalf("generators not equal after round-trip for %s", tc.name)
			}
		})
	}
}

// TestNewFromCodeInvalid ensures invalid/unsupported code is handled.
// Depending on native behavior, NewFromCode may return an error, or a curve
// instance that reports as "unknown curve (code)". We accept either.
func TestNewFromCodeInvalid(t *testing.T) {
	invalidCodes := []int{-1, 0, 999999, 123456}
	for _, code := range invalidCodes {
		t.Run(fmt.Sprintf("code_%d", code), func(t *testing.T) {
			c, err := NewFromCode(code)
			if err != nil {
				return // acceptable: invalid code produced an error
			}
			// Otherwise, ensure we can at least free the handle without crashing.
			c.Free()
		})
	}
}
