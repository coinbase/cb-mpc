#pragma once

#include <cbmpc/api/curve.h>
#include <cbmpc/core/access_structure.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>

namespace coinbase::api::ecdsa_mp {

// Run the multi-party key generation protocol.
//
// The output `key_blob` is a versioned, opaque byte string that can be persisted
// by the caller and used in other API calls.
//
// Supported curves: `curve_id::p256`, `curve_id::secp256k1`.
error_t dkg_additive(const job_mp_t& job, curve_id curve, buf_t& key_blob, buf_t& sid);

// Run the multi-party key generation protocol with a general access
// structure.
//
// Notes:
// - This is an n-party protocol: **all** parties in
//   `job.party_names` must be online and participate.
// - Only the provided `quorum_party_names` actively contribute to the generated
//   key shares.
// - The output key blob represents an access-structure key share and
//   is not directly usable with `sign_additive()`. Use `sign_ac()` to sign with an online
//   quorum (it derives additive shares internally).
// - `sid` is an in/out session id used by the protocol. Callers may pass an
//   empty buffer to let the protocol derive one.
//
// Supported curves: `curve_id::p256`, `curve_id::secp256k1`.
error_t dkg_ac(const job_mp_t& job, curve_id curve, buf_t& sid, const access_structure_t& access_structure,
               const std::vector<std::string_view>& quorum_party_names, buf_t& key_blob);

// Refresh an existing key share set, producing a new key share.
//
// `sid` is an in/out session id used by the refresh protocol. Callers may pass
// an empty buffer to let the protocol derive one.
error_t refresh_additive(const job_mp_t& job, buf_t& sid, mem_t key_blob, buf_t& new_key_blob);

// Refresh an existing key share set using the access-structure refresh protocol.
//
// Notes:
// - See `dkg_ac` for protocol participation semantics.
// - The output key blob represents an access-structure key share and
//   is not directly usable with `sign_additive()`. Use `sign_ac()` to sign with an online
//   quorum (it derives additive shares internally).
// - `sid` is an in/out session id used by the protocol. Callers may pass an
//   empty buffer to let the protocol derive one.
error_t refresh_ac(const job_mp_t& job, buf_t& sid, mem_t key_blob, const access_structure_t& access_structure,
                   const std::vector<std::string_view>& quorum_party_names, buf_t& new_key_blob);

// Sign a message with ECDSA-MP and output signature on `sig_receiver`.
//
// Output signature is DER encoded.
//
// Note: the underlying protocol returns the signature only on `sig_receiver`. On
// other parties, `sig_der` may be left empty on success.
error_t sign_additive(const job_mp_t& job, mem_t key_blob, mem_t msg, party_idx_t sig_receiver, buf_t& sig_der);

// Sign a message with ECDSA-MP using an access-structure key share (from
// `dkg_ac` / `refresh_ac`).
//
// This API first derives an additive-share signing key for the **online** signing
// parties in `job.party_names` and then runs the normal `sign_additive()` protocol among
// those parties.
//
// Notes:
// - Unlike `dkg_ac` / `refresh_ac`, `sign_ac` only requires the parties in
//   `job.party_names` to be online and participate.
// - Output semantics match `sign_additive()`: the signature is returned only on
//   `sig_receiver`. On other parties, `sig_der` may be left empty on success.
error_t sign_ac(const job_mp_t& job, mem_t ac_key_blob, const access_structure_t& access_structure, mem_t msg,
                party_idx_t sig_receiver, buf_t& sig_der);

// Get the compressed public key from a key blob (SEC1 compressed point).
//
// This is the same encoding as `ecdsa_2p::get_public_key_compressed`.
error_t get_public_key_compressed(mem_t key_blob, buf_t& pub_key);

// ---------------------------------------------------------------------------
// Key blob manipulation (private share backup / restore)
// ---------------------------------------------------------------------------

// Get this party's share public point (Qi) from a key blob, returning SEC1
// compressed point encoding.
//
// This is useful for verifiable backup schemes like PVE, where a scalar x can be
// verified against its corresponding curve point Q = x*G.
//
// Notes:
// - Works for both additive (v1) and access-structure (v2) ECDSA-MP key blobs.
// - This can be called on a blob produced by `detach_private_scalar` since Qi_self
//   remains present in the key blob even after redaction.
error_t get_public_share_compressed(mem_t key_blob, buf_t& out_public_share_compressed);

// Detach the private scalar share from a key blob, producing:
// - a "public" key blob with its private scalar wiped, and
// - the private scalar x encoded as a fixed-length big-endian buffer.
//
// The public blob is safe to persist as public-only material, but is not usable
// for signing/refresh until restored with `attach_private_scalar`.
//
// Output:
// - `out_private_scalar_fixed` length equals the curve order size in bytes
//   (e.g., 32 bytes for secp256k1/p256).
error_t detach_private_scalar(mem_t key_blob, buf_t& out_public_key_blob, buf_t& out_private_scalar_fixed);

// Restore a full key blob by attaching a fixed-length private scalar x into a
// public key blob (produced by `detach_private_scalar`).
//
// This validates that x matches the key blob by checking:
// - `public_share_compressed` matches the blob's Qi_self, and
// - x*G == Qi_self.
//
// Input:
// - `private_scalar_fixed` must be a fixed-length big-endian scalar encoding with
//   length equal to the curve order size in bytes.
// - `public_share_compressed` must be the SEC1 compressed point encoding of
//   this party's share public point (Qi_self), e.g. from `get_public_share_compressed`.
error_t attach_private_scalar(mem_t public_key_blob, mem_t private_scalar_fixed, mem_t public_share_compressed,
                              buf_t& out_key_blob);

}  // namespace coinbase::api::ecdsa_mp
