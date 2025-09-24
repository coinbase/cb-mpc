#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_pki.h>

namespace coinbase::mpc {

struct pve_base_pke_i {
  virtual ~pve_base_pke_i() = default;
  virtual error_t encrypt(const void* ek, mem_t label, mem_t plain, mem_t rho, buf_t& out_ct) const = 0;
  virtual error_t decrypt(const void* dk, mem_t label, mem_t ct, buf_t& out_plain) const = 0;
};

// Generic adapter that turns any KEM policy into a PVE base PKE via kem_aead_ciphertext_t
template <class KEM_POLICY>
struct kem_pve_base_pke_t : public pve_base_pke_i {
  using EK = typename KEM_POLICY::ek_t;
  using DK = typename KEM_POLICY::dk_t;
  using CT = crypto::kem_aead_ciphertext_t<KEM_POLICY>;

  error_t encrypt(const void* ek, mem_t label, mem_t plain, mem_t rho, buf_t& out_ct) const override {
    crypto::drbg_aes_ctr_t drbg(rho);
    CT ct;
    error_t rv = ct.encrypt(*static_cast<const EK*>(ek), label, plain, &drbg);
    if (rv) return rv;
    out_ct = ser(ct);
    return SUCCESS;
  }

  error_t decrypt(const void* dk, mem_t label, mem_t ct_ser, buf_t& out_plain) const override {
    error_t rv = UNINITIALIZED_ERROR;
    CT ct;
    if (rv = deser(ct_ser, ct)) return rv;
    return ct.decrypt(*static_cast<const DK*>(dk), label, out_plain);
  }
};

template <class KEM_POLICY>
inline const pve_base_pke_i& kem_pve_base_pke() {
  static const kem_pve_base_pke_t<KEM_POLICY> pke;
  return pke;
}

// Accessors to built-in base PKE implementations for testing and convenience
const pve_base_pke_i& pve_base_pke_unified();
const pve_base_pke_i& pve_base_pke_rsa();
const pve_base_pke_i& pve_base_pke_ecies();

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