#include <array>
#include <gtest/gtest.h>

#include <cbmpc/api/pve.h>
#include <cbmpc/api/pve_batch.h>
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

TEST(ApiPveBatch, EncVerDec_DefBasePke_RsaBlob) {
  const curve_id curve = curve_id::secp256k1;
  const buf_t label = buf_t("label");

  buf_t ek_blob;
  buf_t dk_blob;
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(ek_blob, dk_blob), SUCCESS);

  constexpr int n = 4;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++) xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_batch(curve, ek_blob, label, xs, ct), SUCCESS);

  int batch_count = 0;
  ASSERT_EQ(coinbase::api::pve::get_batch_count(ct, batch_count), SUCCESS);
  ASSERT_EQ(batch_count, n);

  buf_t label_extracted;
  ASSERT_EQ(coinbase::api::pve::get_Label_batch(ct, label_extracted), SUCCESS);
  ASSERT_EQ(label_extracted, label);

  std::vector<buf_t> Qs_extracted;
  ASSERT_EQ(coinbase::api::pve::get_Qs_batch(ct, Qs_extracted), SUCCESS);
  ASSERT_EQ(Qs_extracted.size(), static_cast<size_t>(n));

  std::vector<buf_t> Qs_expected;
  Qs_expected.reserve(n);
  for (int i = 0; i < n; i++) Qs_expected.push_back(expected_Q(curve, xs[static_cast<size_t>(i)]));

  for (int i = 0; i < n; i++) EXPECT_EQ(Qs_extracted[static_cast<size_t>(i)], Qs_expected[static_cast<size_t>(i)]);

  std::vector<mem_t> Qs_expected_mem;
  Qs_expected_mem.reserve(n);
  for (const auto& q : Qs_expected) Qs_expected_mem.emplace_back(q.data(), q.size());

  ASSERT_EQ(coinbase::api::pve::verify_batch(curve, ek_blob, ct, Qs_expected_mem, label), SUCCESS);

  std::vector<buf_t> xs_out;
  ASSERT_EQ(coinbase::api::pve::decrypt_batch(curve, dk_blob, ek_blob, ct, label, xs_out), SUCCESS);
  ASSERT_EQ(xs_out.size(), static_cast<size_t>(n));
  for (int i = 0; i < n; i++) EXPECT_EQ(xs_out[static_cast<size_t>(i)], buf_t(xs[static_cast<size_t>(i)]));
}

TEST(ApiPveBatch, EncryptRejectsOversizedX) {
  const curve_id curve = curve_id::secp256k1;
  const buf_t label = buf_t("label");

  buf_t ek_blob;
  buf_t dk_blob;
  ASSERT_EQ(coinbase::api::pve::generate_base_pke_rsa_keypair(ek_blob, dk_blob), SUCCESS);

  std::array<uint8_t, 33> x_bytes{};
  for (int i = 0; i < 33; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x10 + i);
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ct;
  EXPECT_EQ(coinbase::api::pve::encrypt_batch(curve, ek_blob, label, xs, ct), E_RANGE);
}

TEST(ApiPveBatch, EncryptVerifyDecrypt_CustomBasePke) {
  const toy_base_pke_t base_pke;

  const curve_id curve = curve_id::secp256k1;
  const buf_t ek = buf_t("ek");
  const buf_t dk = buf_t("dk");
  const buf_t label = buf_t("label");

  constexpr int n = 3;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++)
      xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(0x77 + i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_batch(base_pke, curve, ek, label, xs, ct), SUCCESS);

  std::vector<buf_t> Qs_expected;
  Qs_expected.reserve(n);
  for (int i = 0; i < n; i++) Qs_expected.push_back(expected_Q(curve, xs[static_cast<size_t>(i)]));

  std::vector<mem_t> Qs_expected_mem;
  Qs_expected_mem.reserve(n);
  for (const auto& q : Qs_expected) Qs_expected_mem.emplace_back(q.data(), q.size());

  ASSERT_EQ(coinbase::api::pve::verify_batch(base_pke, curve, ek, ct, Qs_expected_mem, label), SUCCESS);

  std::vector<buf_t> xs_out;
  ASSERT_EQ(coinbase::api::pve::decrypt_batch(base_pke, curve, dk, ek, ct, label, xs_out), SUCCESS);
  ASSERT_EQ(xs_out.size(), static_cast<size_t>(n));
  for (int i = 0; i < n; i++) EXPECT_EQ(xs_out[static_cast<size_t>(i)], buf_t(xs[static_cast<size_t>(i)]));
}
