#include <gtest/gtest.h>
#include <sstream>
#include <utility>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_bn256.h>
#include <cbmpc/internal/crypto/base_ec_core.h>
#include <cbmpc/internal/crypto/base_ecc_secp256k1.h>
#include <cbmpc/internal/crypto/base_pki.h>
#include <cbmpc/internal/crypto/ec25519_core.h>

#include "utils/test_macros.h"

namespace {
using namespace coinbase::crypto;
using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;

error_t ecdh_callback(void* ctx, mem_t pub_key, buf_t& out_secret) { return ecdh_t::execute(ctx, pub_key, out_secret); }

std::vector<std::pair<unsigned, bool>> read_wnaf(booth_wnaf_t& wnaf) {
  std::vector<std::pair<unsigned, bool>> digits;
  unsigned value = 0;
  bool neg = false;
  while (wnaf.get(value, neg)) digits.emplace_back(value, neg);
  return digits;
}

bn_t bip340_challenge(const bn_t& rx, const ecc_point_t& pub_key, mem_t message) {
  const buf_t tag_hash = sha256_t::hash(mem_t("BIP0340/challenge"));
  return bn_t::from_bin(sha256_t::hash(tag_hash, tag_hash, rx.to_bin(32), pub_key.get_x().to_bin(32), message)) %
         curve_secp256k1.order();
}

class default_hook_curve_t final : public ecurve_interface_t {
 public:
  default_hook_curve_t() {
    type = ecurve_type_e::ossl;
    name = "DEFAULT_HOOK_TEST";
    bits = 256;
    openssl_code = 0;
  }

