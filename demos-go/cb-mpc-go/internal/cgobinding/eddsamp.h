// eddsamp.h – Signing-only C interface for EdDSA multi-party
#pragma once

#include "eckeymp.h"

#ifdef __cplusplus
extern "C" {
#endif

// EdDSA-MPC signing API (other key-management functions are shared via eckeymp.h)
int mpc_eddsampc_sign(job_mp_ref* j, mpc_eckey_mp_ref* k, cmem_t msg_mem, int sig_receiver, cmem_t* sig_mem);

#ifdef __cplusplus
}  // extern "C"
#endif 