#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// `cmem_t` is the C ABI representation of a byte slice.
//
// Invariants:
// - `size` must be non-negative.
// - If `size > 0`, then `data` must be non-NULL.
//
// Ownership:
// - A `cmem_t` may be a view into caller-owned memory *or* an owning buffer
//   allocated by the library or a callback, depending on the API contract.
typedef struct tag_cmem_t {
  uint8_t* data;
  int size;
} cmem_t;

// `cmems_t` is the C ABI representation of a list of byte slices, flattened into
// one contiguous `data` buffer and a parallel `sizes[]` array.
//
// Invariants:
// - `count` must be non-negative.
// - If `count > 0`, then `sizes` must be non-NULL.
// - Each `sizes[i]` must be non-negative.
// - Let `total = sum_i sizes[i]`.
//   - If `total > 0`, then `data` must be non-NULL.
//   - If `total == 0`, then `data` may be NULL.
//
// Layout:
// - Slice i is stored at `data + offset` with length `sizes[i]`, where
//   `offset = sum_{j < i} sizes[j]`.
//
// Ownership: same caveats as `cmem_t` (API-specific).
typedef struct tag_cmems_t {
  int count;
  uint8_t* data;
  int* sizes;
} cmems_t;

#ifdef __cplusplus
}
#endif
