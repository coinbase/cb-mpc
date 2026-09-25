#include <array>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/param_build.h>
#include <openssl/params.h>

#include <cbmpc/api/pve_base_pke.h>
#include <cbmpc/api/pve_batch_ac.h>
#include <cbmpc/api/pve_batch_single_recipient.h>
#include <cbmpc/core/buf.h>

#include "recovery_example.h"

using namespace coinbase;

namespace {

// Custom-backend example: delegate to built-in encryption.
// PVE requires deterministic encryption given rho; do not sample new randomness here.
class application_base_pke_t final : public coinbase::api::pve::base_pke_i {
 public:
  error_t encrypt(mem_t ek, mem_t label, mem_t plain, mem_t rho, buf_t& out) const override {
    return coinbase::api::pve::base_pke_default().encrypt(ek, label, plain, rho, out);
  }
  error_t decrypt(mem_t dk, mem_t label, mem_t ct, buf_t& out) const override {
    return coinbase::api::pve::base_pke_default().decrypt(dk, label, ct, out);
  }
};

void demo_default_base_pke_rsa() {
  std::cout << "\n=== PVE (api) + built-in RSA key blob ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-demo-label");

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0xC0 + i);
  const mem_t x(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ek_blob;
  buf_t dk_blob;
  cb_assert(coinbase::api::pve::generate_base_pke_rsa_keypair(ek_blob, dk_blob) == SUCCESS);

  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt(curve, ek_blob, label, x, ct) == SUCCESS);

  buf_t Q;
  cb_assert(coinbase::api::pve::get_public_key_compressed(ct, Q) == SUCCESS);

  buf_t label_extracted;
  cb_assert(coinbase::api::pve::get_Label(ct, label_extracted) == SUCCESS);
  std::cout << "label extracted matches? " << (label_extracted == buf_t(label)) << "\n";

  cb_assert(coinbase::api::pve::verify(curve, ek_blob, ct, Q, label) == SUCCESS);

  buf_t x_out;
  cb_assert(coinbase::api::pve::decrypt(curve, dk_blob, ek_blob, ct, label, x_out) == SUCCESS);
  std::cout << "decrypt ok? " << (x_out == buf_t(x)) << "\n";
}

void demo_default_base_pke_ecies() {
  std::cout << "\n=== PVE (api) + built-in ECIES(P-256) key blob ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-demo-label");

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x44 + i);
  const mem_t x(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ek_blob;
  buf_t dk_blob;
  cb_assert(coinbase::api::pve::generate_base_pke_ecies_p256_keypair(ek_blob, dk_blob) == SUCCESS);

  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt(curve, ek_blob, label, x, ct) == SUCCESS);

  buf_t Q;
  cb_assert(coinbase::api::pve::get_public_key_compressed(ct, Q) == SUCCESS);
  cb_assert(coinbase::api::pve::verify(curve, ek_blob, ct, Q, label) == SUCCESS);

  buf_t x_out;
  cb_assert(coinbase::api::pve::decrypt(curve, dk_blob, ek_blob, ct, label, x_out) == SUCCESS);
  std::cout << "decrypt ok? " << (x_out == buf_t(x)) << "\n";
}

struct ec_group_deleter_t {
  void operator()(EC_GROUP* g) const { EC_GROUP_free(g); }
};
struct ec_point_deleter_t {
  void operator()(EC_POINT* p) const { EC_POINT_free(p); }
};
struct ossl_param_bld_deleter_t {
  void operator()(OSSL_PARAM_BLD* bld) const { OSSL_PARAM_BLD_free(bld); }
};
struct ossl_param_deleter_t {
  void operator()(OSSL_PARAM* p) const { OSSL_PARAM_free(p); }
};
struct evp_pkey_ctx_deleter_t {
  void operator()(EVP_PKEY_CTX* ctx) const { EVP_PKEY_CTX_free(ctx); }
};
struct evp_pkey_deleter_t {
  void operator()(EVP_PKEY* pkey) const { EVP_PKEY_free(pkey); }
};

using evp_pkey_ctx_ptr_t = std::unique_ptr<EVP_PKEY_CTX, evp_pkey_ctx_deleter_t>;
using evp_pkey_ptr_t = std::unique_ptr<EVP_PKEY, evp_pkey_deleter_t>;

