#include <gtest/gtest.h>

#include <cbmpc/internal/protocol/util.h>

using namespace coinbase::crypto;

TEST(ProtocolUtil, TestSUMLambdaWithInitialZero) {
  // SUM with an explicit zero, using a lambda that increments sum by index
  auto result = SUM<int>(0, 5, [](int& sum, int idx) { sum += idx; });
  // Expected sum of indices [0..4] is 10
  EXPECT_EQ(result, 10);
}

TEST(ProtocolUtil, TestSUMImplicitZero) {
  // SUM with implicit zero-initialized T
  auto result = SUM<int>(5, [](int& sum, int idx) {
    sum += (idx + 1);  // summing [1..5]
  });
  // Expected: 1+2+3+4+5 = 15
  EXPECT_EQ(result, 15);
}

TEST(ProtocolUtil, TestSUMVectorInt) {
  std::vector<int> values{2, 4, 6, 1};
  auto result = SUM(values);
  // Should be 13
  EXPECT_EQ(result, 13);

  std::vector<int> empty;
  EXPECT_EQ(SUM(empty), 0);
}

TEST(ProtocolUtil, TestSUMVectorRefInt) {
  // We'll construct a vector of references
  int a = 2, b = 4, c = 6, d = 1;
  std::vector<std::reference_wrapper<int>> refs{a, b, c, d};
  auto result = SUM(refs);
  // Should mirror TestSUMVectorInt: 13
  EXPECT_EQ(result, 13);

  std::vector<std::reference_wrapper<int>> empty;
  EXPECT_EQ(SUM(empty), 0);
}

TEST(ProtocolUtil, TestSUMMap) {
  std::map<coinbase::crypto::pname_t, int> values{{"p1", 2}, {"p2", 4}, {"p3", 6}};
  EXPECT_EQ(SUM(values), 12);

  std::map<coinbase::crypto::pname_t, int> empty;
  EXPECT_EQ(SUM(empty), 0);
}

TEST(ProtocolUtil, TestSUMBN) {
  // Test with bn_t under a modulus
  mod_t m(bn_t(7));  // modulus = 7 for demonstration
  std::vector<bn_t> bnVals{bn_t(2), bn_t(3), bn_t(6)};
  // 2 + 3 + 6 = 11 mod 7 = 4
  bn_t result = SUM(bnVals, m);
  EXPECT_EQ((int)result, 4);
}

TEST(ProtocolUtil, TestSUMBNRef) {
  // Similarly for reference_wrapper, under modulus
  mod_t m(bn_t(13));  // modulus = 10
  bn_t a(7), b(9), c(6);
  std::vector<std::reference_wrapper<bn_t>> bnRefs{a, b, c};
  // 7 + 9 + 6 = 22 mod 10 = 2
  bn_t result = SUM(bnRefs, m);
  EXPECT_EQ((int)result, 9);
}

TEST(ProtocolUtil, TestMapArgsToTuple) {
  // Simple test of map_args_to_tuple
  auto resultTuple = map_args_to_tuple([](int x) { return x * 2; }, 1, 2, 3);
  // resultTuple should be (2, 4, 6)
  EXPECT_EQ(std::get<0>(resultTuple), 2);
  EXPECT_EQ(std::get<1>(resultTuple), 4);
  EXPECT_EQ(std::get<2>(resultTuple), 6);
}

TEST(ProtocolUtil, CurveMsgToBnTruncatesToCurveSize) {
  const ecurve_t curve = curve_secp256k1;
  const coinbase::buf_t short_msg = coinbase::crypto::gen_random(curve.size() - 1);
  EXPECT_EQ(curve_msg_to_bn(short_msg, curve), bn_t::from_bin(short_msg));

  const coinbase::buf_t long_msg = coinbase::crypto::gen_random(curve.size() + 5);
  const bn_t expected = bn_t::from_bin(coinbase::mem_t(long_msg.data(), curve.size()));
  EXPECT_EQ(curve_msg_to_bn(long_msg, curve), expected);
}
