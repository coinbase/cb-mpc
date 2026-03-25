#pragma once

#include <cbmpc/core/buf.h>
#include <cbmpc/core/macros.h>

namespace coinbase {

#ifdef INTEL_X64
bool support_x64_mulx();
#endif

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

struct uint256_t {
  uint64_t w0, w1, w2, w3;

  void to_bin(byte_ptr bin) const;
  buf_t to_bin() const;
  static uint256_t from_bin(mem_t bin);

  bool is_zero() const { return (w0 | w1 | w2 | w3) == 0; }
  bool is_odd() const { return bool(w0 & 1); }
  bool operator==(const uint256_t& b) const;
  void cnd_assign(bool flag, const uint256_t& b);
  static uint256_t make(uint64_t w0, uint64_t w1 = 0, uint64_t w2 = 0, uint64_t w3 = 0) {
    return uint256_t{w0, w1, w2, w3};
  }
};

}  // namespace coinbase
