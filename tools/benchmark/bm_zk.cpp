#include <benchmark/benchmark.h>

#include <cbmpc/zk/zk_elgamal_com.h>
#include <cbmpc/zk/zk_paillier.h>

#include "data/zk_data_generator.h"
#include "util.h"

using namespace coinbase;
using namespace coinbase::crypto;

static void ZK_DL_Proof(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::uc_dl_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::uc_dl_t zk;

  for (auto _ : state) zk.prove(input.Q, input.w, input.sid, input.aux);
}
static void ZK_DL_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::uc_dl_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::uc_dl_t zk;
  zk.prove(input.Q, input.w, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.Q, input.sid, input.aux);
}

static void ZK_DH_Proof(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::dh_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::dh_t zk;

  for (auto _ : state) zk.prove(input.Q, input.A, input.B, input.w, input.sid, input.aux);
}
static void ZK_DH_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::dh_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::dh_t zk;
  zk.prove(input.Q, input.A, input.B, input.w, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.Q, input.A, input.B, input.sid, input.aux);
}

static void ZK_Batch_DL_Proof(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));
  int size = state.range(1);

  test::input_generator_t<test::uc_batch_dl_input_t> input_generator(curve);
  auto input = input_generator.generate(size);
  zk::uc_batch_dl_t zk;

  for (auto _ : state) {
    zk.prove(input.Qs, input.ws, input.sid, input.aux);
  }
}
BENCHMARK(ZK_Batch_DL_Proof)->Name("ZK/Batch-DL/Prover")->ArgsProduct({{3, 4}, {1, 4, 16, 64}});
static void ZK_Batch_DL_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));
  int size = state.range(1);

  test::input_generator_t<test::uc_batch_dl_input_t> input_generator(curve);
  auto input = input_generator.generate(size);
  zk::uc_batch_dl_t zk;
  zk.prove(input.Qs, input.ws, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) {
    zk.verify(input.Qs, input.sid, input.aux);
  }
}
BENCHMARK(ZK_Batch_DL_Verify)->Name("ZK/Batch-DL/Verify")->ArgsProduct({{3, 4}, {1, 4, 16, 64}});

static void ZK_ValidPaillier_Proof(benchmark::State& state) {
  test::input_generator_t<test::valid_paillier_input_t> input_generator;
  auto input = input_generator.generate();
  zk::valid_paillier_t zk;

  for (auto _ : state) zk.prove(input.p_p, input.sid, input.aux);
}
static void ZK_ValidPaillier_Verify(benchmark::State& state) {
  test::input_generator_t<test::valid_paillier_input_t> input_generator;
  auto input = input_generator.generate();
  zk::valid_paillier_t zk;
  zk.prove(input.p_p, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.v_p, input.sid, input.aux);
}

static void ZK_ValidPaillierInteractive_V1(benchmark::State& state) {
  test::input_generator_t<test::valid_paillier_input_t> input_generator;
  auto input = input_generator.generate();
  zk::valid_paillier_interactive_t zk;

  for (auto _ : state) {
    zk::valid_paillier_interactive_t::challenge_msg_t v1_msg;
    zk.challenge(v1_msg);
  }
}
BENCHMARK(ZK_ValidPaillierInteractive_V1)->Name("ZK/ValidPaillier-Int/V1");
static void ZK_ValidPaillierInteractive_P2(benchmark::State& state) {
  test::input_generator_t<test::valid_paillier_input_t> input_generator;
  auto input = input_generator.generate();
  zk::valid_paillier_interactive_t zk;
  zk::valid_paillier_interactive_t::challenge_msg_t v1_msg;
  zk.challenge(v1_msg);

  state.counters["size"] = converter_t::convert_write(v1_msg, 0);
  crypto::mpc_pid_t prover_pid = crypto::pid_from_name("test");

  for (auto _ : state) {
    zk::valid_paillier_interactive_t::prover_msg_t p2_msg;
    zk.prove(input.p_p, v1_msg, prover_pid, p2_msg);
  }
}
BENCHMARK(ZK_ValidPaillierInteractive_P2)->Name("ZK/ValidPaillier-Int/P2");
static void ZK_ValidPaillierInteractive_Verify(benchmark::State& state) {
  test::input_generator_t<test::valid_paillier_input_t> input_generator;
  auto input = input_generator.generate();
  zk::valid_paillier_interactive_t zk;
  zk::valid_paillier_interactive_t::challenge_msg_t v1_msg;
  zk::valid_paillier_interactive_t::prover_msg_t p2_msg;
  crypto::mpc_pid_t prover_pid = crypto::pid_from_name("test");
  zk.challenge(v1_msg);
  zk.prove(input.p_p, v1_msg, prover_pid, p2_msg);

  state.counters["size"] = converter_t::convert_write(p2_msg, 0);

  for (auto _ : state) {
    zk.verify(input.v_p, prover_pid, p2_msg);
  }
}
BENCHMARK(ZK_ValidPaillierInteractive_Verify)->Name("ZK/ValidPaillier-Int/Verify");

