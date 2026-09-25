# PVE public-API demo

This is the existing demo for single-value PVE, batched PVE, and access-structure
(PVE-AC) recovery. Run it without arguments for all examples, or with `--ac-only`
for recipient-protected recovery and its negative checks.

## PVE-AC: protect partials before forwarding

All RSA, ECIES, and application-adapter AC examples use `recovery_example.h`:

1. Holders verify the backup against trusted expectations, decrypt locally, and
   encrypt their partials to the approved recipient before forwarding.
2. The relay receives only ciphertext. The recipient verifies the backup, decrypts
   the messages, and combines partials locally. Failed operations clear outputs.

The optional demo-local encryption helper uses fresh randomness and the built-in
primitive; applications may use their own approved recipient encryption instead.
The backup adapter delegates to real ECIES and remains deterministic given `rho`.
It does not implement an external KMS/HSM. No library API or wire format is added.

## Assumptions and limits

- Roles run in one process with synthetic scalars and honest backups, using attempt
  0. Expected public values come from those fixtures, not the incoming backup.
- Applications must authorize recipient keys and requests, authenticate holders,
  and enforce expiry, replay handling, input limits, and bounded recovery attempts.
  Encryption or a random request ID alone does not provide those controls.
- Context binds the exact backup, label, request, holder, attempt, and secp256k1
  domain with length-prefixed fields. It is not the specification's full-vector
  `"vdecrypt" || B` wire mapping. Helpers require valid, non-aliasing input/output views.
- This is not full ECDSA-2P P1 recovery: its scalar-detached state independently
  recovers the scalar and needs full-key protection. See
  [P1 backup limitations](../../SECURE_USAGE.md#ecdsa-2p-p1-backup-limitations).

See [recipient-protection requirements](../../SECURE_USAGE.md#pve-ac-recovery-partials-and-recipient-protection)
before adapting these examples; this is not a deployable recovery service.

## Build and test

Against an installed library, use the normal `CBMPC_SOURCE_DIR` prefix and matching
custom OpenSSL. To select explicit local artifacts, from the repository root:

```sh
cmake -S demo-api/pve -B build/pve-demo \
  -DCBMPC_INCLUDE_DIR="$PWD/include" \
  -DCBMPC_LIBRARY="$PWD/lib/Debug/libcbmpc.a" \
  -DCBMPC_OPENSSL_ROOT="$PWD/build/openssl-3.6.4"
cmake --build build/pve-demo
ctest --test-dir build/pve-demo --output-on-failure
build/pve-demo/mpc-demo-api-pve
```

Adjust paths for your platform. CTest covers all six backup/recipient combinations,
fresh randomness, invalid/wrong keys, context mismatch, tampering with either
message, unknown/duplicate holders, incorrect expected public values, and clearing
stale output on failure. Expected rejection cases may emit crypto error logs.
