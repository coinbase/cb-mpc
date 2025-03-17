
#include "ec25519_core.h"

#include <cbmpc/core/utils.h>

#include "base_ec_core.h"

#define EXTENDED_COORD

namespace coinbase::crypto::ec25519_core {

struct bn256_t {
  uint64_t d[4];
  static bn256_t from_hex(const std::string& hex) { return from_bn(bn_t::from_hex(hex.c_str())); }
  static bn256_t from_str(const std::string& str) { return from_bn(bn_t::from_string(str.c_str())); }

  static bn256_t make(uint64_t d0, uint64_t d1 = 0, uint64_t d2 = 0, uint64_t d3 = 0) {
    bn256_t r;
    r.d[0] = d0;
    r.d[1] = d1;
    r.d[2] = d2;
    r.d[3] = d3;
    return r;
  }

  void set(uint64_t d0, uint64_t d1 = 0, uint64_t d2 = 0, uint64_t d3 = 0) {
    d[0] = d0;
    d[1] = d1;
    d[2] = d2;
    d[3] = d3;
  }

  static bn256_t zero() { return make(0); }
  static bn256_t one() { return make(1); }

  static bn256_t from_bn(const bn_t& x) { return from_bin(x.to_bin(32)); }

  static bn256_t from_bin(mem_t bin) {
    cb_assert(bin.size == 32);
    bn256_t r;
    r.d[0] = coinbase::be_get_8(bin.data + 24);
    r.d[1] = coinbase::be_get_8(bin.data + 16);
    r.d[2] = coinbase::be_get_8(bin.data + 8);
    r.d[3] = coinbase::be_get_8(bin.data + 0);
    return r;
  }

  bn_t to_bn() const { return bn_t::from_bin(to_bin()); }

  buf_t to_bin() const {
    buf_t r(32);
    to_bin(r.data());
    return r;
  }

  void to_bin(byte_ptr r) const {
    coinbase::be_set_8(&r[24], d[0]);
    coinbase::be_set_8(&r[16], d[1]);
    coinbase::be_set_8(&r[8], d[2]);
    coinbase::be_set_8(&r[0], d[3]);
  }

  bool operator==(const bn256_t& b) const {
    uint64_t x = d[0] ^ b.d[0];
    x |= d[1] ^ b.d[1];
    x |= d[2] ^ b.d[2];
    x |= d[3] ^ b.d[3];
    return x == 0;
  }

  bool operator!=(const bn256_t& b) const { return !(*this == b); }

  bool is_zero() const { return (d[0] | d[1] | d[2] | d[3]) == 0; }

  bool is_odd() const { return bool(d[0] & 1); }

