#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_bn256.h>
#include <cbmpc/internal/crypto/ro.h>

using namespace coinbase::crypto;

namespace {

using coinbase::buf_t;
using coinbase::mem_t;

TEST(RandomOracle, EncodeAndUpdateHappyPath) {
  ro::hmac_state_t s1;
  s1.encode_and_update(0);
  buf_t h1 = s1.final();
}

TEST(RandomOracle, EncodeAndUpdateCollisionResist) {
  {
    ro::hmac_state_t s1;
    s1.encode_and_update(mem_t("AABBCCDD"));
    s1.encode_and_update(mem_t("EEFF"));
    buf_t h1 = s1.final();

    ro::hmac_state_t s2;
    s2.encode_and_update(mem_t("AABB"));
    s2.encode_and_update(mem_t("CCDDEEFF"));
    buf_t h2 = s2.final();

    EXPECT_NE(h1, h2);
  }

  {
    ro::hmac_state_t s1;
    s1.encode_and_update(mem_t());
    s1.encode_and_update(mem_t("AABBCC"));
    buf_t h1 = s1.final();

    ro::hmac_state_t s2;
    s2.encode_and_update(mem_t("AABBCC"));
    buf_t h2 = s2.final();

    ro::hmac_state_t s3;
    s3.encode_and_update(mem_t("AABBCC"));
    s3.encode_and_update(mem_t());
    buf_t h3 = s3.final();

    EXPECT_NE(h1, h2);
    EXPECT_NE(h2, h3);
    EXPECT_NE(h3, h1);
  }
}

TEST(RandomOracle, EncodeAndUpdateConcatenation) {
  ro::hmac_state_t s1;
  s1.encode_and_update(mem_t("AA"), mem_t("BB"), mem_t("CC"));
  buf_t h1 = s1.final();

  ro::hmac_state_t s2;
  s2.encode_and_update(mem_t("AA"));
  s2.encode_and_update(mem_t("BB"));
  s2.encode_and_update(mem_t("CC"));
  buf_t h2 = s2.final();

  cb_assert(h1 == h2);
}

TEST(RandomOracle, HashNumbersMod256StaysWithinCurveOrder) {
  const mod_t& q = curve_secp256k1.order();

  auto values = ro::hash_numbers(mem_t("mod256"), 7).count(8).mod256(q);
  auto repeat = ro::hash_numbers(mem_t("mod256"), 7).count(8).mod256(q);

  ASSERT_EQ(values.size(), 8u);
  ASSERT_EQ(repeat.size(), values.size());
  for (std::size_t i = 0; i < values.size(); ++i) EXPECT_TRUE(values[i] == repeat[i]);
  for (const auto& value : values) EXPECT_TRUE((bn_t(value) < q.value()));
}

TEST(RandomOracle, HashNumbersMod256RejectsModuliWiderThan256Bits) {
  EXPECT_THROW(
      { ro::hash_numbers(mem_t("mod256-wide")).count(1).mod256(LARGEST_PRIME_MOD_2048); },
      coinbase::assertion_failed_t);
}

}  // namespace