static void ZK_PaillierZero_Proof(benchmark::State& state) {
  test::input_generator_t<test::paillier_zero_input_t> input_generator;
  auto input = input_generator.generate();
  zk::paillier_zero_t zk;

  for (auto _ : state) zk.prove(input.p_p, input.c, input.r, input.sid, input.aux);
}
static void ZK_PaillierZero_Verify(benchmark::State& state) {
  test::input_generator_t<test::paillier_zero_input_t> input_generator;
  auto input = input_generator.generate();
  zk::paillier_zero_t zk;
  zk.prove(input.p_p, input.c, input.r, input.sid, input.aux);
  zk.paillier_valid_key = zk::zk_flag::verified;
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.v_p, input.c, input.sid, input.aux);
}

static void ZK_PaillierZeroInteractive_P1(benchmark::State& state) {
  test::input_generator_t<test::paillier_zero_input_t> input_generator;
  auto input = input_generator.generate();
  zk::paillier_zero_interactive_t zk(input.pid);

  for (auto _ : state) zk.prover_msg1(input.p_p);
}
BENCHMARK(ZK_PaillierZeroInteractive_P1)->Name("ZK/PaillierZero-Int/P1");
static void ZK_PaillierZeroInteractive_V2(benchmark::State& state) {
  test::input_generator_t<test::paillier_zero_input_t> input_generator;
  auto input = input_generator.generate();
  zk::paillier_zero_interactive_t zk(input.pid);
  zk.prover_msg1(input.p_p);

  zk.paillier_valid_key = zk::zk_flag::verified;
  state.counters["size"] = converter_t::convert_write(zk.msg1, 0);

  for (auto _ : state) zk.verifier_challenge();
}
BENCHMARK(ZK_PaillierZeroInteractive_V2)->Name("ZK/PaillierZero-Int/V2");
static void ZK_PaillierZeroInteractive_P3(benchmark::State& state) {
  test::input_generator_t<test::paillier_zero_input_t> input_generator;
  auto input = input_generator.generate();
  zk::paillier_zero_interactive_t zk(input.pid);
  zk.paillier_valid_key = zk::zk_flag::verified;
  zk.prover_msg1(input.p_p);
  zk.verifier_challenge();

  state.counters["size"] = converter_t::convert_write(zk.challenge, 0);

  for (auto _ : state) zk.prover_msg2(input.p_p, input.r);
}
BENCHMARK(ZK_PaillierZeroInteractive_P3)->Name("ZK/PaillierZero-Int/P3");
static void ZK_PaillierZeroInteractive_Verify(benchmark::State& state) {
  test::input_generator_t<test::paillier_zero_input_t> input_generator;
  auto input = input_generator.generate();
  zk::paillier_zero_interactive_t zk(input.pid);
  zk.paillier_valid_key = zk::zk_flag::verified;
  zk.prover_msg1(input.p_p);
  zk.verifier_challenge();
  zk.prover_msg2(input.p_p, input.r);

  state.counters["size"] = converter_t::convert_write(zk.msg2, 0);

  for (auto _ : state) zk.verify(input.v_p, input.c);
}
BENCHMARK(ZK_PaillierZeroInteractive_Verify)->Name("ZK/PaillierZero-Int/Verify");

