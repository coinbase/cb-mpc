#pragma once

#include <stdint.h>

#include <cbmpc/core/cmem.h>

#include "ac.h"
#include "curve.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Public Verifiable Encryption (PVE) helper C API – extracted from cblib.h
// ============================================================================

// Generate |n| ECIES encryption key pairs (private/public).
// The caller *owns* the returned cmems_t buffers and must free them via
// CMEMSGet (from the Go side) which zeroizes and frees the memory.
int get_n_enc_keypairs(int n, cmems_t* prv_keys_ptr, cmems_t* pub_keys_ptr);

// Generate |n| raw EC scalar + EC point key pairs.
int get_n_ec_keypairs(int n, cmems_t* prv_keys_ptr, cmems_t* pub_keys_ptr);

// Perform quorum decryption.
int pve_quorum_decrypt(crypto_ss_node_ref* root_ptr, cmems_t quorum_prv_keys_list_ptr, int quorum_prv_keys_count,
                       cmems_t all_pub_keys_list_ptr, int all_pub_keys_count, cmem_t pve_bundle_cmem,
                       cmems_t Xs_list_ptr, int xs_count, const char* label_ptr, cmems_t* out_ptr);

// Generate a single base encryption key pair (crypto::pub_key_t, crypto::prv_key_t).
// The returned cmem_t buffers are owned by the caller and must be freed via CMEMGet on the Go side.
int generate_base_enc_keypair(cmem_t* prv_key_ptr, cmem_t* pub_key_ptr);

// Quorum encryption / decryption operating on a full access-structure pointer.
int pve_quorum_encrypt_map(crypto_ss_ac_ref* ac_ptr, cmems_t names_list_ptr, cmems_t pub_keys_list_ptr,
                            int pub_keys_count, cmems_t xs_list_ptr, int xs_count, const char* label_ptr,
                            int curve_code, cmem_t* out_ptr);

int pve_quorum_decrypt_map(crypto_ss_ac_ref* ac_ptr, cmems_t quorum_prv_keys_list_ptr, int quorum_prv_keys_count,
                           cmems_t all_pub_keys_list_ptr, int all_pub_keys_count, cmem_t pve_bundle_cmem,
                           cmems_t Xs_list_ptr, int xs_count, const char* label_ptr, cmems_t* out_ptr);

// === New API: Quorum verification without private keys ===
int pve_quorum_verify_map(crypto_ss_ac_ref* ac_ptr, cmems_t names_list_ptr, cmems_t pub_keys_list_ptr,
                          int pub_keys_count, cmem_t pve_bundle_cmem, cmems_t Xs_list_ptr, int xs_count,
                          const char* label_ptr);

#ifdef __cplusplus
}  // extern "C"
#endif