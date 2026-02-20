#pragma once

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/protocol/pve_base.h>
#include <cbmpc/internal/zk/zk_ec.h>

namespace coinbase::mpc {

class ec_pve_batch_t {
 public:
  explicit ec_pve_batch_t(int batch_count) : n(batch_count), rows(kappa) {
    cb_assert(batch_count > 0 && batch_count <= MAX_BATCH_COUNT);
    Q.resize(n);
  }

  const static int kappa = SEC_P_COM;
  // Upper bound to prevent integer-overflow and unbounded memory allocation when `n` is untrusted.
  // This is a defensive limit; callers should treat any larger batch as invalid input.
  static constexpr int MAX_BATCH_COUNT = 100000;
  // We assume the base encryption scheme requires 32 bytes of randomness. If it needs more, it can be changed to use
  // DRBG with 32 bytes of randomness as the seed.
  const static int rho_size = 32;

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vencrypt-batch-1P
   */
  error_t encrypt(const pve_base_pke_i& base_pke, pve_keyref_t ek, mem_t label, ecurve_t curve,
                  const std::vector<bn_t>& x);

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vverify-batch-1P
   */
  error_t verify(const pve_base_pke_i& base_pke, pve_keyref_t ek, const std::vector<ecc_point_t>& Q, mem_t label) const;

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vdecrypt-batch-1P
   */
  error_t decrypt(const pve_base_pke_i& base_pke, pve_keyref_t dk, pve_keyref_t ek, mem_t label, ecurve_t curve,
                  std::vector<bn_t>& x, bool skip_verify = false) const;

  int batch_count() const { return n; }
  const std::vector<ecc_point_t>& get_Qs() const { return Q; }
  const buf_t& get_Label() const { return L; }

  void convert(coinbase::converter_t& converter) {
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

  error_t restore_from_decrypted(int row_index, mem_t decrypted_x_buf, ecurve_t curve, std::vector<bn_t>& xs) const;
};

}  // namespace coinbase::mpc