static void ZK_TwoPaillierEqual_Proof(benchmark::State& state) {
  test::input_generator_t<test::two_paillier_equal_input_t> input_generator;
  auto input = input_generator.generate();
  zk::two_paillier_equal_t zk;

  for (auto _ : state)
    zk.prove(input.q, input.p_p_1, input.c1, input.p_p_2, input.c2, input.x, input.r1, input.r2, input.sid, input.aux);
}
static void ZK_TwoPaillierEqual_Verify(benchmark::State& state) {
  test::input_generator_t<test::two_paillier_equal_input_t> input_generator;
  auto input = input_generator.generate();
  zk::two_paillier_equal_t zk;
  zk.p0_valid_key = coinbase::zk::zk_flag::verified;
  zk.p1_valid_key = coinbase::zk::zk_flag::verified;
  zk.c0_plaintext_range = coinbase::zk::zk_flag::verified;
  zk.prove(input.q, input.p_p_1, input.c1, input.p_p_2, input.c2, input.x, input.r1, input.r2, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) {
    zk.verify(input.q, input.v_p_1, input.c1, input.v_p_2, input.c2, input.sid, input.aux);
  }
}

static void ZK_TwoPaillierEqualInteractive_P1(benchmark::State& state) {
  test::input_generator_t<test::two_paillier_equal_input_t> input_generator;
  auto input = input_generator.generate();
  zk::two_paillier_equal_interactive_t zk(input.pid);

  for (auto _ : state) {
    zk::two_paillier_equal_interactive_t::prover_msg1_t msg1;
    zk.prover_msg1(input.q, input.p_p_1, input.p_p_2, msg1);
  }
}
BENCHMARK(ZK_TwoPaillierEqualInteractive_P1)->Name("ZK/TwoPaillierEqual-Int/P1");
static void ZK_TwoPaillierEqualInteractive_V2(benchmark::State& state) {
  test::input_generator_t<test::two_paillier_equal_input_t> input_generator;
  auto input = input_generator.generate();
  zk::two_paillier_equal_interactive_t zk(input.pid);
  zk::two_paillier_equal_interactive_t::prover_msg1_t msg1;
  zk.prover_msg1(input.q, input.p_p_1, input.p_p_2, msg1);

  for (auto _ : state) {
    zk::two_paillier_equal_interactive_t::verifier_challenge_msg_t msg2;
    zk.verifier_challenge_msg(msg2);
  }
}
BENCHMARK(ZK_TwoPaillierEqualInteractive_V2)->Name("ZK/TwoPaillierEqual-Int/V2");
static void ZK_TwoPaillierEqualInteractive_P3(benchmark::State& state) {
  test::input_generator_t<test::two_paillier_equal_input_t> input_generator;
  auto input = input_generator.generate();
  zk::two_paillier_equal_interactive_t zk(input.pid);
  zk::two_paillier_equal_interactive_t::prover_msg1_t msg1;
  zk::two_paillier_equal_interactive_t::verifier_challenge_msg_t msg2;
  zk.prover_msg1(input.q, input.p_p_1, input.p_p_2, msg1);
  zk.verifier_challenge_msg(msg2);

  for (auto _ : state) {
    zk::two_paillier_equal_interactive_t::prover_msg2_t msg3;
    zk.prover_msg2(input.p_p_1, input.p_p_2, input.x, input.r1, input.r2, msg2, msg3);
  }
}
BENCHMARK(ZK_TwoPaillierEqualInteractive_P3)->Name("ZK/TwoPaillierEqual-Int/P3");
static void ZK_TwoPaillierEqualInteractive_Verify(benchmark::State& state) {
  test::input_generator_t<test::two_paillier_equal_input_t> input_generator;
  auto input = input_generator.generate();
  zk::two_paillier_equal_interactive_t zk(input.pid);
  zk::two_paillier_equal_interactive_t::prover_msg1_t msg1;
  zk::two_paillier_equal_interactive_t::verifier_challenge_msg_t msg2;
  zk::two_paillier_equal_interactive_t::prover_msg2_t msg3;
  zk.prover_msg1(input.q, input.p_p_1, input.p_p_2, msg1);
  zk.verifier_challenge_msg(msg2);
  zk.prover_msg2(input.p_p_1, input.p_p_2, input.x, input.r1, input.r2, msg2, msg3);
  zk.p0_valid_key = coinbase::zk::zk_flag::verified;
  zk.p1_valid_key = coinbase::zk::zk_flag::verified;
  zk.c0_plaintext_range = coinbase::zk::zk_flag::verified;

  for (auto _ : state) zk.verify(input.q, input.v_p_1, input.c1, input.v_p_2, input.c2, msg1, msg3);
}
BENCHMARK(ZK_TwoPaillierEqualInteractive_Verify)->Name("ZK/TwoPaillierEqual-Int/Verify");

