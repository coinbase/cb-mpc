// Package mpc exposes high-level, ergonomic APIs for the multi-party
// computation (MPC) protocols implemented in the CB-MPC library.
//
// Instead of dealing with round messages, state-machines and network plumbing
// you interact with simple, synchronous request/response helpers.  Under the
// hood each helper drives the native C++ engine and uses a `transport.Messenger`
// implementation to move data between parties.
//
// Highlights
//
//   - Uniform Go API for 2–N-party ECDSA/EdDSA key generation, key refresh,
//     signing and more.
//   - Pluggable transport layer – run the same code against an in-process
//     `mocknet` during unit tests and switch to a production‐grade mTLS
//     transport with no changes.
//   - First-class test-utilities that spin up realistic local networks in a
//     single process.
//
// Quick example (random agreement between two parties):
//
//	import "github.com/cb-mpc/api/mpc"
//
//	// Agree on 128 bits of randomness between two parties.
//	out, err := mpc.AgreeRandomWithMockNet(2 /* parties */, 128 /* bits */)
//	if err != nil {
//	    log.Fatalf("mpc: %v", err)
//	}
//	fmt.Printf("Shared random value: %x\n", out[0].Random)
//
// For production you would create a `transport.Messenger` (for example via the
// `mtls` sub-package) and then build a `Job*` value:
//
//	messenger, _ := mtls.NewMTLSMessenger(cfg)
//	job, _ := mpc.NewJob2P(messenger, selfIndex, []string{"alice", "bob"})
//	resp, err := mpc.AgreeRandom(job, &mpc.AgreeRandomRequest{BitLen: 256})
//
// Every exported helper returns rich, declarative request and response structs
// making it straightforward to marshal results into JSON or protobuf.
package mpc
