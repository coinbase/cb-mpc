#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/secret_sharing.h>

#include "pve.h"

namespace coinbase::mpc {

class ec_pve_ac_t {
 public:
  struct ciphertext_adapter_t {
    buf_t ct_ser;
    void convert(coinbase::converter_t& converter) { converter.convert(ct_ser); }
  };

  typedef std::map<std::string, const void*> pks_t;  // maps leaf path -> encryption key pointer
  typedef std::map<std::string, const void*> sks_t;  // maps leaf path -> decryption key pointer

  static constexpr int kappa = SEC_P_COM;
  static constexpr std::size_t iv_size = crypto::KEM_AEAD_IV_SIZE;
  static constexpr std::size_t tag_size = crypto::KEM_AEAD_TAG_SIZE;
  static constexpr std::size_t iv_bitlen = iv_size * 8;

  // Default to unified PKE when not provided explicitly
  ec_pve_ac_t();
  explicit ec_pve_ac_t(const pve_base_pke_i& base_pke) : base_pke(base_pke), rows(kappa) {}

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
   * - publicly-verifiable-encryption-spec | vdecrypt-local-batch-many-1P
   *
   * @notes:
   * Each party calls party_decrypt_row to produce its share for a specific row.
   * Then, the caller aggregates shares using aggregate_to_restore_row to recover x.
   * This is different from the spec since the decryption is not done in a loop, rather at each
   * invocation, a single row is decrypted. As a result, it is the responsibility of the caller application
   * to call this api multiple times if needed.
   */
  error_t party_decrypt_row(const crypto::ss::ac_t& ac, int row_index, const std::string& path, const void* prv_key_ptr,
                            mem_t label, bn_t& out_share) const;

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vdecrypt-combine-batch-many-1P
   */
  error_t aggregate_to_restore_row(const crypto::ss::ac_t& ac, int row_index, mem_t label,
                                   const std::map<std::string, bn_t>& quorum_decrypted, std::vector<bn_t>& x,
                                   bool skip_verify = false, const pks_t& all_ac_pks = pks_t()) const;
  const std::vector<ecc_point_t>& get_Q() const { return Q; }

 private:
  const pve_base_pke_i& base_pke;
  std::vector<ecc_point_t> Q;
  buf_t L;
  buf128_t b;
  struct row_t {
    buf_t x_bin, r, c;
    std::vector<ciphertext_adapter_t> quorum_c;
  };
  std::vector<row_t> rows;

  void encrypt_row(const crypto::ss::ac_t& ac, const pks_t& ac_pks, mem_t label, ecurve_t curve, mem_t seed,
                   mem_t plain, buf_t& c, std::vector<ciphertext_adapter_t>& quorum_c) const;

  void encrypt_row0(const crypto::ss::ac_t& ac, const pks_t& ac_pks, mem_t label, ecurve_t curve, mem_t r0_1,
                    mem_t r0_2, int batch_size, std::vector<bn_t>& x0, buf_t& c,
                    std::vector<ciphertext_adapter_t>& quorum_c) const;

  void encrypt_row1(const crypto::ss::ac_t& ac, const pks_t& ac_pks, mem_t label, ecurve_t curve, mem_t r1,
                    mem_t x1_bin, buf_t& c, std::vector<ciphertext_adapter_t>& quorum_c) const;

  static error_t find_quorum_ciphertext(const std::vector<std::string>& sorted_leaves, const std::string& path,
                                        const row_t& row, const ciphertext_adapter_t*& c);
};

}  // namespace coinbase::mpc
