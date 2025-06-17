#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <cbmpc/core/cmem.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*send_f)(void* go_impl_ptr, int receiver, uint8_t* message, int message_size);
typedef int (*receive_f)(void* go_impl_ptr, int receiver, uint8_t** message, int* message_size);
typedef int (*receive_all_f)(void* go_impl_ptr, int* receivers, int receiver_count, uint8_t** messages,
                             int* message_sizes);

typedef struct data_transport_callbacks_t
{
  send_f send_fun;
  receive_f receive_fun;
  receive_all_f receive_all_fun;
} data_transport_callbacks_t;

typedef struct JOB_SESSION_2P_PTR
{
  void* opaque;  // Opaque pointer to the C++ class instance
} JOB_SESSION_2P_PTR;

typedef struct JOB_SESSION_MP_PTR
{
  void* opaque;  // Opaque pointer to the C++ class instance
} JOB_SESSION_MP_PTR;

inline void free_job_session_2p(JOB_SESSION_2P_PTR* ptr) { free(ptr->opaque); }
inline void free_job_session_mp(JOB_SESSION_MP_PTR* ptr) { free(ptr->opaque); }

// ---------------- JOB_SESSION_2P_PTR ------------
JOB_SESSION_2P_PTR* new_job_session_2p(data_transport_callbacks_t* callbacks, void* go_impl_ptr, int party_index, char** pnames, int pname_count);
int is_peer1(JOB_SESSION_2P_PTR* job);
int is_peer2(JOB_SESSION_2P_PTR* job);
int is_role_index(JOB_SESSION_2P_PTR* job, int party_index);
int get_role_index(JOB_SESSION_2P_PTR* job);
int mpc_2p_send(JOB_SESSION_2P_PTR* job, int receiver, const uint8_t* msg, const int msg_len);
int mpc_2p_receive(JOB_SESSION_2P_PTR* job, int sender, uint8_t** msg, int* msg_len);

// ---------------- JOB_SESSION_MP_PTR ------------
JOB_SESSION_MP_PTR* new_job_session_mp(data_transport_callbacks_t* callbacks, void* go_impl_ptr, int party_count, int party_index, int job_session_id, char** pnames, int pname_count);
int is_party(JOB_SESSION_MP_PTR* job, int party_index);
int get_party_idx(JOB_SESSION_MP_PTR* job);

// ---------------- Agree Randoms ------------
int mpc_agree_random(JOB_SESSION_2P_PTR* job, int bit_len, cmem_t* out);

#ifdef __cplusplus
}  // extern "C"
#endif
