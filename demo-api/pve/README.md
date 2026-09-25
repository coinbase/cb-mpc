# PVE public-API demo

Demonstrates single-value PVE, batched PVE, and access-structure (PVE-AC) recovery.
Run without arguments for all examples, or with `--ac-only` for PVE-AC recovery
and tests that invalid inputs are rejected.

## PVE-AC: protect partial decryptions before forwarding

The RSA, ECIES, and custom-backend AC examples use `recovery_example.h`:

1. Each holder verifies the backup using the expected access structure, public keys,
   public values, and label, then computes its partial decryption and encrypts it
   to the recipient.
2. The relay receives only ciphertext. The recipient verifies the backup, decrypts
   the messages, and combines the partial decryptions. Failed recovery leaves no output.

Backup keys and recipient keys serve different purposes: holders use backup keys
to compute their partial decryptions; the recipient uses a separate key to open
the forwarded messages. Each AC example runs with both RSA and ECIES recipient keys.
The custom backup adapter delegates to built-in ECIES, not an external KMS or HSM.

## Assumptions and limits

- All roles run in one process using test scalars and valid backups. Recovery uses
  attempt 0. Expected public values are computed from the input scalars, not read
  from the backup.
- Applications must authorize recipient keys and recovery requests, authenticate
  holders, reject expired or replayed requests, and limit input sizes.
  Encryption or a random request ID alone does not provide these controls.
- The encryption context identifies the backup, label, request, holder, attempt, and
  curve. This demo's per-attempt encoding differs from the specification's
  `"vdecrypt" || B` encoding.
- The demo helpers require valid input buffers that do not overlap output buffers.
- PVE scalar recovery alone does not restore a complete ECDSA-2P P1 key. See
  [P1 backup limitations](../../SECURE_USAGE.md#ecdsa-2p-p1-backup-limitations).

See [recipient-protection requirements](../../SECURE_USAGE.md#pve-ac-recovery-partial-decryptions-and-recipient-protection)
for application responsibilities.

## Build and test

For an installed library, pass `-DCBMPC_SOURCE_DIR=/path/to/install` to CMake
instead of the explicit include and library paths below.
For a local build, first build cb-mpc, then run these commands from the repository root:

```sh
cmake -S demo-api/pve -B build/pve-demo \
  -DCBMPC_INCLUDE_DIR="$PWD/include" \
  -DCBMPC_LIBRARY="$PWD/lib/Debug/libcbmpc.a" \
  -DCBMPC_OPENSSL_ROOT="$PWD/build/openssl-3.6.4"
cmake --build build/pve-demo
ctest --test-dir build/pve-demo --output-on-failure
build/pve-demo/mpc-demo-api-pve
```

Adjust paths for your build. CTest covers all six backup/recipient combinations,
fresh encryption randomness, invalid or wrong keys, mismatched context, tampered
messages, unknown or duplicate holders, incorrect expected public values, and
clearing output on failure. Expected rejection cases may emit crypto error logs.
