#include <benchmark/benchmark.h>

#include <cbmpc/crypto/tdh2.h>

#include "data/tdh2.h"

using namespace coinbase;
using namespace coinbase::crypto;

static void BM_tdh2_encrypt(benchmark::State& state) {
  int n = state.range(0);
  int size = state.range(1);

  buf_t plain = crypto::gen_random(size);
  buf_t label = crypto::gen_random(10);

  std::vector<tdh2::private_share_t> dec_shares;
  tdh2::public_key_t enc_key;
  tdh2::pub_shares_t pub_shares;
  testutils::generate_additive_shares(n, enc_key, pub_shares, dec_shares, curve_p256);
  for (auto _ : state) {
    tdh2::ciphertext_t ciphertext = enc_key.encrypt(plain, label);
  }
}
BENCHMARK(BM_tdh2_encrypt)->Name("tdh2-encrypt-1P")->ArgsProduct({{4, 30}, {256, 65536}});

static void BM_tdh2_verify(benchmark::State& state) {
  int n = state.range(0);
  int size = state.range(1);

  buf_t plain = crypto::gen_random(size);
  buf_t label = crypto::gen_random(10);

  std::vector<tdh2::private_share_t> dec_shares;
  tdh2::public_key_t enc_key;
  tdh2::pub_shares_t pub_shares;
  testutils::generate_additive_shares(n, enc_key, pub_shares, dec_shares, curve_p256);
  tdh2::ciphertext_t ciphertext = enc_key.encrypt(plain, label);
  for (auto _ : state) {
    ciphertext.verify(enc_key, label);
  }
}
BENCHMARK(BM_tdh2_verify)->Name("tdh2-verify-1P")->ArgsProduct({{4, 30}, {256, 65536}});

static void BM_tdh2_local_decrypt(benchmark::State& state) {
  int n = state.range(0);
  int size = state.range(1);

  buf_t plain = crypto::gen_random(size);
  buf_t label = crypto::gen_random(10);

  std::vector<tdh2::private_share_t> dec_shares;
  tdh2::public_key_t enc_key;
  tdh2::pub_shares_t pub_shares;
  testutils::generate_additive_shares(n, enc_key, pub_shares, dec_shares, curve_p256);
  tdh2::ciphertext_t ciphertext = enc_key.encrypt(plain, label);
  auto share = dec_shares[0];
  for (auto _ : state) {
    tdh2::partial_decryption_t partial;
    share.decrypt(ciphertext, label, partial);
  }
}
BENCHMARK(BM_tdh2_local_decrypt)->Name("tdh2-local-decrypt-1P")->ArgsProduct({{4, 30}, {256, 65536}});

static void BM_tdh2_combine(benchmark::State& state) {
  int n = state.range(0);
  int size = state.range(1);

  buf_t plain = crypto::gen_random(size);
  buf_t label = crypto::gen_random(10);

  std::vector<tdh2::private_share_t> dec_shares;
  tdh2::public_key_t enc_key;
  tdh2::pub_shares_t pub_shares;
  testutils::generate_additive_shares(n, enc_key, pub_shares, dec_shares, curve_p256);
  tdh2::ciphertext_t ciphertext = enc_key.encrypt(plain, label);

  tdh2::partial_decryptions_t partial_decryptions(n);

  for (int i = 0; i < n; i++) {
    dec_shares[i].decrypt(ciphertext, label, partial_decryptions[i]);
  }

  for (auto _ : state) {
    buf_t decrypted;
    combine_additive(enc_key, pub_shares, label, partial_decryptions, ciphertext, decrypted);
  }
}
BENCHMARK(BM_tdh2_combine)->Name("tdh2-combine-1P")->ArgsProduct({{4, 30}, {256, 65536}});
