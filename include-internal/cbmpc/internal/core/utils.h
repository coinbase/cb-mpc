#pragma once

#if defined(_DEBUG)
#include <iostream>
#include <mutex>
static std::mutex coutMutex;

#define DEBUG_PRINT(msg)                         \
  do {                                           \
    std::lock_guard<std::mutex> lock(coutMutex); \
    std::cout << msg << std::endl;               \
  } while (0)
#endif

#include <cstring>
#include <type_traits>

#include <cbmpc/core/buf.h>
#include <cbmpc/core/macros.h>

namespace coinbase {

inline int bits_to_bytes_floor(int bits) {
  cb_assert(bits >= 0);
  return bits >> 3;
}
inline int bits_to_bytes(int bits) {
  cb_assert(bits >= 0);
  return (bits + 7) >> 3;
}
inline int bytes_to_bits(int bytes) {
  cb_assert(bytes >= 0);
  return bytes << 3;
}

namespace detail {
inline uint16_t load_u16_unaligned(const_byte_ptr src) noexcept {
  uint16_t v;
  std::memcpy(&v, src, sizeof(v));
  return v;
}
inline uint32_t load_u32_unaligned(const_byte_ptr src) noexcept {
  uint32_t v;
  std::memcpy(&v, src, sizeof(v));
  return v;
}
inline uint64_t load_u64_unaligned(const_byte_ptr src) noexcept {
  uint64_t v;
  std::memcpy(&v, src, sizeof(v));
  return v;
}
inline void store_u16_unaligned(byte_ptr dst, uint16_t v) noexcept { std::memcpy(dst, &v, sizeof(v)); }
inline void store_u32_unaligned(byte_ptr dst, uint32_t v) noexcept { std::memcpy(dst, &v, sizeof(v)); }
inline void store_u64_unaligned(byte_ptr dst, uint64_t v) noexcept { std::memcpy(dst, &v, sizeof(v)); }
}  // namespace detail

// NOTE: These helpers must be safe for unaligned buffers and must not rely on strict-aliasing violations.
// Modern compilers typically inline fixed-size memcpy into efficient loads/stores.
inline uint16_t le_get_2(const_byte_ptr src) { return detail::load_u16_unaligned(src); }
inline uint32_t le_get_4(const_byte_ptr src) { return detail::load_u32_unaligned(src); }
inline uint64_t le_get_8(const_byte_ptr src) { return detail::load_u64_unaligned(src); }
inline void le_set_2(byte_ptr dst, uint16_t value) { detail::store_u16_unaligned(dst, value); }
inline void le_set_4(byte_ptr dst, uint32_t value) { detail::store_u32_unaligned(dst, value); }
inline void le_set_8(byte_ptr dst, uint64_t value) { detail::store_u64_unaligned(dst, value); }

#if defined(__x86_64__)
inline uint16_t be_get_2(const_byte_ptr src) { return __builtin_bswap16(detail::load_u16_unaligned(src)); }
inline uint32_t be_get_4(const_byte_ptr src) { return __builtin_bswap32(detail::load_u32_unaligned(src)); }
inline uint64_t be_get_8(const_byte_ptr src) { return __builtin_bswap64(detail::load_u64_unaligned(src)); }
inline void be_set_2(byte_ptr dst, uint16_t value) { detail::store_u16_unaligned(dst, __builtin_bswap16(value)); }
inline void be_set_4(byte_ptr dst, uint32_t value) { detail::store_u32_unaligned(dst, __builtin_bswap32(value)); }
inline void be_set_8(byte_ptr dst, uint64_t value) { detail::store_u64_unaligned(dst, __builtin_bswap64(value)); }
#else
inline uint16_t be_get_2(const_byte_ptr src) { return (uint16_t(src[0]) << 8) | src[1]; }
inline uint32_t be_get_4(const_byte_ptr src) { return (uint32_t(be_get_2(src + 0)) << 16) | be_get_2(src + 2); }
inline uint64_t be_get_8(const_byte_ptr src) { return (uint64_t(be_get_4(src + 0)) << 32) | be_get_4(src + 4); }
inline void be_set_2(byte_ptr dst, uint16_t value) {
  dst[0] = uint8_t(value >> 8);
  dst[1] = uint8_t(value);
}
inline void be_set_4(byte_ptr dst, uint32_t value) {
  be_set_2(dst, uint16_t(value >> 16));
  be_set_2(dst + 2, uint16_t(value));
}
inline void be_set_8(byte_ptr dst, uint64_t value) {
  be_set_4(dst, uint32_t(value >> 32));
  be_set_4(dst + 4, uint32_t(value));
}
#endif

inline uint64_t make_uint64(uint32_t lo, uint32_t hi) { return (uint64_t(hi) << 32) | lo; }

template <typename T>
class array_view_t {
 public:
  array_view_t(const T* _ptr, int _count) : ptr((T*)_ptr), count(_count) {}
  array_view_t(T* _ptr, int _count) : ptr(_ptr), count(_count) {}

