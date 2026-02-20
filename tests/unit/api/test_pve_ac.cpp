#include <array>
#include <gtest/gtest.h>

#include <cbmpc/api/pve_base_pke.h>
#include <cbmpc/api/pve_batch_ac.h>
#include <cbmpc/core/access_structure.h>
#include <cbmpc/core/macros.h>
#include <cbmpc/internal/crypto/base_ecc.h>

namespace {

using coinbase::buf_t;
using coinbase::error_t;
using coinbase::mem_t;

using coinbase::api::curve_id;

static buf_t expected_Q(curve_id cid, mem_t x) {
  const coinbase::crypto::ecurve_t curve = (cid == curve_id::p256)        ? coinbase::crypto::curve_p256
                                           : (cid == curve_id::secp256k1) ? coinbase::crypto::curve_secp256k1
                                           : (cid == curve_id::ed25519)   ? coinbase::crypto::curve_ed25519
                                                                          : coinbase::crypto::ecurve_t();
  cb_assert(curve.valid());

  const coinbase::crypto::bn_t bn_x = coinbase::crypto::bn_t::from_bin(x) % curve.order();
  const coinbase::crypto::ecc_point_t Q = bn_x * curve.generator();
  return Q.to_compressed_bin();
}

class toy_base_pke_t final : public coinbase::api::pve::base_pke_i {
 public:
  error_t encrypt(mem_t /*ek*/, mem_t /*label*/, mem_t plain, mem_t /*rho*/, buf_t& out_ct) const override {
    out_ct = buf_t(plain);
    return SUCCESS;
  }

  error_t decrypt(mem_t /*dk*/, mem_t /*label*/, mem_t ct, buf_t& out_plain) const override {
    out_plain = buf_t(ct);
    return SUCCESS;
  }
};

}  // namespace

TEST(ApiPveAc, EncVer_PDec_Agg_DefPke_Rsa) {
  const curve_id curve = curve_id::secp256k1;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  constexpr int n = 4;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++) xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  std::array<buf_t, 3> eks{};
  std::array<buf_t, 3> dks{};
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(eks[0], dks[0]), SUCCESS);
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(eks[1], dks[1]), SUCCESS);
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(eks[2], dks[2]), SUCCESS);

  coinbase::api::pve::leaf_keys_t ac_pks;
  ASSERT_TRUE(ac_pks.emplace("p1", mem_t(eks[0].data(), eks[0].size())).second);
  ASSERT_TRUE(ac_pks.emplace("p2", mem_t(eks[1].data(), eks[1].size())).second);
  ASSERT_TRUE(ac_pks.emplace("p3", mem_t(eks[2].data(), eks[2].size())).second);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(curve, ac, ac_pks, label, xs, ct), SUCCESS);

  int batch_count = 0;
  ASSERT_EQ(coinbase::api::pve::get_ac_batch_count(ct, batch_count), SUCCESS);
  ASSERT_EQ(batch_count, n);

  std::vector<buf_t> Qs_expected;
  Qs_expected.reserve(n);
  for (int i = 0; i < n; i++) Qs_expected.push_back(expected_Q(curve, xs[static_cast<size_t>(i)]));

  std::vector<mem_t> Qs_expected_mem;
  Qs_expected_mem.reserve(n);
  for (const auto& q : Qs_expected) Qs_expected_mem.emplace_back(q.data(), q.size());

  ASSERT_EQ(coinbase::api::pve::verify_ac(curve, ac, ac_pks, ct, Qs_expected_mem, label), SUCCESS);

  const int attempt_index = 0;
  buf_t share_p1;
  buf_t share_p2;
  ASSERT_EQ(coinbase::api::pve::partial_decrypt_ac_attempt(curve, ac, ct, attempt_index, "p1",
                                                           mem_t(dks[0].data(), dks[0].size()), label, share_p1),
            SUCCESS);
  ASSERT_EQ(coinbase::api::pve::partial_decrypt_ac_attempt(curve, ac, ct, attempt_index, "p2",
                                                           mem_t(dks[1].data(), dks[1].size()), label, share_p2),
            SUCCESS);

  coinbase::api::pve::leaf_shares_t quorum;
  ASSERT_TRUE(quorum.emplace("p1", mem_t(share_p1.data(), share_p1.size())).second);
  ASSERT_TRUE(quorum.emplace("p2", mem_t(share_p2.data(), share_p2.size())).second);

  std::vector<buf_t> xs_out;
  ASSERT_EQ(coinbase::api::pve::combine_ac(curve, ac, ct, attempt_index, label, quorum, xs_out), SUCCESS);

  ASSERT_EQ(xs_out.size(), static_cast<size_t>(n));
  for (int i = 0; i < n; i++) EXPECT_EQ(xs_out[static_cast<size_t>(i)], buf_t(xs[static_cast<size_t>(i)]));

  // Insufficient quorum should fail.
  coinbase::api::pve::leaf_shares_t insufficient;
  ASSERT_TRUE(insufficient.emplace("p1", mem_t(share_p1.data(), share_p1.size())).second);
  std::vector<buf_t> xs_out2;
  EXPECT_NE(coinbase::api::pve::combine_ac(curve, ac, ct, attempt_index, label, insufficient, xs_out2), SUCCESS);
}

