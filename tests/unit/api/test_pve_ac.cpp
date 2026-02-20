#include <array>
#include <gtest/gtest.h>

#include <cbmpc/api/pve.h>
#include <cbmpc/api/pve_ac.h>
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

  const int row_index = 0;
  buf_t share_p1;
  buf_t share_p2;
  ASSERT_EQ(coinbase::api::pve::party_decrypt_row_ac(curve, ac, ct, row_index, "p1",
                                                     mem_t(dks[0].data(), dks[0].size()), label, share_p1),
            SUCCESS);
  ASSERT_EQ(coinbase::api::pve::party_decrypt_row_ac(curve, ac, ct, row_index, "p2",
                                                     mem_t(dks[1].data(), dks[1].size()), label, share_p2),
            SUCCESS);

  coinbase::api::pve::leaf_shares_t quorum;
  ASSERT_TRUE(quorum.emplace("p1", mem_t(share_p1.data(), share_p1.size())).second);
  ASSERT_TRUE(quorum.emplace("p2", mem_t(share_p2.data(), share_p2.size())).second);

  std::vector<buf_t> xs_out;
  ASSERT_EQ(coinbase::api::pve::aggregate_to_restore_row_ac(curve, ac, ct, row_index, label, quorum, xs_out), SUCCESS);

  ASSERT_EQ(xs_out.size(), static_cast<size_t>(n));
  for (int i = 0; i < n; i++) EXPECT_EQ(xs_out[static_cast<size_t>(i)], buf_t(xs[static_cast<size_t>(i)]));

  // Insufficient quorum should fail.
  coinbase::api::pve::leaf_shares_t insufficient;
  ASSERT_TRUE(insufficient.emplace("p1", mem_t(share_p1.data(), share_p1.size())).second);
  std::vector<buf_t> xs_out2;
  EXPECT_NE(coinbase::api::pve::aggregate_to_restore_row_ac(curve, ac, ct, row_index, label, insufficient, xs_out2),
            SUCCESS);
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

  const int row_index = 0;
  buf_t share_p1;
  buf_t share_p3;
  ASSERT_EQ(coinbase::api::pve::party_decrypt_row_ac(base_pke, curve, ac, ct, row_index, "p1", ek1, label, share_p1),
            SUCCESS);
  ASSERT_EQ(coinbase::api::pve::party_decrypt_row_ac(base_pke, curve, ac, ct, row_index, "p3", ek3, label, share_p3),
            SUCCESS);

  coinbase::api::pve::leaf_shares_t quorum;
  ASSERT_TRUE(quorum.emplace("p1", mem_t(share_p1.data(), share_p1.size())).second);
  ASSERT_TRUE(quorum.emplace("p3", mem_t(share_p3.data(), share_p3.size())).second);

  std::vector<buf_t> xs_out;
  ASSERT_EQ(coinbase::api::pve::aggregate_to_restore_row_ac(base_pke, curve, ac, ct, row_index, label, quorum, xs_out),
            SUCCESS);

  ASSERT_EQ(xs_out.size(), static_cast<size_t>(n));
  for (int i = 0; i < n; i++) EXPECT_EQ(xs_out[static_cast<size_t>(i)], buf_t(xs[static_cast<size_t>(i)]));
}
