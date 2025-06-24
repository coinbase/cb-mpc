#pragma once

#include <stdint.h>

#include <cbmpc/core/cmem.h>

#include "ac.h"
#include "curve.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------- Type Wrappers ---------------------------
// Naming convention:
//  - drop the initial 'coinbase' namespace, replace :: with _ add ptr instead of _t
//  - We do this since direct usage of namespaces in C is not supported
// case case Example:
//    coinbase::mpc::ecdsa2pc::key_t is represented here as mpc_ecdsa2pc_key_ptr

// ------------------------- Function/Method Wrappers ----------------
// For each function in the library, create a wrapper that uses the following types:
//  - primitive types such as int, char, void, ...
//  - PTR types defined above
//  - PTR types defined in the network directory
//  - cmem_t, cmems_t types defined in the library
//
// Conventions:
//  - Implementing a method of a class, receives the class pointer as the first argument, called ctx

#ifdef __cplusplus
}  // extern "C"
#endif
