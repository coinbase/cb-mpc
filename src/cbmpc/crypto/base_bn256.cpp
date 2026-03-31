#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/core/utils.h>
#include <cbmpc/internal/crypto/base_bn256.h>
#include <cbmpc/internal/crypto/base_mod.h>

namespace coinbase::crypto {

void mul256_noasm(uint64_t r[8], const uint64_t a[4], const uint64_t b[4]) {
  uint128_t t;
  uint64_t h;
  t = uint128_t(a[0]) * b[0];
  r[0] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[1]) * b[0] + h;
  r[1] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[2]) * b[0] + h;
  r[2] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[3]) * b[0] + h;
  r[3] = uint64_t(t);
  r[4] = t >> 64;

  t = uint128_t(a[0]) * b[1] + r[1];
  r[1] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[1]) * b[1] + r[2] + h;
  r[2] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[2]) * b[1] + r[3] + h;
  r[3] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[3]) * b[1] + r[4] + h;
  r[4] = uint64_t(t);
  r[5] = t >> 64;

  t = uint128_t(a[0]) * b[2] + r[2];
  r[2] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[1]) * b[2] + r[3] + h;
  r[3] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[2]) * b[2] + r[4] + h;
  r[4] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[3]) * b[2] + r[5] + h;
  r[5] = uint64_t(t);
  r[6] = t >> 64;

  t = uint128_t(a[0]) * b[3] + r[3];
  r[3] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[1]) * b[3] + r[4] + h;
  r[4] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[2]) * b[3] + r[5] + h;
  r[5] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a[3]) * b[3] + r[6] + h;
  r[6] = uint64_t(t);
  r[7] = t >> 64;
}

void barrett_reduce256_noasm(uint64_t* res, uint64_t x[8], const uint64_t* m, const uint64_t* mu) {
  uint64_t* q1 = x + 3;

  uint64_t q2[10];  // = {0};

  uint128_t t;
  uint64_t h;
  // q2[5] = mul_add_regular_mu(q2+0, q1[0], mu);

  uint128_t b = uint128_t(q1[0]);
  t = b * mu[0];
  q2[0 + 0] = uint64_t(t);
  h = t >> 64;
  t = b * mu[1] + h;
  q2[0 + 1] = uint64_t(t);
  h = t >> 64;
  t = b * mu[2] + h;
  q2[0 + 2] = uint64_t(t);
  h = t >> 64;
  t = b * mu[3] + h;
  q2[0 + 3] = uint64_t(t);
  h = t >> 64;
  t = b * mu[4] + h;
  q2[0 + 4] = uint64_t(t);
  h = t >> 64;
  q2[5] = h;

  // q2[6] = mul_add_regular_mu(q2+1, q1[1], mu);
  b = uint128_t(q1[1]);
  t = b * mu[0] + q2[1 + 0];
  q2[1 + 0] = uint64_t(t);
  h = t >> 64;
  t = b * mu[1] + q2[1 + 1] + h;
  q2[1 + 1] = uint64_t(t);
  h = t >> 64;
  t = b * mu[2] + q2[1 + 2] + h;
  q2[1 + 2] = uint64_t(t);
  h = t >> 64;
  t = b * mu[3] + q2[1 + 3] + h;
  q2[1 + 3] = uint64_t(t);
  h = t >> 64;
  t = b * mu[4] + q2[1 + 4] + h;
  q2[1 + 4] = uint64_t(t);
  h = t >> 64;
  q2[6] = h;

  // q2[7] = mul_add_regular_mu(q2+2, q1[2], mu);
  b = uint128_t(q1[2]);
  t = b * mu[0] + q2[2 + 0];
  q2[2 + 0] = uint64_t(t);
  h = t >> 64;
  t = b * mu[1] + q2[2 + 1] + h;
  q2[2 + 1] = uint64_t(t);
  h = t >> 64;
  t = b * mu[2] + q2[2 + 2] + h;
  q2[2 + 2] = uint64_t(t);
  h = t >> 64;
  t = b * mu[3] + q2[2 + 3] + h;
  q2[2 + 3] = uint64_t(t);
  h = t >> 64;
  t = b * mu[4] + q2[2 + 4] + h;
  q2[2 + 4] = uint64_t(t);
  h = t >> 64;
  q2[7] = h;

  // q2[8] = mul_add_regular_mu(q2+3, q1[3], mu);
  b = uint128_t(q1[3]);
  t = b * mu[0] + q2[3 + 0];
  q2[3 + 0] = uint64_t(t);
  h = t >> 64;
  t = b * mu[1] + q2[3 + 1] + h;
  q2[3 + 1] = uint64_t(t);
  h = t >> 64;
  t = b * mu[2] + q2[3 + 2] + h;
  q2[3 + 2] = uint64_t(t);
  h = t >> 64;
  t = b * mu[3] + q2[3 + 3] + h;
  q2[3 + 3] = uint64_t(t);
  h = t >> 64;
  t = b * mu[4] + q2[3 + 4] + h;
  q2[3 + 4] = uint64_t(t);
  h = t >> 64;
  q2[8] = h;

  // q2[9] = mul_add_regular_mu(q2+4, q1[4], mu);
  b = uint128_t(q1[4]);
  t = b * mu[0] + q2[4 + 0];
  q2[4 + 0] = uint64_t(t);
  h = t >> 64;
  t = b * mu[1] + q2[4 + 1] + h;
  q2[4 + 1] = uint64_t(t);
  h = t >> 64;
  t = b * mu[2] + q2[4 + 2] + h;
  q2[4 + 2] = uint64_t(t);
  h = t >> 64;
  t = b * mu[3] + q2[4 + 3] + h;
  q2[4 + 3] = uint64_t(t);
  h = t >> 64;
  t = b * mu[4] + q2[4 + 4] + h;
  q2[4 + 4] = uint64_t(t);
  h = t >> 64;
  q2[9] = h;

  const uint64_t* q3 = q2 + 5;
  uint64_t r2[5];  // = {0};

  //  barrett_partial_mul_row<5>(r2+0, q3, m[0]);
  b = uint128_t(m[0]);
  t = b * q3[0];
  r2[0 + 0] = uint64_t(t);
  h = t >> 64;
  t = b * q3[1] + h;
  r2[0 + 1] = uint64_t(t);
  h = t >> 64;
  t = b * q3[2] + h;
  r2[0 + 2] = uint64_t(t);
  h = t >> 64;
  t = b * q3[3] + h;
  r2[0 + 3] = uint64_t(t);
  h = t >> 64;
  t = b * q3[4] + h;
  r2[0 + 4] = uint64_t(t);

  //  barrett_partial_mul_row<4>(r2+1, q3, m[1]);
  b = uint128_t(m[1]);
  t = b * q3[0] + r2[1 + 0];
  r2[1 + 0] = uint64_t(t);
  h = t >> 64;
  t = b * q3[1] + r2[1 + 1] + h;
  r2[1 + 1] = uint64_t(t);
  h = t >> 64;
  t = b * q3[2] + r2[1 + 2] + h;
  r2[1 + 2] = uint64_t(t);
  h = t >> 64;
  t = b * q3[3] + r2[1 + 3] + h;
  r2[1 + 3] = uint64_t(t);

  //  barrett_partial_mul_row<3>(r2+2, q3, m[2]);
  b = uint128_t(m[2]);
  t = b * q3[0] + r2[2 + 0];
  r2[2 + 0] = uint64_t(t);
  h = t >> 64;
  t = b * q3[1] + r2[2 + 1] + h;
  r2[2 + 1] = uint64_t(t);
  h = t >> 64;
  t = b * q3[2] + r2[2 + 2] + h;
  r2[2 + 2] = uint64_t(t);

  //  barrett_partial_mul_row<2>(r2+3, q3, m[3]);
  b = uint128_t(m[3]);
  t = b * q3[0] + r2[3 + 0];
  r2[3 + 0] = uint64_t(t);
  h = t >> 64;
  t = b * q3[1] + r2[3 + 1] + h;
  r2[3 + 1] = uint64_t(t);

  uint64_t borrow = 0;
  uint64_t w0 = subx(x[0], r2[0], borrow);
  uint64_t w1 = subx(x[1], r2[1], borrow);
  uint64_t w2 = subx(x[2], r2[2], borrow);
  uint64_t w3 = subx(x[3], r2[3], borrow);
  uint64_t r4 = subx(x[4], r2[4], borrow);

  borrow = 0;
  r2[0] = subx(w0, m[0], borrow);
  r2[1] = subx(w1, m[1], borrow);
  r2[2] = subx(w2, m[2], borrow);
  r2[3] = subx(w3, m[3], borrow);
  borrow &= (r4 == 0);

  uint64_t mask = ~(uint64_t(0) - borrow);
  w0 ^= (w0 ^ r2[0]) & mask;
  w1 ^= (w1 ^ r2[1]) & mask;
  w2 ^= (w2 ^ r2[2]) & mask;
  w3 ^= (w3 ^ r2[3]) & mask;

  borrow = 0;
  r2[0] = subx(w0, m[0], borrow);
  r2[1] = subx(w1, m[1], borrow);
  r2[2] = subx(w2, m[2], borrow);
  r2[3] = subx(w3, m[3], borrow);

  mask = ~(uint64_t(0) - borrow);
  w0 ^= (w0 ^ r2[0]) & mask;
  w1 ^= (w1 ^ r2[1]) & mask;
  w2 ^= (w2 ^ r2[2]) & mask;
  w3 ^= (w3 ^ r2[3]) & mask;

  res[0] = w0;
  res[1] = w1;
  res[2] = w2;
  res[3] = w3;
}

