#include <benchmark/benchmark.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

#include "util.h"

using namespace coinbase::crypto;

static void DRBG_String(benchmark::State& state)
{
  int u = state.range(0);
  buf_t res;
  for (auto _ : state)
  {
    res = ro::drbg_sample_string(mem_t("test"), u);
  }
}
BENCHMARK(DRBG_String)->Name("Crypto/DRBG/String")->RangeMultiplier(2)->Range(1 << 10, 1 << 18);

static void DRBG_Number(benchmark::State& state)
{
  int u = state.range(0);
  mod_t m(bn_t::generate_prime(u, false), /* multiplicative_dense */ true);
  bn_t res;
  for (auto _ : state)
  {
    res = ro::drbg_sample_number(mem_t("test"), m);
  }
}
BENCHMARK(DRBG_Number)->Name("Crypto/DRBG/Number")->RangeMultiplier(2)->Range(1 << 8, 1 << 12);

static void DRBG_Curve(benchmark::State& state)
{
  ecurve_t curve = get_curve(state.range(0));
  ecc_point_t res;
  for (auto _ : state)
  {
    res = ro::drbg_sample_curve(mem_t("test"), curve);
  }
}
BENCHMARK(DRBG_Curve)->Name("Crypto/DRBG/Curve")->Arg(3)->Arg(4);
