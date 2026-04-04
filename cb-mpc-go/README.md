# cb-mpc-go

Go bindings for Coinbase MPC, implemented strictly on top of the public C API (`include/cbmpc/c_api/*`).

## Status

`cb-mpc-go` is beta software. Its primary goal is to make the public `cb-mpc`
C API easier to use from Go while the wrapper surface continues to mature.

This directory now lives inside the main `cb-mpc` repository. It keeps its own
`go.mod`, `Makefile`, examples, and demos, but it builds against the parent
`cb-mpc` checkout instead of a nested `cb-mpc` submodule.

Project policies:

- [License](../LICENSE.md)
- [Security Policy](../SECURITY.md)
- [Bug Bounty Scope](../BUG_BOUNTY.md)
- [Security Testing Notes](pkg/mpc/SECURITY_TESTING.md)
- [Contributing](../CONTRIBUTING.md)
- [Code of Conduct](../CODE_OF_CONDUCT.md)

## Scope

- Uses only the **public install** of `cb-mpc` (`include/` + `lib/`)
- Does **not** depend on `include-internal/` or any internal C++ headers
- Wraps the public C API surface in `pkg/mpc`

## Supported Protocols

- ECDSA 2PC: DKG, refresh, sign, key/public-share helpers
- ECDSA MP: additive + access-structure variants
- EdDSA 2PC/MP: additive + access-structure variants
- Schnorr 2PC/MP: additive + access-structure variants, x-only pubkey extract
- TDH2: DKG, encrypt/verify, partial decrypt, combine (additive + AC)
- PVE / PVE Batch / PVE AC: encrypt/verify/decrypt + helper constructors for
  built-in RSA/ECIES keys and external RSA modulus / ECIES public-key import +
  optional callback bridges (base PKE / KEM / HSM)

## Requirements

- Go 1.25+
- CMake + C++17 toolchain
- A checkout of the parent `cb-mpc` repository with vendor submodules initialized
- Git LFS if you want the PDF specifications/docs shipped in the parent repository
- OpenSSL/toolchain setup required by the parent `cb-mpc` checkout (see `../README.md`)

## Imported Baseline

This in-tree module was imported from the standalone `cb-mpc-go`
`v0.1.0-beta` codebase and was originally paired with `cb-mpc v0.2.1`.
The code here now builds against the parent checkout instead of that former
submodule layout.

Initial compatibility baseline:

| `cb-mpc-go` | `cb-mpc` | Notes |
| --- | --- | --- |
| `v0.1.0-beta` | `v0.2.1` | Uses only public install artifacts and follows the OpenSSL 3.6.1 flow required by `cb-mpc v0.2.1` |

## Build and Test

```bash
# Run these commands from cb-mpc/cb-mpc-go.

# Initialize the parent cb-mpc vendor submodules
make deps

# Build parent cb-mpc + public install, then Go code
make build

# Run tests
make test

# Run focused ASAN/UBSAN coverage for the CGO boundary hardening tests
make sanitize-go

# Run the repo-pinned golangci-lint
make lint

# Run the repo-pinned govulncheck
make vulncheck

# Run protocol examples
make examples

# Show production-like demo workflows
make demos-help
```

The runnable interactive examples use `mpctest.NewMockNetwork` for local demos
and tests. That transport is not authenticated or confidential. Production
deployments should provide their own `Transport`.

For interactive protocols, every `Job2P` / `JobMP` must use stable globally
unique party names. Good examples include UUIDs, public key fingerprints, or
other identifiers that are unique across all protocol participants and all
deployments that may interact. Do not use placeholders like `party0`, integer
indices, hostnames, or network endpoints as party names. Reusing non-unique
names can break the security assumptions of access-structure and interactive
protocol flows by letting messages or authorization state be associated with the
wrong logical party.

This Go module keeps zero-setup examples and production-like demos separate:

