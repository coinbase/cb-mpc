#include "eckeymp.h"

#include <memory>

#include <cbmpc/core/buf.h>
#include <cbmpc/crypto/base.h>
#include <cbmpc/protocol/ec_dkg.h>
#include <cbmpc/protocol/mpc_job_session.h>
#include <cbmpc/ffi/cmem_adapter.h>

#include "curve.h"
#include "network.h"

using namespace coinbase;
using namespace coinbase::crypto;
using namespace coinbase::mpc;

// ------------------------- Memory helpers ---------------------------
void free_mpc_eckey_mp(mpc_eckey_mp_ref ctx) {
  if (ctx.opaque) {
    delete static_cast<eckey::key_share_mp_t*>(ctx.opaque);
  }
}

// --------------------------- Field accessors -----------------------
int mpc_eckey_mp_get_party_name(mpc_eckey_mp_ref* k, cmem_t* party_name_mem) {
  if (k == nullptr || k->opaque == nullptr) {
    return 1;  // Invalid key reference
  }

  eckey::key_share_mp_t* key = static_cast<eckey::key_share_mp_t*>(k->opaque);
  *party_name_mem = coinbase::ffi::copy_to_cmem(coinbase::mem_t(key->party_name));
  return 0;
}

int mpc_eckey_mp_get_x_share(mpc_eckey_mp_ref* k, cmem_t* x_share_mem) {
  if (k == nullptr || k->opaque == nullptr) {
    return 1;  // Invalid key reference
  }
  eckey::key_share_mp_t* key = static_cast<eckey::key_share_mp_t*>(k->opaque);
  buf_t x_buf = key->x_share.to_bin(key->curve.order().get_bin_size());
  *x_share_mem = coinbase::ffi::copy_to_cmem(x_buf);
  return 0;
}

ecc_point_ref mpc_eckey_mp_get_Q(mpc_eckey_mp_ref* k) {
  if (k == nullptr || k->opaque == nullptr) {
    return ecc_point_ref{nullptr};
  }
  eckey::key_share_mp_t* key = static_cast<eckey::key_share_mp_t*>(k->opaque);
  ecc_point_t* Q_copy = new ecc_point_t(key->Q);
  return ecc_point_ref{Q_copy};
}

ecurve_ref mpc_eckey_mp_get_curve(mpc_eckey_mp_ref* k) {
  if (k == nullptr || k->opaque == nullptr) {
    return ecurve_ref{nullptr};
  }
  eckey::key_share_mp_t* key = static_cast<eckey::key_share_mp_t*>(k->opaque);
  // Allocate a copy so the caller can own it independently.
  ecurve_t* curve_copy = new ecurve_t(key->curve);
  return ecurve_ref{curve_copy};
}

int mpc_eckey_mp_get_Qis(mpc_eckey_mp_ref* k, cmems_t* party_names_mem, cmems_t* points_mem) {
  if (k == nullptr || k->opaque == nullptr) {
    return 1;  // Invalid key reference
  }
  eckey::key_share_mp_t* key = static_cast<eckey::key_share_mp_t*>(k->opaque);

  std::vector<coinbase::buf_t> name_bufs;
  std::vector<coinbase::buf_t> point_bufs;
  name_bufs.reserve(key->Qis.size());
  point_bufs.reserve(key->Qis.size());

  for (const auto& kv : key->Qis) {
    name_bufs.emplace_back(coinbase::mem_t(kv.first));
    point_bufs.push_back(coinbase::ser(kv.second));
  }

  *party_names_mem = coinbase::ffi::copy_to_cmems(buf_t::to_mems(name_bufs));
  *points_mem = coinbase::ffi::copy_to_cmems(buf_t::to_mems(point_bufs));
  return 0;
}

// ------------------------- Protocols -----------------------------------------
int mpc_eckey_mp_dkg(job_mp_ref* j, ecurve_ref* curve_ref, mpc_eckey_mp_ref* k) {
  job_mp_t* job = static_cast<job_mp_t*>(j->opaque);
  ecurve_t* curve_ptr = static_cast<ecurve_t*>(curve_ref->opaque);
  if (curve_ptr == nullptr) {
    return 1;  // Invalid curve reference
  }

  // Allocate key on the heap – ensure we release it on failure to avoid leaks.
  std::unique_ptr<eckey::key_share_mp_t> key(new eckey::key_share_mp_t());

  buf_t sid;
  error_t err = eckey::key_share_mp_t::dkg(*job, *curve_ptr, *key, sid);
  if (err) {
    return err;  // unique_ptr automatically frees memory
  }

  // Transfer ownership to the caller – release smart pointer so object lives on.
  *k = mpc_eckey_mp_ref{key.release()};
  return 0;
}