#ifdef __x86_64__
void __attribute__((naked)) mul256_intel_mulx(uint64_t r[8], const uint64_t a[4], const uint64_t b[4]) {
  __asm__ volatile(
      "\
  push %%rbp;\
  push %%r12;\
  push %%r13;\
  push %%r14;\
  movq %%rdx, %%rcx;\
  \
  movq 0*8(%%rsi), %%rdx;  xorq %%rbp, %%rbp; \
  mulx 0*8(%%rcx), %%r10, %%r9;  adcxq %%rbp, %%r10;  \
  mulx 1*8(%%rcx), %%r11, %%r8;  adcxq %%r9,  %%r11;  \
  mulx 2*8(%%rcx), %%r12, %%r9;  adcxq %%r8,  %%r12;  \
  mulx 3*8(%%rcx), %%r13, %%r14; adcxq %%r9,  %%r13;  \
  adcxq	%%rbp, %%r14; movq %%r10, 0*8(%%rdi); \
  \
  movq 1*8(%%rsi), %%rdx;  xorq %%rbp, %%rbp; \
  mulx 0*8(%%rcx),  %%rax, %%r9;  adcxq %%rbp, %%rax;  adoxq %%rax, %%r11; \
  mulx 1*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r12; \
  mulx 2*8(%%rcx),  %%rax, %%r9;  adcxq %%r8,  %%rax;  adoxq %%rax, %%r13; \
  mulx 3*8(%%rcx),  %%rax, %%r10; adcxq %%r9,  %%rax;  adoxq %%rax, %%r14; \
  adcxq	%%rbp, %%r10; adoxq	%%rbp, %%r10; movq %%r11, 1*8(%%rdi); \
  \
  movq 2*8(%%rsi), %%rdx;  xorq %%rbp, %%rbp; \
  mulx 0*8(%%rcx),  %%rax, %%r9;  adcxq %%rbp, %%rax;  adoxq %%rax, %%r12; \
  mulx 1*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r13; \
  mulx 2*8(%%rcx),  %%rax, %%r9;  adcxq %%r8,  %%rax;  adoxq %%rax, %%r14; \
  mulx 3*8(%%rcx),  %%rax, %%r11; adcxq %%r9,  %%rax;  adoxq %%rax, %%r10; \
  adcxq	%%rbp, %%r11; adoxq	%%rbp, %%r11; movq %%r12, 2*8(%%rdi); \
  \
  movq 3*8(%%rsi), %%rdx;  xorq %%rbp, %%rbp; \
  mulx 0*8(%%rcx),  %%rax, %%r9;  adcxq %%rbp, %%rax;  adoxq %%rax, %%r13; \
  mulx 1*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r14; \
  mulx 2*8(%%rcx),  %%rax, %%r9;  adcxq %%r8,  %%rax;  adoxq %%rax, %%r10; \
  mulx 3*8(%%rcx),  %%rax, %%r12; adcxq %%r9,  %%rax;  adoxq %%rax, %%r11; \
  adcxq	%%rbp, %%r12; adoxq	%%rbp, %%r12; movq %%r13, 3*8(%%rdi); \
  \
  movq %%r14, 4*8(%%rdi); \
  movq %%r10, 5*8(%%rdi); \
  movq %%r11, 6*8(%%rdi); \
  movq %%r12, 7*8(%%rdi); \
  \
  pop %%r14;\
  pop %%r13;\
  pop %%r12;\
  pop %%rbp;\
  ret;\
  "
      :
      :
      :);
}

