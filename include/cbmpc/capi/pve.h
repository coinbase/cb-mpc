#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <cbmpc/capi/common.h>

#ifdef __cplusplus
extern "C" {
#endif

// Pluggable base PKE callbacks for PVE (C API).
//
// Requirements:
// - `encrypt` must be deterministic given `rho`.
// - Any callback that writes `cmem_t* out` MUST allocate `out->data` with
//   `cbmpc_malloc(out->size)`. cbmpc will free returned buffers with
//   `cbmpc_cmem_free(...)` (zeroizes before free).
//
// Key format is defined by the callback implementation.
typedef cbmpc_error_t (*cbmpc_pve_base_pke_encrypt_fn)(void* ctx, cmem_t ek, cmem_t label, cmem_t plain, cmem_t rho,
                                                       cmem_t* out_ct);

typedef cbmpc_error_t (*cbmpc_pve_base_pke_decrypt_fn)(void* ctx, cmem_t dk, cmem_t label, cmem_t ct,
                                                       cmem_t* out_plain);

typedef struct cbmpc_pve_base_pke_t {
  void* ctx;
  cbmpc_pve_base_pke_encrypt_fn encrypt;
  cbmpc_pve_base_pke_decrypt_fn decrypt;
} cbmpc_pve_base_pke_t;

// ---------------------------------------------------------------------------
// Built-in base PKE software keys
// ---------------------------------------------------------------------------

// Generate a keypair for cbmpc's built-in RSA-OAEP (2048-bit) base PKE.
//
// Outputs are opaque, versioned byte strings compatible with passing
// `base_pke = NULL` to `cbmpc_pve_encrypt/verify/decrypt`.
//
// Ownership: same as `cbmpc_pve_encrypt`.
cbmpc_error_t cbmpc_pve_generate_base_pke_rsa_keypair(cmem_t* out_ek, cmem_t* out_dk);

// Generate a keypair for cbmpc's built-in ECIES (P-256) base PKE.
//
// Outputs are opaque, versioned byte strings compatible with passing
// `base_pke = NULL` to `cbmpc_pve_encrypt/verify/decrypt`.
//
// Ownership: same as `cbmpc_pve_encrypt`.
cbmpc_error_t cbmpc_pve_generate_base_pke_ecies_p256_keypair(cmem_t* out_ek, cmem_t* out_dk);

// Build a cbmpc ECIES(P-256) base PKE public key blob from an external public key.
//
// This is useful when the private key lives in an HSM (or other external system)
// and only the public key can be exported to software.
//
// Input format:
// - `pub_key_oct` must be the *uncompressed* NIST P-256 public key octet string:
//   65 bytes: 0x04 || X(32) || Y(32).
//
// Output:
// - `out_ek` is an opaque, versioned byte string compatible with passing
//   `base_pke = NULL` to `cbmpc_pve_encrypt/verify/decrypt`, and with
//   `cbmpc_pve_decrypt_ecies_p256_hsm`.
//
// Ownership: same as `cbmpc_pve_encrypt`.
cbmpc_error_t cbmpc_pve_base_pke_ecies_p256_ek_from_oct(cmem_t pub_key_oct, cmem_t* out_ek);

// ---------------------------------------------------------------------------
// Built-in base PKE HSM support (KEM decapsulation callbacks)
// ---------------------------------------------------------------------------

// RSA-OAEP decapsulation callback.
//
// The callback must allocate `out_kem_ss->data` with `cbmpc_malloc`.
// The output is the KEM shared secret (OAEP decrypted value).
typedef cbmpc_error_t (*cbmpc_pve_rsa_oaep_hsm_decap_fn)(void* ctx, cmem_t dk_handle, cmem_t kem_ct,
                                                         cmem_t* out_kem_ss);

typedef struct cbmpc_pve_rsa_oaep_hsm_decap_t {
  void* ctx;
  cbmpc_pve_rsa_oaep_hsm_decap_fn decap;
} cbmpc_pve_rsa_oaep_hsm_decap_t;

// ECIES(P-256) ECDH callback.
//
// The callback must allocate `out_dh_x32->data` with `cbmpc_malloc`.
// The output must be exactly 32 bytes (affine-X coordinate, big-endian).
typedef cbmpc_error_t (*cbmpc_pve_ecies_p256_hsm_ecdh_fn)(void* ctx, cmem_t dk_handle, cmem_t kem_ct,
                                                          cmem_t* out_dh_x32);

typedef struct cbmpc_pve_ecies_p256_hsm_ecdh_t {
  void* ctx;
  cbmpc_pve_ecies_p256_hsm_ecdh_fn ecdh;
} cbmpc_pve_ecies_p256_hsm_ecdh_t;

// Decrypt using an HSM-backed RSA private key (KEM decapsulation callback).
//
// - `dk_handle` is an opaque handle understood by the callback.
// - `ek` is the software public key blob (used to validate key type and for optional pre-validation via
// `cbmpc_pve_verify`).
//
// Ownership:
// - On success, `out_x->data` is allocated by the library and must be freed with
//   `cbmpc_cmem_free(*out_x)`.
// - On failure, `*out_x` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_pve_decrypt_rsa_oaep_hsm(cbmpc_curve_id_t curve, cmem_t dk_handle, cmem_t ek, cmem_t ciphertext,
                                             cmem_t label, const cbmpc_pve_rsa_oaep_hsm_decap_t* cb, cmem_t* out_x);

