#include <array>
#include <gtest/gtest.h>
#include <vector>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_bn256.h>
#include <cbmpc/internal/crypto/ro.h>

#include "utils/test_macros.h"

using namespace coinbase::crypto;

namespace {

using coinbase::array_view_t;
using coinbase::buf256_t;
using coinbase::buf_t;
using coinbase::mem_t;

template <typename T>
buf_t encoded_digest(const T& value) {
  ro::hmac_state_t state;
  state.encode_and_update(value);
  return state.final();
}

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

TEST(RandomOracle, EncodeAndUpdateContainerOverloadsAreFramed) {
  const std::array<bn_t, 2> array_values = {bn_t(1), bn_t(2)};
  const std::array<bn_t, 2> array_values_repeat = {bn_t(1), bn_t(2)};
  const std::array<bn_t, 2> array_values_reordered = {bn_t(2), bn_t(1)};
  const std::array<bn_t, 3> array_values_extended = {bn_t(1), bn_t(2), bn_t(0)};

  EXPECT_EQ(encoded_digest(array_values), encoded_digest(array_values_repeat));
  EXPECT_NE(encoded_digest(array_values), encoded_digest(array_values_reordered));
  EXPECT_NE(encoded_digest(array_values), encoded_digest(array_values_extended));

  const bn_t c_array[] = {bn_t(1), bn_t(2)};
  ro::hmac_state_t c_array_state;
  c_array_state.encode_and_update(c_array);

  ro::hmac_state_t scalars_state;
  scalars_state.encode_and_update(bn_t(1), bn_t(2));
  EXPECT_NE(c_array_state.final(), scalars_state.final());

  const std::vector<bn_t> vector_values = {bn_t(1), bn_t(2)};
  const std::vector<bn_t> vector_values_reordered = {bn_t(2), bn_t(1)};
  EXPECT_NE(encoded_digest(vector_values), encoded_digest(vector_values_reordered));

  array_view_t<bn_t> view(vector_values.data(), int(vector_values.size()));
  EXPECT_EQ(encoded_digest(vector_values), encoded_digest(view));
}

TEST(RandomOracle, ExplicitKeyAndRawUpdateAreDeterministic) {
  ro::hmac_state_t keyed1(mem_t("key"));
  keyed1.update(mem_t("payload"));
  const buf_t digest1 = keyed1.final();

  ro::hmac_state_t keyed2(mem_t("key"));
  keyed2.update(mem_t("payload"));
  EXPECT_EQ(digest1, keyed2.final());

  ro::hmac_state_t different_key(mem_t("other-key"));
  different_key.update(mem_t("payload"));
  EXPECT_NE(digest1, different_key.final());
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

TEST(RandomOracle, HashStringDeterministicAndSized) {
  const coinbase::buf128_t digest = ro::hash_string(mem_t("domain"), 42, mem_t("payload")).bitlen128();
  const coinbase::buf128_t repeat = ro::hash_string(mem_t("domain"), 42, mem_t("payload")).bitlen128();
  EXPECT_EQ(digest, repeat);
  EXPECT_EQ(mem_t(digest).size, 16);
}

TEST(RandomOracle, HashStringBitlenCoversShortAndExpandedOutputs) {
  const buf256_t digest = ro::hash_string(mem_t("domain"), mem_t("payload")).bitlen256();
  const buf256_t repeat = ro::hash_string(mem_t("domain"), mem_t("payload")).bitlen256();
  EXPECT_EQ(digest, repeat);

  const buf_t one_byte = ro::hash_string(mem_t("domain"), 1).bitlen(7);
  EXPECT_EQ(one_byte.size(), 1);

  const buf_t digest_256 = ro::hash_string(mem_t("domain"), 256).bitlen(256);
  EXPECT_EQ(digest_256.size(), 32);

  const buf_t expanded = ro::hash_string(mem_t("domain"), 257).bitlen(257);
  const buf_t expanded_repeat = ro::hash_string(mem_t("domain"), 257).bitlen(257);
  EXPECT_EQ(expanded.size(), 33);
  EXPECT_EQ(expanded, expanded_repeat);
}

TEST(RandomOracle, HashNumberModMatchesFieldOrder) {
  const mod_t& q = curve_secp256k1.order();
  const bn_t value = ro::hash_number(mem_t("challenge"), 7).mod(q);
  const bn_t repeat = ro::hash_number(mem_t("challenge"), 7).mod(q);
  EXPECT_EQ(value, repeat);
  EXPECT_TRUE(value < q.value());
}

TEST(RandomOracle, HashNumberModIntIsDeterministicAndBounded) {
  const int value = ro::hash_number(mem_t("choice"), 3).mod(17);
  const int repeat = ro::hash_number(mem_t("choice"), 3).mod(17);
  EXPECT_EQ(value, repeat);
  EXPECT_GE(value, 0);
  EXPECT_LT(value, 17);
}

TEST(RandomOracle, HashNumbersModReturnsRequestedCountInRange) {
  const mod_t& q = curve_secp256k1.order();
  auto values = ro::hash_numbers(mem_t("numbers"), 11).count(5).mod(q);
  auto repeat = ro::hash_numbers(mem_t("numbers"), 11).count(5).mod(q);

  ASSERT_EQ(values.size(), 5u);
  ASSERT_EQ(repeat.size(), values.size());
  for (std::size_t i = 0; i < values.size(); ++i) {
    EXPECT_EQ(values[i], repeat[i]);
    EXPECT_TRUE(values[i] < q.value());
  }
}

TEST(RandomOracle, HashNumbersRequiresPositiveCountBeforeSampling) {
  const mod_t& q = curve_secp256k1.order();
  EXPECT_CB_ASSERT(ro::hash_numbers(mem_t("numbers")).count(0), "l must be > 0");
  EXPECT_CB_ASSERT(ro::hash_numbers(mem_t("numbers")).count(-1), "l must be > 0");
  EXPECT_CB_ASSERT(ro::hash_numbers(mem_t("numbers")).mod(q), "call count");
  EXPECT_CB_ASSERT(ro::hash_numbers(mem_t("numbers")).mod256(q), "call count");
}

TEST(RandomOracle, HashCurveIsOnCurve) {
  const ecc_point_t point = ro::hash_curve(mem_t("curve-ro"), 99).curve(curve_p256);
  EXPECT_TRUE(point.is_on_curve());
  EXPECT_FALSE(point.is_infinity());
  EXPECT_EQ(point.get_curve(), curve_p256);
}

TEST(RandomOracle, DrbgSampleHelpersAreDeterministic) {
  const buf_t seed = buf_t("0123456789abcdef0123456789abcdef");
  const buf_t sample1 = ro::drbg_sample_string(seed, 128);
  const buf_t sample2 = ro::drbg_sample_string(seed, 128);
  EXPECT_EQ(sample1, sample2);
  EXPECT_EQ(sample1.size(), coinbase::bits_to_bytes(128));

  const bn_t number = ro::drbg_sample_number(seed, curve_secp256k1.order());
  EXPECT_TRUE(number < curve_secp256k1.order().value());

  const ecc_point_t curve_point = ro::drbg_sample_curve(seed, curve_secp256k1);
  EXPECT_TRUE(curve_point.is_on_curve());
}

TEST(RandomOracle, DrbgSampleNumberRejectsNonPositiveModulus) {
  buf_t seed = buf_t("0123456789abcdef");
  EXPECT_CB_ASSERT(ro::drbg_sample_number(seed, 0), "m > 0");
  EXPECT_CB_ASSERT(ro::drbg_sample_number(seed, -7), "m > 0");
}

TEST(RandomOracle, DrbgSampleStringRejectsNonPositiveBitLength) {
  buf_t seed = buf_t("0123456789abcdef");
  EXPECT_CB_ASSERT(ro::drbg_sample_string(seed, 0), "bits > 0");
  EXPECT_CB_ASSERT(ro::drbg_sample_string(seed, -1), "bits > 0");
}

}  // namespace
