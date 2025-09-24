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
//	import "github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/mpc"
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
//
// Package mpc exposes a thin, Go-idiomatic wrapper around the core C++
// Publicly-Verifiable-Encryption (PVE) primitives found in cb-mpc.  The wrapper
// is intentionally small – it forwards heavy cryptographic operations to the
// native library while letting Go take care of configuration, concurrency and
// pluggable encryption back-ends.
//
// Architecture
//
//	Go (mpc.PVE) ──▶ Cgo shim (internal/cgobinding) ──▶ C++ core (src/cbmpc)
//	  ▲                     ▲                                │
//	  │                     │                                │
//	  │   per-backend ctx   │   stub functions               │
//	  │   registry          │                                │
//	  │                     │                                ▼
//	 Backend impls      ←── thread-local ctx  ←────────  ffi_pke_t
//
// A caller supplies a custom encapsulation implementation that satisfies the
// cgobinding.KEM interface (aliased as mpc.KEM).  Each implementation is
// registered once and receives an opaque *context pointer*.  That pointer is
// shipped down to the C++ code on every call so the correct backend can be
// picked without any global state.
//
// The wrapper therefore supports multiple, independent encryption schemes –­
// including non-ECIES KEM hybrids – running side-by-side inside the same Go
// process.
//
// # Concurrency Model
//
// The context pointer is stored in a thread-local variable inside the shim
// (`thread_local void* g_ctx`).  Every user-visible helper first calls the
// unexported activateCtx() method which sets the variable, thereby guaranteeing
// that concurrent goroutines operating on different PVE handles never clash.
//
// Adding a New Backend
//
//  1. Implement the KEM methods.
//  2. Pass an instance via `NewPVE(Config{KEM: yourImpl})`.
//  3. Use the returned *PVE handle* for Encrypt / Verify / Decrypt.
//
// The backend is registered automatically and its required `rho` size is cached
// once at start-up for zero-alloc fast paths inside the native code.
//
// See the unit tests in pve_test.go for some example backends.
package mpc
