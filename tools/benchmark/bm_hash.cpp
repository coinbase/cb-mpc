#include <benchmark/benchmark.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

using namespace coinbase::crypto;

static void BM_SHA256(benchmark::State& state) {
  buf_t input = gen_random(state.range(0));
  for (auto _ : state) {
    auto hash = sha256_t::hash(input);
    benchmark::DoNotOptimize(hash);
  }
}
BENCHMARK(BM_SHA256)->Name("Core/Hash/SHA256")->RangeMultiplier(4)->Range(1 << 4, 1 << 12);

static void BM_HMAC_SHA256(benchmark::State& state) {
  buf_t input = gen_random(state.range(0));
  buf_t key = gen_random(16);

  for (auto _ : state) {
    hmac_sha256_t hmac(key);
    auto mac = hmac.calculate(input);
    benchmark::DoNotOptimize(mac);
  }
}
BENCHMARK(BM_HMAC_SHA256)->Name("Core/Hash/HMAC-SHA256")->RangeMultiplier(4)->Range(1 << 4, 1 << 12);

static void BM_AES_GCM_128(benchmark::State& state) {
  buf_t input = gen_random(state.range(0));

  buf_t key = gen_random(16);
  buf_t iv = gen_random(12);

  buf_t output;
  for (auto _ : state) {
    aes_gcm_t::encrypt(key, iv, mem_t(), 12, input, output);
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(BM_AES_GCM_128)->Name("Core/Hash/AES-GCM-128")->RangeMultiplier(4)->Range(1 << 10, 1 << 22);

static void BM_AES_GCM_256(benchmark::State& state) {
  buf_t input = gen_random(state.range(0));

  buf_t key = gen_random(32);
  buf_t iv = gen_random(12);

  buf_t output;
  for (auto _ : state) {
    aes_gcm_t::encrypt(key, iv, mem_t(), 12, input, output);
    benchmark::DoNotOptimize(output);
  }
}
BENCHMARK(BM_AES_GCM_256)->Name("Core/Hash/AES-GCM-256")->RangeMultiplier(4)->Range(1 << 10, 1 << 22);