static error_t ensure_p256_pubkey_oct_uncompressed(mem_t pub_oct_any, buf_t& out_pub_oct_uncompressed) {
  if (pub_oct_any.size == 65 && pub_oct_any.data && pub_oct_any.data[0] == 0x04) {
    out_pub_oct_uncompressed = buf_t(pub_oct_any.data, pub_oct_any.size);
    return SUCCESS;
  }

  std::unique_ptr<EC_GROUP, ec_group_deleter_t> group(EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1));
  if (!group) return coinbase::error(E_INSUFFICIENT, "EC_GROUP_new_by_curve_name failed");

  std::unique_ptr<EC_POINT, ec_point_deleter_t> pt(EC_POINT_new(group.get()));
  if (!pt) return coinbase::error(E_INSUFFICIENT, "EC_POINT_new failed");

  if (EC_POINT_oct2point(group.get(), pt.get(), pub_oct_any.data, static_cast<size_t>(pub_oct_any.size), /*ctx=*/nullptr) != 1) {
    return coinbase::error(E_FORMAT, "invalid EC(P-256) public key encoding");
  }

  uint8_t oct[65];
  const size_t written =
      EC_POINT_point2oct(group.get(), pt.get(), POINT_CONVERSION_UNCOMPRESSED, oct, sizeof(oct), /*ctx=*/nullptr);
  if (written != sizeof(oct)) return coinbase::error(E_GENERAL, "unexpected P-256 public key size");

  out_pub_oct_uncompressed = buf_t(oct, static_cast<int>(written));
  return SUCCESS;
}

// Demo-only "HSM" that holds EC(P-256) private keys and can perform ECDH.
class fake_hsm_ecies_p256_t {
 public:
  error_t generate_key(std::string handle, buf_t& out_pub_key_oct_uncompressed) {
    if (keys_.find(handle) != keys_.end()) return coinbase::error(E_BADARG, "duplicate HSM key handle");

    evp_pkey_ctx_ptr_t keygen_ctx(EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr));
    if (!keygen_ctx) return coinbase::error(E_INSUFFICIENT, "EVP_PKEY_CTX_new_from_name(EC) failed");
    if (EVP_PKEY_keygen_init(keygen_ctx.get()) <= 0) return coinbase::error(E_GENERAL, "EVP_PKEY_keygen_init failed");

    std::unique_ptr<OSSL_PARAM_BLD, ossl_param_bld_deleter_t> bld(OSSL_PARAM_BLD_new());
    if (!bld) return coinbase::error(E_INSUFFICIENT, "OSSL_PARAM_BLD_new failed");
    if (OSSL_PARAM_BLD_push_utf8_string(bld.get(), "group", SN_X9_62_prime256v1, 0) <= 0) {
      return coinbase::error(E_GENERAL, "OSSL_PARAM_BLD_push_utf8_string(group) failed");
    }
    std::unique_ptr<OSSL_PARAM, ossl_param_deleter_t> params(OSSL_PARAM_BLD_to_param(bld.get()));
    if (!params) return coinbase::error(E_INSUFFICIENT, "OSSL_PARAM_BLD_to_param failed");
    if (EVP_PKEY_CTX_set_params(keygen_ctx.get(), params.get()) <= 0) {
      return coinbase::error(E_GENERAL, "EVP_PKEY_CTX_set_params(group) failed");
    }

    EVP_PKEY* key_raw = nullptr;
    if (EVP_PKEY_keygen(keygen_ctx.get(), &key_raw) <= 0 || !key_raw) {
      return coinbase::error(E_GENERAL, "EVP_PKEY_keygen failed");
    }
    evp_pkey_ptr_t key(key_raw);

    // Extract public key octets from the provider key and ensure uncompressed
    // format (65 bytes, 0x04 || X || Y).
    size_t pub_len = 0;
    if (EVP_PKEY_get_octet_string_param(key.get(), "pub", /*out=*/nullptr, /*outlen=*/0, &pub_len) <= 0 || pub_len == 0) {
      return coinbase::error(E_GENERAL, "EVP_PKEY_get_octet_string_param(pub) failed (size)");
    }
    std::vector<uint8_t> pub(pub_len);
    if (EVP_PKEY_get_octet_string_param(key.get(), "pub", pub.data(), pub.size(), &pub_len) <= 0) {
      return coinbase::error(E_GENERAL, "EVP_PKEY_get_octet_string_param(pub) failed");
    }
    pub.resize(pub_len);
    error_t rv = ensure_p256_pubkey_oct_uncompressed(mem_t(pub.data(), static_cast<int>(pub.size())), out_pub_key_oct_uncompressed);
    if (rv) return rv;

