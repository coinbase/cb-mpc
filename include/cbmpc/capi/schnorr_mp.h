#pragma once

#include <stdint.h>

#include <cbmpc/capi/access_structure.h>
#include <cbmpc/capi/common.h>
#include <cbmpc/capi/job.h>

#ifdef __cplusplus
extern "C" {
#endif

// Interactive multi-party key generation for Schnorr-MP (BIP340).
//
// Ownership:
// - On success, `out_key_blob->data` and `out_sid->data` are allocated by the
//   library and must be freed with `cbmpc_cmem_free(...)`.
// - On failure, `*out_key_blob` and `*out_sid` are set to `{NULL, 0}`.
//
// Supported curves: `CBMPC_CURVE_SECP256K1`.
cbmpc_error_t cbmpc_schnorr_mp_dkg_additive(const cbmpc_mp_job_t* job, cbmpc_curve_id_t curve, cmem_t* out_key_blob,
                                            cmem_t* out_sid);

// Interactive multi-party key generation for Schnorr-MP (BIP340) with a general
// access structure.
//
// Notes:
// - This is an n-party protocol: **all** parties in `job->party_names` must be
//   online and participate.
// - Only the provided `quorum_party_names` actively contribute to the generated
//   key shares.
// - The output key blob represents an access-structure key share and
//   is not directly usable with `cbmpc_schnorr_mp_sign_additive`. Use `cbmpc_schnorr_mp_sign_ac`
//   to sign with an online quorum (it derives additive shares internally).
// - `sid_in` is the in/out session id used by the protocol; callers may pass
//   `{NULL, 0}` and let the protocol derive one.
//
// Ownership:
// - On success, `out_ac_key_blob->data` and `out_sid->data` are allocated by
//   the library and must be freed with `cbmpc_cmem_free(...)`.
// - On failure, `*out_ac_key_blob` and `*out_sid` are set to `{NULL, 0}`.
//
// Supported curves: `CBMPC_CURVE_SECP256K1`.
cbmpc_error_t cbmpc_schnorr_mp_dkg_ac(const cbmpc_mp_job_t* job, cbmpc_curve_id_t curve, cmem_t sid_in,
                                      const cbmpc_access_structure_t* access_structure,
                                      const char* const* quorum_party_names, int quorum_party_names_count,
                                      cmem_t* out_ac_key_blob, cmem_t* out_sid);

// Interactive multi-party key refresh (same public key).
//
// `sid_in` is the in/out session id used by the refresh protocol; callers may
// pass `{NULL, 0}` and let the protocol derive one. If `sid_out` is non-NULL, it
// will receive the session id used.
//
// Ownership:
// - On success, `out_new_key_blob->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_new_key_blob)`.
// - If `sid_out` is non-NULL, then on success `sid_out->data` is allocated by
//   the library and must be freed with `cbmpc_cmem_free(*sid_out)`.
// - On failure, `*out_new_key_blob` (and `*sid_out` when provided) are set to
//   `{NULL, 0}`.
cbmpc_error_t cbmpc_schnorr_mp_refresh_additive(const cbmpc_mp_job_t* job, cmem_t sid_in, cmem_t key_blob,
                                                cmem_t* sid_out, cmem_t* out_new_key_blob);

// Interactive multi-party access-structure key refresh (same public key).
//
// Notes:
// - See `cbmpc_schnorr_mp_dkg_ac` for protocol participation semantics.
// - The output key blob represents an access-structure key share and
//   is not directly usable with `cbmpc_schnorr_mp_sign_additive`. Use `cbmpc_schnorr_mp_sign_ac`
//   to sign with an online quorum (it derives additive shares internally).
// - `sid_in` is the in/out session id used by the refresh protocol; callers may
//   pass `{NULL, 0}` and let the protocol derive one. If `sid_out` is non-NULL, it
//   will receive the session id used.
//
// Ownership: same as `cbmpc_schnorr_mp_refresh_additive`.
cbmpc_error_t cbmpc_schnorr_mp_refresh_ac(const cbmpc_mp_job_t* job, cmem_t sid_in, cmem_t ac_key_blob,
                                          const cbmpc_access_structure_t* access_structure,
                                          const char* const* quorum_party_names, int quorum_party_names_count,
                                          cmem_t* sid_out, cmem_t* out_new_ac_key_blob);

// Sign a message with Schnorr-MP (BIP340). Outputs a 64-byte signature on `sig_receiver`.
//
// Ownership:
// - On success, `sig_out->data` is allocated by the library and must be freed
//   with `cbmpc_cmem_free(*sig_out)`.
// - On failure, `*sig_out` is set to `{NULL, 0}`.
//
// Note: the underlying protocol returns the signature only on `sig_receiver`. On
// other parties, `*sig_out` may be `{NULL, 0}` on success.
cbmpc_error_t cbmpc_schnorr_mp_sign_additive(const cbmpc_mp_job_t* job, cmem_t key_blob, cmem_t msg,
                                             int32_t sig_receiver, cmem_t* sig_out);

// Sign a message with Schnorr-MP (BIP340) using an access-structure key share (from
// `cbmpc_schnorr_mp_dkg_ac` / `cbmpc_schnorr_mp_refresh_ac`).
//
// This API first derives an additive-share signing key for the **online** signing
// parties in `job->party_names` and then runs the normal `cbmpc_schnorr_mp_sign_additive`
// protocol among those parties.
//
// Notes:
// - Unlike `cbmpc_schnorr_mp_dkg_ac` / `cbmpc_schnorr_mp_refresh_ac`, `cbmpc_schnorr_mp_sign_ac`
//   only requires the parties in `job->party_names` to be online and participate.
// - Output semantics match `cbmpc_schnorr_mp_sign_additive`: the signature is returned only
//   on `sig_receiver`. On other parties, `*sig_out` may be `{NULL, 0}` on success.
//
// Ownership: same as `cbmpc_schnorr_mp_sign_additive`.
cbmpc_error_t cbmpc_schnorr_mp_sign_ac(const cbmpc_mp_job_t* job, cmem_t ac_key_blob,
                                       const cbmpc_access_structure_t* access_structure, cmem_t msg,
                                       int32_t sig_receiver, cmem_t* sig_out);

// Get the Schnorr/BIP340 public key (SEC1 compressed point encoding) from a key blob.
//
// Ownership:
// - On success, `out_pub_key->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_pub_key)`.
// - On failure, `*out_pub_key` is set to `{NULL, 0}`.
//
// Output size is 33 bytes for secp256k1: 0x02/0x03 || x (32 bytes).
cbmpc_error_t cbmpc_schnorr_mp_get_public_key_compressed(cmem_t key_blob, cmem_t* out_pub_key);

// Extract the Schnorr/BIP340 x-only public key (32 bytes) from a key blob.
//
// Ownership: same as `cbmpc_schnorr_mp_get_public_key_compressed`.
cbmpc_error_t cbmpc_schnorr_mp_extract_public_key_xonly(cmem_t key_blob, cmem_t* out_pub_key);

// ---------------------------------------------------------------------------
// Key blob manipulation (private scalar backup / restore)
// ---------------------------------------------------------------------------

// Get this party's share public point (Qi) from a key blob, returning SEC1
// compressed point encoding.
//
// Ownership: same as `cbmpc_schnorr_mp_get_public_key_compressed`.
cbmpc_error_t cbmpc_schnorr_mp_get_public_share_compressed(cmem_t key_blob, cmem_t* out_public_share);

// Detach a key blob into a public blob + fixed-length private scalar.
//
// Ownership:
// - On success, `out_public_key_blob->data` and `out_private_scalar_fixed->data` are
//   allocated by the library and must be freed with `cbmpc_cmem_free(...)`.
// - On failure, outputs are set to `{NULL, 0}`.
cbmpc_error_t cbmpc_schnorr_mp_detach_private_scalar(cmem_t key_blob, cmem_t* out_public_key_blob,
                                                     cmem_t* out_private_scalar_fixed);

// Attach a fixed-length private scalar into a public key blob, validating it
// against the expected public share point.
//
// Ownership:
// - On success, `out_key_blob->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_key_blob)`.
// - On failure, `*out_key_blob` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_schnorr_mp_attach_private_scalar(cmem_t public_key_blob, cmem_t private_scalar_fixed,
                                                     cmem_t public_share_compressed, cmem_t* out_key_blob);

#ifdef __cplusplus
}
#endif
