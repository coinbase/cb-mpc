#pragma once

#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/core/bip32_path.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>

namespace coinbase::api::hd_keyset_eddsa_2p {

using coinbase::api::bip32_path_t;

// Run the 2-party HD keyset initialization protocol (EdDSA / Ed25519).
//
// The output `keyset_blob` is a versioned, opaque byte string that can be persisted
// by the caller and used for refresh / derivation.
//
// Supported curves: `curve_id::ed25519`.
error_t dkg(const coinbase::api::job_2p_t& job, curve_id curve, buf_t& keyset_blob);

// Refresh an existing HD keyset share, producing a new share (same public root keys).
error_t refresh(const coinbase::api::job_2p_t& job, mem_t keyset_blob, buf_t& new_keyset_blob);

// Derive per-path EdDSA-2PC key blobs from an HD keyset.
//
// - `hardened_path` selects the hardened derivation branch (VRF-based step).
// - `non_hardened_paths` are applied to the derived public key using BIP32 non-hardened steps.
// - `sid` is an in/out session id used by the protocol; callers may pass an empty
//   buffer and let the protocol derive one.
//
// Output:
// - `out_eddsa_2p_key_blobs.size() == non_hardened_paths.size()`
// - Each element is an `eddsa_2p` key blob compatible with `coinbase::api::eddsa_2p::*`.
error_t derive_eddsa_2p_keys(const coinbase::api::job_2p_t& job, mem_t keyset_blob, const bip32_path_t& hardened_path,
                             const std::vector<bip32_path_t>& non_hardened_paths, buf_t& sid,
                             std::vector<buf_t>& out_eddsa_2p_key_blobs);

// Extract the compressed root public key Q from a keyset blob.
//
// Output encoding matches `coinbase::api::eddsa_2p::get_public_key_compressed`:
// a 32-byte Ed25519 compressed public key.
error_t extract_root_public_key_compressed(mem_t keyset_blob, buf_t& out_Q_compressed);

}  // namespace coinbase::api::hd_keyset_eddsa_2p
