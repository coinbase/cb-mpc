#include <gtest/gtest.h>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>
#include <cbmpc/protocol/ot.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::mpc;

namespace {

TEST(OT_Base, PVW) {
  const int u = 256;
  base_ot_protocol_pvw_ctx_t ot;
  bits_t b = crypto::gen_random_bits(u);
  std::vector<buf_t> x0, x1, x_out;
  x0.resize(u);
  x1.resize(u);
  for (int j = 0; j < u; ++j) {
    x0[j] = crypto::gen_random(16);
    x1[j] = crypto::gen_random(16);
  }
  ot.sid = crypto::gen_random(16);
  EXPECT_OK(ot.step1_R2S(b));
  EXPECT_OK(ot.step2_S2R(x0, x1));
  EXPECT_OK(ot.output_R(x_out));
  for (int j = 0; j < u; ++j) {
    buf_t x_truth = b[j] ? x1[j] : x0[j];
    EXPECT_EQ(x_truth, x_out[j]);
  }
}

TEST(OT_Extension, Main) {
  const int u = 256;
  const int m = 1 << 16;
  auto curve = crypto::curve_secp256k1;
  auto q = curve.order();
  ot_ext_protocol_ctx_t ot;
  bits_t s = crypto::gen_random_bits(u);
  std::vector<buf_t> sigma0, sigma1, sigma, x_out;
  sigma0.resize(u);
  sigma1.resize(u);
  sigma.resize(u);
  for (int j = 0; j < u; ++j) {
    sigma0[j] = crypto::gen_random(16);
    sigma1[j] = crypto::gen_random(16);
  }
  for (int j = 0; j < u; ++j) {
    sigma[j] = s[j] ? sigma1[j] : sigma0[j];
  }
  std::vector<buf_t> x0, x1;
  x0.resize(m);
  x1.resize(m);
  for (int j = 0; j < m; ++j) {
    x0[j] = crypto::gen_random(16);
    x1[j] = crypto::gen_random(16);
  }
  // Start of OT Extension
  std::vector<buf_t> x_bin;
  buf_t sid = crypto::gen_random(16);
  bits_t r = crypto::gen_random_bits(m);
  int l = x0[0].size() * 8;
  EXPECT_OK(ot.step1_R2S(sid, sigma0, sigma1, r, l));
  EXPECT_OK(ot.step2_S2R(sid, s, sigma, x0, x1));
  EXPECT_OK(ot.output_R(m, x_bin));

  for (int j = 0; j < m; ++j) {
    buf_t x = r[j] ? x1[j] : x0[j];
    EXPECT_EQ(x, x_bin[j]);
  }
}

TEST(OT_Extension, SenderOneInputRandom) {
  const int u = 256;
  const int m = 1 << 16;
  auto curve = crypto::curve_secp256k1;
  auto q = curve.order();
  ot_ext_protocol_ctx_t ot;
  bits_t s = crypto::gen_random_bits(u);
  std::vector<buf_t> sigma0, sigma1, sigma, x_out;
  sigma0.resize(u);
  sigma1.resize(u);
  sigma.resize(u);
  for (int j = 0; j < u; ++j) {
    sigma0[j] = crypto::gen_random(16);
    sigma1[j] = crypto::gen_random(16);
  }
  for (int j = 0; j < u; ++j) {
    sigma[j] = s[j] ? sigma1[j] : sigma0[j];
  }
  std::vector<bn_t> delta(m);
  for (int j = 0; j < m; ++j) delta[j] = bn_t::rand(q);
  // Start of OT Extension
  std::vector<bn_t> x0, x1;
  std::vector<buf_t> x_bin;
  buf_t sid = crypto::gen_random(16);
  bits_t r = crypto::gen_random_bits(m);
  int l = q.get_bits_count();

  EXPECT_OK(ot.step1_R2S(sid, sigma0, sigma1, r, l));
  EXPECT_OK(ot.step2_S2R_sender_one_input_random(sid, s, sigma, delta, q, x0, x1));
  EXPECT_OK(ot.output_R(m, x_bin));
  for (int j = 0; j < m; ++j) {
    bn_t x = r[j] ? x1[j] : x0[j];
    EXPECT_EQ(x, bn_t::from_bin(x_bin[j]));
    EXPECT_EQ(x1[j], (x0[j] + delta[j]) % q);
  }
}

TEST(OT_Extension, SenderRandom) {
  const int u = 256;
  const int m = 1 << 16;
  auto curve = crypto::curve_secp256k1;
  auto q = curve.order();
  ot_ext_protocol_ctx_t ot;
  bits_t s = crypto::gen_random_bits(u);
  std::vector<buf_t> sigma0, sigma1, sigma, x, x_out;
  sigma0.resize(u);
  sigma1.resize(u);
  sigma.resize(u);
  for (int j = 0; j < u; ++j) {
    sigma0[j] = crypto::gen_random(16);
    sigma1[j] = crypto::gen_random(16);
    x.push_back(crypto::gen_random(16));
  }
  for (int j = 0; j < u; ++j) {
    sigma[j] = s[j] ? sigma1[j] : sigma0[j];
  }
  std::vector<bn_t> delta(m);
  for (int j = 0; j < m; ++j) delta[j] = bn_t::rand(q);
  // Start of OT Extension
  std::vector<buf_t> x0_bin, x1_bin;
  buf_t sid = crypto::gen_random(16);
  bits_t r = crypto::gen_random_bits(m);
  int l = q.get_bits_count();

  EXPECT_OK(ot.sender_random_step1_R2S(sid, sigma0, sigma1, r, l, x));
  EXPECT_OK(ot.sender_random_output_S(sid, s, sigma, m, l, x0_bin, x1_bin));
  for (int j = 0; j < m; ++j) {
    EXPECT_EQ(r[j] ? x1_bin[j] : x0_bin[j], x[j]);
  }
}

TEST(OT, FullOT2P) {
  const int u = 256;
  const int m = 1 << 16;
  auto curve = crypto::curve_secp256k1;
  auto q = curve.order();
  int l = q.get_bits_count();
  bits_t r = crypto::gen_random_bits(m);

  std::vector<bn_t> x0, x1;
  x0.resize(m);
  x1.resize(m);
  for (int j = 0; j < m; ++j) {
    x0[j] = bn_t::rand(q);
    x1[j] = bn_t::rand(q);
  }

  // Start of OT
  std::vector<buf_t> x_bin;
  ot_protocol_pvw_ctx_t ot(curve);
  ot.base.sid = crypto::gen_random(16);
  EXPECT_OK(ot.step1_S2R());
  EXPECT_OK(ot.step2_R2S(r, l));
  EXPECT_OK(ot.step3_S2R(x0, x1, l));
  EXPECT_OK(ot.output_R(m, x_bin));
  for (int j = 0; j < m; ++j) {
    bn_t x = r[j] ? x1[j] : x0[j];
    EXPECT_EQ(x, bn_t::from_bin(x_bin[j]));
  }
}

TEST(OT, SenderOneInputRandomOT2P) {
  const int u = 256;
  const int m = 1 << 16;
  auto curve = crypto::curve_secp256k1;
  auto q = curve.order();
  int l = q.get_bits_count();
  bits_t r = crypto::gen_random_bits(m);

  std::vector<bn_t> x0, x1;
  std::vector<bn_t> delta(m);
  for (int j = 0; j < m; ++j) delta[j] = bn_t::rand(q);

  // Start of OT
  std::vector<buf_t> x_bin;
  ot_protocol_pvw_ctx_t ot(curve);
  ot.base.sid = crypto::gen_random(16);
  EXPECT_OK(ot.step1_S2R());
  EXPECT_OK(ot.step2_R2S(r, l));
  EXPECT_OK(ot.step3_S2R(delta, q, x0, x1));
  EXPECT_OK(ot.output_R(m, x_bin));
  for (int j = 0; j < m; ++j) {
    bn_t x = r[j] ? x1[j] : x0[j];
    EXPECT_EQ(x, bn_t::from_bin(x_bin[j]));
    EXPECT_EQ(x1[j], (x0[j] + delta[j]) % q);
  }
}

}  // namespace