- `examples/` are self-contained local runs meant for `make examples`
- `demos/` are networked reference workflows that may require setup such as PEM cert generation
- some `examples/` run multiple logical parties concurrently in one process for convenience; treat that as demo scaffolding, not as a thread-safety guarantee or a recommended deployment model
- the reference demo mTLS transport in `demos/common/mtls.go` is intentionally local/demo-oriented, not a production-ready networking layer
- the browser-driven web demo is intentionally localhost/demo-oriented, exposes an unauthenticated HTTP control plane, and persists sensitive MPC blobs in plaintext demo state files

## Dependency Setup

This directory keeps the `github.com/coinbase/cb-mpc-go` module path to
minimize import churn inside the combined repository layout.

`cb-mpc-go` is a CGO wrapper, not a pure-Go package. Consumers must make the
`cb-mpc` public install artifacts available at build time:

- public headers under `cbmpc/c_api/*`
- `libcbmpc`
- OpenSSL libraries compatible with the parent `cb-mpc` checkout

When working from `cb-mpc/cb-mpc-go`, `bash scripts/go_with_cpp.sh ...` sets
the required CGO include/library paths automatically.

If you are building outside this in-tree wrapper script, set the equivalent
environment yourself, for example:

```bash
export CBMPC_PREFIX="/path/to/cb-mpc/build/install/public"
export CGO_CFLAGS="-I${CBMPC_PREFIX}/include"
export CGO_CXXFLAGS="-I${CBMPC_PREFIX}/include"
export CGO_LDFLAGS="-L${CBMPC_PREFIX}/lib -L/path/to/openssl/lib"
```

The helper script also supports overriding the install locations with:

- `CBMPC_PREFIX`
- `CBMPC_OPENSSL_ROOT`

### Linux / OpenSSL Notes

The default `CBMPC_OPENSSL_ROOT` in `scripts/go_with_cpp.sh` points at a
Homebrew-style macOS location. On Linux or other layouts, set
`CBMPC_OPENSSL_ROOT` explicitly before invoking the script, or mirror the
OpenSSL setup described in `../README.md` and `scripts/openssl/`.

All Go commands should run through `scripts/go_with_cpp.sh` (the Makefile already does this) so `CGO_CFLAGS` and `CGO_LDFLAGS` point at:

- `../build/install/public/include`
- `../build/install/public/lib`

## Quick Example (ECDSA-2PC)

```go
import (
	"context"
	"log"
	"sync"
	"time"

	"github.com/coinbase/cb-mpc-go/pkg/mpc"
	"github.com/coinbase/cb-mpc-go/pkg/mpctest"
)

ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
defer cancel()

// mpctest.NewMockNetwork is for local demos/tests only.
// This quick example runs both logical parties in one process for convenience.
// Use demos/ for process-isolated, production-like topologies.
transports := mpctest.NewMockNetwork(2)
defer transports[0].Close()
defer transports[1].Close()

jobs := []mpc.Job2P{
	{Self: mpc.TwoPartyP1, P1Name: "urn:uuid:2f4b567e-7d58-52c5-9c74-0d0c8b6a3e10", P2Name: "urn:uuid:7a1f5e4a-b8b4-5c67-a4cb-5777f284d64e", Transport: transports[0]},
	{Self: mpc.TwoPartyP2, P1Name: "urn:uuid:2f4b567e-7d58-52c5-9c74-0d0c8b6a3e10", P2Name: "urn:uuid:7a1f5e4a-b8b4-5c67-a4cb-5777f284d64e", Transport: transports[1]},
}

var (
	keyBlobs [2][]byte
	wg       sync.WaitGroup
	errCh    = make(chan error, 2)
)

for i := 0; i < 2; i++ {
	wg.Add(1)
	go func(idx int) {
		defer wg.Done()
		blob, err := mpc.ECDSA2PDKG(ctx, jobs[idx], mpc.CurveSecp256k1)
		if err != nil {
			errCh <- err
			return
		}
		keyBlobs[idx] = blob
	}(i)
}
wg.Wait()
close(errCh)

for err := range errCh {
	if err != nil {
		log.Fatalf("ecdsa 2pc dkg failed: %v", err)
	}
}
```