TEST(ApiPveAc, EncryptRejectsOversizedX) {
  const curve_id curve = curve_id::secp256k1;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  std::array<uint8_t, 33> x_bytes{};
  for (int i = 0; i < 33; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x20 + i);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));

  std::array<buf_t, 3> eks{};
  std::array<buf_t, 3> dks{};
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(eks[0], dks[0]), SUCCESS);
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(eks[1], dks[1]), SUCCESS);
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(eks[2], dks[2]), SUCCESS);

  coinbase::api::pve::leaf_keys_t ac_pks;
  ASSERT_TRUE(ac_pks.emplace("p1", mem_t(eks[0].data(), eks[0].size())).second);
  ASSERT_TRUE(ac_pks.emplace("p2", mem_t(eks[1].data(), eks[1].size())).second);
  ASSERT_TRUE(ac_pks.emplace("p3", mem_t(eks[2].data(), eks[2].size())).second);

  buf_t ct;
  EXPECT_EQ(coinbase::api::pve::encrypt_ac(curve, ac, ac_pks, label, xs, ct), E_RANGE);
}

TEST(ApiPveAc, EncVer_PartDec_Agg_CustomBasePke) {
  const toy_base_pke_t base_pke;

  const curve_id curve = curve_id::secp256k1;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  constexpr int n = 3;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++)
      xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(0x77 + i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  // Toy per-leaf keys.
  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");

  coinbase::api::pve::leaf_keys_t ac_pks;
  ASSERT_TRUE(ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size())).second);
  ASSERT_TRUE(ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size())).second);
  ASSERT_TRUE(ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size())).second);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve, ac, ac_pks, label, xs, ct), SUCCESS);

  std::vector<buf_t> Qs_expected;
  Qs_expected.reserve(n);
  for (int i = 0; i < n; i++) Qs_expected.push_back(expected_Q(curve, xs[static_cast<size_t>(i)]));

  std::vector<mem_t> Qs_expected_mem;
  Qs_expected_mem.reserve(n);
  for (const auto& q : Qs_expected) Qs_expected_mem.emplace_back(q.data(), q.size());

  ASSERT_EQ(coinbase::api::pve::verify_ac(base_pke, curve, ac, ac_pks, ct, Qs_expected_mem, label), SUCCESS);

  const int attempt_index = 0;
  buf_t share_p1;
  buf_t share_p3;
  ASSERT_EQ(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve, ac, ct, attempt_index, "p1", ek1, label,
                                                           share_p1),
            SUCCESS);
  ASSERT_EQ(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve, ac, ct, attempt_index, "p3", ek3, label,
                                                           share_p3),
            SUCCESS);

  coinbase::api::pve::leaf_shares_t quorum;
  ASSERT_TRUE(quorum.emplace("p1", mem_t(share_p1.data(), share_p1.size())).second);
  ASSERT_TRUE(quorum.emplace("p3", mem_t(share_p3.data(), share_p3.size())).second);

  std::vector<buf_t> xs_out;
  ASSERT_EQ(coinbase::api::pve::combine_ac(base_pke, curve, ac, ct, attempt_index, label, quorum, xs_out), SUCCESS);

  ASSERT_EQ(xs_out.size(), static_cast<size_t>(n));
  for (int i = 0; i < n; i++) EXPECT_EQ(xs_out[static_cast<size_t>(i)], buf_t(xs[static_cast<size_t>(i)]));
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

