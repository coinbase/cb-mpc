#pragma once
#include <cbmpc/core/error.h>
#include <cbmpc/core/macros.h>

namespace coinbase {

void memmove_reverse(byte_ptr dst, const_byte_ptr src, int size);
inline void bzero(byte_ptr pointer, int size) { memset(pointer, 0, size); }

inline void secure_bzero(byte_ptr pointer, int size) {
  volatile unsigned char* p = pointer;
  while (size--) *p++ = 0;
}

template <size_t size>
void bzero(uint8_t (&buffer)[size]) {
  bzero(buffer, size);
}
template <size_t size>
void bzero(char (&buffer)[size]) {
  bzero(byte_ptr(buffer), size);
}
template <size_t size>
void secure_bzero(uint8_t (&buffer)[size]) {
  secure_bzero(buffer, size);
}
template <size_t size>
void secure_bzero(char (&buffer)[size]) {
  secure_bzero(byte_ptr(buffer), size);
}

class buf_t;
class converter_t;

struct mem_t {
  byte_ptr data;
  int size;
  mem_t() noexcept(true) : data(0), size(0) {}
  mem_t(const_byte_ptr the_data, int the_size) noexcept(true) : data(byte_ptr(the_data)), size(the_size) {}
  mem_t(const std::string& s) noexcept(true) : data(byte_ptr(s.data())), size(int(s.size())) {
    cb_assert(s.size() <= INT_MAX);
  }
  template <size_t N>
  mem_t(const char (&s)[N]) : data(byte_ptr(s)), size(N) {
    if (N > 0 && s[N - 1] == '\0') size--;  // zero-terminated
  }

  void bzero() { coinbase::bzero(data, size); }
  void secure_bzero() { coinbase::secure_bzero(data, size); }
  void reverse();
  buf_t rev() const;

  bool operator==(const mem_t& b2) const;
  bool operator!=(const mem_t& b2) const;
  bool operator==(const buf_t& b2) const;
  bool operator!=(const buf_t& b2) const;

  uint8_t operator[](int index) const {
    cb_assert(index >= 0 && index < size);
    return data[index];
  }
  uint8_t& operator[](int index) {
    cb_assert(index >= 0 && index < size);
    return data[index];
  }

  mem_t range(int offset, int len) const {
    cb_assert(offset >= 0 && offset <= size);
    cb_assert(len >= 0 && len <= size - offset);
    return mem_t(data + offset, len);
  }
  mem_t skip(int offset) const {
    cb_assert(offset >= 0 && offset <= size);
    return mem_t(data + offset, size - offset);
  }
  mem_t take(int len) const {
    cb_assert(len >= 0 && len <= size);
    return mem_t(data, len);
  }
  static mem_t from_string(const std::string& str) { return mem_t(const_byte_ptr(str.c_str()), int(str.length())); }

  size_t non_crypto_hash() const;
  std::string to_string() const;

 private:
  static bool equal(mem_t m1, mem_t m2);
};

}  // namespace coinbase

using coinbase::mem_t;

std::ostream& operator<<(std::ostream& os, mem_t mem);

#include "buf128.h"
#include "buf256.h"

namespace coinbase {

class buf_t {
 public:
  buf_t() noexcept(true);
  explicit buf_t(int new_size);
  buf_t(const_byte_ptr src, int src_size);
  buf_t(mem_t mem);
  buf_t(const buf_t& src);
  buf_t(buf_t&& src);
  buf_t(buf128_t src);
  buf_t(buf256_t src);

  void free();
  ~buf_t();

  byte_ptr data() const;
  byte_ptr ptr() const;
  int size() const;
  bool empty() const;
  byte_ptr resize(int new_size);
  byte_ptr alloc(int new_size);
  void bzero();
  void secure_bzero();
  void reverse();
  buf_t rev() const { return mem_t(*this).rev(); }

  buf_t& operator=(const buf_t& src);
  buf_t& operator=(buf_t&& src);
  buf_t& operator=(mem_t src);
  buf_t& operator=(buf128_t src);
  buf_t& operator=(buf256_t src);
  buf_t& operator+=(mem_t src);

  bool operator==(const buf_t& src) const;
  bool operator!=(const buf_t& src) const;

  /**
   * @notes:
   * - Bounds checking: cb_assert(0 ≤ index < size()) is performed.
   */
  uint8_t operator[](int index) const;
  uint8_t& operator[](int index);

