#pragma once

#include <cbmpc/api/curve.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>

namespace coinbase::api::schnorr_2p {

using party_t = coinbase::api::party_2p_t;

// Run the 2-party key generation protocol.
//
// The output `key_blob` is a versioned, opaque byte string that can be persisted
// by the caller and used in other API calls.
//
// This wrapper implements the BIP340 Schnorr signature scheme, which is defined
// for secp256k1 only.
//
// Supported curves: `curve_id::secp256k1`.
error_t dkg(const coinbase::api::job_2p_t& job, curve_id curve, buf_t& key_blob);

// Refresh an existing key share, producing a new share (same public key).
error_t refresh(const coinbase::api::job_2p_t& job, mem_t key_blob, buf_t& new_key_blob);

// Sign a message with Schnorr-2PC (BIP340).
//
// Input:
// - `msg` must be exactly 32 bytes (BIP340 message digest).
//
// Output signature is 64 bytes: r_x (32 bytes) || s (32 bytes).
//
// Note: the underlying protocol returns the signature only on P1. On P2,
// `sig` may be left empty on success.
error_t sign(const coinbase::api::job_2p_t& job, mem_t key_blob, mem_t msg, buf_t& sig);

// Get the public key from a key blob.
//
// Notes:
// - For BIP340, the "standard" public key encoding is x-only (32 bytes). This
//   API provides both x-only and SEC1 compressed encodings for convenience.
// - Output is deterministic and derived from the key blob contents.
//
// `pub_key_compressed` is a SEC1 compressed point encoding (33 bytes) on
// secp256k1: 0x02/0x03 || x (32 bytes).
error_t get_public_key_compressed(mem_t key_blob, buf_t& pub_key_compressed);

// Extract the BIP340 x-only public key (32 bytes).
error_t extract_public_key_xonly(mem_t key_blob, buf_t& pub_key_xonly);

// ---------------------------------------------------------------------------
// Key blob manipulation (private scalar backup / restore)
// ---------------------------------------------------------------------------

// Get this party's share public point (Qi) from a key blob, returning SEC1
// compressed point encoding (33 bytes).
//
// This is useful for verifiable backup schemes like PVE, where a scalar x can be
// verified against its corresponding curve point Q = x*G.
//
// Notes:
// - Intended for full key blobs. If you detached a scalar share, persist the
//   returned public share point separately (or call this API before detaching).
error_t get_public_share_compressed(mem_t key_blob, buf_t& out_public_share_compressed);

// Detach the private scalar share from a key blob, producing:
// - a "public" key blob with its private scalar removed, and
// - the private scalar x encoded as a fixed-length big-endian buffer.
//
// The public blob is safe to persist as public-only material, but is not usable
// for signing/refresh until restored with `attach_private_scalar`.
//
// Output:
// - `out_private_scalar_fixed` length equals the curve order size in bytes
//   (32 bytes for secp256k1).
error_t detach_private_scalar(mem_t key_blob, buf_t& out_public_key_blob, buf_t& out_private_scalar_fixed);

// Restore a full key blob by attaching a fixed-length private scalar x into a
// public key blob (produced by `detach_private_scalar`).
//
// This validates that x matches the expected share point by checking:
// x*G == public_share_compressed.
//
// Input:
// - `private_scalar_fixed` must be a fixed-length big-endian scalar encoding with
//   length equal to the curve order size in bytes (32 bytes for secp256k1).
// - `public_share_compressed` must be the SEC1 compressed point encoding of this
//   party's share public point (Qi_self), e.g. from `get_public_share_compressed`.
error_t attach_private_scalar(mem_t public_key_blob, mem_t private_scalar_fixed, mem_t public_share_compressed,
                              buf_t& out_key_blob);

}  // namespace coinbase::api::schnorr_2p
