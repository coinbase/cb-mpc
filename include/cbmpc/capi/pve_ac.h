#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <cbmpc/capi/access_structure.h>
#include <cbmpc/capi/common.h>
#include <cbmpc/capi/pve.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
// PVE-AC (access-structure / quorum decryption) API (C API)
// ---------------------------------------------------------------------------
//
// This API encrypts a *batch* of scalars {x_i} under a leaf-keyed access structure.
//
// Decryption is stepwise:
// - Each party calls `cbmpc_pve_ac_party_decrypt_row` to produce a leaf share for a specific row.
// - The application collects enough shares and calls `cbmpc_pve_ac_aggregate_to_restore_row` to recover {x_i}.
//
// Notes:
// - Leaf keys are passed as a mapping (parallel arrays) from leaf name to key blob.
// - If `base_pke` is NULL, cbmpc uses its built-in default base PKE.

cbmpc_error_t cbmpc_pve_ac_encrypt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve,
                                   const cbmpc_access_structure_t* ac, const char* const* leaf_names,
                                   const cmem_t* leaf_eks, int leaf_count, cmem_t label, cmems_t xs,
                                   cmem_t* out_ciphertext);

cbmpc_error_t cbmpc_pve_ac_verify(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve,
                                  const cbmpc_access_structure_t* ac, const char* const* leaf_names,
                                  const cmem_t* leaf_eks, int leaf_count, cmem_t ciphertext, cmems_t Qs_compressed,
                                  cmem_t label);

// Step 1: decrypt a single leaf share for `row_index`.
//
// Ownership: same as `cbmpc_pve_encrypt`.
cbmpc_error_t cbmpc_pve_ac_party_decrypt_row(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve,
                                             const cbmpc_access_structure_t* ac, cmem_t ciphertext, int row_index,
                                             const char* leaf_name, cmem_t dk, cmem_t label, cmem_t* out_share);

// Step 1 (HSM): decrypt a single leaf share for `row_index` using an HSM-backed
// RSA-OAEP private key (KEM decapsulation callback).
//
// - `dk_handle` is an opaque handle understood by the callback.
// - `ek` is the leaf's built-in base PKE public key blob (used to validate key type).
//
// Ownership: same as `cbmpc_pve_ac_party_decrypt_row`.
cbmpc_error_t cbmpc_pve_ac_party_decrypt_row_rsa_oaep_hsm(cbmpc_curve_id_t curve, const cbmpc_access_structure_t* ac,
                                                          cmem_t ciphertext, int row_index, const char* leaf_name,
                                                          cmem_t dk_handle, cmem_t ek, cmem_t label,
                                                          const cbmpc_pve_rsa_oaep_hsm_decap_t* cb, cmem_t* out_share);

// Step 1 (HSM): decrypt a single leaf share for `row_index` using an HSM-backed
// ECIES(P-256) private key (ECDH callback only).
//
// - `dk_handle` is an opaque handle understood by the callback.
// - `ek` is the leaf's built-in base PKE public key blob (used to validate key type
//   and derive the KEM context).
//
// Ownership: same as `cbmpc_pve_ac_party_decrypt_row`.
cbmpc_error_t cbmpc_pve_ac_party_decrypt_row_ecies_p256_hsm(cbmpc_curve_id_t curve, const cbmpc_access_structure_t* ac,
                                                            cmem_t ciphertext, int row_index, const char* leaf_name,
                                                            cmem_t dk_handle, cmem_t ek, cmem_t label,
                                                            const cbmpc_pve_ecies_p256_hsm_ecdh_t* cb,
                                                            cmem_t* out_share);

// Step 2: aggregate enough leaf shares to recover {x_i} for `row_index`.
//
// - `quorum_leaf_names[i]` corresponds to `quorum_shares[i]`.
//
// Notes:
// - This function intentionally does not verify `ciphertext` before reconstruction.
//   Invalid ciphertexts may cause reconstruction to fail, but are designed to not
//   leak secret information.
// - If you need ciphertext validation, call `cbmpc_pve_ac_verify(...)` first.
//
// Ownership: same as `cbmpc_pve_batch_decrypt`.
cbmpc_error_t cbmpc_pve_ac_aggregate_to_restore_row(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve,
                                                    const cbmpc_access_structure_t* ac,
                                                    const char* const* quorum_leaf_names, const cmem_t* quorum_shares,
                                                    int quorum_count, cmem_t ciphertext, int row_index, cmem_t label,
                                                    cmems_t* out_xs);

// Extract batch count from a PVE-AC ciphertext.
cbmpc_error_t cbmpc_pve_ac_get_count(cmem_t ciphertext, int* out_batch_count);

// Extract {Q_i} from a PVE-AC ciphertext (compressed point encodings).
//
// Ownership: same as `cbmpc_pve_batch_decrypt`.
cbmpc_error_t cbmpc_pve_ac_get_Qs(cmem_t ciphertext, cmems_t* out_Qs_compressed);

#ifdef __cplusplus
}
#endif
