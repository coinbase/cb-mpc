// ecdsamp.h – Signing-only C interface (key management moved to eckeymp.h)
#pragma once

#include "eckeymp.h"

#ifdef __cplusplus
extern "C" {
#endif

// Other ECDSA-MPC protocols are in eckeymp.h, to be shared with EdDSA-MPC protocols
int mpc_ecdsampc_sign(job_mp_ref* j, mpc_eckey_mp_ref* k, cmem_t msg_mem, int sig_receiver, cmem_t* sig_mem);

int mpc_ecdsampc_sign_with_ot_roles(job_mp_ref* j, mpc_eckey_mp_ref* k, cmem_t msg_mem, int sig_receiver,
                                    cmems_t ot_role_map, int n_parties, cmem_t* sig_mem);

#ifdef __cplusplus
}  // extern "C"
#endif