#pragma once

#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/pve.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

namespace coinbase::api::pve {

// ---------------------------------------------------------------------------
// Batch PVE (1P) API
// ---------------------------------------------------------------------------
//
// This API batches the core PVE algorithm for multiple scalars {x_i} in one
// ciphertext, sharing the same label and base PKE key.
//
// Notes:
// - `xs[i]` is interpreted as a big-endian integer and reduced modulo the curve order.
// - Defensive limit: each `xs[i].size` must be <= the curve order size in bytes.
// - `Qs_compressed[i]` is the compressed point encoding for the outer curve:
//   33 bytes for P-256/secp256k1, 32 bytes for Ed25519.

error_t encrypt_batch(const base_pke_i& base_pke, curve_id curve, mem_t ek, mem_t label, const std::vector<mem_t>& xs,
                      buf_t& out_ciphertext);

// Same as `encrypt_batch(...)` using `base_pke_default()`.
error_t encrypt_batch(curve_id curve, mem_t ek, mem_t label, const std::vector<mem_t>& xs, buf_t& out_ciphertext);

error_t verify_batch(const base_pke_i& base_pke, curve_id curve, mem_t ek, mem_t ciphertext,
                     const std::vector<mem_t>& Qs_compressed, mem_t label);

// Same as `verify_batch(...)` using `base_pke_default()`.
error_t verify_batch(curve_id curve, mem_t ek, mem_t ciphertext, const std::vector<mem_t>& Qs_compressed, mem_t label);

// Decrypt a batch ciphertext, recovering the vector of scalars {x_i}.
//
// Output:
// - On success, `out_xs[i]` is a fixed-length big-endian encoding of x_i with
//   length equal to the curve order size in bytes (e.g., 32 bytes for P-256).
//
// Notes:
// - This function intentionally does not verify `ciphertext` before decryption.
//   Invalid ciphertexts may cause decryption to fail, but are designed to not
//   leak secret information.
// - If you need ciphertext validation, call `verify_batch(...)` first.
error_t decrypt_batch(const base_pke_i& base_pke, curve_id curve, mem_t dk, mem_t ek, mem_t ciphertext, mem_t label,
                      std::vector<buf_t>& out_xs);

// Same as `decrypt_batch(...)` using `base_pke_default()`.
error_t decrypt_batch(curve_id curve, mem_t dk, mem_t ek, mem_t ciphertext, mem_t label, std::vector<buf_t>& out_xs);

// Decrypt a batch ciphertext using an HSM-backed RSA private key.
error_t decrypt_batch_rsa_oaep_hsm(curve_id curve, mem_t dk_handle, mem_t ek, mem_t ciphertext, mem_t label,
                                   const rsa_oaep_hsm_decap_cb_t& cb, std::vector<buf_t>& out_xs);

// Decrypt a batch ciphertext using an HSM-backed ECIES(P-256) private key.
error_t decrypt_batch_ecies_p256_hsm(curve_id curve, mem_t dk_handle, mem_t ek, mem_t ciphertext, mem_t label,
                                     const ecies_p256_hsm_ecdh_cb_t& cb, std::vector<buf_t>& out_xs);

// Extract batch count from a batch ciphertext.
error_t get_batch_count(mem_t ciphertext, int& out_batch_count);

// Extract the public values {Q_i} whose discrete logs are encrypted as the ciphertext, returning compressed point
// encodings.
error_t get_Qs_batch(mem_t ciphertext, std::vector<buf_t>& out_Qs_compressed);

// Extract the associated label from a batch ciphertext.
error_t get_Label_batch(mem_t ciphertext, buf_t& out_label);

}  // namespace coinbase::api::pve
