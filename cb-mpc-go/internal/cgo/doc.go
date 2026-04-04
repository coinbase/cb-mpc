// Package cgo contains all CGO bindings to cb-mpc's public C API.
//
// # Design Principles
//
// 1. Isolation: ALL import "C" usage lives in this package.
//
// 2. CAPI-only: Bind only symbols from include/cbmpc/c_api/*.
//
// 3. Memory discipline: Follow cmem/cmems ownership and free contracts exactly.
//
// 4. Error handling: Convert cbmpc_error_t values to Go errors immediately.
//
//  5. Transport callback bridge: Interactive protocols pass through cbmpc_transport_t
//     using exported Go callbacks.
//
// # Threading
//
// The cb-mpc library is NOT thread-safe. Callers must ensure proper
// synchronization.
package cgo
