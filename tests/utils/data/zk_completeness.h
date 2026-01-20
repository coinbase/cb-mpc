#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_pki.h>
#include <cbmpc/zk/zk_ec.h>
#include <cbmpc/zk/zk_elgamal_com.h>
#include <cbmpc/zk/zk_paillier.h>
#include <cbmpc/zk/zk_unknown_order.h>

namespace coinbase::test::data {

struct test_nizk_t {
  uint64_t aux = 0;
  buf_t sid = coinbase::crypto::gen_random(16);

  virtual void setup() = 0;
  virtual void prove() = 0;
  virtual error_t verify() = 0;
  virtual uint64_t proof_size() = 0;

  virtual ~test_nizk_t() = default;
};

struct test_2rzk_t {
  uint64_t aux = 0;
  buf_t sid = coinbase::crypto::gen_random(16);

  virtual void setup() = 0;
  virtual void v1() = 0;
  virtual uint64_t v1_size() = 0;
  virtual void p2() = 0;
  virtual uint64_t p2_size() = 0;
  virtual error_t verify() = 0;
};

struct test_3rzk_t {
  uint64_t aux = 0;
  buf_t sid = coinbase::crypto::gen_random(16);

  virtual void setup() = 0;
  virtual void p1() = 0;
  virtual uint64_t p1_size() = 0;
  virtual void v2() = 0;
  virtual uint64_t v2_size() = 0;
  virtual void p3() = 0;
  virtual uint64_t p3_size() = 0;
  virtual error_t verify() = 0;
};

struct test_niuc_dl_t : public test_nizk_t {
  ecurve_t curve;
  coinbase::zk::uc_dl_t zk;
  ecc_point_t G, Q;
  mod_t q;
  bn_t w;

  test_niuc_dl_t(ecurve_t c) : curve(c), G(c.generator()), q(c.order()) {}

  void setup() override {
    w = bn_t::rand(q);
    Q = w * G;
  }

