#pragma once
#include <cbmpc/core/error.h>
#include <cbmpc/core/macros.h>

#define ZERO128 (coinbase::buf128_t::zero())

namespace coinbase {
class converter_t;

#if defined(__x86_64__)
typedef __m128i u128_t;
#elif defined(__aarch64__)
typedef uint8x16_t u128_t;
#else
struct u128_t {
  uint64_t low, high;
};
#endif

u128_t u128_zero();
u128_t u128_load(const void*);
void u128_save(void*, u128_t);
void u128_convert(coinbase::converter_t&, u128_t&);
uint64_t u128_lo(u128_t);
uint64_t u128_hi(u128_t);
u128_t u128_make(uint64_t lo, uint64_t hi);
bool u128_lsb(u128_t);
bool u128_msb(u128_t);
u128_t u128_mask(bool);

u128_t u128_and(u128_t, bool);
bool u128_equ(u128_t, u128_t);
u128_t u128_not(u128_t);
u128_t u128_xor(u128_t, u128_t);
u128_t u128_and(u128_t, u128_t);
u128_t u128_or(u128_t, u128_t);

struct buf128_t {
  u128_t value;

  static buf128_t zero() { return u128(u128_zero()); }

  operator mem_t() const { return mem_t(byte_ptr(this), sizeof(buf128_t)); }
  buf128_t& operator=(std::nullptr_t);  // zeroization
  buf128_t& operator=(mem_t);

  operator const_byte_ptr() const { return const_byte_ptr(this); }
  operator byte_ptr() { return byte_ptr(this); }

  uint64_t lo() const;
  uint64_t hi() const;

  static buf128_t make(uint64_t lo, uint64_t hi = 0);

  static buf128_t load(const_byte_ptr src) noexcept(true);
  static buf128_t load(mem_t src);
  void save(byte_ptr dst) const;

  bool get_bit(int index) const;
  void set_bit(int index, bool bit);
  int get_bits_count() const;
  bool lsb() const { return u128_lsb(value); }
  bool msb() const { return u128_msb(value); }

  bool operator==(std::nullptr_t) const;
  bool operator!=(std::nullptr_t) const;
  bool operator==(const buf128_t& src) const;
  bool operator!=(const buf128_t& src) const;
  buf128_t operator~() const;
  buf128_t operator^(const buf128_t& src) const;
  buf128_t operator|(const buf128_t& src) const;
  buf128_t operator&(const buf128_t& src) const;
  buf128_t operator&(bool c) const { return *this & mask(c); }
  buf128_t& operator^=(const buf128_t& src);
  buf128_t& operator|=(const buf128_t& src);
  buf128_t& operator&=(const buf128_t& src);
  buf128_t& operator&=(bool c) { return *this &= mask(c); }

  static buf128_t from_bit_index(int bit_index);
  static buf128_t mask(bool x);

  buf128_t reverse_bytes() const;

  buf128_t operator<<(unsigned n) const;
  buf128_t& operator<<=(unsigned n) { return *this = *this << n; }
  buf128_t operator>>(unsigned n) const;
  buf128_t& operator>>=(unsigned n) { return *this = *this >> n; }

  byte_t operator[](int index) const {
    cb_assert(index >= 0 && index < 16);
    return (byte_ptr(this))[index];
  }
  byte_t& operator[](int index) {
    cb_assert(index >= 0 && index < 16);
    return (byte_ptr(this))[index];
  }

  void convert(coinbase::converter_t& converter);

 private:
  static buf128_t u128(u128_t val) {
    buf128_t r;
    r.value = val;
    return r;
  }
};

inline std::ostream& operator<<(std::ostream& os, const buf128_t& buf) {
  os << "buf128_t(hi: 0x" << std::hex << buf.hi() << ", lo: 0x" << std::hex << buf.lo() << std::dec << ")";
  return os;
}

}  // namespace coinbase
