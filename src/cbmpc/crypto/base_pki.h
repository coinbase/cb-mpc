#pragma once

#include <cbmpc/crypto/ro.h>

#include "base.h"
#include "base_ecc.h"
#include "base_rsa.h"

namespace coinbase::crypto {

inline mpc_pid_t pid_from_name(const pname_t& name) { return bn_t(ro::hash_string(name).bitlen128()); }

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
  rsa_kem_ciphertext_t rsa_kem;
  coinbase::crypto::ecies_ciphertext_t ecies;

  error_t encrypt(const pub_key_t& pub_key, mem_t label, mem_t plain, drbg_aes_ctr_t* drbg = nullptr);

  error_t decrypt(const prv_key_t& prv_key, mem_t label, buf_t& plain) const;
  error_t decrypt_begin(buf_t& enc_info) const;
  error_t decrypt_end(mem_t label, mem_t dec_info, buf_t& plain) const;
  error_t decrypt_end(mem_t label, mem_t dec_info, bn_t& plain) const {
    buf_t bin;
    error_t rv = decrypt_end(label, dec_info, bin);
    if (rv) return rv;
    plain = bn_t::from_bin(bin);
    return SUCCESS;
  }

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
struct hybrid_cipher_bundle_t {
  using ek_t = EK_T;
  using dk_t = DK_T;
  using ct_t = CT_T;
};

using hybrid_cipher_t = hybrid_cipher_bundle_t<pub_key_t, prv_key_t, ciphertext_t>;
using rsa_kem_t = hybrid_cipher_bundle_t<rsa_pub_key_t, rsa_prv_key_t, rsa_kem_ciphertext_t>;
using ecies_t = hybrid_cipher_bundle_t<ecc_pub_key_t, ecc_prv_key_t, ecies_ciphertext_t>;

template <class SK_T, class VK_T>
struct sign_scheme_bundle_t {
  using dk_t = SK_T;
  using vk_t = VK_T;
};

using ecc_sign_scheme_t = sign_scheme_bundle_t<ecc_prv_key_t, ecc_pub_key_t>;

template <class CIPHER_T, class SIGN_T>
struct pki_bundle_t {
  using cipher_t = CIPHER_T;
  using pub_key_t = typename CIPHER_T::ek_t;
  using prv_key_t = typename CIPHER_T::dk_t;
  using ciphertext_t = typename CIPHER_T::ct_t;

  using sign_scheme_t = SIGN_T;
  using sign_key_t = typename SIGN_T::dk_t;
  using verify_key_t = typename SIGN_T::vk_t;
};

using ecc_pki_t = pki_bundle_t<ecies_t, ecc_sign_scheme_t>;

}  // namespace coinbase::crypto
