#include "buf.h"

#include <cbmpc/core/convert.h>
#include <cbmpc/core/strext.h>

namespace coinbase {

static byte_ptr cgo_malloc(int size) { return (uint8_t*)::malloc(size); }  // NOLINT:cppcoreguidelines-no-malloc
void cgo_free(void* ptr) { ::free(ptr); }                                  // NOLINT:cppcoreguidelines-no-malloc

buf_t::buf_t() noexcept(true) : s(0) { static_assert(sizeof(buf_t) == 40, "Invalid buf_t size."); }

buf_t::buf_t(int new_size) : s(new_size) {  // NOLINT(*init*)
  if (new_size > short_size) set_long_ptr(new byte_t[new_size]);
}

buf_t::buf_t(const_byte_ptr src, int src_size) {  // NOLINT(*init*)
  if (src_size <= short_size)
    assign_short(src, src_size);
  else
    assign_long(src, src_size);
}

buf_t::buf_t(mem_t mem) {  // NOLINT(*init*)
  if (mem.size <= short_size)
    assign_short(mem.data, mem.size);
  else
    assign_long(mem.data, mem.size);
}

buf_t::operator buf128_t() const {
  cb_assert(s == sizeof(buf128_t));
  return buf128_t::load(m);
}

buf_t& buf_t::operator=(buf128_t src) {
  if (s > short_size) {
    byte_ptr old_long_ptr = get_long_ptr();
    coinbase::secure_bzero(old_long_ptr, s);
    delete[] old_long_ptr;
  }

  s = sizeof(buf128_t);
  src.save(m);
  return *this;
}

buf_t::buf_t(buf128_t src) : s(sizeof(buf128_t)) {  // NOLINT(*init*)
  src.save(m);
}

buf_t::operator buf256_t() const {
  cb_assert(s == sizeof(buf256_t));
  return buf256_t::load(m);
}

buf_t& buf_t::operator=(buf256_t src) {
  if (s > short_size) {
    byte_ptr old_long_ptr = get_long_ptr();
    coinbase::secure_bzero(old_long_ptr, s);
    delete[] old_long_ptr;
  }

  s = sizeof(buf256_t);
  src.save(m);
  return *this;
}

buf_t::buf_t(buf256_t src) : s(sizeof(buf256_t)) {  // NOLINT(*init*)
  src.save(m);
}

void buf_t::free() {
  int size = s;
  byte_ptr ptr = data();
  coinbase::secure_bzero(ptr, s);
  if (size > short_size) delete[] ptr;
  s = 0;
}

buf_t::~buf_t() { free(); }

byte_ptr buf_t::data() const { return (s <= short_size) ? byte_ptr(m) : get_long_ptr(); }

byte_ptr buf_t::ptr() const { return s ? data() : nullptr; }

int buf_t::size() const { return s; }

bool buf_t::empty() const { return s == 0; }

buf_t& buf_t::operator=(mem_t src) {
  if (s != src.size || data() != src.data) {
    free();

    if (src.size <= short_size)
      assign_short(src.data, src.size);
    else
      assign_long(src.data, src.size);
  }
  return *this;
}

buf_t::buf_t(const buf_t& src) {
  if (src.s <= short_size)
    assign_short(src);
  else
    assign_long(src.data(), src.s);
}

buf_t::buf_t(buf_t&& src) {
  if (src.s <= short_size)
    assign_short(src);
  else {
    assign_long_ptr(src.get_long_ptr(), src.s);
    src.s = 0;
  }
}

buf_t& buf_t::operator=(const buf_t& src) {
  if (this != &src) {
    free();

    if (src.s <= short_size)
      assign_short(src);
    else
      assign_long(src.data(), src.s);
  }

  return *this;
}

buf_t& buf_t::operator=(buf_t&& src) {
  if (&src != this) {
    free();

    if (src.s <= short_size)
      assign_short(src);
    else {
      assign_long_ptr(src.get_long_ptr(), src.s);
      src.s = 0;
    }
  }
  return *this;
}

byte_ptr buf_t::resize_save_short_to_short(int new_size) {
  if (new_size < s) coinbase::secure_bzero(m + new_size, s - new_size);
  s = new_size;
  return m;
}

byte_ptr buf_t::resize_save_short_to_long(int new_size) {
  byte_ptr new_ptr = new byte_t[new_size];
  memmove(new_ptr, m, s);
  coinbase::secure_bzero(m, s);
  assign_long_ptr(new_ptr, new_size);
  return new_ptr;
}

byte_ptr buf_t::resize_save_long_to_short(int new_size) {
  byte_ptr old_ptr = get_long_ptr();
  memmove(m, old_ptr, new_size);
  coinbase::secure_bzero(old_ptr, s);
  delete[] old_ptr;
  s = new_size;
  return m;
}

byte_ptr buf_t::resize_save_long_to_long(int new_size) {
  byte_ptr old_ptr = get_long_ptr();
  byte_ptr new_ptr = new byte_t[new_size];

  int copy_size = s < new_size ? s : new_size;
  memmove(new_ptr, old_ptr, copy_size);

  coinbase::secure_bzero(old_ptr, s);
  delete[] old_ptr;

  assign_long_ptr(new_ptr, new_size);
  return new_ptr;
}

byte_ptr buf_t::resize(int new_size) {
  if (s == new_size) return data();

  if (s <= short_size) {
    if (new_size <= short_size) return resize_save_short_to_short(new_size);
    return resize_save_short_to_long(new_size);
  }

  if (new_size <= short_size) return resize_save_long_to_short(new_size);
  return resize_save_long_to_long(new_size);
}

byte_ptr buf_t::alloc(int new_size) {
  if (s == new_size) return data();

  free();
  s = new_size;
  if (new_size <= short_size) return m;
  byte_ptr new_ptr = new byte_t[new_size];
  set_long_ptr(new_ptr);
  return new_ptr;
}

void buf_t::bzero() { coinbase::bzero(data(), s); }

void buf_t::secure_bzero() { coinbase::secure_bzero(data(), s); }

bool buf_t::operator==(const buf_t& src) const {
  // This comparison is NOT constant-time. Do NOT use for private values.
  return s == src.s && 0 == memcmp(data(), src.data(), s);
}

bool buf_t::operator!=(const buf_t& src) const { return s != src.s || 0 != memcmp(data(), src.data(), s); }

buf_t::operator mem_t() const { return mem_t(data(), s); }

uint8_t buf_t::operator[](int index) const { return data()[index]; }
uint8_t& buf_t::operator[](int index) { return data()[index]; }

buf_t operator^(mem_t src1, mem_t src2) {
  cb_assert(src1.size == src2.size);
  buf_t out(src1.size);

  byte_ptr dst = out.data();
  for (int i = 0; i < src2.size; i++) dst[i] = src1.data[i] ^ src2.data[i];

  return out;
}

buf_t& buf_t::operator^=(mem_t src2) {
  cb_assert(src2.size == s);

  byte_ptr dst = data();
  for (int i = 0; i < src2.size; i++) dst[i] ^= src2.data[i];
  return *this;
}

buf_t operator+(mem_t src1, mem_t src2) {
  buf_t out(src1.size + src2.size);
  memmove(out.data(), src1.data, src1.size);
  memmove(out.data() + src1.size, src2.data, src2.size);
  return out;
}

buf_t& buf_t::operator+=(mem_t src) {
  int old_size = s;
  byte_ptr new_ptr = resize(old_size + src.size);
  memmove(new_ptr + old_size, src.data, src.size);
  return *this;
}

void buf_t::reverse() { mem_t(*this).reverse(); }

std::string buf_t::to_string() const { return std::string(const_char_ptr(data()), s); }

byte_ptr buf_t::get_long_ptr() const { return ((byte_ptr*)m)[0]; }

void buf_t::set_long_ptr(byte_ptr ptr) { ((byte_ptr*)m)[0] = ptr; }

void buf_t::assign_short(const_byte_ptr src, int src_size) {
  for (int i = 0; i < src_size; i++) m[i] = src[i];
  s = src_size;
}

void buf_t::assign_short(const buf_t& src) {
  for (int i = 0; i < 5; i++) ((uint64_t*)m)[i] = ((uint64_t*)src.m)[i];
}

void buf_t::assign_long_ptr(byte_ptr ptr, int size) {
  set_long_ptr(ptr);
  s = size;
}

void buf_t::assign_long(const_byte_ptr ptr, int size) {
  byte_ptr new_ptr = new byte_t[size];
  memmove(new_ptr, ptr, size);
  assign_long_ptr(new_ptr, size);
}

void buf_t::convert(converter_t& converter) {
  uint32_t value_size = size();
  converter.convert_len(value_size);

  if (converter.is_write()) {
    if (!converter.is_calc_size()) memmove(converter.current(), data(), value_size);
  } else {
    if (int(value_size) < 0) {
      converter.set_error();
      return;
    }  // deserialization length validation

    if (converter.is_error() || !converter.at_least(value_size)) {
      converter.set_error();
      return;
    }
    memmove(alloc(value_size), converter.current(), value_size);
  }
  converter.forward(value_size);
}

int buf_t::get_convert_size(int data_size) {  // static
  coinbase::converter_t converter(true);
  uint32_t s = data_size;
  converter.convert_len(s);
  return converter.get_size() + data_size;
}

void buf_t::convert_fixed_size(coinbase::converter_t& converter, int fixed_size) {
  if (converter.is_write()) {
    if (!converter.is_calc_size()) {
      cb_assert(size() == fixed_size);
      memmove(converter.current(), data(), fixed_size);
    }
  } else {
    if (converter.is_error() || !converter.at_least(fixed_size)) {
      converter.set_error();
      return;
    }
    memmove(alloc(fixed_size), converter.current(), fixed_size);
  }
  converter.forward(fixed_size);
}

void buf_t::convert_last(converter_t& converter) {
  if (converter.is_write()) {
    if (!converter.is_calc_size()) memmove(converter.current(), data(), size());
  } else {
    if (converter.is_error()) return;
    int s = converter.get_size() - converter.get_offset();
    memmove(alloc(s), converter.current(), s);
  }
  converter.forward(size());
}

buf_t buf_t::from_cmem(cmem_t cmem) {
  buf_t buf(cmem.data, cmem.size);
  cgo_free(cmem.data);
  return buf;
}

// ----------------------- mem_t ------------------

cmem_t mem_t::to_cmem() const {
  cmem_t out{nullptr, size};
  if (size) {
    out.data = cgo_malloc(size);
    memmove(out.data, data, size);
  }
  return out;
}

void memmove_reverse(byte_ptr dst, const_byte_ptr src, int size) {
  dst += size;
  while (size--) *--dst = *src++;
}

void mem_t::reverse() {
  int l = 0;
  int r = size - 1;
  while (l < r) {
    uint8_t t = data[l];
    data[l] = data[r];
    data[r] = t;
    l++;
    r--;
  }
}

buf_t mem_t::rev() const {
  buf_t out(size);
  memmove_reverse(out.data(), data, size);
  return out;
}

bool mem_t::equal(mem_t m1, mem_t m2) { return m1.size == m2.size && 0 == memcmp(m1.data, m2.data, m1.size); }

bool mem_t::operator==(const mem_t& m2) const { return mem_t::equal(*this, m2); }
bool mem_t::operator!=(const mem_t& m2) const { return !mem_t::equal(*this, m2); }

bool mem_t::operator==(const buf_t& m2) const { return mem_t::equal(*this, mem_t(m2)); }
bool mem_t::operator!=(const buf_t& m2) const { return !mem_t::equal(*this, mem_t(m2)); }

size_t mem_t::non_crypto_hash() const {
  const_byte_ptr p = data;
  int n = size;
  uint32_t x = 1;

  while (n >= 4) {
    x ^= *(const uint32_t*)p;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    n -= 4;
    p += 4;
  }

  while (n > 0) {
    x ^= *p;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    n--;
    p++;
  }

  return x;
}

std::string mem_t::to_string() const { return std::string(const_char_ptr(data), size); }

// ------------------------- bits_t ---------------------

bool bits_t::get(const_byte_ptr data, int index) {
  int offset = index >> 3;
  int n = index & 7;
  return ((data[offset] >> n) & 1) != 0;
}

void bits_t::set(byte_ptr data, int index, bool bit) {
  if (bit)
    set_true(data, index);
  else
    set_false(data, index);
}

void bits_t::set_true(byte_ptr data, int index) {
  int offset = index >> 3;
  int n = index & 7;
  uint8_t mask = 1 << n;
  data[offset] |= mask;
}

void bits_t::set_false(byte_ptr data, int index) {
  int offset = index >> 3;
  int n = index & 7;
  uint8_t mask = 1 << n;
  data[offset] &= ~mask;
}

bits_t::bits_t() : data(nullptr), bits(0) {}

bits_t::bits_t(bits_t&& src) {  // move constructor
  data = src.data;
  bits = src.bits;
  src.data = nullptr;
  src.bits = 0;
}

bits_t& bits_t::operator=(bits_t&& src) {  // move assignment
  if (&src != this) {
    free();
    data = src.data;
    bits = src.bits;
    src.data = nullptr;
    src.bits = 0;
  }
  return *this;
}

void bits_t::free() {
  if (bits) {
    int n = bits_to_limbs(bits);
    secure_bzero((byte_ptr)data, n * int(sizeof(limb_t)));
    delete[] data;
  }
  data = nullptr;
  bits = 0;
}

void bits_t::copy_from(const bits_t& src) {
  if (&src == this) return;

  alloc(src.bits);

  int n = bits_to_limbs(bits);
  memmove(data, src.data, n * sizeof(limb_t));
}

bits_t::bits_t(const bits_t& src) : data(nullptr), bits(0) {  // copy constructor
  copy_from(src);
}

bits_t::bits_t(int count) : data(nullptr), bits(0) {
  if (!count) return;
  bits = count;
  int n = bits_to_limbs(bits);
  data = new limb_t[n];
  memset(data, 0, n * sizeof(limb_t));
}

bits_t& bits_t::operator=(const bits_t& src) {  // copy assignment
  if (&src != this) copy_from(src);
  return *this;
}

void bits_t::bzero() { coinbase::bzero(byte_ptr(data), bits_to_limbs(bits) * int(sizeof(limb_t))); }

void bits_t::convert(converter_t& converter) {
  uint32_t count = bits;
  converter.convert_len(count);

  int size = coinbase::bits_to_bytes(count);

  if (converter.is_write()) {
    if (!converter.is_calc_size()) {
      bzero_unused();
      memmove(converter.current(), data, size);
    }
  } else {
    alloc(count);
    memmove(data, converter.current(), size);
  }
  converter.forward(size);
}

void bits_t::bzero_unused() const {
  int n = bits_to_limbs(bits);
  int unused_bits = n * bits_in_limb - bits;
  if (unused_bits > 0) data[n - 1] &= (limb_t(-1) >> unused_bits);
}

mem_t bits_t::to_bin() const {
  bzero_unused();
  return mem_t(const_byte_ptr(data), bits_to_bytes(bits));
}

bits_t bits_t::from_bin(mem_t src) {
  bits_t dst(bytes_to_bits(src.size));
  if (src.size) memmove(dst.data, src.data, src.size);  // NOLINT
  return dst;
}

void bits_t::resize(int count) {
  int n_old = bits_to_limbs(bits);
  int n_new = bits_to_limbs(count);
  if (n_old == n_new) {
    bits = count;
    return;
  }

  if (count == 0) {
    free();
    return;
  }

  limb_t* old_data = data;
  data = new limb_t[n_new];
  bits = count;

  int n_copy = std::min(n_old, n_new);
  if (n_copy) memmove(data, old_data, n_copy * sizeof(limb_t));

  if (n_old) {
    secure_bzero((byte_ptr)old_data, n_old * int(sizeof(limb_t)));
    delete[] old_data;
  }
}

void bits_t::alloc(int count) {
  int n_old = bits_to_limbs(bits);
  int n_new = bits_to_limbs(count);
  if (n_old == n_new) {
    bits = count;
    return;
  }

  free();
  if (count > 0) {
    bits = count;
    data = new limb_t[n_new];
  }
}

bits_t::ref_t::ref_t(limb_t* ptr, int index) : data(ptr + index / bits_in_limb), offset(index & (bits_in_limb - 1)) {}

bool bits_t::get(int index) const { return ref_t(data, index).get(); }

void bits_t::set(int index, bool value) { ref_t(data, index).set(value); }

void bits_t::append(bool value) {
  resize(bits + 1);
  set(bits - 1, value);
}

void bits_t::ref_t::set(bool value) {
  limb_t mask_value = limb_t(value ? 1 : 0) << offset;
  limb_t mask = limb_t(1) << offset;
  *data = (*data & ~mask) | mask_value;
}

bool bits_t::ref_t::get() const { return 0 != ((*data >> offset) & 1); }

bits_t::ref_t bits_t::operator[](int index) { return ref_t(data, index); }

bool bits_t::equ(const bits_t& src1, const bits_t& src2) {
  if (src1.bits != src2.bits) return false;

  int n = src1.bits / 64;
  if (n > 0) {
    if (0 != memcmp(src1.data, src2.data, n * sizeof(uint64_t))) return false;
  }

  for (int i = n * 64; i < src1.bits; i++) {
    if (src1[i] != src2[i]) return false;
  }

  return true;
}

bits_t& bits_t::operator^=(const bits_t& src) {
  cb_assert(src.bits == bits);
  int n = bits_to_limbs(bits);
  for (int i = 0; i < n; i++) data[i] ^= src.data[i];
  return *this;
}

bits_t operator^(const bits_t& src1, const bits_t& src2) {
  cb_assert(src1.bits == src2.bits);

  bits_t out;
  out.alloc(src1.bits);

  int n = bits_t::bits_to_limbs(src1.bits);
  for (int i = 0; i < n; i++) out.data[i] = src1.data[i] ^ src2.data[i];  // NOLINT

  return out;
}

bits_t& bits_t::operator+=(const bits_t& src2) {
  int n1 = count();
  int n2 = src2.count();

  mem_t src1_mem = to_bin();
  mem_t src2_mem = src2.to_bin();

  resize(n1 + n2);
  if ((n1 % 8) == 0)
    memmove(byte_ptr(data) + src1_mem.size, src2_mem.data, src2_mem.size);
  else {
    for (int i = 0; i < n2; i++) (*this)[n1 + i] = src2[i];
  }
  return *this;
}

bits_t bits_t::operator+(const bits_t& src2) const {
  int n1 = count();
  int n2 = src2.count();
  bits_t dst(n1 + n2);

  mem_t dst_mem = dst.to_bin();
  mem_t src1_mem = to_bin();
  mem_t src2_mem = src2.to_bin();

  memmove(dst.data, src1_mem.data, src1_mem.size);
  if ((n1 % 8) == 0)
    memmove(byte_ptr(dst.data) + src1_mem.size, src2_mem.data, src2_mem.size);
  else {
    for (int i = 0; i < n2; i++) dst[n1 + i] = src2[i];
  }
  return dst;
}

std::vector<mem_t> buf_t::to_mems(const std::vector<buf_t>& in) {
  std::vector<mem_t> out(in.size());
  for (int i = 0; i < int(in.size()); i++) out[i] = in[i];
  return out;
}

std::vector<mem_t> buf_t::to_mems(const std::vector<std::string>& in) {
  std::vector<mem_t> out(in.size());
  for (int i = 0; i < int(in.size()); i++) out[i] = mem_t::from_string(in[i]);
  return out;
}

std::vector<buf_t> buf_t::from_mems(const std::vector<mem_t>& in) {
  std::vector<buf_t> out(in.size());
  for (int i = 0; i < int(in.size()); i++) out[i] = in[i];
  return out;
}

mems_t::mems_t(cmems_t cmems) : sizes(cmems.sizes, cmems.sizes + cmems.count) {
  int n = 0;
  for (int i = 0; i < cmems.count; i++) n += cmems.sizes[i];
  buffer = mem_t(cmems.data, n);
}

mems_t::operator cmems_t() const {
  cmems_t out{0, nullptr, nullptr};
  if (!sizes.empty()) {
    out.count = int(sizes.size());
    out.data = buffer.data();
    out.sizes = (int*)sizes.data();
  }
  return out;
}

mems_t mems_t::from_cmems(cmems_t cmems) {  // static
  mems_t out;
  if (cmems.count) {
    out.sizes.assign(cmems.sizes, cmems.sizes + cmems.count);
    int n = 0;
    for (int i = 0; i < cmems.count; i++) n += cmems.sizes[i];
    out.buffer = mem_t(cmems.data, n);

    cgo_free(cmems.data);
    cgo_free(cmems.sizes);
  }
  return out;
}

cmems_t mems_t::to_cmems() const {
  cmems_t out{0, nullptr, nullptr};
  int count = int(sizes.size());
  if (count) {
    out.count = count;
    out.data = cgo_malloc(buffer.size());
    memmove(out.data, buffer.data(), buffer.size());
    out.sizes = (int*)cgo_malloc(count * sizeof(int));
    memmove(out.sizes, sizes.data(), count * sizeof(int));
  }
  return out;
}

std::vector<mem_t> mems_t::mems() const {
  int count = int(sizes.size());
  std::vector<mem_t> out(count);
  int n = 0;
  for (int i = 0; i < count; i++) {
    out[i] = buffer.range(n, sizes[i]);
    n += sizes[i];
  }
  return out;
}

void mems_t::init(const std::vector<mem_t>& mems) {
  int count = int(mems.size());
  int n = 0;
  for (int i = 0; i < count; i++) n += mems[i].size;
  buffer.alloc(n);
  sizes.resize(count);
  n = 0;
  for (int i = 0; i < count; i++) {
    int size = mems[i].size;
    sizes[i] = size;
    memmove(buffer.data() + n, mems[i].data, size);
    n += size;
  }
}

}  // namespace coinbase

std::ostream& operator<<(std::ostream& os, mem_t mem) {
  os << strext::to_hex(mem);
  return os;
}