    keys_.emplace(std::move(handle), std::move(key));
    return SUCCESS;
  }

  error_t ecdh_x32(mem_t handle, mem_t peer_pub_oct_uncompressed, buf_t& out_dh_x32) const {
    const std::string h(reinterpret_cast<const char*>(handle.data), static_cast<size_t>(handle.size));
    const auto it = keys_.find(h);
    if (it == keys_.end()) return coinbase::error(E_BADARG, "unknown HSM key handle");

    if (peer_pub_oct_uncompressed.size != 65 || !peer_pub_oct_uncompressed.data || peer_pub_oct_uncompressed.data[0] != 0x04) {
      return coinbase::error(E_FORMAT, "peer public key must be uncompressed P-256 octets");
    }

    // Import peer public key as EVP_PKEY (provider-based).
    std::unique_ptr<OSSL_PARAM_BLD, ossl_param_bld_deleter_t> peer_bld(OSSL_PARAM_BLD_new());
    if (!peer_bld) return coinbase::error(E_INSUFFICIENT, "OSSL_PARAM_BLD_new failed");
    if (OSSL_PARAM_BLD_push_utf8_string(peer_bld.get(), "group", SN_X9_62_prime256v1, 0) <= 0) {
      return coinbase::error(E_GENERAL, "OSSL_PARAM_BLD_push_utf8_string(group) failed");
    }
    if (OSSL_PARAM_BLD_push_octet_string(peer_bld.get(), "pub", peer_pub_oct_uncompressed.data,
                                         static_cast<size_t>(peer_pub_oct_uncompressed.size)) <= 0) {
      return coinbase::error(E_GENERAL, "OSSL_PARAM_BLD_push_octet_string(pub) failed");
    }
    std::unique_ptr<OSSL_PARAM, ossl_param_deleter_t> peer_params(OSSL_PARAM_BLD_to_param(peer_bld.get()));
    if (!peer_params) return coinbase::error(E_INSUFFICIENT, "OSSL_PARAM_BLD_to_param failed");

    evp_pkey_ctx_ptr_t peer_fromdata_ctx(EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr));
    if (!peer_fromdata_ctx) return coinbase::error(E_INSUFFICIENT, "EVP_PKEY_CTX_new_from_name(EC) failed");
    if (EVP_PKEY_fromdata_init(peer_fromdata_ctx.get()) <= 0) return coinbase::error(E_GENERAL, "EVP_PKEY_fromdata_init failed");
    EVP_PKEY* peer_key_raw = nullptr;
    if (EVP_PKEY_fromdata(peer_fromdata_ctx.get(), &peer_key_raw, EVP_PKEY_PUBLIC_KEY, peer_params.get()) <= 0 || !peer_key_raw) {
      return coinbase::error(E_FORMAT, "EVP_PKEY_fromdata(peer pub) failed");
    }
    evp_pkey_ptr_t peer_key(peer_key_raw);

    EVP_PKEY* key = it->second.get();
    evp_pkey_ctx_ptr_t derive_ctx(EVP_PKEY_CTX_new(key, nullptr));
    if (!derive_ctx) return coinbase::error(E_INSUFFICIENT, "EVP_PKEY_CTX_new failed");
    if (EVP_PKEY_derive_init(derive_ctx.get()) <= 0) return coinbase::error(E_GENERAL, "EVP_PKEY_derive_init failed");
    if (EVP_PKEY_derive_set_peer(derive_ctx.get(), peer_key.get()) <= 0) {
      return coinbase::error(E_GENERAL, "EVP_PKEY_derive_set_peer failed");
    }

    size_t dh_len = 0;
    if (EVP_PKEY_derive(derive_ctx.get(), /*key=*/nullptr, &dh_len) <= 0 || dh_len == 0) {
      return coinbase::error(E_GENERAL, "EVP_PKEY_derive(size) failed");
    }
    std::vector<uint8_t> dh(dh_len);
    if (EVP_PKEY_derive(derive_ctx.get(), dh.data(), &dh_len) <= 0) {
      return coinbase::error(E_GENERAL, "EVP_PKEY_derive failed");
    }
    dh.resize(dh_len);
    if (dh.size() > 32) return coinbase::error(E_GENERAL, "unexpected ECDH output length");

    std::array<uint8_t, 32> x32{};
    std::memmove(x32.data() + (32 - dh.size()), dh.data(), dh.size());
    out_dh_x32 = buf_t(x32.data(), static_cast<int>(x32.size()));
    return SUCCESS;
  }

 private:
  std::map<std::string, evp_pkey_ptr_t> keys_;
};

