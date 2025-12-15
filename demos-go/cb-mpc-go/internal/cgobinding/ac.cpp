#include "ac.h"

#include <cbmpc/core/buf.h>
#include <cbmpc/ffi/cmem_adapter.h>
#include <cbmpc/crypto/secret_sharing.h>

using namespace coinbase;
using namespace coinbase::crypto;
using node_t = coinbase::crypto::ss::node_t;
using node_e = coinbase::crypto::ss::node_e;

#ifdef __cplusplus
extern "C" {
#endif

// ============ PVE (Access Structure) utilities ================
crypto_ss_node_ref new_node(int node_type, cmem_t node_name, int threshold) {
  std::string name = coinbase::ffi::view(node_name).to_string();
  node_t* node = new node_t(node_e(node_type), name, threshold);
  return crypto_ss_node_ref{node};
}

void add_child(crypto_ss_node_ref* parent, crypto_ss_node_ref* child) {
  node_t* p = static_cast<node_t*>(parent->opaque);
  node_t* c = static_cast<node_t*>(child->opaque);
  p->add_child_node(c);
}

crypto_ss_ac_ref new_access_structure(crypto_ss_node_ref* root, ecurve_ref* curve_ref) {
  crypto::ss::node_t* root_node = static_cast<crypto::ss::node_t*>(root->opaque);

  // Resolve the curve reference passed from Go and obtain its generator.
  crypto::ecurve_t* curve = static_cast<crypto::ecurve_t*>(curve_ref->opaque);

  crypto::ss::ac_t* ac = new crypto::ss::ac_t();
  if (curve) {
    ac->G = curve->generator();
  }
  ac->root = root_node;
  return crypto_ss_ac_ref{ac};
}

// ============ Memory Management ================================

// Frees the native access-structure instance allocated by
// new_access_structure. Calling this function more than once on the same
// object or passing an already-freed reference is undefined behaviour.
void free_crypto_ss_ac(crypto_ss_ac_ref ac) {
  if (ac.opaque != nullptr) {
    crypto::ss::ac_t* ptr = static_cast<crypto::ss::ac_t*>(ac.opaque);
    delete ptr;
  }
}

#ifdef __cplusplus
}
#endif