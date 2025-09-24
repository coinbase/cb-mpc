#pragma once

#include <utility>

#include <cbmpc/crypto/ro.h>

#include "base.h"
#include "base_ecc.h"
#include "base_rsa.h"
#include "pki_ffi.h"

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

  error_t seal(const typename KEM_POLICY::ek_t& pub_key, mem_t aad, mem_t plain, drbg_aes_ctr_t* drbg = nullptr) {
    error_t rv = UNINITIALIZED_ERROR;
    kem_ct = buf_t();
    aead_ciphertext = buf_t();

    buf_t kem_ss;
    if (rv = KEM_POLICY::encapsulate(pub_key, kem_ct, kem_ss, drbg)) return rv;

    buf_t iv_buf = drbg ? drbg->gen(iv_size) : gen_random(iv_size);
    cb_assert(iv_buf.size() == iv_size);
    memmove(iv, iv_buf.data(), iv_size);

    buf_t aes_key = crypto::sha256_t::hash(kem_ss);
    crypto::aes_gcm_t::encrypt(aes_key, mem_t(iv, iv_size), aad, tag_size, plain, aead_ciphertext);
    return SUCCESS;
  }

  error_t open(const typename KEM_POLICY::dk_t& prv_key_handle, mem_t aad, buf_t& plain) const {
    error_t rv = UNINITIALIZED_ERROR;
    buf_t kem_ss;
    if (rv = KEM_POLICY::decapsulate(prv_key_handle, kem_ct, kem_ss)) return rv;
    buf_t aes_key = crypto::sha256_t::hash(kem_ss);
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

  static error_t encapsulate(const ek_t& pub_key, buf_t& kem_ct, buf_t& kem_ss, drbg_aes_ctr_t* drbg) {
    cb_assert(pub_key.get_curve() == curve_p256);
    const mod_t& q = curve_p256.order();
    bn_t e = drbg ? drbg->gen_bn(q) : bn_t::rand(q);
    const auto& G = curve_p256.generator();
    ecc_point_t E = e * G;
    kem_ct = E.to_oct();
    kem_ss = (e * pub_key).get_x().to_bin(32);
    return SUCCESS;
  }

  static error_t decapsulate(const dk_t& prv_key, mem_t kem_ct, buf_t& kem_ss) {
    error_t rv = UNINITIALIZED_ERROR;
    ecc_point_t E;
    if (rv = E.from_oct(curve_p256, kem_ct)) return rv;
    if (rv = curve_p256.check(E)) return rv;
    kem_ss = prv_key.ecdh(E);
    return SUCCESS;
  }
};

// ---------------------------------------------------------------------------
// External KEM types (encapsulate/decapsulate via FFI)
// ---------------------------------------------------------------------------

struct ffi_kem_ek_t : public buf_t {
  using buf_t::buf_t;
  using buf_t::operator=;
};

struct ffi_kem_dk_t {
  void* handle = nullptr;  // Opaque process-local handle to the private key

  ffi_kem_dk_t() = default;
  explicit ffi_kem_dk_t(void* h) : handle(h) {}

  // Derive the public key using user-supplied callback.
  ffi_kem_ek_t pub() const {
    ffi_kem_dk_to_ek_fn derive_fn = get_ffi_kem_dk_to_ek_fn();
    cb_assert(derive_fn && "ffi_kem_dk_to_ek_fn not set");

    cmem_t out;
    int rc = derive_fn(static_cast<const void*>(handle), &out);
    cb_assert(rc == 0 && "ffi_kem_dk_to_ek_fn failed");

    ffi_kem_ek_t ek;
    ek = buf_t::from_cmem(out);
    return ek;
  }
};

// Opaque container for the KEM ciphertext produced by the external PKI.
struct ffi_kem_ct_t : public buf_t {
  using buf_t::operator=;
  using buf_t::buf_t;
};

// Policy adapter that uses the external KEM FFI:
// - encapsulate: produce (kem_ct, kem_ss)
// - decapsulate: recover kem_ss from kem_ct
struct kem_policy_ffi_t {
  using ek_t = ffi_kem_ek_t;
  using dk_t = ffi_kem_dk_t;

  static error_t encapsulate(const ek_t& pub_key, buf_t& kem_ct, buf_t& kem_ss, drbg_aes_ctr_t* drbg) {
    ffi_kem_encap_fn enc_fn = get_ffi_kem_encap_fn();
    if (!enc_fn) return E_BADARG;
    constexpr int rho_size = 32;
    buf_t rho = drbg ? drbg->gen(rho_size) : gen_random(rho_size);
    cmem_t ct_out;
    cmem_t ss_out;
    int rc = enc_fn(cmem_t{pub_key.data(), pub_key.size()}, cmem_t{rho.data(), rho.size()}, &ct_out, &ss_out);
    if (rc) return E_CRYPTO;
    kem_ct = buf_t::from_cmem(ct_out);
    kem_ss = buf_t::from_cmem(ss_out);
    return SUCCESS;
  }

  static error_t decapsulate(const dk_t& prv_key, mem_t kem_ct, buf_t& kem_ss) {
    ffi_kem_decap_fn dec_fn = get_ffi_kem_decap_fn();
    if (!dec_fn) return E_BADARG;
    cmem_t ss_out;
    int rc = dec_fn(static_cast<const void*>(prv_key.handle), kem_ct, &ss_out);
    if (rc) return E_CRYPTO;
    kem_ss = buf_t::from_cmem(ss_out);
    return SUCCESS;
  }
};

// ---------------------------------------------------------------------------
// External Signing types
// ---------------------------------------------------------------------------

struct ffi_sign_sk_t : public buf_t {
  using buf_t::buf_t;
  using buf_t::operator=;

  ffi_sign_sk_t(const buf_t& other) : buf_t(other) {}
  ffi_sign_sk_t(buf_t&& other) : buf_t(std::move(other)) {}

  buf_t sign(mem_t hash) const {
    ffi_sign_fn sign_fn = get_ffi_sign_fn();
    if (!sign_fn) return buf_t();
    cmem_t out;
    int rc = sign_fn(cmem_t{this->data(), this->size()}, cmem_t{hash.data, hash.size}, &out);
    if (rc) {
      return buf_t();
    }
    buf_t sig = buf_t::from_cmem(out);
    return sig;
  }
};

struct ffi_sign_vk_t : public buf_t {
  using buf_t::buf_t;
  using buf_t::operator=;

  // Allow construction from a signing key (they share format here)
  ffi_sign_vk_t(const ffi_sign_sk_t& sk) : buf_t(sk) {}

  error_t verify(mem_t hash, mem_t signature) const {
    ffi_verify_fn verify_fn = get_ffi_verify_fn();
    if (!verify_fn) return E_BADARG;
    int rc = verify_fn(cmem_t{this->data(), this->size()}, cmem_t{hash.data, hash.size},
                       cmem_t{signature.data, signature.size});
    if (rc) return E_CRYPTO;
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
using ffi_pke_t = hybrid_pke_t<ffi_kem_ek_t, ffi_kem_dk_t, kem_aead_ciphertext_t<kem_policy_ffi_t>>;
using unified_pke_t = hybrid_pke_t<pub_key_t, prv_key_t, ciphertext_t>;

template <class SK_T, class VK_T>
struct sign_scheme_t {
  using dk_t = SK_T;
  using vk_t = VK_T;
};

using ffi_sign_scheme_t = sign_scheme_t<ffi_sign_sk_t, ffi_sign_vk_t>;
using ecc_sign_scheme_t = sign_scheme_t<ecc_prv_key_t, ecc_pub_key_t>;

}  // namespace coinbase::crypto
