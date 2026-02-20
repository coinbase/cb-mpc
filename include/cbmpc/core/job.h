#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

namespace coinbase::api {

// Party index (0..n-1) used by transports and multi-party protocols.
using party_idx_t = int32_t;

// Two-party role used by 2PC protocols.
enum class party_2p_t : party_idx_t {
  p1 = 0,
  p2 = 1,
};

// Transport abstraction for MPC protocols.
//
// Implementations are expected to be *blocking*: `receive` should wait until a
// message from `sender` is available.
class data_transport_i {
 public:
  virtual ~data_transport_i() = default;

  virtual error_t send(party_idx_t receiver, mem_t msg) = 0;
  virtual error_t receive(party_idx_t sender, buf_t& msg) = 0;

  // Multi-party receive. Implementations may choose any ordering contract;
  // cbmpc callers should pass the same `senders` ordering on both sides.
  virtual error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& msgs) = 0;
};

// Execution context for 2-party protocols.
//
// Notes:
// - This is a lightweight view type. It does not own the transport object or
//   the name strings.
// - The caller must ensure referenced objects (including the character data
//   backing the `std::string_view`s) outlive the protocol call.
//   (For example, do not pass temporary `std::string`s when constructing a job.)
struct job_2p_t {
  party_2p_t self;
  std::string_view p1_name;
  std::string_view p2_name;
  data_transport_i& transport;
};

// Execution context for multi-party protocols.
//
// Notes:
// - This is a lightweight view type. It does not own the transport object or
//   the name strings.
// - `self` is the caller party index in `party_names`.
// - The caller must ensure referenced objects (including the character data
//   backing `party_names`) outlive the protocol call.
//   (For example, do not build `party_names` from temporary `std::string`s.)
struct job_mp_t {
  party_idx_t self;
  std::vector<std::string_view> party_names;
  data_transport_i& transport;
};

}  // namespace coinbase::api
