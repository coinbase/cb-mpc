#include <gtest/gtest.h>
#include <vector>

#include <cbmpc/crypto/base.h>

namespace {

using namespace coinbase;
using namespace coinbase::crypto;

TEST(BaseHash, MemTVectorEncodesBoundariesAndLength) {
  const std::vector<mem_t> msgs_a = {mem_t("a"), mem_t("bc")};  // concat: "abc"
  const std::vector<mem_t> msgs_b = {mem_t("ab"), mem_t("c")};  // concat: "abc"
  const std::vector<mem_t> msgs_c = {mem_t("abc")};             // concat: "abc"

  const auto ha = sha256_t::hash(msgs_a);
  const auto hb = sha256_t::hash(msgs_b);
  const auto hc = sha256_t::hash(msgs_c);

  EXPECT_NE(ha, hb);
  EXPECT_NE(ha, hc);
  EXPECT_NE(hb, hc);
}

}  // namespace
