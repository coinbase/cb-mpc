#include <cbmpc/api/hd_keyset_ecdsa_2p.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/protocol/ecdsa_2p.h>
#include <cbmpc/internal/protocol/hd_keyset_ecdsa_2p.h>
#include <cbmpc/internal/protocol/hd_tree_bip32.h>

#include "curve_util.h"
#include "hd_keyset_util.h"
#include "job_util.h"
#include "mem_util.h"

namespace coinbase::api::hd_keyset_ecdsa_2p {

namespace {

constexpr uint32_t keyset_blob_version_v1 = 1;

using coinbase::api::detail::from_internal_curve;
using coinbase::api::detail::to_internal_bip32_path;
using coinbase::api::detail::to_internal_curve;
using coinbase::api::detail::to_internal_job;
using coinbase::api::detail::to_internal_party;
using coinbase::api::detail::validate_job_2p;
using coinbase::api::detail::validate_no_duplicate_bip32_paths;

// Mirror of the `coinbase::api::ecdsa_2p` key blob format (see `src/cbmpc/api/ecdsa2pc.cpp`).
constexpr uint32_t ecdsa2pc_key_blob_version_v1 = 1;
struct ecdsa2pc_key_blob_v1_t {
  uint32_t version = ecdsa2pc_key_blob_version_v1;
  uint32_t role = 0;   // 0=p1, 1=p2
  uint32_t curve = 0;  // coinbase::api::curve_id

  buf_t Q_compressed;
  coinbase::crypto::bn_t x_share;
  coinbase::crypto::bn_t c_key;
  coinbase::crypto::paillier_t paillier;

  void convert(coinbase::converter_t& c) { c.convert(version, role, curve, Q_compressed, x_share, c_key, paillier); }
};

static error_t serialize_ecdsa2pc_key_blob(const coinbase::mpc::ecdsa2pc::key_t& key, buf_t& out) {
  curve_id cid;
  if (!from_internal_curve(key.curve, cid)) return coinbase::error(E_BADARG, "unsupported curve");
  if (cid == curve_id::ed25519) return coinbase::error(E_BADARG, "unsupported curve");

  ecdsa2pc_key_blob_v1_t blob;
  blob.role = static_cast<uint32_t>(key.role);
  blob.curve = static_cast<uint32_t>(cid);
  blob.Q_compressed = key.Q.to_compressed_bin();
  blob.x_share = key.x_share;
  blob.c_key = key.c_key;
  blob.paillier = key.paillier;
  out = coinbase::convert(blob);
  return SUCCESS;
}

struct keyset_blob_v1_t {
  uint32_t version = keyset_blob_version_v1;
  uint32_t role = 0;   // 0=p1, 1=p2
  uint32_t curve = 0;  // coinbase::api::curve_id

  buf_t root_Q_compressed;
  buf_t root_K_compressed;
  coinbase::crypto::bn_t x_share;
  coinbase::crypto::bn_t k_share;
  coinbase::crypto::paillier_t paillier;
  coinbase::crypto::bn_t c_key;

