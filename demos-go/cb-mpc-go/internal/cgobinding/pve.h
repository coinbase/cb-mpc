#pragma once

#include <stdint.h>

#include <cbmpc/core/cmem.h>
#include <cbmpc/crypto/pki_ffi.h>

#include "ac.h"
#include "curve.h"
#include "kem.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

void pve_register_kem_functions(kem_encap_ctx_fn e, kem_decap_ctx_fn d, void* /*ignored*/, kem_dk_to_ek_ctx_fn dpub);

// Switch the currently active PKI context (used by shim wrappers).
void pve_activate_ctx(void* ctx);

int pve_encrypt(cmem_t pub_key_cmem, cmem_t x_cmem, const char* label_ptr, int curve_code, cmem_t* out_ptr);
int pve_decrypt(cmem_t prv_key_cmem, cmem_t pve_bundle_cmem, const char* label_ptr, int curve_code, cmem_t* out_x_ptr);
int pve_verify(cmem_t pub_key_cmem, cmem_t pve_bundle_cmem, cmem_t Q_cmem, const char* label_ptr);

// Quorum encryption / verification operating on a full access-structure pointer.
int pve_ac_encrypt(crypto_ss_ac_ref* ac_ptr, cmems_t names_list_ptr, cmems_t pub_keys_list_ptr, int pub_keys_count,
                   cmems_t xs_list_ptr, int xs_count, const char* label_ptr, int curve_code, cmem_t* out_ptr);
int pve_ac_verify(crypto_ss_ac_ref* ac_ptr, cmems_t names_list_ptr, cmems_t pub_keys_list_ptr, int pub_keys_count,
                  cmem_t pve_bundle_cmem, cmems_t Xs_list_ptr, int xs_count, const char* label_ptr);

// Interactive quorum decryption APIs
int pve_ac_party_decrypt_row(crypto_ss_ac_ref* ac_ptr,
    cmem_t prv_key_cmem,
    cmem_t pve_bundle_cmem,
    const char* label_ptr,
    const char* path_ptr,
    int row_index,
    cmem_t* out_share_ptr);

int pve_ac_aggregate_to_restore_row(crypto_ss_ac_ref* ac_ptr,
    cmem_t pve_bundle_cmem,
    const char* label_ptr,
    cmems_t paths_list_ptr,
    cmems_t shares_list_ptr,
    int quorum_count,
    int row_index,
    cmems_t* out_values_ptr);

#ifdef __cplusplus
}  // extern "C"
#endif