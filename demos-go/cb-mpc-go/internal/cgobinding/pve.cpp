#include "pve.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <cbmpc/core/buf.h>
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_pki.h>
#include <cbmpc/ffi/pki.h>
#include <cbmpc/ffi/cmem_adapter.h>
#include <cbmpc/crypto/secret_sharing.h>
#include <cbmpc/protocol/ec_dkg.h>
#include <cbmpc/protocol/mpc_job_session.h>
#include <cbmpc/protocol/pve.h>
#include <cbmpc/protocol/pve_ac.h>
#include <cbmpc/zk/zk_ec.h>

#include "curve.h"
#include "network.h"

using namespace coinbase;
using namespace coinbase::crypto;
using namespace coinbase::mpc;
using node_t = coinbase::crypto::ss::node_t;
using node_e = coinbase::crypto::ss::node_e;

static thread_local void* g_ctx = nullptr;

// KEM
static kem_encap_ctx_fn g_kem_enc = nullptr;
static kem_decap_ctx_fn g_kem_dec = nullptr;
static kem_dk_to_ek_ctx_fn g_kem_derive_pub = nullptr;

// KEM registration and stub shims (third arg kept for backward compat but ignored)
void pve_register_kem_functions(kem_encap_ctx_fn e, kem_decap_ctx_fn d, void* /*ignored*/, kem_dk_to_ek_ctx_fn dpub) {
  g_kem_enc = e;
  g_kem_dec = d;
  g_kem_derive_pub = dpub;
}

static int stub_kem_encapsulate(cmem_t ek, cmem_t rho, cmem_t* ct_out, cmem_t* ss_out) {
  if (g_kem_enc == nullptr || g_ctx == nullptr) return 1;
  return g_kem_enc(g_ctx, ek, rho, ct_out, ss_out);
}

static int stub_kem_decapsulate(const void* dk, cmem_t ct, cmem_t* ss_out) {
  if (g_kem_dec == nullptr || g_ctx == nullptr) return 1;
  return g_kem_dec(g_ctx, dk, ct, ss_out);
}

static int stub_kem_dk_to_ek(const void* dk, cmem_t* out) {
  if (g_kem_derive_pub == nullptr || g_ctx == nullptr) return 1;
  return g_kem_derive_pub(g_ctx, dk, out);
}

ffi_kem_encap_fn get_ffi_kem_encap_fn(void) { return stub_kem_encapsulate; }
ffi_kem_decap_fn get_ffi_kem_decap_fn(void) { return stub_kem_decapsulate; }
ffi_kem_dk_to_ek_fn get_ffi_kem_dk_to_ek_fn(void) { return stub_kem_dk_to_ek; }

// ============================================================================
// PVE – single receiver, single value
// ============================================================================

int pve_encrypt(cmem_t pub_key_cmem, cmem_t x_cmem, const char* label_ptr, int curve_code, cmem_t* out_ptr) {
  if (label_ptr == nullptr || out_ptr == nullptr) {
    return coinbase::error(E_BADARG);
  }
  error_t rv = UNINITIALIZED_ERROR;

  // Wrap public key bytes into FFI PKI key type (opaque buffer).
  ffi_kem_ek_t pub_key;
  pub_key = coinbase::ffi::view(pub_key_cmem);

  // Deserialize secret scalar x
  bn_t x = bn_t::from_bin(coinbase::ffi::view(x_cmem));

  // Resolve curve
  ecurve_t curve = ecurve_t::find(curve_code);
  if (!curve) return coinbase::error(E_CRYPTO, "unsupported curve code");

  // Perform encryption
  ec_pve_t pve(mpc::kem_pve_base_pke<coinbase::crypto::kem_policy_ffi_t>());
  try {
    pve.encrypt(&pub_key, std::string(label_ptr), curve, x);
  } catch (const std::exception& ex) {
    return coinbase::error(E_CRYPTO, ex.what());
  }

  buf_t out = coinbase::convert(pve);
  *out_ptr = coinbase::ffi::copy_to_cmem(out);
  return SUCCESS;
}