void __attribute__((naked)) barrett_reduce256_intel_mulx(uint64_t res[4], uint64_t x[8], const uint64_t m[4],
                                                         const uint64_t mu[5]) {
  // r0=rbx, r1=rcx, r2=rdi, r3=r14, r4=rbp
  __asm__ volatile(
      "\
  push %%rbx;\
  push %%rbp;\
  push %%r12;\
  push %%r13;\
  push %%r14;\
  push %%r15;\
  \
  push %%rdi;\
  movdqu 0(%%rdx), %%xmm0;\
  movdqu 16(%%rdx), %%xmm1;\
  \
  movq 3*8(%%rsi), %%rdx;  xorq %%rbp, %%rbp; \
  mulx 0*8(%%rcx), %%r10, %%r9;  adcxq %%rbp, %%r10;  \
  mulx 1*8(%%rcx), %%r11, %%r8;  adcxq %%r9,  %%r11;  \
  mulx 2*8(%%rcx), %%r12, %%r9;  adcxq %%r8,  %%r12;  \
  mulx 3*8(%%rcx), %%r13, %%r8;  adcxq %%r9,  %%r13;  \
  mulx 4*8(%%rcx), %%r14, %%r15; adcxq %%r8,  %%r14;  \
  adcxq	%%rbp, %%r15; \
  \
  movq 4*8(%%rsi), %%rdx;  xorq %%rbp, %%rbp; \
  mulx 0*8(%%rcx),  %%rax, %%r9;  adcxq %%rbp, %%rax;  adoxq %%rax, %%r11; \
  mulx 1*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r12; \
  mulx 2*8(%%rcx),  %%rax, %%r9;  adcxq %%r8,  %%rax;  adoxq %%rax, %%r13; \
  mulx 3*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r14; \
  mulx 4*8(%%rcx),  %%rax, %%r10; adcxq %%r8,  %%rax;  adoxq %%rax, %%r15; \
  adcxq	%%rbp, %%r10; adoxq	%%rbp, %%r10; \
  \
  movq 5*8(%%rsi), %%rdx;  xorq %%rbp, %%rbp; \
  mulx 0*8(%%rcx),  %%rax, %%r9;  adcxq %%rbp, %%rax;  adoxq %%rax, %%r12; \
  mulx 1*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r13; \
  mulx 2*8(%%rcx),  %%rax, %%r9;  adcxq %%r8,  %%rax;  adoxq %%rax, %%r14; \
  mulx 3*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r15; \
  mulx 4*8(%%rcx),  %%rax, %%r11; adcxq %%r8,  %%rax;  adoxq %%rax, %%r10; \
  adcxq	%%rbp, %%r11; adoxq	%%rbp, %%r11; \
  \
  movq 6*8(%%rsi), %%rdx;  xorq %%rbp, %%rbp; \
  mulx 0*8(%%rcx),  %%rax, %%r9;  adcxq %%rbp, %%rax;  adoxq %%rax, %%r13; \
  mulx 1*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r14; \
  mulx 2*8(%%rcx),  %%rax, %%r9;  adcxq %%r8,  %%rax;  adoxq %%rax, %%r15; \
  mulx 3*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r10; \
  mulx 4*8(%%rcx),  %%rax, %%r12; adcxq %%r8,  %%rax;  adoxq %%rax, %%r11; \
  adcxq	%%rbp, %%r12; adoxq	%%rbp, %%r12; \
  \
  movq 7*8(%%rsi), %%rdx;  xorq %%rbp, %%rbp; \
  mulx 0*8(%%rcx),  %%rax, %%r9;  adcxq %%rbp, %%rax;  adoxq %%rax, %%r14; \
  mulx 1*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r15; \
  mulx 2*8(%%rcx),  %%rax, %%r9;  adcxq %%r8,  %%rax;  adoxq %%rax, %%r10; \
  mulx 3*8(%%rcx),  %%rax, %%r8;  adcxq %%r9,  %%rax;  adoxq %%rax, %%r11; \
  mulx 4*8(%%rcx),  %%rax, %%r13; adcxq %%r8,  %%rax;  adoxq %%rax, %%r12; \
  adcxq	%%rbp, %%r13; adoxq	%%rbp, %%r13; \
  \
  \
  movq %%xmm0, %%rdx; xorq %%r8, %%r8; \
  mulx %%r15,  %%rbx, %%r9;  adcxq %%r8, %%rbx; \
  mulx %%r10,  %%rcx, %%r8;  adcxq %%r9, %%rcx; \
  mulx %%r11,  %%rdi, %%r9;  adcxq %%r8, %%rdi; \
  mulx %%r12,  %%r14, %%r8;  adcxq %%r9, %%r14; \
  mulx %%r13,  %%rbp, %%r9;  adcxq %%r8, %%rbp; \
  \
  pextrq $1, %%xmm0, %%rdx;  xorq %%r8, %%r8; \
  mulx %%r15,  %%rax, %%r9;  adcxq %%r8, %%rax;  adoxq %%rax, %%rcx; \
  mulx %%r10,  %%rax, %%r8;  adcxq %%r9, %%rax;  adoxq %%rax, %%rdi; \
  mulx %%r11,  %%rax, %%r9;  adcxq %%r8, %%rax;  adoxq %%rax, %%r14; \
  mulx %%r12,  %%rax, %%r8;  adcxq %%r9, %%rax;  adoxq %%rax, %%rbp; \
  \
  movq %%xmm1, %%rdx;  xorq %%r8, %%r8; \
  mulx %%r15,  %%rax, %%r9;  adcxq %%r8, %%rax;  adoxq %%rax, %%rdi; \
  mulx %%r10,  %%rax, %%r8;  adcxq %%r9, %%rax;  adoxq %%rax, %%r14; \
  mulx %%r11,  %%rax, %%r9;  adcxq %%r8, %%rax;  adoxq %%rax, %%rbp; \
  \
  pextrq $1, %%xmm1, %%rdx;  xorq %%r8, %%r8; \
  mulx %%r15,  %%rax, %%r9;  adcxq %%r8, %%rax;  adoxq %%rax, %%r14; \
  mulx %%r10,  %%rax, %%r8;  adcxq %%r9, %%rax;  adoxq %%rax, %%rbp; \
  \
  movq 0*8(%%rsi), %%r10; subq %%rbx, %%r10; \
  movq 1*8(%%rsi), %%r11; sbbq %%rcx, %%r11; \
  movq 2*8(%%rsi), %%r12; sbbq %%rdi, %%r12; \
  movq 3*8(%%rsi), %%r13; sbbq %%r14, %%r13; \
  movq 4*8(%%rsi), %%r14; sbbq %%rbp, %%r14; \
  \
  setz %%al; \
  movq %%r10, %%rbx; movq       %%xmm0, %%r15;  subq %%r15, %%rbx; \
  movq %%r11, %%rcx; pextrq $1, %%xmm0, %%rdx;  sbbq %%rdx, %%rcx; \
  movq %%r12, %%rdi; movq       %%xmm1, %%r8;   sbbq %%r8,  %%rdi; \
  movq %%r13, %%r14; pextrq $1, %%xmm1, %%r9;   sbbq %%r9,  %%r14; \
  setc %%ah; test %%al, %%ah ;\
  cmovzq %%rbx, %%r10; cmovzq %%rcx, %%r11; cmovzq %%rdi, %%r12; cmovzq %%r14, %%r13; \
  movq %%r10, %%rbx; movq       %%xmm0, %%r15;  subq %%r15, %%rbx; \
  movq %%r11, %%rcx; pextrq $1, %%xmm0, %%rdx;  sbbq %%rdx, %%rcx; \
  movq %%r12, %%rdi; movq       %%xmm1, %%r8;   sbbq %%r8,  %%rdi; \
  movq %%r13, %%r14; pextrq $1, %%xmm1, %%r9;   sbbq %%r9,  %%r14; \
  cmovnbq %%rbx, %%r10; cmovnbq %%rcx, %%r11; cmovnbq %%rdi, %%r12; cmovnbq %%r14, %%r13; \
  \
  pop %%rdi;\
  movq %%r10, 0*8(%%rdi);\
  movq %%r11, 1*8(%%rdi);\
  movq %%r12, 2*8(%%rdi);\
  movq %%r13, 3*8(%%rdi);\
  \
  pop %%r15;\
  pop %%r14;\
  pop %%r13;\
  pop %%r12;\
  pop %%rbp;\
  pop %%rbx;\
  ret;\
  "
      :
      :
      :);
}

