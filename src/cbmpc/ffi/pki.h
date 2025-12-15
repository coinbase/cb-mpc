#pragma once

#include <cbmpc/crypto/base_pki.h>
#include <cbmpc/crypto/pki_ffi.h>

#include "cmem_adapter.h"

namespace coinbase::crypto {

// External KEM types (encapsulate/decapsulate via FFI)
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

    cmem_t out{};
    int rc = derive_fn(static_cast<const void*>(handle), &out);
    cb_assert(rc == 0 && "ffi_kem_dk_to_ek_fn failed");

    ffi_kem_ek_t ek;
    ek = ffi::copy_from_cmem_and_free(out);
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
    cmem_t ct_out{};
    cmem_t ss_out{};
    int rc = enc_fn(cmem_t{pub_key.data(), pub_key.size()}, cmem_t{rho.data(), rho.size()}, &ct_out, &ss_out);
    if (rc) return E_CRYPTO;
    kem_ct = ffi::copy_from_cmem_and_free(ct_out);
    kem_ss = ffi::copy_from_cmem_and_free(ss_out);
    return SUCCESS;
  }

  static error_t decapsulate(const dk_t& prv_key, mem_t kem_ct, buf_t& kem_ss) {
    ffi_kem_decap_fn dec_fn = get_ffi_kem_decap_fn();
    if (!dec_fn) return E_BADARG;
    cmem_t ss_out{};
    cmem_t kem_ct_c = cmem_t{kem_ct.data, kem_ct.size};
    int rc = dec_fn(static_cast<const void*>(prv_key.handle), kem_ct_c, &ss_out);
    if (rc) return E_CRYPTO;
    kem_ss = ffi::copy_from_cmem_and_free(ss_out);
    return SUCCESS;
  }
};

// External Signing types
struct ffi_sign_sk_t : public buf_t {
  using buf_t::buf_t;
  using buf_t::operator=;

  ffi_sign_sk_t(const buf_t& other) : buf_t(other) {}
  ffi_sign_sk_t(buf_t&& other) : buf_t(std::move(other)) {}

  buf_t sign(mem_t hash) const {
    ffi_sign_fn sign_fn = get_ffi_sign_fn();
    if (!sign_fn) return buf_t();
    cmem_t out{};
    int rc = sign_fn(cmem_t{this->data(), this->size()}, cmem_t{hash.data, hash.size}, &out);
    if (rc) return buf_t();
    return ffi::copy_from_cmem_and_free(out);
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

using ffi_pke_t = hybrid_pke_t<ffi_kem_ek_t, ffi_kem_dk_t, kem_aead_ciphertext_t<kem_policy_ffi_t>>;
using ffi_sign_scheme_t = sign_scheme_t<ffi_sign_sk_t, ffi_sign_vk_t>;

}  // namespace coinbase::crypto
