#include <cbmpc/core/utils.h>
#include <cbmpc/crypto/base.h>

namespace coinbase::crypto {

#if defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR
static thread_local int vartime_scope = 1;
#else
static thread_local int vartime_scope = 0;
#endif

vartime_scope_t::vartime_scope_t() { vartime_scope++; }
vartime_scope_t::~vartime_scope_t() { vartime_scope--; }
bool is_vartime_scope() { return vartime_scope != 0; }

mod_t::mod_t() {}

mod_t::~mod_t() {
  if (mont) BN_MONT_CTX_free(mont);
}

void mod_t::convert(coinbase::converter_t& converter) {
  converter.convert(m);
  if (!converter.is_write()) {
    if (converter.is_error()) return;
    if (m <= 0) {
      converter.set_error();
      return;
    }
    if (!m.is_odd()) {
      converter.set_error();
      return;
    }
    init(m);
  }
}

mod_t::mod_t(const mod_t& src)
    : m(src.m), mu(src.mu), b_pow_k_plus1(src.b_pow_k_plus1), multiplicative_dense(src.multiplicative_dense) {
  if (src.mont) {
    mont = BN_MONT_CTX_new();
    if (!mont) throw std::bad_alloc();

    auto res = BN_MONT_CTX_copy(mont, src.mont);
    cb_assert(res);
  }
}

mod_t::mod_t(mod_t&& src)
    : m(std::move(src.m)),
      mu(std::move(src.mu)),
      b_pow_k_plus1(std::move(src.b_pow_k_plus1)),
      mont(src.mont),
      multiplicative_dense(src.multiplicative_dense) {
  src.mont = nullptr;
}

mod_t& mod_t::operator=(const mod_t& src) {
  if (&src != this) {
    if (!mont) mont = BN_MONT_CTX_new();
    if (!mont) throw std::bad_alloc();

    if (src.mont) {
      auto res = BN_MONT_CTX_copy(mont, src.mont);
      cb_assert(res);
    }
    m = src.m;
    mu = src.mu;
    b_pow_k_plus1 = src.b_pow_k_plus1;
    multiplicative_dense = src.multiplicative_dense;
  }
  return *this;
}

mod_t& mod_t::operator=(mod_t&& src) {
  if (&src != this) {
    if (mont) BN_MONT_CTX_free(mont);
    mont = src.mont;
    src.mont = nullptr;
    m = std::move(src.m);
    mu = std::move(src.mu);
    b_pow_k_plus1 = std::move(src.b_pow_k_plus1);
    multiplicative_dense = src.multiplicative_dense;
  }
  return *this;
}

void mod_t::check(const bn_t& a) const {
  assert(a >= 0 && "out of range for constant-time operations");
  assert(a < m && "out of range for constant-time operations");
}

void mod_t::_add(bn_t& r, const bn_t& a, const bn_t& b) const {
  if (vartime_scope) {
    int res = BN_mod_add(r, a, b, m, bn_t::thread_local_storage_bn_ctx());
    cb_assert(res);
  } else {
    check(a);
    check(b);
    int res = bn_mod_add_fixed_top(r, a, b, m);
    cb_assert(res);
  }
}

void mod_t::_sub(bn_t& r, const bn_t& a, const bn_t& b) const {
  if (vartime_scope) {
    int res = BN_mod_sub(r, a, b, m, bn_t::thread_local_storage_bn_ctx());
    cb_assert(res);
  } else {
    check(a);
    check(b);
    int res = bn_mod_sub_fixed_top(r, a, b, m);
    cb_assert(res);
  }
}

void mod_t::_neg(bn_t& r, const bn_t& a) const {
  if (vartime_scope) {
    if (BN_is_zero(a)) {
      r = a;
      return;
    }
    int res = BN_mod_sub(r, m, a, m, bn_t::thread_local_storage_bn_ctx());
    cb_assert(res);
  } else {
    check(a);
    int res = bn_mod_sub_fixed_top(r, m, a, m);
    cb_assert(res);
    res = bn_mod_sub_fixed_top(r, r, m, m);
    cb_assert(res);
  }
}

static BIGNUM bn_buf(BN_ULONG* ptr, int size) {
  BIGNUM a;
  a.d = ptr;
  a.top = a.dmax = size;
  a.flags = BN_FLG_FIXED_TOP | BN_FLG_STATIC_DATA | BN_FLG_CONSTTIME;
  a.neg = 0;
  return a;
}

static BIGNUM bn_skip(BIGNUM a, int n) {
  a.d += n;
  a.top -= n;
  a.dmax -= n;
  return a;
}

static void bn_copy(BIGNUM r, BIGNUM a) {
  int len = std::min(a.top, r.top);
  std::copy(a.d, a.d + len, r.d);
  if (r.top > len) std::fill(r.d + len, r.d + r.top, 0);
}

void mod_t::_mul(bn_t& r, const bn_t& a, const bn_t& b) const {
  if (vartime_scope) {
    int res = BN_mod_mul(r, a, b, m, bn_t::thread_local_storage_bn_ctx());
    cb_assert(res);
    return;
  }

  check(a);
  check(b);

  const BIGNUM& aa = *(const BIGNUM*)a;
  const BIGNUM& bb = *(const BIGNUM*)b;
  BN_ULONG buf[aa.top + bb.top];
  bn_mul_normal(buf, aa.d, aa.top, bb.d, bb.top);

  BIGNUM temp = bn_buf(buf, aa.top + bb.top);
  _mod(*(BIGNUM*)r, temp);
}

bn_t mod_t::div(const bn_t& a, const bn_t& b) const { return mul(a, inv(b)); }

enum { BN_ULONG_BITS = sizeof(BN_ULONG) * 8 };

static BN_ULONG div_words_by_two(int n, BN_ULONG* r) {
  uint64_t carry = 0;
  for (int i = n - 1; i >= 0; i--) {
    uint64_t c = r[i] << (BN_ULONG_BITS - 1);
    r[i] = (r[i] >> 1) | carry;
    carry = c;
  }
  return carry;
}

// Returns a mask of all bits set (0xFFFFFFFFF...) if flag == true,
// or 0 if flag == false, with a small inline assembly barrier to keep
// the compiler from optimizing it away under LTCG or at high -O levels.
static inline BN_ULONG constant_time_mask_64(bool flag) {
  BN_ULONG mask = (BN_ULONG)0 - (BN_ULONG)flag;
#if defined(__GNUC__) || defined(__clang__)
  // A small barrier so the compiler can't trivially treat mask as a compile-time constant
  __asm__("" : "+r"(mask) : :);
#endif
  return mask;
}

static void cnd_swap(int n, bool flag, BN_ULONG a[], BN_ULONG b[]) {
  BN_ULONG mask = constant_time_mask_64(flag);
  for (int i = 0; i < n; i++) {
    BN_ULONG delta = (a[i] ^ b[i]) & mask;
    a[i] ^= delta;
    b[i] ^= delta;
  }
}

static BN_ULONG ct_bn_add_words(BN_ULONG* r, const BN_ULONG* a, const BN_ULONG* b, int n) {
  BN_ULONG carry = 0;
  for (int i = 0; i < n; i++) {
    r[i] = addx(a[i], b[i], (uint64_t&)carry);
  }
  return carry;
}

static BN_ULONG ct_bn_sub_words(BN_ULONG* r, const BN_ULONG* a, const BN_ULONG* b, int n) {
  BN_ULONG borrow = 0;
  for (int i = 0; i < n; i++) {
    r[i] = subx(a[i], b[i], (uint64_t&)borrow);
  }
  return borrow;
}

static BN_ULONG cnd_add_words(int n, BN_ULONG r[], bool flag, const BN_ULONG a[]) {
  BN_ULONG mask = constant_time_mask_64(flag);
  BN_ULONG temp[n];
  for (int i = 0; i < n; i++) temp[i] = a[i] & mask;
  return ct_bn_add_words(r, r, temp, n);
}

static BN_ULONG cnd_sub_words(int n, BN_ULONG r[], bool flag, const BN_ULONG a[]) {
  BN_ULONG mask = constant_time_mask_64(flag);
  BN_ULONG temp[n];
  for (int i = 0; i < n; i++) temp[i] = a[i] & mask;
  return ct_bn_sub_words(r, r, temp, n);
}

static BN_ULONG cnd_neg_words(int n, BN_ULONG r[], bool flag) {
  BN_ULONG mask = constant_time_mask_64(flag);
  for (int i = 0; i < n; i++) r[i] ^= mask;
  BN_ULONG temp[n];
  std::fill(temp, temp + n, 0);
  temp[0] = (BN_ULONG)flag;
  return ct_bn_add_words(r, r, temp, n);
}

/* Algorithm 5 in https://inria.hal.science/hal-01506572/document */
void mod_t::scr_inv(bn_t& res, const bn_t& in) const {
  cb_assert(in < m);
  auto q = (const BIGNUM*)m;
  int n = q->top;

  auto x = (const BIGNUM*)in;
  bn_t val;

  auto r = (BIGNUM*)res;
  auto exp_res = bn_wexpand(r, n);
  cb_assert(exp_res);
  r->top = n;

  const BN_ULONG* m = q->d;
  BN_ULONG a[n];
  auto top = std::min(x->top, n);
  std::copy(x->d, x->d + top, a);
  std::fill(a + top, a + n, 0);  // a
  BN_ULONG b[n];
  std::copy(m, m + n, b);  // b = m
  BN_ULONG u[n];
  std::fill(u, u + n, 0);
  u[0] = 1;  // u = 1
  BN_ULONG* v = r->d;
  std::fill(v, v + n, 0);  // v = 0
  BN_ULONG mp1o2[n];       // (m+1)/2
  ct_bn_add_words(mp1o2, m, u, n);
  div_words_by_two(n, mp1o2);

  for (int i = 0; i < n * BN_ULONG_BITS * 2; i++) {
    bool a_is_odd = bool(a[0] & 1);
    bool underflow = bool(cnd_sub_words(n, a, a_is_odd, b));  // if (a_is_odd)   a -= b
    cnd_add_words(n, b, underflow, a);                        // if (underflow)  b += a
    cnd_neg_words(n, a, underflow);                           // if (underflow)  a = -a
    cnd_swap(n, underflow, u, v);                             // if (underflow)  swap u <=> v
    div_words_by_two(n, a);                                   // a /= 2
    bool borrow = bool(cnd_sub_words(n, u, a_is_odd, v));     // if (a_is_odd)   u -= v
    cnd_add_words(n, u, borrow, m);                           // if (borrow)     u += m;
    bool u_is_odd = bool(u[0] & 1);
    div_words_by_two(n, u);                // u /= 2
    cnd_add_words(n, u, u_is_odd, mp1o2);  // if (u_is_odd)   u += (m+1) / 2
  }
}

void mod_t::random_masking_inv(bn_t& r, const bn_t& a) const {
  // Eventhough, this function is not truely constant-time, the running time is not dependent on the input (bn_t a).
  // Therefore, it doesn't leak any information of the input.
  bn_t mask = rand();
  bn_t masked_a = mul(a, mask);
  masked_a.correct_top();
  auto res = BN_mod_inverse(r, masked_a, m, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res && "mod_t::random_masking_inv failed");
  r = mul(r, mask);
}

void mod_t::_inv(bn_t& r, const bn_t& a, inv_algo_e alg) const {
  if (vartime_scope) {
    a.correct_top();
    auto res = BN_mod_inverse(r, a, m, bn_t::thread_local_storage_bn_ctx());
    cb_assert(res && "vartime mod_t::inv failed");
  } else {
    if (alg == inv_algo_e::SCR) {
      scr_inv(r, a);
      return;
    } else if (alg == inv_algo_e::RandomMasking) {
      random_masking_inv(r, a);
      return;
    } else {
      cb_assert(false && "invalid algorithm");
    }
  }
}

void mod_t::_pow(bn_t& r, const bn_t& x, const bn_t& e) const {
  cb_assert(e.sign() >= 0 && "only support non-negative exponent");
  int res = BN_mod_exp_mont_consttime(r, x, e, m, bn_t::thread_local_storage_bn_ctx(), mont);
  cb_assert(res);
}

bn_t mod_t::rand() const {
  if (vartime_scope) return bn_t::rand(m);

  int n = coinbase::bits_to_bytes(m.get_bits_count() + SEC_P_COM);
  buf_t bin = crypto::gen_random(n);
  bn_t a = bn_t::from_bin(bin);

  bn_t r;
  int res = BN_from_montgomery(r, a, mont, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
  return r;
}

bn_t mod_t::to_mont(const bn_t& x) const {
  bn_t r;
  int res = BN_to_montgomery(r, x, mont, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
  return r;
}

bn_t mod_t::from_mont(const bn_t& x) const {
  bn_t r;
  int res = BN_from_montgomery(r, x, mont, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
  return r;
}

bn_t mod_t::mul_mont(const bn_t& x, const bn_t& y) const {
  bn_t r;
  int res = BN_mod_mul_montgomery(r, x, y, mont, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res);
  return r;
}

void mod_t::init(const bn_t& m) {
  if (!mont) mont = BN_MONT_CTX_new();
  if (!mont) throw std::bad_alloc();

  int res = BN_MONT_CTX_set(mont, m, bn_t::thread_local_storage_bn_ctx());
  cb_assert(res && "BN_MONT_CTX_set failed");
  this->m = m;

  // barrett
  int k = (m.get_bits_count() + 63) / 64;
  bn_t b_pow_2k = bn_t(1).mul_2_pow(2 * k * 64);    // b^{2k}
  mu = b_pow_2k / m;                                // µ = ⌊b^{2k} / m⌋
  b_pow_k_plus1 = bn_t(1).mul_2_pow((k + 1) * 64);  // b^{k+1}
}

static void barrett_partial_mul(int ResultLength, BN_ULONG r[], int M, const BN_ULONG u[], int N, const BN_ULONG v[]) {
  using TT = unsigned __int128;
  std::fill(r, r + ResultLength, 0);

  for (int j = 0; j < N; ++j) {
    uint64_t k = 0;
    const auto m = std::min(M, ResultLength - j);
    for (auto i = 0; i < m; ++i) {
      TT t = TT(u[i]) * TT(v[j]) + r[i + j] + k;
      r[i + j] = uint64_t(t);
      k = t >> 64;
    }
    if (j + M < ResultLength) r[j + M] = k;
  }
}

void mod_t::_mod(bn_t& r, const bn_t& x) const {
  const BIGNUM* _m = (const BIGNUM*)m;
  const BIGNUM* _x = (const BIGNUM*)x;

  bn_t temp;
  if (_x->top > 2 * _m->top) {
    bn_t mSquare;
    BN_mul(mSquare, m, m, bn_t::thread_local_storage_bn_ctx());

    mod_t mBig;
    mBig.init(mSquare);
    mBig._mod(temp, x);
    _x = (const BIGNUM*)temp;
  }

  _mod(*(BIGNUM*)r, *_x);
}

void mod_t::_mod(BIGNUM& r, const BIGNUM& x) const {
  if (vartime_scope) {
    int res = BN_div(nullptr, &r, &x, m, bn_t::thread_local_storage_bn_ctx());
    cb_assert(res);
    return;
  }

  cb_assert(!x.neg);

  const BIGNUM& mu = *(const BIGNUM*)this->mu;
  const BIGNUM& mm = *(const BIGNUM*)m;
  cb_assert(mu.top == mm.top + 1);
  cb_assert(x.top <= 2 * mm.top);

  const int k = mm.top;
  if (x.top < k) {
    BN_copy(&r, &x);
    return;
  }

  BIGNUM q1 = bn_skip(x, k - 1);  // q1 = x / b^(k-1)

  int q2_len = q1.top + mu.top;
  BN_ULONG q2_buf[q2_len];
  BIGNUM q2 = bn_buf(q2_buf, q2_len);
  bn_mul_normal(q2.d, q1.d, q1.top, mu.d, mu.top);  // q2 = q1 * mu;

  BIGNUM q3 = bn_skip(q2, k + 1);  // q3 = q2 / b^(k+1)

  BN_ULONG r2_buf[k + 1];
  BIGNUM r2 = bn_buf(r2_buf, k + 1);
  barrett_partial_mul(r2.top, r2.d, q3.top, q3.d, mm.top, mm.d);  // r2 = partial_mul<k + 1>(q3, modulus);

  BIGNUM r1 = bn_buf(q2_buf, k + 1);
  bn_copy(r1, x);  // r1 = x mod b^(k+1)

  uint64_t borrow = ct_bn_sub_words(r1.d, r1.d, r2.d, k + 1);

  r2 = bn_buf(r2_buf, k);
  borrow = ct_bn_sub_words(r2.d, r1.d, mm.d, k);
  borrow &= (r1.d[k] == 0);
  uint64_t mask = (uint64_t)0 - borrow;
  for (int i = 0; i < k; i++) {
    r1.d[i] = MASKED_SELECT(mask, r1.d[i], r2.d[i]);
  }

  borrow = ct_bn_sub_words(r2.d, r1.d, mm.d, k);
  mask = (uint64_t)0 - borrow;
  for (int i = 0; i < k; i++) {
    r1.d[i] = MASKED_SELECT(mask, r1.d[i], r2.d[i]);
  }

  auto exp_res = bn_wexpand(&r, k);
  cb_assert(exp_res);
  r.flags |= BN_FLG_CONSTTIME;
  r.top = k;

  bn_copy(r, r1);
}

// static
bn_t mod_t::mod(const bn_t& a, const bn_t& m) {
  if (is_vartime_scope()) {
    bn_t result;
    int res = BN_mod(result, a, m, bn_t::thread_local_storage_bn_ctx());
    cb_assert(res);
    return result;
  } else {
    cb_assert(m.is_odd());
    return mod_t(m).mod(a);
  }
}

bool mod_t::coprime(const bn_t& a, const mod_t& m) {
  if (vartime_scope) {
    return bn_t::gcd(a, m.m) == 1;
  }
  bn_t a_mod = m.mod(a);
  bn_t a_inv = m.inv(a_mod);
  return m.mul(a_inv, a_mod) == 1;
}

// static
bn_t mod_t::N_inv_mod_phiN_2048(const bn_t& N, const bn_t& phiN) {
  if (vartime_scope) {
    bn_t result;
    auto res = BN_mod_inverse(result, N, phiN, bn_t::thread_local_storage_bn_ctx());
    cb_assert(res);
    return result;
  }
  assert(!phiN.is_odd());
  assert(N.is_odd());
  bn_t N_minus_phiN = LARGEST_PRIME_MOD_2048.sub(N, phiN);
  N_minus_phiN.correct_top();
  mod_t mod_N_minus_phiN(N_minus_phiN, false);
  bn_t alpha = mod_N_minus_phiN.inv(mod_N_minus_phiN.mod(phiN));  // alpha = m^-1 % a
  bn_t beta = mod_t(N).inv(N_minus_phiN);                         // beta = a^-1 % (m+a)
  bn_t result;
  MODULO(LARGEST_PRIME_MOD_2048) { result = beta + alpha - N_minus_phiN; }
  return result;
}

}  // namespace coinbase::crypto
