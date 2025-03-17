#include <cbmpc/core/convert.h>

namespace coinbase {

#if defined(__x86_64__)

u128_t u128_zero() { return _mm_setzero_si128(); }
u128_t u128_load(const void* p) { return _mm_loadu_si128((__m128i*)p); }
void u128_save(void* p, u128_t x) { _mm_storeu_si128((__m128i*)p, x); }
uint64_t u128_lo(u128_t x) { return _mm_cvtsi128_si64(x); }
__attribute__((target("sse4.1"))) uint64_t u128_hi(u128_t x) { return _mm_extract_epi64(x, 1); }
u128_t u128_make(uint64_t lo, uint64_t hi) { return _mm_set_epi64x(hi, lo); }
bool u128_lsb(u128_t x) { return (u128_lo(x) & 1) != 0; }
bool u128_msb(u128_t x) { return short(_mm_movemask_epi8(x)) < 0; }
u128_t u128_mask(bool x) { return _mm_set1_epi64x(-int64_t(x)); }
bool u128_equ(u128_t x, u128_t y) { return _mm_movemask_epi8(_mm_cmpeq_epi8(x, y)) == 0xffff; }
u128_t u128_xor(u128_t x, u128_t y) { return _mm_xor_si128(x, y); }
u128_t u128_and(u128_t x, u128_t y) { return _mm_and_si128(x, y); }
u128_t u128_or(u128_t x, u128_t y) { return _mm_or_si128(x, y); }
u128_t u128_not(u128_t x) { return _mm_xor_si128(x, _mm_set1_epi32(-1)); }
u128_t u128_and(u128_t x, bool y) { return u128_and(x, u128_mask(y)); }

#elif defined(__aarch64__)

u128_t u128_zero() { return u128_make(0, 0); }
u128_t u128_load(const void* p) { return vld1q_u8(const_byte_ptr(p)); }
void u128_save(void* p, u128_t x) { vst1q_u8(byte_ptr(p), x); }
uint64_t u128_lo(u128_t x) { return le_get_8(const_byte_ptr(&x)); }
uint64_t u128_hi(u128_t x) { return le_get_8(const_byte_ptr(&x) + 8); }
u128_t u128_make(uint64_t lo, uint64_t hi) { return vcombine_u8(uint8x8_t(lo), uint8x8_t(hi)); }
bool u128_msb(u128_t x) { return ((const int8_t*)(&x))[15] < 0; }
u128_t u128_mask(bool x) { return u128_make(-int64_t(x), -int64_t(x)); }
bool u128_lsb(u128_t x) { return (*const_byte_ptr(&x) & 1) != 0; }
bool u128_equ(u128_t x, u128_t y) { return 0 == memcmp(&x, &y, 16); }
u128_t u128_xor(u128_t x, u128_t y) { return x ^ y; }
u128_t u128_and(u128_t x, u128_t y) { return x & y; }
u128_t u128_or(u128_t x, u128_t y) { return x | y; }
u128_t u128_not(u128_t x) { return u128_make(~u128_lo(x), ~u128_hi(x)); }
u128_t u128_and(u128_t x, bool y) { return u128_and(x, u128_mask(y)); }

#else

u128_t u128_zero() { return u128_make(0, 0); }
u128_t u128_load(const void* p) { return *(const u128_t*)p; }
void u128_save(void* p, u128_t x) { *(u128_t*)p = x; }
uint64_t u128_lo(u128_t x) { return x.low; }
uint64_t u128_hi(u128_t x) { return x.high; }
u128_t u128_make(uint64_t lo, uint64_t hi) {
  u128_t r;
  r.low = lo;
  r.high = hi;
  return r;
}
bool u128_lsb(u128_t x) { return (u128_lo(x) & 1) != 0; }
bool u128_msb(u128_t x) { return int64_t(u128_hi(x)) < 0; }
u128_t u128_mask(bool x) { return u128_make(-int64_t(x), -int64_t(x)); }
bool u128_equ(u128_t x, u128_t y) { return u128_lo(x) == u128_lo(y) && u128_hi(x) == u128_hi(y); }
u128_t u128_xor(u128_t x, u128_t y) { return u128_make(u128_lo(x) ^ u128_lo(y), u128_hi(x) ^ u128_hi(y)); }
u128_t u128_and(u128_t x, u128_t y) { return u128_make(u128_lo(x) & u128_lo(y), u128_hi(x) & u128_hi(y)); }
u128_t u128_and(u128_t x, bool y) { return u128_make(u128_lo(x) & -int64_t(y), u128_hi(x) & -int64_t(y)); }
u128_t u128_or(u128_t x, u128_t y) { return u128_make(u128_lo(x) | u128_lo(y), u128_hi(x) | u128_hi(y)); }
u128_t u128_not(u128_t x) { return u128_make(~u128_lo(x), ~u128_hi(x)); }

#endif

void u128_convert(coinbase::converter_t& converter, u128_t& x) {
  if (converter.is_write()) {
    if (!converter.is_calc_size()) u128_save(converter.current(), x);
  } else {
    if (converter.is_error() || !converter.at_least(16)) {
      converter.set_error();
      return;
    }
    x = u128_load(converter.current());
  }
  converter.forward(16);
}

bool buf128_t::operator==(std::nullptr_t null_ptr) const { return u128_equ(value, u128_zero()); }
bool buf128_t::operator!=(std::nullptr_t null_ptr) const { return !u128_equ(value, u128_zero()); }

buf128_t& buf128_t::operator=(std::nullptr_t) {  // zeroization
  value = u128_zero();
  return *this;
}

buf128_t& buf128_t::operator=(mem_t src) {
  cb_assert(src.size == sizeof(buf128_t));
  return *this = load(src.data);
}

uint64_t buf128_t::lo() const { return u128_lo(value); }
uint64_t buf128_t::hi() const { return u128_hi(value); }
buf128_t buf128_t::load(const_byte_ptr src) noexcept(true) { return u128(u128_load(src)); }
void buf128_t::save(byte_ptr dst) const { u128_save(dst, value); }
buf128_t buf128_t::make(uint64_t lo, uint64_t hi) { return u128(u128_make(lo, hi)); }
buf128_t buf128_t::mask(bool x) { return u128(u128_mask(x)); }

buf128_t buf128_t::load(mem_t src) {
  cb_assert(src.size == 16);
  return u128(u128_load(src.data));
}

buf128_t buf128_t::from_bit_index(int bit_index) {
  if (bit_index < 64) return make(uint64_t(1) << bit_index, 0);
  return make(0, uint64_t(1) << (bit_index - 64));
}

bool buf128_t::get_bit(int index) const {
  int n = index / 64;
  index %= 64;
  return ((((const uint64_t*)(this))[n] >> index) & 1) != 0;
}

void buf128_t::set_bit(int index, bool bit) {
  uint64_t l = lo();
  uint64_t h = hi();
  if (index < 64)
    l = l & ~(uint64_t(1) << index) | (uint64_t(bit) << index);
  else
    h = h & ~(uint64_t(1) << (index - 64)) | (uint64_t(bit) << (index - 64));
  *this = make(l, h);
}

int buf128_t::get_bits_count() const { return __builtin_popcountll(lo()) + __builtin_popcountll(hi()); }

bool buf128_t::operator==(const buf128_t& src) const { return u128_equ(value, src.value); }
bool buf128_t::operator!=(const buf128_t& src) const { return !u128_equ(value, src.value); }

buf128_t buf128_t::operator~() const { return u128(u128_not(value)); }
buf128_t buf128_t::operator^(const buf128_t& src) const { return u128(u128_xor(value, src.value)); }
buf128_t buf128_t::operator|(const buf128_t& src) const { return u128(u128_or(value, src.value)); }
buf128_t buf128_t::operator&(const buf128_t& src) const { return u128(u128_and(value, src.value)); }

buf128_t& buf128_t::operator^=(const buf128_t& src) { return *this = *this ^ src; }
buf128_t& buf128_t::operator|=(const buf128_t& src) { return *this = *this | src; }
buf128_t& buf128_t::operator&=(const buf128_t& src) { return *this = *this & src; }

void buf128_t::be_inc() {
  byte_ptr p = byte_ptr(this) + 16;
  for (int i = 0; i < 16; i++) {
    byte_t x = *--p;
    *p = ++x;
    if (x) break;
  }
}

void buf128_t::convert(coinbase::converter_t& converter) {
  if (converter.is_write()) {
    if (!converter.is_calc_size()) save(converter.current());
  } else {
    if (converter.is_error() || !converter.at_least(16)) {
      converter.set_error();
      return;
    }
    *this = load(converter.current());
  }
  converter.forward(16);
}

buf128_t buf128_t::galois_field_mult(const buf128_t& a, const buf128_t& b) {
  return buf256_t::binary_galois_field_reduce(buf256_t::caryless_mul(a, b));
}

#if defined(__x86_64__)
static __m128i get_bswap() noexcept(true) { return _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15); }
static const __m128i BSWAP = get_bswap();
#endif