static void ZK_ELGAMAL_EXP_Proof(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::elgamal_com_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::uc_elgamal_com_t zk;

  for (auto _ : state) zk.prove(input.Q, input.UV, input.x, input.r, input.sid, input.aux);
}
static void ZK_ELGAMAL_EXP_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::elgamal_com_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::uc_elgamal_com_t zk;
  zk.prove(input.Q, input.UV, input.x, input.r, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.Q, input.UV, input.sid, input.aux);
}

static void ZK_ELGAMAL_PUB_SHARE_EQ_Proof(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::elgamal_com_pub_share_equal_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::elgamal_com_pub_share_equ_t zk;

  for (auto _ : state) zk.prove(input.E, input.A, input.eA, input.r_eA, input.sid, input.aux);
}
static void ZK_ELGAMAL_PUB_SHARE_EQ_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::elgamal_com_pub_share_equal_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::elgamal_com_pub_share_equ_t zk;
  zk.prove(input.E, input.A, input.eA, input.r_eA, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.E, input.A, input.eA, input.sid, input.aux);
}

static void ZK_ELGAMAL_EXP_MULT_Proof(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::elgamal_com_mult_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::elgamal_com_mult_t zk;

  for (auto _ : state)
    zk.prove(input.E, input.eA, input.eB, input.eC, input.r_eB, input.r_eC, input.b, input.sid, input.aux);
}
static void ZK_ELGAMAL_EXP_MULT_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::elgamal_com_mult_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::elgamal_com_mult_t zk;
  zk.prove(input.E, input.eA, input.eB, input.eC, input.r_eB, input.r_eC, input.b, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.E, input.eA, input.eB, input.eC, input.sid, input.aux);
}

// static void ZK_ELGAMAL_EXP_MULT_PRIV_SCALAR_Proof(benchmark::State& state) {
//   ecurve_t curve = get_curve(state.range(0));

//   test::input_generator_t<test::elgamal_com_mult_private_scalar_input_t> input_generator(curve);
//   auto input = input_generator.generate();
//   zk::elgamal_com_mult_private_scalar_t zk;

//   for (auto _ : state) zk.prove(input.E, input.eA, input.eB, input.r, input.c, input.sid, input.aux);
// }
// static void ZK_ELGAMAL_EXP_MULT_PRIV_SCALAR_Verify(benchmark::State& state) {
//   ecurve_t curve = get_curve(state.range(0));

//   test::input_generator_t<test::elgamal_com_mult_private_scalar_input_t> input_generator(curve);
//   auto input = input_generator.generate();
//   zk::elgamal_com_mult_private_scalar_t zk;
//   zk.prove(input.E, input.eA, input.eB, input.r, input.c, input.sid, input.aux);
//   state.counters["size"] = converter_t::convert_write(zk, 0);

//   for (auto _ : state) zk.verify(input.E, input.eA, input.eB, input.sid, input.aux);
// }

static void ZK_UC_ELGAMAL_EXP_MULT_PRIV_SCALAR_Proof(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::elgamal_com_mult_private_scalar_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::uc_elgamal_com_mult_private_scalar_t zk;

  for (auto _ : state) zk.prove(input.E, input.eA, input.eB, input.r, input.c, input.sid, input.aux);
}
static void ZK_UC_ELGAMAL_EXP_MULT_PRIV_SCALAR_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::elgamal_com_mult_private_scalar_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::uc_elgamal_com_mult_private_scalar_t zk;
  zk.prove(input.E, input.eA, input.eB, input.r, input.c, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.E, input.eA, input.eB, input.sid, input.aux);
}

static void ZK_RANGE_PEDERSEN_Proof(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::range_pedersen_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::range_pedersen_t zk;

  for (auto _ : state) zk.prove(input.q, input.c, input.x, input.r, input.sid, input.aux);
}
static void ZK_RANGE_PEDERSEN_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::range_pedersen_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::range_pedersen_t zk;
  zk.prove(input.q, input.c, input.x, input.r, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.q, input.c, input.sid, input.aux);
}

