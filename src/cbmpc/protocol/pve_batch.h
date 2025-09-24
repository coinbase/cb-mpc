#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/pve_base.h>
#include <cbmpc/zk/zk_ec.h>

namespace coinbase::mpc {

class ec_pve_batch_t {
 public:
  // Default to unified PKE when not provided explicitly
  explicit ec_pve_batch_t(int batch_count);
  ec_pve_batch_t(int batch_count, const pve_base_pke_i& base_pke)
      : base_pke(base_pke), n(batch_count), rows(kappa), Q(n) {}

  // Custom copy/move ctors bind the reference member correctly
  ec_pve_batch_t(const ec_pve_batch_t& other)
      : base_pke(other.base_pke), n(other.n), L(other.L), Q(other.Q), b(other.b), rows(other.rows) {}
  ec_pve_batch_t(ec_pve_batch_t&& other) noexcept
      : base_pke(other.base_pke),
        n(other.n),
        L(std::move(other.L)),
        Q(std::move(other.Q)),
        b(other.b),
        rows(std::move(other.rows)) {}
  // Assignment operators copy payload fields; reference member remains bound
  ec_pve_batch_t& operator=(const ec_pve_batch_t& other) {
    if (this == &other) return *this;
    n = other.n;
    L = other.L;
    Q = other.Q;
    b = other.b;
    rows = other.rows;
    return *this;
  }
  ec_pve_batch_t& operator=(ec_pve_batch_t&& other) noexcept {
    if (this == &other) return *this;
    n = other.n;
    L = std::move(other.L);
    Q = std::move(other.Q);
    b = other.b;
    rows = std::move(other.rows);
    return *this;
  }

  const static int kappa = SEC_P_COM;
  // We assume the base encryption scheme requires 32 bytes of randomness. If it needs more, it can be changed to use
  // DRBG with 32 bytes of randomness as the seed.
  const static int rho_size = 32;

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vencrypt-batch-1P
   */
  void encrypt(const void* ek, mem_t label, ecurve_t curve, const std::vector<bn_t>& x);

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vverify-batch-1P
   */
  error_t verify(const void* ek, const std::vector<ecc_point_t>& Q, mem_t label) const;

  /**
   * @specs:
   * - publicly-verifiable-encryption-spec | vdecrypt-batch-1P
   */
  error_t decrypt(const void* dk, const void* ek, mem_t label, ecurve_t curve, std::vector<bn_t>& x,
                  bool skip_verify = false) const;

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
  const pve_base_pke_i& base_pke;
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