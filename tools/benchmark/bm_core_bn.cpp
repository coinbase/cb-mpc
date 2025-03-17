
#include <benchmark/benchmark.h>

#include <cbmpc/crypto/base.h>

#define bit_len_lb 1 << 8
#define bit_len_ub 1 << 12

static void BM_ModAdd(benchmark::State& state) {
  bn_t q_value;
  q_value = bn_t::generate_prime(state.range(0), false);
  mod_t q(q_value, /* multiplicative_dense */ true);
  bn_t a = bn_t::rand(q);
  bn_t b = bn_t::rand(q);
  for (auto _ : state) MODULO(q) {
      a + b;
    }
}
BENCHMARK(BM_ModAdd)->Name("Core/BN/ModAdd")->RangeMultiplier(2)->Range(bit_len_lb, bit_len_ub);

static void BM_ModSub(benchmark::State& state) {
  bn_t q_value;
  q_value = bn_t::generate_prime(state.range(0), false);
  mod_t q(q_value, /* multiplicative_dense */ true);
  bn_t a = bn_t::rand(q);
  bn_t b = bn_t::rand(q);
  for (auto _ : state) MODULO(q) {
      a - b;
    }
}
// Register the function as a benchmark
BENCHMARK(BM_ModSub)->Name("Core/BN/ModSubtract")->RangeMultiplier(2)->Range(bit_len_lb, bit_len_ub);

static void BM_ModMul(benchmark::State& state) {
  bn_t q_value;
  q_value = bn_t::generate_prime(state.range(0), false);
  mod_t q(q_value, /* multiplicative_dense */ true);
  bn_t a = bn_t::rand(q);
  bn_t b = bn_t::rand(q);
  for (auto _ : state) MODULO(q) {
      a* b;
    }
}
// Register the function as a benchmark
BENCHMARK(BM_ModMul)->Name("Core/BN/ModMultiply")->RangeMultiplier(2)->Range(bit_len_lb, bit_len_ub);

static void BM_ModExp(benchmark::State& state) {
  bn_t q_value;
  q_value = bn_t::generate_prime(state.range(0), false);
  mod_t q(q_value, /* multiplicative_dense */ true);
  bn_t a = bn_t::rand(q);
  bn_t b = bn_t::rand(q);
  for (auto _ : state) MODULO(q) {
      a.pow(b);
    }
}
BENCHMARK(BM_ModExp)->Name("Core/BN/ModExponentiate")->RangeMultiplier(2)->Range(bit_len_lb, bit_len_ub);

static void BM_ModInv(benchmark::State& state) {
  bn_t q_value;
  q_value = bn_t::generate_prime(state.range(0), false);
  mod_t q(q_value, /* multiplicative_dense */ true);
  bn_t a = bn_t::rand(q);
  for (auto _ : state) MODULO(q) {
      a.inv();
    }
}
BENCHMARK(BM_ModInv)->Name("Core/BN/ModInvert")->RangeMultiplier(2)->Range(bit_len_lb, bit_len_ub);

static void BM_GCD(benchmark::State& state) {
  bn_t q_value;
  q_value = bn_t::generate_prime(state.range(0), false);
  mod_t q(q_value, /* multiplicative_dense */ true);
  bn_t a = bn_t::rand(q);
  bn_t b = bn_t::rand(q);
  for (auto _ : state) auto c = bn_t::gcd(a, b);
}
BENCHMARK(BM_GCD)->Name("Core/BN/GCD")->RangeMultiplier(2)->Range(256, 4096);

static void BM_GCDRSAMod(benchmark::State& state) {
  bn_t p, q;
  p = bn_t::generate_prime(state.range(0) / 2, false);
  q = bn_t::generate_prime(state.range(0) / 2, false);
  mod_t N(p * q, /* multiplicative_dense */ true);
  bn_t a = bn_t::rand(N);
  for (auto _ : state) auto c = bn_t::gcd(a, N);
}
BENCHMARK(BM_GCDRSAMod)->Name("Core/BN/GCD-RSA-Modulus")->RangeMultiplier(2)->Range(256, 4096);

static void BM_BatchGCDRSAMod(benchmark::State& state) {
  int n = 16;
  bn_t p, q;
  p = bn_t::generate_prime(state.range(0) / 2, false);
  q = bn_t::generate_prime(state.range(0) / 2, false);
  mod_t N(p * q, /* multiplicative_dense */ true);
  std::vector<bn_t> a(n);
  for (int i = 0; i < n; i++) {
    a[i] = bn_t::rand(N);
  }
  for (auto _ : state) {
    bn_t prod = a[0];
    for (int i = 1; i < n; i++) {
      MODULO(N) { prod = prod * a[i]; };
    }
    auto c = bn_t::gcd(prod, N);
  }
}
BENCHMARK(BM_BatchGCDRSAMod)
    ->Name("Core/BN/GCD-Batch(16)RSA-Modulus")
    ->ArgsProduct({benchmark::CreateRange(256, 4096, /*multi=*/2)});
