#pragma once
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/commitment.h>
#include <cbmpc/zk/zk_util.h>

namespace coinbase::zk {

struct pedersen_commitment_params_t {
  mod_t p, p_tag;
  bn_t g, h;
  bn_t sqrt_g, sqrt_h;

  static const pedersen_commitment_params_t& get();
  error_t check_safe_prime_subgroup(const bn_t& x) const;

 private:
  pedersen_commitment_params_t();
};

struct range_pedersen_t {
  inline static constexpr int t = SEC_P_COM;

  buf128_t e;
  bn_t d[t];
  bn_t f[t];
  bn_t c_tilde[t];

  void convert(coinbase::converter_t& converter) { converter.convert(e, d, f, c_tilde); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-ZK-Range-Pedersen-1P
   */
  void prove(const bn_t& q, const bn_t& c, const bn_t& x, const bn_t& r, mem_t session_id, uint64_t aux);
  void prove(const bn_t& q, const bn_t& g, const bn_t& h, const bn_t& c, const bn_t& x, const bn_t& r, mem_t session_id,
             uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-ZK-Range-Pedersen-1P
   */
  error_t verify(const bn_t& q, const bn_t& c, mem_t session_id, uint64_t aux) const;
  error_t verify(const bn_t& q, const bn_t& g, const bn_t& h, const bn_t& c, mem_t session_id, uint64_t aux) const;
};

/**
 * @specs:
 * - zk-proofs-spec | ZK-Range-Pedersen-2P
 */
struct range_pedersen_interactive_t {
  constexpr static int t = SEC_P_STAT_SHORT;

  range_pedersen_interactive_t() = delete;

  const crypto::mpc_pid_t prover_pid;
  range_pedersen_interactive_t(const crypto::mpc_pid_t& pid) : prover_pid(pid) {}

  coinbase::crypto::commitment_t com;
  uint64_t e;
  std::array<bn_t, t> c_tilde;
  std::array<bn_t, t> d;
  std::array<bn_t, t> f;

  bn_t xi[t];
  bn_t ri[t];
  bn_t q_bn;

  AUTO(msg1, std::tie(com.msg));
  AUTO(challenge, std::tie(e));
  AUTO(msg2, std::tie(c_tilde, com.rand, d, f));

  void prover_msg1(const mod_t& q);
  void verifier_challenge();
  void prover_msg2(const bn_t& x, const bn_t& r);
  error_t verify(const bn_t& c, const mod_t& q);
};

struct paillier_pedersen_equal_t {
  using param = paillier_non_interactive_param_t;

  zk_flag paillier_valid_key = zk_flag::unverified;
  zk_flag paillier_no_small_factors = zk_flag::unverified;
  zk_flag paillier_valid_ciphertext = zk_flag::unverified;

  bn_t e;
  bn_t di[param::t];
  bn_t D[param::t];
  bn_t Com_tilde;
  bn_t nu;

  void convert(coinbase::converter_t& converter) { converter.convert(e, di, D, Com_tilde, nu); }

  /**
   * @specs:
   * - zk-proofs-spec | Prove-Paillier-Pedersen-Equal-1P
   */
  void prove(const crypto::paillier_t& paillier, const bn_t& c, const mod_t& q, const bn_t& Com, const bn_t& x,
             const bn_t& R, const bn_t& rho, mem_t session_id, uint64_t aux);

  /**
   * @specs:
   * - zk-proofs-spec | Verify-Paillier-Pedersen-Equal-1P
   */
  error_t verify(const crypto::paillier_t& paillier, const bn_t& c, const mod_t& q, const bn_t& Com, mem_t session_id,
                 uint64_t aux);
};

/**
 * @specs:
 * - zk-proofs-spec | ZK-Paillier-Pedersen-Equal-2P
 */
struct paillier_pedersen_equal_interactive_t {
  using param = paillier_interactive_param_t;

  const crypto::mpc_pid_t prover_pid;
  paillier_pedersen_equal_interactive_t() = delete;
  paillier_pedersen_equal_interactive_t(const crypto::mpc_pid_t& pid) : prover_pid(pid) {}

  zk_flag paillier_valid_key = zk_flag::unverified;
  zk_flag paillier_no_small_factors = zk_flag::unverified;

  coinbase::crypto::commitment_t com;
  bn_t e;
  std::array<bn_t, param::t> c_tilde;
  std::array<bn_t, param::t> di;
  std::array<bn_t, param::t> Di;
  bn_t Com_tilde;
  bn_t mu, nu, r;

  bn_t ri[param::t];
  bn_t R_tilde[param::t];

  AUTO(msg1, std::tie(com.msg));
  AUTO(challenge, std::tie(e));
  AUTO(msg2, std::tie(c_tilde, Com_tilde, com.rand, di, Di, nu));

  void prover_msg1(const crypto::paillier_t& paillier, const mod_t& q);
  void verifier_challenge();
  void prover_msg2(const crypto::paillier_t& paillier, const bn_t& x, const bn_t& R, const bn_t& rho);
  error_t verify(const crypto::paillier_t& paillier, const bn_t& c, const mod_t& q, const bn_t& Com);
};

}  // namespace coinbase::zk
