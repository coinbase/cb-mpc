# Security Testing Coverage

This document describes the current security-relevant automated coverage for the
`cb-mpc-go` wrapper itself.

It is intentionally scoped to the Go API, the CGO boundary, and test/demo
helpers in this Go module. It does not claim deep cryptographic validation of
the parent `cb-mpc` implementation.

## Active Test Suites

### `pkg/mpc/api_test.go`

Integration-style happy-path coverage for the shipped wrapper APIs:

The current package test suite directly exercises the exported `pkg/mpc`
surface for this pinned release, including protocol flows, helper constructors,
memory/sensitivity helpers, and callback bridges.

- ECDSA 2PC and MP additive flows
- ECDSA MP access-structure DKG / sign / refresh flows
- EdDSA 2PC and MP additive flows
- EdDSA MP access-structure DKG / sign / refresh flows
- Schnorr 2PC and MP additive flows
- Schnorr MP access-structure DKG / sign / refresh flows
- TDH2 additive and access-structure DKG / encrypt / verify / partial decrypt / combine flows
- PVE single-recipient, batch, and access-structure flows
- Advanced PVE helper paths:
  - built-in ECIES-P256 software key generation
  - RSA modulus and ECIES public-key helper constructors for HSM-backed keys
  - RSA / ECIES HSM callback decrypt paths for single, batch, and AC flows
  - custom KEM callback encrypt / verify / decrypt flow
- Key-blob helper round-trips:
  - ECDSA / EdDSA / Schnorr detach-private-scalar + attach-private-scalar
    round-trips for 2PC and MP additive key blobs

Security-relevant assertions in these tests include:

- all parties derive the same public key material for a shared protocol run
- designated signature receivers are the only parties that receive MP outputs
- refreshed AC key blobs preserve the original public key
- Ed25519 signatures verify against the derived public key
- ECDSA signatures are well-formed DER with positive `r`/`s` values
- Schnorr signatures have the expected x-only public key and output width
- TDH2 and PVE encrypt/verify/decrypt paths round-trip the expected plaintext
- detached/reattached key blobs preserve public keys, public shares, and
  signing usability

### `pkg/mpc/malicious_session_test.go`

Active negative coverage for shipped test helpers and selected failure modes:

- `mpctest.MaliciousTransport` behavior isolation and error messages
- dropped-message behavior for interactive protocols with explicit timeouts
- garbage-message injection causing at least one protocol participant to fail

These tests intentionally focus on wrapper-visible behavior rather than making
claims about Byzantine fault tolerance or upstream protocol proofs.

### `internal/cgo/binding_hardening_test.go`

Boundary and hardening coverage for the CGO layer:

- `cmem_t` / `cmems_t` round-trip conversions
- rejection of embedded NUL bytes in C strings
- access-structure validation (cycles, unreachable nodes, invalid thresholds)
- panic recovery in exported transport and PVE callback bridges
- oversized callback-output rejection before `C.int` truncation
- fast-fail validation for MP signature receivers and AC quorum inputs
- TDH2 AC parallel-slice contract validation

### `make sanitize-go`

Focused sanitizer coverage for the Go wrapper and CGO boundary:

- rebuilds `cb-mpc` with ASAN/UBSAN-style compiler and linker flags
- installs a sanitized public prefix for the wrapper to link against
- reruns `./internal/cgo` against that sanitized build by default

You can pass additional package arguments to `scripts/sanitize_go.sh` if you
want to expand the scope beyond the default hardening package.

This job is meant to catch wrapper-visible memory misuse and undefined behavior
that can be missed by normal Go tests alone.

## How To Run

Run the full wrapper test suite with the in-tree CGO environment:

```bash
bash scripts/go_with_cpp.sh go test ./...
```

Run focused sanitizer coverage for the wrapper + CGO boundary:

```bash
make sanitize-go
```

Run only the Go wrapper integration tests:

```bash
bash scripts/go_with_cpp.sh go test ./pkg/mpc
```

Run only the CGO hardening tests:

```bash
bash scripts/go_with_cpp.sh go test ./internal/cgo
```

## Important Limits

The current automated coverage does not claim:

- deep cryptographic review of upstream `cb-mpc`
- proof of constant-time behavior or side-channel resistance
- leak detection in every environment; the sanitizer job currently favors stable
  ASAN/UBSAN coverage for the Go+cgo boundary over always-on leak checking
- authenticated or confidential transport from `mpctest.NewMockNetwork`
- general Byzantine fault tolerance, collusion resistance, or replay resistance
- production readiness of example/demo code or dev certificate scripts

## Production Assumptions

The wrapper assumes that production callers provide their own `Transport`
implementation with:

- authenticated transport
- confidential transport
- stable globally unique party names in every interactive `Job2P` / `JobMP`
- reasonable message-size limits
- timeout/cancellation handling for stalled or malicious peers

`mpctest.NewMockNetwork`, `mpctest.MaliciousTransport`, and the example/demo
transports exist to test or demonstrate integration behavior. They are not
drop-in production transports.
