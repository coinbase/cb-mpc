#pragma once

#include <utility>

#include <cbmpc/crypto/ro.h>

#include "base.h"
#include "base_ecc.h"
#include "base_rsa.h"

namespace coinbase::crypto {

inline mpc_pid_t pid_from_name(const pname_t& name) { return bn_t(ro::hash_string(name).bitlen128()); }

inline constexpr int KEM_AEAD_IV_SIZE = 12;
inline constexpr int KEM_AEAD_TAG_SIZE = 12;

// -------------------- Generic KEM -> AEAD (AES-GCM) wrapper --------------------
// A policy must define:
//   - using ek_t = <encapsulation public key type>
//   - using dk_t = <decapsulation private key type>
//   - static error_t encapsulate(const ek_t&, buf_t& kem_ct, buf_t& kem_ss, drbg_aes_ctr_t*)
//   - static error_t decapsulate(const dk_t&, mem_t kem_ct, buf_t& kem_ss)
template <class KEM_POLICY>
struct kem_aead_ciphertext_t {
  enum { iv_size = KEM_AEAD_IV_SIZE, tag_size = KEM_AEAD_TAG_SIZE };

  // KEM encapsulation data (e.g., RSA-OAEP ciphertext or ephemeral ECDH point)
  buf_t kem_ct;
  // AEAD nonce/IV for AES-GCM
  uint8_t iv[iv_size];
  // AEAD ciphertext produced by AES-GCM. Includes the authentication tag of size tag_size at the end
  buf_t aead_ciphertext;

  void convert(coinbase::converter_t& c) {
    c.convert(kem_ct);
    c.convert(iv);
    c.convert(aead_ciphertext);
  }

  /**
   * @specs:
   * - basic-primitives-spec | kemdem-seal-1P
   */
  error_t seal(const typename KEM_POLICY::ek_t& pub_key, mem_t aad, mem_t plain, drbg_aes_ctr_t* drbg = nullptr) {
    error_t rv = UNINITIALIZED_ERROR;
    kem_ct = buf_t();
    aead_ciphertext = buf_t();

    buf_t kem_ss;
    if (rv = KEM_POLICY::encapsulate(pub_key, kem_ct, kem_ss, drbg)) return rv;

    buf_t iv_buf = drbg ? drbg->gen(iv_size) : gen_random(iv_size);
    cb_assert(iv_buf.size() == iv_size);
    memmove(iv, iv_buf.data(), iv_size);

    // RFC 5869 HKDF: AES-GCM-256 key derivation from KEM shared secret
    buf_t prk = crypto::hkdf_extract_sha256(mem_t(), kem_ss);
    buf_t aes_key = crypto::hkdf_expand_sha256(prk, mem_t("CBMPC|KEM-AEAD|v1|KDF=HKDF-SHA256|AEAD=AES-GCM-256"), 32);
    crypto::aes_gcm_t::encrypt(aes_key, mem_t(iv, iv_size), aad, tag_size, plain, aead_ciphertext);
    return SUCCESS;
  }

  /**
   * @specs:
   * - basic-primitives-spec | kemdem-open-1P
   */
  error_t open(const typename KEM_POLICY::dk_t& prv_key_handle, mem_t aad, buf_t& plain) const {
    error_t rv = UNINITIALIZED_ERROR;
    buf_t kem_ss;
    if (rv = KEM_POLICY::decapsulate(prv_key_handle, kem_ct, kem_ss)) return rv;
    buf_t prk = crypto::hkdf_extract_sha256(mem_t(), kem_ss);
    buf_t aes_key = crypto::hkdf_expand_sha256(prk, mem_t("CBMPC|KEM-AEAD|v1|KDF=HKDF-SHA256|AEAD=AES-GCM-256"), 32);
    return crypto::aes_gcm_t::decrypt(aes_key, mem_t(iv, iv_size), aad, tag_size, aead_ciphertext, plain);
  }

  error_t encrypt(const typename KEM_POLICY::ek_t& pub_key, mem_t aad, mem_t plain, drbg_aes_ctr_t* drbg = nullptr) {
    return seal(pub_key, aad, plain, drbg);
  }

  error_t decrypt(const typename KEM_POLICY::dk_t& prv_key_handle, mem_t aad, buf_t& plain) const {
    return open(prv_key_handle, aad, plain);
  }
};

struct kem_policy_rsa_oaep_t {
  using ek_t = rsa_pub_key_t;
  using dk_t = rsa_prv_key_t;

