#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/secret_sharing.h>

#include "pve.h"

namespace coinbase::mpc {

template <class PKI_T = crypto::hybrid_cipher_t>
class ec_pve_ac_t {
 public:
  using PK_T = typename PKI_T::ek_t;
  using SK_T = typename PKI_T::dk_t;
  using CT_T = typename PKI_T::ct_t;

  typedef std::map<std::string, PK_T> pks_t;
  typedef std::map<std::string, SK_T> sks_t;

  static constexpr int kappa = SEC_P_COM;
  static constexpr std::size_t iv_size = crypto::ecies_ciphertext_t::iv_size;
  static constexpr std::size_t tag_size = crypto::ecies_ciphertext_t::tag_size;
  static constexpr std::size_t iv_bitlen = iv_size * 8;

  ec_pve_ac_t() : rows(kappa) {}

  void convert(coinbase::converter_t& converter) {
    converter.convert(Q, L, b);

    for (int i = 0; i < kappa; i++) {
      converter.convert(rows[i].x_bin);
      converter.convert(rows[i].r);
      converter.convert(rows[i].c);
      converter.convert(rows[i].quorum_c);
    }
  }

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vencrypt-batch-many-1P
   */
  void encrypt(const crypto::ss::ac_t& ac, const pks_t& ac_pks, mem_t label, ecurve_t curve,
               const std::vector<bn_t>& x);

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vverify-batch-many-1P
   */
  error_t verify(const crypto::ss::ac_t& ac, const pks_t& ac_pks, const std::vector<ecc_point_t>& Q, mem_t label) const;

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vdecrypt-batch-many-1P
   */
  error_t decrypt(const crypto::ss::ac_t& ac, const sks_t& quorum_ac_sks, const pks_t& all_ac_pks, mem_t label,
                  std::vector<bn_t>& x, bool skip_verify = false) const;
  const std::vector<ecc_point_t>& get_Q() const { return Q; }

 private:
  std::vector<ecc_point_t> Q;
  buf_t L;
  buf128_t b;
  struct row_t {
    buf_t x_bin, r, c;
    std::vector<CT_T> quorum_c;
  };
  std::vector<row_t> rows;

  static void encrypt_row(const crypto::ss::ac_t& ac, const pks_t& ac_pks, mem_t label, ecurve_t curve, mem_t seed,
                          mem_t plain, buf_t& c, std::vector<CT_T>& quorum_c);

  static void encrypt_row0(const crypto::ss::ac_t& ac, const pks_t& ac_pks, mem_t label, ecurve_t curve, mem_t r0_1,
                           mem_t r0_2, int batch_size, std::vector<bn_t>& x0, buf_t& c, std::vector<CT_T>& quorum_c);

  static void encrypt_row1(const crypto::ss::ac_t& ac, const pks_t& ac_pks, mem_t label, ecurve_t curve, mem_t r1,
                           mem_t x1_bin, buf_t& c, std::vector<CT_T>& quorum_c);

  static error_t find_quorum_ciphertext(const std::vector<std::string>& sorted_leaves, const std::string& path,
                                        const row_t& row, const CT_T*& c);

  error_t get_row_to_decrypt(const crypto::ss::ac_t& ac, int row_index, const std::string& path, buf_t& out) const;
  error_t restore_row(const crypto::ss::ac_t& ac, int row_index, const std::map<std::string, buf_t>& decrypted,
                      mem_t label, std::vector<bn_t>& x) const;
};

}  // namespace coinbase::mpc
