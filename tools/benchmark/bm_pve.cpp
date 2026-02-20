#include <benchmark/benchmark.h>

#include <cbmpc/internal/crypto/secret_sharing.h>
#include <cbmpc/internal/protocol/pve.h>
#include <cbmpc/internal/protocol/pve_ac.h>
#include <cbmpc/internal/protocol/pve_batch.h>

#include "data/test_node.h"
#include "util.h"

using namespace coinbase;
using namespace coinbase::mpc;

namespace {
constexpr int RSA_KEY_BITS = 2048;
}  // namespace

static void BM_PVE_Encrypt(benchmark::State& state) {
  ec_pve_t pve;

  const auto& curve = crypto::curve_p256;
  const mod_t q = curve.order();
  bn_t x = bn_t::rand(q);

  const pve_base_pke_i* base_pke = nullptr;
  pve_keyref_t ek;

  crypto::rsa_prv_key_t rsa_prv_key;
  crypto::rsa_pub_key_t rsa_pub_key;
  crypto::ecc_prv_key_t ecc_prv_key;
  crypto::ecc_pub_key_t ecc_pub_key;

  if (state.range(0) == 0) {
    rsa_prv_key.generate(RSA_KEY_BITS);
    rsa_pub_key = rsa_prv_key.pub();
    base_pke = &pve_base_pke_rsa();
    ek = pve_keyref(rsa_pub_key);
  } else {
    ecc_prv_key.generate(curve);
    ecc_pub_key = ecc_prv_key.pub();
    base_pke = &pve_base_pke_ecies();
    ek = pve_keyref(ecc_pub_key);
  }

  for (auto _ : state) {
    auto rv = pve.encrypt(*base_pke, ek, "test-label", curve, x);
    benchmark::DoNotOptimize(rv);
  }
}

static void BM_PVE_Verify(benchmark::State& state) {
  ec_pve_t pve;

  const auto& curve = crypto::curve_p256;
  const mod_t q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();
  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;

  const pve_base_pke_i* base_pke = nullptr;
  pve_keyref_t ek;

  crypto::rsa_prv_key_t rsa_prv_key;
  crypto::rsa_pub_key_t rsa_pub_key;
  crypto::ecc_prv_key_t ecc_prv_key;
  crypto::ecc_pub_key_t ecc_pub_key;

  if (state.range(0) == 0) {
    rsa_prv_key.generate(RSA_KEY_BITS);
    rsa_pub_key = rsa_prv_key.pub();
    base_pke = &pve_base_pke_rsa();
    ek = pve_keyref(rsa_pub_key);
  } else {
    ecc_prv_key.generate(curve);
    ecc_pub_key = ecc_prv_key.pub();
    base_pke = &pve_base_pke_ecies();
    ek = pve_keyref(ecc_pub_key);
  }

  pve.encrypt(*base_pke, ek, "test-label", curve, x);

  for (auto _ : state) {
    auto rv = pve.verify(*base_pke, ek, X, "test-label");
    benchmark::DoNotOptimize(rv);
  }
}

static void BM_PVE_Decrypt(benchmark::State& state) {
  ec_pve_t pve;

  const auto& curve = crypto::curve_p256;
  const mod_t q = curve.order();
  bn_t x = bn_t::rand(q);

  const pve_base_pke_i* base_pke = nullptr;
  pve_keyref_t ek;
  pve_keyref_t dk;

  crypto::rsa_prv_key_t rsa_prv_key;
  crypto::rsa_pub_key_t rsa_pub_key;
  crypto::ecc_prv_key_t ecc_prv_key;
  crypto::ecc_pub_key_t ecc_pub_key;

  if (state.range(0) == 0) {
    rsa_prv_key.generate(RSA_KEY_BITS);
    rsa_pub_key = rsa_prv_key.pub();
    base_pke = &pve_base_pke_rsa();
    ek = pve_keyref(rsa_pub_key);
    dk = pve_keyref(rsa_prv_key);
  } else {
    ecc_prv_key.generate(curve);
    ecc_pub_key = ecc_prv_key.pub();
    base_pke = &pve_base_pke_ecies();
    ek = pve_keyref(ecc_pub_key);
    dk = pve_keyref(ecc_prv_key);
  }

  pve.encrypt(*base_pke, ek, "test-label", curve, x);

  for (auto _ : state) {
    bn_t decrypted;
    auto rv = pve.decrypt(*base_pke, dk, ek, "test-label", curve, decrypted);
    benchmark::DoNotOptimize(rv);
    benchmark::DoNotOptimize(decrypted);
  }
}