static error_t ecies_p256_hsm_ecdh_cb(void* ctx, mem_t dk_handle, mem_t kem_ct, buf_t& out_dh_x32) {
  if (!ctx) return coinbase::error(E_BADARG, "missing HSM ctx");
  return static_cast<const fake_hsm_ecies_p256_t*>(ctx)->ecdh_x32(dk_handle, kem_ct, out_dh_x32);
}

void demo_hsm_ecies_p256() {
  std::cout << "\n=== PVE (api) + ECIES(P-256) simulated HSM (ECDH callback only) ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-demo-label");

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x88 + i);
  const mem_t x(x_bytes.data(), static_cast<int>(x_bytes.size()));

  // 1) Simulate an HSM that generates/stores the private key and exports only the public key.
  fake_hsm_ecies_p256_t hsm;
  const std::string hsm_handle = "hsm-ecies-p256-key-1";
  buf_t pub_key_oct_uncompressed;
  cb_assert(hsm.generate_key(hsm_handle, pub_key_oct_uncompressed) == SUCCESS);

  // 2) Wrap the exported public key into cbmpc's opaque base-PKE ek blob.
  buf_t ek_blob;
  cb_assert(coinbase::api::pve::base_pke_ecies_p256_ek_from_oct(pub_key_oct_uncompressed, ek_blob) == SUCCESS);

  // 3) Encrypt and verify using the software public key blob.
  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt(curve, ek_blob, label, x, ct) == SUCCESS);

  buf_t Q;
  cb_assert(coinbase::api::pve::get_public_key_compressed(ct, Q) == SUCCESS);
  cb_assert(coinbase::api::pve::verify(curve, ek_blob, ct, Q, label) == SUCCESS);

  // 4) Decrypt using the HSM callback for *only* the ECDH step.
  coinbase::api::pve::ecies_p256_hsm_ecdh_cb_t cb;
  cb.ctx = &hsm;
  cb.ecdh = ecies_p256_hsm_ecdh_cb;

  const mem_t dk_handle(hsm_handle);
  buf_t x_out;
  cb_assert(coinbase::api::pve::decrypt_ecies_p256_hsm(curve, dk_handle, ek_blob, ct, label, cb, x_out) == SUCCESS);
  std::cout << "decrypt ok? " << (x_out == buf_t(x)) << "\n";
}

void demo_custom_base_pke() {
  std::cout << "\n=== PVE (api) + custom base PKE ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-demo-label");

  std::array<uint8_t, 32> x_bytes{};
  for (int i = 0; i < 32; i++) x_bytes[static_cast<size_t>(i)] = static_cast<uint8_t>(0x66 + i);
  const mem_t x(x_bytes.data(), static_cast<int>(x_bytes.size()));

  buf_t ek, dk;
  cb_assert(coinbase::api::pve::generate_base_pke_ecies_p256_keypair(ek, dk) == SUCCESS);

  application_base_pke_t backend;

  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt(backend, curve, ek, label, x, ct) == SUCCESS);

  buf_t Q;
  cb_assert(coinbase::api::pve::get_public_key_compressed(ct, Q) == SUCCESS);
  cb_assert(coinbase::api::pve::verify(backend, curve, ek, ct, Q, label) == SUCCESS);

  buf_t x_out;
  cb_assert(coinbase::api::pve::decrypt(backend, curve, dk, ek, ct, label, x_out) == SUCCESS);
  std::cout << "decrypt ok? " << (x_out == buf_t(x)) << "\n";
}