#endif

// -------------------------- uint256_t ----------------------------

void uint256_t::to_bin(byte_ptr bin) const {
  coinbase::be_set_8(&bin[24], w0);
  coinbase::be_set_8(&bin[16], w1);
  coinbase::be_set_8(&bin[8], w2);
  coinbase::be_set_8(&bin[0], w3);
}

buf_t uint256_t::to_bin() const {
  buf_t r(32);
  to_bin(r.data());
  return r;
}

void uint256_t::to_bin_le(byte_ptr bin) const {
  coinbase::le_set_8(&bin[0], w0);
  coinbase::le_set_8(&bin[8], w1);
  coinbase::le_set_8(&bin[16], w2);
  coinbase::le_set_8(&bin[24], w3);
}

buf_t uint256_t::to_bin_le() const {
  buf_t r(32);
  to_bin_le(r.data());
  return r;
}

uint256_t uint256_t::from_bin(mem_t bin) {
  cb_assert(bin.size == 32);
  uint256_t r;
  r.w0 = coinbase::be_get_8(bin.data + 24);
  r.w1 = coinbase::be_get_8(bin.data + 16);
  r.w2 = coinbase::be_get_8(bin.data + 8);
  r.w3 = coinbase::be_get_8(bin.data + 0);
  return r;
}

uint256_t uint256_t::from_bin_le(mem_t bin) {
  cb_assert(bin.size == 32);
  uint256_t r;
  r.w0 = coinbase::le_get_8(bin.data + 0);
  r.w1 = coinbase::le_get_8(bin.data + 8);
  r.w2 = coinbase::le_get_8(bin.data + 16);
  r.w3 = coinbase::le_get_8(bin.data + 24);
  return r;
}

bn_t uint256_t::to_bn() const {
  buf_t bin(32);
  to_bin(bin.data());
  return bn_t::from_bin(bin);
}

uint256_t uint256_t::from_bn(const bn_t& bn)  // static
{
  return from_bin(bn.to_bin(32));
}

uint256_t uint256_t::make(uint64_t w0, uint64_t w1, uint64_t w2, uint64_t w3) { return uint256_t{w0, w1, w2, w3}; }

bool uint256_t::is_zero() const { return (w0 | w1 | w2 | w3) == 0; }

bool uint256_t::is_odd() const { return bool(w0 & 1); }

bool uint256_t::operator==(const uint256_t& b) const {
  uint64_t x = w0 ^ b.w0;
  x |= w1 ^ b.w1;
  x |= w2 ^ b.w2;
  x |= w3 ^ b.w3;
  return x == 0;
}

bool uint256_t::operator!=(const uint256_t& b) const { return !(*this == b); }

void uint256_t::cnd_assign(bool flag, const uint256_t& a) {
  uint64_t mask = constant_time_mask_64(flag);
  w0 = ((a.w0 ^ w0) & mask) ^ w0;
  w1 = ((a.w1 ^ w1) & mask) ^ w1;
  w2 = ((a.w2 ^ w2) & mask) ^ w2;
  w3 = ((a.w3 ^ w3) & mask) ^ w3;
}

uint64_t uint256_t::mul_add_regular(const uint256_t& a, uint64_t b) {
  uint128_t t;
  uint64_t h;
  t = uint128_t(a.w0) * b + w0;
  w0 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a.w1) * b + w1 + h;
  w1 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a.w2) * b + w2 + h;
  w2 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a.w3) * b + w3 + h;
  w3 = uint64_t(t);
  h = t >> 64;
  return h;
}

#ifdef __x86_64__

bool support_x64_mulx() {
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

typedef void (*mul256_f)(uint64_t r[8], const uint64_t a[4], const uint64_t b[4]);
static mul256_f mul256 = support_x64_mulx() ? mul256_intel_mulx : mul256_noasm;

typedef void (*sqr256_f)(uint64_t r[8], const uint256_t& a);
static sqr256_f sqr256 = uint256_t::sqr_noasm;

#endif

void uint256_t::mul(uint64_t r[8], const uint256_t& a, const uint256_t& b) {
#ifdef __x86_64__
  mul256(r, (const uint64_t*)&a, (const uint64_t*)&b);
#else
  mul256_noasm(r, (const uint64_t*)&a, (const uint64_t*)&b);
#endif
}

void uint256_t::sqr_noasm(uint64_t r[8], const uint256_t& a) {
  uint128_t t, t01, t02, t03, t12, t13, t23;
  uint64_t h;
  t = uint128_t(a.w0) * a.w0;
  r[0] = uint64_t(t);
  h = t >> 64;
  t01 = uint128_t(a.w1) * a.w0;
  t = t01 + h;
  r[1] = uint64_t(t);
  h = t >> 64;
  t02 = uint128_t(a.w2) * a.w0;
  t = t02 + h;
  r[2] = uint64_t(t);
  h = t >> 64;
  t03 = uint128_t(a.w3) * a.w0;
  t = t03 + h;
  r[3] = uint64_t(t);
  r[4] = t >> 64;

  t = t01 + r[1];
  r[1] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a.w1) * a.w1 + r[2] + h;
  r[2] = uint64_t(t);
  h = t >> 64;
  t12 = uint128_t(a.w2) * a.w1;
  t = t12 + r[3] + h;
  r[3] = uint64_t(t);
  h = t >> 64;
  t13 = uint128_t(a.w3) * a.w1;
  t = t13 + r[4] + h;
  r[4] = uint64_t(t);
  r[5] = t >> 64;

  t = t02 + r[2];
  r[2] = uint64_t(t);
  h = t >> 64;
  t = t12 + r[3] + h;
  r[3] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a.w2) * a.w2 + r[4] + h;
  r[4] = uint64_t(t);
  h = t >> 64;
  t23 = uint128_t(a.w3) * a.w2;
  t = t23 + r[5] + h;
  r[5] = uint64_t(t);
  r[6] = t >> 64;

  t = t03 + r[3];
  r[3] = uint64_t(t);
  h = t >> 64;
  t = t13 + r[4] + h;
  r[4] = uint64_t(t);
  h = t >> 64;
  t = t23 + r[5] + h;
  r[5] = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a.w3) * a.w3 + r[6] + h;
  r[6] = uint64_t(t);
  r[7] = t >> 64;
}