TEST(ApiPveAcNeg, EncryptAc_InvalidCurve) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  buf_t ct;
  EXPECT_NE(coinbase::api::pve::encrypt_ac(base_pke, static_cast<curve_id>(0), ac, ac_pks, label, xs, ct), SUCCESS);
  EXPECT_NE(coinbase::api::pve::encrypt_ac(base_pke, static_cast<curve_id>(4), ac, ac_pks, label, xs, ct), SUCCESS);
  EXPECT_NE(coinbase::api::pve::encrypt_ac(base_pke, static_cast<curve_id>(255), ac, ac_pks, label, xs, ct), SUCCESS);
}

TEST(ApiPveAcNeg, EncryptAc_EmptyLabel) {
  const toy_base_pke_t base_pke;

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  buf_t ct;
  EXPECT_NE(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, mem_t(), xs, ct), SUCCESS);
}

TEST(ApiPveAcNeg, EncryptAc_EmptyXsVector) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::vector<mem_t> xs;
  buf_t ct;
  EXPECT_NE(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);
}

TEST(ApiPveAcNeg, EncryptAc_XsWithEmptyElement) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::vector<mem_t> xs;
  xs.emplace_back(mem_t());
  buf_t ct;
  EXPECT_NE(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);
}

TEST(ApiPveAcNeg, EncryptAc_EmptyAcPks) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  coinbase::api::pve::leaf_keys_t ac_pks;
  buf_t ct;
  EXPECT_NE(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);
}

TEST(ApiPveAcNeg, EncryptAc_AcPksMissingLeaf) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));

  buf_t ct;
  EXPECT_NE(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);
}

TEST(ApiPveAcNeg, EncryptAc_AcPksExtraLeaf) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  const buf_t ek4 = buf_t("ek4");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));
  ac_pks.emplace("unknown", mem_t(ek4.data(), ek4.size()));

  buf_t ct;
  EXPECT_NE(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);
}

TEST(ApiPveAcNeg, EncryptAc_AcNoLeaves) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(2, {});

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  coinbase::api::pve::leaf_keys_t ac_pks;
  buf_t ct;
  EXPECT_NE(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);
}

TEST(ApiPveAcNeg, VerifyAc_InvalidCurve) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  buf_t Q = expected_Q(curve_id::secp256k1, xs[0]);
  std::vector<mem_t> Qs;
  Qs.emplace_back(Q.data(), Q.size());

  EXPECT_NE(coinbase::api::pve::verify_ac(base_pke, static_cast<curve_id>(0), ac, ac_pks, ct, Qs, label), SUCCESS);
  EXPECT_NE(coinbase::api::pve::verify_ac(base_pke, static_cast<curve_id>(4), ac, ac_pks, ct, Qs, label), SUCCESS);
  EXPECT_NE(coinbase::api::pve::verify_ac(base_pke, static_cast<curve_id>(255), ac, ac_pks, ct, Qs, label), SUCCESS);
}

TEST(ApiPveAcNeg, VerifyAc_EmptyCiphertext) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  buf_t Q_dummy = buf_t("Q");
  std::vector<mem_t> Qs;
  Qs.emplace_back(Q_dummy.data(), Q_dummy.size());

  EXPECT_NE(coinbase::api::pve::verify_ac(base_pke, curve_id::secp256k1, ac, ac_pks, mem_t(), Qs, label), SUCCESS);
}

TEST(ApiPveAcNeg, VerifyAc_EmptyQsCompressed) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  std::vector<mem_t> empty_Qs;
  EXPECT_NE(coinbase::api::pve::verify_ac(base_pke, curve_id::secp256k1, ac, ac_pks, ct, empty_Qs, label), SUCCESS);
}

TEST(ApiPveAcNeg, VerifyAc_EmptyLabel) {
  const toy_base_pke_t base_pke;

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  const buf_t label = buf_t("label");
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  buf_t Q = expected_Q(curve_id::secp256k1, xs[0]);
  std::vector<mem_t> Qs;
  Qs.emplace_back(Q.data(), Q.size());

  EXPECT_NE(coinbase::api::pve::verify_ac(base_pke, curve_id::secp256k1, ac, ac_pks, ct, Qs, mem_t()), SUCCESS);
}

TEST(ApiPveAcNeg, VerifyAc_GarbageCiphertext) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  const std::array<uint8_t, 4> garbage = {0xDE, 0xAD, 0xBE, 0xEF};
  const mem_t garbage_ct(garbage.data(), 4);

  buf_t Q_dummy = buf_t("Q");
  std::vector<mem_t> Qs;
  Qs.emplace_back(Q_dummy.data(), Q_dummy.size());

  EXPECT_NE(coinbase::api::pve::verify_ac(base_pke, curve_id::secp256k1, ac, ac_pks, garbage_ct, Qs, label), SUCCESS);
}

