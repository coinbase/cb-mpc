#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <cbmpc/core/cmem.h>

#include "curve.h"

// Stand-alone opaque pointer wrapper for access-structure trees.
// The concrete type is defined inside the C++ implementation.
typedef struct crypto_ss_ac_ref {
  void* opaque;  // Opaque pointer to the C++ access-structure instance
} crypto_ss_ac_ref;

// Opaque pointer wrapper for secret-sharing tree nodes.
typedef struct crypto_ss_node_ref {
  void* opaque;  // Opaque pointer to the C++ node_t instance
} crypto_ss_node_ref;

// Function prototypes for secret-sharing access structure nodes.
// Creates a new node of the given type/name/threshold. The caller owns the returned pointer.
crypto_ss_node_ref new_node(int node_type, cmem_t node_name, int threshold);
// Adds |child| as a child of |parent|. Both pointers must reference valid nodes.
void add_child(crypto_ss_node_ref* parent, crypto_ss_node_ref* child);
// Constructs and returns a new access-structure given a root node and the
// curve reference. The caller owns the returned pointer and must release it
// via free_crypto_ss_ac.
crypto_ss_ac_ref new_access_structure(crypto_ss_node_ref* root, ecurve_ref* curve);

// Releases memory held by a native access-structure. The caller must
// invoke this once the object is no longer needed to avoid memory leaks.
void free_crypto_ss_ac(crypto_ss_ac_ref ac);

#ifdef __cplusplus
}  // extern "C"
#endif