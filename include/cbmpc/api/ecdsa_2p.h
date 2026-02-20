#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>

namespace coinbase::api::ecdsa_2p {

using party_t = coinbase::api::party_2p_t;

// Run the 2-party key generation protocol.
//
// The output `key_blob` is a versioned, opaque byte string that can be persisted
// by the caller and used in other API calls.
error_t dkg(const coinbase::api::job_2p_t& job, curve_id curve, buf_t& key_blob);

// Refresh an existing key share, producing a new share (same public key).
error_t refresh(const coinbase::api::job_2p_t& job, mem_t key_blob, buf_t& new_key_blob);

// Sign a message hash (not the raw message) using ECDSA-2PC.
//
// `sid` is an in/out session id used by the protocol; callers may pass an empty
// buffer and let the protocol derive one.
//
// Output signature is DER encoded.
//
// Note: the underlying protocol returns the signature only on P1. On P2,
// `sig_der` may be left empty on success.
error_t sign(const coinbase::api::job_2p_t& job, mem_t key_blob, mem_t msg_hash, buf_t& sid, buf_t& sig_der);

// Get the compressed public key from a key blob.
error_t get_public_key_compressed(mem_t key_blob, buf_t& pub_key);

// ---------------------------------------------------------------------------
// Key blob manipulation (private scalar backup / restore)
// ---------------------------------------------------------------------------

// Get this party's share public point (Qi) from a key blob, returning SEC1
// compressed point encoding.
//
// This is useful for verifiable backup schemes like PVE, where a scalar x can be
// verified against its corresponding curve point Q = x*G.
//
// Notes:
// - Intended for full key blobs. If you detached a scalar share, persist the
//   returned public share point separately (or call this API before detaching).
error_t get_public_share_compressed(mem_t key_blob, buf_t& out_public_share_compressed);

// Detach the private scalar share from a key blob, producing:
// - a key blob with its private scalar removed, and
// - the private scalar x encoded as a big-endian buffer.
//
// The scalar-removed blob is not usable for signing/refresh until restored with
// `attach_private_scalar`.
//
// Note (ECDSA-2PC encoding):
// - Unlike ECDSA-MP, this scalar encoding is NOT fixed-length. ECDSA-2PC keeps
//   the share as a Paillier-compatible integer representative and it may grow
//   after refresh.
error_t detach_private_scalar(mem_t key_blob, buf_t& out_public_key_blob, buf_t& out_private_scalar);

// Restore a full key blob by attaching a big-endian private scalar share x into a
// scalar-removed key blob (produced by `detach_private_scalar`).
//
// This validates that x matches the expected share point by checking:
// (x mod q)*G == public_share_compressed.
//
// Input:
// - `private_scalar` is a big-endian scalar encoding (variable-length).
// - `public_share_compressed` must be the SEC1 compressed point encoding of this
//   party's share public point (Qi_self), e.g. from `get_public_share_compressed`.
error_t attach_private_scalar(mem_t public_key_blob, mem_t private_scalar, mem_t public_share_compressed,
                              buf_t& out_key_blob);

}  // namespace coinbase::api::ecdsa_2p
