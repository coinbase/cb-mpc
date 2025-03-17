#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/zk/fischlin.h>

namespace coinbase::zk {

struct uc_dl_t {
  uc_dl_t() : params{32, 4, 9} {}

  fischlin_params_t params;
  std::vector<ecc_point_t> A;
  std::vector<int> e;
  std::vector<bn_t> z;
  void convert(coinbase::converter_t& c) { c.convert(params, A, e, z); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-UC-ZK-DL-1P
   */
  void prove(const ecc_point_t& Q, const bn_t& w, mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-UC-ZK-DL-1P
   */
  error_t verify(const ecc_point_t& Q, mem_t session_id, uint64_t aux) const;
};

struct uc_batch_dl_finite_difference_impl_t {
  fischlin_params_t params;
  std::vector<ecc_point_t> R;
  std::vector<int> e;
  std::vector<bn_t> z;

  void convert(coinbase::converter_t& c) { c.convert(params, R, e, z); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-UC-ZK-Batch-DL-1P
   *
   * @notes: with dedicated optimization and the optimization for Step 3 of the prover
   */
  void prove(const std::vector<ecc_point_t>& Q, const std::vector<bn_t>& w, mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-UC-ZK-Batch-DL-1P
   *
   * @notes: with dedicated optimization and the optimization for Step 3 of the prover
   */
  error_t verify(const std::vector<ecc_point_t>& Q, mem_t session_id, uint64_t aux) const;

  struct matrix_sum_t {
   public:
    matrix_sum_t(int n) : offset((n + 1) / 2), data(n + 3, std::vector<bn_t>(n + 1)) {}
    const std::vector<bn_t>& operator[](int i) const { return data[i + offset]; }
    std::vector<bn_t>& operator[](int i) { return data[i + offset]; }

   private:
    int offset;
    std::vector<std::vector<bn_t>> data;
  };

  struct vector_sum_t {
   public:
    vector_sum_t(int n, int t) : offset((n + 1) / 2), data(1 << t) {}
    const bn_t& operator[](int i) const { return data[i + offset]; }
    bn_t& operator[](int i) { return data[i + offset]; }

   private:
    int offset;
    std::vector<bn_t> data;
  };
};
using uc_batch_dl_t = uc_batch_dl_finite_difference_impl_t;

struct dh_t {
  bn_t e, z;
  void convert(coinbase::converter_t& converter) { converter.convert(e, z); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-ZK-DH-1P
   */
  void prove(const ecc_point_t& Q, const ecc_point_t& A, const ecc_point_t& B, const bn_t& w, mem_t session_id,
             uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-ZK-DH-1P
   */
  error_t verify(const ecc_point_t& Q, const ecc_point_t& A, const ecc_point_t& B, mem_t session_id,
                 uint64_t aux) const;
};

}  // namespace coinbase::zk