TEST(ApiPveAcNeg, PartialDecryptAcAttempt_InvalidCurve) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  buf_t share;
  EXPECT_NE(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, static_cast<curve_id>(0), ac, ct, 0, "p1", ek1,
                                                           label, share),
            SUCCESS);
  EXPECT_NE(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, static_cast<curve_id>(4), ac, ct, 0, "p1", ek1,
                                                           label, share),
            SUCCESS);
  EXPECT_NE(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, static_cast<curve_id>(255), ac, ct, 0, "p1", ek1,
                                                           label, share),
            SUCCESS);
}

TEST(ApiPveAcNeg, PartialDecryptAcAttempt_EmptyCiphertext) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t dk = buf_t("dk1");
  buf_t share;
  EXPECT_NE(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve_id::secp256k1, ac, mem_t(), 0, "p1", dk,
                                                           label, share),
            SUCCESS);
}

TEST(ApiPveAcNeg, PartialDecryptAcAttempt_EmptyDk) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  buf_t share;
  EXPECT_NE(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve_id::secp256k1, ac, ct, 0, "p1", mem_t(),
                                                           label, share),
            SUCCESS);
}

TEST(ApiPveAcNeg, PartialDecryptAcAttempt_EmptyLabel) {
  const toy_base_pke_t base_pke;

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  const buf_t label = buf_t("label");
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  buf_t share;
  EXPECT_NE(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve_id::secp256k1, ac, ct, 0, "p1", ek1, mem_t(),
                                                           share),
            SUCCESS);
}

TEST(ApiPveAcNeg, PartialDecryptAcAttempt_EmptyLeafName) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  const buf_t dk = buf_t("dk1");
  buf_t share;
  EXPECT_NE(
      coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve_id::secp256k1, ac, ct, 0, "", dk, label, share),
      SUCCESS);
}

TEST(ApiPveAcNeg, PartialDecryptAcAttempt_UnknownLeafName) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  const buf_t dk = buf_t("dk_nonexistent");
  buf_t share;
  EXPECT_NE(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve_id::secp256k1, ac, ct, 0, "nonexistent", dk,
                                                           label, share),
            SUCCESS);
}

TEST(ApiPveAcNeg, PartialDecryptAcAttempt_GarbageCiphertext) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const std::array<uint8_t, 4> garbage = {0xDE, 0xAD, 0xBE, 0xEF};
  const mem_t garbage_ct(garbage.data(), 4);

  const buf_t dk = buf_t("dk1");
  buf_t share;
  EXPECT_NE(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve_id::secp256k1, ac, garbage_ct, 0, "p1", dk,
                                                           label, share),
            SUCCESS);
}

TEST(ApiPveAcNeg, CombineAc_InvalidCurve) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  buf_t share_p1;
  buf_t share_p2;
  ASSERT_EQ(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve_id::secp256k1, ac, ct, 0, "p1", ek1, label,
                                                           share_p1),
            SUCCESS);
  ASSERT_EQ(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve_id::secp256k1, ac, ct, 0, "p2", ek2, label,
                                                           share_p2),
            SUCCESS);

  coinbase::api::pve::leaf_shares_t quorum;
  quorum.emplace("p1", mem_t(share_p1.data(), share_p1.size()));
  quorum.emplace("p2", mem_t(share_p2.data(), share_p2.size()));

  std::vector<buf_t> xs_out;
  EXPECT_NE(coinbase::api::pve::combine_ac(base_pke, static_cast<curve_id>(0), ac, ct, 0, label, quorum, xs_out),
            SUCCESS);
  EXPECT_NE(coinbase::api::pve::combine_ac(base_pke, static_cast<curve_id>(4), ac, ct, 0, label, quorum, xs_out),
            SUCCESS);
  EXPECT_NE(coinbase::api::pve::combine_ac(base_pke, static_cast<curve_id>(255), ac, ct, 0, label, quorum, xs_out),
            SUCCESS);
}

TEST(ApiPveAcNeg, CombineAc_EmptyCiphertext) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t share_dummy = buf_t("share_dummy_32bytes_padding_here");
  coinbase::api::pve::leaf_shares_t quorum;
  quorum.emplace("p1", mem_t(share_dummy.data(), share_dummy.size()));
  quorum.emplace("p2", mem_t(share_dummy.data(), share_dummy.size()));

  std::vector<buf_t> xs_out;
  EXPECT_NE(coinbase::api::pve::combine_ac(base_pke, curve_id::secp256k1, ac, mem_t(), 0, label, quorum, xs_out),
            SUCCESS);
}