#if defined(__x86_64__)
__attribute__((target("ssse3")))
#endif
buf128_t
buf128_t::reverse_bytes() const {
#if defined(__x86_64__)
  return u128(_mm_shuffle_epi8(value, BSWAP));
#else
  uint8_t t[16];
  save(t);
  std::swap(t[0], t[15]);
  std::swap(t[1], t[14]);
  std::swap(t[2], t[13]);
  std::swap(t[3], t[12]);
  std::swap(t[4], t[11]);
  std::swap(t[5], t[10]);
  std::swap(t[6], t[9]);
  std::swap(t[7], t[8]);
  return load(t);
#endif  //
}

buf128_t buf128_t::operator<<(unsigned n) const {
  uint64_t l = lo();
  uint64_t r = hi();
  if (n == 64) {
    r = l;
    l = 0;
  } else if (n > 64) {
    r = l << (n - 64);
    l = 0;
  } else {
    r <<= n;
    r |= l >> (64 - n);
    l <<= n;
  }
  return make(l, r);
}

buf128_t buf128_t::operator>>(unsigned n) const {
  uint64_t l = lo();
  uint64_t r = hi();
  if (n == 64) {
    l = r;
    r = 0;
  } else if (n > 64) {
    l = r >> (n - 64);
    r = 0;
  } else {
    l >>= n;
    l |= r << (64 - n);
    r >>= n;
  }
  return make(l, r);
}