int pve_decrypt(cmem_t prv_key_cmem, cmem_t pve_bundle_cmem, const char* label_ptr, int curve_code, cmem_t* out_x_ptr) {
  if (label_ptr == nullptr || out_x_ptr == nullptr) {
    return coinbase::error(E_BADARG);
  }
  error_t rv = UNINITIALIZED_ERROR;

  // The dk can be either raw bytes or a handle encoded as bytes.
  // We pass it through as an opaque handle pointer by default. For pure
  // byte-backed dk, we pass a pointer to the cmem_t on the stack whose
  // lifetime spans the call chain.
  ffi_kem_dk_t prv_key;
  cmem_t dk_bytes = prv_key_cmem;
  prv_key.handle = static_cast<void*>(&dk_bytes);

  // Deserialize ciphertext bundle
  ec_pve_t pve(mpc::kem_pve_base_pke<coinbase::crypto::kem_policy_ffi_t>());
  rv = coinbase::deser(coinbase::ffi::view(pve_bundle_cmem), pve);
  if (rv) return rv;

  // Resolve curve
  ecurve_t curve = ecurve_t::find(curve_code);
  if (!curve) return coinbase::error(E_CRYPTO, "unsupported curve code");

  // Decrypt
  bn_t x_out;
  rv = pve.decrypt(&prv_key, nullptr /*unused ek*/, std::string(label_ptr), curve, x_out, /*skip_verify=*/true);
  if (rv) return rv;

  buf_t x_buf = x_out.to_bin(curve.order().get_bin_size());
  *out_x_ptr = coinbase::ffi::copy_to_cmem(x_buf);
  return SUCCESS;
}

int pve_verify(cmem_t pub_key_cmem, cmem_t pve_bundle_cmem, cmem_t Q_cmem, const char* label_ptr) {
  if (label_ptr == nullptr) {
    return coinbase::error(E_BADARG);
  }
  error_t rv = UNINITIALIZED_ERROR;

  // Deserialize inputs
  ffi_kem_ek_t pub_key;
  pub_key = coinbase::ffi::view(pub_key_cmem);

  ecc_point_t Q;
  rv = coinbase::deser(coinbase::ffi::view(Q_cmem), Q);
  if (rv) return rv;

  ec_pve_t pve(mpc::kem_pve_base_pke<coinbase::crypto::kem_policy_ffi_t>());
  rv = coinbase::deser(coinbase::ffi::view(pve_bundle_cmem), pve);
  if (rv) return rv;

  // Verify
  rv = pve.verify(&pub_key, Q, std::string(label_ptr));
  if (rv) return rv;

  return SUCCESS;
}

// No explicit template instantiation needed; ec_pve_ac_t is non-templated.

// ============================================================================
// PVE-AC - many receivers, many values
// =========================================================================
int pve_ac_encrypt(crypto_ss_ac_ref* ac_ptr, cmems_t names_list_ptr, cmems_t pub_keys_list_ptr, int pub_keys_count,
                   cmems_t xs_list_ptr, int xs_count, const char* label_ptr, int curve_code, cmem_t* out_ptr) {
  if (ac_ptr == nullptr || ac_ptr->opaque == nullptr) {
    return coinbase::error(E_CRYPTO, "null access-structure pointer");
  }

  error_t rv = UNINITIALIZED_ERROR;
  crypto::ss::ac_t* ac = static_cast<crypto::ss::ac_t*>(ac_ptr->opaque);
  crypto::ss::node_t* root = const_cast<node_t*>(ac->root);

  // Deserialize names
  std::vector<buf_t> name_bufs = coinbase::ffi::bufs_from_cmems(names_list_ptr);
  if (name_bufs.size() != (size_t)pub_keys_count) {
    return coinbase::error(E_CRYPTO, "names list and key list size mismatch");
  }
  std::vector<std::string> names(pub_keys_count);
  for (int i = 0; i < pub_keys_count; i++) {
    names[i] = std::string((const char*)name_bufs[i].data(), name_bufs[i].size());
  }

  // Deserialize public keys (opaque FFI KEM ek)
  std::vector<buf_t> pub_bufs = coinbase::ffi::bufs_from_cmems(pub_keys_list_ptr);
  std::vector<crypto::ffi_kem_ek_t> pub_keys_list(pub_keys_count);
  for (int i = 0; i < pub_keys_count; i++) {
    pub_keys_list[i] = pub_bufs[i];
  }

  // Deserialize xs
  std::vector<buf_t> xs_bufs = coinbase::ffi::bufs_from_cmems(xs_list_ptr);
  std::vector<bn_t> xs(xs_count);
  for (int i = 0; i < xs_count; i++) {
    xs[i] = bn_t::from_bin(xs_bufs[i]);
  }

  // Resolve curve
  ecurve_t curve = ecurve_t::find(curve_code);
  if (!curve) return coinbase::error(E_CRYPTO, "unsupported curve code");

  // Validate inputs
  if (xs.empty()) {
    return coinbase::error(E_CRYPTO, "empty xs list");
  }
  if (pub_keys_list.empty()) {
    return coinbase::error(E_CRYPTO, "empty public keys list");
  }

  // Build access structure and get leaf names
  ss::ac_owned_t ac_owned(root);
  auto leaf_set = ac_owned.list_leaf_names();
  std::vector<std::string> leaves(leaf_set.begin(), leaf_set.end());

  if (names.size() != pub_keys_list.size()) {
    return coinbase::error(E_CRYPTO, "names list and key list size mismatch");
  }
  if (pub_keys_list.size() != leaves.size()) {
    return coinbase::error(E_CRYPTO, "leaf count and key list size mismatch");
  }

  // Build the mapping leaf_name -> pub_key
  std::map<std::string, crypto::ffi_kem_ek_t> pub_keys;
  std::vector<crypto::ffi_kem_ek_t> pub_keys_storage(leaves.size());
  for (size_t i = 0; i < leaves.size(); ++i) {
    pub_keys_storage[i] = pub_keys_list[i];
    pub_keys[names[i]] = pub_keys_storage[i];
  }

  // Encrypt using FFI KEM base PKE
  ec_pve_ac_t pve(mpc::kem_pve_base_pke<coinbase::crypto::kem_policy_ffi_t>());
  std::map<std::string, const void*> ac_pks;
  for (size_t i = 0; i < leaves.size(); ++i) {
    ac_pks[names[i]] = static_cast<const void*>(&pub_keys_storage[i]);
  }
  pve.encrypt(ac_owned, ac_pks, std::string(label_ptr), curve, xs);
  buf_t out = coinbase::convert(pve);
  *out_ptr = coinbase::ffi::copy_to_cmem(out);
  return SUCCESS;
}

