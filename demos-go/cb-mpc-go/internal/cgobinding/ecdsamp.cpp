// ecdsamp.cpp – Signing-only bindings (key management moved to eckeymp.cpp)

#include "ecdsamp.h"

#include <memory>

#include <cbmpc/core/buf.h>
#include <cbmpc/protocol/ecdsa_mp.h>
#include <cbmpc/protocol/mpc_job_session.h>
#include <cbmpc/ffi/cmem_adapter.h>

#include "curve.h"
#include "network.h"

using namespace coinbase;
using namespace coinbase::mpc;

// -----------------------------------------------------------------------------
// ECDSA-MPC signing helpers
// -----------------------------------------------------------------------------

int mpc_ecdsampc_sign(job_mp_ref* j, mpc_eckey_mp_ref* k, cmem_t msg_mem, int sig_receiver, cmem_t* sig_mem) {
  job_mp_t* job = static_cast<job_mp_t*>(j->opaque);
  ecdsampc::key_t* key = static_cast<ecdsampc::key_t*>(k->opaque);

  buf_t msg = coinbase::ffi::view(msg_mem);
  buf_t sig;
  error_t err = ecdsampc::sign(*job, *key, msg, party_idx_t(sig_receiver), sig);
  if (err) return err;
  *sig_mem = coinbase::ffi::copy_to_cmem(sig);
  return 0;
}

int mpc_ecdsampc_sign_with_ot_roles(job_mp_ref* j, mpc_eckey_mp_ref* k, cmem_t msg_mem, int sig_receiver,
                                    cmems_t ot_role_map, int n_parties, cmem_t* sig_mem) {
  job_mp_t* job = static_cast<job_mp_t*>(j->opaque);
  ecdsampc::key_t* key = static_cast<ecdsampc::key_t*>(k->opaque);

  buf_t msg = coinbase::ffi::view(msg_mem);
  std::vector<buf_t> role_bufs = coinbase::ffi::bufs_from_cmems(ot_role_map);
  std::vector<std::vector<int>> ot_roles(n_parties, std::vector<int>(n_parties));

  for (int i = 0; i < n_parties; i++) {
    if (i < role_bufs.size()) {
      const uint8_t* data = role_bufs[i].data();
      for (int j = 0; j < n_parties && j * sizeof(int) < role_bufs[i].size(); j++) {
        memcpy(&ot_roles[i][j], data + j * sizeof(int), sizeof(int));
      }
    }
  }

  buf_t sig;
  error_t err = ecdsampc::sign(*job, *key, msg, party_idx_t(sig_receiver), ot_roles, sig);
  if (err) return err;

  *sig_mem = coinbase::ffi::copy_to_cmem(sig);
  return 0;
} 