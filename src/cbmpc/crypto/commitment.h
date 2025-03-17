#pragma once

#include <cbmpc/crypto/ro.h>

namespace coinbase::crypto {

/**
 * @notes:
 * - Based on the conventions in the basic-primitives-spec, by default we assume that the
 *   library has access to good pid values (e.g., the actual public key of the committing party).
 *   Therefore, the main way of using the commitment
 *   is to call it with a pid value (e.g., commitment_t(pid) or commitment_t(pid, receiver_pid)).
 *   These calls will generate a local sid together with the pid.
 *
 * - The calls to commitment_t(sid, pid) or commitment_t(sid, pid, receiver_pid) are meant to be
 *   used in two cases
 *   1. If a local sid has already been generated, and we want to be more efficient by reusing it.
 *      In this case, it is important to make sure the same local sid is NOT reused in two
 *      different commitments.
 *   2. If a good pid and global sid are already available and they are both provided just to be
 *      safe.
 *   3. If a good global sid is already available but a good pid is not available. This should
 *      not happen given our assumption above and is only considered for the rare cases that it
 *      might be needed.
 *
 * - The receiver_pid is used in cases that we want to bind the commitment to a specific recipient.
 *   It is typically not used, but sometimes can be useful (e.g., in committed pairwise broadcast).
 *
 * - How to use:
 *   - If sid is not passed:
 *     - create the commitment:
 *       - com(pid) followed by com.gen(args...)
 *     - send the commitment hash and sid to the receiver:
 *       - mpc_broadcast(com.msg)
 *     - send the opening:
 *       - mpc_broadcast(com.rand)
 *     - verify (sometimes when sending a batch of commitments, you may need to set the
 *       randomness, local sid, etc as well)
 *   - If sid is passed:
 *     - In this case, com.local_sid is not used and the caller is responsible for making sure
 *       that both sides have the same sid.
 *   - Both of these options can be used using the `id` method by first defining the commitment
 *     instance and then calling `id` with the appropriate arguments.
 */
struct commitment_t {
  buf256_t rand;
  buf_t msg;
  static constexpr size_t HASH_SIZE = 32;
  static constexpr size_t LOCAL_SID_SIZE = SEC_P_COM / 8;

  explicit commitment_t(const mpc_pid_t& pid) : pid(pid){};
  explicit commitment_t(const mpc_pid_t& pid, const mpc_pid_t& receiver_pid) : pid(pid), receiver_pid(receiver_pid){};
  explicit commitment_t(const mem_t sid, const mpc_pid_t& pid) : external_sid(sid), pid(pid){};
  explicit commitment_t(const mem_t sid, const mpc_pid_t& pid, const mpc_pid_t& receiver_pid)
      : external_sid(sid), pid(pid), receiver_pid(receiver_pid){};
  commitment_t() {}

  /**
   * @specs:
   * - basic-primitives-spec | Comp-1P
   */
  template <typename... ARGS>
  void gen(const ARGS&... args) {
    crypto::gen_random(rand);
    gen_with_set_rand(args...);
  }

  template <typename... ARGS>
  void gen_with_set_rand(const ARGS&... args) {
    if (external_sid.size == 0) {
      local_sid = gen_random_bitlen(SEC_P_COM);
    }

    ro::hmac_state_t state(rand);
    state.encode_and_update(args...);
    msg = final(state);
  }

  template <typename... ARGS>
  error_t open(const ARGS&... args) {
    ro::hmac_state_t state(rand);
    state.encode_and_update(args...);
    if (external_sid.size == 0) {
      // Means that local_sid was used and therefore it should be extracted out of the hash parameter
      cb_assert(msg.size() == HASH_SIZE + LOCAL_SID_SIZE);
      local_sid = msg.skip(HASH_SIZE);
    } else {
      cb_assert(msg.size() == HASH_SIZE);
    }
    buf_t m = final(state);
    if (m != msg) return coinbase::error(E_CRYPTO);
    return SUCCESS;
  }

  commitment_t& id(const mem_t _sid, const mpc_pid_t& _pid, const mpc_pid_t& _receiver_pid) {
    external_sid = _sid;
    pid = _pid;
    receiver_pid = _receiver_pid;
    return *this;
  }
  commitment_t& id(const mem_t _sid, const mpc_pid_t& _pid) {
    external_sid = _sid;
    pid = _pid;
    return *this;
  }
  commitment_t& id(const mpc_pid_t& _pid) {
    pid = _pid;
    return *this;
  }

  commitment_t& set(buf256_t _rand, buf_t _msg) {
    rand = _rand;
    msg = _msg;
    return *this;
  }

 private:
  mem_t external_sid;
  mpc_pid_t pid;
  mpc_pid_t receiver_pid;
  buf_t local_sid;

  buf_t final(ro::hmac_state_t& state) {
    if (external_sid.size == 0) {
      cb_assert(local_sid.size() > 0 && "no external sid or local sid provided");
      cb_assert(pid > 0 && "when using local sid, pid must be provided");
      state.update(local_sid);
    } else {
      state.update(external_sid);
    }
    if (pid > 0) state.update(pid);
    if (receiver_pid > 0) state.update(receiver_pid);
    buf256_t hmac_hash = buf256_t::load(state.final());
    buf_t combined_hash = hmac_hash;
    if (external_sid.size == 0) combined_hash += local_sid;
    return combined_hash;
  }
};

}  // namespace coinbase::crypto