extern "C" int pve_ac_party_decrypt_row(crypto_ss_ac_ref* ac_ptr, cmem_t prv_key_cmem, cmem_t pve_bundle_cmem,
                            const char* label_ptr, const char* path_ptr, int row_index, cmem_t* out_share_ptr) {
  if (ac_ptr == nullptr || ac_ptr->opaque == nullptr) {
    return coinbase::error(E_CRYPTO, "null access-structure pointer");
  }

  // Deserialize PVE bundle
  ec_pve_ac_t pve(mpc::kem_pve_base_pke<coinbase::crypto::kem_policy_ffi_t>());
  error_t rv = coinbase::deser(coinbase::ffi::view(pve_bundle_cmem), pve);
  if (rv) return rv;

  // Access structure
  crypto::ss::ac_t* ac = static_cast<crypto::ss::ac_t*>(ac_ptr->opaque);
  ss::ac_owned_t ac_owned(const_cast<node_t*>(ac->root));

  // Prepare DK handle wrapper for FFI KEM
  ffi_kem_dk_t prv_key;
  cmem_t dk_bytes = prv_key_cmem;
  prv_key.handle = static_cast<void*>(&dk_bytes);

  // Compute share
  bn_t share;
  rv = pve.party_decrypt_row(ac_owned, row_index, std::string(path_ptr), static_cast<const void*>(&prv_key),
                             std::string(label_ptr), share);
  if (rv) return rv;

  buf_t share_buf = share.to_bin();
  *out_share_ptr = coinbase::ffi::copy_to_cmem(share_buf);
  return SUCCESS;
}

extern "C" int pve_ac_aggregate_to_restore_row(crypto_ss_ac_ref* ac_ptr, cmem_t pve_bundle_cmem, const char* label_ptr,
                                    cmems_t paths_list_ptr, cmems_t shares_list_ptr, int quorum_count, int row_index,
                                    cmems_t* out_values_ptr) {
  if (ac_ptr == nullptr || ac_ptr->opaque == nullptr) {
    return coinbase::error(E_CRYPTO, "null access-structure pointer");
  }

  // Deserialize PVE bundle
  ec_pve_ac_t pve(mpc::kem_pve_base_pke<coinbase::crypto::kem_policy_ffi_t>());
  error_t rv = coinbase::deser(coinbase::ffi::view(pve_bundle_cmem), pve);
  if (rv) return rv;

  // Access structure
  crypto::ss::ac_t* ac = static_cast<crypto::ss::ac_t*>(ac_ptr->opaque);
  ss::ac_owned_t ac_owned(const_cast<node_t*>(ac->root));

  // Build quorum shares map: path -> bn share
  std::vector<buf_t> name_bufs = coinbase::ffi::bufs_from_cmems(paths_list_ptr);
  std::vector<buf_t> share_bufs = coinbase::ffi::bufs_from_cmems(shares_list_ptr);
  if ((int)name_bufs.size() != quorum_count || (int)share_bufs.size() != quorum_count) {
    return coinbase::error(E_CRYPTO, "quorum lists size mismatch");
  }
  std::map<std::string, bn_t> quorum_decrypted;
  for (int i = 0; i < quorum_count; i++) {
    std::string path((const char*)name_bufs[i].data(), name_bufs[i].size());
    quorum_decrypted[path] = bn_t::from_bin(share_bufs[i]);
  }

  // Recover values for the specified row
  std::vector<bn_t> x;
  rv = pve.aggregate_to_restore_row(ac_owned, row_index, std::string(label_ptr), quorum_decrypted, x,
                                    true /*skip_verify*/);
  if (rv) return rv;

  // Serialize outputs to fixed-size bins
  const std::vector<ecc_point_t>& Q = pve.get_Q();
  if (Q.empty()) return coinbase::error(E_CRYPTO, "empty Q");
  ecurve_t curve = Q[0].get_curve();
  int fixed_size = curve.order().get_bin_size();
  std::vector<buf_t> out(x.size());
  for (size_t i = 0; i < x.size(); i++) out[i] = x[i].to_bin(fixed_size);
  *out_values_ptr = coinbase::ffi::copy_to_cmems(buf_t::to_mems(out));
  return SUCCESS;
}

