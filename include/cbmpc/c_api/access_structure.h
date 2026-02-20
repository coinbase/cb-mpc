#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// C representation of an access structure (a boolean / threshold policy tree).
//
// This type is used by access-structure protocols (e.g. *_dkg_ac, *_refresh_ac)
// to describe quorum conditions over party names.
//
// Encoding:
// - The access structure is represented as a rooted tree stored in flat arrays.
// - Each node specifies its children as a slice in the global `child_indices` array.
// - Leaf nodes carry a party name (NUL-terminated string).
//
// Requirements:
// - `root_index` must be a valid index in [0, nodes_count).
// - The node graph rooted at `root_index` must be a tree:
//   - no cycles
//   - no node reuse (DAGs are rejected)
//   - all nodes must be reachable from the root (unreachable nodes are rejected)
// - The root node must not be a leaf.
// - Leaf nodes:
//   - `leaf_name` must be non-NULL and non-empty
//   - `child_indices_count` must be 0
//   - `threshold_k` must be 0
// - AND/OR nodes:
//   - `leaf_name` must be NULL
//   - `child_indices_count` must be > 0
//   - `threshold_k` must be 0
// - THRESHOLD nodes:
//   - `leaf_name` must be NULL
//   - `child_indices_count` must be > 0
//   - `threshold_k` must satisfy 1 <= threshold_k <= child_indices_count
//
// Memory/lifetime:
// - This is a view type; it does not own any memory.
// - All pointers (including `leaf_name`) must remain valid for the duration of
//   the protocol call that consumes the access structure.
typedef enum cbmpc_access_structure_node_type_e {
  CBMPC_ACCESS_STRUCTURE_NODE_LEAF = 1,
  CBMPC_ACCESS_STRUCTURE_NODE_AND = 2,
  CBMPC_ACCESS_STRUCTURE_NODE_OR = 3,
  CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD = 4,
} cbmpc_access_structure_node_type_t;

typedef struct cbmpc_access_structure_node_t {
  cbmpc_access_structure_node_type_t type;

  // Leaf-only: party name (NUL-terminated).
  // Must be NULL for non-leaf nodes.
  const char* leaf_name;

  // Threshold-only: k in THRESHOLD[k](...).
  // Must be 0 for non-threshold nodes.
  int32_t threshold_k;

  // Children slice (global indices into `cbmpc_access_structure_t::nodes`).
  int32_t child_indices_offset;
  int32_t child_indices_count;
} cbmpc_access_structure_node_t;

typedef struct cbmpc_access_structure_t {
  const cbmpc_access_structure_node_t* nodes;
  int32_t nodes_count;

  // Flattened concatenation of all child index lists.
  const int32_t* child_indices;
  int32_t child_indices_count;

  int32_t root_index;
} cbmpc_access_structure_t;

#ifdef __cplusplus
}
#endif
