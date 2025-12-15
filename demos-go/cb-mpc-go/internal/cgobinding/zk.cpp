#include "zk.h"

#include <cbmpc/zk/zk_ec.h>
#include <cbmpc/ffi/cmem_adapter.h>

int zk_dl_prove(ecc_point_ref* Q_ref, cmem_t w_mem, cmem_t sid_mem, uint64_t aux, cmem_t* proof_mem) {
  // Deserialize inputs
  ecc_point_t* Q = static_cast<ecc_point_t*>(Q_ref->opaque);
  buf_t sid = coinbase::ffi::view(sid_mem);
  bn_t w = bn_t::from_bin(coinbase::ffi::view(w_mem));

  // Prove
  coinbase::zk::uc_dl_t zk;
  zk.prove(*Q, w, sid, aux);

  // Serialize proof
  buf_t proof = coinbase::ser(zk);
  *proof_mem = coinbase::ffi::copy_to_cmem(proof);

  return SUCCESS;
}

int zk_dl_verify(ecc_point_ref* Q_ref, cmem_t proof_mem, cmem_t sid_mem, uint64_t aux) {
  // Deserialize inputs
  ecc_point_t* Q = static_cast<ecc_point_t*>(Q_ref->opaque);
  coinbase::zk::uc_dl_t zk;
  buf_t sid = coinbase::ffi::view(sid_mem);

  error_t rv = coinbase::deser(coinbase::ffi::view(proof_mem), zk);
  if (rv != SUCCESS) return rv;

  return zk.verify(*Q, sid, aux);
}