void uint256_t::sqr(uint64_t r[8], const uint256_t& a) {
#ifdef __x86_64__
  sqr256(r, a);
#else
  sqr_noasm(r, a);
#endif
}

uint64_t uint256_t::div_by_two() {
  uint64_t c, carry;
  c = w3 << 63;
  w3 = (w3 >> 1);
  carry = c;
  c = w2 << 63;
  w2 = (w2 >> 1) | carry;
  carry = c;
  c = w1 << 63;
  w1 = (w1 >> 1) | carry;
  carry = c;
  c = w0 << 63;
  w0 = (w0 >> 1) | carry;
  carry = c;
  return carry;
}

uint64_t uint256_t::add_raw(const uint256_t& a, const uint256_t& b) {
  uint64_t carry = 0;
  w0 = addx(a.w0, b.w0, carry);
  w1 = addx(a.w1, b.w1, carry);
  w2 = addx(a.w2, b.w2, carry);
  w3 = addx(a.w3, b.w3, carry);
  return carry;
}

uint64_t uint256_t::sub_raw(const uint256_t& a, const uint256_t& b) {
  uint64_t borrow = 0;
  w0 = subx(a.w0, b.w0, borrow);
  w1 = subx(a.w1, b.w1, borrow);
  w2 = subx(a.w2, b.w2, borrow);
  w3 = subx(a.w3, b.w3, borrow);
  return borrow;
}

uint64_t uint256_t::cnd_add_raw(bool flag, const uint256_t& a) {
  uint64_t mask = constant_time_mask_64(flag);
  uint64_t s0 = a.w0 & mask;
  uint64_t s1 = a.w1 & mask;
  uint64_t s2 = a.w2 & mask;
  uint64_t s3 = a.w3 & mask;

  uint64_t carry = 0;
  w0 = addx(w0, s0, carry);
  w1 = addx(w1, s1, carry);
  w2 = addx(w2, s2, carry);
  w3 = addx(w3, s3, carry);
  return carry;
}

uint64_t uint256_t::cnd_sub_raw(bool flag, const uint256_t& a) {
  uint64_t mask = constant_time_mask_64(flag);
  uint64_t s0 = a.w0 & mask;
  uint64_t s1 = a.w1 & mask;
  uint64_t s2 = a.w2 & mask;
  uint64_t s3 = a.w3 & mask;

  uint64_t borrow = 0;
  w0 = subx(w0, s0, borrow);
  w1 = subx(w1, s1, borrow);
  w2 = subx(w2, s2, borrow);
  w3 = subx(w3, s3, borrow);
  return borrow;
}

uint64_t uint256_t::cnd_neg_raw(bool flag) {
  uint64_t mask = constant_time_mask_64(flag);
  uint64_t s0 = w0 ^ mask;
  uint64_t s1 = w1 ^ mask;
  uint64_t s2 = w2 ^ mask;
  uint64_t s3 = w3 ^ mask;

  uint64_t carry = 0;
  w0 = addx(s0, uint64_t(flag), carry);
  w1 = addx(s1, 0, carry);
  w2 = addx(s2, 0, carry);
  w3 = addx(s3, 0, carry);
  return carry;
}

void uint256_t::cnd_swap(bool flag, uint256_t& a, uint256_t& b)  // static
{
  uint64_t mask = constant_time_mask_64(flag);
  uint64_t delta;
  delta = (a.w0 ^ b.w0) & mask;
  a.w0 ^= delta;
  b.w0 ^= delta;
  delta = (a.w1 ^ b.w1) & mask;
  a.w1 ^= delta;
  b.w1 ^= delta;
  delta = (a.w2 ^ b.w2) & mask;
  a.w2 ^= delta;
  b.w2 ^= delta;
  delta = (a.w3 ^ b.w3) & mask;
  a.w3 ^= delta;
  b.w3 ^= delta;
}

void uint256_t::inv_mod(const uint256_t& x, const uint256_t& m) {
  uint256_t u = {1};
  uint256_t& v = *this = uint256_t{0};

  uint256_t a = x;
  uint256_t b = m;

  uint256_t mp1o2;
  mp1o2.add_raw(m, u);
  mp1o2.div_by_two();

  for (int i = 0; i < 512; i++) {
    bool a_is_odd = a.is_odd();
    bool underflow = bool(a.cnd_sub_raw(a_is_odd, b));
    b.cnd_add_raw(underflow, a);
    a.cnd_neg_raw(underflow);
    cnd_swap(underflow, u, v);
    a.div_by_two();
    bool borrow = bool(u.cnd_sub_raw(a_is_odd, v));
    u.cnd_add_raw(borrow, m);
    bool u_is_odd = u.is_odd();
    u.div_by_two();
    u.cnd_add_raw(u_is_odd, mp1o2);
  }
}

uint256_t uint256_t::get_mont_rr(const uint256_t& mod)  //  (1<<512) % m
{
  bn_t p = bn_t(1) << 512;
  BN_div(nullptr, p, p, mod.to_bn(), bn_t::thread_local_storage_bn_ctx());

  return from_bn(p);
}

// ------------------------- uint320_t --------------------

void uint320_t::from_bn(const bn_t& bn) {
  buf_t bin = bn.to_bin(40);
  w0 = coinbase::be_get_8(bin.data() + 32);
  w1 = coinbase::be_get_8(bin.data() + 24);
  w2 = coinbase::be_get_8(bin.data() + 16);
  w3 = coinbase::be_get_8(bin.data() + 8);
  w4 = coinbase::be_get_8(bin.data() + 0);
}

uint320_t uint320_t::get_barrett_mu(const uint256_t& mod)  //  ((1<<512) / m) % (1<<320)
{
  bn_t p = bn_t(1) << 512;
  BN_div(p, nullptr, p, mod.to_bn(), bn_t::thread_local_storage_bn_ctx());

  BN_mask_bits(p, 320);

  uint320_t mu;
  mu.from_bn(p);
  return mu;
}

