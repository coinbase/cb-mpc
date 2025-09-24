#include "base_pki.h"

#include "pki_ffi.h"

// ---------------------------------------------------------------------------
// Weak stubs for callback getters so core C++ unit/integration tests can link
// without any language-specific FFI layer (Go, Python, Rust, …). When an FFI
// layer *is* linked, its strong definitions override these stubs, so no
// duplicate-symbol clashes occur.
// ---------------------------------------------------------------------------

extern "C" {

__attribute__((weak)) ffi_verify_fn get_ffi_verify_fn(void) { return nullptr; }
__attribute__((weak)) ffi_sign_fn get_ffi_sign_fn(void) { return nullptr; }

__attribute__((weak)) ffi_kem_encap_fn get_ffi_kem_encap_fn(void) { return nullptr; }
__attribute__((weak)) ffi_kem_decap_fn get_ffi_kem_decap_fn(void) { return nullptr; }
__attribute__((weak)) ffi_kem_dk_to_ek_fn get_ffi_kem_dk_to_ek_fn(void) { return nullptr; }

}  // extern "C"

namespace coinbase::crypto {

// For unified PKE types

pub_key_t pub_key_t::from(const rsa_pub_key_t& src) {
  pub_key_t out;
  out.rsa_key = src;
  out.key_type = key_type_e::RSA;
  return out;
}

pub_key_t pub_key_t::from(const ecc_pub_key_t& src) {
  pub_key_t out;
  out.ecc_key = src;
  out.key_type = key_type_e::ECC;
  return out;
}

prv_key_t prv_key_t::from(const rsa_prv_key_t& src) {
  prv_key_t out;
  out.rsa_key = src;
  out.key_type = key_type_e::RSA;
  return out;
}

prv_key_t prv_key_t::from(const ecc_prv_key_t& src) {
  prv_key_t out;
  out.ecc_key = src;
  out.key_type = key_type_e::ECC;
  return out;
}

pub_key_t prv_key_t::pub() const {
  if (key_type == key_type_e::ECC)
    return pub_key_t::from(ecc_key.pub());
  else if (key_type == key_type_e::RSA)
    return pub_key_t::from(rsa_key.pub());
  cb_assert(false && "Invalid key type");
  return pub_key_t();
}

error_t prv_key_t::execute(mem_t enc_info, buf_t& dec_info) const {
  if (key_type == key_type_e::ECC) {
    return ecc_key.execute(enc_info, dec_info);
  } else if (key_type == key_type_e::RSA)
    return rsa_key.execute(enc_info, dec_info);
  else
    return coinbase::error(E_BADARG, "Invalid key type");
}

// ------------------------- PKI ciphertext --------------------
error_t ciphertext_t::encrypt(const pub_key_t& pub_key, mem_t label, mem_t plain, drbg_aes_ctr_t* drbg) {
  key_type = pub_key.get_type();
  if (key_type == key_type_e::ECC) {
    return ecies.encrypt(pub_key.ecc(), label, plain, drbg);
  } else if (key_type == key_type_e::RSA) {
    return rsa_kem.encrypt(pub_key.rsa(), label, plain, drbg);
  } else {
    return coinbase::error(E_BADARG, "Invalid key type to encrypt");
  }
}

error_t ciphertext_t::decrypt(const prv_key_t& prv_key, mem_t label, buf_t& plain) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (prv_key.get_type() != key_type) return coinbase::error(E_BADARG, "Key type and ciphertext mismatch");
  if (key_type == key_type_e::ECC) {
    return ecies.decrypt(prv_key.ecc(), label, plain);
  } else if (key_type == key_type_e::RSA) {
    return rsa_kem.decrypt(prv_key.rsa(), label, plain);
  }
  return coinbase::error(E_BADARG);
}

}  // namespace coinbase::crypto
