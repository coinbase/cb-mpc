#pragma once

#include <stddef.h>
#include <stdint.h>

#include <cbmpc/capi/cmem.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int cbmpc_error_t;

enum { CBMPC_SUCCESS = 0 };

// Error codes.
//
// The C API returns a subset of cbmpc's internal error codes as integers.
// We expose the common values here so C/FFI consumers can interpret failures
// without including C++ headers.
//
// Error code encoding:
//   0xff000000 | (category << 16) | code
#define CBMPC_ERRCODE(category, code) ((cbmpc_error_t)(0xff000000u | (((uint32_t)(category)) << 16) | (uint32_t)(code)))
#define CBMPC_ECATEGORY(errcode) (((uint32_t)(errcode) >> 16) & 0x00ffu)

#define CBMPC_ECATEGORY_GENERIC 0x01u
#define CBMPC_ECATEGORY_NETWORK 0x03u
#define CBMPC_ECATEGORY_CRYPTO 0x04u
#define CBMPC_ECATEGORY_OPENSSL 0x06u
#define CBMPC_ECATEGORY_CONTROL_FLOW 0x0au

#define CBMPC_UNINITIALIZED_ERROR CBMPC_ERRCODE(CBMPC_ECATEGORY_GENERIC, 0x0000u)
#define CBMPC_E_GENERAL CBMPC_ERRCODE(CBMPC_ECATEGORY_GENERIC, 0x0001u)
#define CBMPC_E_BADARG CBMPC_ERRCODE(CBMPC_ECATEGORY_GENERIC, 0x0002u)
#define CBMPC_E_FORMAT CBMPC_ERRCODE(CBMPC_ECATEGORY_GENERIC, 0x0003u)
#define CBMPC_E_NOT_SUPPORTED CBMPC_ERRCODE(CBMPC_ECATEGORY_GENERIC, 0x0005u)
#define CBMPC_E_NOT_FOUND CBMPC_ERRCODE(CBMPC_ECATEGORY_GENERIC, 0x0006u)
#define CBMPC_E_INSUFFICIENT CBMPC_ERRCODE(CBMPC_ECATEGORY_GENERIC, 0x000cu)
#define CBMPC_E_RANGE CBMPC_ERRCODE(CBMPC_ECATEGORY_GENERIC, 0x0012u)

#define CBMPC_E_NET_GENERAL CBMPC_ERRCODE(CBMPC_ECATEGORY_NETWORK, 0x0001u)

// Crypto-category errors that can surface via the API wrappers.
#define CBMPC_E_CRYPTO CBMPC_ERRCODE(CBMPC_ECATEGORY_CRYPTO, 0x0001u)
#define CBMPC_E_ECDSA_2P_BIT_LEAK CBMPC_ERRCODE(CBMPC_ECATEGORY_CRYPTO, 0x0002u)

typedef enum cbmpc_curve_id_e {
  CBMPC_CURVE_P256 = 1,
  CBMPC_CURVE_SECP256K1 = 2,
  CBMPC_CURVE_ED25519 = 3,
} cbmpc_curve_id_t;

// Memory helpers for the C API.
//
// - `cbmpc_malloc`/`cbmpc_free` are provided so FFI bindings (Go/Rust/...) can
//   use the same allocator for buffers passed across the ABI boundary.
// - Any `cmem_t` returned by the library must be freed with `cbmpc_cmem_free`
//   (which zeroizes the buffer contents before freeing).
// - Any `cmems_t` returned by the library must be freed with `cbmpc_cmems_free`
//   (which zeroizes the flat `data` buffer before freeing).
void* cbmpc_malloc(size_t size);
void cbmpc_free(void* ptr);
void cbmpc_cmem_free(cmem_t mem);
void cbmpc_cmems_free(cmems_t mems);

#ifdef __cplusplus
}
#endif
