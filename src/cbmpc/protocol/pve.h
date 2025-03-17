#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_pki.h>
#include <cbmpc/crypto/lagrange.h>
#include <cbmpc/crypto/tdh2.h>
#include <cbmpc/zk/zk_ec.h>

namespace coinbase::mpc {

/**
 * @notes:
 * - This is the underlying encryption used in PVE
 */
template <class PKI_T>
buf_t pve_base_encrypt(const typename PKI_T::ek_t &pub_key, mem_t label, const buf_t &plaintext, mem_t rho) {
  crypto::drbg_aes_ctr_t drbg(rho);
  typename PKI_T::ct_t pki;
  pki.encrypt(pub_key, label, plaintext, &drbg);
  return coinbase::convert(pki);
}

/**
 * @notes:
 * - This is the underlying decryption used in PVE
 */
template <class PKI_T>
error_t pve_base_decrypt(const typename PKI_T::dk_t &prv_key, mem_t label, mem_t ciphertext, buf_t &plain) {
  error_t rv = UNINITIALIZED_ERROR;
  typename PKI_T::ct_t pki;
  if (rv = coinbase::convert(pki, ciphertext)) return rv;
  if (rv = pki.decrypt(prv_key, label, plain)) return rv;
  return SUCCESS;
}

template <class PKI_T = crypto::hybrid_cipher_t>
class ec_pve_t {
  using PK_T = typename PKI_T::ek_t;
  using SK_T = typename PKI_T::dk_t;
  using CT_T = typename PKI_T::ct_t;

 public:
  ec_pve_t() {}
  const static int kappa = SEC_P_COM;
  const static int rho_size = 32;

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vencrypt-1P
   */
  void encrypt(const PK_T &key, mem_t label, ecurve_t curve, const bn_t &x);

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vverify-1P
   */
  error_t verify(const PK_T &key, const ecc_point_t &Q, mem_t label) const;

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vdecrypt-1P
   */
  error_t decrypt(const SK_T &key, mem_t label, ecurve_t curve, bn_t &x, bool skip_verify = false) const;

  const ecc_point_t &get_Q() const { return Q; }

  const buf_t &get_Label() const { return L; }

  void convert(coinbase::converter_t &converter) {
    converter.convert(Q, L, b);

    for (int i = 0; i < kappa; i++) {
      converter.convert(x[i]);
      converter.convert(r[i]);
      converter.convert(c[i]);
    }
  }

 private:
  buf_t L;
  ecc_point_t Q;
  buf128_t b;

  bn_t x[kappa];
  buf128_t r[kappa];
  buf_t c[kappa];

  error_t restore_from_decrypted(int row_index, mem_t decrypted_x_buf, ecurve_t curve, bn_t &x_value) const;
};

template <class PKI_T>
class ec_pve_batch_t {
  using PK_T = typename PKI_T::ek_t;
  using SK_T = typename PKI_T::dk_t;
  using CT_T = typename PKI_T::ct_t;

 public:
  ec_pve_batch_t(int batch_count) : n(batch_count), rows(kappa), Q(n) {}

  const static int kappa = SEC_P_COM;
  // We assume the base encryption scheme requires 32 bytes of randomness. If it needs more, it can be changed to use
  // DRBG with 32 bytes of randomness as the seed.
  const static int rho_size = 32;

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vencrypt-batch-1P
   */
  void encrypt(const PK_T &key, mem_t label, ecurve_t curve, const std::vector<bn_t> &x);

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vverify-batch-1P
   */
  error_t verify(const PK_T &key, const std::vector<ecc_point_t> &Q, mem_t label) const;

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vdecrypt-batch-1P
   */
  error_t decrypt(const SK_T &key, mem_t label, ecurve_t curve, std::vector<bn_t> &x, bool skip_verify = false) const;

  void convert(coinbase::converter_t &converter) {
    if (int(Q.size()) != n) {
      converter.set_error();
      return;
    }

    converter.convert(Q, L, b);

    for (int i = 0; i < kappa; i++) {
      converter.convert(rows[i].x_bin);
      converter.convert(rows[i].r);
      converter.convert(rows[i].c);
    }
  }

 private:
  int n;

  buf_t L;
  std::vector<ecc_point_t> Q;
  buf128_t b;

  struct row_t {
    buf_t x_bin;
    buf_t r;
    buf_t c;
  };
  std::vector<row_t> rows;

  error_t restore_from_decrypted(int row_index, mem_t decrypted_x_buf, ecurve_t curve, std::vector<bn_t> &xs) const;
};

}  // namespace coinbase::mpc
