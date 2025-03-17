#include <cbmpc/crypto/base.h>

extern "C" void bn_correct_top(BIGNUM* a);

namespace coinbase::crypto {

static thread_local BN_CTX* g_tls_bn_ctx = nullptr;

static thread_local const mod_t* g_thread_local_storage_modo = nullptr;
static const mod_t* thread_local_storage_mod() { return g_thread_local_storage_modo; }
/**
 * @notes:
 * - Although static code analysis marks this is dangerous, it is safe the way we use it:
 *   - We use it in the MODULE macros such that in `MODULE(q) { operation }`, all operations
 *     are done modulo q. In this case, the mod is set once and the pointer is valid until
 *     we exit the scope of the MODULE.
 */
static void thread_local_storage_set_mod(const mod_t* ptr) { g_thread_local_storage_modo = ptr; }

static void thread_local_storage_bn_ctx_free(void* dummy) {
  if (g_tls_bn_ctx) {
    BN_CTX_free(g_tls_bn_ctx);
    g_tls_bn_ctx = nullptr;
  }
}

BN_CTX* bn_t::thread_local_storage_bn_ctx() {  // static
  BN_CTX* ctx = g_tls_bn_ctx;
  if (!ctx) {
    g_tls_bn_ctx = ctx = BN_CTX_new();
  }
  return ctx;
}

buf_t bn_to_buf(const BIGNUM* bn, int size) {
  int n = BN_num_bytes(bn);
  if (n > size) return bn_to_buf(bn);
  buf_t result(size);
  memset(result.data(), 0, size);
  BN_bn2bin(bn, result.data() + size - n);
  return result;
}

buf_t bn_to_buf(const BIGNUM* bn) {
  int size = BN_num_bytes(bn);
  buf_t result(size);
  BN_bn2bin(bn, result.data());
  return result;
}

void bn_to_mem(const BIGNUM* bn, mem_t mem) {
  int size = BN_num_bytes(bn);
  cb_assert(size <= mem.size);
  memset(mem.data, 0, mem.size);
  BN_bn2bin(bn, mem.data + mem.size - size);
}

void bn_t::init() {
  val.d = nullptr;
  val.top = val.dmax = val.neg = val.flags = 0;
}

bn_t::bn_t(const BIGNUM* src) {
  init();
  if (src) BN_copy(*this, src);
}

bn_t::bn_t() { init(); }

bn_t::bn_t(mem_t src) {
  init();
  auto res = BN_bin2bn(src.data, src.size, *this);
  if (!res) throw std::bad_alloc();
}

bn_t::bn_t(buf128_t src) {
  init();
  auto res = BN_bin2bn(const_byte_ptr(&src), sizeof(src), *this);
  if (!res) throw std::bad_alloc();
}

bn_t::bn_t(buf256_t src) {
  init();
  auto res = BN_bin2bn(const_byte_ptr(&src), sizeof(src), *this);
  if (!res) throw std::bad_alloc();
}

void bn_t::attach(const uint64_t* data, int size) {
  val.d = (BN_ULONG*)data;
  val.top = val.dmax = size;
  val.flags = BN_FLG_CONSTTIME | BN_FLG_STATIC_DATA | BN_FLG_FIXED_TOP;
}

void bn_t::detach() { init(); }

bn_t::~bn_t() { BN_clear_free(*this); }

bn_t::bn_t(int src) {
  init();
  set_int64(src);
}

bn_t::bn_t(const bn_t& src) {
  init();
  BN_copy(*this, src);
}

bn_t::bn_t(bn_t&& src) noexcept(true) {  // move constructor
  val = src.val;
  src.init();
}

bn_t& bn_t::operator=(const bn_t& src) {
  if (this != &src) BN_copy(*this, src);
  return *this;
}

bn_t& bn_t::operator=(const BIGNUM* src) {
  BN_copy(*this, src);
  return *this;
}

bn_t& bn_t::operator=(bn_t&& src) {  // move assignment
  if (this != &src) {
    BN_clear_free(*this);
    val = src.val;
    src.init();
  }
  return *this;
}

bn_t::operator const BIGNUM*() const { return &val; }
bn_t::operator BIGNUM*() { return &val; }

void bn_t::correct_top() const { bn_correct_top((BIGNUM*)&val); }

int64_t bn_t::get_int64() const {
  int64_t result = (int64_t)BN_get_word(*this);
  if (BN_is_negative(*this)) result = -result;
  return result;
}

void bn_t::set_int64(int64_t src) {
  bool neg = src < 0;
  if (neg) src = -src;
  int res = BN_set_word(*this, (BN_ULONG)src);
  cb_assert(res);
  if (neg) BN_set_negative(*this, 1);
}

bn_t::operator int() const { return (int)get_int64(); }

bn_t& bn_t::operator=(int src) {
  set_int64(src);
  return *this;
}

bool bn_t::operator==(const bn_t& src2) const { return compare(*this, src2) == 0; }
bool bn_t::operator!=(const bn_t& src2) const { return compare(*this, src2) != 0; }
bool bn_t::operator>(const bn_t& src2) const { return compare(*this, src2) > 0; }
bool bn_t::operator<(const bn_t& src2) const { return compare(*this, src2) < 0; }
bool bn_t::operator>=(const bn_t& src2) const { return compare(*this, src2) >= 0; }
bool bn_t::operator<=(const bn_t& src2) const { return compare(*this, src2) <= 0; }

bool bn_t::operator==(int src2) const { return compare(*this, bn_t(src2)) == 0; }
bool bn_t::operator!=(int src2) const { return compare(*this, bn_t(src2)) != 0; }
bool bn_t::operator>(int src2) const { return compare(*this, bn_t(src2)) > 0; }
bool bn_t::operator<(int src2) const { return compare(*this, bn_t(src2)) < 0; }
bool bn_t::operator>=(int src2) const { return compare(*this, bn_t(src2)) >= 0; }
bool bn_t::operator<=(int src2) const { return compare(*this, bn_t(src2)) <= 0; }

bn_t& bn_t::operator+=(const bn_t& src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return *this = mod->add(*this, src2);

  int res = BN_add(*this, *this, src2);
  cb_assert(res);
  return *this;
}

bn_t& bn_t::operator-=(const bn_t& src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return *this = mod->sub(*this, src2);

  int res = BN_sub(*this, *this, src2);
  cb_assert(res);
  return *this;
}

bn_t& bn_t::operator*=(const bn_t& src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return *this = mod->mul(*this, src2);

  int res = BN_mul(*this, *this, src2, thread_local_storage_bn_ctx());
  cb_assert(res);
  return *this;
}

bn_t& bn_t::operator/=(const bn_t& src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return *this = mod->div(*this, src2);

  int res = BN_div(*this, nullptr, *this, src2, thread_local_storage_bn_ctx());
  cb_assert(res);
  return *this;
}

bn_t& bn_t::operator%=(const mod_t& src2) { return *this = src2.mod(*this); }

bn_t& bn_t::operator++() {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return *this = mod->add(*this, bn_t(1));

  int res = BN_add_word(*this, 1);
  cb_assert(res);
  return *this;
}

const bn_t bn_t::operator++(int dummy) {
  bn_t old = *this;  // copy old value
  operator++();      // prefix increment
  return old;        // return old value
}

bn_t& bn_t::operator+=(int src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return *this = mod->add(*this, mod->mod(src2));

  int res;
  if (src2 >= 0)
    res = BN_add_word(*this, src2);
  else
    res = BN_sub_word(*this, -src2);
  cb_assert(res);
  return *this;
}

bn_t& bn_t::operator-=(int src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return *this = mod->sub(*this, mod->mod(src2));

  int res;
  if (src2 >= 0)
    res = BN_sub_word(*this, src2);
  else
    res = BN_add_word(*this, -src2);
  cb_assert(res);
  return *this;
}

bn_t& bn_t::operator*=(int src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return *this = mod->mul(*this, mod->mod(src2));

  bool neg = src2 < 0;
  if (neg) src2 = -src2;
  int res = BN_mul_word(*this, src2);
  cb_assert(res);
  if (neg) BN_set_negative(*this, !BN_is_negative(*this));
  cb_assert(res);
  return *this;
}

bn_t& bn_t::operator/=(int src2) {
  int res = BN_div(*this, nullptr, *this, bn_t(src2), thread_local_storage_bn_ctx());
  cb_assert(res);
  return *this;
}

bn_t operator+(const bn_t& src1, const bn_t& src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return mod->add(src1, src2);

  bn_t result;
  int res = BN_add(result, src1, src2);
  cb_assert(res);
  return result;
}

bn_t operator+(const bn_t& src1, int src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return mod->add(src1, mod->mod(src2));

  int res;
  bn_t result = src1;
  if (src2 >= 0)
    res = BN_add_word(result, src2);
  else
    res = BN_sub_word(result, -src2);
  cb_assert(res);
  return result;
}

bn_t operator-(const bn_t& src1, const bn_t& src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return mod->sub(src1, src2);

  bn_t result;
  int res = BN_sub(result, src1, src2);
  cb_assert(res);
  return result;
}

bn_t operator-(const bn_t& src1, int src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return mod->sub(src1, mod->mod(src2));

  bn_t result = src1;
  int res;
  if (src2 >= 0)
    res = BN_sub_word(result, src2);
  else
    res = BN_add_word(result, -src2);
  cb_assert(res);
  return result;
}

bn_t operator*(const bn_t& src1, const bn_t& src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return mod->mul(src1, src2);

  bn_t result;
  int res = BN_mul(result, src1, src2, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
  return result;
}

bn_t operator*(const bn_t& src1, int src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return mod->mul(src1, mod->mod(src2));

  bn_t result = src1;
  bool neg = src2 < 0;
  if (neg) src2 = -src2;
  int res = BN_mul_word(result, src2);
  cb_assert(res);
  if (neg) BN_set_negative(result, !BN_is_negative(result));
  return result;
}

bn_t operator/(const bn_t& src1, const bn_t& src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return mod->div(src1, src2);

  bn_t result;
  int res = BN_div(result, nullptr, src1, src2, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
  return result;
}

bn_t operator/(const bn_t& src1, int src2) {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return mod->div(src1, mod->mod(src2));

  return src1 / bn_t(src2);
}

bn_t operator%(const bn_t& src1, const mod_t& src2) { return src2.mod(src1); }

bn_t operator-(const bn_t& src1) { return src1.neg(); }

bn_t bn_t::pow(const bn_t& exp) const { return pow(*this, exp); }

bn_t bn_t::pow(const bn_t& src1, const bn_t& src2) {  // static
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return mod->pow(src1, src2);

  bn_t result;
  int res = BN_exp(result, src1, src2, thread_local_storage_bn_ctx());
  cb_assert(res);
  return result;
}

bn_t bn_t::div(const bn_t& src1, const bn_t& src2, bn_t* rem) {  // static
  bn_t result;
  int res = BN_div(result, rem ? (BIGNUM*)*rem : nullptr, src1, src2, thread_local_storage_bn_ctx());
  cb_assert(res);
  return result;
}

bn_t& bn_t::operator<<=(int value) {
  int res = BN_lshift(*this, *this, value);
  cb_assert(res);
  return *this;
}

bn_t& bn_t::operator>>=(int value) {
  int res = BN_rshift(*this, *this, value);
  cb_assert(res);
  return *this;
}

bn_t bn_t::lshift(int n) const {
  bn_t result;
  int res = BN_lshift(result, *this, n);
  cb_assert(res);
  return result;
}

bn_t bn_t::rshift(int n) const {
  bn_t result;
  int res = BN_rshift(result, *this, n);
  cb_assert(res);
  return result;
}

void bn_t::set_bit(int n, bool bit) {
  if (bit)
    BN_set_bit(*this, n);
  else
    BN_clear_bit(*this, n);
}

bool bn_t::is_bit_set(int n) const {
  if (BN_is_bit_set(*this, n)) return true;
  return false;
}

bool bn_t::is_odd() const { return BN_is_odd(*this) ? true : false; }

bool bn_t::is_zero() const { return BN_is_zero(*this) ? true : false; }

bn_t bn_t::neg() const {
  const mod_t* mod = thread_local_storage_mod();
  if (mod) return mod->neg(*this);

  if (BN_is_zero(*this)) return *this;

  bn_t result = *this;
  BN_set_negative(result, !BN_is_negative(*this));
  return result;
}

bn_t bn_t::rand_bitlen(int bits, bool top_bit_set) {
  bn_t result;
  int top = top_bit_set ? 1 : -1;
  int res = BN_rand(result, bits, top, 0);
  cb_assert(res > 0);
  return result;
}

bn_t bn_t::rand(const bn_t& range) {  // static
  bn_t result;
  int res = BN_rand_range(result, range);
  cb_assert(res > 0);
  return result;
}

bn_t bn_t::pow_mod(const bn_t& exp, const mod_t& mod) const { return mod.pow(*this, exp); }

bn_t bn_t::inv() const {  // only valid for modulo
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  return mod->inv(*this);
}

int bn_t::get_bit(int n) const { return BN_is_bit_set(*this, n); }

int bn_t::get_bin_size() const { return BN_num_bytes(*this); }

int bn_t::get_bits_count() const { return BN_num_bits(*this); }

int bn_t::to_bin(byte_ptr dst) const { return BN_bn2bin(*this, dst); }

void bn_t::to_bin(byte_ptr dst, int size) const {
  int bin_size = get_bin_size();
  cb_assert(size >= bin_size);
  BN_bn2binpad(*this, dst, size);
}

buf_t bn_t::to_bin() const {
  correct_top();

  buf_t out(get_bin_size());
  to_bin(out.data());
  return out;
}

buf_t bn_t::to_bin(int size) const {
  buf_t out(size);
  to_bin(out.data(), size);
  return out;
}

buf_t bn_t::vector_to_bin(const std::vector<bn_t>& vals, int val_size) {
  buf_t out(val_size * vals.size());
  mem_t out_mem = out;
  for (int i = 0; i < vals.size(); i++, out_mem = out_mem.skip(val_size)) vals[i].to_bin(out_mem.take(val_size));
  return out;
}

bn_t bn_t::from_bin(mem_t mem) {  // static
  bn_t result;
  auto res = BN_bin2bn(mem.data, mem.size, result);
  cb_assert(res);
  return result;
}

std::vector<bn_t> bn_t::vector_from_bin(mem_t mem, int n, int size, const mod_t& q) {  // static
  std::vector<bn_t> result(n);
  cb_assert(mem.size == n * size);
  for (int i = 0; i < n; i++, mem = mem.skip(size)) result[i] = bn_t::from_bin(mem.take(size)) % q;
  return result;
}

bn_t bn_t::from_bin_bitlen(mem_t mem, int bits) {  // static
  cb_assert(mem.size == coinbase::bits_to_bytes(bits));
  int unused_bits = bytes_to_bits(mem.size) - bits;
  byte_t mask = 0xff >> unused_bits;
  if (mem[0] == (mem[0] & mask)) return from_bin(mem);

  buf_t temp = mem;
  temp[0] &= mask;
  return from_bin(temp);
}

std::string bn_t::to_string() const {
  char* s = BN_bn2dec(*this);
  if (!s) throw std::bad_alloc();
  std::string result = s;
  OPENSSL_free(s);
  return result;
}

std::string bn_t::to_hex() const {
  char* s = BN_bn2hex(*this);
  if (!s) throw std::bad_alloc();
  std::string result = s;
  OPENSSL_free(s);
  return result;
}

bn_t bn_t::from_string(const_char_ptr str) {
  bn_t result;
  BIGNUM* ptr = result;
  cb_assert(0 != BN_dec2bn(&ptr, str));
  return result;
}

bn_t bn_t::from_hex(const_char_ptr str) {
  bn_t result;
  BIGNUM* ptr = result;
  cb_assert(0 != BN_hex2bn(&ptr, str));
  return result;
}

int bn_t::sign() const {
  if (BN_is_zero(*this)) return 0;
  if (BN_is_negative(*this)) return -1;
  return +1;
}

bn_t operator<<(const bn_t& src1, int src2) {
  bn_t result;
  int res = BN_lshift(result, src1, src2);
  cb_assert(res);
  return result;
}

bn_t operator>>(const bn_t& src1, int src2) {
  bn_t result;
  int res = BN_rshift(result, src1, src2);
  cb_assert(res);
  return result;
}

void bn_t::convert(coinbase::converter_t& converter) {
  uint32_t neg = sign() < 0;
  uint32_t value_size = get_bin_size();
  uint32_t header = (value_size << 1) | neg;
  converter.convert_len(header);

  if (converter.is_write()) {
    if (!converter.is_calc_size()) to_bin(converter.current());
  } else {
    neg = header & 1;
    value_size = header >> 1;
    if (converter.is_error() || !converter.at_least(value_size)) {
      converter.set_error();
      return;
    }
    if (value_size == 0 && neg) {
      converter.set_error();
      return;
    }
    auto res = BN_bin2bn(converter.current(), value_size, *this);
    if (!res) throw std::bad_alloc();
    if (neg) BN_set_negative(*this, 1);
  }
  converter.forward(value_size);
}

bn_t bn_t::generate_prime(int bits, bool safe, gen_prime_callback callback, void* ctx) {
  bn_t result;
  BN_GENCB* cb = nullptr;

  if (callback) cb = BN_GENCB_new();
  if (cb) BN_GENCB_set_old(cb, callback, ctx);

  int res = BN_generate_prime_ex(result, bits, safe, NULL, NULL, cb);
  cb_assert(res);
  cb_assert(result.get_bits_count() == bits);

  if (cb) BN_GENCB_free(cb);
  return result;
}

bool bn_t::prime() const { return BN_check_prime(*this, thread_local_storage_bn_ctx(), NULL) ? true : false; }

bn_t bn_t::gcd(const bn_t& src1, const bn_t& src2) {
  bn_t result;
  int res = BN_gcd(result, src1, src2, thread_local_storage_bn_ctx());
  // This will act as returning an error, since the GCD is never 0.
  if (res == 0) return 0;
  return result;
}

void bn_t::set_modulo(const mod_t& mod) { thread_local_storage_set_mod(&mod); }
bool bn_t::check_modulo(const mod_t& mod) { return thread_local_storage_mod() != nullptr; }
void bn_t::reset_modulo(const mod_t& mod) { thread_local_storage_set_mod(nullptr); }

std::ostream& operator<<(std::ostream& os, const bn_t& obj) {
  os << obj.to_string();
  return os;
}

static inline BN_ULONG consttime_gt(BN_ULONG x, BN_ULONG y) {
  BN_ULONG z = y - x;
  z ^= (x ^ y) & (x ^ z);

#if BN_BYTES == 4
  return int32_t(z) >> 31;
#else
  return int64_t(z) >> 63;
#endif
}

static int bn_cmp_ct(const BIGNUM& a, const BIGNUM& b) {
  int len = std::max(a.top, b.top);

  BN_ULONG xa = 1 - a.neg;
  BN_ULONG xb = 1 - b.neg;
  BN_ULONG lt = consttime_gt(xb, xa);
  BN_ULONG gt = consttime_gt(xa, xb);

  for (int i = len - 1; i >= 0; i--) {
    xa = (i < a.top) ? a.d[i] : 0;
    xb = (i < b.top) ? b.d[i] : 0;
    BN_ULONG xlt = consttime_gt(xb, xa) & ~gt;
    BN_ULONG xgt = consttime_gt(xa, xb) & ~lt;
    lt |= xlt;
    gt |= xgt;
  }
  return int(lt - gt);
}

extern "C" int BN_cmpCT(const BIGNUM* a, const BIGNUM* b) {
  if (!a || !b) {
    if (a) return -1;
    if (b) return 1;
    return 0;
  }
  return bn_cmp_ct(*a, *b);
}

int bn_t::compare(const bn_t& src1, const bn_t& src2) {  // static
  return bn_cmp_ct(*(const BIGNUM*)src1, *(const BIGNUM*)src2);
}

// min <= x <= max
error_t check_closed_range(const bn_t& min, const bn_t& x, const bn_t& max) {
  if (x < min || x > max) return coinbase::error(E_CRYPTO, "check_closed_range failed");
  return SUCCESS;
}

// min <= x < max
error_t check_right_open_range(const bn_t& min, const bn_t& x, const bn_t& max) {
  if (x < min || x >= max) return coinbase::error(E_CRYPTO, "check_right_open_range failed");
  return SUCCESS;
}

// min < x < max
error_t check_open_range(const bn_t& min, const bn_t& x, const bn_t& max) {
  if (x <= min || x >= max) return coinbase::error(E_CRYPTO, "check_open_range failed");
  return SUCCESS;
}

};  // namespace coinbase::crypto
