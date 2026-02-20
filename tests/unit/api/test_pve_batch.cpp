#include <array>
#include <gtest/gtest.h>

#include <cbmpc/api/pve_base_pke.h>
#include <cbmpc/api/pve_batch_single_recipient.h>
#include <cbmpc/core/macros.h>
#include <cbmpc/internal/core/log.h>
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
  ASSERT_EQ(coinbase::api::pve::get_public_keys_compressed_batch(ct, Qs_extracted), SUCCESS);
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

// ------------ Disclaimer: All the following tests have been generated by AI ------------

TEST(ApiPveBatchNeg, EncryptBatchInvalidCurve) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  dylog_disable_scope_t no_log_err;
  for (int c : {0, 4, 255}) {
    EXPECT_NE(coinbase::api::pve::encrypt_batch(base_pke, static_cast<curve_id>(c), ek, label, xs, ct), SUCCESS);
  }
}

TEST(ApiPveBatchNeg, EncryptBatchEmptyEk) {
  const toy_base_pke_t base_pke;
  const buf_t label = buf_t("label");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, mem_t(), label, xs, ct), SUCCESS);
}

TEST(ApiPveBatchNeg, EncryptBatchEmptyLabel) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, ek, mem_t(), xs, ct), SUCCESS);
}

TEST(ApiPveBatchNeg, EncryptBatchEmptyXsVector) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  std::vector<mem_t> xs;
  buf_t ct;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, ek, label, xs, ct), SUCCESS);
}

TEST(ApiPveBatchNeg, EncryptBatchXsWithEmptyElement) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  std::vector<mem_t> xs;
  xs.emplace_back(mem_t());
  buf_t ct;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, ek, label, xs, ct), SUCCESS);
}

TEST(ApiPveBatchNeg, VerifyBatchInvalidCurve) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, ek, label, xs, ct), SUCCESS);
  const buf_t Q = expected_Q(curve_id::secp256k1, xs[0]);
  std::vector<mem_t> Qs;
  Qs.emplace_back(Q.data(), Q.size());
  dylog_disable_scope_t no_log_err;
  for (int c : {0, 4, 255}) {
    EXPECT_NE(coinbase::api::pve::verify_batch(base_pke, static_cast<curve_id>(c), ek, ct, Qs, label), SUCCESS);
  }
}

TEST(ApiPveBatchNeg, VerifyBatchEmptyEk) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, ek, label, xs, ct), SUCCESS);
  const buf_t Q = expected_Q(curve_id::secp256k1, xs[0]);
  std::vector<mem_t> Qs;
  Qs.emplace_back(Q.data(), Q.size());
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::verify_batch(base_pke, curve_id::secp256k1, mem_t(), ct, Qs, label), SUCCESS);
}

TEST(ApiPveBatchNeg, VerifyBatchEmptyCiphertext) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  const buf_t Q = buf_t("Q");
  std::vector<mem_t> Qs;
  Qs.emplace_back(Q.data(), Q.size());
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::verify_batch(base_pke, curve_id::secp256k1, ek, mem_t(), Qs, label), SUCCESS);
}

TEST(ApiPveBatchNeg, VerifyBatchEmptyQsVector) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, ek, label, xs, ct), SUCCESS);
  std::vector<mem_t> Qs;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::verify_batch(base_pke, curve_id::secp256k1, ek, ct, Qs, label), SUCCESS);
}

TEST(ApiPveBatchNeg, VerifyBatchEmptyLabel) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, ek, label, xs, ct), SUCCESS);
  const buf_t Q = expected_Q(curve_id::secp256k1, xs[0]);
  std::vector<mem_t> Qs;
  Qs.emplace_back(Q.data(), Q.size());
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::verify_batch(base_pke, curve_id::secp256k1, ek, ct, Qs, mem_t()), SUCCESS);
}

TEST(ApiPveBatchNeg, VerifyBatchGarbageCiphertext) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  const buf_t Q = buf_t("Q");
  std::vector<mem_t> Qs;
  Qs.emplace_back(Q.data(), Q.size());
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::verify_batch(base_pke, curve_id::secp256k1, ek, mem_t(garbage, 4), Qs, label), SUCCESS);
}

