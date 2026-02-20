#pragma once

#include <cbmpc/c_api/access_structure.h>
#include <cbmpc/c_api/common.h>
#include <cbmpc/c_api/job.h>

// All the functions have two versions: additive and ac. Additive means that the
// sharing is additive, while ac means that the sharing is according to a given access structure.
#ifdef __cplusplus
extern "C" {
#endif

// Interactive multi-party key generation for TDH2 (additive shares).
//
// Ownership:
// - On success, outputs are allocated by the library and must be freed with:
//   - `cbmpc_cmem_free(*out_public_key)`
//   - `cbmpc_cmems_free(*out_public_shares)`
//   - `cbmpc_cmem_free(*out_private_share)`
//   - `cbmpc_cmem_free(*out_sid)`
// - On failure, output parameters are set to empty values (`{NULL, 0}` / `{0, NULL, NULL}`).
//
// Supported curves: `CBMPC_CURVE_P256`, `CBMPC_CURVE_SECP256K1`.
cbmpc_error_t cbmpc_tdh2_dkg_additive(const cbmpc_mp_job_t* job, cbmpc_curve_id_t curve, cmem_t* out_public_key,
                                      cmems_t* out_public_shares, cmem_t* out_private_share, cmem_t* out_sid);

// Interactive multi-party key generation for TDH2 with a general access structure.
//
// Notes:
// - This is an n-party protocol: **all** parties in `job->party_names` must be
//   online and participate.
// - Only the provided `quorum_party_names` actively contribute to the generated
//   key shares.
// - `sid_in` is the in/out session id used by the protocol; callers may pass `{NULL, 0}` and let the protocol derive
// one.
//
// Ownership: same as `cbmpc_tdh2_dkg_additive`.
cbmpc_error_t cbmpc_tdh2_dkg_ac(const cbmpc_mp_job_t* job, cbmpc_curve_id_t curve, cmem_t sid_in,
                                const cbmpc_access_structure_t* access_structure, const char* const* quorum_party_names,
                                int quorum_party_names_count, cmem_t* out_public_key, cmems_t* out_public_shares,
                                cmem_t* out_private_share, cmem_t* out_sid);

// Encrypt a plaintext under a TDH2 public key.
//
// Ownership:
// - On success, `out_ciphertext->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_ciphertext)`.
// - On failure, `*out_ciphertext` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_tdh2_encrypt(cmem_t public_key, cmem_t plaintext, cmem_t label, cmem_t* out_ciphertext);

cbmpc_error_t cbmpc_tdh2_verify(cmem_t public_key, cmem_t ciphertext, cmem_t label);

// Compute a partial decryption share.
//
// Ownership:
// - On success, `out_partial_decryption->data` is allocated by the library and
//   must be freed with `cbmpc_cmem_free(*out_partial_decryption)`.
// - On failure, `*out_partial_decryption` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_tdh2_partial_decrypt(cmem_t private_share, cmem_t ciphertext, cmem_t label,
                                         cmem_t* out_partial_decryption);

// Combine additive shares / partial decryptions to recover the plaintext.
//
// Ownership:
// - On success, `out_plaintext->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_plaintext)`.
// - On failure, `*out_plaintext` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_tdh2_combine_additive(cmem_t public_key, cmems_t public_shares, cmem_t label,
                                          cmems_t partial_decryptions, cmem_t ciphertext, cmem_t* out_plaintext);

// Combine access-structure shares / partial decryptions to recover the plaintext.
//
// - `party_names` and `public_shares` define the mapping name -> Qi for *all*
//   parties in the access structure (order-aligned).
// - `partial_decryption_party_names` and `partial_decryptions` provide the quorum subset used for decryption.
//
// Requirements:
// - `party_names_count == public_shares.count`
// - `partial_decryption_party_names_count == partial_decryptions.count`
// - The leaf set of `access_structure` must match `party_names` exactly.
//
// Ownership: same as `cbmpc_tdh2_combine_additive`.
cbmpc_error_t cbmpc_tdh2_combine_ac(const cbmpc_access_structure_t* access_structure, cmem_t public_key,
                                    const char* const* party_names, int party_names_count, cmems_t public_shares,
                                    cmem_t label, const char* const* partial_decryption_party_names,
                                    int partial_decryption_party_names_count, cmems_t partial_decryptions,
                                    cmem_t ciphertext, cmem_t* out_plaintext);

#ifdef __cplusplus
}
#endif
