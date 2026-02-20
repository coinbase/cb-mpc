#include <cbmpc/internal/protocol/pve_base.h>

namespace coinbase::mpc {

namespace {

// Generic helper to invoke a specific HPKE-like type from the base_pke.
template <typename EK, typename DK, typename CT>
struct pve_base_pke_impl_t : public pve_base_pke_i {
  error_t encrypt(pve_keyref_t ek, mem_t label, mem_t plain, mem_t rho, buf_t& out_ct) const override {
    const EK* pub_key = ek.get<EK>();
    if (!pub_key) return coinbase::error(E_BADARG, "invalid encryption key");
    crypto::drbg_aes_ctr_t drbg(rho);
    CT ct;
    error_t rv = UNINITIALIZED_ERROR;
    if (rv = ct.encrypt(*pub_key, label, plain, &drbg)) return rv;
    out_ct = ser(ct);
    return SUCCESS;
  }

  error_t decrypt(pve_keyref_t dk, mem_t label, mem_t ct_ser, buf_t& out_plain) const override {
    const DK* prv_key = dk.get<DK>();
    if (!prv_key) return coinbase::error(E_BADARG, "invalid decryption key");
    error_t rv = UNINITIALIZED_ERROR;
    CT ct;
    if (rv = deser(ct_ser, ct)) return rv;
    if (rv = ct.decrypt(*prv_key, label, out_plain)) return rv;
    return SUCCESS;
  }
};

// Built-in base_pke for convenience
const pve_base_pke_impl_t<crypto::rsa_pub_key_t, crypto::rsa_prv_key_t,
                          crypto::kem_aead_ciphertext_t<crypto::kem_policy_rsa_oaep_t>>
    base_pke_rsa;

const pve_base_pke_impl_t<crypto::ecc_pub_key_t, crypto::ecc_prv_key_t,
                          crypto::kem_aead_ciphertext_t<crypto::kem_policy_ecdh_p256_t>>
    base_pke_ecies;

}  // namespace

const pve_base_pke_i& pve_base_pke_rsa() { return base_pke_rsa; }
const pve_base_pke_i& pve_base_pke_ecies() { return base_pke_ecies; }

}  // namespace coinbase::mpc