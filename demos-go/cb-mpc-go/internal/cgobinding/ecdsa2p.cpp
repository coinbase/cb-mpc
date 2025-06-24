#include "ecdsa2p.h"

#include <memory>

#include <cbmpc/core/buf.h>
#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/ecdsa_2p.h>
#include <cbmpc/protocol/mpc_job_session.h>

#include "curve.h"
#include "network.h"

using namespace coinbase;
using namespace coinbase::mpc;

int mpc_ecdsa2p_dkg(job_2p_ref* j, int curve_code, mpc_ecdsa2pc_key_ref* k) {
  job_2p_t* job = static_cast<job_2p_t*>(j->opaque);
  ecurve_t curve = ecurve_t::find(curve_code);

  ecdsa2pc::key_t* key = new ecdsa2pc::key_t();

  error_t err = ecdsa2pc::dkg(*job, curve, *key);
  if (err) return err;
  *k = mpc_ecdsa2pc_key_ref{key};

  return 0;
}

int mpc_ecdsa2p_refresh(job_2p_ref* j, mpc_ecdsa2pc_key_ref* k, mpc_ecdsa2pc_key_ref* nk) {
  job_2p_t* job = static_cast<job_2p_t*>(j->opaque);

  ecdsa2pc::key_t* key = static_cast<ecdsa2pc::key_t*>(k->opaque);
  ecdsa2pc::key_t* new_key = new ecdsa2pc::key_t();

  error_t err = ecdsa2pc::refresh(*job, *key, *new_key);
  if (err) return err;
  *nk = mpc_ecdsa2pc_key_ref{new_key};

  return 0;
}

int mpc_ecdsa2p_sign(job_2p_ref* j, cmem_t sid_mem, mpc_ecdsa2pc_key_ref* k, cmems_t msgs, cmems_t* sigs) {
  job_2p_t* job = static_cast<job_2p_t*>(j->opaque);
  ecdsa2pc::key_t* key = static_cast<ecdsa2pc::key_t*>(k->opaque);
  buf_t sid = mem_t(sid_mem);
  std::vector<mem_t> messages = coinbase::mems_t(msgs).mems();

  std::vector<buf_t> signatures;
  error_t err = ecdsa2pc::sign_batch(*job, sid, *key, messages, signatures);
  if (err) return err;
  *sigs = coinbase::mems_t(signatures).to_cmems();

  return 0;
}

// ============ Memory Management =================
void free_mpc_ecdsa2p_key(mpc_ecdsa2pc_key_ref ctx) {
  if (ctx.opaque) {
    delete static_cast<ecdsa2pc::key_t*>(ctx.opaque);
  }
}

// ============ Accessors =========================

int mpc_ecdsa2p_key_get_role_index(mpc_ecdsa2pc_key_ref* key) {
  if (key == NULL || key->opaque == NULL) {
    return -1;  // error: invalid key
  }
  ecdsa2pc::key_t* k = static_cast<ecdsa2pc::key_t*>(key->opaque);
  return static_cast<int>(k->role);
}

ecc_point_ref mpc_ecdsa2p_key_get_Q(mpc_ecdsa2pc_key_ref* key) {
  if (key == NULL || key->opaque == NULL) {
    return ecc_point_ref{nullptr};
  }
  ecdsa2pc::key_t* k = static_cast<ecdsa2pc::key_t*>(key->opaque);
  ecc_point_t* Q_copy = new ecc_point_t(k->Q);  // deep copy
  return ecc_point_ref{Q_copy};
}

cmem_t mpc_ecdsa2p_key_get_x_share(mpc_ecdsa2pc_key_ref* key) {
  if (key == NULL || key->opaque == NULL) {
    return cmem_t{nullptr, 0};
  }
  ecdsa2pc::key_t* k = static_cast<ecdsa2pc::key_t*>(key->opaque);
  // Serialize bn_t to bytes (minimal length) preserving order size
  int bin_size = std::max(k->x_share.get_bin_size(), k->curve.order().get_bin_size());
  buf_t x_buf = k->x_share.to_bin(bin_size);
  return x_buf.to_cmem();
}

int mpc_ecdsa2p_key_get_curve_code(mpc_ecdsa2pc_key_ref* key) {
  if (key == NULL || key->opaque == NULL) {
    return -1;
  }
  ecdsa2pc::key_t* k = static_cast<ecdsa2pc::key_t*>(key->opaque);
  return k->curve.get_openssl_code();
}