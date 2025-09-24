#include <gtest/gtest.h>

#include "cbmpc/protocol/util.h"

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
}

TEST(ProtocolUtil, TestSUMVectorRefInt) {
  // We'll construct a vector of references
  int a = 2, b = 4, c = 6, d = 1;
  std::vector<std::reference_wrapper<int>> refs{a, b, c, d};
  auto result = SUM(refs);
  // Should mirror TestSUMVectorInt: 13
  EXPECT_EQ(result, 13);
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