  operator mem_t() const;

  mem_t range(int offset, int len) const {
    cb_assert(offset >= 0 && offset <= size());
    cb_assert(len >= 0 && len <= size() - offset);
    return mem_t(data() + offset, len);
  }
  mem_t skip(int offset) const {
    cb_assert(offset >= 0 && offset <= size());
    return mem_t(data() + offset, size() - offset);
  }
  mem_t take(int len) const {
    cb_assert(len >= 0 && len <= size());
    return mem_t(data(), len);
  }

  explicit operator buf128_t() const;
  explicit operator buf256_t() const;

  buf_t& operator^=(mem_t src2);
  std::string to_string() const;

  void convert(converter_t& converter);
  void convert_fixed_size(converter_t& converter, int fixed_size);
  void convert_last(converter_t& converter);
  static int get_convert_size(int data_size);

  static std::vector<mem_t> to_mems(const std::vector<buf_t>& bufs);
  static std::vector<buf_t> from_mems(const std::vector<mem_t>& mems);
  static std::vector<mem_t> to_mems(const std::vector<std::string>& strings);

 private:
  enum { short_size = 36 };

  /**
   * @notes:
   * - The order of `m` and `s` is important.
   * - The safety of `buf_t::assign_short` relies on this. (See notes in`buf_t::assign_short` for more details.)
   */
  byte_t m[short_size];
  int s = 0;

  byte_ptr get_long_ptr() const;
  void set_long_ptr(byte_ptr ptr);
  void assign_short(const_byte_ptr src, int src_size);
  void assign_short(const buf_t& src);
  void assign_long_ptr(byte_ptr ptr, int size);
  void assign_long(const_byte_ptr ptr, int size);

  byte_ptr resize_save_short_to_short(int new_size);
  byte_ptr resize_save_short_to_long(int new_size);
  byte_ptr resize_save_long_to_short(int new_size);
  byte_ptr resize_save_long_to_long(int new_size);
};

buf_t operator+(mem_t src1, mem_t src2);
buf_t operator^(mem_t src1, mem_t src2);

class bits_t {
 public:
  bits_t();
  explicit bits_t(int count);

  bits_t(const bits_t& src);             // copy constructor
  bits_t(bits_t&& src);                  // move constructor
  bits_t& operator=(const bits_t& src);  // copy assignment
  bits_t& operator=(bits_t&& src);       // move assignment

  ~bits_t() { free(); }

  int count() const { return bits; }
  bool empty() const { return bits == 0; }

  void free();
  void resize(int count);
  void alloc(int count);
  void bzero();

  void convert(converter_t& converter);

  static bool get(const_byte_ptr data, int bit_index);
  static void set(byte_ptr data, int bit_index, bool bit);
  static void set_false(byte_ptr data, int bit_index);
  static void set_true(byte_ptr data, int bit_index);

  bits_t& operator^=(const bits_t& src);
  friend bits_t operator^(const bits_t& src1, const bits_t& src2);
  mem_t to_bin() const;
  operator mem_t() const { return to_bin(); }
  static bits_t from_bin(mem_t src);

  bits_t& operator+=(const bits_t& src2);      // concat
  bits_t operator+(const bits_t& src2) const;  // concat

 private:
  typedef uint64_t limb_t;

  enum { bits_in_limb = sizeof(limb_t) * 8 };

  static int bits_to_limbs(int bits) { return (bits + bits_in_limb - 1) / bits_in_limb; }

  class ref_t {
    friend class bits_t;

   public:
    ref_t& operator=(bool value) noexcept {
      set(value);
      return *this;
    }
    operator bool() const { return get(); }

   private:
    bool get() const;
    void set(bool value);

    ref_t(limb_t* ptr, int index);
    limb_t* data;

    int offset;
  };

 public:
  bool operator[](int index) const {
    cb_assert(index >= 0 && index < bits);
    return get(index);
  }
  ref_t operator[](int index);

  bool get(int index) const;
  void set(int index, bool value);
  void append(bool bit);

  static bool equ(const bits_t& src1, const bits_t& src2);

 private:
  void copy_from(const bits_t& src);
  void bzero_unused() const;

  limb_t* data = nullptr;
  int bits = 0;
};

}  // namespace coinbase

using coinbase::buf_t;