// bn256_t
bn256_t::bn256_t() : w0(0), w1(0), w2(0), w3(0) {}
bn256_t::~bn256_t() { secure_bzero(byte_ptr(this), 32); }
bn256_t::bn256_t(const bn256_t& x) : w0(x.w0), w1(x.w1), w2(x.w2), w3(x.w3) {}
bn256_t::bn256_t(uint64_t x) : w0(x), w1(0), w2(0), w3(0) {}

bn256_t& bn256_t::operator=(const bn256_t& x) {
  w0 = x.w0;
  w1 = x.w1;
  w2 = x.w2;
  w3 = x.w3;
  return *this;
}

bn256_t& bn256_t::operator=(uint64_t x) {
  w0 = x;
  w1 = w2 = w3 = 0;
  return *this;
}

bool bn256_t::operator==(const bn256_t& b) const {
  uint64_t x = (w0 ^ b.w0) | (w1 ^ b.w1) | (w2 ^ b.w2) | (w3 ^ b.w3);
  return x == 0;
}

bool bn256_t::operator!=(const bn256_t& b) const {
  uint64_t x = (w0 ^ b.w0) | (w1 ^ b.w1) | (w2 ^ b.w2) | (w3 ^ b.w3);
  return x != 0;
}

bool bn256_t::operator==(uint64_t b) const {
  uint64_t x = (w0 ^ b) | w1 | w2 | w3;
  return x == 0;
}

bool bn256_t::operator!=(uint64_t b) const {
  uint64_t x = (w0 ^ b) | w1 | w2 | w3;
  return x != 0;
}

bn256_t bn256_t::two_to_pow(int n) {
  cb_assert(n >= 0 && n < 256);
  bn256_t r;
  if (n >= 64 + 64 + 64)
    r.w3 = uint64_t(1) << (n - (64 + 64 + 64));
  else if (n >= 64 + 64)
    r.w2 = uint64_t(1) << (n - (64 + 64));
  else if (n >= 64)
    r.w1 = uint64_t(1) << (n - 64);
  else
    r.w0 = uint64_t(1) << n;
  return r;
}

bn256_t bn256_t::operator+(const bn256_t& b) const {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  const auto m = mod->value().val.d;
  bn256_t r;
  const bn256_t& a = *this;

  uint64_t carry = 0;
  uint64_t a0 = addx(a.w0, b.w0, carry);
  uint64_t a1 = addx(a.w1, b.w1, carry);
  uint64_t a2 = addx(a.w2, b.w2, carry);
  uint64_t a3 = addx(a.w3, b.w3, carry);

  uint64_t borrow = 0;
  uint64_t s0 = subx(a0, m[0], borrow);
  uint64_t s1 = subx(a1, m[1], borrow);
  uint64_t s2 = subx(a2, m[2], borrow);
  uint64_t s3 = subx(a3, m[3], borrow);

  subx(carry, 0, borrow);
  uint64_t mask = -uint64_t(bool(borrow));
  r.w0 = s0 ^ ((a0 ^ s0) & mask);
  r.w1 = s1 ^ ((a1 ^ s1) & mask);
  r.w2 = s2 ^ ((a2 ^ s2) & mask);
  r.w3 = s3 ^ ((a3 ^ s3) & mask);
  return r;
}

bn256_t bn256_t::operator-(const bn256_t& b) const {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  const auto m = mod->value().val.d;
  bn256_t r;
  const bn256_t& a = *this;

  uint64_t borrow = 0;
  uint64_t s0 = subx(a.w0, b.w0, borrow);
  uint64_t s1 = subx(a.w1, b.w1, borrow);
  uint64_t s2 = subx(a.w2, b.w2, borrow);
  uint64_t s3 = subx(a.w3, b.w3, borrow);

  uint64_t mask = -uint64_t(bool(borrow));
  uint64_t t0 = m[0] & mask;
  uint64_t t1 = m[1] & mask;
  uint64_t t2 = m[2] & mask;
  uint64_t t3 = m[3] & mask;

  uint64_t carry = 0;
  r.w0 = addx(s0, t0, carry);
  r.w1 = addx(s1, t1, carry);
  r.w2 = addx(s2, t2, carry);
  r.w3 = addx(s3, t3, carry);
  return r;
}

bn256_t bn256_t::operator-() const {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  const auto m = mod->value().val.d;
  bn256_t r;
  const bn256_t& b = *this;

  uint64_t borrow = 0;
  uint64_t s0 = subx(0, b.w0, borrow);
  uint64_t s1 = subx(0, b.w1, borrow);
  uint64_t s2 = subx(0, b.w2, borrow);
  uint64_t s3 = subx(0, b.w3, borrow);

  uint64_t mask = -uint64_t(borrow);
  uint64_t t0 = m[0] & mask;
  uint64_t t1 = m[1] & mask;
  uint64_t t2 = m[2] & mask;
  uint64_t t3 = m[3] & mask;

  uint64_t carry = 0;
  r.w0 = addx(s0, t0, carry);
  r.w1 = addx(s1, t1, carry);
  r.w2 = addx(s2, t2, carry);
  r.w3 = addx(s3, t3, carry);
  return r;
}

bn256_t& bn256_t::operator+=(const bn256_t& b) { return *this = *this + b; }
bn256_t& bn256_t::operator-=(const bn256_t& b) { return *this = *this - b; }
bn256_t& bn256_t::operator*=(const bn256_t& b) { return *this = *this * b; }
bn256_t& bn256_t::operator/=(const bn256_t& b) { return *this = *this / b; }

void bn256_t::to_bin(byte_ptr bin) const {
  coinbase::be_set_8(&bin[24], w0);
  coinbase::be_set_8(&bin[16], w1);
  coinbase::be_set_8(&bin[8], w2);
  coinbase::be_set_8(&bin[0], w3);
}

buf_t bn256_t::to_bin() const {
  buf_t r(32);
  to_bin(r.data());
  return r;
}

bn256_t bn256_t::from_bin(mem_t bin) {
  cb_assert(bin.size == 32);
  bn256_t r;
  r.w0 = coinbase::be_get_8(bin.data + 24);
  r.w1 = coinbase::be_get_8(bin.data + 16);
  r.w2 = coinbase::be_get_8(bin.data + 8);
  r.w3 = coinbase::be_get_8(bin.data + 0);
  return r;
}

bn256_t::bn256_t(const bn_t& x) { *this = from_bin(x.to_bin(32)); }

bn256_t& bn256_t::operator=(const bn_t& x) { return *this = from_bin(x.to_bin(32)); }

bn256_t::operator bn_t() const { return bn_t::from_bin(to_bin()); }

bn256_t bn256_t::rand(const mod_t& q) { return bn_t::rand(q); }