void demo_ac_default_base_pke_rsa() {
  std::cout << "\n=== PVE-AC (api) + RSA backup keys + recipient-protected recovery ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-ac-demo-label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  constexpr int n = 8;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++) xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(0x10 + i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  std::array<buf_t, 3> eks{};
  std::array<buf_t, 3> dks{};
  cb_assert(coinbase::api::pve::generate_base_pke_rsa_keypair(eks[0], dks[0]) == SUCCESS);
  cb_assert(coinbase::api::pve::generate_base_pke_rsa_keypair(eks[1], dks[1]) == SUCCESS);
  cb_assert(coinbase::api::pve::generate_base_pke_rsa_keypair(eks[2], dks[2]) == SUCCESS);

  coinbase::api::pve::leaf_keys_t ac_pks;
  cb_assert(ac_pks.emplace("p1", mem_t(eks[0].data(), eks[0].size())).second);
  cb_assert(ac_pks.emplace("p2", mem_t(eks[1].data(), eks[1].size())).second);
  cb_assert(ac_pks.emplace("p3", mem_t(eks[2].data(), eks[2].size())).second);

  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt_ac(curve, ac, ac_pks, label, xs, ct) == SUCCESS);

  int batch_count = 0;
  cb_assert(coinbase::api::pve::get_ac_batch_count(ct, batch_count) == SUCCESS);
  std::cout << "batch_count: " << batch_count << "\n";

  pve_demo::demonstrate_protected_recovery(coinbase::api::pve::base_pke_default(), ac, ac_pks, ct, label, xs,
                                         {"p1", "p2"}, {mem_t(dks[0]), mem_t(dks[1])});
}

void demo_ac_default_base_pke_ecies() {
  std::cout << "\n=== PVE-AC (api) + ECIES backup keys + recipient-protected recovery ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-ac-demo-label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  constexpr int n = 8;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++) xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(0x40 + i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  std::array<buf_t, 3> eks{};
  std::array<buf_t, 3> dks{};
  cb_assert(coinbase::api::pve::generate_base_pke_ecies_p256_keypair(eks[0], dks[0]) == SUCCESS);
  cb_assert(coinbase::api::pve::generate_base_pke_ecies_p256_keypair(eks[1], dks[1]) == SUCCESS);
  cb_assert(coinbase::api::pve::generate_base_pke_ecies_p256_keypair(eks[2], dks[2]) == SUCCESS);

  coinbase::api::pve::leaf_keys_t ac_pks;
  cb_assert(ac_pks.emplace("p1", mem_t(eks[0].data(), eks[0].size())).second);
  cb_assert(ac_pks.emplace("p2", mem_t(eks[1].data(), eks[1].size())).second);
  cb_assert(ac_pks.emplace("p3", mem_t(eks[2].data(), eks[2].size())).second);

  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt_ac(curve, ac, ac_pks, label, xs, ct) == SUCCESS);

  pve_demo::demonstrate_protected_recovery(coinbase::api::pve::base_pke_default(), ac, ac_pks, ct, label, xs,
                                         {"p2", "p3"}, {mem_t(dks[1]), mem_t(dks[2])});
}

void demo_ac_custom_base_pke() {
  std::cout << "\n=== PVE-AC (api) + application backup adapter + recipient-protected recovery ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-ac-demo-label");

  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      2, {coinbase::api::access_structure_t::leaf("p1"), coinbase::api::access_structure_t::leaf("p2"),
          coinbase::api::access_structure_t::leaf("p3")});

  constexpr int n = 8;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++) xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(0x70 + i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  // Holder keys produce partial decryptions; separate recipient keys protect their delivery.
  std::array<buf_t, 3> eks, dks;
  for (size_t i = 0; i < eks.size(); ++i) {
    cb_assert(coinbase::api::pve::generate_base_pke_ecies_p256_keypair(eks[i], dks[i]) == SUCCESS);
  }

  coinbase::api::pve::leaf_keys_t ac_pks;
  cb_assert(ac_pks.emplace("p1", mem_t(eks[0])).second);
  cb_assert(ac_pks.emplace("p2", mem_t(eks[1])).second);
  cb_assert(ac_pks.emplace("p3", mem_t(eks[2])).second);

  application_base_pke_t backend;

  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt_ac(backend, curve, ac, ac_pks, label, xs, ct) == SUCCESS);

  pve_demo::demonstrate_protected_recovery(backend, ac, ac_pks, ct, label, xs,
                                         {"p1", "p3"}, {mem_t(dks[0]), mem_t(dks[2])});
}