TEST(ApiPveAcNeg, CombineAc_EmptyLabel) {
  const toy_base_pke_t base_pke;

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  const buf_t label = buf_t("label");
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  buf_t share_p1;
  buf_t share_p2;
  ASSERT_EQ(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve_id::secp256k1, ac, ct, 0, "p1", ek1, label,
                                                           share_p1),
            SUCCESS);
  ASSERT_EQ(coinbase::api::pve::partial_decrypt_ac_attempt(base_pke, curve_id::secp256k1, ac, ct, 0, "p2", ek2, label,
                                                           share_p2),
            SUCCESS);

  coinbase::api::pve::leaf_shares_t quorum;
  quorum.emplace("p1", mem_t(share_p1.data(), share_p1.size()));
  quorum.emplace("p2", mem_t(share_p2.data(), share_p2.size()));

  std::vector<buf_t> xs_out;
  EXPECT_NE(coinbase::api::pve::combine_ac(base_pke, curve_id::secp256k1, ac, ct, 0, mem_t(), quorum, xs_out), SUCCESS);
}

TEST(ApiPveAcNeg, CombineAc_EmptyQuorumShares) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const buf_t ek1 = buf_t("ek1");
  const buf_t ek2 = buf_t("ek2");
  const buf_t ek3 = buf_t("ek3");
  coinbase::api::pve::leaf_keys_t ac_pks;
  ac_pks.emplace("p1", mem_t(ek1.data(), ek1.size()));
  ac_pks.emplace("p2", mem_t(ek2.data(), ek2.size()));
  ac_pks.emplace("p3", mem_t(ek3.data(), ek3.size()));

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(i + 1);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), 32);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_ac(base_pke, curve_id::secp256k1, ac, ac_pks, label, xs, ct), SUCCESS);

  coinbase::api::pve::leaf_shares_t empty_quorum;
  std::vector<buf_t> xs_out;
  EXPECT_NE(coinbase::api::pve::combine_ac(base_pke, curve_id::secp256k1, ac, ct, 0, label, empty_quorum, xs_out),
            SUCCESS);
}

TEST(ApiPveAcNeg, CombineAc_GarbageCiphertext) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  const std::array<uint8_t, 4> garbage = {0xDE, 0xAD, 0xBE, 0xEF};
  const mem_t garbage_ct(garbage.data(), 4);

  const buf_t share_dummy = buf_t("share_dummy_32bytes_padding_here");
  coinbase::api::pve::leaf_shares_t quorum;
  quorum.emplace("p1", mem_t(share_dummy.data(), share_dummy.size()));
  quorum.emplace("p2", mem_t(share_dummy.data(), share_dummy.size()));

  std::vector<buf_t> xs_out;
  EXPECT_NE(coinbase::api::pve::combine_ac(base_pke, curve_id::secp256k1, ac, garbage_ct, 0, label, quorum, xs_out),
            SUCCESS);
}

TEST(ApiPveAcNeg, GetAcBatchCount_EmptyCiphertext) {
  int count = 0;
  EXPECT_NE(coinbase::api::pve::get_ac_batch_count(mem_t(), count), SUCCESS);
}

TEST(ApiPveAcNeg, GetAcBatchCount_GarbageCiphertext) {
  const std::array<uint8_t, 4> garbage = {0xDE, 0xAD, 0xBE, 0xEF};
  int count = 0;
  EXPECT_NE(coinbase::api::pve::get_ac_batch_count(mem_t(garbage.data(), 4), count), SUCCESS);
}

TEST(ApiPveAcNeg, GetPublicKeysCompressedAc_EmptyCiphertext) {
  std::vector<buf_t> Qs;
  EXPECT_NE(coinbase::api::pve::get_public_keys_compressed_ac(mem_t(), Qs), SUCCESS);
}

TEST(ApiPveAcNeg, GetPublicKeysCompressedAc_GarbageCiphertext) {
  const std::array<uint8_t, 4> garbage = {0xDE, 0xAD, 0xBE, 0xEF};
  std::vector<buf_t> Qs;
  EXPECT_NE(coinbase::api::pve::get_public_keys_compressed_ac(mem_t(garbage.data(), 4), Qs), SUCCESS);
}
