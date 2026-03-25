#pragma once

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_pki.h>

namespace coinbase::mpc {

namespace detail {
// A lightweight type tag for runtime-checked key type erasure.
//
// We intentionally avoid RTTI (`typeid`) so this works even if RTTI is disabled.
template <class T>
inline const void* pve_type_tag() noexcept {
  static const int kTag = 0;
  return &kTag;
}
}  // namespace detail

struct pve_keyref_t {
  const void* ptr = nullptr;
  const void* tag = nullptr;

  template <class T>
  static pve_keyref_t from(const T& v) noexcept {
    return pve_keyref_t{&v, detail::pve_type_tag<T>()};
  }

  template <class T>
  static pve_keyref_t from_ptr(const T* p) noexcept {
    return pve_keyref_t{p, detail::pve_type_tag<T>()};
  }

  template <class T>
  const T* get() const noexcept {
    if (tag != detail::pve_type_tag<T>()) return nullptr;
    return static_cast<const T*>(ptr);
  }
};

template <class T>
inline pve_keyref_t pve_keyref(const T& v) noexcept {
  return pve_keyref_t::from(v);
}

template <class T>
inline pve_keyref_t pve_keyref(const T* p) noexcept {
  return pve_keyref_t::from_ptr(p);
}

struct pve_base_pke_i {
  virtual ~pve_base_pke_i() = default;
  virtual error_t encrypt(pve_keyref_t ek, mem_t label, mem_t plain, mem_t rho, buf_t& out_ct) const = 0;
  virtual error_t decrypt(pve_keyref_t dk, mem_t label, mem_t ct, buf_t& out_plain) const = 0;
};

// Generic adapter that turns any KEM policy into a PVE base PKE via kem_aead_ciphertext_t
template <class KEM_POLICY>
struct kem_pve_base_pke_t : public pve_base_pke_i {
  using EK = typename KEM_POLICY::ek_t;
  using DK = typename KEM_POLICY::dk_t;
  using CT = crypto::kem_aead_ciphertext_t<KEM_POLICY>;

  error_t encrypt(pve_keyref_t ek, mem_t label, mem_t plain, mem_t rho, buf_t& out_ct) const override {
    const EK* pub_key = ek.get<EK>();
    if (!pub_key) return coinbase::error(E_BADARG, "invalid encryption key");
    crypto::drbg_aes_ctr_t drbg(rho);
    CT ct;
    error_t rv = ct.encrypt(*pub_key, label, plain, &drbg);
    if (rv) return rv;
    out_ct = ser(ct);
    return SUCCESS;
  }

  error_t decrypt(pve_keyref_t dk, mem_t label, mem_t ct_ser, buf_t& out_plain) const override {
    const DK* prv_key = dk.get<DK>();
    if (!prv_key) return coinbase::error(E_BADARG, "invalid decryption key");
    error_t rv = UNINITIALIZED_ERROR;
    CT ct;
    if (rv = deser(ct_ser, ct)) return rv;
    return ct.decrypt(*prv_key, label, out_plain);
  }
};

template <class KEM_POLICY>
inline const pve_base_pke_i& kem_pve_base_pke() {
  static const kem_pve_base_pke_t<KEM_POLICY> pke;
  return pke;
}

// Accessors to built-in base PKE implementations for testing and convenience.
// (RSA-OAEP(2048) and ECIES(P-256), both implemented via kem_aead_ciphertext_t.)
const pve_base_pke_i& pve_base_pke_rsa();
const pve_base_pke_i& pve_base_pke_ecies();

// ---------------------------------------------------------------------------
// Runtime / callback-based KEM adapters (for HSM / FFI / wrappers)
// ---------------------------------------------------------------------------

// Generic runtime KEM callbacks. Intended for wrappers that want to provide a
// custom KEM but still reuse cbmpc's KEM/DEM transform (kem_aead_ciphertext_t).
//
// Requirements:
// - `encap` MUST be deterministic given `rho32`.
// - `decap` MUST return the same shared secret produced by `encap`.
struct pve_runtime_kem_callbacks_t {
  using encap_fn_t = error_t (*)(void* ctx, mem_t ek_bytes, mem_t rho32, buf_t& kem_ct, buf_t& kem_ss);
  using decap_fn_t = error_t (*)(void* ctx, const void* dk_handle, mem_t kem_ct, buf_t& kem_ss);

