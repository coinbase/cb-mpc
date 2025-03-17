#include <benchmark/benchmark.h>

#include <cbmpc/crypto/secret_sharing.h>
#include <cbmpc/protocol/pve.h>
#include <cbmpc/protocol/pve_ac.h>

#include "data/test_node.h"
#include "util.h"

using namespace coinbase;
using namespace coinbase::mpc;

static void BM_PVE_Encrypt(benchmark::State& state) {
  ec_pve_t<crypto::hybrid_cipher_t> pve;

  crypto::pub_key_t pub_key;
  if (state.range(0) == 0) {
    crypto::rsa_prv_key_t rsa_prv_key;
    rsa_prv_key.generate(2048);
    pub_key = crypto::pub_key_t::from(rsa_prv_key.pub());
  } else {
    crypto::ecc_prv_key_t ecc_prv_key;
    ecc_prv_key.generate(crypto::curve_p256);
    pub_key = crypto::pub_key_t::from(ecc_prv_key.pub());
  }
  const mod_t q = crypto::curve_p256.order();
  const crypto::ecc_generator_point_t& G = crypto::curve_p256.generator();
  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;

  for (auto _ : state) {
    pve.encrypt(pub_key, "test-label", crypto::curve_p256, x);
  }
}

static void BM_PVE_Verify(benchmark::State& state) {
  ec_pve_t<crypto::hybrid_cipher_t> pve;

  crypto::pub_key_t pub_key;
  if (state.range(0) == 0) {
    crypto::rsa_prv_key_t rsa_prv_key;
    rsa_prv_key.generate(2048);
    pub_key = crypto::pub_key_t::from(rsa_prv_key.pub());
  } else {
    crypto::ecc_prv_key_t ecc_prv_key;
    ecc_prv_key.generate(crypto::curve_p256);
    pub_key = crypto::pub_key_t::from(ecc_prv_key.pub());
  }
  const mod_t q = crypto::curve_p256.order();
  const crypto::ecc_generator_point_t& G = crypto::curve_p256.generator();
  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;
  pve.encrypt(pub_key, "test-label", crypto::curve_p256, x);

  for (auto _ : state) {
    pve.verify(pub_key, X, "test-label");
  }
}

static void BM_PVE_Decrypt(benchmark::State& state) {
  ec_pve_t<crypto::hybrid_cipher_t> pve;

  crypto::pub_key_t pub_key;
  crypto::prv_key_t prv_key;
  if (state.range(0) == 0) {
    crypto::rsa_prv_key_t rsa_prv_key;
    rsa_prv_key.generate(2048);
    pub_key = crypto::pub_key_t::from(rsa_prv_key.pub());
    prv_key = crypto::prv_key_t::from(rsa_prv_key);
  } else {
    crypto::ecc_prv_key_t ecc_prv_key;
    ecc_prv_key.generate(crypto::curve_p256);
    pub_key = crypto::pub_key_t::from(ecc_prv_key.pub());
    prv_key = crypto::prv_key_t::from(ecc_prv_key);
  }
  const mod_t q = crypto::curve_p256.order();
  const crypto::ecc_generator_point_t& G = crypto::curve_p256.generator();
  bn_t x = bn_t::rand(q);
  ecc_point_t X = x * G;
  pve.encrypt(pub_key, "test-label", crypto::curve_p256, x);

  for (auto _ : state) {
    pve.decrypt(prv_key, "test-label", crypto::curve_p256, x);
  }
}

static void BM_PVE_Batch_Encrypt(benchmark::State& state) {
  int n = state.range(1);
  ec_pve_batch_t<crypto::hybrid_cipher_t> pve(n);

  crypto::pub_key_t pub_key;
  if (state.range(0) == 0) {
    crypto::rsa_prv_key_t rsa_prv_key;
    rsa_prv_key.generate(2048);
    pub_key = crypto::pub_key_t::from(rsa_prv_key.pub());
  } else {
    crypto::ecc_prv_key_t ecc_prv_key;
    ecc_prv_key.generate(crypto::curve_p256);
    pub_key = crypto::pub_key_t::from(ecc_prv_key.pub());
  }
  const mod_t q = crypto::curve_p256.order();
  const crypto::ecc_generator_point_t& G = crypto::curve_p256.generator();
  std::vector<bn_t> x(n);
  for (int i = 0; i < n; i++) {
    x[i] = bn_t::rand(q);
  }
  std::vector<ecc_point_t> X(n);
  for (int i = 0; i < n; i++) {
    X[i] = x[i] * G;
  }

  for (auto _ : state) {
    pve.encrypt(pub_key, "test-label", crypto::curve_p256, x);
  }
}