TEST(ApiPveBatchNeg, DecryptBatchInvalidCurve) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t dk = buf_t("dk");
  const buf_t label = buf_t("label");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, ek, label, xs, ct), SUCCESS);
  std::vector<buf_t> xs_out;
  dylog_disable_scope_t no_log_err;
  for (int c : {0, 4, 255}) {
    EXPECT_NE(coinbase::api::pve::decrypt_batch(base_pke, static_cast<curve_id>(c), dk, ek, ct, label, xs_out),
              SUCCESS);
  }
}

TEST(ApiPveBatchNeg, DecryptBatchEmptyDk) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, ek, label, xs, ct), SUCCESS);
  std::vector<buf_t> xs_out;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::decrypt_batch(base_pke, curve_id::secp256k1, mem_t(), ek, ct, label, xs_out), SUCCESS);
}

TEST(ApiPveBatchNeg, DecryptBatchEmptyEk) {
  const toy_base_pke_t base_pke;
  const buf_t dk = buf_t("dk");
  const buf_t label = buf_t("label");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, buf_t("ek"), label, xs, ct), SUCCESS);
  std::vector<buf_t> xs_out;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::decrypt_batch(base_pke, curve_id::secp256k1, dk, mem_t(), ct, label, xs_out), SUCCESS);
}

TEST(ApiPveBatchNeg, DecryptBatchEmptyCiphertext) {
  const toy_base_pke_t base_pke;
  const buf_t dk = buf_t("dk");
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  std::vector<buf_t> xs_out;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::decrypt_batch(base_pke, curve_id::secp256k1, dk, ek, mem_t(), label, xs_out), SUCCESS);
}

TEST(ApiPveBatchNeg, DecryptBatchEmptyLabel) {
  const toy_base_pke_t base_pke;
  const buf_t ek = buf_t("ek");
  const buf_t dk = buf_t("dk");
  const buf_t label = buf_t("label");
  std::array<uint8_t, 32> x_bytes{};
  x_bytes[0] = 1;
  std::vector<mem_t> xs;
  xs.emplace_back(x_bytes.data(), static_cast<int>(x_bytes.size()));
  buf_t ct;
  ASSERT_EQ(coinbase::api::pve::encrypt_batch(base_pke, curve_id::secp256k1, ek, label, xs, ct), SUCCESS);
  std::vector<buf_t> xs_out;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::decrypt_batch(base_pke, curve_id::secp256k1, dk, ek, ct, mem_t(), xs_out), SUCCESS);
}

TEST(ApiPveBatchNeg, DecryptBatchGarbageCiphertext) {
  const toy_base_pke_t base_pke;
  const buf_t dk = buf_t("dk");
  const buf_t ek = buf_t("ek");
  const buf_t label = buf_t("label");
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  std::vector<buf_t> xs_out;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::decrypt_batch(base_pke, curve_id::secp256k1, dk, ek, mem_t(garbage, 4), label, xs_out),
            SUCCESS);
}

TEST(ApiPveBatchNeg, GetBatchCountEmptyCiphertext) {
  int count = 0;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::get_batch_count(mem_t(), count), SUCCESS);
}

TEST(ApiPveBatchNeg, GetBatchCountGarbageCiphertext) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  int count = 0;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::get_batch_count(mem_t(garbage, 4), count), SUCCESS);
}

TEST(ApiPveBatchNeg, GetPublicKeysCompressedBatchEmptyCiphertext) {
  std::vector<buf_t> Qs;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::get_public_keys_compressed_batch(mem_t(), Qs), SUCCESS);
}

TEST(ApiPveBatchNeg, GetPublicKeysCompressedBatchGarbageCiphertext) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  std::vector<buf_t> Qs;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::get_public_keys_compressed_batch(mem_t(garbage, 4), Qs), SUCCESS);
}

TEST(ApiPveBatchNeg, GetLabelBatchEmptyCiphertext) {
  buf_t label;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::get_Label_batch(mem_t(), label), SUCCESS);
}

TEST(ApiPveBatchNeg, GetLabelBatchGarbageCiphertext) {
  const uint8_t garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
  buf_t label;
  dylog_disable_scope_t no_log_err;
  EXPECT_NE(coinbase::api::pve::get_Label_batch(mem_t(garbage, 4), label), SUCCESS);
}
