#include <benchmark/benchmark.h>

#include <cbmpc/crypto/base.h>

static void BM_Paillier_Gen(benchmark::State& state)
{
  for (auto _ : state)
  {
    coinbase::crypto::paillier_t paillier;
    paillier.generate();
  }
}
BENCHMARK(BM_Paillier_Gen)->Name("BP/Paillier/Gen");

static void BM_Paillier_Enc(benchmark::State& state)
{
  coinbase::crypto::paillier_t paillier;
  paillier.generate();
  bn_t x = bn_t::rand(paillier.get_N());

  for (auto _ : state)
  {
    paillier.encrypt(x);
  }
}
BENCHMARK(BM_Paillier_Enc)->Name("BP/Paillier/Enc");

static void BM_Paillier_Pub_Enc(benchmark::State& state)
{
  coinbase::crypto::paillier_t paillier;
  paillier.generate();
  coinbase::crypto::paillier_t paillier_pub;
  paillier_pub.create_pub(paillier.get_N());
  bn_t x = bn_t::rand(paillier.get_N());
  for (auto _ : state)
  {
    paillier_pub.encrypt(x);
  }
}
BENCHMARK(BM_Paillier_Pub_Enc)->Name("BP/Paillier/Pub-Enc");

static void BM_Paillier_Dec(benchmark::State& state)
{
  coinbase::crypto::paillier_t paillier;
  paillier.generate();
  bn_t x = bn_t::rand(paillier.get_N());
  auto c_x = paillier.encrypt(x);

  for (auto _ : state)
  {
    paillier.decrypt(c_x);
  }
}
BENCHMARK(BM_Paillier_Dec)->Name("BP/Paillier/Dec");

static void BM_Paillier_Add(benchmark::State& state)
{
  coinbase::crypto::paillier_t paillier;
  paillier.generate();
  bn_t x = bn_t::rand(paillier.get_N());
  bn_t y = bn_t::rand(paillier.get_N());
  auto c_x = paillier.enc(x);
  auto c_y = paillier.enc(y);

  for (auto _ : state)
  {
    auto dummy = c_x + c_y;
  }
}
BENCHMARK(BM_Paillier_Add)->Name("BP/Paillier/Add");

static void BM_Paillier_Add_Scalar(benchmark::State& state)
{
  coinbase::crypto::paillier_t paillier;
  paillier.generate();
  bn_t x = bn_t::rand(paillier.get_N());
  bn_t y = bn_t::rand(paillier.get_N());
  auto c_x = paillier.enc(x);

  for (auto _ : state)
  {
    auto dummy = c_x + y;
  }
}
BENCHMARK(BM_Paillier_Add_Scalar)->Name("BP/Paillier/Add-Scalar");

static void BM_Paillier_Multiply_Scalar(benchmark::State& state)
{
  coinbase::crypto::paillier_t paillier;
  paillier.generate();
  bn_t x = bn_t::rand(paillier.get_N());
  bn_t y = bn_t::rand(paillier.get_N());
  auto c_x = paillier.enc(x);

  for (auto _ : state)
  {
    auto dummy = c_x * y;
  }
}
BENCHMARK(BM_Paillier_Multiply_Scalar)->Name("BP/Paillier/Mul-Scalar");
