#include "cblib.h"

#include <memory>

#include <cbmpc/core/buf.h>
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/secret_sharing.h>
#include <cbmpc/protocol/ecdsa_2p.h>
#include <cbmpc/protocol/ecdsa_mp.h>
#include <cbmpc/protocol/mpc_job_session.h>
#include <cbmpc/protocol/pve.h>
#include <cbmpc/protocol/pve_ac.h>
#include <cbmpc/zk/zk_ec.h>

#include "network.h"

using namespace coinbase;
using namespace coinbase::crypto;
using namespace coinbase::mpc;
using node_t = coinbase::crypto::ss::node_t;
using node_e = coinbase::crypto::ss::node_e;

// ============ ECDSA 2PC =============
int mpc_ecdsa2p_dkg(JOB_SESSION_2P_PTR* j, int curve_code, MPC_ECDSA2PC_KEY_PTR* k) {
  job_session_2p_t* job = static_cast<job_session_2p_t*>(j->opaque);
  ecurve_t curve = ecurve_t::find(curve_code);

  ecdsa2pc::key_t* key = new ecdsa2pc::key_t();

  error_t err = ecdsa2pc::dkg(*job, curve, *key);
  if (err) return err;
  *k = MPC_ECDSA2PC_KEY_PTR{key};

  return 0;
}

int mpc_ecdsa2p_refresh(JOB_SESSION_2P_PTR* j, MPC_ECDSA2PC_KEY_PTR* k, MPC_ECDSA2PC_KEY_PTR* nk) {
  job_session_2p_t* job = static_cast<job_session_2p_t*>(j->opaque);

  ecdsa2pc::key_t* key = static_cast<ecdsa2pc::key_t*>(k->opaque);
  ecdsa2pc::key_t* new_key = new ecdsa2pc::key_t();

  error_t err = ecdsa2pc::refresh(*job, *key, *new_key);
  if (err) return err;
  *nk = MPC_ECDSA2PC_KEY_PTR{new_key};

  return 0;
}

int mpc_ecdsa2p_sign(JOB_SESSION_2P_PTR* j, cmem_t sid, MPC_ECDSA2PC_KEY_PTR* k, cmems_t msgs, cmems_t* sigs) {
  job_session_2p_t* job = static_cast<job_session_2p_t*>(j->opaque);
  ecdsa2pc::key_t* key = static_cast<ecdsa2pc::key_t*>(k->opaque);
  buf_t session_id = mem_t(sid);
  std::vector<mem_t> messages = coinbase::mems_t(msgs).mems();

  std::vector<buf_t> signatures;
  error_t err = ecdsa2pc::sign_batch(*job, session_id, *key, messages, signatures);
  if (err) return err;
  *sigs = coinbase::mems_t(signatures).to_cmems();

  return 0;
}

// ============ ECDSA MPC ==============
int mpc_ecdsampc_dkg(JOB_SESSION_MP_PTR* j, int curve_code, MPC_ECDSAMPC_KEY_PTR* k) {
  job_session_mp_t* job = static_cast<job_session_mp_t*>(j->opaque);
  ecurve_t curve = ecurve_t::find(curve_code);

  ecdsampc::key_t* key = new ecdsampc::key_t();

  buf_t sid;
  error_t err = ecdsampc::dkg(*job, curve, *key, sid);
  if (err) return err;
  *k = MPC_ECDSAMPC_KEY_PTR{key};

  return 0;
}

int mpc_ecdsampc_sign(JOB_SESSION_MP_PTR* j, MPC_ECDSAMPC_KEY_PTR* k, cmem_t msg_mem, int sig_receiver,
                      cmem_t* sig_mem) {
  job_session_mp_t* job = static_cast<job_session_mp_t*>(j->opaque);
  ecdsampc::key_t* key = static_cast<ecdsampc::key_t*>(k->opaque);

  buf_t msg = coinbase::mem_t(msg_mem);
  buf_t sig;
  error_t err = ecdsampc::sign(*job, *key, msg, party_idx_t(sig_receiver), sig);
  if (err) return err;
  *sig_mem = sig.to_cmem();

  return 0;
}

