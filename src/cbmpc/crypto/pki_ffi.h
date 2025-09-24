#pragma once

#include <stddef.h>
#include <stdint.h>

#include <cbmpc/core/cmem.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations of functions that retrieve callbacks exposed via FFI.

// Digital signature functions
typedef int (*ffi_sign_fn)(cmem_t /* sk */, cmem_t /* hash */, cmem_t* /* signature out */);
typedef int (*ffi_verify_fn)(cmem_t /* vk */, cmem_t /* hash */, cmem_t /* signature */);

ffi_sign_fn get_ffi_sign_fn(void);
ffi_verify_fn get_ffi_verify_fn(void);

// KEM functions
typedef int (*ffi_kem_encap_fn)(cmem_t /* ek_bytes */, cmem_t /* rho */, cmem_t* /* kem_ct out */,
                                cmem_t* /* kem_ss out */);
// Private key is treated as an opaque, process-local handle managed by the host.
// It must not be serialized or inspected by the callee.
typedef int (*ffi_kem_decap_fn)(const void* /* dk_handle */, cmem_t /* kem_ct */, cmem_t* /* kem_ss out */);
typedef int (*ffi_kem_dk_to_ek_fn)(const void* /* dk_handle */, cmem_t* /* out ek_bytes */);

ffi_kem_encap_fn get_ffi_kem_encap_fn(void);
ffi_kem_decap_fn get_ffi_kem_decap_fn(void);
ffi_kem_dk_to_ek_fn get_ffi_kem_dk_to_ek_fn(void);

#ifdef __cplusplus
}
#endif