  void* ctx = nullptr;
  encap_fn_t encap = nullptr;
  decap_fn_t decap = nullptr;
};

struct pve_runtime_kem_ek_t {
  mem_t ek_bytes;
  const pve_runtime_kem_callbacks_t* callbacks = nullptr;
};

struct pve_runtime_kem_dk_t {
  const void* dk_handle = nullptr;
  const pve_runtime_kem_callbacks_t* callbacks = nullptr;
};

struct kem_policy_runtime_kem_t {
  using ek_t = pve_runtime_kem_ek_t;
  using dk_t = pve_runtime_kem_dk_t;

  static error_t encapsulate(const ek_t& pub_key, buf_t& kem_ct, buf_t& kem_ss, crypto::drbg_aes_ctr_t* drbg) {
    if (!pub_key.callbacks || !pub_key.callbacks->encap) return E_BADARG;
    constexpr int rho_size = 32;
    buf_t rho = drbg ? drbg->gen(rho_size) : crypto::gen_random(rho_size);
    return pub_key.callbacks->encap(pub_key.callbacks->ctx, pub_key.ek_bytes, rho, kem_ct, kem_ss);
  }

  static error_t decapsulate(const dk_t& prv_key, mem_t kem_ct, buf_t& kem_ss) {
    if (!prv_key.callbacks || !prv_key.callbacks->decap) return E_BADARG;
    return prv_key.callbacks->decap(prv_key.callbacks->ctx, prv_key.dk_handle, kem_ct, kem_ss);
  }
};

inline const pve_base_pke_i& pve_base_pke_runtime_kem() { return kem_pve_base_pke<kem_policy_runtime_kem_t>(); }

// HSM-backed decapsulation for the built-in RSA-OAEP KEM.
//
// The callback must perform RSA-OAEP decryption (OAEP label is empty per our KEM policy)
// and return the recovered KEM shared secret.
struct pve_rsa_oaep_hsm_dk_t {
  using decap_fn_t = error_t (*)(void* ctx, mem_t dk_handle, mem_t kem_ct, buf_t& kem_ss);

  mem_t dk_handle;
  void* ctx = nullptr;
  decap_fn_t decap = nullptr;
};

struct kem_policy_rsa_oaep_hsm_t {
  using ek_t = crypto::rsa_pub_key_t;
  using dk_t = pve_rsa_oaep_hsm_dk_t;

  static error_t encapsulate(const ek_t& pub_key, buf_t& kem_ct, buf_t& kem_ss, crypto::drbg_aes_ctr_t* drbg) {
    return crypto::kem_policy_rsa_oaep_t::encapsulate(pub_key, kem_ct, kem_ss, drbg);
  }

  static error_t decapsulate(const dk_t& prv_key, mem_t kem_ct, buf_t& kem_ss) {
    if (!prv_key.decap) return E_BADARG;
    error_t rv = prv_key.decap(prv_key.ctx, prv_key.dk_handle, kem_ct, kem_ss);
    if (rv) return rv;

    // Our RSA-OAEP KEM policy uses a 32-byte shared secret (SHA-256 output size).
    const int expected_ss_size = crypto::hash_alg_t::get(crypto::hash_e::sha256).size;
    if (kem_ss.size() != expected_ss_size) return coinbase::error(E_CRYPTO, "invalid RSA KEM output size");

    return SUCCESS;
  }
};

inline const pve_base_pke_i& pve_base_pke_rsa_oaep_hsm() { return kem_pve_base_pke<kem_policy_rsa_oaep_hsm_t>(); }

// HSM-backed decapsulation for the built-in ECIES(P-256) KEM.
//
// The callback only needs to perform the ECDH step and return the raw affine-X
// coordinate as a 32-byte big-endian buffer. The library derives the final KEM
// shared secret per RFC 9180 (DHKEM(P-256, HKDF-SHA256)).
struct pve_ecies_p256_hsm_dk_t {
  using ecdh_fn_t = error_t (*)(void* ctx, mem_t dk_handle, mem_t kem_ct, buf_t& dh_x32);