  static error_t encapsulate(const ek_t& pub_key, buf_t& kem_ct, buf_t& kem_ss, drbg_aes_ctr_t* drbg) {
    const int sha256_size_bytes = hash_alg_t::get(hash_e::sha256).size;
    kem_ss = drbg ? drbg->gen(sha256_size_bytes) : gen_random(sha256_size_bytes);
    if (drbg) {
      buf_t seed = drbg->gen_bitlen(sha256_size_bytes * 8);
      return pub_key.encrypt_oaep_with_seed(kem_ss, hash_e::sha256, hash_e::sha256, mem_t(), seed, kem_ct);
    }
    return pub_key.encrypt_oaep(kem_ss, hash_e::sha256, hash_e::sha256, mem_t(), kem_ct);
  }

  static error_t decapsulate(const dk_t& prv_key, mem_t kem_ct, buf_t& kem_ss) {
    return rsa_oaep_t(prv_key).execute(hash_e::sha256, hash_e::sha256, mem_t(), kem_ct, kem_ss);
  }
};

struct kem_policy_ecdh_p256_t {
  using ek_t = ecc_pub_key_t;  // must be on curve P-256
  using dk_t = ecc_prv_key_t;

  // RFC 9180 helpers for DHKEM(P-256, HKDF-SHA256)
  static buf_t suite_id_kem() {
    buf_t s;
    s += mem_t("KEM");
    uint8_t kem_id[2] = {0x00, 0x10};  // 0x0010 for P-256 (Table 2)
    s += mem_t(kem_id, 2);
    return s;
  }

  static buf_t labeled_extract(mem_t label, mem_t ikm, mem_t salt = mem_t()) {
    buf_t labeled_ikm;
    labeled_ikm += mem_t("HPKE-v1");
    labeled_ikm += suite_id_kem();
    labeled_ikm += label;
    labeled_ikm += ikm;
    return hkdf_extract_sha256(salt, labeled_ikm);
  }

  static buf_t labeled_expand(mem_t prk, mem_t label, mem_t info, int L) {
    uint8_t L2[2] = {static_cast<uint8_t>((L >> 8) & 0xFF), static_cast<uint8_t>(L & 0xFF)};
    buf_t labeled_info;
    labeled_info += mem_t(L2, 2);
    labeled_info += mem_t("HPKE-v1");
    labeled_info += suite_id_kem();
    labeled_info += label;
    labeled_info += info;
    return hkdf_expand_sha256(prk, labeled_info, L);
  }

  /**
   * @specs:
   * - basic-primitives-spec | ecies-encapsulate-1P
   */
  static error_t encapsulate(const ek_t& pub_key, buf_t& kem_ct, buf_t& kem_ss, drbg_aes_ctr_t* drbg) {
    cb_assert(pub_key.get_curve() == curve_p256);
    const mod_t& q = curve_p256.order();
    bn_t e = drbg ? drbg->gen_bn(q) : bn_t::rand(q);
    const auto& G = curve_p256.generator();
    ecc_point_t E = e * G;
    // enc: uncompressed NIST point (65 bytes for P-256, RFC 9180 §7.1.1)
    buf_t enc = E.to_oct();
    kem_ct = enc;

    // raw ECDH secret: affine X coordinate, 32-byte big-endian
    buf_t dh = (e * pub_key).get_x().to_bin(32);

    // kem_context = enc || pub_key
    buf_t kem_context;
    kem_context += enc;
    kem_context += pub_key.to_oct();

    buf_t eae_prk = labeled_extract(mem_t("eae_prk"), dh, mem_t());
    buf_t shared_secret = labeled_expand(eae_prk, mem_t("shared_secret"), kem_context, 32);
    kem_ss = shared_secret;
    return SUCCESS;
  }