See full runnable examples in:

- `examples/ecdsa2pc`
- `examples/schnorr2pc`
- `examples/eddsa2pc`
- `examples/eddsa_mp_backup` for multi-key EdDSA-MP share backup and restore via PVE-AC
- `examples/pve_showcase` for custom base-PKE callbacks and simulated ECIES HSM-backed PVE flows
- `examples/tdh2`
- `examples/pve_batch`

Looking for concise multi-party usage patterns?

- `pkg/mpc/api_test.go` covers additive ECDSA-MP, EdDSA-MP, Schnorr-MP, TDH2,
  and PVE flows end-to-end through the public API.
- `pkg/mpc/ac_flows_test.go` covers access-structure ECDSA-MP, EdDSA-MP,
  Schnorr-MP, and TDH2 flows through the public API.
- `pkg/mpc/protocol_gap_test.go` exercises additional refresh and helper paths
  that are useful when mapping Go calls back to the public C API families.
- `examples/eddsa_mp_backup` is the smallest runnable MP example in `examples/`
  and also demonstrates PVE-AC backup/restore.

Production-like demos live under:

- `demos/ecdsa2pc_mtls` for a two-process ECDSA-2PC flow over a reference mTLS transport
- `demos/ecdsa_mp_ac_mtls` for a three-process ECDSA-MP access-structure DKG/sign/refresh/sign workflow over mTLS
- `demos/ecdsa_mp_ac_web` for a three-party browser-driven ECDSA-MP access-structure application demo backed by isolated Go party servers
- `demos/eddsa_mp_backup_mtls` for a three-process EdDSA-MP backup and restore showcase over mTLS
- `demos/tdh2_mtls` for a three-process TDH2 encrypt/partial-decrypt/combine workflow over mTLS

See `demos/README.md` for certificate generation and multi-terminal launch
commands.

## Architecture

```text
pkg/mpc (public Go API)
  -> internal/cgo (only package importing "C")
    -> cb-mpc public C API (cbmpc/c_api/*) + libcbmpc
```

## Notes

- The underlying `cb-mpc` implementation is not thread-safe. Treat protocol calls and protocol state as single-threaded unless you have validated a stronger synchronization strategy against upstream `cb-mpc` guidance.
- Some zero-setup examples intentionally run multiple logical parties in one process for local convenience. Those examples are not a thread-safety guarantee and should not be treated as a recommended deployment model.
- Interactive protocols require all parties to run concurrently and exchange messages through a `Transport` while passing explicit `Job2P` / `JobMP` values.
- `Transport.ReceiveAll` must return messages in the same order as the requested sender list.
- `Job2P` and `JobMP` party names should be stable globally unique identifiers such as UUIDs or public key fingerprints. The library rejects obvious placeholders like `party0` and rejects bare IP addresses or `host:port` endpoints as party names.
- In 2PC sign flows (`ECDSA2PSign`, `EdDSA2PSign`, `Schnorr2PSign`), the signature is returned only on P1; P2 may observe an empty signature on success.
- `mpctest.NewMockNetwork` and the example transports are for demos and tests; production code should use a hardened authenticated/confidential transport with appropriate peer-identity binding and deadlines/timeouts.
- Many returned blobs (key blobs, private shares, decryption keys) contain sensitive material. They are returned as Go `[]byte` and may remain in memory until garbage collected; zeroize buffers when you are done with them if you need stricter in-memory handling (see `mpc.Zeroize` / `mpc.ZeroizeSlices` or `mpc.Sensitive` / `mpc.SensitiveSlices`).
- Production `Transport` implementations should provide authenticated + confidential transport and enforce reasonable message size limits to avoid memory-exhaustion attacks from untrusted peers.
