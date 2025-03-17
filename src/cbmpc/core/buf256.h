#pragma once
#include "buf128.h"

#define ZERO256 (buf256_t::zero())

namespace coinbase {
struct buf256_t {
  buf128_t lo, hi;

  buf256_t& operator=(mem_t src);
  buf256_t& operator=(const coinbase::buf_t& src);
  buf256_t& operator=(std::nullptr_t);  // zeroization

  operator const_byte_ptr() const { return const_byte_ptr(this); }
  operator byte_ptr() { return byte_ptr(this); }
  operator mem_t() const { return mem_t(byte_ptr(this), sizeof(buf256_t)); }

  static buf256_t zero() { return make(ZERO128, ZERO128); }
  static buf256_t make(buf128_t lo, buf128_t hi = ZERO128);
  static buf256_t load(const_byte_ptr src);
  static buf256_t load(mem_t src);
  void save(byte_ptr dst) const;

  bool get_bit(int index) const;
  void set_bit(int index, bool value);

  bool operator==(std::nullptr_t) const;
  bool operator!=(std::nullptr_t) const;
  bool operator==(const buf256_t& src) const;
  bool operator!=(const buf256_t& src) const;
  buf256_t operator~() const;
  buf256_t operator^(const buf256_t& src) const;
  buf256_t operator|(const buf256_t& src) const;
  buf256_t operator&(const buf256_t& src) const;
  buf256_t operator&(bool src) const;
  buf256_t& operator^=(const buf256_t& src);
  buf256_t& operator|=(const buf256_t& src);
  buf256_t& operator&=(const buf256_t& src);
  buf256_t& operator&=(bool src);

  buf256_t operator<<(unsigned n) const;
  buf256_t& operator<<=(unsigned n) { return *this = *this << n; }
  buf256_t operator>>(unsigned n) const;
  buf256_t& operator>>=(unsigned n) { return *this = *this >> n; }

  void be_inc();
  static buf256_t caryless_mul(buf128_t a, buf128_t b);
  static buf128_t binary_galois_field_reduce(buf256_t x);

  buf256_t reverse_bytes() const;

  void convert(coinbase::converter_t& converter);
};

std::ostream& operator<<(std::ostream& os, buf256_t b);

}  // namespace coinbase