int pve_ac_verify(crypto_ss_ac_ref* ac_ptr, cmems_t names_list_ptr, cmems_t pub_keys_list_ptr, int pub_keys_count,
                  cmem_t pve_bundle_cmem, cmems_t Xs_list_ptr, int xs_count, const char* label_ptr) {
  if (ac_ptr == nullptr || ac_ptr->opaque == nullptr) {
    return coinbase::error(E_CRYPTO, "null access-structure pointer");
  }

  error_t rv = UNINITIALIZED_ERROR;
  crypto::ss::ac_t* ac = static_cast<crypto::ss::ac_t*>(ac_ptr->opaque);
  crypto::ss::node_t* root = const_cast<node_t*>(ac->root);

  // Deserialize names
  std::vector<buf_t> name_bufs = coinbase::ffi::bufs_from_cmems(names_list_ptr);
  if (name_bufs.size() != (size_t)pub_keys_count) {
    return coinbase::error(E_CRYPTO, "names list and key list size mismatch");
  }
  std::vector<std::string> names(pub_keys_count);
  for (int i = 0; i < pub_keys_count; i++) {
    names[i] = std::string((const char*)name_bufs[i].data(), name_bufs[i].size());
  }

  // Deserialize public keys (opaque FFI KEM ek)
  std::vector<buf_t> pub_bufs = coinbase::ffi::bufs_from_cmems(pub_keys_list_ptr);
  std::vector<crypto::ffi_kem_ek_t> pub_keys_list(pub_keys_count);
  for (int i = 0; i < pub_keys_count; i++) {
    pub_keys_list[i] = pub_bufs[i];
  }

  // Deserialize Xs (public shares)
  std::vector<buf_t> Xs_bufs = coinbase::ffi::bufs_from_cmems(Xs_list_ptr);
  std::vector<ecc_point_t> Xs(xs_count);
  for (int i = 0; i < xs_count; i++) {
    rv = coinbase::deser(Xs_bufs[i], Xs[i]);
    if (rv) return rv;
  }

  // Deserialize the PVE bundle
  ec_pve_ac_t pve(mpc::kem_pve_base_pke<coinbase::crypto::kem_policy_ffi_t>());
  buf_t pve_bundle = coinbase::ffi::view(pve_bundle_cmem);
  rv = coinbase::deser(pve_bundle, pve);
  if (rv) return rv;

  // Build leaf names from access structure
  ss::ac_owned_t ac_owned(root);
  auto leaf_set = ac_owned.list_leaf_names();
  std::vector<std::string> leaves(leaf_set.begin(), leaf_set.end());
  if (leaves.size() != names.size()) {
    return coinbase::error(E_CRYPTO, "leaf count and names list size mismatch");
  }

  // Build mapping leaf_name -> pub_key
  std::map<std::string, crypto::ffi_kem_ek_t> pub_keys;
  for (size_t i = 0; i < leaves.size(); ++i) {
    pub_keys[names[i]] = pub_keys_list[i];
  }

  // Perform verification
  std::string label(label_ptr);
  std::vector<crypto::ffi_kem_ek_t> pub_keys_storage(leaves.size());
  std::map<std::string, const void*> ac_pks;
  for (size_t i = 0; i < leaves.size(); ++i) {
    pub_keys_storage[i] = pub_keys[names[i]];
    ac_pks[names[i]] = static_cast<const void*>(&pub_keys_storage[i]);
  }
  rv = pve.verify(*ac, ac_pks, Xs, label);
  if (rv) return rv;

  return SUCCESS;
}

extern "C" void pve_activate_ctx(void* ctx) { g_ctx = ctx; }
