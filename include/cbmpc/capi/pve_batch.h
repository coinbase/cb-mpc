#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <cbmpc/capi/common.h>
#include <cbmpc/capi/pve.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
// Batch PVE (1P) API (C API)
// ---------------------------------------------------------------------------
//
// This API batches the core PVE algorithm for multiple scalars {x_i} in one ciphertext.
//
// - `xs` / `Qs_compressed` / `out_xs` use `cmems_t` (flattened segments).
// - If `base_pke` is NULL, cbmpc uses its built-in default base PKE.

// Encrypt a batch of scalars {x_i}, producing a single batch ciphertext.
//
// Ownership:
// - On success, `out_ciphertext->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_ciphertext)`.
// - On failure, `*out_ciphertext` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_pve_batch_encrypt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t ek,
                                      cmem_t label, cmems_t xs, cmem_t* out_ciphertext);

cbmpc_error_t cbmpc_pve_batch_verify(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t ek,
                                     cmem_t ciphertext, cmems_t Qs_compressed, cmem_t label);

// Decrypt a batch ciphertext, recovering {x_i}.
//
// Ownership:
// - On success, `out_xs->data` and `out_xs->sizes` are allocated by the library and must be freed with
//   `cbmpc_cmems_free(*out_xs)`.
// - On failure, `*out_xs` is set to `{0, NULL, NULL}`.
//
// Notes:
// - This function intentionally does not verify `ciphertext` before decryption.
//   Invalid ciphertexts may cause decryption to fail, but are designed to not
//   leak secret information.
// - If you need ciphertext validation, call `cbmpc_pve_batch_verify(...)` first.
cbmpc_error_t cbmpc_pve_batch_decrypt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t dk,
                                      cmem_t ek, cmem_t ciphertext, cmem_t label, cmems_t* out_xs);

// Decrypt using an HSM-backed RSA private key (KEM decapsulation callback).
//
// Ownership: same as `cbmpc_pve_batch_decrypt`.
cbmpc_error_t cbmpc_pve_batch_decrypt_rsa_oaep_hsm(cbmpc_curve_id_t curve, cmem_t dk_handle, cmem_t ek,
                                                   cmem_t ciphertext, cmem_t label,
                                                   const cbmpc_pve_rsa_oaep_hsm_decap_t* cb, cmems_t* out_xs);

// Decrypt using an HSM-backed ECIES(P-256) private key (ECDH callback).
//
// Ownership: same as `cbmpc_pve_batch_decrypt`.
cbmpc_error_t cbmpc_pve_batch_decrypt_ecies_p256_hsm(cbmpc_curve_id_t curve, cmem_t dk_handle, cmem_t ek,
                                                     cmem_t ciphertext, cmem_t label,
                                                     const cbmpc_pve_ecies_p256_hsm_ecdh_t* cb, cmems_t* out_xs);

// Extract batch count from a batch ciphertext.
cbmpc_error_t cbmpc_pve_batch_get_count(cmem_t ciphertext, int* out_batch_count);

// Extract {Q_i} from a batch ciphertext (compressed point encodings).
//
// Ownership:
// - On success, `out_Qs_compressed->data` and `out_Qs_compressed->sizes` are
//   allocated by the library and must be freed with `cbmpc_cmems_free(*out_Qs_compressed)`.
// - On failure, `*out_Qs_compressed` is set to `{0, NULL, NULL}`.
cbmpc_error_t cbmpc_pve_batch_get_Qs(cmem_t ciphertext, cmems_t* out_Qs_compressed);

// Extract label from a batch ciphertext.
//
// Ownership:
// - On success, `out_label->data` is allocated by the library and must be freed with
//   `cbmpc_cmem_free(*out_label)`.
// - On failure, `*out_label` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_pve_batch_get_Label(cmem_t ciphertext, cmem_t* out_label);

#ifdef __cplusplus
}
#endif