  void get_params(bn_t& p, bn_t& a, bn_t& b) const override {}
  void mul_to_generator(const bn_t& val, ecc_point_t& P) const override {}
  void mul_to_generator_vartime(const bn_t& val, ecc_point_t& P) const override {}
  void init_point(ecc_point_t& P) const override {}
  void free_point(ecc_point_t& P) const override {}
  void invert_point(ecc_point_t& P) const override {}
  void copy_point(ecc_point_t& Dst, const ecc_point_t& Src) const override {}
  bool equ_points(const ecc_point_t& P1, const ecc_point_t& P2) const override { return false; }
  bool is_on_curve(const ecc_point_t& P) const override { return false; }
  bool is_in_subgroup(const ecc_point_t& P) const override { return false; }
  bool is_infinity(const ecc_point_t& P) const override { return false; }
  void set_infinity(ecc_point_t& P) const override {}
  void add(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const override {}
  void add_consttime(const ecc_point_t& P, const ecc_point_t& x, ecc_point_t& R) const override {}
  void mul(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const override {}
  void mul_vartime(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const override {}
  int to_compressed_bin(const ecc_point_t& P, byte_ptr out) const override { return 0; }
  error_t from_bin(ecc_point_t& P, mem_t bin) const override { return SUCCESS; }
  void get_coordinates(const ecc_point_t& P, bn_t& x, bn_t& y) const override {}
  bool hash_to_point(mem_t bin, ecc_point_t& Q) const override { return false; }
  const mod_t& order() const override { return curve_p256.order(); }
  const mod_t& p() const override { return curve_p256.p(); }
  const ecc_generator_point_t& generator() const override { return curve_p256.generator(); }
  buf_t pub_to_der(const ecc_pub_key_t& P) const override { return buf_t(); }
  buf_t prv_to_der(const ecc_prv_key_t& K) const override { return buf_t(); }
  error_t verify(const ecc_pub_key_t& P, mem_t hash, mem_t sig) const override { return E_NOT_SUPPORTED; }
  buf_t sign(const ecc_prv_key_t& K, mem_t hash) const override { return buf_t(); }
};

class ECC : public ::testing::Test {
 protected:
  void SetUp() override {
    // Setup code if needed
  }

  void TearDown() override {
    // Cleanup code if needed
  }
};

TEST_F(ECC, NullCurveComparisonsAreExplicit) {
  ecurve_t empty;

  EXPECT_TRUE(empty == nullptr);
  EXPECT_FALSE(curve_p256 == nullptr);
  EXPECT_FALSE(empty != nullptr);
  EXPECT_TRUE(curve_p256 != nullptr);
  EXPECT_EQ(empty.ct_add_support(), ct_add_support_e::None);
}

TEST_F(ECC, secp256k1) {
  ecurve_t curve = curve_secp256k1;
  const mod_t& q = curve.order();
  const auto& G = curve.generator();
  EXPECT_TRUE(G.is_on_curve());

  ecc_point_t GG = G;

  for (int i = 0; i < 1000; i++) {
    bn_t a = bn_t::rand(q);
    bn_t b = bn_t::rand(q);
    bn_t c;
    MODULO(q) c = a + b;

    ecc_point_t A = a * G;
    EXPECT_TRUE(A == a * GG);
    ecc_point_t B = b * G;
    EXPECT_TRUE(B == b * GG);
    ecc_point_t C = c * G;
    EXPECT_TRUE(C == c * GG);

    EXPECT_TRUE(A.is_on_curve());
    EXPECT_TRUE(B.is_on_curve());
    EXPECT_TRUE(C.is_on_curve());
    {
      vartime_scope_t vartime_scope;
      EXPECT_TRUE(A + B == C);
    }

    MODULO(q) c = a - b;
    C = c * G;
    EXPECT_TRUE(C.is_on_curve());
    {
      vartime_scope_t vartime_scope;
      EXPECT_TRUE(A - B == C);
    }

    buf_t bin = C.to_compressed_bin();
    ecc_point_t D;
    EXPECT_OK(D.from_bin(curve, bin));
    EXPECT_TRUE(D.is_on_curve());
    EXPECT_TRUE(C == D);

    {
      vartime_scope_t vartime_scope;
      EXPECT_TRUE(((q - 1) * A + A).is_infinity());
      EXPECT_TRUE(((q - 1) * B + B).is_infinity());
      EXPECT_TRUE(((q - 1) * C + C).is_infinity());
    }
  }
}

TEST_F(ECC, secp256k1_batch_get_coordinates) {
  std::vector<ecc_point_t> points;
  for (int i = 1; i <= 8; ++i) points.push_back(bn_t(i) * curve_secp256k1.generator());

  std::vector<bn256_t> xs, ys;
  ecurve_secp256k1_t::get_coordinates(points, xs, ys);

  ASSERT_EQ(xs.size(), points.size());
  ASSERT_EQ(ys.size(), points.size());
  for (size_t i = 0; i < points.size(); ++i) {
    bn_t x, y;
    points[i].get_coordinates(x, y);
    EXPECT_EQ(xs[i], bn256_t(x));
    EXPECT_EQ(ys[i], bn256_t(y));
  }
}

TEST_F(ECC, secp256k1_batch_get_coordinates_rejects_wrong_curve) {
  std::vector<ecc_point_t> points = {ecc_point_t(curve_p256.generator())};
  std::vector<bn256_t> xs, ys;
  EXPECT_CB_ASSERT(ecurve_secp256k1_t::get_coordinates(points, xs, ys), "curve_secp256k1");
}

TEST_F(ECC, SumMulMatchesSequentialEvaluation) {
  auto check = [](ecurve_t curve, int count) {
    const auto& G = curve.generator();
    std::vector<ecc_point_t> points(count);
    std::vector<bn_t> scalars(count);
    ecc_point_t expected = curve.infinity();
    vartime_scope_t vartime_scope;
    for (int i = 0; i < count; i++) {
      points[i] = bn_t(i + 2) * G;
      scalars[i] = (i % 5 == 0) ? bn_t(0) : bn_t(i + 3);
      expected += scalars[i] * points[i];
    }

    EXPECT_EQ(sum_mul(points, scalars), expected);
  };

  check(curve_p256, 32);
  check(curve_p256, 31);
  check(curve_secp256k1, 32);

  std::vector<ecc_point_t> zero_points(32, curve_p256.generator());
  std::vector<bn_t> zero_scalars(32, bn_t(0));
  EXPECT_TRUE(sum_mul(zero_points, zero_scalars).is_infinity());

  dylog_disable_scope_t no_log;
  EXPECT_CB_ASSERT(sum_mul({}, {}), "!points.empty()");
  EXPECT_CB_ASSERT(sum_mul({curve_p256.generator()}, {}), "points.size() == scalars.size()");

  std::vector<ecc_point_t> mixed_points(32, curve_p256.generator());
  std::vector<bn_t> mixed_scalars(32, bn_t(1));
  mixed_points[7] = curve_secp256k1.generator();
  EXPECT_CB_ASSERT(sum_mul(mixed_points, mixed_scalars), "sum_mul: point curve mismatch");

  std::vector<ecc_point_t> detached_points(32, curve_p256.generator());
  std::vector<bn_t> detached_scalars(32, bn_t(1));
  EC_POINT* detached = detached_points[0].detach();
  EXPECT_CB_ASSERT(sum_mul(detached_points, detached_scalars), "sum_mul: point curve mismatch");
  EC_POINT_free(detached);
}

TEST_F(ECC, RejectsInvalidPointEncoding) {
  ecurve_t curve = curve_secp256k1;
  ecc_point_t point(curve);

  buf_t invalid_prefix = curve.generator().to_compressed_bin();
  invalid_prefix[0] = 0x05;
  EXPECT_ER(point.from_bin(curve, invalid_prefix));

  buf_t truncated(32);
  truncated[0] = 0x02;
  EXPECT_ER(point.from_bin(curve, truncated));
}

TEST_F(ECC, Secp256k1InfinitySerializationZeroesFullOutput) {
  const ecc_point_t infinity = curve_secp256k1.infinity();

  buf_t compressed(33);
  memset(compressed.data(), 0xa5, compressed.size());
  EXPECT_EQ(infinity.to_compressed_bin(compressed.data()), compressed.size());
  for (int i = 0; i < compressed.size(); i++) EXPECT_EQ(compressed[i], 0);

  buf_t uncompressed(65);
  memset(uncompressed.data(), 0xa5, uncompressed.size());
  EXPECT_EQ(infinity.to_bin(uncompressed.data()), uncompressed.size());
  for (int i = 0; i < uncompressed.size(); i++) EXPECT_EQ(uncompressed[i], 0);
}

TEST_F(ECC, PointOctetWrappersRoundTrip) {
  for (const ecurve_t& curve : {curve_p256, curve_secp256k1, curve_ed25519}) {
    vartime_scope_t vartime_scope;
    const ecc_point_t point = bn_t(7) * curve.generator();

    const buf_t compressed = point.to_compressed_oct();
    ecc_point_t compressed_roundtrip;
    EXPECT_OK(compressed_roundtrip.from_oct(curve, compressed));
    EXPECT_TRUE(compressed_roundtrip == point);

    const buf_t octets = point.to_oct();
    ecc_point_t octets_roundtrip;
    EXPECT_OK(octets_roundtrip.from_oct(curve, octets));
    EXPECT_TRUE(octets_roundtrip == point);
  }
}

TEST_F(ECC, PointOctetWrappersWriteIntoCallerBuffers) {
  const ecc_point_t point = bn_t(9) * curve_p256.generator();

  buf_t compressed(point.to_compressed_oct().size());
  EXPECT_EQ(point.to_compressed_oct(compressed.data()), compressed.size());
  EXPECT_EQ(compressed, point.to_compressed_oct());

  buf_t uncompressed(point.to_oct().size());
  EXPECT_EQ(point.to_oct(uncompressed.data()), uncompressed.size());
  EXPECT_EQ(uncompressed, point.to_oct());

  ecc_point_t roundtrip;
  EXPECT_OK(roundtrip.from_oct(curve_p256, uncompressed));
  EXPECT_TRUE(roundtrip == point);
}

TEST_F(ECC, EmptyPointSerializationIsRejectedOnDeserialize) {
  // Empty ecc_point_t still serializes to the historical null-curve sentinel,
  // but that sentinel is no longer accepted from untrusted input on read.
  const ecc_point_t empty;
  const buf_t encoded = coinbase::ser(empty);
  ASSERT_EQ(encoded.size(), 2);
  EXPECT_EQ(encoded[0], 0);
  EXPECT_EQ(encoded[1], 0);

  const ecc_point_t original = bn_t(5) * curve_p256.generator();
  ecc_point_t decoded = original;
  ASSERT_TRUE(decoded.valid());

  {
    dylog_disable_scope_t no_log_err;
    EXPECT_ER(coinbase::deser(encoded, decoded));
  }
  EXPECT_TRUE(decoded == original);
}

TEST_F(ECC, DetachTransfersOsslPointOwnershipUntilReattached) {
  const ecc_point_t original = bn_t(5) * curve_p256.generator();
  ecc_point_t detached_source(original);

  EC_POINT* raw = detached_source.detach();
  ASSERT_NE(raw, nullptr);
  EXPECT_FALSE(detached_source.valid());

  ecc_point_t reattached;
  reattached.attach(curve_p256, raw);
  EXPECT_TRUE(reattached == original);
}

TEST_F(ECC, KeyValidityAndPubKeyEquality) {
  ecc_prv_key_t empty;
  EXPECT_FALSE(empty.valid());

  ecc_prv_key_t prv_key;
  prv_key.generate(curve_ed25519);
  EXPECT_TRUE(prv_key.valid());
  EXPECT_FALSE(prv_key.get_ed_bin().empty());

  ecc_pub_key_t pub_key = prv_key.pub();
  ecc_pub_key_t same_pub_key(pub_key);
  EXPECT_TRUE(pub_key == same_pub_key);
  EXPECT_FALSE(pub_key != same_pub_key);

  ecc_prv_key_t other_key;
  other_key.generate(curve_ed25519);
  EXPECT_TRUE(pub_key != other_key.pub());
}

TEST_F(ECC, SignatureWithPubKeySerializesAndVerifies) {
  ecc_prv_key_t prv_key;
  prv_key.generate(curve_ed25519);
  const buf_t message = buf_t("message to sign");

  sig_with_pub_key_t signature = prv_key.sign_and_output_pub_key(message);
  EXPECT_OK(signature.verify(message));

  buf_t encoded = coinbase::ser(signature);
  sig_with_pub_key_t decoded;
  EXPECT_OK(coinbase::deser(encoded, decoded));

  EXPECT_TRUE(decoded.Q == signature.Q);
  EXPECT_EQ(decoded.sig, signature.sig);
  EXPECT_OK(decoded.verify(message));
}

TEST_F(ECC, SignatureVerificationFailures) {
  ecurve_t curve = curve_ed25519;
  ecc_prv_key_t prv_key;
  prv_key.generate(curve);
  ecc_pub_key_t pub_key(prv_key.pub());

  const buf_t message = gen_random(64);
  const buf_t signature = prv_key.sign(message);
  EXPECT_OK(pub_key.verify(message, signature));

  const buf_t wrong_message = gen_random(64);
  EXPECT_ER(pub_key.verify(wrong_message, signature));

  ecc_prv_key_t other_key;
  other_key.generate(curve);
  EXPECT_ER(other_key.pub().verify(message, signature));
}

TEST_F(ECC, ConstTimeAddAndConditionalCopy) {
  ecurve_t curve = curve_secp256k1;
  const mod_t& q = curve.order();
  const ecc_point_t a = bn_t(3) * curve.generator();
  const ecc_point_t b = bn_t(5) * curve.generator();
  const ecc_point_t expected = bn_t(8) * curve.generator();

  EXPECT_TRUE(ecc_point_t::add_consttime(a, b) == expected);

  ecc_point_t dst = curve.infinity();
  curve.cnd_copy_point(true, a, dst);
  EXPECT_TRUE(dst == a);

  ecc_point_t unchanged = curve.infinity();
  curve.cnd_copy_point(false, a, unchanged);
  EXPECT_TRUE(unchanged.is_infinity());
}

TEST_F(ECC, SelfAdditionMatchesDistinctStorageAndScalarDoubling) {
  for (const ecurve_t& curve : {curve_secp256k1, curve_ed25519}) {
    const ecc_point_t point = bn_t(7) * curve.generator();
    const ecc_point_t equal_but_distinct = point;
    const ecc_point_t expected = bn_t(14) * curve.generator();

    EXPECT_EQ(ecc_point_t::add(point, point), expected);
    EXPECT_EQ(ecc_point_t::add(point, equal_but_distinct), expected);
    EXPECT_EQ(ecc_point_t::add_consttime(point, point), expected);

    const ecc_point_t infinity = curve.infinity();
    EXPECT_TRUE(ecc_point_t::add(infinity, infinity).is_infinity());
  }
}

TEST_F(ECC, EcdhExecuteSupportsKeyAndCallbackBackends) {
  ecc_prv_key_t alice;
  alice.generate(curve_p256);
  ecc_prv_key_t bob;
  bob.generate(curve_p256);
  const ecc_pub_key_t bob_pub = bob.pub();
  const buf_t expected = alice.ecdh(bob_pub);

  ecdh_t key_backed(alice);
  buf_t key_backed_secret;
  EXPECT_OK(key_backed.execute(bob_pub, key_backed_secret));
  EXPECT_EQ(key_backed_secret, expected);

  ecdh_t callback_backed(ecdh_callback, &alice);
  buf_t callback_secret;
  EXPECT_OK(callback_backed.execute(bob_pub, callback_secret));
  EXPECT_EQ(callback_secret, expected);

  buf_t direct_secret;
  EXPECT_OK(alice.execute(bob_pub.to_oct(), direct_secret));
  EXPECT_EQ(direct_secret, expected);
}

TEST_F(ECC, DefaultCurveInterfaceHooksAreNoOpOrUnsupported) {
  default_hook_curve_t curve;
  ecc_point_t point;
  ecc_pub_key_t pub_key;
  ecc_prv_key_t prv_key;
  dylog_disable_scope_t no_log;

  curve.set_ossl_point(point, nullptr);
  EXPECT_EQ(curve.pub_from_der(pub_key, mem_t()), E_NOT_SUPPORTED);
  EXPECT_EQ(curve.prv_from_der(prv_key, mem_t()), E_NOT_SUPPORTED);
}

TEST_F(ECC, SignatureWithPubKeyVerifyAllChecksAggregateAndCurves) {
  const buf_t message = buf_t("aggregate signature message");

  ecc_prv_key_t key1;
  key1.generate(curve_ed25519);
  ecc_prv_key_t key2;
  key2.generate(curve_ed25519);

  std::vector<sig_with_pub_key_t> sigs = {key1.sign_and_output_pub_key(message), key2.sign_and_output_pub_key(message)};
  ecc_point_t aggregate = curve_ed25519.infinity();
  aggregate += sigs[0].Q;
  aggregate += sigs[1].Q;

  EXPECT_OK(sig_with_pub_key_t::verify_all(aggregate, message, sigs));
  EXPECT_ER(sig_with_pub_key_t::verify_all(aggregate + curve_ed25519.generator(), message, sigs));

  std::vector<sig_with_pub_key_t> empty;
  EXPECT_ER(sig_with_pub_key_t::verify_all(aggregate, message, empty));

  ecc_prv_key_t p256_key;
  p256_key.set(curve_p256, bn_t(19));
  sigs.push_back(p256_key.sign_and_output_pub_key(sha256_t::hash(message)));
  EXPECT_ER(sig_with_pub_key_t::verify_all(aggregate, message, sigs));
}

TEST_F(ECC, SchnorrSignVerifyAndRejectsTampering) {
  ecc_prv_key_t prv_key;
  prv_key.set(curve_secp256k1, bn_t::from_hex("123456789abcdef123456789abcdef"));
  const ecc_pub_key_t pub_key = prv_key.pub();
  const buf_t message = sha256_t::hash(mem_t("schnorr-message"));

  buf_t signature = prv_key.sign_schnorr(message);
  EXPECT_EQ(signature.size(), size_t(curve_secp256k1.size() * 2));
  EXPECT_OK(pub_key.verify_schnorr(message, signature));

  buf_t wrong_message = sha256_t::hash(mem_t("different-schnorr-message"));
  EXPECT_ER(pub_key.verify_schnorr(wrong_message, signature));

  signature[signature.size() - 1] ^= 0x01;
  EXPECT_ER(pub_key.verify_schnorr(message, signature));
  EXPECT_ER(pub_key.verify_schnorr(message, signature.take(signature.size() - 1)));
}

TEST_F(ECC, BoothWnafWordsConstructorMatchesBnConstructor) {
  const uint256_t scalar = uint256_t::from_hex("06c1d4e7f0a3b6c9dceff1023456789abcdef112233445566778899aabbccdde");
  const uint64_t limbs[4] = {scalar.w0, scalar.w1, scalar.w2, scalar.w3};

  booth_wnaf_t from_bn_forward(5, scalar.to_bn(), 256, false);
  booth_wnaf_t from_words_forward(5, limbs, 256, false);
  EXPECT_EQ(read_wnaf(from_words_forward), read_wnaf(from_bn_forward));

  booth_wnaf_t from_bn_backward(5, scalar.to_bn(), 256, true);
  booth_wnaf_t from_words_backward(5, limbs, 256, true);
  EXPECT_EQ(read_wnaf(from_words_backward), read_wnaf(from_bn_backward));
}

TEST_F(ECC, EcdsaSignatureDerRoundTrip) {
  const bn_t r = 7;
  const bn_t s = 11;
  ecdsa_signature_t signature(curve_secp256k1, r, s);
  EXPECT_TRUE(signature.valid());
  EXPECT_EQ(signature.get_curve(), curve_secp256k1);
  EXPECT_EQ(signature.get_r(), r);
  EXPECT_EQ(signature.get_s(), s);

  const buf_t der = signature.to_der();
  ecdsa_signature_t parsed;
  EXPECT_OK(parsed.from_der(curve_secp256k1, der));
  EXPECT_TRUE(parsed.valid());
  EXPECT_EQ(parsed.get_curve(), curve_secp256k1);
  EXPECT_EQ(parsed.get_r(), r);
  EXPECT_EQ(parsed.get_s(), s);

  EXPECT_ER(parsed.from_der(curve_secp256k1, mem_t("not-der")));
}

TEST_F(ECC, EcdsaSignVerifyAndRecoverPublicKey) {
  for (const ecurve_t& curve : {curve_p256, curve_secp256k1}) {
    ecc_prv_key_t prv_key;
    prv_key.set(curve, bn_t::from_hex("1d1ce5c0ffee123456789abcdef"));
    const ecc_pub_key_t pub_key = prv_key.pub();
    const buf_t digest = sha256_t::hash(mem_t("ecdsa recovery message"));

    const buf_t der = prv_key.sign(digest);
    EXPECT_OK(pub_key.verify(digest, der));
    EXPECT_ER(pub_key.verify(sha256_t::hash(mem_t("wrong ecdsa message")), der));

    ecdsa_signature_t parsed;
    ASSERT_OK(parsed.from_der(curve, der));

    int recovery_code = -1;
    EXPECT_OK(parsed.get_recovery_code(digest, pub_key, recovery_code));
    EXPECT_TRUE(recovery_code == 0 || recovery_code == 1);

    ecc_point_t recovered;
    EXPECT_OK(parsed.recover_pub_key(digest, recovery_code, recovered));
    EXPECT_TRUE(recovered == pub_key);
    EXPECT_ER(parsed.recover_pub_key(digest, 2, recovered));
  }
}

TEST_F(ECC, CurveAndPointFormattingFindAndCoordinateEdges) {
  std::ostringstream curve_out;
  curve_out << curve_p256;
  EXPECT_EQ(curve_out.str(), curve_p256.get_name());
  EXPECT_EQ(ecurve_t::find(curve_p256.get_group()), curve_p256);
  EXPECT_EQ(ecurve_t::find(curve_secp256k1.get_group()), curve_secp256k1);
  EXPECT_EQ(curve_p256.point_bin_size(), 65);
  EXPECT_EQ(curve_secp256k1.point_bin_size(), 65);
  EXPECT_EQ(curve_ed25519.point_bin_size(), 32);
  EXPECT_EQ(curve_p256.p().get_bits_count(), 256);

  std::ostringstream infinity_out;
  infinity_out << curve_secp256k1.infinity();
  EXPECT_EQ(infinity_out.str(), "infinity");

  std::ostringstream generator_out;
  generator_out << curve_secp256k1.generator();
  EXPECT_NE(generator_out.str().find("..."), std::string::npos);

  bn_t ix = 1;
  bn_t iy = 1;
  curve_secp256k1.infinity().get_coordinates(ix, iy);
  EXPECT_EQ(ix, 0);
  EXPECT_EQ(iy, 0);

  std::vector<bn256_t> xs = {bn256_t(1)};
  std::vector<bn256_t> ys = {bn256_t(2)};
  std::vector<ecc_point_t> empty_points;
  ecurve_secp256k1_t::get_coordinates(empty_points, xs, ys);
  EXPECT_TRUE(xs.empty());
  EXPECT_TRUE(ys.empty());
}

TEST_F(ECC, PointBn256CoordinatesMatchBnCoordinates) {
  const ecc_point_t point = bn_t(5) * curve_secp256k1.generator();

  bn_t x;
  bn_t y;
  point.get_coordinates(x, y);

  bn256_t x256;
  bn256_t y256;
  point.get_coordinates(x256, y256);

  EXPECT_EQ(bn_t(x256), x);
  EXPECT_EQ(bn_t(y256), y);
}

TEST_F(ECC, Bip340VerifyAcceptsConstructedSignatureAndRejectsMalformedInputs) {
  const mod_t& q = curve_secp256k1.order();
  const ecc_generator_point_t& G = curve_secp256k1.generator();

  bn_t x = 3;
  ecc_point_t Q = x * G;
  if (Q.get_y().is_odd()) {
    MODULO(q) x = -x;
    Q = x * G;
  }

  bn_t k = 5;
  ecc_point_t R = k * G;
  if (R.get_y().is_odd()) {
    MODULO(q) k = -k;
    R = k * G;
  }

  const buf_t message = sha256_t::hash(mem_t("bip340 message"));
  const bn_t e = bip340_challenge(R.get_x(), Q, message);
  bn_t s;
  MODULO(q) s = k + e * x;

  buf_t signature = R.get_x().to_bin(32) + s.to_bin(32);
  EXPECT_OK(bip340::verify(Q, message, signature));

  buf_t wrong_message = sha256_t::hash(mem_t("wrong bip340 message"));
  EXPECT_ER(bip340::verify(Q, wrong_message, signature));
  EXPECT_ER(bip340::verify(Q, message.take(message.size() - 1), signature));
  EXPECT_ER(bip340::verify(Q, message, signature.take(signature.size() - 1)));

  buf_t bad_r = signature;
  memmove(bad_r.data(), curve_secp256k1.p().value().to_bin(32).data(), 32);
  EXPECT_ER(bip340::verify(Q, message, bad_r));

  buf_t bad_s = signature;
  memmove(bad_s.data() + 32, q.value().to_bin(32).data(), 32);
  EXPECT_ER(bip340::verify(Q, message, bad_s));
}

TEST_F(ECC, UnsupportedKeyDerSerializationFailsExplicitly) {
  ecc_prv_key_t prv_key;
  prv_key.generate(curve_secp256k1);
  ecc_pub_key_t pub_key = prv_key.pub();

  dylog_disable_scope_t no_log;
  EXPECT_CB_ASSERT(pub_key.to_der(), "pub_to_der is not supported");
}

TEST_F(ECC, SigningScheme2) {
  for (const auto len : {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024}) {
    std::cout << "======================================== len: " << len << std::endl;
    for (int i = 0; i < 5; i++) {
      ecurve_t curve = curve_ed25519;
      const mod_t& q = curve.order();

      ecc_prv_key_t prv_key;
      prv_key.generate(curve);
      ecc_pub_key_t pub_key(prv_key.pub());

      buf_t hash = gen_random(len);
      buf_t signature = prv_key.sign(hash);
      EXPECT_OK(pub_key.verify(hash, signature));
    }
  }
}

}  // namespace