// -------------------------- coinbase::bufs128_t ---------------------------

static buf128_t* allocate_buffer(int s) { return s ? new buf128_t[s] : nullptr; }

static void copy(buf128_t* ptr, bufs128_ref_t ref) {
  if (ref.size) memmove(ptr, ref.data, ref.size * sizeof(buf128_t));
}

bufs128_t& bufs128_t::operator=(bufs128_ref_t src) {
  if (s != src.size) allocate(src.size);
  copy(b, src);
  return *this;
}

bufs128_t::bufs128_t(int size) : s(size) { b = allocate_buffer(s); }

bufs128_t::bufs128_t(const bufs128_t& src) : s(src.s) {
  b = allocate_buffer(s);
  copy(b, src);
}

bufs128_t::bufs128_t(bufs128_ref_t src) : s(src.size) {
  b = allocate_buffer(s);
  copy(b, src);
}

// move ct'or
bufs128_t::bufs128_t(bufs128_t&& src) : b(src.b), s(src.s) {
  src.b = nullptr;
  src.s = 0;
}

// move assignment
bufs128_t& bufs128_t::operator=(bufs128_t&& src) {
  if (&src != this) {
    free();
    b = src.b;
    s = src.s;
    src.b = nullptr;
    src.s = 0;
  }
  return *this;
}

void bufs128_t::free() {
  delete[] b;
  b = nullptr;
  s = 0;
}

buf128_t* bufs128_t::allocate(int size) {
  if (size != s) {
    free();
    b = allocate_buffer(s = size);
  }
  return b;
}

buf128_t* bufs128_t::resize(int size) {
  if (size != s) {
    buf128_t* old = b;
    int n = std::min(s, size);
    b = allocate_buffer(size);
    copy(b, bufs128_ref_t(old, n));
    delete[] old;
    s = size;
  }
  return b;
}

bool bufs128_t::operator==(const bufs128_t& other) const { return mem_t(*this) == mem_t(other); }

bool bufs128_t::operator!=(const bufs128_t& other) const { return mem_t(*this) != mem_t(other); }

bufs128_t& bufs128_t::operator=(const bufs128_t& src) {
  if (&src != this) {
    allocate(src.s);
    copy(b, src);
  }
  return *this;
}

void bufs128_t::convert(coinbase::converter_t& converter) {
  uint32_t count = size();
  converter.convert_len(count);
  if (!converter.is_write()) {
    converter.set_error();
    return;
  }

  int data_size = count * int(sizeof(buf128_t));
  if (!converter.is_write() && data_size < 0) {
    converter.set_error();
    return;
  }

  if (converter.is_write()) {
    if (!converter.is_calc_size()) memmove(converter.current(), b, data_size);
  } else {
    if (converter.is_error() || !converter.at_least(count * int(sizeof(buf128_t)))) {
      converter.set_error();
      return;
    }
    allocate(count);
    if (data_size) memmove(b, converter.current(), data_size);
  }
  converter.forward(data_size);
}

void bufs128_ref_t::convert(converter_t& converter) {
  cb_assert(converter.is_write());
  converter.convert(size);
  int data_size = size * int(sizeof(buf128_t));
  if (!converter.is_calc_size()) memmove(converter.current(), data, data_size);
  converter.forward(data_size);
}

}  // namespace coinbase