void demo_batch_default_base_pke_rsa() {
  std::cout << "\n=== PVE Batch (api) + built-in RSA key blob ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-demo-label");

  constexpr int n = 8;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++) xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  buf_t ek_blob;
  buf_t dk_blob;
  cb_assert(coinbase::api::pve::generate_base_pke_rsa_keypair(ek_blob, dk_blob) == SUCCESS);

  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt_batch(curve, ek_blob, label, xs, ct) == SUCCESS);

  int batch_count = 0;
  cb_assert(coinbase::api::pve::get_batch_count(ct, batch_count) == SUCCESS);
  std::cout << "batch_count: " << batch_count << "\n";

  std::vector<buf_t> Qs;
  cb_assert(coinbase::api::pve::get_public_keys_compressed_batch(ct, Qs) == SUCCESS);
  std::vector<mem_t> Qs_mem;
  Qs_mem.reserve(Qs.size());
  for (const auto& q : Qs) Qs_mem.emplace_back(q.data(), q.size());

  buf_t label_extracted;
  cb_assert(coinbase::api::pve::get_Label_batch(ct, label_extracted) == SUCCESS);
  std::cout << "label extracted matches? " << (label_extracted == buf_t(label)) << "\n";

  cb_assert(coinbase::api::pve::verify_batch(curve, ek_blob, ct, Qs_mem, label) == SUCCESS);

  std::vector<buf_t> xs_out;
  cb_assert(coinbase::api::pve::decrypt_batch(curve, dk_blob, ek_blob, ct, label, xs_out) == SUCCESS);

  bool ok = (xs_out.size() == xs.size());
  for (int i = 0; ok && i < n; i++) ok = (xs_out[static_cast<size_t>(i)] == buf_t(xs[static_cast<size_t>(i)]));
  std::cout << "decrypt ok? " << ok << "\n";
}

void demo_batch_default_base_pke_ecies() {
  std::cout << "\n=== PVE Batch (api) + built-in ECIES(P-256) key blob ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-demo-label");

  constexpr int n = 8;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++) xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(0x55 + i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  buf_t ek_blob;
  buf_t dk_blob;
  cb_assert(coinbase::api::pve::generate_base_pke_ecies_p256_keypair(ek_blob, dk_blob) == SUCCESS);

  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt_batch(curve, ek_blob, label, xs, ct) == SUCCESS);

  std::vector<buf_t> Qs;
  cb_assert(coinbase::api::pve::get_public_keys_compressed_batch(ct, Qs) == SUCCESS);
  std::vector<mem_t> Qs_mem;
  Qs_mem.reserve(Qs.size());
  for (const auto& q : Qs) Qs_mem.emplace_back(q.data(), q.size());

  cb_assert(coinbase::api::pve::verify_batch(curve, ek_blob, ct, Qs_mem, label) == SUCCESS);

  std::vector<buf_t> xs_out;
  cb_assert(coinbase::api::pve::decrypt_batch(curve, dk_blob, ek_blob, ct, label, xs_out) == SUCCESS);

  bool ok = (xs_out.size() == xs.size());
  for (int i = 0; ok && i < n; i++) ok = (xs_out[static_cast<size_t>(i)] == buf_t(xs[static_cast<size_t>(i)]));
  std::cout << "decrypt ok? " << ok << "\n";
}

