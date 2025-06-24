#include "pve.h"

#include <memory>
#include <string>
#include <vector>
#include <map>

#include <cbmpc/core/buf.h>
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_pki.h>
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

// ============================================================================
// Helper – generate a random EC private key on P-256
// ============================================================================
static crypto::ecc_prv_key_t generate_prv_key() {
  crypto::ecc_prv_key_t prv_key_ecc;
  prv_key_ecc.generate(crypto::curve_p256);
  return prv_key_ecc;
}

// ============================================================================
// Key-pair generation helpers
// ============================================================================
int get_n_enc_keypairs(int n, cmems_t* prv_keys_ptr, cmems_t* pub_keys_ptr) {
  std::vector<buf_t> prv_keys(n);
  std::vector<buf_t> pub_keys(n);
  for (int i = 0; i < n; i++) {
    crypto::ecc_prv_key_t prv_key = generate_prv_key();
    prv_keys[i] = coinbase::ser(prv_key);
    pub_keys[i] = coinbase::ser(prv_key.pub());
  }
  *prv_keys_ptr = coinbase::mems_t(prv_keys).to_cmems();
  *pub_keys_ptr = coinbase::mems_t(pub_keys).to_cmems();
  return SUCCESS;
}

int get_n_ec_keypairs(int n, cmems_t* prv_keys_ptr, cmems_t* pub_keys_ptr) {
  ecurve_t curve = crypto::curve_p256;
  mod_t q = curve.order();
  const ecc_generator_point_t& G = curve.generator();

  std::vector<buf_t> xs(n);
  std::vector<buf_t> Xs(n);
  for (int i = 0; i < n; i++) {
    bn_t x = bn_t::rand(q);
    xs[i] = coinbase::ser(x);
    Xs[i] = coinbase::ser(x * G);
  }
  *prv_keys_ptr = coinbase::mems_t(xs).to_cmems();
  *pub_keys_ptr = coinbase::mems_t(Xs).to_cmems();
  return SUCCESS;
}

// ============================================================================
// Base Encryption Key Pair – single pair generation
// ============================================================================
int generate_base_enc_keypair(cmem_t* prv_key_ptr, cmem_t* pub_key_ptr) {
  try {
    // Generate EC private key on P-256
    crypto::ecc_prv_key_t ecc_prv;
    ecc_prv.generate(crypto::curve_p256);

    // Serialize
    buf_t prv_buf = coinbase::ser(ecc_prv);
    buf_t pub_buf = coinbase::ser(ecc_prv.pub());

    *prv_key_ptr = prv_buf.to_cmem();
    *pub_key_ptr = pub_buf.to_cmem();

    return SUCCESS;
  } catch (const std::exception& ex) {
    return coinbase::error(E_CRYPTO, ex.what());
  }
}

// ============================================================================
// PVE – quorum encrypt (AccessStructure pointer API)
// =========================================================================
int pve_quorum_encrypt_map(crypto_ss_ac_ref* ac_ptr, cmems_t names_list_ptr, cmems_t pub_keys_list_ptr,
                              int pub_keys_count, cmems_t xs_list_ptr, int xs_count, const char* label_ptr,
                              int curve_code, cmem_t* out_ptr) {
  if (ac_ptr == nullptr || ac_ptr->opaque == nullptr) {
    return coinbase::error(E_CRYPTO, "null access-structure pointer");
  }

  error_t rv = UNINITIALIZED_ERROR;
  crypto::ss::ac_t* ac = static_cast<crypto::ss::ac_t*>(ac_ptr->opaque);
  crypto::ss::node_t* root = const_cast<node_t*>(ac->root);

  // Deserialize names
  std::vector<buf_t> name_bufs = coinbase::mems_t(names_list_ptr).bufs();
  if (name_bufs.size() != (size_t)pub_keys_count) {
    return coinbase::error(E_CRYPTO, "names list and key list size mismatch");
  }
  std::vector<std::string> names(pub_keys_count);
  for (int i = 0; i < pub_keys_count; i++) {
    names[i] = std::string((const char*)name_bufs[i].data(), name_bufs[i].size());
  }

  // Deserialize public keys
  std::vector<buf_t> pub_bufs = coinbase::mems_t(pub_keys_list_ptr).bufs();
  std::vector<crypto::ecc_pub_key_t> pub_keys_list(pub_keys_count);
  for (int i = 0; i < pub_keys_count; i++) {
    crypto::ecc_pub_key_t pk;
    rv = coinbase::deser(pub_bufs[i], pk);
    if (rv) return rv;
    pub_keys_list[i] = pk;
  }

  // Deserialize xs
  std::vector<buf_t> xs_bufs = coinbase::mems_t(xs_list_ptr).bufs();
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
  std::map<std::string, crypto::ecc_pub_key_t> pub_keys;
  for (size_t i = 0; i < leaves.size(); ++i) {
    pub_keys[names[i]] = pub_keys_list[i];
  }

  // Encrypt
  ec_pve_ac_t<ecies_t> pve;
  pve.encrypt(ac_owned, pub_keys, std::string(label_ptr), curve, xs);
  buf_t out = coinbase::convert(pve);
  *out_ptr = out.to_cmem();
  return SUCCESS;
}

