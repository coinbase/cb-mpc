#include "pki.h"

// Weak stubs for callback getters so core C++ unit/integration tests can link
// without any language-specific FFI layer (Go, Python, Rust, …). When an FFI
// layer is linked, its strong definitions override these stubs.
extern "C" {

__attribute__((weak)) ffi_verify_fn get_ffi_verify_fn(void) { return nullptr; }
__attribute__((weak)) ffi_sign_fn get_ffi_sign_fn(void) { return nullptr; }

__attribute__((weak)) ffi_kem_encap_fn get_ffi_kem_encap_fn(void) { return nullptr; }
__attribute__((weak)) ffi_kem_decap_fn get_ffi_kem_decap_fn(void) { return nullptr; }
__attribute__((weak)) ffi_kem_dk_to_ek_fn get_ffi_kem_dk_to_ek_fn(void) { return nullptr; }

}  // extern "C"
