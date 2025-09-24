#pragma once

#include <stdint.h>

#include <cbmpc/crypto/pki_ffi.h>

#ifdef __cplusplus
extern "C" {
#endif

// KEM-specific context-based PKI callbacks
typedef int (*kem_encap_ctx_fn)(void* ctx, cmem_t /* ek_bytes */, cmem_t /* rho */, cmem_t* /* kem_ct out */,
                                cmem_t* /* kem_ss out */);

// Private key is passed as an opaque handle owned by the caller. For byte-based
// keys, the handle points to a cmem_t describing the bytes for the duration of
// the call.
typedef int (*kem_decap_ctx_fn)(void* ctx, const void* /* dk_handle */, cmem_t /* kem_ct */, cmem_t* /* kem_ss out */);

typedef int (*kem_dk_to_ek_ctx_fn)(void* ctx, const void* /* dk_handle */, cmem_t* /* out ek_bytes */);

#ifdef __cplusplus
}  // extern "C"
#endif