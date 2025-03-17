#pragma once
#include <cbmpc/crypto/base.h>
#include <cbmpc/zk/zk_pedersen.h>
#include <cbmpc/zk/zk_util.h>

namespace coinbase::zk {

struct valid_paillier_t {
  using param = paillier_non_interactive_param_t;

  bn_t sigma[param::t];
  zk_flag paillier_valid_key = zk_flag::unverified;
  zk_flag paillier_no_small_factors = zk_flag::unverified;

  void convert(coinbase::converter_t& converter) { converter.convert(sigma); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-ZK-Valid-Paillier-1P
   */
  void prove(const crypto::paillier_t& paillier, mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-ZK-Valid-Paillier-1P
   */
  error_t verify(const crypto::paillier_t& paillier, mem_t session_id, uint64_t aux);
};

/**
 * @specs:
 * - zk-proofs-spec | ZK-Valid-Paillier-Interactive-2P
 */
struct valid_paillier_interactive_t {
  using param = paillier_interactive_param_t;

  zk_flag paillier_valid_key = zk_flag::unverified;
  zk_flag paillier_no_small_factors = zk_flag::unverified;

  struct challenge_msg_t {
    buf128_t kV;
    void convert(coinbase::converter_t& converter) { converter.convert(kV); }
  };

  struct prover_msg_t {
    bn_t sigma[param::t];
    void convert(coinbase::converter_t& converter) { converter.convert(sigma); }
  };

  void challenge(challenge_msg_t& challenge_msg);
  void prove(const crypto::paillier_t& paillier, const challenge_msg_t& challenge_msg,
             const crypto::mpc_pid_t& prover_pid, prover_msg_t& prover_msg) const;
  error_t verify(const crypto::paillier_t& paillier, const crypto::mpc_pid_t& prover_pid,
                 const prover_msg_t& prover_msg);

 private:
  buf128_t kV;
};

struct paillier_zero_t {
  using param = paillier_non_interactive_param_t;
  zk_flag paillier_valid_key = zk_flag::unverified;
  zk_flag paillier_valid_ciphertext = zk_flag::unverified;
  zk_flag paillier_no_small_factors = zk_flag::unverified;

  buf_t e;
  bn_t z[param::t];
  void convert(coinbase::converter_t& converter) { converter.convert(e, z); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-ZK-Paillier-Zero-1P
   */
  void prove(const crypto::paillier_t& paillier, const bn_t& c, const bn_t& r, mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-ZK-Paillier-Zero-1P
   */
  error_t verify(const crypto::paillier_t& paillier, const bn_t& c, mem_t session_id, uint64_t aux);
};

/**
 * @specs:
 * - zk-proofs-spec | ZK-Paillier-Zero-Interactive-2P
 */
struct paillier_zero_interactive_t {
  using param = paillier_interactive_param_t;

  const crypto::mpc_pid_t prover_pid;
  paillier_zero_interactive_t() = delete;
  paillier_zero_interactive_t(const crypto::mpc_pid_t& pid) : prover_pid(pid) {}

  zk_flag paillier_valid_key = zk_flag::unverified;
  zk_flag paillier_valid_ciphertext = zk_flag::unverified;
  zk_flag paillier_no_small_factors = zk_flag::unverified;

  bn_t rho[param::t];
  std::array<bn_t, param::t> a;
  std::array<uint16_t, param::t> e;
  std::array<bn_t, param::t> z;
  coinbase::crypto::commitment_t com;

  AUTO(msg1, std::tie(com.msg));
  AUTO(challenge, std::tie(e));
  AUTO(msg2, std::tie(a, z, com.rand));

  void prover_msg1(const crypto::paillier_t& paillier);
  void verifier_challenge();
  void prover_msg2(const crypto::paillier_t& paillier, const bn_t& r);
  error_t verify(const crypto::paillier_t& paillier, const bn_t& c);
};

struct two_paillier_equal_t {
  using param = paillier_non_interactive_param_t;

  zk_flag p0_valid_key = zk_flag::unverified;
  zk_flag p1_valid_key = zk_flag::unverified;
  zk_flag p0_valid_ciphertext = zk_flag::unverified;
  zk_flag p1_valid_ciphertext = zk_flag::unverified;
  zk_flag p0_no_small_factors = zk_flag::unverified;
  zk_flag p1_no_small_factors = zk_flag::unverified;
  zk_flag c0_plaintext_range = zk_flag::unverified;
  zk_flag c1_plaintext_range = zk_flag::unverified;

  buf_t e;
  bn_t d[param::t];
  bn_t r0_hat[param::t];
  bn_t r1_hat[param::t];

  void convert(coinbase::converter_t& converter) { converter.convert(e, d, r0_hat, r1_hat); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-ZK-Two-Paillier-Equal-1P
   */
  void prove(const mod_t& q, const crypto::paillier_t& P0, const bn_t& c0, const crypto::paillier_t& P1, const bn_t& c1,
             const bn_t& x, const bn_t& r0, const bn_t& r1, mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-ZK-Two-Paillier-Equal-1P
   */
  error_t verify(const mod_t& q, const crypto::paillier_t& P0, const bn_t& c0, const crypto::paillier_t& P1,
                 const bn_t& c1, mem_t session_id, uint64_t aux);
};

/**
 * @specs:
 * - zk-proofs-spec | ZK-Two-Paillier-Equal-Interactive-2P
 */
struct two_paillier_equal_interactive_t {
  using param = paillier_interactive_param_t;

  const crypto::mpc_pid_t prover_pid;
  two_paillier_equal_interactive_t() = delete;
  two_paillier_equal_interactive_t(const crypto::mpc_pid_t& pid) : prover_pid(pid) {}

  zk_flag p0_valid_key = zk_flag::unverified;
  zk_flag p1_valid_key = zk_flag::unverified;
  zk_flag p0_valid_ciphertext = zk_flag::unverified;
  zk_flag p1_valid_ciphertext = zk_flag::unverified;
  zk_flag p0_no_small_factors = zk_flag::unverified;
  zk_flag p1_no_small_factors = zk_flag::unverified;
  zk_flag c0_plaintext_range = zk_flag::unverified;
  zk_flag c1_plaintext_range = zk_flag::unverified;

  struct prover_msg1_t {
    buf_t com_msg;
    void convert(coinbase::converter_t& converter) { converter.convert(com_msg); }
  };

  struct verifier_challenge_msg_t {
    buf_t e;
    void convert(coinbase::converter_t& converter) { converter.convert(e); }
  };

  struct prover_msg2_t {
    buf256_t com_rand;
    bn_t c0_tilde[param::t];
    bn_t c1_tilde[param::t];
    bn_t d[param::t];
    bn_t r0_hat[param::t];
    bn_t r1_hat[param::t];
    void convert(coinbase::converter_t& converter) {
      converter.convert(com_rand, c0_tilde, c1_tilde, d, r0_hat, r1_hat);
    }
  };

 private:
  buf_t e;
  bn_t tau[param::t];
  bn_t c0_tilde[param::t];
  bn_t c1_tilde[param::t];
  bn_t R0_tilde[param::t];
  bn_t R1_tilde[param::t];
  buf256_t com_rand;

 public:
  void prover_msg1(const mod_t& q, const crypto::paillier_t& P0, const crypto::paillier_t& P1, prover_msg1_t& msg1);

  void verifier_challenge_msg(verifier_challenge_msg_t& msg);

  error_t prover_msg2(const crypto::paillier_t& P0, const crypto::paillier_t& P1, const bn_t& x, const bn_t& r0,
                      const bn_t& r1, const verifier_challenge_msg_t& challenge_msg, prover_msg2_t& msg2) const;

  error_t verify(const mod_t& q, const crypto::paillier_t& P0, const bn_t& c0, const crypto::paillier_t& P1,
                 const bn_t& c1, const prover_msg1_t& msg1, const prover_msg2_t& msg2);
};

struct paillier_range_exp_slack_t {
  zk_flag paillier_valid_key = zk_flag::unverified;
  zk_flag paillier_no_small_factors = zk_flag::unverified;

  bn_t Com;
  paillier_pedersen_equal_t zk_paillier_pedersen_equal;
  range_pedersen_t zk_range_pedersen;

  void convert(coinbase::converter_t& converter) {
    converter.convert(Com, zk_paillier_pedersen_equal, zk_range_pedersen);
  }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-ZK-Paillier-Range-Exp-Slack-1P
   */
  void prove(const crypto::paillier_t& paillier, const mod_t& q, const bn_t& c, const bn_t& x, const bn_t& r,
             mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-ZK-Paillier-Range-Exp-Slack-1P
   */
  error_t verify(const crypto::paillier_t& paillier, const mod_t& q, const bn_t& c, mem_t session_id, uint64_t aux);
};

struct pdl_t {
  zk_flag paillier_valid_key = zk_flag::unverified;
  zk_flag paillier_no_small_factors = zk_flag::unverified;
  zk_flag paillier_valid_ciphertext = zk_flag::unverified;
  zk_flag paillier_range_exp_slack_proof = zk_flag::unverified;

  bn_t c_r, z, r_z;
  ecc_point_t R;
  paillier_range_exp_slack_t zk_paillier_range_exp_slack;

  void convert(coinbase::converter_t& converter) { converter.convert(c_r, R, z, r_z, zk_paillier_range_exp_slack); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-ZK-PDL-1P
   */
  void prove(const bn_t& c_key,
             const crypto::paillier_t& paillier,  // private
             const ecc_point_t& Q, const bn_t& x1, const bn_t& r_key, mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-ZK-PDL-1P
   */
  error_t verify(const bn_t& c_key,
                 const crypto::paillier_t& paillier,  // public
                 const ecc_point_t& Q, mem_t session_id, uint64_t aux);
};

}  // namespace coinbase::zk