static void ZK_RANGE_PEDERSEN_INTERACTIVE_P1(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::range_pedersen_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::range_pedersen_interactive_t zk(crypto::pid_from_name("test"));
  buf_t sid = coinbase::crypto::gen_random(16);

  for (auto _ : state) zk.prover_msg1(input.q);
}
BENCHMARK(ZK_RANGE_PEDERSEN_INTERACTIVE_P1)->Name("ZK/RangePedersenInt/P1")->ArgsProduct({{3, 4}});
static void ZK_RANGE_PEDERSEN_INTERACTIVE_V2(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::range_pedersen_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::range_pedersen_interactive_t zk(crypto::pid_from_name("test"));
  buf_t sid = coinbase::crypto::gen_random(16);
  zk.prover_msg1(input.q);
  state.counters["size"] = converter_t::convert_write(zk.msg1, 0);

  for (auto _ : state) zk.verifier_challenge();
}
BENCHMARK(ZK_RANGE_PEDERSEN_INTERACTIVE_V2)->Name("ZK/RangePedersenInt/V2")->ArgsProduct({{3, 4}});
static void ZK_RANGE_PEDERSEN_INTERACTIVE_P3(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::range_pedersen_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::range_pedersen_interactive_t zk(crypto::pid_from_name("test"));
  buf_t sid = coinbase::crypto::gen_random(16);
  zk.prover_msg1(input.q);
  zk.verifier_challenge();
  state.counters["size"] = converter_t::convert_write(zk.challenge, 0);

  for (auto _ : state) zk.prover_msg2(input.x, input.r);
}
BENCHMARK(ZK_RANGE_PEDERSEN_INTERACTIVE_P3)->Name("ZK/RangePedersenInt/P3")->ArgsProduct({{3, 4}});
static void ZK_RANGE_PEDERSEN_INTERACTIVE_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::range_pedersen_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::range_pedersen_interactive_t zk(crypto::pid_from_name("test"));
  buf_t sid = coinbase::crypto::gen_random(16);
  zk.prover_msg1(input.q);
  zk.verifier_challenge();
  zk.prover_msg2(input.x, input.r);
  state.counters["size"] = converter_t::convert_write(zk.msg2, 0);

  for (auto _ : state) zk.prover_msg2(input.x, input.r);
}
BENCHMARK(ZK_RANGE_PEDERSEN_INTERACTIVE_Verify)->Name("ZK/RangePedersenInt/Verify")->ArgsProduct({{3, 4}});

static void ZK_PAILLIER_PEDERSEN_EQ_Proof(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::paillier_pedersen_equal_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::paillier_pedersen_equal_t zk;

  for (auto _ : state)
    zk.prove(input.p_p, input.c, input.q, input.Com, input.x, input.r, input.rho, input.sid, input.aux);
}
static void ZK_PAILLIER_PEDERSEN_EQ_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::paillier_pedersen_equal_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::paillier_pedersen_equal_t zk;
  zk.prove(input.p_p, input.c, input.q, input.Com, input.x, input.r, input.rho, input.sid, input.aux);
  zk.paillier_valid_key = coinbase::zk::zk_flag::verified;
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.v_p, input.c, input.q, input.Com, input.sid, input.aux);
}