  void prove() override { zk.prove(Q, w, sid, aux); }
  error_t verify() override { return zk.verify(Q, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_niuc_batch_dl_t : public test_nizk_t {
  ecurve_t curve;
  int batch_size;
  coinbase::zk::uc_batch_dl_t zk;
  ecc_point_t G;
  mod_t q;
  std::vector<bn_t> ws;
  std::vector<ecc_point_t> Qs;

  test_niuc_batch_dl_t(ecurve_t c, int b) : curve(c), G(c.generator()), q(c.order()), batch_size(b) {
    ws.resize(batch_size);
    Qs.resize(batch_size);
  }

  void setup() override {
    for (int i = 0; i < batch_size; i++) {
      ws[i] = bn_t::rand(q);
      Qs[i] = ws[i] * G;
    }
  }

  void prove() override { zk.prove(Qs, ws, sid, aux); }
  error_t verify() override { return zk.verify(Qs, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_nidh_t : public test_nizk_t {
  ecurve_t curve;
  coinbase::zk::dh_t zk;
  ecc_point_t G, Q, A, B;
  mod_t q;
  bn_t w;

  test_nidh_t(ecurve_t c) : curve(c), G(c.generator()), q(c.order()) {}

  void setup() override {
    Q = bn_t::rand(q) * G;
    w = bn_t::rand(q);
    A = w * G;
    B = w * Q;
  }

  void prove() override { zk.prove(Q, A, B, w, sid, aux); }
  error_t verify() override { return zk.verify(Q, A, B, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_nizk_uc_elgamal_com : public test_nizk_t {
  coinbase::zk::uc_elgamal_com_t zk;
  ecurve_t curve;
  ecc_point_t G, Q;
  bn_t q, x, r;
  elg_com_t* UV;

  test_nizk_uc_elgamal_com(ecurve_t c) : curve(c), G(c.generator()), q(c.order()) {}

  void setup() override {
    Q = bn_t::rand(q) * G;

    x = bn_t::rand(q);
    r = bn_t::rand(q);

    UV = new elg_com_t(r * G, curve.mul_add(x, Q, r));
  }

  void prove() override { zk.prove(Q, *UV, x, r, sid, aux); }
  error_t verify() override { return zk.verify(Q, *UV, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_nizk_elgamal_com_pub_share_equ : public test_nizk_t {
  coinbase::zk::elgamal_com_pub_share_equ_t zk;
  ecurve_t curve;
  ecc_point_t G, E, A;
  bn_t q, a, r_eA;
  elg_com_t eA;

  test_nizk_elgamal_com_pub_share_equ(ecurve_t c) : curve(c), G(c.generator()), q(c.order()) {}

  void setup() override {
    E = bn_t::rand(q) * G;

    a = bn_t::rand(q);
    A = a * G;
    r_eA = bn_t::rand(q);
    eA = elg_com_t(r_eA * G, curve.mul_add(a, E, r_eA));
  }

  void prove() override { zk.prove(E, A, eA, r_eA, sid, aux); }
  error_t verify() override { return zk.verify(E, A, eA, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_nizk_elgamal_com_mult : public test_nizk_t {
  coinbase::zk::elgamal_com_mult_t zk;
  ecurve_t curve;
  ecc_point_t G, E;
  bn_t q, b, x, r_eB, r_eC;
  elg_com_t eA, eB, eC;

  test_nizk_elgamal_com_mult(ecurve_t c) : curve(c), G(c.generator()), q(c.order()) {}

  void setup() override {
    E = bn_t::rand(q) * G;

    x = bn_t::rand(q);
    b = bn_t::rand(q);
    r_eB = bn_t::rand(q);
    r_eC = bn_t::rand(q);
    eA = elg_com_t::random_commit(E, x);
    eB = elg_com_t(r_eB * G, curve.mul_add(b, E, r_eB));
    eC = (b * eA).rerand(E, r_eC);
  }

  void prove() override { zk.prove(E, eA, eB, eC, r_eB, r_eC, b, sid, aux); }
  error_t verify() override { return zk.verify(E, eA, eB, eC, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_nizk_elgamal_com_mult_private_scalar : public test_nizk_t {
  coinbase::zk::uc_elgamal_com_mult_private_scalar_t zk;
  ecurve_t curve;
  ecc_point_t G, E;
  bn_t q, x, c, r_0;
  elg_com_t eA, eB;

  test_nizk_elgamal_com_mult_private_scalar(ecurve_t c) : curve(c), G(c.generator()), q(c.order()) {}

  void setup() override {
    E = bn_t::rand(q) * G;

    x = bn_t::rand(q);
    c = bn_t::rand(q);
    r_0 = bn_t::rand(q);
    eA = elg_com_t::random_commit(E, x);
    eB = (c * eA).rerand(E, r_0);
  }

  void prove() override { zk.prove(E, eA, eB, r_0, c, sid, aux); }
  error_t verify() override { return zk.verify(E, eA, eB, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_nizk_valid_paillier : public test_nizk_t {
  coinbase::zk::valid_paillier_t zk;
  coinbase::crypto::paillier_t p_p, v_p;
  mod_t N;

  test_nizk_valid_paillier() {}

  void setup() override {
    p_p.generate();
    N = p_p.get_N();
    v_p.create_pub(N);
  }

  void prove() override { zk.prove(p_p, sid, aux); }
  error_t verify() override { return zk.verify(v_p, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_2rzk_valid_paillier : public test_2rzk_t {
  coinbase::zk::valid_paillier_interactive_t zk;
  coinbase::zk::valid_paillier_interactive_t::challenge_msg_t v1_msg;
  coinbase::zk::valid_paillier_interactive_t::prover_msg_t p2_msg;
  coinbase::crypto::paillier_t p_p, v_p;
  mod_t N;
  crypto::mpc_pid_t prover_pid = crypto::pid_from_name("test");

  test_2rzk_valid_paillier() {}

  void setup() override {
    p_p.generate();
    N = p_p.get_N();
    v_p.create_pub(N);
  }

  void v1() override { zk.challenge(v1_msg); }
  uint64_t v1_size() override { return coinbase::converter_t::convert_write(v1_msg, 0); }
  void p2() override { zk.prove(p_p, v1_msg, prover_pid, p2_msg); }
  uint64_t p2_size() override { return coinbase::converter_t::convert_write(p2_msg, 0); }
  error_t verify() override { return zk.verify(v_p, prover_pid, p2_msg); }
};

struct test_nizk_paillier_zero : public test_nizk_t {
  coinbase::zk::paillier_zero_t zk;
  coinbase::crypto::paillier_t p_p, v_p;
  mod_t N;
  bn_t x, r, c;

  test_nizk_paillier_zero() {}

  void setup() override {
    zk.paillier_valid_key = coinbase::zk::zk_flag::verified;
    p_p.generate();
    N = p_p.get_N();
    v_p.create_pub(N);

    x = 0;
    r = bn_t::rand(N);
    c = p_p.encrypt(x, r);
  }

  void prove() override { zk.prove(p_p, c, r, sid, aux); }
  error_t verify() override { return zk.verify(v_p, c, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_nizk_two_paillier_equal : public test_nizk_t {
  coinbase::zk::two_paillier_equal_t zk;
  const int q_size = 256;
  coinbase::crypto::paillier_t p_p1, p_p2, v_p1, v_p2;
  mod_t N1, N2;
  bn_t q, x, r1, r2, c1, c2;

  test_nizk_two_paillier_equal() {}

  void setup() override {
    zk.p0_valid_key = coinbase::zk::zk_flag::verified;
    zk.p1_valid_key = coinbase::zk::zk_flag::verified;
    zk.c0_plaintext_range = coinbase::zk::zk_flag::verified;

    q = bn_t::generate_prime(q_size, false, nullptr, nullptr);
    p_p1.generate();
    N1 = p_p1.get_N();
    v_p1.create_pub(N1);
    p_p2.generate();
    N2 = p_p2.get_N();
    v_p2.create_pub(N2);

    x = bn_t::rand(q);
    r1 = bn_t::rand(N1);
    r2 = bn_t::rand(N2);
    c1 = p_p1.encrypt(x, r1);
    c2 = p_p2.encrypt(x, r2);
  }

  void prove() override { zk.prove(q, p_p1, c1, p_p2, c2, x, r1, r2, sid, aux); }
  error_t verify() override { return zk.verify(q, v_p1, c1, v_p2, c2, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_3rzk_two_paillier_equal : public test_3rzk_t {
  coinbase::zk::two_paillier_equal_interactive_t zk{crypto::pid_from_name("test")};
  coinbase::zk::two_paillier_equal_interactive_t::prover_msg1_t msg1;
  coinbase::zk::two_paillier_equal_interactive_t::verifier_challenge_msg_t msg2;
  coinbase::zk::two_paillier_equal_interactive_t::prover_msg2_t msg3;
  const int q_size = 256;
  coinbase::crypto::paillier_t p_p1, p_p2, v_p1, v_p2;
  mod_t N1, N2;
  bn_t q, x, r1, r2, c1, c2;
  buf_t sid = coinbase::crypto::gen_random(16);

  test_3rzk_two_paillier_equal() {}

  void setup() override {
    zk.p0_valid_key = coinbase::zk::zk_flag::verified;
    zk.p1_valid_key = coinbase::zk::zk_flag::verified;
    zk.c0_plaintext_range = coinbase::zk::zk_flag::verified;

    q = bn_t::generate_prime(q_size, false, nullptr, nullptr);
    p_p1.generate();
    N1 = p_p1.get_N();
    v_p1.create_pub(N1);
    p_p2.generate();
    N2 = p_p2.get_N();
    v_p2.create_pub(N2);

    x = bn_t::rand(q);
    r1 = bn_t::rand(N1);
    r2 = bn_t::rand(N2);
    c1 = p_p1.encrypt(x, r1);
    c2 = p_p2.encrypt(x, r2);
  }

  void p1() override { zk.prover_msg1(q, p_p1, p_p2, msg1); }
  uint64_t p1_size() override { return coinbase::convert(msg1).size(); }
  void v2() override { zk.verifier_challenge_msg(msg2); }
  uint64_t v2_size() override { return coinbase::convert(msg2).size(); }
  void p3() override { ASSERT_EQ(zk.prover_msg2(p_p1, p_p2, x, r1, r2, msg2, msg3), SUCCESS); }
  uint64_t p3_size() override { return coinbase::convert(msg3).size(); }
  error_t verify() override { return zk.verify(q, v_p1, c1, v_p2, c2, msg1, msg3); }
};

struct test_nizk_range_pedersen : public test_nizk_t {
  coinbase::zk::range_pedersen_t zk;
  const int x_len = 256;
  mod_t p, p_tag, q;
  bn_t g, h, x, r, c;

  test_nizk_range_pedersen() {
    const auto& params = coinbase::zk::pedersen_commitment_params_t::get();
    p_tag = params.p_tag;
    p = params.p;
    g = params.g;
    h = params.h;
  }

  void setup() override {
    q = bn_t::generate_prime(x_len, false, nullptr, nullptr);
    x = bn_t::rand(q);
    r = bn_t::rand(p_tag);
    MODULO(p) { c = g.pow(x) * h.pow(r); }
  }

  void prove() override { zk.prove(q, c, x, r, sid, aux); }
  error_t verify() override { return zk.verify(q, c, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_i3rzk_range_pedersen : public test_3rzk_t {
  std::unique_ptr<coinbase::zk::range_pedersen_interactive_t> zk;
  const int x_len = 256;
  mod_t p, p_tag, q;
  bn_t g, h, x, r, c;
  crypto::mpc_pid_t pid = crypto::pid_from_name("test");

  test_i3rzk_range_pedersen() {
    zk = std::make_unique<coinbase::zk::range_pedersen_interactive_t>(pid);
    const auto& params = coinbase::zk::pedersen_commitment_params_t::get();
    p_tag = params.p_tag;
    p = params.p;
    g = params.g;
    h = params.h;
  }

  void setup() override {
    q = bn_t::generate_prime(x_len, false, nullptr, nullptr);
    x = bn_t::rand(q);
    r = bn_t::rand(p_tag);
    MODULO(p) { c = g.pow(x) * h.pow(r); }
  }

  void p1() override { zk->prover_msg1(q); }
  uint64_t p1_size() override { return coinbase::convert(zk->msg1).size(); }
  void v2() override { zk->verifier_challenge(); }
  uint64_t v2_size() override { return coinbase::convert(zk->challenge).size(); }
  void p3() override { zk->prover_msg2(x, r); }
  uint64_t p3_size() override { return coinbase::convert(zk->msg2).size(); }
  error_t verify() override { return zk->verify(c, q); }
};

struct test_nizk_paillier_pedersen_equal : public test_nizk_t {
  coinbase::zk::paillier_pedersen_equal_t zk;
  const int x_len = 256;
  coinbase::crypto::paillier_t p_p, v_p;
  mod_t p, p_tag, q, N;
  bn_t g, h, c, Com, x, R, rho;

  test_nizk_paillier_pedersen_equal() {
    const auto& params = coinbase::zk::pedersen_commitment_params_t::get();
    p_tag = params.p_tag;
    p = params.p;
    g = params.g;
    h = params.h;
    q = bn_t::generate_prime(x_len, false, nullptr, nullptr);
  }

  void setup() override {
    zk.paillier_valid_key = coinbase::zk::zk_flag::verified;

    p_p.generate();
    N = p_p.get_N();
    v_p.create_pub(N);

    x = bn_t::rand(q);
    R = bn_t::rand(N);
    c = p_p.encrypt(x, R);
    rho = bn_t::rand(p_tag);
    MODULO(p) { Com = g.pow(x) * h.pow(rho); }
  }

  void prove() override { zk.prove(p_p, c, q, Com, x, R, rho, sid, aux); }
  error_t verify() override { return zk.verify(v_p, c, q, Com, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_i3rzk_paillier_pedersen_equal : public test_3rzk_t {
  coinbase::zk::paillier_pedersen_equal_interactive_t zk{crypto::pid_from_name("test")};
  const int x_len = 256;
  coinbase::crypto::paillier_t p_p, v_p;
  mod_t p, p_tag, q, N;
  bn_t g, h, c, Com, x, R, rho;

  test_i3rzk_paillier_pedersen_equal() {
    const auto& params = coinbase::zk::pedersen_commitment_params_t::get();
    p_tag = params.p_tag;
    p = params.p;
    g = params.g;
    h = params.h;
    q = bn_t::generate_prime(x_len, false, nullptr, nullptr);
  }

  void setup() override {
    zk.paillier_valid_key = coinbase::zk::zk_flag::verified;

    p_p.generate();
    N = p_p.get_N();
    v_p.create_pub(N);

    x = bn_t::rand(q);
    R = bn_t::rand(N);
    c = p_p.encrypt(x, R);
    rho = bn_t::rand(p_tag);
    MODULO(p) { Com = g.pow(x) * h.pow(rho); }
  }

  void p1() override { zk.prover_msg1(p_p, q); }
  uint64_t p1_size() override { return coinbase::convert(zk.msg1).size(); }
  void v2() override { zk.verifier_challenge(); }
  uint64_t v2_size() override { return coinbase::convert(zk.challenge).size(); }
  void p3() override { zk.prover_msg2(p_p, x, R, rho); }
  uint64_t p3_size() override { return coinbase::convert(zk.msg2).size(); }
  error_t verify() override { return zk.verify(v_p, c, q, Com); }
};

struct test_nizk_paillier_range_exp_slack : public test_nizk_t {
  coinbase::zk::paillier_range_exp_slack_t zk;
  const int q_size = 256;
  coinbase::crypto::paillier_t p_p, v_p;
  mod_t N;
  bn_t q, x, r, c;

  test_nizk_paillier_range_exp_slack() {}

  void setup() override {
    zk.paillier_valid_key = coinbase::zk::zk_flag::verified;

    q = bn_t::generate_prime(q_size, false, nullptr, nullptr);

    p_p.generate();
    N = p_p.get_N();
    v_p.create_pub(N);

    x = bn_t::rand(q);
    r = bn_t::rand(N);
    c = p_p.encrypt(x, r);
  }

  void prove() override { zk.prove(p_p, q, c, x, r, sid, aux); }
  error_t verify() override { return zk.verify(v_p, q, c, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_nizk_pdl : public test_nizk_t {
  ecurve_t curve;
  coinbase::zk::pdl_t zk;
  const int q_size = 256;
  ecc_point_t G, Q1;
  coinbase::crypto::paillier_t p_p, v_p;
  mod_t N;
  bn_t q, x1, r, c;

  test_nizk_pdl(ecurve_t c) : curve(c), G(c.generator()) {}

  void setup() override {
    zk.paillier_valid_key = coinbase::zk::zk_flag::verified;

    // q = bn_t::generate_prime(q_size, false, nullptr, nullptr);
    q = curve.order();

    p_p.generate();
    N = p_p.get_N();
    v_p.create_pub(N);

    x1 = bn_t::rand(q);
    Q1 = x1 * G;
    r = bn_t::rand(N);
    c = p_p.encrypt(x1, r);
  }

  void prove() override { zk.prove(c, p_p, Q1, x1, r, sid, aux); }
  error_t verify() override { return zk.verify(c, v_p, Q1, sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

struct test_unknown_order_dl : public test_nizk_t {
  coinbase::zk::unknown_order_dl_t zk;
  coinbase::crypto::paillier_t pai;
  mod_t N;
  bn_t a, b, w;
  int l;

  test_unknown_order_dl() {}

  void setup() override {
    pai.generate();
    N = pai.get_N();
    a = bn_t::rand(N);
    l = N.get_bits_count();
    w = bn_t::rand_bitlen(l);
    b = N.pow(a, w);
  }

  void prove() override { zk.prove(a, b, N, l, w, sid, aux); }
  error_t verify() override { return zk.verify(a, b, N, N.get_bits_count(), sid, aux); }
  uint64_t proof_size() override { return coinbase::converter_t::convert_write(zk, 0); }
};

}  // namespace coinbase::test::data