
#include "base_pki.h"

namespace coinbase::crypto {

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

error_t ciphertext_t::decrypt_begin(buf_t& enc_info) const {
  if (key_type == key_type_e::RSA)
    return rsa_kem.decrypt_begin(enc_info);
  else if (key_type == key_type_e::ECC)
    return ecies.decrypt_begin(enc_info);
  else
    return coinbase::error(E_BADARG, "Invalid key type to decrypt_begin");
  return SUCCESS;
}

error_t ciphertext_t::decrypt_end(mem_t label, mem_t dec_info, buf_t& plain) const {
  if (key_type == key_type_e::ECC)
    return ecies.decrypt_end(label, dec_info, plain);
  else if (key_type == key_type_e::RSA)
    return rsa_kem.decrypt_end(label, dec_info, plain);
  else
    return coinbase::error(E_BADARG, "Invalid key type to decrypt_end");
}

error_t ciphertext_t::decrypt(const prv_key_t& prv_key, mem_t label, buf_t& plain) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (prv_key.get_type() != key_type) return coinbase::error(E_BADARG, "Key type and ciphertext mismatch");
  buf_t enc_info;
  if (rv = decrypt_begin(enc_info)) return rv;
  buf_t dec_info;
  if (rv = prv_key.execute(enc_info, dec_info)) return rv;
  return decrypt_end(label, dec_info, plain);
}

}  // namespace coinbase::crypto