static void BM_PVE_Batch_Verify(benchmark::State& state) {
  int n = state.range(1);
  ec_pve_batch_t<crypto::hybrid_cipher_t> pve(n);

  crypto::pub_key_t pub_key;
  if (state.range(0) == 0) {
    crypto::rsa_prv_key_t rsa_prv_key;
    rsa_prv_key.generate(2048);
    pub_key = crypto::pub_key_t::from(rsa_prv_key.pub());
  } else {
    crypto::ecc_prv_key_t ecc_prv_key;
    ecc_prv_key.generate(crypto::curve_p256);
    pub_key = crypto::pub_key_t::from(ecc_prv_key.pub());
  }
  const mod_t q = crypto::curve_p256.order();
  const crypto::ecc_generator_point_t& G = crypto::curve_p256.generator();
  std::vector<bn_t> x(n);
  for (int i = 0; i < n; i++) {
    x[i] = bn_t::rand(q);
  }
  std::vector<ecc_point_t> X(n);
  for (int i = 0; i < n; i++) {
    X[i] = x[i] * G;
  }
  pve.encrypt(pub_key, "test-label", crypto::curve_p256, x);
  for (auto _ : state) {
    pve.verify(pub_key, X, "test-label");
  }
}

static void BM_PVE_Batch_Decrypt(benchmark::State& state) {
  int n = state.range(1);
  ec_pve_batch_t<crypto::hybrid_cipher_t> pve(n);

  crypto::pub_key_t pub_key;
  crypto::prv_key_t prv_key;
  if (state.range(0) == 0) {
    crypto::rsa_prv_key_t rsa_prv_key;
    rsa_prv_key.generate(2048);
    pub_key = crypto::pub_key_t::from(rsa_prv_key.pub());
    prv_key = crypto::prv_key_t::from(rsa_prv_key);
  } else {
    crypto::ecc_prv_key_t ecc_prv_key;
    ecc_prv_key.generate(crypto::curve_p256);
    pub_key = crypto::pub_key_t::from(ecc_prv_key.pub());
    prv_key = crypto::prv_key_t::from(ecc_prv_key);
  }
  const mod_t q = crypto::curve_p256.order();
  const crypto::ecc_generator_point_t& G = crypto::curve_p256.generator();
  std::vector<bn_t> x(n);
  for (int i = 0; i < n; i++) {
    x[i] = bn_t::rand(q);
  }
  pve.encrypt(pub_key, "test-label", crypto::curve_p256, x);

  for (auto _ : state) {
    pve.decrypt(prv_key, "test-label", crypto::curve_p256, x);
  }
}

crypto::ecc_prv_key_t get_ecc_prv_key(int participant_index) {
  crypto::ecc_prv_key_t prv_key_ecc;
  prv_key_ecc.generate(crypto::curve_p256);
  return prv_key_ecc;
}
crypto::rsa_prv_key_t get_rsa_prv_key(int participant_index) {
  crypto::rsa_prv_key_t prv_key_rsa;
  prv_key_rsa.generate(2048);
  return prv_key_rsa;
}
crypto::prv_key_t get_prv_key(int participant_index) {
  if (participant_index & 1)
    return crypto::prv_key_t::from(get_ecc_prv_key(participant_index));
  else
    return crypto::prv_key_t::from(get_rsa_prv_key(participant_index));
}

class PVEACFixture : public benchmark::Fixture {
 protected:
  const crypto::ecurve_t& curve = crypto::curve_p256;
  mod_t q;
  crypto::ecc_generator_point_t G;
  crypto::ss::ac_t ac;

  std::map<std::string, crypto::pub_key_t> pub_keys;
  std::map<std::string, crypto::prv_key_t> prv_keys;
  std::vector<bn_t> xs;
  std::vector<ecc_point_t> Xs;
  std::string label = "test-label";

  ec_pve_ac_t<crypto::hybrid_cipher_t> pve;

 public:
  void SetUp(const benchmark::State&) override {
    q = curve.order();
    G = curve.generator();
    ac = crypto::ss::ac_t(testutils::getTestRoot());

    auto leaves = ac.list_leaf_names();
    int participant_index = 0;
    for (auto path : leaves) {
      auto prv_key = get_prv_key(participant_index);
      if (!ac.enough_for_quorum(pub_keys)) {
        prv_keys[path] = prv_key;
      }
      pub_keys[path] = prv_key.pub();
      participant_index++;
    }

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
  for (auto _ : state) {
    pve.encrypt(ac, pub_keys, label, curve, xs);
  }
}
BENCHMARK_DEFINE_F(PVEACFixture, BM_AC_Verify)(benchmark::State& state) {
  pve.encrypt(ac, pub_keys, label, curve, xs);
  for (auto _ : state) {
    pve.verify(ac, pub_keys, Xs, label);
  }
}
BENCHMARK_DEFINE_F(PVEACFixture, BM_AC_Decrypt)(benchmark::State& state) {
  pve.encrypt(ac, pub_keys, label, curve, xs);
  std::vector<bn_t> decrypted_xs;
  for (auto _ : state) {
    pve.decrypt(ac, prv_keys, pub_keys, label, decrypted_xs);
  }
}
BENCHMARK_REGISTER_F(PVEACFixture, BM_AC_Encrypt)->Name("PVE/vencrypt-batch-many/Encrypt");
BENCHMARK_REGISTER_F(PVEACFixture, BM_AC_Verify)->Name("PVE/vencrypt-batch-many/Verify");
BENCHMARK_REGISTER_F(PVEACFixture, BM_AC_Decrypt)->Name("PVE/vencrypt-batch-many/Decrypt")->Iterations(5);
