#include "agree_random.h"

#include <cbmpc/crypto/commitment.h>
#include <cbmpc/protocol/committed_broadcast.h>

namespace coinbase::mpc {

error_t agree_random(job_2p_t& job, int bitlen, buf_t& out) {
  error_t rv = UNINITIALIZED_ERROR;
  buf_t r1, r2;
  const crypto::mpc_pid_t& sender_pid = job.get_pid(party_t::p1);
  coinbase::crypto::commitment_t com(sender_pid);

  if (job.is_p1()) {
    r1 = crypto::gen_random_bitlen(bitlen);
    com.gen(r1);
  }

  if (rv = job.p1_to_p2(com.msg)) return rv;

  if (job.is_p2()) {
    r2 = crypto::gen_random_bitlen(bitlen);
  }

  if (rv = job.p2_to_p1(r2)) return rv;
  if (rv = job.p1_to_p2(r1, com.rand)) return rv;

  if (job.is_p2()) {
    if (rv = com.open(r1)) return rv;
  }

  if (r1.size() != coinbase::bits_to_bytes(bitlen)) return coinbase::error(E_CRYPTO);
  if (r2.size() != coinbase::bits_to_bytes(bitlen)) return coinbase::error(E_CRYPTO);

  out = mem_t(r1) ^ mem_t(r2);
  return SUCCESS;
}

error_t weak_agree_random_p1_first(job_2p_t& job, int bitlen, buf_t& out) {
  if (bitlen < SEC_P_COM) return coinbase::error(E_CRYPTO);
  error_t rv = UNINITIALIZED_ERROR;
  buf_t rnd1, rnd2;
  if (job.is_p1()) rnd1 = crypto::gen_random_bitlen(SEC_P_COM);
  if (rv = job.p1_to_p2(rnd1)) return rv;

  if (job.is_p2()) rnd2 = crypto::gen_random_bitlen(SEC_P_COM);
  if (rv = job.p2_to_p1(rnd2)) return rv;

  if (rnd1.size() != coinbase::bits_to_bytes(SEC_P_COM)) return coinbase::error(E_CRYPTO);
  if (rnd2.size() != coinbase::bits_to_bytes(SEC_P_COM)) return coinbase::error(E_CRYPTO);

  crypto::ro::hash_string_t h(rnd1, rnd2);
  out = h.bitlen(bitlen);

  return SUCCESS;
}

error_t weak_agree_random_p2_first(job_2p_t& job, int bitlen, buf_t& out) {
  if (bitlen < SEC_P_COM) return coinbase::error(E_CRYPTO);
  error_t rv = UNINITIALIZED_ERROR;
  buf_t rnd1, rnd2;
  if (job.is_p2()) rnd1 = crypto::gen_random_bitlen(SEC_P_COM);
  if (rv = job.p2_to_p1(rnd1)) return rv;

  if (job.is_p1()) rnd2 = crypto::gen_random_bitlen(SEC_P_COM);
  if (rv = job.p1_to_p2(rnd2)) return rv;

  if (rnd1.size() != coinbase::bits_to_bytes(SEC_P_COM)) return coinbase::error(E_CRYPTO);
  if (rnd2.size() != coinbase::bits_to_bytes(SEC_P_COM)) return coinbase::error(E_CRYPTO);

  crypto::ro::hash_string_t h(rnd1, rnd2);
  out = h.bitlen(bitlen);

  return SUCCESS;
}

error_t multi_agree_random(job_mp_t& job, int t, buf_t& out) {
  error_t rv = UNINITIALIZED_ERROR;

  job_mp_t::uniform_msg_t<buf_t> r = job.uniform_msg<buf_t>(crypto::gen_random_bitlen(t));

  if (rv = committed_group_broadcast(job, r)) return rv;

  out = r.msg;
  for (int i = 0; i < job.get_n_parties(); i++) {
    if (i == job.get_party_idx()) continue;
    out ^= r.received(i);
  }

  return SUCCESS;
}

error_t weak_multi_agree_random(job_mp_t& job, int t, buf_t& out) {
  error_t rv = UNINITIALIZED_ERROR;

  if (t < SEC_P_COM) return coinbase::error(E_CRYPTO);
  auto r = job.uniform_msg<buf_t>(crypto::gen_random_bitlen(SEC_P_COM));
  if (rv = job.plain_broadcast(r)) return rv;

  auto hashed_r = job.uniform_msg<buf_t>(crypto::ro::hash_string(r.all_received_refs()).bitlen(t));
  if (rv = job.plain_broadcast(hashed_r)) return rv;

  for (int i = 0; i < job.get_n_parties(); i++)
    if (hashed_r.received(i) != hashed_r.msg) return coinbase::error(E_CRYPTO);

  out = hashed_r.msg;

  return SUCCESS;
}

error_t multi_pairwise_agree_random(job_mp_t& job, int bitlen, std::vector<buf_t>& out) {
  error_t rv = UNINITIALIZED_ERROR;

  int n = job.get_n_parties();
  auto r = job.nonuniform_msg<buf_t>();
  for (int i = 0; i < n; i++) r[i] = crypto::gen_random_bitlen(SEC_P_COM);
  if (rv = committed_pairwise_broadcast(job, r)) return rv;

  out.resize(n);
  for (int i = 0; i < n; i++) {
    if (r.received(i).size() != coinbase::bits_to_bytes(SEC_P_COM)) return coinbase::error(E_CRYPTO);
    buf_t k = r[i] ^ r.received(i);
    out[i] = crypto::ro::drbg_sample_string(k, bitlen);
  }

  return SUCCESS;
}

}  // namespace coinbase::mpc