  mem_t dk_handle;
  void* ctx = nullptr;
  ecdh_fn_t ecdh = nullptr;

  // Uncompressed public key octets (for kem_context = enc || pub_key).
  buf_t pub_key_oct;
};

struct kem_policy_ecdh_p256_hsm_t {
  using ek_t = crypto::ecc_pub_key_t;
  using dk_t = pve_ecies_p256_hsm_dk_t;

  static error_t encapsulate(const ek_t& pub_key, buf_t& kem_ct, buf_t& kem_ss, crypto::drbg_aes_ctr_t* drbg) {
    return crypto::kem_policy_ecdh_p256_t::encapsulate(pub_key, kem_ct, kem_ss, drbg);
  }

  static error_t decapsulate(const dk_t& prv_key, mem_t kem_ct, buf_t& kem_ss) {
    error_t rv = UNINITIALIZED_ERROR;
    if (!prv_key.ecdh) return E_BADARG;
    if (prv_key.pub_key_oct.empty()) return coinbase::error(E_BADARG, "missing ECIES public key");

    crypto::ecc_point_t E;
    if (rv = E.from_oct(crypto::curve_p256, kem_ct)) return rv;
    if (rv = crypto::curve_p256.check(E)) return rv;

    buf_t dh;
    if (rv = prv_key.ecdh(prv_key.ctx, prv_key.dk_handle, kem_ct, dh)) return rv;
    if (dh.size() != 32) return coinbase::error(E_CRYPTO, "invalid ECDH output size");

    // kem_context = enc || pub_key
    buf_t kem_context;
    kem_context += kem_ct;
    kem_context += prv_key.pub_key_oct;

    buf_t eae_prk = crypto::kem_policy_ecdh_p256_t::labeled_extract(mem_t("eae_prk"), dh, mem_t());
    kem_ss = crypto::kem_policy_ecdh_p256_t::labeled_expand(eae_prk, mem_t("shared_secret"), kem_context, 32);
    return SUCCESS;
  }
};

inline const pve_base_pke_i& pve_base_pke_ecies_p256_hsm() { return kem_pve_base_pke<kem_policy_ecdh_p256_hsm_t>(); }

/**
 * @notes:
 * - This is the underlying encryption used in PVE
 */
template <class HPKE_T>
buf_t pve_base_encrypt(const typename HPKE_T::ek_t& pub_key, mem_t label, const buf_t& plaintext, mem_t rho) {
  crypto::drbg_aes_ctr_t drbg(rho);
  typename HPKE_T::ct_t ct;
  ct.encrypt(pub_key, label, plaintext, &drbg);
  return ser(ct);
}

/**
 * @notes:
 * - This is the underlying decryption used in PVE
 */
template <class HPKE_T>
error_t pve_base_decrypt(const typename HPKE_T::dk_t& prv_key, mem_t label, mem_t ciphertext, buf_t& plain) {
  error_t rv = UNINITIALIZED_ERROR;
  typename HPKE_T::ct_t ct;
  if (rv = deser(ciphertext, ct)) return rv;
  if (rv = ct.decrypt(prv_key, label, plain)) return rv;
  return SUCCESS;
}

template <typename T>
static buf_t genPVELabelWithPoint(mem_t label, const T& Q) {
  return buf_t(label) + "-" + strext::to_hex(crypto::sha256_t::hash(Q));
}

}  // namespace coinbase::mpc