// ============ ZKPs =================
int ZK_DL_Example() {
  uint64_t aux = 0;
  buf_t sid = coinbase::crypto::gen_random(16);

  ecurve_t c = coinbase::crypto::curve_secp256k1;
  coinbase::zk::uc_dl_t zk;
  ecc_point_t G, Q;
  mod_t q;
  bn_t w;

  G = c.generator();
  q = c.order();
  w = bn_t::rand(q);
  Q = w * G;

  zk.prove(Q, w, sid, aux);
  auto v = zk.verify(Q, sid, aux);

  return v + 10;
}

// ============ PVE ================
CRYPTO_SS_NODE_PTR new_node(int node_type, cmem_t node_name, int threshold) {
  std::string name = mem_t(node_name).to_string();
  node_t* node = new node_t(node_e(node_type), name, threshold);
  return CRYPTO_SS_NODE_PTR{node};
}

void add_child(CRYPTO_SS_NODE_PTR* parent, CRYPTO_SS_NODE_PTR* child) {
  crypto::ss::node_t* p = static_cast<crypto::ss::node_t*>(parent->opaque);
  crypto::ss::node_t* c = static_cast<crypto::ss::node_t*>(child->opaque);
  p->add_child_node(c);
}

crypto::ecc_prv_key_t get_prv_key() {
  crypto::ecc_prv_key_t prv_key_ecc;
  prv_key_ecc.generate(crypto::curve_p256);
  return prv_key_ecc;
}

