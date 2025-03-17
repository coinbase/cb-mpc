#include <benchmark/benchmark.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

#define bit_len_lb 1 << 8
#define bit_len_ub 1 << 12

using namespace coinbase::crypto;

static void BM_SHA256(benchmark::State& state) {
  buf_t input = gen_random(state.range(0));
  for (auto _ : state) auto _dummy = sha256_t::hash(input);
}
BENCHMARK(BM_SHA256)->Name("Core/Hash/SHA256")->RangeMultiplier(4)->Range(1, 4096);

static void BM_HMAC_SHA256(benchmark::State& state) {
  buf_t input = gen_random(state.range(0));
  buf_t key = gen_random(16);

  for (auto _ : state) {
    hmac_sha256_t hmac(key);
    hmac.calculate(input);
  }
}
BENCHMARK(BM_SHA256)->Name("Core/Hash/HMAC-SHA256")->RangeMultiplier(4)->Range(1, 4096);

static void BM_AEC_GCM_128(benchmark::State& state) {
  buf_t input = gen_random(state.range(0));

  buf_t key = gen_random(16);
  buf_t iv = gen_random(12);

  buf_t output;
  for (auto _ : state) aes_gcm_t::encrypt(key, iv, mem_t(), 12, input, output);
}
BENCHMARK(BM_AEC_GCM_128)->Name("Core/Hash/AES-GCM-128")->RangeMultiplier(4)->Range(1, 4096);

static void BM_AEC_GCM_256(benchmark::State& state) {
  buf_t input = gen_random(state.range(0));

  buf_t key = gen_random(32);
  buf_t iv = gen_random(12);

  buf_t output;
  for (auto _ : state) aes_gcm_t::encrypt(key, iv, mem_t(), 12, input, output);
}
BENCHMARK(BM_AEC_GCM_128)->Name("Core/Hash/AES-GCM-256")->RangeMultiplier(4)->Range(1, 4096);
