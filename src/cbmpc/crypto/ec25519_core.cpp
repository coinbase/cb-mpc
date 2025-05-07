
#include "ec25519_core.h"

#include <cbmpc/core/extended_uint.h>
#include <cbmpc/core/utils.h>

#include "base_ec_core.h"

#define EXTENDED_COORD

namespace coinbase::crypto::ec25519_core {

static void fe_freeze(uint256_t& r) {
  uint64_t r0 = r.w0;
  uint64_t r1 = r.w1;
  uint64_t r2 = r.w2;
  uint64_t r3 = r.w3;
  uint64_t t0 = r0;
  uint64_t t1 = r1;
  uint64_t t2 = r2;
  uint64_t t3 = r3;
  const uint64_t hi = 0x8000000000000000;

  uint64_t c = 0;
  t0 = addx(t0, 19, c);
  t1 = addx(t1, 0, c);
  t2 = addx(t2, 0, c);
  t3 = addx(t3, hi, c);
  uint64_t mask = constant_time_mask_64(c);
  t0 = r0 = MASKED_SELECT(mask, t0, r0);
  t1 = r1 = MASKED_SELECT(mask, t1, r1);
  t2 = r2 = MASKED_SELECT(mask, t2, r2);
  t3 = r3 = MASKED_SELECT(mask, t3, r3);

  c = 0;
  t0 = addx(t0, 19, c);
  t1 = addx(t1, 0, c);
  t2 = addx(t2, 0, c);
  t3 = addx(t3, hi, c);
  mask = constant_time_mask_64(c);
  r.w0 = MASKED_SELECT(mask, t0, r0);
  r.w1 = MASKED_SELECT(mask, t1, r1);
  r.w2 = MASKED_SELECT(mask, t2, r2);
  r.w3 = MASKED_SELECT(mask, t3, r3);
}

static void fe_add(uint256_t& r, const uint256_t& x, const uint256_t& y) {
  uint64_t c = 0;
  uint64_t x0 = addx(x.w0, y.w0, c);
  uint64_t x1 = addx(x.w1, y.w1, c);
  uint64_t x2 = addx(x.w2, y.w2, c);
  uint64_t x3 = addx(x.w3, y.w3, c);

  uint64_t t = constant_time_select_u64(c, 38, 0);
  c = 0;
  x0 = addx(x0, t, c);
  x1 = addx(x1, 0, c);
  x2 = addx(x2, 0, c);
  x3 = addx(x3, 0, c);

  t = constant_time_select_u64(c, t, 0);
  r.w0 = x0 + t;
  r.w1 = x1;
  r.w2 = x2;
  r.w3 = x3;
}

static void fe_sub(uint256_t& r, const uint256_t& x, const uint256_t& y) {
  uint64_t c = 0;
  uint64_t x0 = subx(x.w0, y.w0, c);
  uint64_t x1 = subx(x.w1, y.w1, c);
  uint64_t x2 = subx(x.w2, y.w2, c);
  uint64_t x3 = subx(x.w3, y.w3, c);

  uint64_t t = constant_time_select_u64(c, 38, 0);
  c = 0;
  x0 = subx(x0, t, c);
  x1 = subx(x1, 0, c);
  x2 = subx(x2, 0, c);
  x3 = subx(x3, 0, c);

  t = constant_time_select_u64(c, t, 0);
  r.w0 = x0 - t;
  r.w1 = x1;
  r.w2 = x2;
  r.w3 = x3;
}

static void fe_square_noasm(uint256_t& r, const uint256_t& x) {
  uint64_t t0, t1, t2, t3, t4, t5, t6, t7, s0, s1, s2, s3, s4, s5;
  uint128_t z;
  z = uint128_t(x.w1) * x.w0;
  t0 = uint64_t(z);
  t1 = uint64_t(z >> 64);
  z = uint128_t(x.w2) * x.w1;
  t2 = uint64_t(z);
  t3 = uint64_t(z >> 64);
  z = uint128_t(x.w3) * x.w2;
  t4 = uint64_t(z);
  t5 = uint64_t(z >> 64);
  z = uint128_t(x.w2) * x.w0;
  t6 = uint64_t(z);
  t7 = uint64_t(z >> 64);
  uint64_t c = 0;
  t1 = addx(t1, t6, c);
  t2 = addx(t2, t7, c);
  t3 = addx(t3, 0, c);
  z = uint128_t(x.w3) * x.w1;
  t6 = uint64_t(z);
  t7 = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, t6, c);
  t4 = addx(t4, t7, c);
  t5 = addx(t5, 0, c);
  z = uint128_t(x.w3) * x.w0;
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
  t6 = x.w0;
  z = uint128_t(t6) * t6;
  s0 = uint64_t(z);
  s1 = uint64_t(z >> 64);
  t6 = x.w1;
  z = uint128_t(t6) * t6;
  s2 = uint64_t(z);
  s3 = uint64_t(z >> 64);
  t6 = x.w2;
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
  t6 = x.w3;
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
  r.w0 = s0 + c * 38;
  r.w1 = t0;
  r.w2 = t1;
  r.w3 = t2;
}

