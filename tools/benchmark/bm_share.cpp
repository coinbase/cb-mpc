#include <benchmark/benchmark.h>

#include <cbmpc/crypto/lagrange.h>
#include <cbmpc/crypto/secret_sharing.h>

#include "util.h"

using namespace coinbase::crypto;

static void BM_ShamirShare(benchmark::State& state) {
  int m = state.range(0);
  int n = state.range(1);
  auto curve = curve_ed25519;
  mod_t q = curve.order();
  bn_t a = curve.get_random_value();
  std::vector<bn_t> shares;

  std::vector<bn_t> pids(n);
  for (int i = 0; i < n; i++) pids[i] = curve.get_random_value();

  for (auto _ : state) {
    ss::share_threshold(q, a, m, n, pids);
  }
}
BENCHMARK(BM_ShamirShare)->Name("BP/Share/Shamir")->ArgsProduct({{10, 20, 30}, {2, 3, 4, 5, 6, 7}});

static void BM_HornerPoly(benchmark::State& state) {
  int n = state.range(0);
  auto curve = curve_ed25519;
  mod_t q = curve.order();
  bn_t x = curve.get_random_value();
  std::vector<bn_t> a(n);
  for (int i = 0; i < n; i++) a[i] = curve.get_random_value();

  bn_t res;
  for (auto _ : state) {
    res = horner_poly(q, a, x);
  }
}
BENCHMARK(BM_HornerPoly)->Name("BP/Share/Horner")->ArgsProduct({{3, 4, 6, 8, 10, 16, 32}});

static void BM_ECHornerPoly(benchmark::State& state) {
  int n = state.range(1);
  auto curve = get_curve(state.range(0));
  mod_t q = curve.order();
  bn_t x = curve.get_random_value();
  std::vector<ecc_point_t> A(n);
  for (int i = 0; i < n; i++) A[i] = curve.mul_to_generator(curve.get_random_value());

  ecc_point_t res;
  for (auto _ : state) {
    res = horner_poly(A, x);
  }
}
BENCHMARK(BM_ECHornerPoly)->Name("BP/Share/ECHorner")->ArgsProduct({{3, 4}, {3, 4, 6, 8, 10, 16, 32}});

static void BM_Lagrange(benchmark::State& state) {
  int n = state.range(0);
  auto curve = curve_ed25519;
  mod_t q = curve.order();
  bn_t x = curve.get_random_value();
  std::vector<bn_t> shares(n);
  std::vector<bn_t> pids(n);
  for (int i = 0; i < n; i++) {
    shares[i] = curve.get_random_value();
    pids[i] = curve.get_random_value();
  }

  bn_t res;
  for (auto _ : state) {
    res = lagrange_interpolate(x, shares, pids, q);
  }
}
BENCHMARK(BM_Lagrange)->Name("BP/Share/Lagrange")->ArgsProduct({{3, 4, 6, 8, 10, 16, 32}});

static void BM_ECLagrange(benchmark::State& state) {
  int n = state.range(1);
  auto curve = get_curve(state.range(0));
  mod_t q = curve.order();
  bn_t x = curve.get_random_value();
  std::vector<ecc_point_t> shares(n);
  std::vector<bn_t> pids(n);
  for (int i = 0; i < n; i++) {
    shares[i] = curve.mul_to_generator(curve.get_random_value());
    pids[i] = curve.get_random_value();
  }

  ecc_point_t res;
  for (auto _ : state) {
    vartime_scope_t vartime_scope;
    res = lagrange_interpolate_exponent(x, shares, pids);
  }
}
BENCHMARK(BM_ECLagrange)->Name("BP/Share/ECLagrange")->ArgsProduct({{3, 4}, {3, 4, 6, 8, 10, 16, 32}});