static void ZK_PAILLIER_PEDERSEN_EQ_INTERACTIVE_P1(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::paillier_pedersen_equal_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::paillier_pedersen_equal_interactive_t zk(input.pid);

  for (auto _ : state) zk.prover_msg1(input.p_p, input.q);
}
BENCHMARK(ZK_PAILLIER_PEDERSEN_EQ_INTERACTIVE_P1)->Name("ZK/PaillierPedersenEq-Int/P1")->ArgsProduct({{3, 4}});
static void ZK_PAILLIER_PEDERSEN_EQ_INTERACTIVE_V2(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::paillier_pedersen_equal_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::paillier_pedersen_equal_interactive_t zk(input.pid);
  zk.prover_msg1(input.p_p, input.q);
  state.counters["size"] = converter_t::convert_write(zk.msg1, 0);

  for (auto _ : state) zk.verifier_challenge();
}
BENCHMARK(ZK_PAILLIER_PEDERSEN_EQ_INTERACTIVE_V2)->Name("ZK/PaillierPedersenEq-Int/V2")->ArgsProduct({{3, 4}});
static void ZK_PAILLIER_PEDERSEN_EQ_INTERACTIVE_P3(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::paillier_pedersen_equal_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::paillier_pedersen_equal_interactive_t zk(input.pid);
  zk.prover_msg1(input.p_p, input.q);
  zk.verifier_challenge();
  state.counters["size"] = converter_t::convert_write(zk.challenge, 0);

  for (auto _ : state) zk.prover_msg2(input.p_p, input.x, input.r, input.rho);
}
BENCHMARK(ZK_PAILLIER_PEDERSEN_EQ_INTERACTIVE_P3)->Name("ZK/PaillierPedersenEq-Int/P3")->ArgsProduct({{3, 4}});
static void ZK_PAILLIER_PEDERSEN_EQ_INTERACTIVE_Veirfier(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::paillier_pedersen_equal_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::paillier_pedersen_equal_interactive_t zk(input.pid);
  zk.prover_msg1(input.p_p, input.q);
  zk.verifier_challenge();
  zk.prover_msg2(input.p_p, input.x, input.r, input.rho);
  zk.paillier_valid_key = coinbase::zk::zk_flag::verified;
  state.counters["size"] = converter_t::convert_write(zk.msg2, 0);

  for (auto _ : state) zk.verify(input.v_p, input.c, input.q, input.Com);
}
BENCHMARK(ZK_PAILLIER_PEDERSEN_EQ_INTERACTIVE_Veirfier)
    ->Name("ZK/PaillierPedersenEq-Int/Verifier")
    ->ArgsProduct({{3, 4}});

static void ZK_PAILLIER_RANGE_EXP_SLACK_Proof(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::paillier_range_exp_slack_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::paillier_range_exp_slack_t zk;

  for (auto _ : state) zk.prove(input.p_p, input.q, input.c, input.x, input.r, input.sid, input.aux);
}
static void ZK_PAILLIER_RANGE_EXP_SLACK_Verify(benchmark::State& state) {
  ecurve_t curve = get_curve(state.range(0));

  test::input_generator_t<test::paillier_range_exp_slack_input_t> input_generator(curve);
  auto input = input_generator.generate();
  zk::paillier_range_exp_slack_t zk;
  zk.prove(input.p_p, input.q, input.c, input.x, input.r, input.sid, input.aux);
  zk.paillier_valid_key = coinbase::zk::zk_flag::verified;
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.v_p, input.q, input.c, input.sid, input.aux);
}

static void ZK_PDL_Proof(benchmark::State& state) {
  test::input_generator_t<test::nizk_pdl_input_t> nizk_pdl_generator(curve_secp256k1);
  auto input = nizk_pdl_generator.generate();
  zk::pdl_t zk;
  zk.paillier_valid_key = zk::zk_flag::verified;

  for (auto _ : state) {
    zk.prove(input.c, input.p_p, input.Q1, input.x1, input.r, input.sid, input.aux);
  }
}
static void ZK_PDL_Verify(benchmark::State& state) {
  test::input_generator_t<test::nizk_pdl_input_t> nizk_pdl_generator(curve_secp256k1);
  auto input = nizk_pdl_generator.generate();
  zk::pdl_t zk;
  zk.paillier_valid_key = zk::zk_flag::verified;
  zk.prove(input.c, input.p_p, input.Q1, input.x1, input.r, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) {
    zk.verify(input.c, input.v_p, input.Q1, input.sid, input.aux);
  }
}

static void ZK_UNKNOWN_ORDER_DL_Proof(benchmark::State& state) {
  test::input_generator_t<test::unknown_order_dl_input_t> input_generator;
  auto input = input_generator.generate();
  zk::unknown_order_dl_t zk;

  for (auto _ : state) zk.prove(input.a, input.b, input.N, input.N.get_bits_count(), input.w, input.sid, input.aux);
}
static void ZK_UNKNOWN_ORDER_DL_Verify(benchmark::State& state) {
  test::input_generator_t<test::unknown_order_dl_input_t> input_generator;
  auto input = input_generator.generate();
  zk::unknown_order_dl_t zk;
  zk.prove(input.a, input.b, input.N, input.N.get_bits_count(), input.w, input.sid, input.aux);
  state.counters["size"] = converter_t::convert_write(zk, 0);

  for (auto _ : state) zk.verify(input.a, input.b, input.N, input.N.get_bits_count(), input.sid, input.aux);
}

