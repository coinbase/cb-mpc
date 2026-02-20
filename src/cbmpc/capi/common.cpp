#include <cstdlib>
#include <openssl/crypto.h>

#include <cbmpc/capi/common.h>

#if defined(__APPLE__)
#include <malloc/malloc.h>
#elif defined(__linux__) && defined(__GLIBC__)
#include <malloc.h>
#endif

extern "C" {

static void secure_bzero(void* ptr, size_t size) {
  if (!ptr || size == 0) return;
  OPENSSL_cleanse(ptr, size);
}

static size_t malloc_usable_bytes(void* ptr) {
  if (!ptr) return 0;
#if defined(__APPLE__)
  return malloc_size(ptr);
#elif defined(__linux__) && defined(__GLIBC__)
  return malloc_usable_size(ptr);
#else
  return 0;
#endif
}

// NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
void* cbmpc_malloc(size_t size) {
  if (size == 0) return nullptr;
  return std::malloc(size);
}

void cbmpc_free(void* ptr) { std::free(ptr); }

void cbmpc_cmem_free(cmem_t mem) {
  if (mem.data && mem.size > 0) secure_bzero(mem.data, static_cast<size_t>(mem.size));
  cbmpc_free(mem.data);
}

void cbmpc_cmems_free(cmems_t mems) {
  if (mems.data) {
    // Prefer wiping the full malloc allocation when possible. This avoids
    // relying on `mems.sizes` being well-formed for zeroization.
    size_t wipe_len = malloc_usable_bytes(mems.data);
    if (wipe_len == 0 && mems.count > 0 && mems.sizes) {
      // Best-effort fallback: sum non-negative segment sizes.
      size_t total = 0;
      for (int i = 0; i < mems.count; i++) {
        const int sz = mems.sizes[i];
        if (sz > 0) total += static_cast<size_t>(sz);
      }
      wipe_len = total;
    }
    secure_bzero(mems.data, wipe_len);
  }
  cbmpc_free(mems.data);
  cbmpc_free(mems.sizes);
}

}  // extern "C"
