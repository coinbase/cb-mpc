#pragma once

#include <stdint.h>

#include <cbmpc/core/cmem.h>

#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------- Type Wrappers ---------------------------
// Naming convention:
//  - drop the initial 'coinbase' namespace, replace :: with _ add ptr instead of _t
//  - We do this since direct usage of namespaces in C is not supported
// case case Example:
//    coinbase::mpc::ecdsa2pc::key_t is represented here as mpc_ecdsa2pc_key_ptr

typedef struct MPC_ECDSA2PC_KEY_PTR
{
  void* opaque;  // Opaque pointer to the C++ class instance
} MPC_ECDSA2PC_KEY_PTR;

typedef struct MPC_ECDSAMPC_KEY_PTR
{
  void* opaque;  // Opaque pointer to the C++ class instance
} MPC_ECDSAMPC_KEY_PTR;

typedef struct CRYPTO_SS_NODE_PTR
{
  void* opaque;  // Opaque pointer to the C++ class instance
} CRYPTO_SS_NODE_PTR;

typedef struct CRYPTO_PRV_KEY_PTR
{
  void* opaque;  // Opaque pointer to the C++ class instance
} CRYPTO_PRV_KEY_PTR;

typedef struct CRYPTO_PUB_KEY_PTR
{
  void* opaque;  // Opaque pointer to the C++ class instance
} CRYPTO_PUB_KEY_PTR;

inline void free_mpc_ecdsa2p_key(MPC_ECDSA2PC_KEY_PTR ctx) { free(ctx.opaque); }
inline void free_mpc_ecdsamp_key(MPC_ECDSAMPC_KEY_PTR ctx) { free(ctx.opaque); }
inline void free_crypto_ss_node(CRYPTO_SS_NODE_PTR ctx) { free(ctx.opaque); }
inline void free_crypto_prv_key(CRYPTO_PRV_KEY_PTR ctx) { free(ctx.opaque); }
inline void free_crypto_pub_key(CRYPTO_PUB_KEY_PTR ctx) { free(ctx.opaque); }
// ------------------------- Function/Method Wrappers ----------------
// For each function in the library, create a wrapper that uses the following types:
//  - primitive types such as int, char, void, ...
//  - PTR types defined above
//  - PTR types defined in the network directory
//  - cmem_t, cmems_t types defined in the library
//
// Conventions:
//  - Implementing a method of a class, receives the class pointer as the first argument, called ctx

// ============ ECDSA 2PC =============

int mpc_ecdsa2p_dkg(JOB_SESSION_2P_PTR* job, int curve, MPC_ECDSA2PC_KEY_PTR* key);

int mpc_ecdsa2p_refresh(JOB_SESSION_2P_PTR* job, MPC_ECDSA2PC_KEY_PTR* key, MPC_ECDSA2PC_KEY_PTR* new_key);

int mpc_ecdsa2p_sign(
    JOB_SESSION_2P_PTR* job, cmem_t session_id, MPC_ECDSA2PC_KEY_PTR* key, cmems_t msgs, cmems_t* sigs);

// ============ ECDSA MPC ==============
int mpc_ecdsampc_dkg(JOB_SESSION_MP_PTR* j, int curve_code, MPC_ECDSAMPC_KEY_PTR* k);

int mpc_ecdsampc_sign(
    JOB_SESSION_MP_PTR* j,
    MPC_ECDSAMPC_KEY_PTR* k,
    cmem_t msg_mem,
    int sig_receiver,
    cmem_t* sig_mem);

// ============ PVE ================
CRYPTO_SS_NODE_PTR new_node(int node_type, cmem_t node_name, int threshold);
void add_child(CRYPTO_SS_NODE_PTR* parent, CRYPTO_SS_NODE_PTR* child);
int pve_quorum_encrypt(CRYPTO_SS_NODE_PTR* root_ptr, cmems_t pub_keys_list_ptr, int pub_keys_count, cmems_t xs_list_ptr, int xs_count, const char* label_ptr, cmem_t* out_ptr);
int pve_quorum_decrypt(CRYPTO_SS_NODE_PTR* root_ptr, cmems_t quorum_prv_keys_list_ptr, int quorum_prv_keys_count, cmems_t all_pub_keys_list_ptr, int all_pub_keys_count, cmem_t pve_bundle_cmem, cmems_t Xs_list_ptr, int xs_count, const char* label_ptr, cmems_t* out_ptr);
int get_n_enc_keypairs(int n, cmems_t* pub_keys_ptr, cmems_t* prv_keys_ptr);
int get_n_ec_keypairs(int n, cmems_t* prv_keys_ptr, cmems_t* pub_keys_ptr);
int convert_ecdsa_share_to_bn_t_share(MPC_ECDSAMPC_KEY_PTR* k, cmem_t* x_ptr, cmem_t* Q_ptr);

// ============ ZKPs =================
int ZK_DL_Example();

// ============ Utilities =============

int ecdsa_mpc_public_key_to_string(MPC_ECDSAMPC_KEY_PTR* k, cmem_t *x_str, cmem_t *y_str);

#ifdef __cplusplus
}  // extern "C"
#endif
