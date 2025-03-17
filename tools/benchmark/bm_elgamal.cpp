#include <benchmark/benchmark.h>

#include <cbmpc/crypto/elgamal.h>

#include "util.h"

using namespace coinbase::crypto;

static void BM_ElGamalLocalKeygen(benchmark::State& state)
{
  ecurve_t curve = get_curve(state.range(0));
  // ec_elgamal_commitment_t elgamal(curve);
  ecc_point_t pub_key;
  bn_t priv_key;
  for (auto _ : state)
  {
    std::tie(pub_key, priv_key) = ec_elgamal_commitment_t::local_keygen(curve);
  }
}
BENCHMARK(BM_ElGamalLocalKeygen)->Name("BP/ElGamal/LocalKeyGen")->ArgsProduct({{3, 4}});

static void BM_ElGamalCommit(benchmark::State& state)
{
  ecurve_t curve = get_curve(state.range(0));

  auto [P, k] = ec_elgamal_commitment_t::local_keygen(curve);
  bn_t m = curve.get_random_value();
  ec_elgamal_commitment_t elgamal;

  for (auto _ : state)
  {
    elgamal = ec_elgamal_commitment_t::random_commit(P, m);
  }
}
BENCHMARK(BM_ElGamalCommit)->Name("BP/ElGamal/Commit")->ArgsProduct({{3, 4}});

static void BM_ElGamalRerand(benchmark::State& state)
{
  ecurve_t curve = get_curve(state.range(0));

  auto [P, k] = ec_elgamal_commitment_t::local_keygen(curve);
  bn_t m = curve.get_random_value();
  ec_elgamal_commitment_t elgamal = ec_elgamal_commitment_t::random_commit(P, m);

  bn_t rerand_r = curve.get_random_value();
  ec_elgamal_commitment_t rerand_elgamal;
  for (auto _ : state)
  {
    rerand_elgamal = elgamal.rerand(P, rerand_r);
  }
}
BENCHMARK(BM_ElGamalRerand)->Name("BP/ElGamal/Rerand")->ArgsProduct({{3, 4}});

static void BM_ElGamalAdd(benchmark::State& state)
{
  ecurve_t curve = get_curve(state.range(0));

  auto [P, k] = ec_elgamal_commitment_t::local_keygen(curve);
  bn_t m1 = curve.get_random_value();
  bn_t m2 = curve.get_random_value();
  ec_elgamal_commitment_t elgamal1 = ec_elgamal_commitment_t::random_commit(P, m1);
  ec_elgamal_commitment_t elgamal2 = ec_elgamal_commitment_t::random_commit(P, m2);

  ec_elgamal_commitment_t elgamal_res;
  for (auto _ : state)
  {
    elgamal_res = elgamal1 + elgamal2;
  }
}
BENCHMARK(BM_ElGamalAdd)->Name("BP/ElGamal/Add")->ArgsProduct({{3, 4}});

static void BM_ElGamalAddScalar(benchmark::State& state)
{
  ecurve_t curve = get_curve(state.range(0));

  auto [P, k] = ec_elgamal_commitment_t::local_keygen(curve);
  bn_t m1 = curve.get_random_value();
  bn_t m2 = curve.get_random_value();
  ec_elgamal_commitment_t elgamal = ec_elgamal_commitment_t::random_commit(P, m1);

  ec_elgamal_commitment_t elgamal_res;
  for (auto _ : state)
  {
    vartime_scope_t vartime_scope;
    elgamal_res = elgamal + m2;
  }
}
BENCHMARK(BM_ElGamalAddScalar)->Name("BP/ElGamal/AddScalar")->ArgsProduct({{3, 4}});

static void BM_ElGamalMulScalar(benchmark::State& state)
{
  ecurve_t curve = get_curve(state.range(0));

  auto [P, k] = ec_elgamal_commitment_t::local_keygen(curve);
  bn_t m1 = curve.get_random_value();
  bn_t m2 = curve.get_random_value();
  ec_elgamal_commitment_t elgamal = ec_elgamal_commitment_t::random_commit(P, m1);

  ec_elgamal_commitment_t elgamal_res;
  for (auto _ : state)
  {
    elgamal_res = elgamal * m2;
  }
}
BENCHMARK(BM_ElGamalMulScalar)->Name("BP/ElGamal/MulScalar")->ArgsProduct({{3, 4}});
