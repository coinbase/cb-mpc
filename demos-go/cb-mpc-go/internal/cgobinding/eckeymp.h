#pragma once

#include <stdint.h>

#include <cbmpc/core/cmem.h>

#include "ac.h"
#include "curve.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Common opaque key reference (same as original definition)
// -----------------------------------------------------------------------------
typedef struct mpc_eckey_mp_ref {
  void* opaque;
} mpc_eckey_mp_ref;

// ------------------------- Memory helpers ------------------------------------
void free_mpc_eckey_mp(mpc_eckey_mp_ref ctx);

// --------------------------- Field accessors ---------------------------------
int mpc_eckey_mp_get_party_name(mpc_eckey_mp_ref* k, cmem_t* party_name_mem);
int mpc_eckey_mp_get_x_share(mpc_eckey_mp_ref* k, cmem_t* x_share_mem);
// Returns a newly allocated ecc_point_t copy – caller must free with free_ecc_point
// (see curve.h).
ecc_point_ref mpc_eckey_mp_get_Q(mpc_eckey_mp_ref* k);
ecurve_ref mpc_eckey_mp_get_curve(mpc_eckey_mp_ref* k);
int mpc_eckey_mp_get_Qis(mpc_eckey_mp_ref* k, cmems_t* party_names_mem, cmems_t* points_mem);

// ------------------------- Protocols -----------------------------------------
int mpc_eckey_mp_dkg(job_mp_ref* j, ecurve_ref* curve, mpc_eckey_mp_ref* k);
int mpc_eckey_mp_refresh(job_mp_ref* j, cmem_t sid_mem, mpc_eckey_mp_ref* k, mpc_eckey_mp_ref* new_key);

// --------------------- Threshold / Quorum helpers ---------------------------
int eckey_dkg_mp_threshold_dkg(job_mp_ref* job, ecurve_ref* curve, cmem_t sid, crypto_ss_ac_ref* ac,
                               mpc_party_set_ref* quorum, mpc_eckey_mp_ref* key);

int eckey_key_share_mp_to_additive_share(mpc_eckey_mp_ref* key, crypto_ss_ac_ref* ac, cmems_t quorum_party_names,
                                         mpc_eckey_mp_ref* additive_key);

// ------------------------- Utilities -----------------------------------------
int serialize_mpc_eckey_mp(mpc_eckey_mp_ref* k, cmems_t* ser);
int deserialize_mpc_eckey_mp(cmems_t ser, mpc_eckey_mp_ref* k);

#ifdef __cplusplus
}  // extern "C"
#endif