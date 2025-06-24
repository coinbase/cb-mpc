#pragma once

#include <stdint.h>

#include <cbmpc/core/cmem.h>

#include "curve.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------- Type Wrappers ---------------------------
// Wrapper for coinbase::mpc::ecdsa2pc::key_t

typedef struct mpc_ecdsa2pc_key_ref {
  void* opaque;  // Opaque pointer to the C++ class instance
} mpc_ecdsa2pc_key_ref;

// ------------------------- Memory management -----------------------
void free_mpc_ecdsa2p_key(mpc_ecdsa2pc_key_ref ctx);

// ------------------------- Function Wrappers -----------------------

int mpc_ecdsa2p_dkg(job_2p_ref* job, int curve, mpc_ecdsa2pc_key_ref* key);

int mpc_ecdsa2p_refresh(job_2p_ref* job, mpc_ecdsa2pc_key_ref* key, mpc_ecdsa2pc_key_ref* new_key);

int mpc_ecdsa2p_sign(job_2p_ref* job, cmem_t sid, mpc_ecdsa2pc_key_ref* key, cmems_t msgs, cmems_t* sigs);

// Returns the role index (e.g., 0 or 1) corresponding to the given key share.
// Returns a negative value on error.
int mpc_ecdsa2p_key_get_role_index(mpc_ecdsa2pc_key_ref* key);

// Returns a freshly allocated copy of the public key point Q.
// The caller is responsible for freeing the returned ecc_point_ref via free_ecc_point.
ecc_point_ref mpc_ecdsa2p_key_get_Q(mpc_ecdsa2pc_key_ref* key);

// Returns the secret share x_i of the private key as a byte slice (big-endian).
// The caller is responsible for freeing the returned memory via cgo_free.
cmem_t mpc_ecdsa2p_key_get_x_share(mpc_ecdsa2pc_key_ref* key);

// Returns the numeric OpenSSL NID identifying the curve associated with the provided key.
// A negative value indicates an error.
int mpc_ecdsa2p_key_get_curve_code(mpc_ecdsa2pc_key_ref* key);

#ifdef __cplusplus
}  // extern "C"
#endif