#include "cmem_adapter.h"

#include <cstdlib>
#include <cstring>

extern "C" {
// NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
void* cgo_malloc(int size) { return std::malloc(static_cast<size_t>(size)); }

// NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
void cgo_free(void* ptr) { std::free(ptr); }
}  // extern "C"

namespace coinbase::ffi {

buf_t copy_from_cmem_and_free(cmem_t cmem) {
  buf_t buf(cmem.data, cmem.size);
  cgo_free(cmem.data);
  return buf;
}

cmem_t copy_to_cmem(mem_t mem) {
  cmem_t out{nullptr, mem.size};
  if (mem.size > 0) {
    out.data = static_cast<uint8_t*>(cgo_malloc(mem.size));
    if (out.data) std::memmove(out.data, mem.data, mem.size);
  }
  return out;
}

cmem_t copy_to_cmem(const buf_t& buf) { return copy_to_cmem(mem_t(buf)); }

std::vector<mem_t> view_cmems(cmems_t cmems) {
  std::vector<mem_t> out;
  if (cmems.count == 0) return out;
  out.reserve(cmems.count);
  int offset = 0;
  for (int i = 0; i < cmems.count; i++) {
    const int sz = cmems.sizes[i];
    out.emplace_back(cmems.data + offset, sz);
    offset += sz;
  }
  return out;
}

std::vector<buf_t> bufs_from_cmems(cmems_t cmems) {
  auto mems = view_cmems(cmems);
  std::vector<buf_t> bufs;
  bufs.reserve(mems.size());
  for (const auto& m : mems) bufs.emplace_back(m);
  return bufs;
}

cmems_t copy_to_cmems(const std::vector<mem_t>& mems) {
  cmems_t out{0, nullptr, nullptr};
  const auto count = static_cast<int>(mems.size());
  if (count == 0) return out;

  // Calculate total bytes.
  int total = 0;
  for (const auto& m : mems) total += m.size;

  out.count = count;
  out.data = static_cast<uint8_t*>(cgo_malloc(total));
  out.sizes = static_cast<int*>(cgo_malloc(sizeof(int) * count));
  if (!out.data || !out.sizes) {
    cgo_free(out.data);
    cgo_free(out.sizes);
    return cmems_t{0, nullptr, nullptr};
  }

  int offset = 0;
  for (int i = 0; i < count; i++) {
    out.sizes[i] = mems[i].size;
    if (mems[i].size) {
      std::memmove(out.data + offset, mems[i].data, mems[i].size);
      offset += mems[i].size;
    }
  }
  return out;
}

}  // namespace coinbase::ffi