int bn256_t::get_bit(int b) const {
  if (b >= 256) return 0;
  if (b < 64) return (w0 >> b) & 1;
  if (b < 64 + 64) return (w1 >> (b - 64)) & 1;
  if (b < 64 + 64 + 64) return (w2 >> (b - 64 - 64)) & 1;
  return (w3 >> (b - 64 - 64 - 64)) & 1;
}

void bn256_t::convert(coinbase::converter_t& converter) {
  if (converter.is_write()) {
    if (!converter.is_calc_size()) to_bin(converter.current());
  } else {
    if (converter.is_error() || !converter.at_least(32)) {
      converter.set_error();
      return;
    }
    *this = from_bin(mem_t(converter.current(), 32));
  }
  converter.forward(32);
}

using uint128_t = unsigned __int128;

template <int m>
static void barrett_partial_mul_row(uint64_t r[], const uint64_t u[], uint64_t v) {
  uint64_t k = 0;
  for (auto i = 0; i < m; ++i) {
    uint128_t t = uint128_t(u[i]) * v + r[i] + k;
    r[i] = uint64_t(t);
    k = t >> 64;
  }
}

#ifdef __x86_64__
typedef void (*barrett_reduce256_f)(uint64_t res[4], uint64_t x[8], const uint64_t* m, const uint64_t* mu);
static barrett_reduce256_f barrett_reduce256 =
    support_x64_mulx() ? barrett_reduce256_intel_mulx : barrett_reduce256_noasm;
#endif

void bn256_t::barrett_reduce(uint64_t x[8], const uint64_t* m, const uint64_t* mu) {
#ifdef __x86_64__
  barrett_reduce256((uint64_t*)this, x, m, mu);
#else
  barrett_reduce256_noasm((uint64_t*)this, x, m, mu);
#endif
}

bn256_t bn256_t::operator*(const bn256_t& b) const {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  const auto m = mod->value().val.d;
  const auto mu = mod->get_barrett_mu().val.d;

  uint64_t temp[8];
  uint256_t::mul(temp, (const uint256_t&)*this, (const uint256_t&)b);
  bn256_t r;
  r.barrett_reduce(temp, (const uint64_t*)m, (const uint64_t*)mu);
  return r;
}

bn256_t bn256_t::operator/(const bn256_t& b) const {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  bn256_t c = b.inv_mod(*mod);
  return *this * c;
}

void bn256_t::add_no_reduce(uint64_t r[8], const bn256_t& a) {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  const auto m = mod->value().val.d;

  uint64_t carry = 0;
  uint64_t a0 = addx(a.w0, r[0], carry);
  uint64_t a1 = addx(a.w1, r[1], carry);
  uint64_t a2 = addx(a.w2, r[2], carry);
  uint64_t a3 = addx(a.w3, r[3], carry);
  uint64_t a4 = addx(0, r[4], carry);
  uint64_t a5 = addx(0, r[5], carry);
  uint64_t a6 = addx(0, r[6], carry);
  uint64_t a7 = addx(0, r[7], carry);

  uint64_t borrow = 0;
  uint64_t s4 = subx(a4, m[0], borrow);
  uint64_t s5 = subx(a5, m[1], borrow);
  uint64_t s6 = subx(a6, m[2], borrow);
  uint64_t s7 = subx(a7, m[3], borrow);

  subx(carry, 0, borrow);
  uint64_t mask = -uint64_t(bool(borrow));
  r[0] = a0;
  r[1] = a1;
  r[2] = a2;
  r[3] = a3;
  r[4] = s4 ^ ((a4 ^ s4) & mask);
  r[5] = s5 ^ ((a5 ^ s5) & mask);
  r[6] = s6 ^ ((a6 ^ s6) & mask);
  r[7] = s7 ^ ((a7 ^ s7) & mask);
}

void bn256_t::mul_add_no_reduce(uint64_t r[8], const bn256_t& a, const bn256_t& b) {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  const auto m = mod->value().val.d;

  uint64_t temp[8];
  uint256_t::mul(temp, (const uint256_t&)a, (const uint256_t&)b);

  uint64_t carry = 0;
  uint64_t a0 = addx(temp[0], r[0], carry);
  uint64_t a1 = addx(temp[1], r[1], carry);
  uint64_t a2 = addx(temp[2], r[2], carry);
  uint64_t a3 = addx(temp[3], r[3], carry);
  uint64_t a4 = addx(temp[4], r[4], carry);
  uint64_t a5 = addx(temp[5], r[5], carry);
  uint64_t a6 = addx(temp[6], r[6], carry);
  uint64_t a7 = addx(temp[7], r[7], carry);

  uint64_t borrow = 0;
  uint64_t s4 = subx(a4, m[0], borrow);
  uint64_t s5 = subx(a5, m[1], borrow);
  uint64_t s6 = subx(a6, m[2], borrow);
  uint64_t s7 = subx(a7, m[3], borrow);

  subx(carry, 0, borrow);
  uint64_t mask = -uint64_t(bool(borrow));
  r[0] = a0;
  r[1] = a1;
  r[2] = a2;
  r[3] = a3;
  r[4] = s4 ^ ((a4 ^ s4) & mask);
  r[5] = s5 ^ ((a5 ^ s5) & mask);
  r[6] = s6 ^ ((a6 ^ s6) & mask);
  r[7] = s7 ^ ((a7 ^ s7) & mask);
}

bn256_t bn256_t::reduce(uint64_t x[8]) {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  const auto m = mod->value().val.d;
  const auto mu = mod->get_barrett_mu().val.d;

  bn256_t r;
  r.barrett_reduce(x, (const uint64_t*)m, (const uint64_t*)mu);
  return r;
}

bn256_t bn256_t::inv_mod(const mod_t& q) const {
  uint256_t m = uint256_t::from_bn(q.value());
  bn256_t r;
  ((uint256_t&)r).inv_mod((const uint256_t&)*this, m);
  return r;
}

