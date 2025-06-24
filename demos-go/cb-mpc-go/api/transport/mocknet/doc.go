// Package mocknet implements the `transport.Messenger` interface entirely in
// memory and is intended ONLY for testing or local development.
//
// A mock network is invaluable when writing unit- or integration-tests because
// it:
//   - removes all external dependencies (no sockets, no certificates),
//   - runs deterministically inside a single OS process, and
//   - is orders of magnitude faster than loop-back TCP.
//
// Under the hood each party is backed by a pair of goroutines and a channel per
// direction which faithfully replicate the semantics of a real network while
// still sharing memory.
//
// For production deployments use the `mtls` transport or build your own
// Messenger that satisfies the `transport.Messenger` interface.
package mocknet
