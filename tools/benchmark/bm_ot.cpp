#include <benchmark/benchmark.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/ot.h>

#define base_ot_m_lb 1 << 6
#define base_ot_m_ub 1 << 11

using namespace coinbase;
using namespace coinbase::mpc;

static void BM_BaseOT_Step1(benchmark::State& state) {
  int u = state.range(0);
  base_ot_protocol_pvw_ctx_t ot;
  ot.sid = crypto::gen_random_bits(SEC_P_COM);

  // Prepare Receiver's private input
  coinbase::bits_t s = crypto::gen_random_bits(u);

  for (auto _ : state) {
    if (ot.step1_R2S(s)) throw std::runtime_error("base ot step1_R2S failed");
  }
  state.counters["size"] = coinbase::converter_t::convert_write(ot.msg1(), 0);
}

static void BM_BaseOT_Step2(benchmark::State& state) {
  int u = state.range(0);
  base_ot_protocol_pvw_ctx_t ot;
  ot.sid = crypto::gen_random_bits(SEC_P_COM);

  // Step 1
  coinbase::bits_t s = crypto::gen_random_bits(u);
  if (ot.step1_R2S(s)) throw std::runtime_error("base ot step1_R2S failed");

  // Prepare Sender's private input
  std::vector<buf_t> sigma0(u);
  std::vector<buf_t> sigma1(u);
  for (int i = 0; i < u; i++) {
    sigma0[i] = crypto::gen_random(16);
    sigma1[i] = crypto::gen_random(16);
  }

  for (auto _ : state) {
    if (ot.step2_S2R(sigma0, sigma1)) throw std::runtime_error("base ot step2_S2R failed");
  }
  state.counters["size"] = coinbase::converter_t::convert_write(ot.msg2(), 0);
}

static void BM_BaseOT_OutputR(benchmark::State& state) {
  int u = state.range(0);
  base_ot_protocol_pvw_ctx_t ot;
  ot.sid = crypto::gen_random_bits(SEC_P_COM);

  // Step 1
  coinbase::bits_t s = crypto::gen_random_bits(u);
  if (ot.step1_R2S(s)) throw std::runtime_error("base ot step1_R2S failed");

  // Step 2
  std::vector<buf_t> sigma0(u);
  std::vector<buf_t> sigma1(u);
  for (int i = 0; i < u; i++) {
    sigma0[i] = crypto::gen_random(16);
    sigma1[i] = crypto::gen_random(16);
  }
  if (ot.step2_S2R(sigma0, sigma1)) throw std::runtime_error("base ot step2_S2R failed");

  for (auto _ : state) {
    std::vector<buf_t> sigma;
    if (ot.output_R(sigma)) throw std::runtime_error("base ot output_R failed");
  }
}

//------------------------------------
// Fixture for Full OT measurements
//------------------------------------
class FullOT2PBenchmark : public benchmark::Fixture {
 public:
  // Protocol context
  std::unique_ptr<ot_protocol_pvw_ctx_t> ot;

  // Pre-generated random bits
  bits_t r;

  // Pre-generated inputs x0, x1
  std::vector<bn_t> x0, x1;

  // We keep curve and parameter l (bits count) as members
  crypto::ecurve_t curve;
  mod_t q;
  int l;

  // buffer size
  int m;

  void SetUp(const ::benchmark::State& st) override {
    // Fix u = 256 (unused in these steps, but matching the prior test logic)
    m = static_cast<int>(st.range(0));
    ot = std::make_unique<ot_protocol_pvw_ctx_t>();
    ot->base.sid = crypto::gen_random(16);
    curve = ot->base.curve;
    q = curve.order();
    l = q.get_bits_count();

    // Generate random bits for the receiver's choice
    r = coinbase::crypto::gen_random_bits(m);

    // Prepare x0, x1 for the protocol
    x0.resize(m);
    x1.resize(m);
    for (int j = 0; j < m; ++j) {
      x0[j] = bn_t::rand(q);
      x1[j] = bn_t::rand(q);
    }
  }
};

