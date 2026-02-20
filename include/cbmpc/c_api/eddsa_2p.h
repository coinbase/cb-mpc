#pragma once

#include <stdint.h>

#include <cbmpc/c_api/common.h>
#include <cbmpc/c_api/job.h>

#ifdef __cplusplus
extern "C" {
#endif

// Interactive key generation for EdDSA-2PC (Ed25519).
//
// Ownership:
// - On success, `out_key_blob->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_key_blob)`.
// - On failure, `*out_key_blob` is set to `{NULL, 0}`.
//
// Supported curves: `CBMPC_CURVE_ED25519`.
cbmpc_error_t cbmpc_eddsa_2p_dkg(const cbmpc_2pc_job_t* job, cbmpc_curve_id_t curve, cmem_t* out_key_blob);

// Interactive key refresh (same public key).
//
// Ownership:
// - On success, `out_new_key_blob->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_new_key_blob)`.
// - On failure, `*out_new_key_blob` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_eddsa_2p_refresh(const cbmpc_2pc_job_t* job, cmem_t key_blob, cmem_t* out_new_key_blob);

// Sign a message with EdDSA-2PC (Ed25519). Outputs a 64-byte signature: R || s.
//
// Ownership:
// - On success, `sig_out->data` is allocated by the library and must be freed
//   with `cbmpc_cmem_free(*sig_out)`.
// - On failure, `*sig_out` is set to `{NULL, 0}`.
//
// Note: the underlying protocol returns the signature only on P1. If `key_blob`
// belongs to P2, `*sig_out` may be `{NULL, 0}` on success.
cbmpc_error_t cbmpc_eddsa_2p_sign(const cbmpc_2pc_job_t* job, cmem_t key_blob, cmem_t msg, cmem_t* sig_out);

// Get the Ed25519 public key from a key blob.
//
// Ownership:
// - On success, `out_pub_key->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_pub_key)`.
// - On failure, `*out_pub_key` is set to `{NULL, 0}`.
//
// Output is the standard Ed25519 32-byte compressed public key encoding.
//
// Note: Ed25519 public keys are always encoded in this compressed format; the
// `_compressed` suffix is provided for naming consistency with ECDSA APIs.
cbmpc_error_t cbmpc_eddsa_2p_get_public_key_compressed(cmem_t key_blob, cmem_t* out_pub_key);

// ---------------------------------------------------------------------------
// Key blob manipulation (private scalar backup / restore)
// ---------------------------------------------------------------------------

// Get this party's share public point (Qi) from a key blob.
//
// Output is the standard Ed25519 32-byte compressed point encoding.
//
// Ownership: same as `cbmpc_eddsa_2p_get_public_key_compressed`.
cbmpc_error_t cbmpc_eddsa_2p_get_public_share_compressed(cmem_t key_blob, cmem_t* out_public_share);

// Detach a key blob into a public blob + fixed-length private scalar.
//
// Ownership:
// - On success, `out_public_key_blob->data` and `out_private_scalar_fixed->data` are
//   allocated by the library and must be freed with `cbmpc_cmem_free(...)`.
// - On failure, outputs are set to `{NULL, 0}`.
cbmpc_error_t cbmpc_eddsa_2p_detach_private_scalar(cmem_t key_blob, cmem_t* out_public_key_blob,
                                                   cmem_t* out_private_scalar_fixed);

// Attach a fixed-length private scalar into a public key blob, validating it
// against the expected public share point.
//
// Ownership:
// - On success, `out_key_blob->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_key_blob)`.
// - On failure, `*out_key_blob` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_eddsa_2p_attach_private_scalar(cmem_t public_key_blob, cmem_t private_scalar_fixed,
                                                   cmem_t public_share_compressed, cmem_t* out_key_blob);

#ifdef __cplusplus
}
#endif
