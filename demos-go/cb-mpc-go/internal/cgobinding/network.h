#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <cbmpc/core/cmem.h>

#ifdef __cplusplus
extern "C" {
#endif

// Error codes for consistent error handling
#define NETWORK_SUCCESS 0
#define NETWORK_ERROR -1
#define NETWORK_PARAM_ERROR -2
#define NETWORK_MEMORY_ERROR -3
#define NETWORK_INVALID_STATE -4

// Callback function types using cmem_t/cmems_t for cleaner interfaces
typedef int (*send_f)(void* go_impl_ptr, int receiver, cmem_t message);
typedef int (*receive_f)(void* go_impl_ptr, int sender, cmem_t* message);
typedef int (*receive_all_f)(void* go_impl_ptr, int* senders, int sender_count, cmems_t* messages);

typedef struct data_transport_callbacks_t {
  send_f send_fun;
  receive_f receive_fun;
  receive_all_f receive_all_fun;
} data_transport_callbacks_t;

typedef struct job_2p_ref {
  void* opaque;
} job_2p_ref;

typedef struct job_mp_ref {
  void* opaque;
} job_mp_ref;

// ---------------------------------------------------------------------------
// Generic wrapper for a party_set_t instance used across multiple APIs.
// Moved from ecdsamp.h / cblib.h to centralize the definition and avoid
// duplication.  Renamed from PARTY_SET_PTR to mpc_party_set_ref.
typedef struct mpc_party_set_ref {
  void* opaque;  // Opaque pointer to the C++ party_set_t instance
} mpc_party_set_ref;

// ---------------------------------------------------------------------------
// Party-set helper C API (moved from ecdsamp / cblib headers)
mpc_party_set_ref new_party_set();
void party_set_add(mpc_party_set_ref* set, int party_idx);
void free_party_set(mpc_party_set_ref ctx);

// job_2p_ref Functions
job_2p_ref* new_job_2p(const data_transport_callbacks_t* callbacks, void* go_impl_ptr, int party_index,
                       const char* const* pnames, int pname_count);
void free_job_2p(job_2p_ref* ptr);
int is_peer1(const job_2p_ref* job);
int is_peer2(const job_2p_ref* job);
int is_role_index(const job_2p_ref* job, int party_index);
int get_role_index(const job_2p_ref* job);
int mpc_2p_send(job_2p_ref* job, int receiver, cmem_t msg);
int mpc_2p_receive(job_2p_ref* job, int sender, cmem_t* msg);

// job_mp_ref Functions
job_mp_ref* new_job_mp(const data_transport_callbacks_t* callbacks, void* go_impl_ptr, int party_count, int party_index,
                       const char* const* pnames, int pname_count);
void free_job_mp(job_mp_ref* ptr);
int is_party(const job_mp_ref* job, int party_index);
int get_party_idx(const job_mp_ref* job);
int get_n_parties(const job_mp_ref* job);

#ifdef __cplusplus
}  // extern "C"
#endif
