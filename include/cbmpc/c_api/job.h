#pragma once

#include <stdint.h>

#include <cbmpc/c_api/common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef cbmpc_error_t (*cbmpc_transport_send_fn)(void* ctx, int32_t receiver, const uint8_t* data, int size);
typedef cbmpc_error_t (*cbmpc_transport_receive_fn)(void* ctx, int32_t sender, cmem_t* out_msg);
typedef cbmpc_error_t (*cbmpc_transport_receive_all_fn)(void* ctx, const int32_t* senders, int senders_count,
                                                        cmems_t* out_msgs);
typedef void (*cbmpc_transport_free_fn)(void* ctx, void* ptr);

// Transport callbacks used by interactive protocols (e.g., ECDSA-2PC, ECDSA-MP).
//
// - `send` does not take ownership of `data`; the caller retains ownership.
// - `receive`/`receive_all` must return message buffers allocated by the caller.
// - The library will free returned buffers with `free(ctx, ptr)` when provided,
//   otherwise it will call `cbmpc_free(ptr)`.
// - For `receive_all`, the callback must allocate **both** `out_msgs->data` and
//   `out_msgs->sizes` (typically as two independent allocations).
// - On success, `receive_all` must set `out_msgs->count == senders_count` and
//   return messages in the same order as `senders[]`.
// - On non-zero return, the library may still free any non-NULL output pointers
//   set by the callback as a best-effort to avoid leaks.
typedef struct cbmpc_transport_t {
  void* ctx;
  cbmpc_transport_send_fn send;
  cbmpc_transport_receive_fn receive;
  cbmpc_transport_receive_all_fn receive_all;  // optional (required by MP protocols, e.g., ECDSA-MP)
  cbmpc_transport_free_fn free;                // optional (defaults to cbmpc_free)
} cbmpc_transport_t;

typedef enum cbmpc_2pc_party_e {
  CBMPC_2PC_P1 = 0,
  CBMPC_2PC_P2 = 1,
} cbmpc_2pc_party_t;

// Execution context for 2-party protocols.
//
// Notes:
// - This is a lightweight view type. It does not own the transport object or
//   the name strings.
// - `p1_name` and `p2_name` must be NUL-terminated strings.
// - The caller must ensure referenced objects (including the name strings)
//   outlive the protocol call.
typedef struct cbmpc_2pc_job_t {
  cbmpc_2pc_party_t self;
  const char* p1_name;
  const char* p2_name;
  const cbmpc_transport_t* transport;
} cbmpc_2pc_job_t;

// Execution context for multi-party protocols.
//
// Notes:
// - This is a lightweight view type. It does not own the transport object or
//   the name strings.
// - `self` is the caller party index in `party_names`.
// - Each entry in `party_names` must be a NUL-terminated string.
// - The caller must ensure referenced objects (including the name strings)
//   outlive the protocol call.
typedef struct cbmpc_mp_job_t {
  int32_t self;
  const char* const* party_names;
  int party_names_count;
  const cbmpc_transport_t* transport;
} cbmpc_mp_job_t;

#ifdef __cplusplus
}
#endif
