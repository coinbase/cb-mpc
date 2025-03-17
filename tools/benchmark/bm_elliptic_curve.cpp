#include <benchmark/benchmark.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

#define bit_len_lb 1 << 8
#define bit_len_ub 1 << 12

using namespace coinbase::crypto;

static void BM_ECAdd(benchmark::State& state, const ecurve_t curve) {
  ecc_point_t P1 = ro::hash_curve(gen_random(8)).curve(curve);
  ecc_point_t P2 = ro::hash_curve(gen_random(8)).curve(curve);

  for (auto _ : state) {
    vartime_scope_t vartime_scope;
    auto _dummy = P1 + P2;
  }
}

static void BM_ECMul(benchmark::State& state, const ecurve_t curve) {
  bn_t x = bn_t::rand(curve.order());
  ecc_point_t P = ro::hash_curve(gen_random(8)).curve(curve);

  for (auto _ : state) {
    auto _dummy = x * P;
  }
}

static void BM_ECMulG(benchmark::State& state, const ecurve_t curve) {
  bn_t x = bn_t::rand(curve.order());
  auto G = curve.generator();

  for (auto _ : state) {
    auto _dummy = x * G;
  }
}

static void BM_ECMulAdd(benchmark::State& state, const ecurve_t curve) {
  bn_t x = bn_t::rand(curve.order());
  bn_t m = bn_t::rand(curve.order());
  bn_t r = bn_t::rand(curve.order());
  const auto& G = curve.generator();
  ecc_point_t P = r * G;

  for (auto _ : state) {
    auto _dummy = curve.mul_add(x, P, m);
  }
}

#define BM_CURVE(name, f, ...) BENCHMARK_SP(f, #name, __VA_ARGS__);
#define BENCHMARK_SP(func, test_case_name, ...)                                                       \
  BENCHMARK_PRIVATE_DECLARE(func) =                                                                   \
      (::benchmark::internal::RegisterBenchmarkInternal(new ::benchmark::internal::FunctionBenchmark( \
          test_case_name, [](::benchmark::State& st) { func(st, __VA_ARGS__); })))

// clang-format off
BM_CURVE(Core/EC/Add/secp256k1, BM_ECAdd, curve_secp256k1);
BM_CURVE(Core/EC/Add/Ed25519, BM_ECAdd, curve_ed25519);

BM_CURVE(Core/EC/Multiply/secp256k1, BM_ECMul, curve_secp256k1);
BM_CURVE(Core/EC/Multiply/Ed25519, BM_ECMul, curve_ed25519);

BM_CURVE(Core/EC/Multiply_G/secp256k1, BM_ECMulG, curve_secp256k1);
BM_CURVE(Core/EC/Multiply_G/Ed25519, BM_ECMulG, curve_ed25519);

BM_CURVE(Core/EC/MulAdd/secp256k1, BM_ECMulAdd, curve_secp256k1);
BM_CURVE(Core/EC/MulAdd/Ed25519, BM_ECMulAdd, curve_ed25519);
