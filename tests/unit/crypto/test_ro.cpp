#include <gtest/gtest.h>

#include <cbmpc/crypto/ro.h>

using namespace coinbase::crypto;

namespace {

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

}  // namespace