#pragma once

#include <map>
#include <string_view>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/pve.h>
#include <cbmpc/core/access_structure.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

namespace coinbase::api::pve {

// ---------------------------------------------------------------------------
// PVE-AC (access-structure / quorum decryption)
// ---------------------------------------------------------------------------
//
// This API encrypts a *batch* of scalars {x_i} under a leaf-keyed access structure.
//
// Decryption is multi-party in principle:
// - Each party decrypts its own leaf ciphertext to produce a share (step function).
// - An application collects enough shares to satisfy the access structure and aggregates them to recover {x_i}.

// Leaf-key maps (leaf name -> base-PKE key blob).
//
// - For the built-in base PKE (`base_pke_default()`), the key blob format is the same as used by `pve.h`:
//   RSA-OAEP(2048) or ECIES(P-256) public/private blobs.
using leaf_keys_t = std::map<std::string_view, mem_t>;

// Leaf-share map (leaf name -> decrypted share bytes).
using leaf_shares_t = std::map<std::string_view, mem_t>;

// Encrypt a batch of scalars {x_i} under an access structure.
//
// - `ac_pks` must provide a public key for every leaf in `ac`.
// - Each `xs[i]` is interpreted as a big-endian integer and reduced modulo the curve order.
// - Defensive limit: each `xs[i].size` must be <= the curve order size in bytes.
error_t encrypt_ac(const base_pke_i& base_pke, curve_id curve, const access_structure_t& ac, const leaf_keys_t& ac_pks,
                   mem_t label, const std::vector<mem_t>& xs, buf_t& out_ciphertext);

// Same as `encrypt_ac(...)` using `base_pke_default()`.
error_t encrypt_ac(curve_id curve, const access_structure_t& ac, const leaf_keys_t& ac_pks, mem_t label,
                   const std::vector<mem_t>& xs, buf_t& out_ciphertext);

// Verify an access-structure ciphertext against expected Qs and label.
error_t verify_ac(const base_pke_i& base_pke, curve_id curve, const access_structure_t& ac, const leaf_keys_t& ac_pks,
                  mem_t ciphertext, const std::vector<mem_t>& Qs_compressed, mem_t label);

// Same as `verify_ac(...)` using `base_pke_default()`.
error_t verify_ac(curve_id curve, const access_structure_t& ac, const leaf_keys_t& ac_pks, mem_t ciphertext,
                  const std::vector<mem_t>& Qs_compressed, mem_t label);

// Step 1: each party decrypts its share for a specific row.
//
// Output:
// - `out_share` is a fixed-length big-endian scalar encoding with length equal
//   to the outer curve order size in bytes (e.g., 32 bytes for secp256k1).
error_t party_decrypt_row_ac(const base_pke_i& base_pke, curve_id curve, const access_structure_t& ac, mem_t ciphertext,
                             int row_index, std::string_view leaf_name, mem_t dk, mem_t label, buf_t& out_share);

// Same as `party_decrypt_row_ac(...)` using `base_pke_default()`.
error_t party_decrypt_row_ac(curve_id curve, const access_structure_t& ac, mem_t ciphertext, int row_index,
                             std::string_view leaf_name, mem_t dk, mem_t label, buf_t& out_share);

// Step 1 (HSM): decrypt a single leaf share for a specific row using an HSM-backed
// RSA-OAEP private key (KEM decapsulation callback).
//
// - `dk_handle` is an opaque handle (or identifier) understood by the HSM callback.
// - `ek` is the leaf's built-in base PKE public key blob (used to validate key type).
error_t party_decrypt_row_ac_rsa_oaep_hsm(curve_id curve, const access_structure_t& ac, mem_t ciphertext, int row_index,
                                          std::string_view leaf_name, mem_t dk_handle, mem_t ek, mem_t label,
                                          const rsa_oaep_hsm_decap_cb_t& cb, buf_t& out_share);

// Step 1 (HSM): decrypt a single leaf share for a specific row using an HSM-backed
// ECIES(P-256) private key (ECDH callback only).
//
// - `dk_handle` is an opaque handle (or identifier) understood by the HSM callback.
// - `ek` is the leaf's built-in base PKE public key blob (used to validate key type
//   and derive the KEM context).
error_t party_decrypt_row_ac_ecies_p256_hsm(curve_id curve, const access_structure_t& ac, mem_t ciphertext,
                                            int row_index, std::string_view leaf_name, mem_t dk_handle, mem_t ek,
                                            mem_t label, const ecies_p256_hsm_ecdh_cb_t& cb, buf_t& out_share);

// Step 2: aggregate enough decrypted shares to recover {x_i} for a specific row.
//
// - `quorum_shares` must satisfy the access structure `ac`.
//
// Notes:
// - This function intentionally does not verify `ciphertext` before reconstruction.
//   Invalid ciphertexts may cause reconstruction to fail, but are designed to not
//   leak secret information.
// - If you need ciphertext validation, call `verify_ac(...)` first.
//
// Output:
// - `out_xs[i]` is a fixed-length big-endian encoding of x_i with length equal
//   to the curve order size in bytes.
error_t aggregate_to_restore_row_ac(const base_pke_i& base_pke, curve_id curve, const access_structure_t& ac,
                                    mem_t ciphertext, int row_index, mem_t label, const leaf_shares_t& quorum_shares,
                                    std::vector<buf_t>& out_xs);

// Same as `aggregate_to_restore_row_ac(...)` using `base_pke_default()`.
error_t aggregate_to_restore_row_ac(curve_id curve, const access_structure_t& ac, mem_t ciphertext, int row_index,
                                    mem_t label, const leaf_shares_t& quorum_shares, std::vector<buf_t>& out_xs);

// Extract batch count (number of scalars) from a PVE-AC ciphertext.
error_t get_ac_batch_count(mem_t ciphertext, int& out_batch_count);

// Extract the public values {Q_i} whose discrete logs are encrypted as the ciphertext, returning compressed point
// encodings.
error_t get_Qs_ac(mem_t ciphertext, std::vector<buf_t>& out_Qs_compressed);

}  // namespace coinbase::api::pve
