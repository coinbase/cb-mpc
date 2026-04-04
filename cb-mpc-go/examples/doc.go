// Package examples provides helper code for runnable protocol examples.
//
// Example binaries live under:
//   - examples/ecdsa2pc
//   - examples/schnorr2pc
//   - examples/eddsa2pc
//   - examples/eddsa_mp_backup
//   - examples/pve_showcase
//   - examples/tdh2
//   - examples/pve_batch
//
// Some examples run multiple logical parties in one process for local
// convenience. Use them as zero-setup API demonstrations, not as thread-safety
// guidance or a recommended deployment model.
//
// For production-like multi-process flows, see `demos/`.
//
// For additional compact multi-party usage patterns that stay close to the
// public API surface, see:
//   - pkg/mpc/api_test.go
//   - pkg/mpc/ac_flows_test.go
//   - pkg/mpc/protocol_gap_test.go
package examples
