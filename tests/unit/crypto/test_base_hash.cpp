#include <gtest/gtest.h>
#include <vector>

#include <cbmpc/internal/crypto/base.h>

namespace {

using namespace coinbase;
using namespace coinbase::crypto;

TEST(BaseHash, MemVecEncodesBoundsAndLen) {
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

TEST(BaseHash, UpdateRejectsNegativeSize) {
  // hash_t::update() should validate that size >= 0
  // Negative sizes would be implicitly converted to huge positive size_t values
  // when passed to EVP_DigestUpdate(), causing out-of-bounds reads

  hash_t hash(hash_e::sha256);
  hash.init();
  byte_t data[4] = {0x01, 0x02, 0x03, 0x04};

  // Should throw assertion_failed_t for negative size
  EXPECT_THROW({ hash.update(data, -1); }, coinbase::assertion_failed_t);
  EXPECT_THROW({ hash.update(data, -100); }, coinbase::assertion_failed_t);

  // Valid size should work
  EXPECT_NO_THROW({ hash.update(data, 4); });
  EXPECT_NO_THROW({ hash.update(data, 0); });
}

}  // namespace
