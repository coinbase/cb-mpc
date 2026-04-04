# ECDSA MP AC Web Demo

This demo shows that `cb-mpc-go` can back a browser-driven application rather
than only command-line flows.

This directory is intentionally its own Go module. Its `go.mod` requires
`github.com/coinbase/cb-mpc-go` and uses a local `replace` to `../..` so the
checked-out `cb-mpc-go/` tree behaves like a consuming application importing
the library. If you copy this demo into another repository, remove or update
that `replace` and pin the `cb-mpc-go` version you want to use.

To keep that consumer shape honest, this module also carries its own demo
transport, address helpers, and ECDSA-MP-AC workflow glue locally instead of
importing `demos/common` from the repository root. The only `cb-mpc-go` package
it imports is `pkg/mpc`.

It runs one isolated Go web server per MPC party. Each server:

- owns its own local on-disk workflow state
- owns its own TLS credentials
- participates in MPC over the demo mTLS transport
- exposes a browser UI for creating workflows and running DKG, refresh, and
  repeated sign steps with any threshold-sized signing quorum
- shows the joint public key, local share material, and receiver-side DER
  signature output in copyable form so the protocol is easier to inspect
- includes both compressed and uncompressed public key encodings for easier
  copy/paste into third-party secp256k1 verification tools

The MPC party names used in `JobMP` are fixed UUID-form identifiers. Transport
addresses are only for wiring the demo network and are not used as party names.
By default, receiver slot `0` within the selected sign quorum receives and
verifies the DER signature.

Warnings:

- the browser control plane is intentionally unauthenticated
- bind it only to trusted localhost or similarly isolated development hosts
- the per-party state directory stores plaintext sensitive MPC blobs, including key material and SIDs
- the `data-dir` layout is demo storage, not a secure vault or a production persistence design
- this demo is meant to prove usability of `pkg/mpc`, not to model a hardened deployment architecture

See [demos/README.md](../README.md) for manual launch steps and the
`make demo-ecdsa-mp-ac-web` convenience target. For a module-local launcher,
run `bash demos/ecdsa_mp_ac_web/run_demo.sh` from `cb-mpc-go/` or
`bash cb-mpc-go/demos/ecdsa_mp_ac_web/run_demo.sh` from the top-level
`cb-mpc/` repository root. You can also run `bash run_demo.sh` from this
directory. The manual commands run this module with
`go -C demos/ecdsa_mp_ac_web run .`.