  static void cnd_move(bn256_t& r, bool flag, const bn256_t& a) {
    uint64_t mask = (uint64_t)0 - ((uint64_t)flag);
#if defined(__GNUC__) || defined(__clang__)
    // A small barrier so the compiler can't trivially treat mask as a compile-time constant
    __asm__("" : "+r"(mask) : :);
#endif
    r.d[0] = MASKED_SELECT(mask, a.d[0], r.d[0]);
    r.d[1] = MASKED_SELECT(mask, a.d[1], r.d[1]);
    r.d[2] = MASKED_SELECT(mask, a.d[2], r.d[2]);
    r.d[3] = MASKED_SELECT(mask, a.d[3], r.d[3]);
  }
};

enum class arch_e {
  intel_mulx,
  regular,
};

#ifdef __x86_64__

static bool support_x64_mulx() {
  // return false;
  uint32_t eax = 0, ebx, ecx, edx;
  __cpuid(0, eax, ebx, ecx, edx);
  if (eax >= 7) {
    ebx = 0;
    __cpuid_count(7, 0, eax, ebx, ecx, edx);
    return (ebx & (bit_BMI2 | bit_ADX)) == (bit_BMI2 | bit_ADX);
  }

  return false;
}

bool g_intel_mulx = support_x64_mulx();

static void __attribute__((naked)) fe_square_mulx(uint64_t r[4], const uint64_t x[4]) {
  __asm__ volatile(
      "push %%r11 ;"
      "push %%r12 ;"
      "push %%r13 ;"
      "push %%r14 ;"
      "push %%r15 ;"
      "push %%rbx ;"
      "push %%rbp ;"

      "xorq    %%r13,%%r13                ;"
      "movq    0(%%rsi),%%rdx             ;"
      "mulx    8(%%rsi),%%r9,%%r10        ;"
      "mulx    16(%%rsi),%%rcx,%%r11      ;"
      "adcx    %%rcx,%%r10                ;"
      "mulx    24(%%rsi),%%rcx,%%r12      ;"
      "adcx    %%rcx,%%r11                ;"
      "adcx    %%r13,%%r12                ;"
      "movq    8(%%rsi),%%rdx             ;"
      "xorq    %%r14,%%r14                ;"
      "mulx    16(%%rsi),%%rcx,%%rdx      ;"
      "adcx    %%rcx,%%r11                ;"
      "adox    %%rdx,%%r12                ;"
      "movq    8(%%rsi),%%rdx             ;"
      "mulx    24(%%rsi),%%rcx,%%rdx      ;"
      "adcx    %%rcx,%%r12                ;"
      "adox    %%rdx,%%r13                ;"
      "adcx    %%r14,%%r13                ;"
      "xorq    %%r15,%%r15                ;"
      "movq    16(%%rsi),%%rdx            ;"
      "mulx    24(%%rsi),%%rcx,%%r14      ;"
      "adcx    %%rcx,%%r13                ;"
      "adcx    %%r15,%%r14                ;"
      "shld    $1,%%r14,%%r15             ;"
      "shld    $1,%%r13,%%r14             ;"
      "shld    $1,%%r12,%%r13             ;"
      "shld    $1,%%r11,%%r12             ;"
      "shld    $1,%%r10,%%r11             ;"
      "shld    $1,%%r9,%%r10              ;"
      "shlq    $1,%%r9                    ;"
      "xorq    %%rdx,%%rdx                ;"
      "movq    0(%%rsi),%%rdx             ;"
      "mulx    %%rdx,%%r8,%%rdx           ;"
      "adcx    %%rdx,%%r9                 ;"
      "movq    8(%%rsi),%%rdx             ;"
      "mulx    %%rdx,%%rcx,%%rdx          ;"
      "adcx    %%rcx,%%r10                ;"
      "adcx    %%rdx,%%r11                ;"
      "movq    16(%%rsi),%%rdx            ;"
      "mulx    %%rdx,%%rcx,%%rdx          ;"
      "adcx    %%rcx,%%r12                ;"
      "adcx    %%rdx,%%r13                ;"
      "movq    24(%%rsi),%%rdx            ;"
      "mulx    %%rdx,%%rcx,%%rdx          ;"
      "adcx    %%rcx,%%r14                ;"
      "adcx    %%rdx,%%r15                ;"
      "xorq    %%rbp,%%rbp                ;"
      "movq    $38,%%rdx                  ;"
      "mulx    %%r12,%%rax,%%r12          ;"
      "adcx    %%rax,%%r8                 ;"
      "adox    %%r12,%%r9                 ;"
      "mulx    %%r13,%%rcx,%%r13          ;"
      "adcx    %%rcx,%%r9                 ;"
      "adox    %%r13,%%r10                ;"
      "mulx    %%r14,%%rcx,%%r14          ;"
      "adcx    %%rcx,%%r10                ;"
      "adox    %%r14,%%r11                ;"
      "mulx    %%r15,%%rcx,%%r15          ;"
      "adcx    %%rcx,%%r11                ;"
      "adox    %%rbp,%%r15                ;"
      "adcx    %%rbp,%%r15                ;"
      "shld    $1,%%r11,%%r15             ;"
      "movq    $0x7fffffffffffffff, %%rax ;"
      "andq	   %%rax,%%r11                ;"
      "imul    $19,%%r15,%%r15            ;"
      "addq    %%r15,%%r8                 ;"
      "adcq    $0,%%r9                    ;"
      "adcq    $0,%%r10                   ;"
      "adcq    $0,%%r11                   ;"
      "movq    %%r8,0(%%rdi)              ;"
      "movq    %%r9,8(%%rdi)              ;"
      "movq    %%r10,16(%%rdi)            ;"
      "movq    %%r11,24(%%rdi)            ;"

      "pop %%rbp ;"
      "pop %%rbx ;"
      "pop %%r15 ;"
      "pop %%r14 ;"
      "pop %%r13 ;"
      "pop %%r12 ;"
      "pop %%r11 ;"
      "ret"
      :
      :
      :);
}

static void __attribute__((naked)) fe_mul_mulx(uint64_t r[4], const uint64_t x[4], const uint64_t y[4]) {
  __asm__ volatile(
      "push %%r11 ;"
      "push %%r12 ;"
      "push %%r13 ;"
      "push %%r14 ;"
      "push %%r15 ;"
      "push %%rbx ;"
      "push %%rbp ;"

      "movq    %%rdx,%%rbx                ;"
      "xorq    %%r13,%%r13                ;"
      "movq    0(%%rbx),%%rdx             ;"
      "mulx    0(%%rsi),%%r8,%%r9         ;"
      "mulx    8(%%rsi),%%rcx,%%r10       ;"
      "adcx    %%rcx,%%r9                 ;"
      "mulx    16(%%rsi),%%rcx,%%r11      ;"
      "adcx    %%rcx,%%r10                ;"
      "mulx    24(%%rsi),%%rcx,%%r12      ;"
      "adcx    %%rcx,%%r11                ;"
      "adcx    %%r13,%%r12                ;"
      "xorq    %%r14,%%r14                ;"
      "movq    8(%%rbx),%%rdx             ;"
      "mulx    0(%%rsi),%%rcx,%%rbp       ;"
      "adcx    %%rcx,%%r9                 ;"
      "adox    %%rbp,%%r10                ;"
      "mulx    8(%%rsi),%%rcx,%%rbp       ;"
      "adcx    %%rcx,%%r10                ;"
      "adox    %%rbp,%%r11                ;"
      "mulx    16(%%rsi),%%rcx,%%rbp      ;"
      "adcx    %%rcx,%%r11                ;"
      "adox    %%rbp,%%r12                ;"
      "mulx    24(%%rsi),%%rcx,%%rbp      ;"
      "adcx    %%rcx,%%r12                ;"
      "adox    %%rbp,%%r13                ;"
      "adcx    %%r14,%%r13                ;"
      "xorq    %%r15,%%r15                ;"
      "movq    16(%%rbx),%%rdx            ;"
      "mulx    0(%%rsi),%%rcx,%%rbp       ;"
      "adcx    %%rcx,%%r10                ;"
      "adox    %%rbp,%%r11                ;"
      "mulx    8(%%rsi),%%rcx,%%rbp       ;"
      "adcx    %%rcx,%%r11                ;"
      "adox    %%rbp,%%r12                ;"
      "mulx    16(%%rsi),%%rcx,%%rbp      ;"
      "adcx    %%rcx,%%r12                ;"
      "adox    %%rbp,%%r13                ;"
      "mulx    24(%%rsi),%%rcx,%%rbp      ;"
      "adcx    %%rcx,%%r13                ;"
      "adox    %%rbp,%%r14                ;"
      "adcx    %%r15,%%r14                ;"
      "xorq    %%rax,%%rax                ;"
      "movq    24(%%rbx),%%rdx            ;"
      "mulx    0(%%rsi),%%rcx,%%rbp       ;"
      "adcx    %%rcx,%%r11                ;"
      "adox    %%rbp,%%r12                ;"
      "mulx    8(%%rsi),%%rcx,%%rbp       ;"
      "adcx    %%rcx,%%r12                ;"
      "adox    %%rbp,%%r13                ;"
      "mulx    16(%%rsi),%%rcx,%%rbp      ;"
      "adcx    %%rcx,%%r13                ;"
      "adox    %%rbp,%%r14                ;"
      "mulx    24(%%rsi),%%rcx,%%rbp      ;"
      "adcx    %%rcx,%%r14                ;"
      "adox    %%rbp,%%r15                ;"
      "adcx    %%rax,%%r15                ;"
      "xorq    %%rbp,%%rbp                ;"
      "movq    $38,%%rdx                  ;"
      "mulx    %%r12,%%rax,%%r12          ;"
      "adcx    %%rax,%%r8                 ;"
      "adox    %%r12,%%r9                 ;"
      "mulx    %%r13,%%rcx,%%r13          ;"
      "adcx    %%rcx,%%r9                 ;"
      "adox    %%r13,%%r10                ;"
      "mulx    %%r14,%%rcx,%%r14          ;"
      "adcx    %%rcx,%%r10                ;"
      "adox    %%r14,%%r11                ;"
      "mulx    %%r15,%%rcx,%%r15          ;"
      "adcx    %%rcx,%%r11                ;"
      "adox    %%rbp,%%r15                ;"
      "adcx    %%rbp,%%r15                ;"
      "shld    $1,%%r11,%%r15             ;"
      "movq    $0x7fffffffffffffff, %%rax ;"
      "andq	   %%rax,%%r11                ;"
      "imul    $19,%%r15,%%r15            ;"
      "addq    %%r15,%%r8                 ;"
      "adcq    $0,%%r9                    ;"
      "adcq    $0,%%r10                   ;"
      "adcq    $0,%%r11                   ;"
      "movq    %%r8,0(%%rdi)              ;"
      "movq    %%r9,8(%%rdi)              ;"
      "movq    %%r10,16(%%rdi)            ;"
      "movq    %%r11,24(%%rdi)            ;"

      "pop %%rbp ;"
      "pop %%rbx ;"
      "pop %%r15 ;"
      "pop %%r14 ;"
      "pop %%r13 ;"
      "pop %%r12 ;"
      "pop %%r11 ;"
      "ret"
      :
      :
      :);
}

#endif

static void fe_freeze(uint64_t r[4]) {
  uint64_t r0 = r[0];
  uint64_t r1 = r[1];
  uint64_t r2 = r[2];
  uint64_t r3 = r[3];
  uint64_t t0 = r0;
  uint64_t t1 = r1;
  uint64_t t2 = r2;
  uint64_t t3 = r3;
  const uint64_t hi = uint64_t(1) << 63;

  uint64_t c = 0;
  t0 = addx(t0, 19, c);
  t1 = addx(t1, 0, c);
  t2 = addx(t2, 0, c);
  t3 = addx(t3, hi, c);
  uint64_t mask = (uint64_t)0 - ((uint64_t)c);
  t0 = r0 = MASKED_SELECT(mask, t0, r0);
  t1 = r1 = MASKED_SELECT(mask, t1, r1);
  t2 = r2 = MASKED_SELECT(mask, t2, r2);
  t3 = r3 = MASKED_SELECT(mask, t3, r3);

  c = 0;
  t0 = addx(t0, 19, c);
  t1 = addx(t1, 0, c);
  t2 = addx(t2, 0, c);
  t3 = addx(t3, hi, c);
  mask = (uint64_t)0 - ((uint64_t)c);
  r[0] = MASKED_SELECT(mask, t0, r0);
  r[1] = MASKED_SELECT(mask, t1, r1);
  r[2] = MASKED_SELECT(mask, t2, r2);
  r[3] = MASKED_SELECT(mask, t3, r3);
}

static void fe_add(uint64_t r[4], const uint64_t x[4], const uint64_t y[4]) {
  uint64_t c = 0;
  uint64_t x0 = addx(x[0], y[0], c);
  uint64_t x1 = addx(x[1], y[1], c);
  uint64_t x2 = addx(x[2], y[2], c);
  uint64_t x3 = addx(x[3], y[3], c);

  uint64_t t = constant_time_select_u64(c, 38, 0);
  c = 0;
  x0 = addx(x0, t, c);
  x1 = addx(x1, 0, c);
  x2 = addx(x2, 0, c);
  x3 = addx(x3, 0, c);

  t = constant_time_select_u64(c, t, 0);
  r[0] = x0 + t;
  r[1] = x1;
  r[2] = x2;
  r[3] = x3;
}

static void fe_sub(uint64_t r[4], const uint64_t x[4], const uint64_t y[4]) {
  uint64_t c = 0;
  uint64_t x0 = subx(x[0], y[0], c);
  uint64_t x1 = subx(x[1], y[1], c);
  uint64_t x2 = subx(x[2], y[2], c);
  uint64_t x3 = subx(x[3], y[3], c);

  uint64_t t = constant_time_select_u64(c, 38, 0);
  c = 0;
  x0 = subx(x0, t, c);
  x1 = subx(x1, 0, c);
  x2 = subx(x2, 0, c);
  x3 = subx(x3, 0, c);

  t = constant_time_select_u64(c, t, 0);
  r[0] = x0 - t;
  r[1] = x1;
  r[2] = x2;
  r[3] = x3;
}

using uint128_t = unsigned __int128;

static void fe_square(uint64_t r[4], const uint64_t x[4]) {
  uint64_t t0, t1, t2, t3, t4, t5, t6, t7, s0, s1, s2, s3, s4, s5;
  uint128_t z;
  z = uint128_t(x[1]) * x[0];
  t0 = uint64_t(z);
  t1 = uint64_t(z >> 64);
  z = uint128_t(x[2]) * x[1];
  t2 = uint64_t(z);
  t3 = uint64_t(z >> 64);
  z = uint128_t(x[3]) * x[2];
  t4 = uint64_t(z);
  t5 = uint64_t(z >> 64);
  z = uint128_t(x[2]) * x[0];
  t6 = uint64_t(z);
  t7 = uint64_t(z >> 64);
  uint64_t c = 0;
  t1 = addx(t1, t6, c);
  t2 = addx(t2, t7, c);
  t3 = addx(t3, 0, c);
  z = uint128_t(x[3]) * x[1];
  t6 = uint64_t(z);
  t7 = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, t6, c);
  t4 = addx(t4, t7, c);
  t5 = addx(t5, 0, c);
  z = uint128_t(x[3]) * x[0];
  t6 = uint64_t(z);
  t7 = uint64_t(z >> 64);
  c = 0;
  t2 = addx(t2, t6, c);
  t3 = addx(t3, t7, c);
  t4 = addx(t4, 0, c);
  t5 = addx(t5, 0, c);
  s5 = c;
  c = 0;
  t0 = addx(t0, t0, c);
  t1 = addx(t1, t1, c);
  t2 = addx(t2, t2, c);
  t3 = addx(t3, t3, c);
  t4 = addx(t4, t4, c);
  t5 = addx(t5, t5, c);
  s5 = addx(s5, s5, c);
  t6 = x[0];
  z = uint128_t(t6) * t6;
  s0 = uint64_t(z);
  s1 = uint64_t(z >> 64);
  t6 = x[1];
  z = uint128_t(t6) * t6;
  s2 = uint64_t(z);
  s3 = uint64_t(z >> 64);
  t6 = x[2];
  z = uint128_t(t6) * t6;
  t6 = uint64_t(z);
  t7 = uint64_t(z >> 64);
  c = 0;
  t0 = addx(t0, s1, c);
  t1 = addx(t1, s2, c);
  t2 = addx(t2, s3, c);
  t3 = addx(t3, t6, c);
  t4 = addx(t4, t7, c);
  t5 = addx(t5, 0, c);
  s5 = addx(s5, 0, c);
  t6 = x[3];
  z = uint128_t(t6) * t6;
  t6 = uint64_t(z);
  t7 = uint64_t(z >> 64);
  c = 0;
  t5 = addx(t5, t6, c);
  s5 = addx(s5, t7, c);
  z = uint128_t(t3) * 38;
  s4 = uint64_t(z);
  t3 = uint64_t(z >> 64);
  z = uint128_t(t4) * 38;
  t6 = uint64_t(z);
  t7 = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, t6, c);
  t4 = addx(0, t7, c);
  z = uint128_t(t5) * 38;
  t6 = uint64_t(z);
  t7 = uint64_t(z >> 64);
  c = 0;
  t4 = addx(t4, t6, c);
  t6 = s5;
  s5 = addx(0, t7, c);
  z = uint128_t(t6) * 38;
  t6 = uint64_t(z);
  t7 = uint64_t(z >> 64);
  c = 0;
  s5 = addx(s5, t6, c);
  t6 = addx(t7, 0, c);
  c = 0;
  s0 = addx(s0, s4, c);
  t0 = addx(t0, t3, c);
  t1 = addx(t1, t4, c);
  t2 = addx(t2, s5, c);
  t6 = addx(t6, 0, c);
  t7 = t6 * 38;
  c = 0;
  s0 = addx(s0, t7, c);
  t0 = addx(t0, 0, c);
  t1 = addx(t1, 0, c);
  t2 = addx(t2, 0, c);
  r[0] = s0 + c * 38;
  r[1] = t0;
  r[2] = t1;
  r[3] = t2;
}

