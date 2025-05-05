#pragma once

struct bignum_st {
  BN_ULONG* d; /* Pointer to an array of 'BN_BITS2' bit
                * chunks. */
  int top;     /* Index of last used d +1. */
  /* The next are internal book keeping for bn_expand. */
  int dmax; /* Size of the d array. */
  int neg;  /* one if the number is negative */
  int flags;
};

namespace coinbase::crypto {

constexpr int div_ceil(int a, int b) { return (a + b - 1) / b; }
static const int BN_FLG_FIXED_TOP = 0x10000;

buf_t bn_to_buf(const BIGNUM* bn);
buf_t bn_to_buf(const BIGNUM* bn, int size);
void bn_to_mem(const BIGNUM* bn, mem_t mem);

class mod_t;
class paillier_t;
class ecdsa_signature_t;
class ecurve_t;
class ecc_point_t;

typedef void (*gen_prime_callback)(int a, int b, void* ctx);

class bn_t {
  friend class ecp_t;
  friend class crypto::ecurve_t;
  friend class crypto::ecc_point_t;
  friend class crypto::paillier_t;
  friend class crypto::ecdsa_signature_t;
  friend class montgomery_t;
  friend std::ostream& operator<<(std::ostream& os, const bn_t& obj);

 public:
  operator const BIGNUM*() const;
  operator BIGNUM*();
  bn_t();
  bn_t(const BIGNUM* src);
  explicit bn_t(mem_t src);
  explicit bn_t(buf128_t src);
  explicit bn_t(buf256_t src);

  ~bn_t();
  bn_t(int src);
  bn_t(const bn_t& src);
  bn_t(bn_t&& src) noexcept(true);  // move constructor
  operator int() const;

  void correct_top() const;

  int64_t get_int64() const;
  void set_int64(int64_t value);

  bn_t& operator=(int src);
  bn_t& operator=(const bn_t& src);
  bn_t& operator=(bn_t&& src);  // move assignment
  bn_t& operator=(const BIGNUM* src);
  bool operator==(const bn_t& val) const;
  bool operator!=(const bn_t& val) const;
  bool operator>(const bn_t& val) const;
  bool operator<(const bn_t& val) const;
  bool operator>=(const bn_t& val) const;
  bool operator<=(const bn_t& val) const;
  bool operator==(int val) const;
  bool operator!=(int val) const;
  bool operator>(int val) const;
  bool operator<(int val) const;
  bool operator>=(int val) const;
  bool operator<=(int val) const;

  bn_t& operator+=(const bn_t& val);
  bn_t& operator-=(const bn_t& val);
  bn_t& operator*=(const bn_t& val);
  bn_t& operator/=(const bn_t& val);
  bn_t& operator%=(const mod_t& val);
  bn_t& operator++();
  const bn_t operator++(int dummy);
  bn_t& operator+=(int val);
  bn_t& operator-=(int val);
  bn_t& operator*=(int val);
  bn_t& operator/=(int val);
  bn_t& operator<<=(int val);
  bn_t& operator>>=(int val);

  static bn_t pow(const bn_t& base, const bn_t& exp);
  static bn_t div(const bn_t& num, const bn_t& denum, bn_t* rem = NULL);
  static bn_t rand(const bn_t& range);
  static bn_t rand_bitlen(int bits, bool top_bit_set = false);

  bn_t neg() const;
  void set_sign(int sign);
  bool is_odd() const;
  bool is_zero() const;
  bn_t pow(const bn_t& exp) const;

  bn_t mul_2_pow(int n) const { return lshift(n); }
  bn_t div_2_pow(int n) const { return rshift(n); }

  bn_t inv() const;  // only valid for in MODULO(p) scopes

  bn_t pow_mod(const bn_t& exp, const mod_t& mod) const;

  bn_t lshift(int n) const;
  bn_t rshift(int n) const;
  bool is_bit_set(int n) const;
  void set_bit(int n, bool bit);

  static bn_t generate_prime(int bits, bool safe, gen_prime_callback callback = nullptr, void* ctx = nullptr);
  bool prime() const;
  static bn_t gcd(const bn_t& val1, const bn_t& val2);

  int get_bit(int n) const;
  int get_bin_size() const;
  int get_bits_count() const;
  buf_t to_bin() const;
  buf_t to_bin(int size) const;
  static buf_t vector_to_bin(const std::vector<bn_t>& vals, int val_size);
  int to_bin(byte_ptr dst) const;
  void to_bin(byte_ptr dst, int size) const;
  void to_bin(mem_t mem) const { to_bin(mem.data, mem.size); }
  static bn_t from_bin(mem_t mem);
  static std::vector<bn_t> vector_from_bin(mem_t mem, int n, int size, const mod_t& q);
  static bn_t from_bin_bitlen(mem_t mem, int bits);

  std::string to_string() const;
  std::string to_hex() const;

  static bn_t from_string(const_char_ptr str);
  static bn_t from_string(const std::string& str) { return from_string(str.c_str()); }
  static bn_t from_hex(const_char_ptr str);

  static int compare(const bn_t& b1, const bn_t& b2);
  int sign() const;

  friend bn_t operator+(const bn_t& val1, const bn_t& val2);
  friend bn_t operator-(const bn_t& val1, const bn_t& val2);
  friend bn_t operator*(const bn_t& val1, const bn_t& val2);
  friend bn_t operator/(const bn_t& val1, const bn_t& val2);
  friend bn_t operator%(const bn_t& val1, const mod_t& val2);
  friend bn_t operator-(const bn_t& val);

  friend bn_t operator+(const bn_t& val1, int val2);
  friend bn_t operator-(const bn_t& val1, int val2);
  friend bn_t operator*(const bn_t& val1, int val2);
  friend bn_t operator/(const bn_t& val1, int val2);

  friend bn_t operator<<(const bn_t& val1, int val2);
  friend bn_t operator>>(const bn_t& val1, int val2);

  static void set_modulo(const bn_t& n) = delete;
  static bool check_modulo(const bn_t& n) = delete;
  static void reset_modulo(const bn_t& n) = delete;

  static void set_modulo(const mod_t& n);
  static bool check_modulo(const mod_t& n);
  static void reset_modulo(const mod_t& n);

  void convert(converter_t& converter);

  // thread local storage for BN_CTX
  static BN_CTX* thread_local_storage_bn_ctx();

  void attach(const uint64_t* data, int size);
  void detach();

 private:
  BIGNUM val;
  void init();
};

#define MODULO(n)                                                                      \
  for (coinbase::crypto::bn_t::set_modulo(n); coinbase::crypto::bn_t::check_modulo(n); \
       coinbase::crypto::bn_t::reset_modulo(n))

bn_t operator+(const bn_t& b1, const bn_t& b2);
bn_t operator-(const bn_t& b1, const bn_t& b2);
bn_t operator*(const bn_t& b1, const bn_t& b2);
bn_t operator/(const bn_t& b1, const bn_t& b2);
bn_t operator%(const bn_t& b1, const mod_t& b2);
bn_t operator-(const bn_t& b1);

bn_t operator+(const bn_t& b1, int b2);
bn_t operator-(const bn_t& b1, int b2);
bn_t operator*(const bn_t& b1, int b2);
bn_t operator/(const bn_t& b1, int b2);

error_t check_closed_range(const bn_t& min, const bn_t& x, const bn_t& max);      // min <= x <= max
error_t check_right_open_range(const bn_t& min, const bn_t& x, const bn_t& max);  // min <= x < max
error_t check_open_range(const bn_t& min, const bn_t& x, const bn_t& max);        // min <  x < max

}  // namespace coinbase::crypto