void demo_batch_hsm_ecies_p256() {
  std::cout << "\n=== PVE Batch (api) + ECIES(P-256) simulated HSM (ECDH callback only) ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-demo-label");

  constexpr int n = 8;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++) xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(0x88 + i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  fake_hsm_ecies_p256_t hsm;
  const std::string hsm_handle = "hsm-ecies-p256-key-batch-1";
  buf_t pub_key_oct_uncompressed;
  cb_assert(hsm.generate_key(hsm_handle, pub_key_oct_uncompressed) == SUCCESS);

  buf_t ek_blob;
  cb_assert(coinbase::api::pve::base_pke_ecies_p256_ek_from_oct(pub_key_oct_uncompressed, ek_blob) == SUCCESS);

  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt_batch(curve, ek_blob, label, xs, ct) == SUCCESS);

  std::vector<buf_t> Qs;
  cb_assert(coinbase::api::pve::get_public_keys_compressed_batch(ct, Qs) == SUCCESS);
  std::vector<mem_t> Qs_mem;
  Qs_mem.reserve(Qs.size());
  for (const auto& q : Qs) Qs_mem.emplace_back(q.data(), q.size());

  cb_assert(coinbase::api::pve::verify_batch(curve, ek_blob, ct, Qs_mem, label) == SUCCESS);

  coinbase::api::pve::ecies_p256_hsm_ecdh_cb_t cb;
  cb.ctx = &hsm;
  cb.ecdh = ecies_p256_hsm_ecdh_cb;

  const mem_t dk_handle(hsm_handle);
  std::vector<buf_t> xs_out;
  cb_assert(coinbase::api::pve::decrypt_batch_ecies_p256_hsm(curve, dk_handle, ek_blob, ct, label, cb, xs_out) == SUCCESS);

  bool ok = (xs_out.size() == xs.size());
  for (int i = 0; ok && i < n; i++) ok = (xs_out[static_cast<size_t>(i)] == buf_t(xs[static_cast<size_t>(i)]));
  std::cout << "decrypt ok? " << ok << "\n";
}

void demo_batch_custom_base_pke() {
  std::cout << "\n=== PVE Batch (api) + custom base PKE ===\n";
  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;
  const mem_t label("pve-demo-label");

  constexpr int n = 8;
  std::array<std::array<uint8_t, 32>, n> xs_bytes{};
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++) xs_bytes[static_cast<size_t>(i)][static_cast<size_t>(j)] = static_cast<uint8_t>(0x22 + i + j);
  }
  std::vector<mem_t> xs;
  xs.reserve(n);
  for (int i = 0; i < n; i++) xs.emplace_back(xs_bytes[static_cast<size_t>(i)].data(), 32);

  buf_t ek, dk;
  cb_assert(coinbase::api::pve::generate_base_pke_ecies_p256_keypair(ek, dk) == SUCCESS);

  application_base_pke_t backend;

  buf_t ct;
  cb_assert(coinbase::api::pve::encrypt_batch(backend, curve, ek, label, xs, ct) == SUCCESS);

  std::vector<buf_t> Qs;
  cb_assert(coinbase::api::pve::get_public_keys_compressed_batch(ct, Qs) == SUCCESS);
  std::vector<mem_t> Qs_mem;
  Qs_mem.reserve(Qs.size());
  for (const auto& q : Qs) Qs_mem.emplace_back(q.data(), q.size());

  cb_assert(coinbase::api::pve::verify_batch(backend, curve, ek, ct, Qs_mem, label) == SUCCESS);

  std::vector<buf_t> xs_out;
  cb_assert(coinbase::api::pve::decrypt_batch(backend, curve, dk, ek, ct, label, xs_out) == SUCCESS);

  bool ok = (xs_out.size() == xs.size());
  for (int i = 0; ok && i < n; i++) ok = (xs_out[static_cast<size_t>(i)] == buf_t(xs[static_cast<size_t>(i)]));
  std::cout << "decrypt ok? " << ok << "\n";
}

}  // namespace

int main(int argc, const char* argv[]) {
  std::cout << std::boolalpha;
  std::cout << "================ PVE Demo (api-only) ================\n";

  if (argc == 2 && std::string(argv[1]) == "--ac-only") {
    demo_ac_default_base_pke_rsa();
    demo_ac_default_base_pke_ecies();
    demo_ac_custom_base_pke();
    return 0;
  }

  demo_default_base_pke_rsa();
  demo_default_base_pke_ecies();
  demo_hsm_ecies_p256();
  demo_custom_base_pke();

  demo_ac_default_base_pke_rsa();
  demo_ac_default_base_pke_ecies();
  demo_ac_custom_base_pke();

  demo_batch_default_base_pke_rsa();
  demo_batch_default_base_pke_ecies();
  demo_batch_hsm_ecies_p256();
  demo_batch_custom_base_pke();

  return 0;
}