 public:
  T* ptr;
  int count;
};

inline int int_log2(uint32_t x) {
  if (x <= 1) return x;
  return 32 - __builtin_clz(x - 1);
};

template <typename Mask, typename Y, typename Z>
constexpr auto masked_select(Mask mask, Y y, Z z) noexcept {
  using Common = std::common_type_t<Mask, Y, Z>;
  static_assert(std::is_integral_v<Common>, "masked_select requires integral inputs");
  return (static_cast<Common>(y) & static_cast<Common>(mask)) | (static_cast<Common>(z) & ~static_cast<Common>(mask));
}

template <typename Key, typename Value>
auto lookup(const std::map<Key, Value>& map, const Key& value) {
  using Ref = typename std::map<Key, Value>::value_type::second_type;
  auto it = map.find(value);
  // NOTE: Never return `it->second` by reference unconditionally here: when `it == map.end()`,
  // dereferencing is undefined behavior even if the caller checks the accompanying boolean.
  const Ref* ref = (it != map.end()) ? &it->second : nullptr;
  return std::tuple<bool, const Ref*>(ref != nullptr, ref);
}

template <typename Container, typename Value>
bool has(const Container& container, const Value& value) {
  return std::find(container.begin(), container.end(), value) != container.end();
}

template <typename Key, typename Value>
bool has(const std::map<Key, Value>& map, const Key& value) {
  return map.find(value) != map.end();
}

template <typename... ARGS, typename Func, std::size_t... Is>
void for_tuple_impl(const std::tuple<ARGS&...>& tuple, Func&& f, std::index_sequence<Is...>) {
  (f(std::get<Is>(tuple)), ...);
}

template <typename... ARGS, typename Func>
void for_tuple(const std::tuple<ARGS&...>& tuple, Func&& f) {
  for_tuple_impl(tuple, std::forward<Func>(f), std::make_index_sequence<sizeof...(ARGS)>{});
}

static inline uint64_t constant_time_value_barrier(uint64_t value) {
#if defined(__GNUC__) || defined(__clang__)
  // A small barrier so the compiler can't trivially treat mask as a compile-time constant
  __asm__("" : "+r"(value) : :);
#endif
  return value;
}

static inline uint64_t constant_time_mask_64(uint64_t flag) { return constant_time_value_barrier((uint64_t)0 - flag); }

static inline uint64_t constant_time_select_u64(bool flag, uint64_t y, uint64_t z) {
  uint64_t mask = constant_time_mask_64(flag);
  return masked_select(mask, y, z);
}

// Constant-time selection for a byte array.
// Writes `y` to `out` when flag == true, else writes `z` to `out`.
static inline void constant_time_select_bytes(bool flag, const_byte_ptr y, const_byte_ptr z, byte_ptr out, int size) {
  // `size` is public; `flag` is assumed potentially secret.
  cb_assert(size >= 0);
  uint8_t mask = static_cast<uint8_t>(constant_time_mask_64(static_cast<uint64_t>(flag)));
  uint8_t inv_mask = static_cast<uint8_t>(~mask);
  for (int i = 0; i < size; i++) {
    out[i] = static_cast<uint8_t>((y[i] & mask) | (z[i] & inv_mask));
  }
}

// Constant-time selection between two same-sized buffers.
// Returns `y` when flag == true, else returns `z`.
static inline buf_t ct_select_buf(bool flag, mem_t y, mem_t z) {
  cb_assert(y.size == z.size);
  buf_t out(y.size);
  constant_time_select_bytes(flag, y.data, z.data, out.data(), y.size);
  return out;
}

}  // namespace coinbase