#include <benchmark/benchmark.h>

#include <cbmpc/crypto/base_pki.h>
#include <cbmpc/crypto/commitment.h>
#include <cbmpc/zk/zk_pedersen.h>

#include "util.h"

using namespace coinbase;
using namespace coinbase::crypto;

static void Commitment(benchmark::State& state) {
  int u = state.range(0);
  buf_t sid = gen_random_bitlen(SEC_P_COM);
  mpc_pid_t pid = pid_from_name("test");
  commitment_t com(sid, pid);
  std::vector<bn_t> a(u);
  for (int i = 0; i < u; i++) {
    a[i] = bn_t::rand_bitlen(256, false);
  }
  for (auto _ : state) {
    com.gen(a);
  }
}
BENCHMARK(Commitment)->Name("Crypto/Commitment/ComBn")->DenseRange(2, 20, 2);