static void fe_mul(uint64_t r[4], const uint64_t x[4], const uint64_t y[4]) {
  uint64_t c, lo, hi, s0, s1, s2, s3, t0, t1, t2, t3, t4, t5, t6, t7;
  uint128_t z;

  s0 = x[0];
  z = uint128_t(y[0]) * s0;
  t0 = uint64_t(z);
  t1 = uint64_t(z >> 64);
  z = uint128_t(y[1]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t1 = addx(t1, lo, c);
  t2 = addx(hi, 0, c);
  z = uint128_t(y[2]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t2 = addx(t2, lo, c);
  t3 = addx(hi, 0, c);
  z = uint128_t(y[3]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, lo, c);
  t4 = addx(hi, 0, c);
  s0 = x[1];
  z = uint128_t(y[0]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t1 = addx(t1, lo, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y[1]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t2 = addx(t2, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t2 = addx(t2, s3, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y[2]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t3 = addx(t3, s3, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y[3]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t4 = addx(t4, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t4 = addx(t4, s3, c);
  t5 = addx(hi, 0, c);
  s0 = x[2];
  z = uint128_t(y[0]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t2 = addx(t2, lo, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y[1]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t3 = addx(t3, s3, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y[2]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t4 = addx(t4, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t4 = addx(t4, s3, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y[3]) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t5 = addx(t5, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t5 = addx(t5, s3, c);
  t6 = addx(hi, 0, c);
  s1 = x[3];
  z = uint128_t(y[0]) * s1;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, lo, c);
  s0 = addx(hi, 0, c);
  z = uint128_t(y[1]) * s1;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t4 = addx(t4, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t4 = addx(t4, s0, c);
  s0 = addx(hi, 0, c);
  z = uint128_t(y[2]) * s1;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t5 = addx(t5, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t5 = addx(t5, s0, c);
  s0 = addx(hi, 0, c);
  z = uint128_t(y[3]) * s1;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t6 = addx(t6, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t6 = addx(t6, s0, c);
  t7 = addx(hi, 0, c);
  z = uint128_t(t4) * 38;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  s1 = lo;
  s2 = hi;
  z = uint128_t(t5) * 38;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  s2 = addx(s2, lo, c);
  t4 = addx(hi, 0, c);
  z = uint128_t(t6) * 38;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t4 = addx(t4, lo, c);
  t5 = addx(hi, 0, c);
  z = uint128_t(t7) * 38;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t5 = addx(t5, lo, c);
  lo = addx(hi, 0, c);
  c = 0;
  t0 = addx(t0, s1, c);
  t1 = addx(t1, s2, c);
  t2 = addx(t2, t4, c);
  t3 = addx(t3, t5, c);
  lo = addx(lo, 0, c);
  hi = lo * 38;
  c = 0;
  t0 = addx(t0, hi, c);
  t1 = addx(t1, 0, c);
  t2 = addx(t2, 0, c);
  t3 = addx(t3, 0, c);
  r[0] = t0 + c * 38;
  r[1] = t1;
  r[2] = t2;
  r[3] = t3;
}

struct fe_t : public bn256_t {
  static fe_t zero() {
    fe_t r;
    r.set(0);
    return r;
  }
  static fe_t one() {
    fe_t r;
    r.set(1);
    return r;
  }

  fe_t operator+(const fe_t& b) const {
    fe_t r;
    add(r, *this, b);
    return r;
  }
  fe_t operator-(const fe_t& b) const {
    fe_t r;
    sub(r, *this, b);
    return r;
  }

  fe_t& operator+=(const fe_t& b) { return *this = *this + b; }
  fe_t& operator-=(const fe_t& b) { return *this = *this - b; }

  fe_t operator*(const fe_t& b) const {
    fe_t r;
    if (this == &b)
      sqr(r, b);
    else
      mul(r, *this, b);
    return r;
  }

  fe_t& operator*=(const fe_t& b) { return *this = *this * b; }

  static void add(fe_t& r, const fe_t& a, const fe_t& b) { fe_add(r.d, a.d, b.d); }

  static void sub(fe_t& r, const fe_t& a, const fe_t& b) { fe_sub(r.d, a.d, b.d); }

  static void sqr(fe_t& r, const fe_t& a) {
#ifdef __x86_64__
    if (g_intel_mulx) {
      fe_square_mulx(r.d, a.d);
      return;
    }
#endif
    fe_square(r.d, a.d);
  }

  static void mul(fe_t& r, const fe_t& a, const fe_t& b) {
#ifdef __x86_64__
    if (g_intel_mulx) {
      fe_mul_mulx(r.d, a.d, b.d);
      return;
    }
#endif
    fe_mul(r.d, a.d, b.d);
  }

  fe_t operator-() const { return zero() - *this; }

  static void sub(fe_t& r, const fe_t& a) { sub(r, r, a); }
  static void add(fe_t& r, const fe_t& a) { add(r, r, a); }
  static void mul(fe_t& r, const fe_t& a) { mul(r, r, a); }
  static void sqr(fe_t& r) { sqr(r, r); }

  static void times_x3(fe_t& r, const fe_t& a) {
    add(r, a, a);
    add(r, r, a);
  }
  static void times_x3(fe_t& r) {
    fe_t t = r;
    add(r, t);
    add(r, t);
  }
  static void times_x4(fe_t& r, const fe_t& a) {
    add(r, a, a);
    add(r, r);
  }
  static void times_x8(fe_t& r, const fe_t& a) {
    add(r, a, a);
    add(r, r);
    add(r, r);
  }
  static void times_x8(fe_t& r) { times_x8(r, r); }

  template <arch_e arch>
  static void mul_arch(fe_t& r, const fe_t& a) {
    mul_arch<arch>(r, r, a);
  }
  template <arch_e arch>
  static void sqr_arch(fe_t& r) {
    sqr_arch<arch>(r, r);
  }

  template <arch_e arch>
  static void mul_arch(fe_t& r, const fe_t& a, const fe_t& b) {
#ifdef __x86_64__
    if constexpr (arch == arch_e::intel_mulx) {
      fe_mul_mulx(r.d, a.d, b.d);
      return;
    }
#endif
    fe_mul(r.d, a.d, b.d);
  }

  template <arch_e arch>
  static void sqr_arch(fe_t& r, const fe_t& a) {
#ifdef __x86_64__
    if constexpr (arch == arch_e::intel_mulx) {
      fe_square_mulx(r.d, a.d);
      return;
    }
#endif
    fe_square(r.d, a.d);
  }

  bn256_t from_fe() const {
    bn256_t r = bn256_t(*this);
    fe_freeze(r.d);
    return r;
  }

  static fe_t to_fe(const bn256_t& a) {
    fe_t r;
    r.d[0] = a.d[0];
    r.d[1] = a.d[1];
    r.d[2] = a.d[2];
    r.d[3] = a.d[3];
    return r;
  }
  static fe_t to_fe(int a) {
    fe_t r;
    r.d[0] = a;
    r.d[1] = r.d[2] = r.d[3] = 0;
    return r;
  }

  bool operator==(const fe_t& b) const { return this->from_fe() == b.from_fe(); }

  bool operator!=(const fe_t& b) const { return !(*this == b); }

  bool is_zero() const { return from_fe().is_zero(); }

  bool is_odd() const { return from_fe().is_odd(); }

  template <arch_e arch>
  static void invert_arch(fe_t& r, const fe_t& x) {
    fe_t z2;
    fe_t z9;
    fe_t z11;
    fe_t z2_5_0;
    fe_t z2_10_0;
    fe_t z2_20_0;
    fe_t z2_50_0;
    fe_t z2_100_0;
    fe_t t;
    int i;

    /* 2 */ sqr_arch<arch>(z2, x);
    /* 4 */ sqr_arch<arch>(t, z2);
    /* 8 */ sqr_arch<arch>(t, t);
    /* 9 */ mul_arch<arch>(z9, t, x);
    /* 11 */ mul_arch<arch>(z11, z9, z2);
    /* 22 */ sqr_arch<arch>(t, z11);
    /* 2^5 - 2^0 = 31 */ mul_arch<arch>(z2_5_0, t, z9);

    /* 2^6 - 2^1 */ sqr_arch<arch>(t, z2_5_0);
    /* 2^20 - 2^10 */ for (i = 1; i < 5; i++) { sqr_arch<arch>(t, t); }
    /* 2^10 - 2^0 */ mul_arch<arch>(z2_10_0, t, z2_5_0);

    /* 2^11 - 2^1 */ sqr_arch<arch>(t, z2_10_0);
    /* 2^20 - 2^10 */ for (i = 1; i < 10; i++) { sqr_arch<arch>(t, t); }
    /* 2^20 - 2^0 */ mul_arch<arch>(z2_20_0, t, z2_10_0);

    /* 2^21 - 2^1 */ sqr_arch<arch>(t, z2_20_0);
    /* 2^40 - 2^20 */ for (i = 1; i < 20; i++) { sqr_arch<arch>(t, t); }
    /* 2^40 - 2^0 */ mul_arch<arch>(t, t, z2_20_0);

    /* 2^41 - 2^1 */ sqr_arch<arch>(t, t);
    /* 2^50 - 2^10 */ for (i = 1; i < 10; i++) { sqr_arch<arch>(t, t); }
    /* 2^50 - 2^0 */ mul_arch<arch>(z2_50_0, t, z2_10_0);

    /* 2^51 - 2^1 */ sqr_arch<arch>(t, z2_50_0);
    /* 2^100 - 2^50 */ for (i = 1; i < 50; i++) { sqr_arch<arch>(t, t); }
    /* 2^100 - 2^0 */ mul_arch<arch>(z2_100_0, t, z2_50_0);

    /* 2^101 - 2^1 */ sqr_arch<arch>(t, z2_100_0);
    /* 2^200 - 2^100 */ for (i = 1; i < 100; i++) { sqr_arch<arch>(t, t); }
    /* 2^200 - 2^0 */ mul_arch<arch>(t, t, z2_100_0);

    /* 2^201 - 2^1 */ sqr_arch<arch>(t, t);
    /* 2^250 - 2^50 */ for (i = 1; i < 50; i++) { sqr_arch<arch>(t, t); }
    /* 2^250 - 2^0 */ mul_arch<arch>(t, t, z2_50_0);

    /* 2^251 - 2^1 */ sqr_arch<arch>(t, t);
    /* 2^252 - 2^2 */ sqr_arch<arch>(t, t);
    /* 2^253 - 2^3 */ sqr_arch<arch>(t, t);

    /* 2^254 - 2^4 */ sqr_arch<arch>(t, t);

    /* 2^255 - 2^5 */ sqr_arch<arch>(t, t);
    /* 2^255 - 21 */ mul_arch<arch>(r, t, z11);
  }

  fe_t inv() const {
    fe_t r;
#ifdef __x86_64__
    if (g_intel_mulx)
      invert_arch<arch_e::intel_mulx>(r, *this);
    else
#endif
      invert_arch<arch_e::regular>(r, *this);
    return r;
  }

  fe_t pow22523() const {
#ifdef __x86_64__
    if (g_intel_mulx)
      return pow22523_arch<arch_e::intel_mulx>();
    else
#endif
      return pow22523_arch<arch_e::regular>();
  }

  // pow22523 returns x^((p-5)/8) where (p-5)/8 is 2^252-3.
  template <arch_e arch>
  fe_t pow22523_arch() const {
    const fe_t& z = *this;
    fe_t t0, t1, t2, out;

    sqr_arch<arch>(t0, z);
    sqr_arch<arch>(t1, t0);
    for (int i = 1; i < 2; ++i) sqr_arch<arch>(t1, t1);
    mul_arch<arch>(t1, z, t1);
    mul_arch<arch>(t0, t0, t1);
    sqr_arch<arch>(t0, t0);
    mul_arch<arch>(t0, t1, t0);
    sqr_arch<arch>(t1, t0);
    for (int i = 1; i < 5; ++i) sqr_arch<arch>(t1, t1);
    mul_arch<arch>(t0, t1, t0);
    sqr_arch<arch>(t1, t0);
    for (int i = 1; i < 10; ++i) sqr_arch<arch>(t1, t1);
    mul_arch<arch>(t1, t1, t0);
    sqr_arch<arch>(t2, t1);
    for (int i = 1; i < 20; ++i) sqr_arch<arch>(t2, t2);
    mul_arch<arch>(t1, t2, t1);
    sqr_arch<arch>(t1, t1);
    for (int i = 1; i < 10; ++i) sqr_arch<arch>(t1, t1);
    mul_arch<arch>(t0, t1, t0);
    sqr_arch<arch>(t1, t0);
    for (int i = 1; i < 50; ++i) sqr_arch<arch>(t1, t1);
    mul_arch<arch>(t1, t1, t0);
    sqr_arch<arch>(t2, t1);
    for (int i = 1; i < 100; ++i) sqr_arch<arch>(t2, t2);
    mul_arch<arch>(t1, t2, t1);
    sqr_arch<arch>(t1, t1);
    for (int i = 1; i < 50; ++i) sqr_arch<arch>(t1, t1);
    mul_arch<arch>(t0, t1, t0);
    sqr_arch<arch>(t0, t0);
    for (int i = 1; i < 2; ++i) sqr_arch<arch>(t0, t0);
    mul_arch<arch>(out, t0, z);
    return out;
  }
};

static fe_t d = fe_t::to_fe(bn256_t::from_hex("52036cee2b6ffe738cc740797779e89800700a4d4141d8ab75eb4dca135978a3"));
static fe_t gx = fe_t::to_fe(bn256_t::from_hex("216936D3CD6E53FEC0A4E231FDD6DC5C692CC7609525A7B2C9562D608F25D51A"));
static fe_t gy = fe_t::to_fe(bn256_t::from_hex("6666666666666666666666666666666666666666666666666666666666666658"));
static fe_t fe_one = fe_t::to_fe(1);

static bool equ(const fe_t& ax, const fe_t& ay, const fe_t& az, const fe_t& bx, const fe_t& by, const fe_t& bz) {
  fe_t ta, tb;
  fe_t::mul(ta, ax, bz);
  fe_t::mul(tb, bx, az);
  if (ta != tb) return false;

  fe_t::mul(ta, ay, bz);
  fe_t::mul(tb, by, az);
  if (ta != tb) return false;

  return true;
}

static void get_affine_xy(const fe_t& x, const fe_t& y, const fe_t& z, fe_t& affine_x, fe_t& affine_y) {
  fe_t zi = z.inv();
  affine_x = x * zi;
  affine_y = y * zi;
}

static bool is_on_curve(const fe_t& x, const fe_t& y) {
  fe_t xx = x * x;
  fe_t yy = y * y;

  fe_t t = yy;
  t -= yy;  // a == -1

  return t == fe_one + d * xx * yy;
}

static bool is_on_curve(const fe_t& x, const fe_t& y, const fe_t& z) {
  fe_t xx = x * x;
  fe_t yy = y * y;
  fe_t zz = z * z;

  fe_t t = yy;
  t -= xx;  // a == -1

  return t * zz == zz * zz + xx * yy * d;
}

template <arch_e arch>
static void dbl_arch(fe_t& rx, fe_t& ry, fe_t& rz, const fe_t& x, const fe_t& y, const fe_t& z) {
  //      if (p.is_infinity()) { r = p; return; }

  fe_t tb, tc, td, te, tf;
  fe_t::template sqr_arch<arch>(tc, x);  // C = X_1^2
  fe_t::template sqr_arch<arch>(td, y);  // D = Y_1^2

  fe_t::sub(tf, td, tc);  // F = E + D , because a = -1

  fe_t::template sqr_arch<arch>(te, z);  // H = Z_1^2
  fe_t::sub(tb, tf, te);
  fe_t::sub(tb, te);  // J = F - 2H

  fe_t::add(rx, x, y);
  fe_t::template sqr_arch<arch>(rx, rx);  // B = (X_1 + Y_1)^2
  fe_t::sub(rx, tc);
  fe_t::sub(rx, td);
  fe_t::template mul_arch<arch>(rx, tb);  // X_3 = (B - C - D) * J

  te = -tc;  // E = aC, a = -1
  fe_t::sub(ry, te, td);
  fe_t::template mul_arch<arch>(ry, tf);      // Y_3 = F * (aC - D)
  fe_t::template mul_arch<arch>(rz, tf, tb);  // Z_3 = F * J
}

template <arch_e arch>
static void add_arch(fe_t& rx, fe_t& ry, fe_t& rz, const fe_t& ax, const fe_t& ay, const fe_t& az, const fe_t& bx,
                     const fe_t& by, const fe_t& bz) {
  bool a_is_inf = ax.is_zero();  // a.is_infinity();
  bool b_is_inf = bx.is_zero();  // b.is_infinity();

  //    point_t aa = a;
  fe_t save_ax = ax;
  fe_t save_ay = ay;
  fe_t save_az = az;

  fe_t ta;
  fe_t::template mul_arch<arch>(ta, az, bz);  // A = Z1 * Z2
  fe_t tb;
  fe_t::template sqr_arch<arch>(tb, ta);  // B = A^2
  fe_t tc;
  fe_t::template mul_arch<arch>(tc, ax, bx);  // C = X1 * X2
  fe_t td;
  fe_t::template mul_arch<arch>(td, ay, by);  // D = Y1 * Y2

  fe_t te;
  fe_t::template mul_arch<arch>(te, d, tc);
  fe_t::template mul_arch<arch>(te, td);  // E = d * C * D

  fe_t tf;
  fe_t::sub(tf, tb, te);  // F = B - E
  fe_t::add(te, tb);      // G = B + E

  fe_t::add(tb, ax, ay);
  fe_t::add(rx, bx, by);
  fe_t::template mul_arch<arch>(rx, tb);
  fe_t::sub(rx, tc);
  fe_t::sub(rx, td);
  fe_t::template mul_arch<arch>(rx, tf);
  fe_t::template mul_arch<arch>(rx, ta);  // X_3 = A * F * ((X_1 + Y_1) * (X_2 + Y_2) - C - D)

  fe_t::add(ry, td, tc);  // because a==-1

  fe_t::template mul_arch<arch>(ry, te);
  fe_t::template mul_arch<arch>(ry, ta);  // Y_3 = A * G * (D - aC)

  fe_t::template mul_arch<arch>(rz, tf, te);  // Z_3 = F * G

  // cnd_move(r, a_is_inf, b);
  fe_t::cnd_move(rx, a_is_inf, bx);
  fe_t::cnd_move(ry, a_is_inf, by);
  fe_t::cnd_move(rz, a_is_inf, bz);

  // cnd_move(r, b_is_inf, aa);
  fe_t::cnd_move(rx, b_is_inf, save_ax);
  fe_t::cnd_move(ry, b_is_inf, save_ay);
  fe_t::cnd_move(rz, b_is_inf, save_az);
}

#ifdef EXTENDED_COORD

template <arch_e arch>
static void add_ext_precomp_cached_arch(fe_t& rx, fe_t& ry, fe_t& rz, fe_t& rt, const fe_t& y_minus_x,
                                        const fe_t& y_plus_x, const fe_t& kt) {
  fe_t a, b, c, d, e, f, g, h;

  fe_t::sub(a, ry, rx);
  fe_t::template mul_arch<arch>(a, a, y_minus_x);

  fe_t::add(b, ry, rx);
  fe_t::template mul_arch<arch>(b, b, y_plus_x);

  fe_t::template mul_arch<arch>(c, rt, kt);

  fe_t::add(d, rz, rz);
  fe_t::sub(e, b, a);
  fe_t::sub(f, d, c);
  fe_t::add(g, d, c);
  fe_t::add(h, b, a);

  fe_t::template mul_arch<arch>(rx, e, f);
  fe_t::template mul_arch<arch>(ry, g, h);
  fe_t::template mul_arch<arch>(rt, e, h);
  fe_t::template mul_arch<arch>(rz, f, g);
}
#else
template <arch_e arch>
static void add_affine_arch(fe_t& X3, fe_t& Y3, fe_t& Z3, const fe_t& X2, const fe_t& Y2) {
  const fe_t& X1 = X3;
  const fe_t& Y1 = Y3;
  const fe_t& Z1 = Z3;
  bool a_is_inf = X1.is_zero();

  fe_t B, C, D, E, F, G, H;
  fe_t::template sqr_arch<arch>(B, Z1);      // B = Z1^2
  fe_t::template mul_arch<arch>(C, X1, X2);  // C = X1*X2
  fe_t::template mul_arch<arch>(D, Y1, Y2);  // D = Y1*Y2
  fe_t::template mul_arch<arch>(E, d, C);
  fe_t::template mul_arch<arch>(E, D);  // E = d*C*D
  fe_t::sub(F, B, E);                   // F = B-E
  fe_t::add(G, B, E);                   // G = B+E
  fe_t::add(H, X1, Y1);                 // H = X1+Y1
  fe_t::add(X3, X2, Y2);
  fe_t::template mul_arch<arch>(X3, H);
  fe_t::sub(X3, C);
  fe_t::sub(X3, D);
  fe_t::template mul_arch<arch>(X3, Z1);
  fe_t::template mul_arch<arch>(X3, F);  // X3 = Z1*F*((X1+Y1)*(X2+Y2)-C-D)
  fe_t::add(Y3, D, C);                   // a==-1
  fe_t::template mul_arch<arch>(Y3, Z1);
  fe_t::template mul_arch<arch>(Y3, G);     // Y3 = Z1*G*(D-a*C)
  fe_t::template mul_arch<arch>(Z3, F, G);  // Z3 = F*G

  fe_t::cnd_move(X3, a_is_inf, X2);
  fe_t::cnd_move(Y3, a_is_inf, Y2);
  fe_t::cnd_move(Z3, a_is_inf, fe_one);
}
#endif

class point_t {
 private:
#ifdef EXTENDED_COORD
  struct precomp_entry_cached_t {
    fe_t y_minus_x, y_plus_x, kt;
  };
  struct extended_t {
    fe_t x, y, z, t;
  };
  using precomp_entry_t = precomp_entry_cached_t;

#else
  struct alignas(64) precomp_entry_t {
    fe_t x, y;
  };
#endif

 public:
  void set(const fe_t& _x, const fe_t& _y) {
    x = _x;
    y = _y;
    z = fe_one;
  }

  bool is_infinity() const { return x.is_zero(); }

  static bool is_on_curve(const fe_t& x, const fe_t& y) { return ec25519_core::is_on_curve(x, y); }

  static point_t infinity() {
    point_t r;
    r.set_infinity();
    return r;
  }

  bool is_on_curve() const { return ec25519_core::is_on_curve(x, y, z); }

  bool set_xy(const bn256_t& x_coord, const bn256_t& y_coord) {
    fe_t x = fe_t::to_fe(x_coord);
    fe_t y = fe_t::to_fe(y_coord);
    if (!is_on_curve(x, y)) return false;
    this->x = x;
    this->y = y;
    this->z = fe_one;
    return true;
  }

  void set_infinity() { x = y = z = fe_t::zero(); }

  void get_xy(fe_t& x_affine, fe_t& y_affine) const {
    if (z == fe_one) {
      x_affine = x;
      y_affine = y;
      return;
    }

    ec25519_core::get_affine_xy(x, y, z, x_affine, y_affine);
  }

  void get_xy(bn256_t& x_coord, bn256_t& y_coord) const {
    fe_t fe_x, fe_y;
    get_xy(fe_x, fe_y);
    x_coord = fe_x.from_fe();
    y_coord = fe_y.from_fe();
  }

  static void neg(point_t& r, const point_t& a) {
    r.x = -a.x;
    r.y = a.y;
    r.z = a.z;
  }

  static void cnd_move(point_t& r, bool flag, const point_t& a) {
    fe_t::cnd_move(r.x, flag, a.x);
    fe_t::cnd_move(r.y, flag, a.y);
    fe_t::cnd_move(r.z, flag, a.z);
  }

  static bool equ(const point_t& a, const point_t& b) {
    if (a.is_infinity()) return b.is_infinity();

    const fe_t& ax = a.x;
    const fe_t& ay = a.y;
    const fe_t& az = a.z;

    const fe_t& bx = b.x;
    const fe_t& by = b.y;
    const fe_t& bz = b.z;

    bool az_is_one = az == fe_one;
    bool bz_is_one = bz == fe_one;

    if (az_is_one && bz_is_one) return ax == bx && ay == by;

    return ec25519_core::equ(a.x, a.y, a.z, b.x, b.y, b.z);
  }

  template <arch_e arch>
  static void dbl_arch(point_t& r, const point_t& a) {
    ec25519_core::template dbl_arch<arch>(r.x, r.y, r.z, a.x, a.y, a.z);
  }

  static void dbl(point_t& r, const point_t& a) {
#ifdef __x86_64__
    if (g_intel_mulx)
      dbl_arch<arch_e::intel_mulx>(r, a);
    else
#endif
      dbl_arch<arch_e::regular>(r, a);
  }

  static void dbl(point_t& r) { dbl(r, r); }

  template <arch_e arch>
  static void add_arch(point_t& r, const point_t& a, const point_t& b) {
    ec25519_core::template add_arch<arch>(r.x, r.y, r.z, a.x, a.y, a.z, b.x, b.y, b.z);
  }

  static void add(point_t& r, const point_t& a, const point_t& b) {
#ifdef __x86_64__
    if (g_intel_mulx)
      add_arch<arch_e::intel_mulx>(r, a, b);
    else
#endif
      add_arch<arch_e::regular>(r, a, b);
  }

  static void add(point_t& r, const point_t& a) { add(r, r, a); }

  point_t operator+(const point_t& b) const {
    point_t r;
    if (this == &b)
      dbl(r, *this);
    else
      add(r, *this, b);
    return r;
  }
  point_t operator-() const {
    point_t r;
    neg(r, *this);
    return r;
  }
  point_t operator-(const point_t& b) const { return *this + (-b); }

  point_t& operator+=(const point_t& b) { return *this = *this + b; }
  point_t& operator-=(const point_t& b) { return *this = *this - b; }

  bool operator==(const point_t& b) const { return equ(*this, b); }
  bool operator!=(const point_t& b) const { return !equ(*this, b); }

  static point_t ct_get(const point_t* table, int line_size, unsigned index) {
    // return table[index];
    point_t R;

#ifdef INTEL_X64
    ct_get3((__m128i*)&R, (const __m128i*)table, line_size, index);
#else
    R.set_infinity();
    for (unsigned i = 1; i < line_size; i++) {
      table++;
      bool flag = index == i;
      fe_t::cnd_move(R.x, flag, table->x);
      fe_t::cnd_move(R.y, flag, table->y);
      fe_t::cnd_move(R.z, flag, table->z);
    }
#endif
    return R;
  }

  static void cnd_neg(bool flag, point_t& p) {
    fe_t neg_x = -p.x;
    fe_t::cnd_move(p.x, flag, neg_x);
  }

  point_t mul(const bn256_t& e) const {
    const point_t& a = *this;
    const int tab_size = 17;
    alignas(64) point_t row[tab_size];
    row[0].set_infinity();
    row[1] = a;
    dbl(row[2], row[1]);
    add(row[3], row[2], a);
    dbl(row[4], row[2]);
    add(row[5], row[4], a);
    dbl(row[6], row[3]);
    add(row[7], row[6], a);
    dbl(row[8], row[4]);
    add(row[9], row[8], a);
    dbl(row[10], row[5]);
    add(row[11], row[10], a);
    dbl(row[12], row[6]);
    add(row[13], row[12], a);
    dbl(row[14], row[7]);
    add(row[15], row[14], a);
    dbl(row[16], row[8]);

    const int win = 5;
    booth_wnaf_t wnaf(win, e.d, 256, true);

    unsigned value;
    bool neg;
    wnaf.get(value, neg);
    point_t r = ct_get(row, tab_size, value);
    cnd_neg(neg, r);

    while (wnaf.get(value, neg)) {
      for (int i = 0; i < win; i++) dbl(r);
      point_t t = ct_get(row, tab_size, value);
      cnd_neg(neg, t);
      add(r, t);
    }
    return r;
  }

  enum {
    mulg_win = 6,
    mulg_line = 1 << (mulg_win - 1),
  };

#ifdef EXTENDED_COORD
  static void select_precomp_cached(unsigned index, precomp_entry_cached_t& dst,
                                    const precomp_entry_cached_t* precomp) {
    precomp++;
    index--;
#ifdef __x86_64__
    ct_get3((__m128i*)&dst, (const __m128i*)precomp, mulg_line, index);
#else
    for (unsigned i = 0; i < mulg_line; i++, precomp++) {
      bool flag = index == i;
      fe_t::cnd_move(dst.y_minus_x, flag, precomp->y_minus_x);
      fe_t::cnd_move(dst.y_plus_x, flag, precomp->y_plus_x);
      fe_t::cnd_move(dst.kt, flag, precomp->kt);
    }
#endif
  }

  static void set_zero_extended(extended_t& ext) {
    ext.x = fe_t::zero();
    ext.y = fe_t::one();
    ext.z = fe_t::one();
    ext.t = fe_t::zero();
  }

  static void set_precomp_cached(precomp_entry_cached_t& precomp, const fe_t& x, const fe_t& y) {
    precomp.y_minus_x = y - x;
    precomp.y_plus_x = y + x;
    fe_t t = y * x;
    precomp.kt = (d + d) * t;
  }

  void from_extended(const extended_t& ext) {
    x = ext.x;
    y = ext.y;
    z = ext.z;
  }

  template <arch_e arch>
  static void add_ext_precomp_cached_arch(extended_t& r, const precomp_entry_cached_t& p) {
    ec25519_core::template add_ext_precomp_cached_arch<arch>(r.x, r.y, r.z, r.t, p.y_minus_x, p.y_plus_x, p.kt);
  }

  static void add_ext_precomp_cached(extended_t& r, const precomp_entry_cached_t& a) {
#ifdef __x86_64__
    if (g_intel_mulx)
      add_ext_precomp_cached_arch<arch_e::intel_mulx>(r, a);
    else
#endif
      add_ext_precomp_cached_arch<arch_e::regular>(r, a);
  }

  static void cnd_neg(bool flag, precomp_entry_cached_t& p) {
    fe_t neg_y_minus_x = p.y_plus_x;
    fe_t neg_y_plus_x = p.y_minus_x;
    fe_t neg_kt = -p.kt;
    fe_t::cnd_move(p.y_minus_x, flag, neg_y_minus_x);
    fe_t::cnd_move(p.y_plus_x, flag, neg_y_plus_x);
    fe_t::cnd_move(p.kt, flag, neg_kt);
  }

  static precomp_entry_cached_t* precompute_from(const fe_t& gx, const fe_t& gy) {
    point_t g;
    g.x = gx;
    g.y = gy;
    g.z = fe_t::one();
    const int n = (253 + mulg_win - 1) / mulg_win;

    precomp_entry_cached_t* precomp = new precomp_entry_cached_t[1 + n * mulg_line];
    precomp[0].y_plus_x = fe_t::zero();
    precomp[0].y_minus_x = fe_t::zero();
    precomp[0].kt = fe_t::zero();

    point_t base = g;
    precomp_entry_cached_t* precomp_line = precomp + 1;

    for (int i = 0; i < n; i++, precomp_line += mulg_line) {
      point_t row = base;
      for (int j = 0; j < mulg_line; j++) {
        fe_t x, y;
        row.get_xy(x, y);  // precomp_line[j].x, precomp_line[j].y);
        set_precomp_cached(precomp_line[j], x, y);
        add(row, base);
      }

      for (int j = 0; j < mulg_win; j++) dbl(base);
    }
    return precomp;
  }

  static point_t mul_to_generator(const bn_t& e) {
    const precomp_entry_cached_t* precomp_line = precomp;

    extended_t q;
    set_zero_extended(q);

    crypto::booth_wnaf_t wnaf(mulg_win, e, 253);
    bool is_neg;
    unsigned ind;

    while (wnaf.get(ind, is_neg)) {
      precomp_entry_cached_t pre;
      select_precomp_cached(ind, pre, precomp_line);
      precomp_line += mulg_line;

      cnd_neg(is_neg, pre);

      extended_t save = q;
      add_ext_precomp_cached(q, pre);

      fe_t::cnd_move(q.x, ind == 0, save.x);
      fe_t::cnd_move(q.y, ind == 0, save.y);
      fe_t::cnd_move(q.z, ind == 0, save.z);
      fe_t::cnd_move(q.t, ind == 0, save.t);
    }

    point_t P;
    P.from_extended(q);
    return P;
  }
#else

  static void precomp_to_point(point_t& r, const precomp_entry_t& pre) {
    r.x = pre.x;
    r.y = pre.y;
    r.z = fe_one;
  }

  template <arch_e arch>
  static void add_affine_arch(point_t& r, const precomp_entry_t& a) {
    ec25519_core::template add_affine_arch<arch>(r.x, r.y, r.z, a.x, a.y);
  }

  static void add_affine(point_t& r, const precomp_entry_t& a) {
#ifdef __x86_64__
    if (g_intel_mulx)
      add_affine_arch<arch_e::intel_mulx>(r, a);
    else
#endif
      add_affine_arch<arch_e::regular>(r, a);
  }
  static void select_precomp(unsigned index, precomp_entry_t& dst, const precomp_entry_t* precomp) {
    // dst = precomp[index];
#ifdef __x86_64__
    __m128i xLo, xHi, yLo, yHi;
    xLo = xHi = yLo = yHi = _mm_setzero_si128();
    const __m128i* ptr = (const __m128i*)precomp;
    for (unsigned i = 1; i < mulg_line + 1; i++) {
      ptr += 4;
      __m128i mask = _mm_set1_epi32(-(index == i));
      xLo = _mm_or_si128(xLo, _mm_and_si128(mask, _mm_load_si128(ptr + 0)));
      xHi = _mm_or_si128(xHi, _mm_and_si128(mask, _mm_load_si128(ptr + 1)));
      yLo = _mm_or_si128(yLo, _mm_and_si128(mask, _mm_load_si128(ptr + 2)));
      yHi = _mm_or_si128(yHi, _mm_and_si128(mask, _mm_load_si128(ptr + 3)));
    }
    __m128i* dst_ptr = (__m128i*)&dst;
    _mm_storeu_si128(dst_ptr + 0, xLo);
    _mm_storeu_si128(dst_ptr + 1, xHi);
    _mm_storeu_si128(dst_ptr + 2, yLo);
    _mm_storeu_si128(dst_ptr + 3, yHi);
#else
    for (unsigned i = 1; i < mulg_line + 1; i++) {
      precomp++;
      bool flag = index == i;
      fe_t::cnd_move(dst.x, flag, precomp->x);
      fe_t::cnd_move(dst.y, flag, precomp->y);
    }
#endif
  }

  static point_t mul_to_generator(const bn_t& e) {
    point_t q;
    q.set_infinity();

    const precomp_entry_t* precomp_line = precomp;

    crypto::booth_wnaf_t wnaf(mulg_win, e, 253);
    bool is_neg;
    unsigned ind;
    wnaf.get(ind, is_neg);

    precomp_entry_t pre;
    select_precomp(ind, pre, precomp_line);
    precomp_line += mulg_line;

    precomp_to_point(q, pre);
    bool q_is_inf = ind == 0;
    fe_t::cnd_move(q.z, q_is_inf, fe_t::zero());

    fe_t neg = -q.x;
    fe_t::cnd_move(q.x, is_neg, neg);

    while (wnaf.get(ind, is_neg)) {
      select_precomp(ind, pre, precomp_line);
      precomp_line += mulg_line;

      neg = -pre.x;
      fe_t::cnd_move(pre.x, is_neg, neg);
      point_t save = q;
      add_affine(q, pre);

      cnd_move(q, ind == 0, save);
      fe_t::cnd_move(q.x, q_is_inf, pre.x);
      fe_t::cnd_move(q.y, q_is_inf, pre.y);
      fe_t::cnd_move(q.z, q_is_inf, fe_one);

      q_is_inf &= (ind == 0);
    }
    return q;
  }

  static precomp_entry_t* precompute_from(const fe_t& gx, const fe_t& gy) {
    point_t g;
    g.x = gx;
    g.y = gy;
    g.z = fe_t::to_fe(1);
    const int n = (253 + mulg_win - 1) / mulg_win;

    precomp_entry_t* precomp = new precomp_entry_t[1 + n * mulg_line];
    precomp[0].x = fe_t::to_fe(0);
    precomp[0].y = fe_t::to_fe(0);

    point_t base = g;
    precomp_entry_t* precomp_line = precomp + 1;

    for (int i = 0; i < n; i++, precomp_line += mulg_line) {
      point_t row = base;
      for (int j = 0; j < mulg_line; j++) {
        row.get_xy(precomp_line[j].x, precomp_line[j].y);
        add(row, base);
      }

      for (int j = 0; j < mulg_win; j++) dbl(base);
    }
    return precomp;
  }
#endif

  buf_t to_bin() const {
    buf_t r(32);
    to_bin(r.data());
    return r;
  }

  void to_bin(byte_ptr r) const {
    if (is_infinity()) {
      r[0] = 1;
      memset(r + 1, 0, 31);
      return;
    }

    bn256_t x, y;
    get_xy(x, y);
    y.to_bin(r);
    mem_t(r, 32).reverse();
    r[31] ^= (x.d[0] & 1) << 7;
  }

  error_t from_bin(mem_t bin) {
    if (bin.size != 32) return coinbase::error(E_FORMAT);

    buf_t buf = bin.rev();
    uint8_t neg = buf[0] >> 7;
    buf[0] &= 0x7f;
    y = fe_t::to_fe(bn256_t::from_bin(buf));

    // x² = (y² - 1) / (dy² + 1)

    fe_t u, v, w, vxx, check;

    u = y * y;
    v = u * d;
    u -= fe_one;       // u = y^2-1
    v += fe_one;       // v = dy^2+1
    w = u * v;         // w = u*v
    x = w.pow22523();  // x = w^((q-5)/8)
    x *= u;            // x = u * w^((q-5)/8)

    vxx = x * x;
    vxx *= v;
    check = vxx - u;  // vx^2-u
    if (!check.is_zero()) {
      check = vxx + u;  // vx^2+u
      if (!check.is_zero()) {
        return coinbase::error(E_CRYPTO);
      }
      static const fe_t sqrtm1 =
          fe_t::to_fe(bn256_t::from_hex("2b8324804fc1df0b2b4d00993dfbd7a72f431806ad2fe478c4ee1b274a0ea0b0"));
      x *= sqrtm1;
    }

    bn256_t x_val = x.from_fe();
    if (neg != (x_val.d[0] & 1)) x = -x;

    z = fe_one;
    return SUCCESS;
  }

 private:
  static precomp_entry_t* precompute() {
#ifdef __x86_64__
    g_intel_mulx = support_x64_mulx();
#endif

    d = fe_t::to_fe(bn256_t::from_hex("52036cee2b6ffe738cc740797779e89800700a4d4141d8ab75eb4dca135978a3"));
    gx = fe_t::to_fe(bn256_t::from_hex("216936D3CD6E53FEC0A4E231FDD6DC5C692CC7609525A7B2C9562D608F25D51A"));
    gy = fe_t::to_fe(bn256_t::from_hex("6666666666666666666666666666666666666666666666666666666666666658"));
    fe_one = fe_t::to_fe(1);

    return precompute_from(gx, gy);
  }

  static inline const precomp_entry_t* precomp = precompute();

  fe_t x, y, z;
};

point_t operator*(const bn_t& x, const point_t& P) { return P.mul(bn256_t::from_bn(x)); }

static point_t set_generator() {
  point_t G;
  G.set(gx, gy);
  cb_assert(G.is_on_curve());
  return G;
}

const point_t& get_generator() {
  static point_t G = set_generator();
  return G;
}

const mod_t& get_order() {
  static const mod_t order = bn_t::from_hex("1000000000000000000000000000000014DEF9DEA2F79CD65812631A5CF5D3ED");
  return order;
}

bool is_on_curve(const point_t* a) { return a->is_on_curve(); }
bool set_xy(point_t* r, const bn_t& x, const bn_t& y) { return r->set_xy(bn256_t::from_bn(x), bn256_t::from_bn(y)); }
void get_xy(const point_t* a, bn_t& x, bn_t& y) {
  bn256_t x_coord, y_coord;
  a->get_xy(x_coord, y_coord);
  x = x_coord.to_bn();
  y = y_coord.to_bn();
}
bool is_infinity(const point_t* a) { return a->is_infinity(); }
void set_infinity(point_t* r) { r->set_infinity(); }
void mul(point_t* r, const point_t* a, const bn_t& x) { *r = a->mul(bn256_t::from_bn(x)); }

void mul_add(point_t* r, const point_t* P, const bn_t& x, const bn_t& y)  // r = x * P + y * G;
{
  point_t X = x * (*P);
  point_t Y = point_t::mul_to_generator(y);
  *r = X + Y;
}

/*
 * `A` is a point on the curve
 * Test if (q-1) * A == -A
 * We don't do q * A = 0, but the software optimizations would have done `q mod q = 0` first before multiplying to A
 */
bool is_in_subgroup(const point_t* A) {
  static const bn256_t q_minus_1 =
      bn256_t::from_bn(bn_t::from_hex("1000000000000000000000000000000014DEF9DEA2F79CD65812631A5CF5D3EC"));
  point_t A_tag = A->mul(q_minus_1);
  return *A == -A_tag;
}

void mul_to_generator(point_t* r, const bn_t& x) { *r = point_t::mul_to_generator(x); }
error_t from_bin(point_t* r, mem_t in) { return r->from_bin(in); }

int to_bin(const point_t* r, uint8_t* out) {
  if (out) r->to_bin(out);
  return 32;
}
void neg(point_t* r, const point_t* a) { point_t::neg(*r, *a); }
void sub(point_t* r, const point_t* a, const point_t* b) { *r = *a - *b; }
void add(point_t* r, const point_t* a, const point_t* b) { *r = *a + *b; }
bool equ(const point_t* a, const point_t* b) { return point_t::equ(*a, *b); }
void copy(point_t* r, const point_t* a) { *r = *a; }
point_t* new_point(const point_t* a) { return new point_t(*a); }
point_t* new_point() {
  point_t* a = new point_t();
  a->set_infinity();
  return a;
}
void free_point(point_t* a) { delete a; }

static bn_t from_le_mod_q(mem_t bin) { return get_order().mod(bn_t::from_bin(bin.rev())); }

static bn_t hash_hram(const uint8_t sig[32], mem_t message, const uint8_t public_key[32]) {
  uint8_t hram[64];
  unsigned int hash_len = 0;
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  EVP_DigestInit(ctx, EVP_sha512());
  EVP_DigestUpdate(ctx, sig, 32);
  EVP_DigestUpdate(ctx, public_key, 32);
  EVP_DigestUpdate(ctx, message.data, message.size);
  EVP_DigestFinal(ctx, hram, &hash_len);
  EVP_MD_CTX_free(ctx);
  return from_le_mod_q(mem_t(hram, 64));
}

static void sign_with_nonce(uint8_t* signature, const uint8_t* message, size_t message_len,
                            const uint8_t public_key[32], const uint8_t az[32], const uint8_t nonce[32]) {
  bn_t nonce_bn = from_le_mod_q(mem_t(nonce, 64));
  point_t R = point_t::mul_to_generator(nonce_bn);
  R.to_bin(signature);

  bn_t hram_bn = hash_hram(signature, mem_t(message, int(message_len)), public_key);

  bn_t az_bn = from_le_mod_q(mem_t(az, 32));
  const mod_t& q = get_order();
  bn_t s = q.mul(hram_bn, az_bn);
  s = q.add(s, nonce_bn);

  s.to_bin(signature + 32, 32);
  mem_t(signature + 32, 32).reverse();
}

static void hash_az(uint8_t* az, const uint8_t private_key[32]) {
  unsigned int hash_len = 0;
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  EVP_DigestInit(ctx, EVP_sha512());
  EVP_DigestUpdate(ctx, private_key, 32);
  EVP_DigestFinal(ctx, az, &hash_len);
  EVP_MD_CTX_free(ctx);

  az[0] &= 248;
  az[31] &= 63;
  az[31] |= 64;
}

extern "C" int ED25519_sign_with_scalar(uint8_t* out_sig, const uint8_t* message, size_t message_len,
                                        const uint8_t public_key[32], const uint8_t scalar_bin[32]) {
  uint8_t nonce[64];
  RAND_bytes(nonce, 64);

  uint8_t az[32];
  for (int i = 0; i < 32; i++) az[i] = scalar_bin[31 - i];

  sign_with_nonce(out_sig, message, message_len, public_key, az, nonce);
  OPENSSL_cleanse(az, sizeof(az));
  return 1;
}

extern "C" void ED25519_scalar_to_public(uint8_t out_public_key[32], const uint8_t scalar_bin[32]) {
  uint8_t az[32];
  for (int i = 0; i < 32; i++) az[i] = scalar_bin[31 - i];

  bn_t az_bn = from_le_mod_q(mem_t(az, 32));
  point_t A = point_t::mul_to_generator(az_bn);
  A.to_bin(out_public_key);

  OPENSSL_cleanse(az, sizeof(az));
}

extern "C" void ED25519_private_to_scalar(uint8_t out_scalar_bin[32], const uint8_t private_key[32]) {
  uint8_t az[64];
  hash_az(az, private_key);

  bn_t scalar = from_le_mod_q(mem_t(az, 32));
  scalar.to_bin(out_scalar_bin, 32);

  OPENSSL_cleanse(az, sizeof(az));
}

}  // namespace coinbase::crypto::ec25519_core

#if OPENSSL_VERSION_NUMBER >= 0x30000000

extern "C" int ossl_ed25519_sign(uint8_t* out_sig, const uint8_t* tbs, size_t tbs_len, const uint8_t public_key[32],
                                 const uint8_t private_key[32], const uint8_t dom2flag, const uint8_t phflag,
                                 const uint8_t csflag, const uint8_t* context, size_t context_len, void* libctx,
                                 const char* propq);
extern "C" int ossl_ed25519_verify(const uint8_t* tbs, size_t tbs_len, const uint8_t signature[64],
                                   const uint8_t public_key[32], const uint8_t dom2flag, const uint8_t phflag,
                                   const uint8_t csflag, const uint8_t* context, size_t context_len, void* libctx,
                                   const char* propq);

extern "C" int ED25519_sign(uint8_t* out_sig, const uint8_t* message, size_t message_len, const uint8_t public_key[32],
                            const uint8_t private_key[32]) {
  return ossl_ed25519_sign(out_sig, message, message_len, public_key, private_key, 0, 0, 0, 0, 0, 0, 0);
}

extern "C" int ED25519_verify(const uint8_t* message, size_t message_len, const uint8_t signature[64],
                              const uint8_t public_key[32]) {
  return ossl_ed25519_verify(message, message_len, signature, public_key, 0, 0, 0, 0, 0, 0, 0);
}

#endif

// NOLINTEND