int pve_quorum_decrypt(crypto_ss_node_ref* root_ptr, cmems_t quorum_prv_keys_list_ptr, int quorum_prv_keys_count,
                       cmems_t all_pub_keys_list_ptr, int all_pub_keys_count, cmem_t pve_bundle_cmem,
                       cmems_t Xs_list_ptr, int xs_count, const char* label_ptr, cmems_t* out_ptr) {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::ss::node_t* root = static_cast<crypto::ss::node_t*>(root_ptr->opaque);

  // Deserialize quorum private keys
  std::vector<buf_t> qprv_bufs = coinbase::mems_t(quorum_prv_keys_list_ptr).bufs();
  std::vector<crypto::ecc_prv_key_t> quorum_prv_keys(quorum_prv_keys_count);
  for (int i = 0; i < quorum_prv_keys_count; i++) {
    rv = coinbase::deser(qprv_bufs[i], quorum_prv_keys[i]);
    if (rv) return rv;
  }

  // Deserialize all public keys
  std::vector<buf_t> pub_bufs = coinbase::mems_t(all_pub_keys_list_ptr).bufs();
  std::vector<crypto::ecc_pub_key_t> all_pub_keys(all_pub_keys_count);
  for (int i = 0; i < all_pub_keys_count; i++) {
    rv = coinbase::deser(pub_bufs[i], all_pub_keys[i]);
    if (rv) return rv;
  }

  // Deserialize Xs (points)
  std::vector<buf_t> Xs_bufs = coinbase::mems_t(Xs_list_ptr).bufs();
  std::vector<ecc_point_t> Xs(xs_count);
  for (int i = 0; i < xs_count; i++) {
    rv = coinbase::deser(Xs_bufs[i], Xs[i]);
    if (rv) return rv;
  }

  // Deserialize the PVE bundle
  ec_pve_ac_t<ecies_t> pve;
  buf_t pve_bundle = coinbase::mem_t(pve_bundle_cmem);
  rv = coinbase::deser(pve_bundle, pve);
  if (rv) return rv;

  ss::ac_owned_t ac(root);
  auto leaf_set = ac.list_leaf_names();
  std::vector<std::string> leaves(leaf_set.begin(), leaf_set.end());

  std::map<std::string, crypto::ecc_pub_key_t> pub_keys;
  std::map<std::string, crypto::ecc_prv_key_t> quorum_prv_map;
  int idx = 0;
  for (auto path : leaves) {
    quorum_prv_map[path] = quorum_prv_keys[idx];
    pub_keys[path] = all_pub_keys[idx];
    idx++;
  }

  std::string label(label_ptr);
  rv = pve.verify(ac, pub_keys, Xs, label);
  if (rv) return rv;

  std::vector<bn_t> decrypted_xs;
  rv = pve.decrypt(ac, quorum_prv_map, pub_keys, label, decrypted_xs, true /*skip_verify*/);
  if (rv) return rv;

  std::vector<buf_t> out(xs_count);
  for (int i = 0; i < xs_count; i++) {
    out[i] = coinbase::ser(decrypted_xs[i]);
  }
  *out_ptr = coinbase::mems_t(out).to_cmems();
  return SUCCESS;
}