  void convert(coinbase::converter_t& c) {
    c.convert(version, role, curve, root_Q_compressed, root_K_compressed, x_share, k_share, paillier, c_key);
  }
};

static error_t blob_to_keyset(const keyset_blob_v1_t& blob, coinbase::mpc::key_share_ecdsa_hdmpc_2p_t& keyset) {
  if (blob.role > 1) return coinbase::error(E_FORMAT, "invalid keyset blob role");

  const auto cid = static_cast<curve_id>(blob.curve);
  if (cid == curve_id::ed25519) return coinbase::error(E_FORMAT, "invalid keyset blob curve");
  const auto icurve = to_internal_curve(cid);
  if (!icurve.valid()) return coinbase::error(E_FORMAT, "invalid keyset blob curve");

  keyset.party_index = static_cast<coinbase::mpc::party_idx_t>(blob.role);
  keyset.curve = icurve;

  const coinbase::crypto::mod_t& q = keyset.curve.order();
  if (!q.is_in_range(blob.k_share)) return coinbase::error(E_FORMAT, "invalid keyset blob");

  keyset.root.x_share = blob.x_share;
  keyset.root.k_share = blob.k_share;
  keyset.paillier = blob.paillier;
  keyset.c_key = blob.c_key;

  // For ECDSA-2PC, party p1 maintains `x_share` as an integer representative compatible with Paillier plaintext
  // operations (it is intentionally not reduced modulo q after refresh). Enforce only that it fits in Z_N.
  const coinbase::crypto::mod_t& N = keyset.paillier.get_N();
  if (!N.is_in_range(blob.x_share)) return coinbase::error(E_FORMAT, "invalid keyset blob");

  error_t rv = keyset.root.Q.from_bin(icurve, blob.root_Q_compressed);
  if (rv) return rv;
  return keyset.root.K.from_bin(icurve, blob.root_K_compressed);
}

static error_t deserialize_keyset_blob(mem_t in, coinbase::mpc::key_share_ecdsa_hdmpc_2p_t& out) {
  keyset_blob_v1_t blob;
  error_t rv = coinbase::convert(blob, in);
  if (rv) return rv;
  if (blob.version != keyset_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported keyset blob version");
  return blob_to_keyset(blob, out);
}

static error_t serialize_keyset_blob(const coinbase::mpc::key_share_ecdsa_hdmpc_2p_t& keyset, buf_t& out) {
  curve_id cid;
  if (!from_internal_curve(keyset.curve, cid)) return coinbase::error(E_BADARG, "unsupported curve");
  if (cid == curve_id::ed25519) return coinbase::error(E_BADARG, "unsupported curve");

  keyset_blob_v1_t blob;
  blob.role = static_cast<uint32_t>(keyset.party_index);
  blob.curve = static_cast<uint32_t>(cid);
  blob.root_Q_compressed = keyset.root.Q.to_compressed_bin();
  blob.root_K_compressed = keyset.root.K.to_compressed_bin();
  blob.x_share = keyset.root.x_share;
  blob.k_share = keyset.root.k_share;
  blob.paillier = keyset.paillier;
  blob.c_key = keyset.c_key;
  out = coinbase::convert(blob);
  return SUCCESS;
}

}  // namespace

error_t dkg(const coinbase::api::job_2p_t& job, curve_id curve, buf_t& keyset_blob) {
  if (const error_t rv = validate_job_2p(job)) return rv;
  if (curve == curve_id::ed25519) return coinbase::error(E_BADARG, "unsupported curve");
  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  coinbase::mpc::job_2p_t mpc_job = to_internal_job(job);

  coinbase::mpc::key_share_ecdsa_hdmpc_2p_t keyset;
  const error_t rv = coinbase::mpc::key_share_ecdsa_hdmpc_2p_t::dkg(mpc_job, icurve, keyset);
  if (rv) return rv;

  return serialize_keyset_blob(keyset, keyset_blob);
}

error_t refresh(const coinbase::api::job_2p_t& job, mem_t keyset_blob, buf_t& new_keyset_blob) {
  if (const error_t rv = validate_job_2p(job)) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(keyset_blob, "keyset_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::key_share_ecdsa_hdmpc_2p_t keyset;
  error_t rv = deserialize_keyset_blob(keyset_blob, keyset);
  if (rv) return rv;

  const auto self = to_internal_party(job.self);
  if (static_cast<uint32_t>(keyset.party_index) != static_cast<uint32_t>(self))
    return coinbase::error(E_BADARG, "job.self mismatch keyset blob role");

  coinbase::mpc::job_2p_t mpc_job = to_internal_job(job);

  coinbase::mpc::key_share_ecdsa_hdmpc_2p_t new_keyset;
  rv = coinbase::mpc::key_share_ecdsa_hdmpc_2p_t::refresh(mpc_job, keyset, new_keyset);
  if (rv) return rv;

  return serialize_keyset_blob(new_keyset, new_keyset_blob);
}

error_t derive_ecdsa_2p_keys(const coinbase::api::job_2p_t& job, mem_t keyset_blob, const bip32_path_t& hardened_path,
                             const std::vector<bip32_path_t>& non_hardened_paths, buf_t& sid,
                             std::vector<buf_t>& out_ecdsa_2p_key_blobs) {
  if (const error_t rv = validate_job_2p(job)) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(keyset_blob, "keyset_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::key_share_ecdsa_hdmpc_2p_t keyset;
  error_t rv = deserialize_keyset_blob(keyset_blob, keyset);
  if (rv) return rv;

  const auto self = to_internal_party(job.self);
  if (static_cast<uint32_t>(keyset.party_index) != static_cast<uint32_t>(self))
    return coinbase::error(E_BADARG, "job.self mismatch keyset blob role");

  rv = validate_no_duplicate_bip32_paths(non_hardened_paths);
  if (rv) return rv;

  coinbase::mpc::job_2p_t mpc_job = to_internal_job(job);

  const coinbase::mpc::bip32_path_t hardened_path_internal = to_internal_bip32_path(hardened_path);
  std::vector<coinbase::mpc::bip32_path_t> non_hardened_paths_internal;
  non_hardened_paths_internal.reserve(non_hardened_paths.size());
  for (const auto& p : non_hardened_paths) non_hardened_paths_internal.push_back(to_internal_bip32_path(p));

  std::vector<coinbase::mpc::ecdsa2pc::key_t> derived_keys(non_hardened_paths.size());
  rv = coinbase::mpc::key_share_ecdsa_hdmpc_2p_t::derive_keys(mpc_job, keyset, hardened_path_internal,
                                                              non_hardened_paths_internal, sid, derived_keys);
  if (rv) {
    out_ecdsa_2p_key_blobs.clear();
    return rv;
  }

  std::vector<buf_t> blobs;
  blobs.resize(derived_keys.size());
  for (size_t i = 0; i < derived_keys.size(); i++) {
    rv = serialize_ecdsa2pc_key_blob(derived_keys[i], blobs[i]);
    if (rv) {
      out_ecdsa_2p_key_blobs.clear();
      return rv;
    }
  }

  out_ecdsa_2p_key_blobs = std::move(blobs);
  return SUCCESS;
}

error_t extract_root_public_key_compressed(mem_t keyset_blob, buf_t& out_Q_compressed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(keyset_blob, "keyset_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::key_share_ecdsa_hdmpc_2p_t keyset;
  const error_t rv = deserialize_keyset_blob(keyset_blob, keyset);
  if (rv) return rv;
  out_Q_compressed = keyset.root.Q.to_compressed_bin();
  return SUCCESS;
}

}  // namespace coinbase::api::hd_keyset_ecdsa_2p
