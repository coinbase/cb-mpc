// Package mpc provides Go wrappers over cb-mpc's public C API.
//
// The API surface mirrors protocol families in cbmpc/c_api/*:
// ECDSA (2PC + MP), EdDSA, Schnorr, TDH2, PVE, PVE batch, and PVE AC.
//
// Interactive protocols require all parties to execute concurrently while
// exchanging messages over a Transport and passing explicit Job2P/JobMP values
// to protocol entry points.
//
// Security notes:
//   - Many functions return opaque blobs that contain sensitive key material.
//     These are returned as Go byte slices and may remain in memory until garbage
//     collected. Zeroize buffers when you are done with them if you need stricter
//     in-memory handling (see Zeroize / ZeroizeSlices or Sensitive / SensitiveSlices).
//   - Party names in Job2P / JobMP are security-sensitive identifiers. Use
//     stable globally unique values such as UUIDs, public key fingerprints, or
//     other cryptographically bound identities. Do not use placeholders like
//     `party0` or network endpoints such as IP addresses or `host:port`.
//   - Test/demo transports live in `pkg/mpctest`. Production code should
//     provide its own Transport implementation.
//   - Production Transport implementations should provide authenticated +
//     confidential transport and enforce reasonable message size limits to avoid
//     memory-exhaustion attacks from untrusted peers.
//
// The underlying cb-mpc implementation is not thread-safe. Treat protocol calls
// and protocol state as single-threaded unless you have validated a stronger
// synchronization strategy against upstream `cb-mpc` guidance.
//
// The runnable `examples/` tree sometimes executes multiple logical parties in
// one process for local convenience only. Those examples are not a
// thread-safety guarantee or a recommended deployment model; prefer the
// multi-process `demos/` flows for production-like integration patterns.
package mpc