static void fe_mul_noasm(uint256_t& r, const uint256_t& x, const uint256_t& y) {
  uint64_t c, lo, hi, s0, s1, s2, s3, t0, t1, t2, t3, t4, t5, t6, t7;
  uint128_t z;

  s0 = x.w0;
  z = uint128_t(y.w0) * s0;
  t0 = uint64_t(z);
  t1 = uint64_t(z >> 64);
  z = uint128_t(y.w1) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t1 = addx(t1, lo, c);
  t2 = addx(hi, 0, c);
  z = uint128_t(y.w2) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t2 = addx(t2, lo, c);
  t3 = addx(hi, 0, c);
  z = uint128_t(y.w3) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, lo, c);
  t4 = addx(hi, 0, c);
  s0 = x.w1;
  z = uint128_t(y.w0) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t1 = addx(t1, lo, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y.w1) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t2 = addx(t2, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t2 = addx(t2, s3, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y.w2) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t3 = addx(t3, s3, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y.w3) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t4 = addx(t4, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t4 = addx(t4, s3, c);
  t5 = addx(hi, 0, c);
  s0 = x.w2;
  z = uint128_t(y.w0) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t2 = addx(t2, lo, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y.w1) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t3 = addx(t3, s3, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y.w2) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t4 = addx(t4, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t4 = addx(t4, s3, c);
  s3 = addx(hi, 0, c);
  z = uint128_t(y.w3) * s0;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t5 = addx(t5, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t5 = addx(t5, s3, c);
  t6 = addx(hi, 0, c);
  s1 = x.w3;
  z = uint128_t(y.w0) * s1;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t3 = addx(t3, lo, c);
  s0 = addx(hi, 0, c);
  z = uint128_t(y.w1) * s1;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t4 = addx(t4, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t4 = addx(t4, s0, c);
  s0 = addx(hi, 0, c);
  z = uint128_t(y.w2) * s1;
  lo = uint64_t(z);
  hi = uint64_t(z >> 64);
  c = 0;
  t5 = addx(t5, lo, c);
  hi = addx(hi, 0, c);
  c = 0;
  t5 = addx(t5, s0, c);
  s0 = addx(hi, 0, c);
  z = uint128_t(y.w3) * s1;
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
  r.w0 = t0 + c * 38;
  r.w1 = t1;
  r.w2 = t2;
  r.w3 = t3;
}

#ifdef INTEL_X64
static void __attribute__((naked)) fe_square_mulx(uint256_t& r, const uint256_t& x) {
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

static void __attribute__((naked)) fe_mul_mulx(uint256_t& r, const uint256_t& x, const uint256_t& y) {
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

typedef void (*fe_mul_f)(uint256_t& r, const uint256_t& a, const uint256_t& b);
typedef void (*fe_sqr_f)(uint256_t& r, const uint256_t& a);

static fe_mul_f fe_mul = fe_mul_noasm;
static fe_sqr_f fe_sqr = fe_square_noasm;

static auto init = []() {
  if (support_x64_mulx()) {
    fe_mul = fe_mul_mulx;
    fe_sqr = fe_square_mulx;
  }
  return 0;
}();

#endif

struct fe_t {
  uint256_t d;

  static fe_t zero() {
    fe_t r;
    r.d = uint256_t::make(0);
    return r;
  }
  static fe_t one() {
    fe_t r;
    r.d = uint256_t::make(1);
    return r;
  }

  bool is_zero() const { return from_fe().is_zero(); }
  bool is_odd() const { return from_fe().is_odd(); }

  static fe_t to_fe(const uint256_t& a) {
    fe_t r;
    r.d = a;
    return r;
  }
  static fe_t to_fe(int a) {
    fe_t r;
    r.d = uint256_t::make(a);
    return r;
  }

  void cnd_assign(bool flag, const fe_t& a) { d.cnd_assign(flag, a.d); }

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

  static void sqr(fe_t& r, const fe_t& a) {
#ifdef INTEL_X64
    fe_sqr(r.d, a.d);
#else
    fe_square_noasm(r.d, a.d);
#endif
  }

  static void mul(fe_t& r, const fe_t& a, const fe_t& b) {
#ifdef INTEL_X64
    fe_mul(r.d, a.d, b.d);
#else
    fe_mul_noasm(r.d, a.d, b.d);
#endif
  }

  fe_t operator-() const { return zero() - *this; }

  static void add(fe_t& r, const fe_t& a, const fe_t& b) { fe_add(r.d, a.d, b.d); }
  static void sub(fe_t& r, const fe_t& a, const fe_t& b) { fe_sub(r.d, a.d, b.d); }
  static void sub(fe_t& r, const fe_t& a) { sub(r, r, a); }
  static void add(fe_t& r, const fe_t& a) { add(r, r, a); }
  static void mul(fe_t& r, const fe_t& a) { mul(r, r, a); }
  static void sqr(fe_t& r) { sqr(r, r); }

  uint256_t from_fe() const {
    uint256_t r = d;
    fe_freeze(r);
    return r;
  }

  bn_t to_bn() const {
    uint256_t r = d;
    fe_freeze(r);
    return bn_t::from_bin(r.to_bin());
  }

  static fe_t from_bn(const bn_t& x) {
    fe_t r;
    r.d = uint256_t::from_bin(x.to_bin(32));
    return r;
  }

  bool operator==(const fe_t& b) const { return this->from_fe() == b.from_fe(); }
  bool operator!=(const fe_t& b) const { return !(*this == b); }

  static void invert(fe_t& r, const fe_t& x) {
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

    /* 2 */ sqr(z2, x);
    /* 4 */ sqr(t, z2);
    /* 8 */ sqr(t, t);
    /* 9 */ mul(z9, t, x);
    /* 11 */ mul(z11, z9, z2);
    /* 22 */ sqr(t, z11);
    /* 2^5 - 2^0 = 31 */ mul(z2_5_0, t, z9);

    /* 2^6 - 2^1 */ sqr(t, z2_5_0);
    /* 2^20 - 2^10 */
    for (i = 1; i < 5; i++) {
      sqr(t, t);
    }
    /* 2^10 - 2^0 */ mul(z2_10_0, t, z2_5_0);

    /* 2^11 - 2^1 */ sqr(t, z2_10_0);
    /* 2^20 - 2^10 */
    for (i = 1; i < 10; i++) {
      sqr(t, t);
    }
    /* 2^20 - 2^0 */ mul(z2_20_0, t, z2_10_0);

    /* 2^21 - 2^1 */ sqr(t, z2_20_0);
    /* 2^40 - 2^20 */
    for (i = 1; i < 20; i++) {
      sqr(t, t);
    }
    /* 2^40 - 2^0 */ mul(t, t, z2_20_0);

    /* 2^41 - 2^1 */ sqr(t, t);
    /* 2^50 - 2^10 */
    for (i = 1; i < 10; i++) {
      sqr(t, t);
    }
    /* 2^50 - 2^0 */ mul(z2_50_0, t, z2_10_0);

    /* 2^51 - 2^1 */ sqr(t, z2_50_0);
    /* 2^100 - 2^50 */
    for (i = 1; i < 50; i++) {
      sqr(t, t);
    }
    /* 2^100 - 2^0 */ mul(z2_100_0, t, z2_50_0);

    /* 2^101 - 2^1 */ sqr(t, z2_100_0);
    /* 2^200 - 2^100 */
    for (i = 1; i < 100; i++) {
      sqr(t, t);
    }
    /* 2^200 - 2^0 */ mul(t, t, z2_100_0);

    /* 2^201 - 2^1 */ sqr(t, t);
    /* 2^250 - 2^50 */
    for (i = 1; i < 50; i++) {
      sqr(t, t);
    }
    /* 2^250 - 2^0 */ mul(t, t, z2_50_0);

    /* 2^251 - 2^1 */ sqr(t, t);
    /* 2^252 - 2^2 */ sqr(t, t);
    /* 2^253 - 2^3 */ sqr(t, t);

    /* 2^254 - 2^4 */ sqr(t, t);

    /* 2^255 - 2^5 */ sqr(t, t);
    /* 2^255 - 21 */ mul(r, t, z11);
  }

  fe_t inv() const {
    fe_t r;
    invert(r, *this);
    return r;
  }

  // pow22523 returns x^((p-5)/8) where (p-5)/8 is 2^252-3.
  fe_t pow22523() const {
    const fe_t& z = *this;
    fe_t t0, t1, t2, out;

    sqr(t0, z);
    sqr(t1, t0);
    for (int i = 1; i < 2; ++i) sqr(t1, t1);
    mul(t1, z, t1);
    mul(t0, t0, t1);
    sqr(t0, t0);
    mul(t0, t1, t0);
    sqr(t1, t0);
    for (int i = 1; i < 5; ++i) sqr(t1, t1);
    mul(t0, t1, t0);
    sqr(t1, t0);
    for (int i = 1; i < 10; ++i) sqr(t1, t1);
    mul(t1, t1, t0);
    sqr(t2, t1);
    for (int i = 1; i < 20; ++i) sqr(t2, t2);
    mul(t1, t2, t1);
    sqr(t1, t1);
    for (int i = 1; i < 10; ++i) sqr(t1, t1);
    mul(t0, t1, t0);
    sqr(t1, t0);
    for (int i = 1; i < 50; ++i) sqr(t1, t1);
    mul(t1, t1, t0);
    sqr(t2, t1);
    for (int i = 1; i < 100; ++i) sqr(t2, t2);
    mul(t1, t2, t1);
    sqr(t1, t1);
    for (int i = 1; i < 50; ++i) sqr(t1, t1);
    mul(t0, t1, t0);
    sqr(t0, t0);
    for (int i = 1; i < 2; ++i) sqr(t0, t0);
    mul(out, t0, z);
    return out;
  }
};

using formula_t = crypto::edwards_projective_t<fe_t, -1>;
using curve_t = crypto::ecurve_core_t<formula_t>;
using point_t = curve_t::point_t;
using generator_point_t = curve_t::generator_point_t;

}  // namespace coinbase::crypto::ec25519_core

template <>
coinbase::crypto::ec25519_core::fe_t coinbase::crypto::ec25519_core::formula_t::get_d()  // static
{
  static const coinbase::crypto::ec25519_core::fe_t d = coinbase::crypto::ec25519_core::fe_t::from_bn(
      bn_t::from_hex("52036cee2b6ffe738cc740797779e89800700a4d4141d8ab75eb4dca135978a3"));
  return d;
}

template <>
const mod_t& coinbase::crypto::ec25519_core::curve_t::order()  // static
{
  static const mod_t q = bn_t::from_hex("1000000000000000000000000000000014DEF9DEA2F79CD65812631A5CF5D3ED");
  return q;
}

template <>
const coinbase::crypto::ec25519_core::curve_t::point_t& coinbase::crypto::ec25519_core::curve_t::generator_point() {
  static const coinbase::crypto::ec25519_core::curve_t::point_t G =
      coinbase::crypto::ec25519_core::curve_t::point_t::affine(
          bn_t::from_hex("216936D3CD6E53FEC0A4E231FDD6DC5C692CC7609525A7B2C9562D608F25D51A"),
          bn_t::from_hex("6666666666666666666666666666666666666666666666666666666666666658"));
  return G;
}

namespace coinbase::crypto::ec25519_core {

const crypto::ecp_storage_t& get_generator() { return (const crypto::ecp_storage_t&)curve_t::generator(); }
bool is_on_curve(const crypto::ecp_storage_t* a) { return ((const point_t*)a)->is_on_curve(); }
bool is_infinity(const crypto::ecp_storage_t* a) { return ((const point_t*)a)->is_infinity(); }
void set_infinity(crypto::ecp_storage_t* r) { ((point_t*)r)->set_infinity(); }
void get_xy(const crypto::ecp_storage_t* a, bn_t& x, bn_t& y) { ((const point_t*)a)->get_xy(x, y); }
void mul(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a, const bn_t& x) {
  curve_t::mul(*(const point_t*)a, x, *(point_t*)r);
}
void mul_vartime(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a, const bn_t& x) {
  curve_t::mul<crypto::ec_vartime>(*(const point_t*)a, x, *(point_t*)r);
}
void mul_to_generator(crypto::ecp_storage_t* r, const bn_t& x) { curve_t::mul_to_generator(x, *(point_t*)r); }
void mul_to_generator_vartime(crypto::ecp_storage_t* r, const bn_t& x) {
  curve_t::mul_to_generator<crypto::ec_vartime>(x, *(point_t*)r);
}

bool is_in_subgroup(const crypto::ecp_storage_t* a) {
  static bn_t q_minus_1 = bn_t::from_hex("1000000000000000000000000000000014DEF9DEA2F79CD65812631A5CF5D3EC");
  point_t x;
  curve_t::mul(*(const point_t*)a, q_minus_1, x);
  return *(const point_t*)a == -x;
}

static error_t from_bin(point_t& R, mem_t bin) {
  if (bin.size != 32) return coinbase::error(E_FORMAT);

  buf_t buf = bin.rev();
  uint8_t neg = buf[0] >> 7;
  buf[0] &= 0x7f;
  fe_t y = fe_t::to_fe(uint256_t::from_bin(buf));

  // x² = (y² - 1) / (dy² + 1)

  fe_t u, v, w, vxx, check;

  u = y * y;
  v = u * formula_t::get_d();
  u -= fe_t::one();       // u = y^2-1
  v += fe_t::one();       // v = dy^2+1
  w = u * v;              // w = u*v
  fe_t x = w.pow22523();  // x = w^((q-5)/8)
  x *= u;                 // x = u * w^((q-5)/8)

  vxx = x * x;
  vxx *= v;
  check = vxx - u;  // vx^2-u
  if (!check.is_zero()) {
    check = vxx + u;  // vx^2+u
    if (!check.is_zero()) {
      return coinbase::error(E_CRYPTO);
    }
    static const fe_t sqrtm1 =
        fe_t::from_bn(bn_t::from_hex("2b8324804fc1df0b2b4d00993dfbd7a72f431806ad2fe478c4ee1b274a0ea0b0"));
    x *= sqrtm1;
  }

  uint256_t x_val = x.from_fe();
  if (neg != (x_val.w0 & 1)) x = -x;

  R.x = x;
  R.y = y;
  R.z = fe_t::one();
  return 0;
}

error_t from_bin(crypto::ecp_storage_t* r, mem_t in) { return from_bin(*(point_t*)r, in); }

static void to_bin(const point_t& P, byte_ptr r) {
  if (P.is_infinity()) {
    r[0] = 1;
    memset(r + 1, 0, 31);
    return;
  }

  fe_t x, y;
  P.get_xy(x, y);
  y.from_fe().to_bin(r);
  mem_t(r, 32).reverse();
  r[31] ^= int(x.is_odd()) << 7;
}

int to_bin(const crypto::ecp_storage_t* p, uint8_t* out) {
  if (out) to_bin(*(const point_t*)p, out);
  return 32;
}

void neg(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a) { *(point_t*)r = -*(const point_t*)a; }
void sub(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a, const crypto::ecp_storage_t* b) {
  *(point_t*)r = *(const point_t*)a - *(const point_t*)b;
}
void add(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a, const crypto::ecp_storage_t* b) {
  *(point_t*)r = *(const point_t*)a + *(const point_t*)b;
}
bool equ(const crypto::ecp_storage_t* a, const crypto::ecp_storage_t* b) {
  return *(const point_t*)a == *(const point_t*)b;
}
void copy(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a) { *r = *a; }
crypto::ecp_storage_t* new_point(const crypto::ecp_storage_t* a) { return new crypto::ecp_storage_t(*a); }
crypto::ecp_storage_t* new_point() {
  crypto::ecp_storage_t* a = new crypto::ecp_storage_t();
  ((point_t*)a)->set_infinity();
  return a;
}
void free_point(crypto::ecp_storage_t* a) { delete a; }

static bn_t from_le_mod_q(mem_t bin) { return curve_t::order().mod(bn_t::from_bin(bin.rev())); }

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
  point_t R;
  curve_t::mul_to_generator(nonce_bn, R);
  to_bin(R, signature);

  bn_t hram_bn = hash_hram(signature, mem_t(message, int(message_len)), public_key);

  bn_t az_bn = from_le_mod_q(mem_t(az, 32));
  const mod_t& q = curve_t::order();
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
  point_t A;
  curve_t::mul_to_generator(az_bn, A);
  to_bin(A, out_public_key);

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
