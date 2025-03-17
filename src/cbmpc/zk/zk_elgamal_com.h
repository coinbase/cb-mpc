#pragma once
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/elgamal.h>
#include <cbmpc/zk/fischlin.h>
#include <cbmpc/zk/zk_ec.h>

namespace coinbase::zk {

struct uc_elgamal_com_t {
  uc_elgamal_com_t() : params{22, 6, 11} {}
  fischlin_params_t params;
  std::vector<elg_com_t> AB;
  std::vector<int> e;
  std::vector<bn_t> z1, z2;

  void convert(coinbase::converter_t& converter) { converter.convert(params, AB, e, z1, z2); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-UC-ZK-ElGamalCom-1P
   */
  void prove(const ecc_point_t& Q, elg_com_t UV, const bn_t& x, const bn_t& r, mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-UC-ZK-ElGamalCom-1P
   */
  error_t verify(const ecc_point_t& Q, const elg_com_t& UV, mem_t session_id, uint64_t aux) const;
};

struct elgamal_com_pub_share_equ_t {
  dh_t zk_dh;

  void convert(coinbase::converter_t& converter) { converter.convert(zk_dh); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-ZK-ElGamalCom-PubShare-Equal-1P
   */
  void prove(const ecc_point_t& Q, const ecc_point_t& A, const elg_com_t B, const bn_t& r, mem_t session_id,
             uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-ZK-ElGamalCom-PubShare-Equal-1P
   */
  error_t verify(const ecc_point_t& Q, const ecc_point_t& A, const elg_com_t B, mem_t session_id, uint64_t aux) const;
};

struct elgamal_com_mult_t {
  bn_t z1, z2, z3, e;
  void convert(coinbase::converter_t& converter) { converter.convert(z1, z2, z3, e); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-ZK-ElGamalCom-Mult-Com-1P
   */
  void prove(const ecc_point_t& Q, const elg_com_t& A, const elg_com_t& B, const elg_com_t& C, const bn_t& r_B,
             const bn_t& r_C, const bn_t& b, mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-ZK-ElGamalCom-Mult-Com-1P
   */
  error_t verify(const ecc_point_t& Q, const elg_com_t& A, const elg_com_t& B, const elg_com_t& C, mem_t session_id,
                 uint64_t aux) const;
};

struct uc_elgamal_com_mult_private_scalar_t {
  uc_elgamal_com_mult_private_scalar_t() : params{19, 7, 12} {}
  fischlin_params_t params;

  std::vector<int> e;
  std::vector<bn_t> z1, z2;
  std::vector<ecc_point_t> A1_tag, A2_tag;

  void convert(coinbase::converter_t& converter) { converter.convert(params, e, z1, z2, A1_tag, A2_tag); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-UC-ZK-ElGamalCom-Mult-Private-Scalar-1P
   *
   * @notes:
   * - with prover optimization
   */
  void prove(const ecc_point_t& E, const elg_com_t& eA, const elg_com_t& eB, const bn_t& r0, const bn_t& c,
             mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-UC-ZK-ElGamalCom-Mult-Private-Scalar-1P
   *
   * @notes:
   * - with verifier optimization
   */
  error_t verify(const ecc_point_t& E, const elg_com_t& eA, const elg_com_t& eB, mem_t session_id, uint64_t aux);
};

}  // namespace coinbase::zk