void bn256_t::mont_mul(const bn256_t& a, const bn256_t& b, const uint64_t* m, uint64_t mont_prime) {
  bn256_t& r = *this;
  uint64_t carry = 0;
  uint64_t y;

  uint128_t t;
  uint64_t h;
  uint64_t t0 = 0, t1 = 0, t2 = 0, t3 = 0, t4;
  uint64_t a0 = a.w0, a1 = a.w1, a2 = a.w2, a3 = a.w3;

  // -----------------------------------------------------------------------
  y = b.w0;
  t = uint128_t(a0) * y;
  t0 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a1) * y + h;
  t1 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a2) * y + h;
  t2 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a3) * y + h;
  t3 = uint64_t(t);
  t4 = t >> 64;

  y = mont_prime * t0;
  t = uint128_t(m[0]) * y + t0;
  h = t >> 64;
  t = uint128_t(m[1]) * y + t1 + h;
  t1 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[2]) * y + t2 + h;
  t2 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[3]) * y + t3 + h;
  t3 = uint64_t(t);
  h = t >> 64;
  t4 = addx(t4, h, carry);

  // -----------------------------------------------------------------------
  y = b.w1;
  t = uint128_t(a0) * y + t1;
  t1 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a1) * y + t2 + h;
  t2 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a2) * y + t3 + h;
  t3 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a3) * y + t4 + h;
  t4 = uint64_t(t);
  t0 = t >> 64;

  y = mont_prime * t1;
  t = uint128_t(m[0]) * y + t1;
  h = t >> 64;
  t = uint128_t(m[1]) * y + t2 + h;
  t2 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[2]) * y + t3 + h;
  t3 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[3]) * y + t4 + h;
  t4 = uint64_t(t);
  h = t >> 64;
  t0 = addx(t0, h, carry);

  // -----------------------------------------------------------------------
  y = b.w2;
  t = uint128_t(a0) * y + t2;
  t2 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a1) * y + t3 + h;
  t3 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a2) * y + t4 + h;
  t4 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a3) * y + t0 + h;
  t0 = uint64_t(t);
  t1 = t >> 64;

  y = mont_prime * t2;
  t = uint128_t(m[0]) * y + t2;
  h = t >> 64;
  t = uint128_t(m[1]) * y + t3 + h;
  t3 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[2]) * y + t4 + h;
  t4 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[3]) * y + t0 + h;
  t0 = uint64_t(t);
  h = t >> 64;
  t1 = addx(t1, h, carry);

  // -----------------------------------------------------------------------
  y = b.w3;
  t = uint128_t(a0) * y + t3;
  t3 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a1) * y + t4 + h;
  t4 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a2) * y + t0 + h;
  t0 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(a3) * y + t1 + h;
  t1 = uint64_t(t);
  t2 = t >> 64;

  y = mont_prime * t3;
  t = uint128_t(m[0]) * y + t3;
  h = t >> 64;
  t = uint128_t(m[1]) * y + t4 + h;
  t4 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[2]) * y + t0 + h;
  t0 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[3]) * y + t1 + h;
  t1 = uint64_t(t);
  h = t >> 64;
  t2 = addx(t2, h, carry);

  // -----------------------------------------------------------------------

  uint64_t borrow = 0;
  uint64_t s0 = subx(t4, m[0], borrow);
  uint64_t s1 = subx(t0, m[1], borrow);
  uint64_t s2 = subx(t1, m[2], borrow);
  uint64_t s3 = subx(t2, m[3], borrow);

  uint64_t mask = -uint64_t(bool(borrow - carry));
  r.w0 = s0 ^ ((t4 ^ s0) & mask);
  r.w1 = s1 ^ ((t0 ^ s1) & mask);
  r.w2 = s2 ^ ((t1 ^ s2) & mask);
  r.w3 = s3 ^ ((t2 ^ s3) & mask);
}

void bn256_t::mont_reduce(uint64_t u[8], const uint64_t* m, uint64_t mont_prime) {
  bn256_t& r = *this;
  uint64_t carry = 0;
  uint128_t t;
  uint64_t y, h, u0, u1, u2, u3, u4;

  y = mont_prime * u[0];
  t = uint128_t(m[0]) * y + u[0];
  h = t >> 64;
  t = uint128_t(m[1]) * y + u[1] + h;
  u1 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[2]) * y + u[2] + h;
  u2 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[3]) * y + u[3] + h;
  u3 = uint64_t(t);
  h = t >> 64;
  u4 = addx(u[4], h, carry);

  y = mont_prime * u1;
  t = uint128_t(m[0]) * y + u1;
  h = t >> 64;
  t = uint128_t(m[1]) * y + u2 + h;
  u2 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[2]) * y + u3 + h;
  u3 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[3]) * y + u4 + h;
  u4 = uint64_t(t);
  h = t >> 64;
  u0 = addx(u[5], h, carry);

  y = mont_prime * u2;
  t = uint128_t(m[0]) * y + u2;
  h = t >> 64;
  t = uint128_t(m[1]) * y + u3 + h;
  u3 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[2]) * y + u4 + h;
  u4 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[3]) * y + u0 + h;
  u0 = uint64_t(t);
  h = t >> 64;
  u1 = addx(u[6], h, carry);

  y = mont_prime * u3;
  t = uint128_t(m[0]) * y + u3;
  h = t >> 64;
  t = uint128_t(m[1]) * y + u4 + h;
  u4 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[2]) * y + u0 + h;
  u0 = uint64_t(t);
  h = t >> 64;
  t = uint128_t(m[3]) * y + u1 + h;
  u1 = uint64_t(t);
  h = t >> 64;
  u2 = addx(u[7], h, carry);

  uint64_t borrow = 0;
  uint64_t s0 = subx(u4, m[0], borrow);
  uint64_t s1 = subx(u0, m[1], borrow);
  uint64_t s2 = subx(u1, m[2], borrow);
  uint64_t s3 = subx(u2, m[3], borrow);

  uint64_t mask = -uint64_t(bool(borrow - carry));
  r.w0 = s0 ^ ((u4 ^ s0) & mask);
  r.w1 = s1 ^ ((u0 ^ s1) & mask);
  r.w2 = s2 ^ ((u1 ^ s2) & mask);
  r.w3 = s3 ^ ((u2 ^ s3) & mask);
}

bn256_t bn256_t::to_mont() const {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  const auto m = mod->value().val.d;
  auto prime = mod->get_mont_ctx()->n0[0];
  auto rr = mod->get_mont_ctx()->RR.d;

  // static uint256_t RR = uint256_t::get_mont_rr(MOD::w);
  // mont_mul(r, a, RR);

  bn256_t b;
  b.w0 = rr[0];
  b.w1 = rr[1];
  b.w2 = rr[2];
  b.w3 = rr[3];

  bn256_t r;
  r.mont_mul(*this, b, (const uint64_t*)m, prime);
  return r;
}

bn256_t bn256_t::from_mont() const {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  const auto m = mod->value().val.d;
  auto prime = mod->get_mont_ctx()->n0[0];

  uint64_t temp[8] = {w0, w1, w2, w3, 0, 0, 0, 0};
  bn256_t r;
  r.mont_reduce(temp, (const uint64_t*)m, prime);
  return r;
}

bn256_t bn256_t::mont_mul(const bn256_t& a, const bn256_t& b) {
  const mod_t* mod = thread_local_storage_mod();
  cb_assert(mod);
  const auto m = mod->value().val.d;
  auto prime = mod->get_mont_ctx()->n0[0];

  bn256_t r;
  r.mont_mul(a, b, (const uint64_t*)m, prime);
  return r;
}

}  // namespace coinbase::crypto
