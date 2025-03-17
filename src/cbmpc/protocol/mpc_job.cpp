#include "mpc_job.h"

namespace coinbase::mpc {

error_t job_mp_t::send_to_parties(party_set_t set, const std::vector<buf_t>& in) {
  error_t rv = UNINITIALIZED_ERROR;
  set.remove(party_index);
  for (int i = 0; i < n_parties; i++) {
    if (!set.has(i)) continue;
    if (rv = send_impl(i, in[i])) return rv;
  }
  return SUCCESS;
}

// default implementation simply by receiving one by one
error_t job_mp_t::receive_many_impl(std::vector<party_idx_t> from_set, std::vector<mem_t>& outs) {
  error_t rv = UNINITIALIZED_ERROR;
  outs.resize(from_set.size());
  for (int i = 0; i < from_set.size(); i++) {
    if (rv = receive_impl(from_set[i], outs[i])) return rv;
  }
  return SUCCESS;
}

error_t job_mp_t::receive_from_parties(party_set_t set, std::vector<buf_t>& v) {
  error_t rv = UNINITIALIZED_ERROR;

  set.remove(party_index);
  std::vector<party_idx_t> peer_roles(0);

  int n = 0;
  for (int i = 0; i < n_parties; i++) {
    if (!set.has(i)) continue;
    peer_roles.push_back(i);
    n++;
  }
  std::vector<mem_t> outs(n);

  if (rv = receive_many_impl(peer_roles, outs)) return rv;

  v.resize(n_parties);
  n = 0;
  for (int i = 0; i < n_parties; i++) {
    if (!set.has(i)) continue;
    v[i] = outs[n];
    n++;
  }
  return SUCCESS;
}

}  // namespace coinbase::mpc