#pragma once

#include <vector>

#include <cbmpc/core/buf.h>
#include <cbmpc/core/cmem.h>

// C-callable allocators used by FFI layers (e.g., cgo).
extern "C" {
void* cgo_malloc(int size);
void cgo_free(void* ptr);
}

namespace coinbase::ffi {

// Non-owning view of a cmem_t as mem_t.
inline mem_t view(cmem_t cmem) { return mem_t(cmem.data, cmem.size); }

// Copy cmem into a new buf_t and free the source buffer.
buf_t copy_from_cmem_and_free(cmem_t cmem);

// Copy mem/buf into freshly allocated cmem_t owned by the caller.
cmem_t copy_to_cmem(mem_t mem);
cmem_t copy_to_cmem(const buf_t& buf);

// Non-owning view of cmems_t (no freeing).
std::vector<mem_t> view_cmems(cmems_t cmems);

// Copy cmems_t into new buffers (does not free the source).
std::vector<buf_t> bufs_from_cmems(cmems_t cmems);

// Convert a flat list of mem views into cmems_t (data + sizes).
cmems_t copy_to_cmems(const std::vector<mem_t>& mems);

}  // namespace coinbase::ffi