int get_n_enc_keypairs(int n, cmems_t* prv_keys_ptr, cmems_t* pub_keys_ptr) {
  std::vector<buf_t> prv_keys(n);
  std::vector<buf_t> pub_keys(n);
  for (int i = 0; i < n; i++) {
    crypto::ecc_prv_key_t prv_key = get_prv_key();
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

int pve_quorum_encrypt(CRYPTO_SS_NODE_PTR* root_ptr, cmems_t pub_keys_list_ptr, int pub_keys_count, cmems_t xs_list_ptr,
                       int xs_count, const char* label_ptr, cmem_t* out_ptr) {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::ss::node_t* root = static_cast<crypto::ss::node_t*>(root_ptr->opaque);
  std::vector<buf_t> pub_keys_bufs = coinbase::mems_t(pub_keys_list_ptr).bufs();
  std::vector<crypto::ecc_pub_key_t> pub_keys_list(pub_keys_count);
  for (int i = 0; i < pub_keys_count; i++) {
    rv = coinbase::deser(pub_keys_bufs[i], pub_keys_list[i]);
    if (rv) return rv;
  }
  std::vector<buf_t> xs_bufs = coinbase::mems_t(xs_list_ptr).bufs();
  std::vector<bn_t> xs(xs_count);
  for (int i = 0; i < xs_count; i++) {
    rv = coinbase::deser(xs_bufs[i], xs[i]);
    if (rv) return rv;
  }

  ecurve_t curve = crypto::curve_p256;
  mod_t q = curve.order();
  const ecc_generator_point_t& G = curve.generator();

  ss::ac_owned_t ac(root);

  auto leaves = ac.list_leaf_names();
  std::map<std::string, crypto::ecc_pub_key_t> pub_keys;

  int i = 0;
  for (auto path : leaves) {
    pub_keys[path] = pub_keys_list[i];
    i++;
  }

  ec_pve_ac_t<ecies_t> pve;
  std::vector<ecc_point_t> Xs(xs_count);
  for (int i = 0; i < xs_count; i++) {
    Xs[i] = xs[i] * G;
  }

  std::string label(label_ptr);
  pve.encrypt(ac, pub_keys, label, curve, xs);
  buf_t out = coinbase::convert(pve);
  *out_ptr = out.to_cmem();
  return SUCCESS;
}

int pve_quorum_decrypt(CRYPTO_SS_NODE_PTR* root_ptr, cmems_t quorum_prv_keys_list_ptr, int quorum_prv_keys_count,
                       cmems_t all_pub_keys_list_ptr, int all_pub_keys_count, cmem_t pve_bundle_cmem,
                       cmems_t Xs_list_ptr, int xs_count, const char* label_ptr, cmems_t* out_ptr) {
  error_t rv = UNINITIALIZED_ERROR;
  crypto::ss::node_t* root = static_cast<crypto::ss::node_t*>(root_ptr->opaque);
  std::vector<buf_t> quorum_prv_keys_bufs = coinbase::mems_t(quorum_prv_keys_list_ptr).bufs();
  std::vector<crypto::ecc_prv_key_t> quorum_prv_keys_list(quorum_prv_keys_count);
  for (int i = 0; i < quorum_prv_keys_count; i++) {
    rv = coinbase::deser(quorum_prv_keys_bufs[i], quorum_prv_keys_list[i]);
    if (rv) return rv;
  }
  std::vector<buf_t> all_pub_keys_bufs = coinbase::mems_t(all_pub_keys_list_ptr).bufs();
  std::vector<crypto::ecc_pub_key_t> all_pub_keys_list(all_pub_keys_count);
  for (int i = 0; i < all_pub_keys_count; i++) {
    rv = coinbase::deser(all_pub_keys_bufs[i], all_pub_keys_list[i]);
    if (rv) return rv;
  }
  std::vector<buf_t> Xs_bufs = coinbase::mems_t(Xs_list_ptr).bufs();
  std::vector<ecc_point_t> Xs(xs_count);
  for (int i = 0; i < xs_count; i++) {
    rv = coinbase::deser(Xs_bufs[i], Xs[i]);
    if (rv) return rv;
  }

  ec_pve_ac_t<ecies_t> pve;
  buf_t pve_bundle_buf_t = coinbase::mem_t(pve_bundle_cmem);
  rv = coinbase::deser(pve_bundle_buf_t, pve);
  if (rv) return rv;

  ecurve_t curve = crypto::curve_p256;
  mod_t q = curve.order();
  const ecc_generator_point_t& G = curve.generator();

  ss::ac_owned_t ac(root);

  auto leaves = ac.list_leaf_names();
  std::map<std::string, crypto::ecc_pub_key_t> pub_keys;
  std::map<std::string, crypto::ecc_prv_key_t> quorum_prv_keys;

  int i = 0;
  for (auto path : leaves) {
    quorum_prv_keys[path] = quorum_prv_keys_list[i];
    pub_keys[path] = all_pub_keys_list[i];
    i++;
  }

  std::string label(label_ptr);
  rv = pve.verify(ac, pub_keys, Xs, label);
  if (rv) return rv;

  std::vector<bn_t> decrypted_xs;
  rv = pve.decrypt(ac, quorum_prv_keys, pub_keys, label, decrypted_xs,
                   true);  // skip_verify = true since it is verified already
  if (rv) return rv;
  std::vector<buf_t> out(xs_count);
  for (int i = 0; i < xs_count; i++) {
    out[i] = coinbase::ser(decrypted_xs[i]);
  }
  *out_ptr = coinbase::mems_t(out).to_cmems();
  return SUCCESS;
}

// ============ Utilities =================
int convert_ecdsa_share_to_bn_t_share(MPC_ECDSAMPC_KEY_PTR* k, cmem_t* x_ptr, cmem_t* Q_ptr) {
  ecdsampc::key_t* key = static_cast<ecdsampc::key_t*>(k->opaque);
  auto x = coinbase::ser(key->x_share);

  ecurve_t curve = crypto::curve_p256;
  const ecc_generator_point_t& G = curve.generator();
  auto Q = coinbase::ser(key->x_share * G);

  *x_ptr = coinbase::mem_t(x).to_cmem();
  *Q_ptr = coinbase::mem_t(Q).to_cmem();
  return 0;
}

int ecdsa_mpc_public_key_to_string(MPC_ECDSAMPC_KEY_PTR* k, cmem_t* x_str, cmem_t* y_str) {
  ecdsampc::key_t* key = static_cast<ecdsampc::key_t*>(k->opaque);
  *x_str = key->Q.get_x().to_bin().to_cmem();
  *y_str = key->Q.get_y().to_bin().to_cmem();
  return 0;
}