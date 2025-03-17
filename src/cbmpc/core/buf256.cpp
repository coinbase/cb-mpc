#include <cbmpc/core/convert.h>

namespace coinbase {

buf256_t& buf256_t::operator=(mem_t src) {
  cb_assert(src.size == sizeof(buf256_t));
  return *this = load(src.data);
}

buf256_t& buf256_t::operator=(const buf_t& src) {
  cb_assert(src.size() == sizeof(buf256_t));
  return *this = load(src.data());
}

buf256_t& buf256_t::operator=(std::nullptr_t) {  // zeroization
  lo = hi = ZERO128;
  return *this;
}

bool buf256_t::operator==(std::nullptr_t null_ptr) const { return *this == ZERO256; }
bool buf256_t::operator!=(std::nullptr_t null_ptr) const { return *this != ZERO256; }

buf256_t buf256_t::make(buf128_t lo, buf128_t hi) {
  buf256_t dst{};
  dst.lo = lo;
  dst.hi = hi;
  return dst;
}

buf256_t buf256_t::load(mem_t src) {
  cb_assert(src.size == 32);
  return load(src.data);
}

buf256_t buf256_t::load(const_byte_ptr src) {  // static
  buf256_t dst{};
  dst.lo = buf128_t::load(src);
  dst.hi = buf128_t::load(src + 16);
  return dst;
}

void buf256_t::save(byte_ptr dst) const {
  lo.save(dst);
  hi.save(dst + 16);
}

bool buf256_t::get_bit(int index) const {
  int n = index / 64;
  index %= 64;
  return ((((const uint64_t*)(this))[n] >> index) & 1) != 0;
}

void buf256_t::set_bit(int index, bool value) {
  int n = index / 64;
  index %= 64;
  uint64_t mask = uint64_t(1) << index;

  if (value)
    ((uint64_t*)(this))[n] |= mask;
  else
    ((uint64_t*)(this))[n] &= ~mask;
}

bool buf256_t::operator==(const buf256_t& src) const { return (src.lo == lo) && (src.hi == hi); }

bool buf256_t::operator!=(const buf256_t& src) const { return (src.lo != lo) || (src.hi != hi); }

buf256_t buf256_t::operator~() const {
  buf256_t dst{};
  dst.lo = ~lo;
  dst.hi = ~hi;
  return dst;
}

buf256_t buf256_t::operator^(const buf256_t& src) const {
  buf256_t dst{};
  dst.lo = lo ^ src.lo;
  dst.hi = hi ^ src.hi;
  return dst;
}

buf256_t buf256_t::operator|(const buf256_t& src) const {
  buf256_t dst{};
  dst.lo = lo | src.lo;
  dst.hi = hi | src.hi;
  return dst;
}

buf256_t buf256_t::operator&(const buf256_t& src) const {
  buf256_t dst{};
  dst.lo = lo & src.lo;
  dst.hi = hi & src.hi;
  return dst;
}

buf256_t buf256_t::operator&(bool src) const {
  buf256_t dst{};
  dst.lo = lo & src;
  dst.hi = hi & src;
  return dst;
}

buf256_t& buf256_t::operator^=(const buf256_t& src) {
  lo ^= src.lo;
  hi ^= src.hi;
  return *this;
}

buf256_t& buf256_t::operator|=(const buf256_t& src) {
  lo |= src.lo;
  hi |= src.hi;
  return *this;
}

buf256_t& buf256_t::operator&=(const buf256_t& src) {
  lo &= src.lo;
  hi &= src.hi;
  return *this;
}

buf256_t& buf256_t::operator&=(bool src) {
  lo &= src;
  hi &= src;
  return *this;
}

void buf256_t::be_inc() {
  byte_ptr p = byte_ptr(this) + 32;
  for (int i = 0; i < 32; i++) {
    byte_t x = *--p;
    *p = ++x;
    if (x) break;
  }
}

buf256_t buf256_t::reverse_bytes() const {
  buf256_t out{};
  byte_ptr dst = byte_ptr(&out);
  const_byte_ptr src = const_byte_ptr(this) + 32;
  for (int i = 0; i < 32; i++) *dst++ = *--src;
  return out;
}

void buf256_t::convert(coinbase::converter_t& converter) {
  if (converter.is_write()) {
    if (!converter.is_calc_size()) save(converter.current());
  } else {
    if (converter.is_error() || !converter.at_least(32)) {
      converter.set_error();
      return;
    }
    *this = load(converter.current());
  }
  converter.forward(32);
}

buf256_t buf256_t::operator<<(unsigned n) const {
  buf128_t l = lo;
  buf128_t r = hi;
  if (n == 128) {
    r = l;
    l = ZERO128;
  } else if (n > 128) {
    r = l << (n - 128);
    l = ZERO128;
  } else {
    r <<= n;
    r |= l >> (128 - n);
    l <<= n;
  }
  return make(l, r);
}

buf256_t buf256_t::operator>>(unsigned n) const {
  buf128_t l = lo;
  buf128_t r = hi;
  if (n == 64) {
    l = r;
    r = ZERO128;
  } else if (n > 128) {
    l = r >> (n - 128);
    r = ZERO128;
  } else {
    l >>= n;
    l |= r << (128 - n);
    r >>= n;
  }
  return make(l, r);
}

std::ostream& operator<<(std::ostream& os, buf256_t b) {
  os << b.hi.hi() << " " << b.hi.lo() << " " << b.lo.hi() << " " << b.lo.lo();
  return os;
}

#if defined(__x86_64__)
__attribute__((target("pclmul")))
#endif
buf256_t
buf256_t::caryless_mul(buf128_t a, buf128_t b) {

#if defined(__x86_64__) || defined(__aarch64__)
#if defined(__x86_64__)
  buf128_t c{};
  c.value = _mm_clmulepi64_si128(a.value, b.value, 0x00);
  buf128_t e{};
  e.value = _mm_clmulepi64_si128(a.value, b.value, 0x10);
  buf128_t f{};
  f.value = _mm_clmulepi64_si128(a.value, b.value, 0x01);
  buf128_t d{};
  d.value = _mm_clmulepi64_si128(a.value, b.value, 0x11);
#else
  buf128_t c{};
  c.value = (u128_t)vmull_p64(vgetq_lane_u64(uint64x2_t(a.value), 0), vgetq_lane_u64(uint64x2_t(b.value), 0));
  buf128_t e{};
  e.value = (u128_t)vmull_p64(vgetq_lane_u64(uint64x2_t(a.value), 0), vgetq_lane_u64(uint64x2_t(b.value), 1));
  buf128_t f{};
  f.value = (u128_t)vmull_p64(vgetq_lane_u64(uint64x2_t(a.value), 1), vgetq_lane_u64(uint64x2_t(b.value), 0));
  buf128_t d{};
  d.value = (u128_t)vmull_p64(vgetq_lane_u64(uint64x2_t(a.value), 1), vgetq_lane_u64(uint64x2_t(b.value), 1));
#endif

  uint64_t c0 = c.lo();
  uint64_t c1 = c.hi();
  uint64_t d0 = d.lo();
  uint64_t d1 = d.hi();
  uint64_t e0 = e.lo();
  uint64_t e1 = e.hi();
  uint64_t f0 = f.lo();
  uint64_t f1 = f.hi();

  uint64_t r0 = c0;
  uint64_t r1 = f0 ^ e0 ^ c1;
  uint64_t r2 = d0 ^ e1 ^ f1;
  uint64_t r3 = d1;

  return buf256_t::make(buf128_t::make(r0, r1), buf128_t::make(r2, r3));

#else
  buf256_t r = ZERO256;
  buf256_t m = buf256_t::make(a, ZERO128);

  for (int i = 0; i < 128; i++) {
    if (b.get_bit(i)) r ^= m;
    m <<= 1;
  }

  return r;
#endif
}

buf128_t buf256_t::binary_galois_field_reduce(buf256_t x) {
  buf128_t res{};

#ifdef __x86_64__
  __m128i tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;
  tmp3 = x.lo.value;
  tmp6 = x.hi.value;

  tmp7 = _mm_srli_epi32(tmp3, 31);
  tmp8 = _mm_srli_epi32(tmp6, 31);
  tmp3 = _mm_slli_epi32(tmp3, 1);
  tmp6 = _mm_slli_epi32(tmp6, 1);
  tmp9 = _mm_srli_si128(tmp7, 12);
  tmp8 = _mm_slli_si128(tmp8, 4);
  tmp7 = _mm_slli_si128(tmp7, 4);
  tmp3 = _mm_or_si128(tmp3, tmp7);
  tmp6 = _mm_or_si128(tmp6, tmp8);
  tmp6 = _mm_or_si128(tmp6, tmp9);
  tmp7 = _mm_slli_epi32(tmp3, 31);
  tmp8 = _mm_slli_epi32(tmp3, 30);
  tmp9 = _mm_slli_epi32(tmp3, 25);
  tmp7 = _mm_xor_si128(tmp7, tmp8);
  tmp7 = _mm_xor_si128(tmp7, tmp9);
  tmp8 = _mm_srli_si128(tmp7, 4);
  tmp7 = _mm_slli_si128(tmp7, 12);
  tmp3 = _mm_xor_si128(tmp3, tmp7);
  tmp2 = _mm_srli_epi32(tmp3, 1);
  tmp4 = _mm_srli_epi32(tmp3, 2);
  tmp5 = _mm_srli_epi32(tmp3, 7);
  tmp2 = _mm_xor_si128(tmp2, tmp4);
  tmp2 = _mm_xor_si128(tmp2, tmp5);
  tmp2 = _mm_xor_si128(tmp2, tmp8);
  tmp3 = _mm_xor_si128(tmp3, tmp2);
  tmp6 = _mm_xor_si128(tmp6, tmp3);
  res.value = tmp6;

#else

  x <<= 1;

  uint64_t x0 = x.lo.lo();
  uint64_t x1 = x.lo.hi();
  uint64_t x2 = x.hi.lo();
  uint64_t x3 = x.hi.hi();

  uint64_t a = x0 << 63;
  uint64_t b = x0 << 62;
  uint64_t c = x0 << 57;
  uint64_t d = x1 ^ a ^ b ^ c;
  buf128_t temp = buf128_t::make(x0, d);
  temp >>= 1;
  uint64_t e0 = temp.lo();
  uint64_t e1 = temp.hi();
  temp >>= 1;
  uint64_t f0 = temp.lo();
  uint64_t f1 = temp.hi();
  temp >>= 5;
  uint64_t g0 = temp.lo();
  uint64_t g1 = temp.hi();

  uint64_t h0 = x0 ^ e0 ^ f0 ^ g0;
  uint64_t h1 = d ^ e1 ^ f1 ^ g1;
  res = buf128_t::make(x2 ^ h0, x3 ^ h1);
#endif
  return res;
}
}  // namespace coinbase