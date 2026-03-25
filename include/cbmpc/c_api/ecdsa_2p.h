#pragma once

#include <stdint.h>

#include <cbmpc/c_api/common.h>
#include <cbmpc/c_api/job.h>

#ifdef __cplusplus
extern "C" {
#endif

// Interactive key generation.
//
// Ownership:
// - On success, `out_key_blob->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_key_blob)`.
// - On failure, `*out_key_blob` is set to `{NULL, 0}`.
//
// Supported curves: `CBMPC_CURVE_P256`, `CBMPC_CURVE_SECP256K1`.
cbmpc_error_t cbmpc_ecdsa_2p_dkg(const cbmpc_2pc_job_t* job, cbmpc_curve_id_t curve, cmem_t* out_key_blob);

// Interactive key refresh.
//
// Ownership:
// - On success, `out_new_key_blob->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_new_key_blob)`.
// - On failure, `*out_new_key_blob` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_ecdsa_2p_refresh(const cbmpc_2pc_job_t* job, cmem_t key_blob, cmem_t* out_new_key_blob);

// Sign a message hash. Outputs DER signature and the session id used.
//
// Ownership:
// - On success, `sig_der_out->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*sig_der_out)`.
// - If `sid_out` is non-NULL, then on success `sid_out->data` is allocated by
//   the library and must be freed with `cbmpc_cmem_free(*sid_out)`.
// - On failure, `*sig_der_out` (and `*sid_out` when provided) are set to
//   `{NULL, 0}`.
//
// Note: the underlying protocol returns the DER signature only on P1. If
// `key_blob` belongs to P2, `*sig_der_out` may be `{NULL, 0}` on success.
cbmpc_error_t cbmpc_ecdsa_2p_sign(const cbmpc_2pc_job_t* job, cmem_t key_blob, cmem_t msg_hash, cmem_t sid_in,
                                  cmem_t* sid_out, cmem_t* sig_der_out);

// Compute compressed public key for a key blob.
//
// Ownership:
// - On success, `out_pub_key->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_pub_key)`.
// - On failure, `*out_pub_key` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_ecdsa_2p_get_public_key_compressed(cmem_t key_blob, cmem_t* out_pub_key);

// ---------------------------------------------------------------------------
// Key blob manipulation (private scalar backup / restore)
// ---------------------------------------------------------------------------

// Get this party's share public point (Qi) from a key blob, returning SEC1
// compressed point encoding.
//
// Ownership: same as `cbmpc_ecdsa_2p_get_public_key_compressed`.
cbmpc_error_t cbmpc_ecdsa_2p_get_public_share_compressed(cmem_t key_blob, cmem_t* out_public_share);

// Detach a key blob into a scalar-removed blob + private scalar.
//
// Notes:
// - Unlike ECDSA-MP, the scalar encoding is NOT fixed-length: after refresh,
//   ECDSA-2PC keeps the share as a Paillier-compatible integer representative and
//   it may grow.
//
// Ownership:
// - On success, `out_public_key_blob->data` and `out_private_scalar->data` are
//   allocated by the library and must be freed with `cbmpc_cmem_free(...)`.
// - On failure, outputs are set to `{NULL, 0}`.
cbmpc_error_t cbmpc_ecdsa_2p_detach_private_scalar(cmem_t key_blob, cmem_t* out_public_key_blob,
                                                   cmem_t* out_private_scalar);

// Attach a variable-length private scalar into a scalar-removed key blob,
// validating it against the expected public share point.
//
// Ownership:
// - On success, `out_key_blob->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_key_blob)`.
// - On failure, `*out_key_blob` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_ecdsa_2p_attach_private_scalar(cmem_t public_key_blob, cmem_t private_scalar,
                                                   cmem_t public_share_compressed, cmem_t* out_key_blob);

#ifdef __cplusplus
}
#endif