// Decrypt using an HSM-backed ECIES(P-256) private key (ECDH callback).
//
// - `dk_handle` is an opaque handle understood by the callback.
// - `ek` is the software public key blob (used for KEM context and optional pre-validation via `cbmpc_pve_verify`).
//
// Ownership: same as `cbmpc_pve_decrypt_rsa_oaep_hsm`.
cbmpc_error_t cbmpc_pve_decrypt_ecies_p256_hsm(cbmpc_curve_id_t curve, cmem_t dk_handle, cmem_t ek, cmem_t ciphertext,
                                               cmem_t label, const cbmpc_pve_ecies_p256_hsm_ecdh_t* cb, cmem_t* out_x);

// ---------------------------------------------------------------------------
// Custom KEM (library provides KEM/DEM transform)
// ---------------------------------------------------------------------------

// Custom KEM encapsulation callback.
//
// - Must be deterministic given `rho32`.
// - Must allocate outputs with `cbmpc_malloc`.
typedef cbmpc_error_t (*cbmpc_pve_kem_encap_fn)(void* ctx, cmem_t ek, cmem_t rho32, cmem_t* out_kem_ct,
                                                cmem_t* out_kem_ss);

// Custom KEM decapsulation callback.
//
// - Must allocate outputs with `cbmpc_malloc`.
typedef cbmpc_error_t (*cbmpc_pve_kem_decap_fn)(void* ctx, cmem_t dk, cmem_t kem_ct, cmem_t* out_kem_ss);

typedef struct cbmpc_pve_base_kem_t {
  void* ctx;
  cbmpc_pve_kem_encap_fn encap;
  cbmpc_pve_kem_decap_fn decap;
} cbmpc_pve_base_kem_t;

// Encrypt using a custom KEM (cbmpc provides HKDF + AES-GCM DEM).
//
// Ownership: same as `cbmpc_pve_encrypt`.
cbmpc_error_t cbmpc_pve_encrypt_with_kem(const cbmpc_pve_base_kem_t* kem, cbmpc_curve_id_t curve, cmem_t ek,
                                         cmem_t label, cmem_t x, cmem_t* out_ciphertext);

// Verify using a custom KEM (requires `kem->encap` for deterministic recomputation).
cbmpc_error_t cbmpc_pve_verify_with_kem(const cbmpc_pve_base_kem_t* kem, cbmpc_curve_id_t curve, cmem_t ek,
                                        cmem_t ciphertext, cmem_t Q_compressed, cmem_t label);

// Decrypt using a custom KEM (cbmpc provides HKDF + AES-GCM DEM).
//
// Ownership: same as `cbmpc_pve_decrypt`.
cbmpc_error_t cbmpc_pve_decrypt_with_kem(const cbmpc_pve_base_kem_t* kem, cbmpc_curve_id_t curve, cmem_t dk, cmem_t ek,
                                         cmem_t ciphertext, cmem_t label, cmem_t* out_x);

// Encrypt a scalar `x` under base encryption key `ek`, producing a PVE ciphertext.
//
// If `base_pke` is NULL, cbmpc uses its built-in default base PKE.
//
// Ownership:
// - On success, `out_ciphertext->data` is allocated by the library and must be
//   freed with `cbmpc_cmem_free(*out_ciphertext)`.
// - On failure, `*out_ciphertext` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_pve_encrypt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t ek, cmem_t label,
                                cmem_t x, cmem_t* out_ciphertext);

// Verify ciphertext validity against the expected Q and label.
//
// - `Q_compressed` is the compressed point encoding for `curve`.
// - If `base_pke` is NULL, cbmpc uses its built-in default base PKE.
cbmpc_error_t cbmpc_pve_verify(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t ek,
                               cmem_t ciphertext, cmem_t Q_compressed, cmem_t label);

// Decrypt a ciphertext, recovering the scalar x.
//
// Notes:
// - This function intentionally does not verify `ciphertext` before decryption.
//   Invalid ciphertexts may cause decryption to fail, but are designed to not
//   leak secret information.
// - If you need ciphertext validation, call `cbmpc_pve_verify(...)` (or `cbmpc_pve_verify_with_kem(...)`) first.
//
// Ownership:
// - On success, `out_x->data` is allocated by the library and must be freed with
//   `cbmpc_cmem_free(*out_x)`.
// - On failure, `*out_x` is set to `{NULL, 0}`.
cbmpc_error_t cbmpc_pve_decrypt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t dk, cmem_t ek,
                                cmem_t ciphertext, cmem_t label, cmem_t* out_x);

// Extract Q from a ciphertext (compressed point encoding).
//
// Ownership: same as `cbmpc_pve_encrypt`.
cbmpc_error_t cbmpc_pve_get_Q(cmem_t ciphertext, cmem_t* out_Q_compressed);

// Extract label from a ciphertext.
//
// Ownership: same as `cbmpc_pve_encrypt`.
cbmpc_error_t cbmpc_pve_get_Label(cmem_t ciphertext, cmem_t* out_label);

#ifdef __cplusplus
}
#endif