static void BM_PVE_Batch_Encrypt(benchmark::State& state) {
  int n = state.range(1);
  ec_pve_batch_t pve(n);

  const auto& curve = crypto::curve_p256;
  const mod_t q = curve.order();
  std::vector<bn_t> x(n);
  for (int i = 0; i < n; i++) {
    x[i] = bn_t::rand(q);
  }

  const pve_base_pke_i* base_pke = nullptr;
  pve_keyref_t ek;

  crypto::rsa_prv_key_t rsa_prv_key;
  crypto::rsa_pub_key_t rsa_pub_key;
  crypto::ecc_prv_key_t ecc_prv_key;
  crypto::ecc_pub_key_t ecc_pub_key;

  if (state.range(0) == 0) {
    rsa_prv_key.generate(RSA_KEY_BITS);
    rsa_pub_key = rsa_prv_key.pub();
    base_pke = &pve_base_pke_rsa();
    ek = pve_keyref(rsa_pub_key);
  } else {
    ecc_prv_key.generate(curve);
    ecc_pub_key = ecc_prv_key.pub();
    base_pke = &pve_base_pke_ecies();
    ek = pve_keyref(ecc_pub_key);
  }

  for (auto _ : state) {
    auto rv = pve.encrypt(*base_pke, ek, "test-label", curve, x);
    benchmark::DoNotOptimize(rv);
  }
}

static void BM_PVE_Batch_Verify(benchmark::State& state) {
  int n = state.range(1);
  ec_pve_batch_t pve(n);

  const auto& curve = crypto::curve_p256;
  const mod_t q = curve.order();
  const crypto::ecc_generator_point_t& G = curve.generator();
  std::vector<bn_t> x(n);
  for (int i = 0; i < n; i++) {
    x[i] = bn_t::rand(q);
  }
  std::vector<ecc_point_t> X(n);
  for (int i = 0; i < n; i++) {
    X[i] = x[i] * G;
  }

  const pve_base_pke_i* base_pke = nullptr;
  pve_keyref_t ek;

  crypto::rsa_prv_key_t rsa_prv_key;
  crypto::rsa_pub_key_t rsa_pub_key;
  crypto::ecc_prv_key_t ecc_prv_key;
  crypto::ecc_pub_key_t ecc_pub_key;

  if (state.range(0) == 0) {
    rsa_prv_key.generate(RSA_KEY_BITS);
    rsa_pub_key = rsa_prv_key.pub();
    base_pke = &pve_base_pke_rsa();
    ek = pve_keyref(rsa_pub_key);
  } else {
    ecc_prv_key.generate(curve);
    ecc_pub_key = ecc_prv_key.pub();
    base_pke = &pve_base_pke_ecies();
    ek = pve_keyref(ecc_pub_key);
  }

  pve.encrypt(*base_pke, ek, "test-label", curve, x);
  for (auto _ : state) {
    auto rv = pve.verify(*base_pke, ek, X, "test-label");
    benchmark::DoNotOptimize(rv);
  }
}

static void BM_PVE_Batch_Decrypt(benchmark::State& state) {
  int n = state.range(1);
  ec_pve_batch_t pve(n);

  const auto& curve = crypto::curve_p256;
  const mod_t q = curve.order();
  std::vector<bn_t> x(n);
  for (int i = 0; i < n; i++) {
    x[i] = bn_t::rand(q);
  }

  const pve_base_pke_i* base_pke = nullptr;
  pve_keyref_t ek;
  pve_keyref_t dk;

  crypto::rsa_prv_key_t rsa_prv_key;
  crypto::rsa_pub_key_t rsa_pub_key;
  crypto::ecc_prv_key_t ecc_prv_key;
  crypto::ecc_pub_key_t ecc_pub_key;

  if (state.range(0) == 0) {
    rsa_prv_key.generate(RSA_KEY_BITS);
    rsa_pub_key = rsa_prv_key.pub();
    base_pke = &pve_base_pke_rsa();
    ek = pve_keyref(rsa_pub_key);
    dk = pve_keyref(rsa_prv_key);
  } else {
    ecc_prv_key.generate(curve);
    ecc_pub_key = ecc_prv_key.pub();
    base_pke = &pve_base_pke_ecies();
    ek = pve_keyref(ecc_pub_key);
    dk = pve_keyref(ecc_prv_key);
  }

  pve.encrypt(*base_pke, ek, "test-label", curve, x);

  for (auto _ : state) {
    std::vector<bn_t> decrypted;
    auto rv = pve.decrypt(*base_pke, dk, ek, "test-label", curve, decrypted);
    benchmark::DoNotOptimize(rv);
    benchmark::DoNotOptimize(decrypted);
  }
}

class PVEACFixture : public benchmark::Fixture {
 protected:
  const crypto::ecurve_t& curve = crypto::curve_p256;
  mod_t q;
  crypto::ecc_generator_point_t G;
  crypto::ss::ac_t ac;

