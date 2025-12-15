// eddsamp.cpp – Signing-only bindings for EdDSA multi-party

#include "eddsamp.h"

#include <memory>

#include <cbmpc/core/buf.h>
#include <cbmpc/protocol/eddsa.h>
#include <cbmpc/protocol/mpc_job_session.h>
#include <cbmpc/ffi/cmem_adapter.h>

#include "curve.h"
#include "network.h"

using namespace coinbase;
using namespace coinbase::mpc;

// -----------------------------------------------------------------------------
// EdDSA-MPC signing helper
// -----------------------------------------------------------------------------

int mpc_eddsampc_sign(job_mp_ref* j, mpc_eckey_mp_ref* k, cmem_t msg_mem, int sig_receiver, cmem_t* sig_mem) {
  job_mp_t* job = static_cast<job_mp_t*>(j->opaque);
  eddsampc::key_t* key = static_cast<eddsampc::key_t*>(k->opaque);

  buf_t msg = coinbase::ffi::view(msg_mem);
  buf_t sig;
  error_t err = eddsampc::sign(*job, *key, msg, party_idx_t(sig_receiver), sig);
  if (err) return err;
  *sig_mem = coinbase::ffi::copy_to_cmem(sig);
  return 0;
} 