#pragma once
#include "base_bn.h"

using uint128_t = unsigned __int128;

namespace coinbase::crypto {

#ifdef __x86_64__
bool support_x64_mulx();
#endif

inline uint64_t addx(uint64_t x, uint64_t y, uint64_t& carry) {
#ifdef __x86_64__
  unsigned long long z;
  carry = _addcarry_u64(uint8_t(carry), x, y, &z);
  return z;
#else
#if __has_builtin(__builtin_addcl)
  return __builtin_addcl(x, y, carry, (unsigned long int*)&carry);
#else
  auto r = uint128_t(x) + y + carry;
  carry = uint64_t(r >> 64);
  return uint64_t(r);
#endif
#endif
}

inline uint64_t subx(uint64_t x, uint64_t y, uint64_t& borrow) {
#ifdef __x86_64__
  unsigned long long z;
  borrow = _subborrow_u64(uint8_t(borrow), x, y, &z);
  return z;
#else
#if __has_builtin(__builtin_subcl)
  return __builtin_subcl(x, y, borrow, (unsigned long int*)&borrow);
#else
  auto r = uint128_t(x) - y - borrow;
  borrow = uint64_t(r >> 64) & 1;
  return uint64_t(r);
#endif
#endif
}

struct alignas(32) uint256_t {
  uint64_t w0, w1, w2, w3;
  static uint256_t from_hex(const std::string& hex) { return from_bn(bn_t::from_hex(hex.c_str())); }
  static uint256_t from_str(const std::string& str) { return from_bn(bn_t::from_string(str.c_str())); }

  void to_bin(byte_ptr bin) const;
  buf_t to_bin() const;
  static uint256_t from_bin(mem_t bin);
  void to_bin_le(byte_ptr bin) const;
  buf_t to_bin_le() const;
  static uint256_t from_bin_le(mem_t bin);
  bn_t to_bn() const;
  static uint256_t from_bn(const bn_t& bn);

  bool is_zero() const;
  bool is_odd() const;
  bool operator==(const uint256_t& b) const;
  bool operator!=(const uint256_t& b) const;
  void cnd_assign(bool flag, const uint256_t& b);
  uint64_t mul_add_regular(const uint256_t& a, uint64_t b);
  uint64_t div_by_two();
  void inv_mod(const uint256_t& x, const uint256_t& m);

  static void mul_noasm(uint64_t r[8], const uint256_t& a, const uint256_t& b);
  static void mul(uint64_t r[8], const uint256_t& a, const uint256_t& b);
  static void sqr_noasm(uint64_t r[8], const uint256_t& a);
  static void sqr(uint64_t r[8], const uint256_t& a);

  static uint256_t get_mont_rr(const uint256_t& mod);
  static uint256_t make(uint64_t w0, uint64_t w1 = 0, uint64_t w2 = 0, uint64_t w3 = 0);

 private:
  uint64_t add_raw(const uint256_t& a, const uint256_t& b);
  uint64_t sub_raw(const uint256_t& a, const uint256_t& b);
  uint64_t cnd_add_raw(bool flag, const uint256_t& a);
  uint64_t cnd_sub_raw(bool flag, const uint256_t& a);
  uint64_t cnd_neg_raw(bool flag);
  static void cnd_swap(bool flag, uint256_t& a, uint256_t& b);
};

struct uint320_t {
  uint64_t w0, w1, w2, w3, w4;
  void from_bn(const bn_t& bn);
  static uint320_t get_barrett_mu(const uint256_t& mod);
};

class alignas(32) bn256_t {
 public:
  bn256_t();
  ~bn256_t();
  bn256_t(const bn256_t& x);
  bn256_t(const bn_t& x);
  bn256_t(uint64_t x);

  bn256_t& operator=(const bn256_t& x);
  bn256_t& operator=(const bn_t& x);
  bn256_t& operator=(uint64_t x);
  operator bn_t() const;

  bool operator==(uint64_t x) const;
  bool operator!=(uint64_t x) const;

  bool operator==(const bn256_t& b) const;
  bool operator!=(const bn256_t& b) const;

  bn256_t operator+(const bn256_t& b) const;
  bn256_t operator-(const bn256_t& b) const;
  bn256_t operator*(const bn256_t& b) const;
  bn256_t operator/(const bn256_t& b) const;

  bn256_t& operator+=(const bn256_t& b);
  bn256_t& operator-=(const bn256_t& b);
  bn256_t& operator*=(const bn256_t& b);
  bn256_t& operator/=(const bn256_t& b);

  bn256_t operator-() const;
  bn256_t inv_mod(const mod_t& q) const;

  void to_bin(byte_ptr m) const;
  buf_t to_bin() const;
  static bn256_t from_bin(mem_t m);
  void convert(coinbase::converter_t& c);
  static bn256_t rand(const mod_t& q);
  int get_bit(int b) const;

  bn256_t to_mont() const;
  bn256_t from_mont() const;
  static bn256_t mont_mul(const bn256_t& a, const bn256_t& b);

  static void mul_add_no_reduce(uint64_t r[8], const bn256_t& a, const bn256_t& b);
  static void add_no_reduce(uint64_t r[8], const bn256_t& a);
  static bn256_t reduce(uint64_t r[8]);
  static bn256_t two_to_pow(int n);

 private:
  uint64_t w0, w1, w2, w3;
  void barrett_reduce(uint64_t x[8], const uint64_t* m, const uint64_t* mu);
  void mont_mul(const bn256_t& a, const bn256_t& b, const uint64_t* m, uint64_t mont_prime);
  void mont_reduce(uint64_t u[8], const uint64_t* m, uint64_t mont_prime);
};

}  // namespace coinbase::crypto

using coinbase::crypto::bn256_t;
