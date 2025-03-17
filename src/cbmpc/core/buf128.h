#pragma once
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

  void be_inc();

  buf128_t reverse_bytes() const;

  buf128_t operator<<(unsigned n) const;
  buf128_t& operator<<=(unsigned n) { return *this = *this << n; }
  buf128_t operator>>(unsigned n) const;
  buf128_t& operator>>=(unsigned n) { return *this = *this >> n; }

  byte_t operator[](int index) const { return (byte_ptr(this))[index]; }
  byte_t& operator[](int index) { return (byte_ptr(this))[index]; }

  void convert(coinbase::converter_t& converter);
  static buf128_t galois_field_mult(const buf128_t& a, const buf128_t& b);

 private:
  static buf128_t u128(u128_t val) {
    buf128_t r;
    r.value = val;
    return r;
  }
};

class bufs128_ref_t {
 public:
  buf128_t* data;
  int size;

 public:
  bufs128_ref_t(buf128_t* _data = nullptr, int _size = 0) : data(_data), size(_size) {}
  void convert(converter_t& converter);
  mem_t mem() const { return mem_t(const_byte_ptr(data), int(size * sizeof(buf128_t))); }
  operator mem_t() const { return mem(); }
  void bzero() { coinbase::bzero((byte_ptr)data, size * sizeof(buf128_t)); }
  void secure_bzero() { coinbase::secure_bzero((byte_ptr)data, size * sizeof(buf128_t)); }

  buf128_t operator[](int index) const { return data[index]; }
  buf128_t& operator[](int index) { return data[index]; }

  bufs128_ref_t range(int offset, int size) const { return bufs128_ref_t(data + offset, size); }
  bufs128_ref_t skip(int offset) const { return range(offset, size - offset); }
  bufs128_ref_t take(int size) const { return range(0, size); }
};

class bufs128_t {
 public:
  bufs128_t() : b(nullptr), s(0) {}
  bufs128_t(bufs128_ref_t ref);
  explicit bufs128_t(int size);
  bufs128_t(const bufs128_t& src);
  bufs128_t(bufs128_t&& src);  // move ct'or
  ~bufs128_t() { free(); }

  bufs128_t& operator=(const bufs128_t& src);
  bufs128_t& operator=(bufs128_t&& src);  // move assignment
  bufs128_t& operator=(bufs128_ref_t src);

  void free();
  bool empty() const { return s == 0; }
  int size() const { return s; }
  buf128_t* data() { return b; }
  const buf128_t* data() const { return b; }
  buf128_t* allocate(int size);
  buf128_t* resize(int size);

  const buf128_t& operator[](int index) const { return b[index]; }
  buf128_t& operator[](int index) { return b[index]; }
  mem_t mem() const { return mem_t(const_byte_ptr(data()), int(s * sizeof(buf128_t))); }
  operator mem_t() const { return mem(); }

  operator bufs128_ref_t() const { return bufs128_ref_t(b, s); }

  bufs128_ref_t range(int offset, int size) const { return bufs128_ref_t(b + offset, size); }
  bufs128_ref_t skip(int offset) const { return range(offset, size() - offset); }
  bufs128_ref_t take(int size) const { return range(0, size); }

  bool operator==(const bufs128_t& other) const;
  bool operator!=(const bufs128_t& other) const;

  void convert(converter_t& converter);

 private:
  buf128_t* b;
  int s;
};

}  // namespace coinbase
