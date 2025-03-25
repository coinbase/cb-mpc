#pragma once

#include <cbmpc/crypto/commitment.h>
#include <cbmpc/protocol/mpc_job.h>

namespace coinbase::mpc {

/**
 * @specs:
 * - basic-primitives-spec | committed-pairwise-broadcast-MP
 */
template <typename... ARGS>
inline error_t committed_pairwise_broadcast(job_mp_t& job, ARGS&... args) {
  error_t rv = UNINITIALIZED_ERROR;
  int n_parties = job.get_n_parties();
  int party_index = job.get_party_idx();
  const crypto::mpc_pid_t& pid = job.get_pid();

  auto com_rand = job.nonuniform_msg<buf256_t>();
  auto com_msg = job.nonuniform_msg<buf_t>();
  for (int i = 0; i < n_parties; i++) {
    coinbase::crypto::commitment_t com(pid, job.get_pid(i));
    // effectively, it does com.gen(args.msgs[i]...)
    std::apply([&com](auto&&... args) { com.gen(args...); },
               map_args_to_tuple([i](auto& arg) { return arg.msgs[i]; }, args...));
    com_rand[i] = com.rand;
    com_msg[i] = com.msg;
  }

  if (rv = job.plain_broadcast(com_msg)) return rv;
  if (rv = job.plain_broadcast(com_rand, args...)) return rv;

  for (int i = 0; i < n_parties; i++) {
    if (party_index == i) continue;

    coinbase::crypto::commitment_t com(job.get_pid(i), pid);

    com.set(com_rand.received(i), com_msg.received(i));
    // effectively, it does com.open(args.all_received_values())...
    rv = std::apply([&com](auto&&... args) { return com.open(args...); },
                    map_args_to_tuple([i](auto& arg) { return arg.received(i); }, args...));
    if (rv) return rv;
  }

  return SUCCESS;
}

/**
 * @specs:
 * - basic-primitives-spec | committed-group-broadcast-MP
 */
template <typename... ARGS>
inline error_t committed_group_broadcast(job_mp_t& job, ARGS&... args) {
  error_t rv = UNINITIALIZED_ERROR;

  const crypto::mpc_pid_t& pid = job.get_pid();
  coinbase::crypto::commitment_t com(pid);
  // effectively, it does com.gen(args.msg...)
  std::apply([&com](auto&&... args) { com.gen(args...); },
             map_args_to_tuple([](auto& arg) { return arg.msg; }, args...));

  auto com_rand = job.uniform_msg<buf256_t>(com.rand);
  auto com_msg = job.uniform_msg<buf_t>(com.msg);

  if (rv = job.plain_broadcast(com_msg)) return rv;

  auto v = job.uniform_msg<buf256_t>(crypto::sha256_t::hash(com_msg.all_received_refs()));

  if (rv = job.plain_broadcast(v, com_rand, args...)) return rv;

  for (int i = 0; i < job.get_n_parties(); i++) {
    if (v.msg != v.received(i)) return rv = job.mpc_abort(E_CRYPTO, "received hash mismatch");

    coinbase::crypto::commitment_t com(job.get_pid(i));
    com.set(com_rand.received(i), com_msg.received(i));
    rv = std::apply([&com](auto&&... args) { return com.open(args...); },
                    map_args_to_tuple([i](auto& arg) { return arg.received(i); }, args...));
    if (rv) return rv;
  }

  return SUCCESS;
}

}  // namespace coinbase::mpc