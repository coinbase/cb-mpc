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

#include <cbmpc/core/macros.h>

namespace coinbase {

inline int bits_to_bytes_floor(int bits) { return bits >> 3; }
inline int bits_to_bytes(int bits) { return (bits + 7) >> 3; }
inline int bytes_to_bits(int bytes) { return bytes << 3; }

inline uint16_t le_get_2(const_byte_ptr src) { return *(uint16_t*)src; }
inline uint32_t le_get_4(const_byte_ptr src) { return *(uint32_t*)src; }
inline uint64_t le_get_8(const_byte_ptr src) { return *(uint64_t*)src; }
inline void le_set_2(byte_ptr dst, uint16_t value) { *(uint16_t*)dst = value; }
inline void le_set_4(byte_ptr dst, uint32_t value) { *(uint32_t*)dst = value; }
inline void le_set_8(byte_ptr dst, uint64_t value) { *(uint64_t*)dst = value; }

#if defined(__x86_64__)
inline uint16_t be_get_2(const_byte_ptr src) { return __builtin_bswap16(*(uint16_t*)src); }
inline uint32_t be_get_4(const_byte_ptr src) { return __builtin_bswap32(*(uint32_t*)src); }
inline uint64_t be_get_8(const_byte_ptr src) { return __builtin_bswap64(*(uint64_t*)src); }
inline void be_set_2(byte_ptr dst, uint16_t value) { *(uint16_t*)dst = __builtin_bswap16(value); }
inline void be_set_4(byte_ptr dst, uint32_t value) { *(uint32_t*)dst = __builtin_bswap32(value); }
inline void be_set_8(byte_ptr dst, uint64_t value) { *(uint64_t*)dst = __builtin_bswap64(value); }
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

template <typename Key, typename Value>
auto lookup(const std::map<Key, Value>& map, const Key& value) {
  using Ref = typename std::map<Key, Value>::value_type::second_type;
  auto it = map.find(value);
  return std::tuple<bool, const Ref&>(it != map.end(), it->second);
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

static inline uint64_t constant_time_select_u64(bool flag, uint64_t y, uint64_t z) {
  uint64_t mask = (uint64_t)0 - ((uint64_t)flag);
#if defined(__GNUC__) || defined(__clang__)
  // A small barrier so the compiler can't trivially treat mask as a compile-time constant
  __asm__("" : "+r"(mask) : :);
#endif
  return MASKED_SELECT(mask, y, z);
}

using uint128_t = unsigned __int128;

static inline uint64_t addx(uint64_t x, uint64_t y, uint64_t& carry) {
#ifdef __x86_64__
  unsigned long long z;
  carry = _addcarry_u64(uint8_t(carry), x, y, &z);
  return z;
#else
  auto r = uint128_t(x) + y + carry;
  carry = uint64_t(r >> 64);
  return uint64_t(r);
#endif
}

static inline uint64_t subx(uint64_t x, uint64_t y, uint64_t& borrow) {
#ifdef __x86_64__
  unsigned long long z;
  borrow = _subborrow_u64(uint8_t(borrow), x, y, &z);
  return z;
#else
  auto r = uint128_t(x) - y - borrow;
  borrow = uint64_t(r >> 64) & 1;
  return uint64_t(r);
#endif
}

}  // namespace coinbase