int mpc_eckey_mp_refresh(job_mp_ref* j, cmem_t sid_mem, mpc_eckey_mp_ref* k, mpc_eckey_mp_ref* nk) {
  job_mp_t* job = static_cast<job_mp_t*>(j->opaque);
  eckey::key_share_mp_t* key = static_cast<eckey::key_share_mp_t*>(k->opaque);

  // Allocate new key with automatic cleanup on error.
  std::unique_ptr<eckey::key_share_mp_t> new_key(new eckey::key_share_mp_t());

  buf_t sid = coinbase::ffi::view(sid_mem);
  error_t err = eckey::key_share_mp_t::refresh(*job, sid, *key, *new_key);
  if (err) {
    return err;  // unique_ptr frees memory
  }

  *nk = mpc_eckey_mp_ref{new_key.release()};
  return 0;
}

// ------------------- Threshold / Quorum helpers --------------------
int eckey_dkg_mp_threshold_dkg(job_mp_ref* job_ptr, ecurve_ref* curve_ref, cmem_t sid, crypto_ss_ac_ref* ac,
                               mpc_party_set_ref* quorum, mpc_eckey_mp_ref* key) {
  job_mp_t* job = static_cast<job_mp_t*>(job_ptr->opaque);
  ecurve_t* curve_ptr = static_cast<ecurve_t*>(curve_ref->opaque);
  if (curve_ptr == nullptr) {
    return 1;  // Invalid curve reference
  }

  buf_t sid_buf = coinbase::ffi::view(sid);
  crypto::ss::ac_t* ac_obj = static_cast<crypto::ss::ac_t*>(ac->opaque);
  party_set_t* quorum_set = static_cast<party_set_t*>(quorum->opaque);

  // Allocate key share with RAII – will auto free on early return.
  std::unique_ptr<eckey::key_share_mp_t> key_share(new eckey::key_share_mp_t());
  error_t err = eckey::key_share_mp_t::threshold_dkg(*job, *curve_ptr, sid_buf, *ac_obj, *quorum_set, *key_share);
  if (err) {
    return err;  // unique_ptr cleans up
  }

  *key = mpc_eckey_mp_ref{key_share.release()};
  return 0;
}

int eckey_key_share_mp_to_additive_share(mpc_eckey_mp_ref* key, crypto_ss_ac_ref* ac, cmems_t quorum_party_names,
                                         mpc_eckey_mp_ref* additive_key) {
  eckey::key_share_mp_t* key_share = static_cast<eckey::key_share_mp_t*>(key->opaque);
  crypto::ss::ac_t* ac_obj = static_cast<crypto::ss::ac_t*>(ac->opaque);

  std::vector<buf_t> name_bufs = coinbase::ffi::bufs_from_cmems(quorum_party_names);
  std::set<crypto::pname_t> quorum_names;
  for (const auto& name_buf : name_bufs) {
    quorum_names.insert(name_buf.to_string());
  }

  // Allocate additive share with RAII to avoid leaks on error.
  std::unique_ptr<eckey::key_share_mp_t> additive_share(new eckey::key_share_mp_t());
  error_t err = key_share->to_additive_share(*ac_obj, quorum_names, *additive_share);
  if (err) {
    return err;  // unique_ptr cleans up automatically
  }

  *additive_key = mpc_eckey_mp_ref{additive_share.release()};
  return 0;
}

// --------------------------- Utilities -----------------------------
int serialize_mpc_eckey_mp(mpc_eckey_mp_ref* k, cmems_t* ser) {
  eckey::key_share_mp_t* key = static_cast<eckey::key_share_mp_t*>(k->opaque);

  auto x = coinbase::ser(key->x_share);
  auto Q = coinbase::ser(key->Q);
  auto Qis = coinbase::ser(key->Qis);
  auto curve = coinbase::ser(key->curve);
  auto party_name = coinbase::ser(key->party_name);

  auto out = std::vector<mem_t>{x, Q, Qis, curve, party_name};
  *ser = coinbase::ffi::copy_to_cmems(out);
  return 0;
}

int deserialize_mpc_eckey_mp(cmems_t sers, mpc_eckey_mp_ref* k) {
  std::unique_ptr<eckey::key_share_mp_t> key(new eckey::key_share_mp_t());
  std::vector<buf_t> sers_vec = coinbase::ffi::bufs_from_cmems(sers);

  if (coinbase::deser(sers_vec[0], key->x_share)) return 1;
  if (coinbase::deser(sers_vec[1], key->Q)) return 1;
  if (coinbase::deser(sers_vec[2], key->Qis)) return 1;
  if (coinbase::deser(sers_vec[3], key->curve)) return 1;
  if (coinbase::deser(sers_vec[4], key->party_name)) return 1;

  *k = mpc_eckey_mp_ref{key.release()};
  return 0;
}