// ============================================================================
// PVE – quorum decrypt (AccessStructure pointer API)
// =========================================================================
int pve_quorum_decrypt_map(crypto_ss_ac_ref* ac_ptr, cmems_t quorum_prv_keys_list_ptr, int quorum_prv_keys_count,
                           cmems_t all_pub_keys_list_ptr, int all_pub_keys_count, cmem_t pve_bundle_cmem,
                           cmems_t Xs_list_ptr, int xs_count, const char* label_ptr, cmems_t* out_ptr) {
  if (ac_ptr == nullptr || ac_ptr->opaque == nullptr) {
    return coinbase::error(E_CRYPTO, "null access-structure pointer");
  }

  crypto::ss::ac_t* ac = static_cast<crypto::ss::ac_t*>(ac_ptr->opaque);
  crypto_ss_node_ref root_ref{reinterpret_cast<void*>(const_cast<node_t*>(ac->root))};
  return pve_quorum_decrypt(&root_ref, quorum_prv_keys_list_ptr, quorum_prv_keys_count,
                            all_pub_keys_list_ptr, all_pub_keys_count, pve_bundle_cmem, Xs_list_ptr, xs_count, label_ptr, out_ptr);
}

// ============================================================================
// PVE – quorum verify (AccessStructure pointer API)
// =========================================================================
int pve_quorum_verify_map(crypto_ss_ac_ref* ac_ptr, cmems_t names_list_ptr, cmems_t pub_keys_list_ptr,
                          int pub_keys_count, cmem_t pve_bundle_cmem, cmems_t Xs_list_ptr, int xs_count,
                          const char* label_ptr) {
  if (ac_ptr == nullptr || ac_ptr->opaque == nullptr) {
    return coinbase::error(E_CRYPTO, "null access-structure pointer");
  }

  error_t rv = UNINITIALIZED_ERROR;
  crypto::ss::ac_t* ac = static_cast<crypto::ss::ac_t*>(ac_ptr->opaque);
  crypto::ss::node_t* root = const_cast<node_t*>(ac->root);

  // Deserialize names
  std::vector<buf_t> name_bufs = coinbase::mems_t(names_list_ptr).bufs();
  if (name_bufs.size() != (size_t)pub_keys_count) {
    return coinbase::error(E_CRYPTO, "names list and key list size mismatch");
  }
  std::vector<std::string> names(pub_keys_count);
  for (int i = 0; i < pub_keys_count; i++) {
    names[i] = std::string((const char*)name_bufs[i].data(), name_bufs[i].size());
  }

  // Deserialize public keys
  std::vector<buf_t> pub_bufs = coinbase::mems_t(pub_keys_list_ptr).bufs();
  std::vector<crypto::ecc_pub_key_t> pub_keys_list(pub_keys_count);
  for (int i = 0; i < pub_keys_count; i++) {
    crypto::ecc_pub_key_t pk;
    rv = coinbase::deser(pub_bufs[i], pk);
    if (rv) return rv;
    pub_keys_list[i] = pk;
  }

  // Deserialize Xs (public shares)
  std::vector<buf_t> Xs_bufs = coinbase::mems_t(Xs_list_ptr).bufs();
  std::vector<ecc_point_t> Xs(xs_count);
  for (int i = 0; i < xs_count; i++) {
    rv = coinbase::deser(Xs_bufs[i], Xs[i]);
    if (rv) return rv;
  }

  // Deserialize the PVE bundle
  ec_pve_ac_t<ecies_t> pve;
  buf_t pve_bundle = coinbase::mem_t(pve_bundle_cmem);
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
  std::map<std::string, crypto::ecc_pub_key_t> pub_keys;
  for (size_t i = 0; i < leaves.size(); ++i) {
    pub_keys[names[i]] = pub_keys_list[i];
  }

  // Perform verification
  std::string label(label_ptr);
  rv = pve.verify(*ac, pub_keys, Xs, label);
  if (rv) return rv;

  return SUCCESS;
}