  std::map<std::string, crypto::ecc_pub_key_t> pub_keys;
  std::map<std::string, crypto::ecc_prv_key_t> prv_keys;
  mpc::ec_pve_ac_t::pks_t pub_key_ptrs;
  mpc::ec_pve_ac_t::sks_t prv_key_ptrs;
  std::vector<bn_t> xs;
  std::vector<ecc_point_t> Xs;
  buf_t label = buf_t("test-label");

  ec_pve_ac_t pve;

 public:
  void SetUp(const benchmark::State&) override {
    q = curve.order();
    G = curve.generator();
    ac = crypto::ss::ac_t(testutils::getTestRoot());

    auto leaves = ac.list_leaf_names();
    for (auto path : leaves) {
      crypto::ecc_prv_key_t prv_key;
      prv_key.generate(curve);
      crypto::ecc_pub_key_t pub_key = prv_key.pub();
      if (!ac.enough_for_quorum(pub_keys)) {
        prv_keys[path] = prv_key;
      }
      pub_keys[path] = std::move(pub_key);
    }

    // Build pointer maps expected by ec_pve_ac_t
    pub_key_ptrs.clear();
    prv_key_ptrs.clear();
    for (auto& kv : pub_keys) pub_key_ptrs[kv.first] = pve_keyref(kv.second);
    for (auto& kv : prv_keys) prv_key_ptrs[kv.first] = pve_keyref(kv.second);

    int n = 20;
    xs.resize(n);
    Xs.resize(n);
    for (int i = 0; i < n; i++) {
      xs[i] = bn_t::rand(q);
      Xs[i] = xs[i] * G;
    }
  }
};

BENCHMARK(BM_PVE_Encrypt)->Name("PVE/vencrypt/Encrypt")->ArgsProduct({{0, 1}});
BENCHMARK(BM_PVE_Verify)->Name("PVE/vencrypt/Verify")->ArgsProduct({{0, 1}});
BENCHMARK(BM_PVE_Decrypt)->Name("PVE/vencrypt/Decrypt")->ArgsProduct({{0, 1}});
BENCHMARK(BM_PVE_Batch_Encrypt)->Name("PVE/vencrypt-batch/Encrypt")->ArgsProduct({{0, 1}, {4, 16}});
BENCHMARK(BM_PVE_Batch_Verify)->Name("PVE/vencrypt-batch/Verify")->ArgsProduct({{0, 1}, {4, 16}});
BENCHMARK(BM_PVE_Batch_Decrypt)->Name("PVE/vencrypt-batch/Decrypt")->ArgsProduct({{0, 1}, {4, 16}});

BENCHMARK_DEFINE_F(PVEACFixture, BM_AC_Encrypt)(benchmark::State& state) {
  const auto& base_pke = pve_base_pke_ecies();
  for (auto _ : state) {
    auto rv = pve.encrypt(base_pke, ac, pub_key_ptrs, label, curve, xs);
    benchmark::DoNotOptimize(rv);
  }
}
BENCHMARK_DEFINE_F(PVEACFixture, BM_AC_Verify)(benchmark::State& state) {
  const auto& base_pke = pve_base_pke_ecies();
  pve.encrypt(base_pke, ac, pub_key_ptrs, label, curve, xs);
  for (auto _ : state) {
    auto rv = pve.verify(base_pke, ac, pub_key_ptrs, Xs, label);
    benchmark::DoNotOptimize(rv);
  }
}
BENCHMARK_DEFINE_F(PVEACFixture, BM_AC_Decrypt)(benchmark::State& state) {
  const auto& base_pke = pve_base_pke_ecies();
  pve.encrypt(base_pke, ac, pub_key_ptrs, label, curve, xs);
  std::vector<bn_t> decrypted_xs;
  for (auto _ : state) {
    int row_index = 0;
    std::map<std::string, bn_t> shares;
    for (auto &kv : prv_key_ptrs) {
      bn_t share;
      auto rv = pve.party_decrypt_row(base_pke, ac, row_index, kv.first, kv.second, label, share);
      if (rv) benchmark::DoNotOptimize(rv);
      shares[kv.first] = share;
    }
    auto rv =
        pve.aggregate_to_restore_row(base_pke, ac, row_index, label, shares, decrypted_xs, /*skip_verify=*/true);
    if (rv) benchmark::DoNotOptimize(rv);
  }
}
BENCHMARK_REGISTER_F(PVEACFixture, BM_AC_Encrypt)->Name("PVE/vencrypt-batch-many/Encrypt");
BENCHMARK_REGISTER_F(PVEACFixture, BM_AC_Verify)->Name("PVE/vencrypt-batch-many/Verify");
BENCHMARK_REGISTER_F(PVEACFixture, BM_AC_Decrypt)->Name("PVE/vencrypt-batch-many/Decrypt")->Iterations(5);