// 3: secp256k1; 4: ed25519
BENCHMARK(ZK_DL_Proof)->Name("ZK/DL/Prover")->ArgsProduct({{3, 4}});
BENCHMARK(ZK_DL_Verify)->Name("ZK/DL/Verify")->ArgsProduct({{3, 4}});
BENCHMARK(ZK_DH_Proof)->Name("ZK/DH/Prove")->ArgsProduct({{3}});
BENCHMARK(ZK_DH_Verify)->Name("ZK/DH/Verify")->ArgsProduct({{3}});
BENCHMARK(ZK_ELGAMAL_EXP_Proof)->Name("ZK/ElGamalCom/Prover")->ArgsProduct({{3}});
BENCHMARK(ZK_ELGAMAL_EXP_Verify)->Name("ZK/ElGamalCom/Verify")->ArgsProduct({{3}});
BENCHMARK(ZK_ELGAMAL_PUB_SHARE_EQ_Proof)->Name("ZK/ElGamalPubShareEqual/Prover")->ArgsProduct({{3}});
BENCHMARK(ZK_ELGAMAL_PUB_SHARE_EQ_Verify)->Name("ZK/ElGamalPubShareEqual/Verify")->ArgsProduct({{3}});
BENCHMARK(ZK_ELGAMAL_EXP_MULT_Proof)->Name("ZK/ElGamalComMult/Prover")->ArgsProduct({{3}});
BENCHMARK(ZK_ELGAMAL_EXP_MULT_Verify)->Name("ZK/ElGamalComMult/Verify")->ArgsProduct({{3}});
BENCHMARK(ZK_UC_ELGAMAL_EXP_MULT_PRIV_SCALAR_Proof)->Name("ZK/UCElGamalComMultPrivScalar/Prover")->ArgsProduct({{3}});
BENCHMARK(ZK_UC_ELGAMAL_EXP_MULT_PRIV_SCALAR_Verify)->Name("ZK/UCElGamalComMultPrivScalar/Verify")->ArgsProduct({{3}});
BENCHMARK(ZK_ValidPaillier_Proof)->Name("ZK/ValidPaillier/Prover");
BENCHMARK(ZK_ValidPaillier_Verify)->Name("ZK/ValidPaillier/Verify");
BENCHMARK(ZK_PaillierZero_Proof)->Name("ZK/PaillierZero/Prover");
BENCHMARK(ZK_PaillierZero_Verify)->Name("ZK/PaillierZero/Verify");
BENCHMARK(ZK_TwoPaillierEqual_Proof)->Name("ZK/TwoPaillierEqual/Prover");
BENCHMARK(ZK_TwoPaillierEqual_Verify)->Name("ZK/TwoPaillierEqual/Verify");
BENCHMARK(ZK_RANGE_PEDERSEN_Proof)->Name("ZK/RangePedersen/Prover")->ArgsProduct({{3}});
BENCHMARK(ZK_RANGE_PEDERSEN_Verify)->Name("ZK/RangePedersen/Verify")->ArgsProduct({{3}});
BENCHMARK(ZK_PAILLIER_PEDERSEN_EQ_Proof)->Name("ZK/PaillierPedersenEq/Prover")->ArgsProduct({{3}});
BENCHMARK(ZK_PAILLIER_PEDERSEN_EQ_Verify)->Name("ZK/PaillierPedersenEq/Verify")->ArgsProduct({{3}});
BENCHMARK(ZK_PAILLIER_RANGE_EXP_SLACK_Proof)->Name("ZK/PaillierRangeExpSlack/Prover")->ArgsProduct({{3}});
BENCHMARK(ZK_PAILLIER_RANGE_EXP_SLACK_Verify)->Name("ZK/PaillierRangeExpSlack/Verify")->ArgsProduct({{3}});
BENCHMARK(ZK_PDL_Proof)->Name("ZK/PDL/Prover");
BENCHMARK(ZK_PDL_Verify)->Name("ZK/PDL/Verify");
BENCHMARK(ZK_UNKNOWN_ORDER_DL_Proof)->Name("ZK/UnknownOrderDL/Prove");
BENCHMARK(ZK_UNKNOWN_ORDER_DL_Verify)->Name("ZK/UnknownOrderDL/Verify");
