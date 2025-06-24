// Package curve provides idiomatic Go bindings for the elliptic-curve primitives
// implemented in the C++ `cbmpc` library.
//
// The package wraps two native C++ handle types:
//  1. `ecurve_t` – an elliptic-curve definition (currently only secp256k1)
//  2. `ecc_point_t` – a point that lives on a particular curve
//
// Because the underlying objects are allocated on the C++ heap, every value that
// is created from this package owns native resources.  ALWAYS call the `Free` or
// `Close` method when you are done with a value, or use `defer` immediately after
// creation.  Failing to do so will leak memory.
//
// # Quick start
//
// The snippet below shows the most common workflow – creating a curve, deriving
// the generator point, performing a scalar multiplication and reading back the
// affine coordinates:
//
//	cur, err := curve.NewSecp256k1()
//	if err != nil {
//	    log.Fatalf("initialising curve: %v", err)
//	}
//	defer cur.Free()
//
//	G := cur.Generator()
//	defer gen.Free()
//
//	// Multiply the generator by a 32-byte scalar.
//	scalar := curve.RandomScalar(cur)
//	p := G.Mul(scalar)
//
// Features
//
//   - Creation of named curves (secp256k1 for now)
//   - Arithmetic on immutable `Point` values: Add, Sub, Neg, Mul
//   - Constant-time, allocation-free serialization (compressed & uncompressed)
//   - Helper utilities for random scalar / point generation (in tests)
//
// All heavy arithmetic is executed in constant time inside C++, guaranteeing that
// the Go bindings themselves never become a side-channel.
package curve
