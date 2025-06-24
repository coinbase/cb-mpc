#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <cbmpc/core/cmem.h>

#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

int mpc_agree_random(job_2p_ref* job, int bit_len, cmem_t* out);

#ifdef __cplusplus
}  // extern "C"
#endif