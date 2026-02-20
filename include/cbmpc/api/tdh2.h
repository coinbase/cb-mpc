#pragma once

#include <string_view>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/core/access_structure.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>

// All the functions have two versions: additive and ac. Additive means that the
// sharing is additive, while ac means that the sharing is according to a given access structure.
namespace coinbase::api::tdh2 {

// Run the multi-party key generation protocol for TDH2 (additive shares).
//
// Outputs:
// - `sid`: session id used by the protocol (output on all parties).
// - `public_key`: TDH2 public key blob (same on all parties).
// - `public_shares`: compressed public shares Qi (same on all parties), ordered
//   to match `job.party_names` (index i corresponds to `job.party_names[i]`).
// - `private_share`: opaque, versioned private share blob for this party only.
//
// Supported curves: `curve_id::p256`, `curve_id::secp256k1`.
error_t dkg_additive(const coinbase::api::job_mp_t& job, curve_id curve, buf_t& public_key,
                     std::vector<buf_t>& public_shares, buf_t& private_share, buf_t& sid);

// Run the multi-party key generation protocol for TDH2 with a
// general access structure.
//
// Notes:
// - This is an n-party protocol (all parties in `job.party_names`
//   participate), but only the provided `quorum_party_names` actively contribute
//   to the generated key shares.
// - `sid` is an in/out session id used by the protocol; callers may pass an
//   empty buffer and let the protocol derive one.
//
// Output semantics match `dkg_additive()`.
//
// Supported curves: `curve_id::p256`, `curve_id::secp256k1`.
error_t dkg_ac(const coinbase::api::job_mp_t& job, curve_id curve, buf_t& sid,
               const access_structure_t& access_structure, const std::vector<std::string_view>& quorum_party_names,
               buf_t& public_key, std::vector<buf_t>& public_shares, buf_t& private_share);

// Encrypt using a TDH2 public key.
error_t encrypt(mem_t public_key, mem_t plaintext, mem_t label, buf_t& ciphertext);

// Verify ciphertext validity for a given public key and label.
error_t verify(mem_t public_key, mem_t ciphertext, mem_t label);

// Locally compute a partial decryption from a private share.
error_t partial_decrypt(mem_t private_share, mem_t ciphertext, mem_t label, buf_t& partial_decryption);

// Combine additive shares + partial decryptions to decrypt.
error_t combine_additive(mem_t public_key, const std::vector<mem_t>& public_shares, mem_t label,
                         const std::vector<mem_t>& partial_decryptions, mem_t ciphertext, buf_t& plaintext);

// Combine access-structure shares + partial decryptions to decrypt.
//
// - `party_names` and `public_shares` define the mapping name -> Qi for *all*
//   parties in the access structure.
// - `partial_decryption_party_names` and `partial_decryptions` provide the quorum
//   subset used for decryption.
//
// Requirements:
// - `party_names.size() == public_shares.size()`
// - `partial_decryption_party_names.size() == partial_decryptions.size()`
// - The leaf set of `access_structure` must match `party_names` exactly.
error_t combine_ac(const access_structure_t& access_structure, mem_t public_key,
                   const std::vector<std::string_view>& party_names, const std::vector<mem_t>& public_shares,
                   mem_t label, const std::vector<std::string_view>& partial_decryption_party_names,
                   const std::vector<mem_t>& partial_decryptions, mem_t ciphertext, buf_t& plaintext);

}  // namespace coinbase::api::tdh2
