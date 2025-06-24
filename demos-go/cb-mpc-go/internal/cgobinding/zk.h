#pragma once

#include <stdint.h>

#include <cbmpc/core/cmem.h>

#include "curve.h"

#ifdef __cplusplus
extern "C" {
#endif

int zk_dl_prove(ecc_point_ref* Q, cmem_t w_mem, cmem_t sid_mem, uint64_t aux, cmem_t* proof_mem);
int zk_dl_verify(ecc_point_ref* Q, cmem_t proof_mem, cmem_t sid_mem, uint64_t aux);

#ifdef __cplusplus
}  // extern "C"
#endif