  /**
   * @specs:
   * - basic-primitives-spec | ecies-decapsulate-1P
   */
  static error_t decapsulate(const dk_t& prv_key, mem_t kem_ct, buf_t& kem_ss) {
    error_t rv = UNINITIALIZED_ERROR;
    ecc_point_t E;
    if (rv = E.from_oct(curve_p256, kem_ct)) return rv;
    if (rv = curve_p256.check(E)) return rv;

    buf_t dh = prv_key.ecdh(E);

    // kem_context = enc || pub_key
    ecc_pub_key_t pub_key = prv_key.pub();
    buf_t kem_context;
    kem_context += kem_ct;
    kem_context += pub_key.to_oct();

    buf_t eae_prk = labeled_extract(mem_t("eae_prk"), dh, mem_t());
    buf_t shared_secret = labeled_expand(eae_prk, mem_t("shared_secret"), kem_context, 32);
    kem_ss = shared_secret;
    return SUCCESS;
  }
};

// ---------------------------------------------------------------------------
// C++ native unified PKE types
// ---------------------------------------------------------------------------

class prv_key_t;

typedef uint8_t key_type_t;

enum key_type_e : uint8_t {
  NONE = 0,
  RSA = 1,
  ECC = 2,
};

class pub_key_t {
  friend class prv_key_t;

 public:
  static pub_key_t from(const rsa_pub_key_t& rsa);
  static pub_key_t from(const ecc_pub_key_t& ecc);
  const rsa_pub_key_t& rsa() const { return rsa_key; }
  const ecc_pub_key_t& ecc() const { return ecc_key; }

  key_type_t get_type() const { return key_type; }

  void convert(coinbase::converter_t& c) {
    c.convert(key_type);
    if (key_type == key_type_e::RSA)
      c.convert(rsa_key);
    else if (key_type == key_type_e::ECC)
      c.convert(ecc_key);
    else
      cb_assert(false && "Invalid key type");
  }

  bool operator==(const pub_key_t& val) const {
    if (key_type != val.key_type) return false;

    if (key_type == key_type_e::RSA)
      return rsa() == val.rsa();
    else if (key_type == key_type_e::ECC)
      return ecc() == val.ecc();
    else {
      cb_assert(false && "Invalid key type");
      return false;
    }
  }
  bool operator!=(const pub_key_t& val) const { return !(*this == val); }

 private:
  key_type_t key_type = key_type_e::NONE;
  rsa_pub_key_t rsa_key;
  ecc_pub_key_t ecc_key;
};

class prv_key_t {
 public:
  static prv_key_t from(const rsa_prv_key_t& rsa);
  static prv_key_t from(const ecc_prv_key_t& ecc);
  const rsa_prv_key_t rsa() const { return rsa_key; }
  const ecc_prv_key_t ecc() const { return ecc_key; }

  key_type_t get_type() const { return key_type; }

  pub_key_t pub() const;
  error_t execute(mem_t in, buf_t& out) const;

 private:
  key_type_t key_type = key_type_e::NONE;
  rsa_prv_key_t rsa_key;
  ecc_prv_key_t ecc_key;
};

struct ciphertext_t {
  key_type_t key_type = key_type_e::NONE;
  kem_aead_ciphertext_t<kem_policy_rsa_oaep_t> rsa_kem;
  kem_aead_ciphertext_t<kem_policy_ecdh_p256_t> ecies;

  error_t encrypt(const pub_key_t& pub_key, mem_t label, mem_t plain, drbg_aes_ctr_t* drbg = nullptr);

  error_t decrypt(const prv_key_t& prv_key, mem_t label, buf_t& plain) const;

  void convert(coinbase::converter_t& c) {
    c.convert(key_type);
    if (key_type == key_type_e::RSA)
      c.convert(rsa_kem);
    else if (key_type == key_type_e::ECC)
      c.convert(ecies);
    else
      cb_assert(false && "Invalid key type");
  }
};

template <class EK_T, class DK_T, class CT_T>
struct hybrid_pke_t {
  using ek_t = EK_T;
  using dk_t = DK_T;
  using ct_t = CT_T;
};

using rsa_pke_t = hybrid_pke_t<rsa_pub_key_t, rsa_prv_key_t, kem_aead_ciphertext_t<kem_policy_rsa_oaep_t>>;
using ecies_t = hybrid_pke_t<ecc_pub_key_t, ecc_prv_key_t, kem_aead_ciphertext_t<kem_policy_ecdh_p256_t>>;
using unified_pke_t = hybrid_pke_t<pub_key_t, prv_key_t, ciphertext_t>;

template <class SK_T, class VK_T>
struct sign_scheme_t {
  using dk_t = SK_T;
  using vk_t = VK_T;
};

using ecc_sign_scheme_t = sign_scheme_t<ecc_prv_key_t, ecc_pub_key_t>;

}  // namespace coinbase::crypto