//------------------------------------
// Step 1 Benchmark
//------------------------------------
BENCHMARK_DEFINE_F(FullOT2PBenchmark, BM_FullOT2P_Step1)(benchmark::State& state) {
  for (auto _ : state) {
    if (ot->step1_S2R()) {
      throw std::runtime_error("Full OT step1_S2R failed");
    }
  }
}

//------------------------------------
// Step 2 Benchmark
//------------------------------------
BENCHMARK_DEFINE_F(FullOT2PBenchmark, BM_FullOT2P_Step2)(benchmark::State& state) {
  // Must do step1 first
  if (ot->step1_S2R()) {
    throw std::runtime_error("Full OT step1_S2R failed");
  }
  for (auto _ : state) {
    // Benchmark step2
    if (ot->step2_R2S(r, l)) {
      throw std::runtime_error("Full OT step2_R2S failed");
    }
  }
  state.counters["size"] = coinbase::converter_t::convert_write(ot->msg1(), 0);
}

//------------------------------------
// Step 3 Benchmark
//------------------------------------
BENCHMARK_DEFINE_F(FullOT2PBenchmark, BM_FullOT2P_Step3)(benchmark::State& state) {
  // Must do step1, step2 first
  if (ot->step1_S2R()) {
    throw std::runtime_error("Full OT step1_S2R failed");
  }
  if (ot->step2_R2S(r, l)) {
    throw std::runtime_error("Full OT step2_R2S failed");
  }
  for (auto _ : state) {
    // Benchmark step3
    if (ot->step3_S2R(x0, x1, l)) {
      throw std::runtime_error("Full OT step3_S2R failed");
    }
  }
  state.counters["size"] = coinbase::converter_t::convert_write(ot->msg2(), 0);
}

//------------------------------------
// Output Benchmark
//------------------------------------
BENCHMARK_DEFINE_F(FullOT2PBenchmark, BM_FullOT2P_Output)(benchmark::State& state) {
  // Must do step1, step2, step3 first
  if (ot->step1_S2R()) {
    throw std::runtime_error("Full OT step1_S2R failed");
  }
  if (ot->step2_R2S(r, l)) {
    throw std::runtime_error("Full OT step2_R2S failed");
  }
  if (ot->step3_S2R(x0, x1, l)) {
    throw std::runtime_error("Full OT step3_S2R failed");
  }
  for (auto _ : state) {
    // Benchmark output step
    std::vector<buf_t> x_bin;
    if (ot->output_R(m, x_bin)) {
      throw std::runtime_error("Full OT output_R failed");
    }
  }
  state.counters["size"] = coinbase::converter_t::convert_write(ot->msg3(), 0);
}

BENCHMARK(BM_BaseOT_Step1)->Name("MPC/OT/BaseOT/Step1_R2S")->RangeMultiplier(2)->Range(base_ot_m_lb, base_ot_m_ub);
BENCHMARK(BM_BaseOT_Step2)->Name("MPC/OT/BaseOT/Step2_S2R")->RangeMultiplier(2)->Range(base_ot_m_lb, base_ot_m_ub);
BENCHMARK(BM_BaseOT_OutputR)->Name("MPC/OT/BaseOT/OutputR")->RangeMultiplier(2)->Range(base_ot_m_lb, base_ot_m_ub);
BENCHMARK_REGISTER_F(FullOT2PBenchmark, BM_FullOT2P_Step1)
    ->Name("Full-OT-2P/1/1")
    ->Args({1 << 11})
    ->Args({1 << 12})
    ->Args({1 << 16});
BENCHMARK_REGISTER_F(FullOT2PBenchmark, BM_FullOT2P_Step2)
    ->Name("Full-OT-2P/2/2")
    ->Args({1 << 11})
    ->Args({1 << 12})
    ->Args({1 << 16});
BENCHMARK_REGISTER_F(FullOT2PBenchmark, BM_FullOT2P_Step3)
    ->Name("Full-OT-2P/3/1")
    ->Args({1 << 11})
    ->Args({1 << 12})
    ->Args({1 << 16});
BENCHMARK_REGISTER_F(FullOT2PBenchmark, BM_FullOT2P_Output)
    ->Name("Full-OT-2P/4/2")
    ->Args({1 << 11})
    ->Args({1 << 12})
    ->Args({1 << 16});
