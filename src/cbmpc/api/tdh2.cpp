#include <set>
#include <string>

#include <cbmpc/api/tdh2.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/crypto/ro.h>
#include <cbmpc/internal/crypto/tdh2.h>
#include <cbmpc/internal/protocol/ec_dkg.h>

#include "access_structure_util.h"
#include "curve_util.h"
#include "job_util.h"
#include "mem_util.h"

namespace coinbase::api::tdh2 {

namespace {

constexpr uint32_t private_share_blob_version_v1 = 1;

using coinbase::api::detail::to_internal_curve;
using coinbase::api::detail::to_internal_job;
using coinbase::api::detail::validate_job_mp;

static error_t validate_public_key(const coinbase::crypto::tdh2::public_key_t& pk) {
  if (!pk.Q.valid() || !pk.Gamma.valid()) return coinbase::error(E_FORMAT, "invalid public key");
  const auto curve = pk.Q.get_curve();
  if (!curve.valid()) return coinbase::error(E_FORMAT, "invalid public key");
  if (pk.Gamma.get_curve() != curve) return coinbase::error(E_FORMAT, "invalid public key");

  // Enforce subgroup / infinity checks (DoS resistance and misuse-hardening).
  if (curve.check(pk.Q)) return coinbase::error(E_FORMAT, "invalid public key");
  if (curve.check(pk.Gamma)) return coinbase::error(E_FORMAT, "invalid public key");

  // `Gamma` is deterministically derived from `(Q, sid)` and must match.
  const auto expected_gamma = coinbase::crypto::ro::hash_curve(mem_t("TDH2-Gamma"), pk.Q, pk.sid).curve(curve);
  if (pk.Gamma != expected_gamma) return coinbase::error(E_FORMAT, "invalid public key");
  return SUCCESS;
}

struct private_share_blob_v1_t {
  uint32_t version = private_share_blob_version_v1;
  uint32_t curve = 0;  // coinbase::api::curve_id

  // Role/index id: 1..n, aligned with `job.party_names` order.
  int rid = 0;
  coinbase::crypto::bn_t x;
  coinbase::crypto::tdh2::public_key_t pub_key;

  void convert(coinbase::converter_t& c) { c.convert(version, curve, rid, x, pub_key); }
};

error_t deserialize_private_share(mem_t in, coinbase::crypto::tdh2::private_share_t& out) {
  private_share_blob_v1_t blob;
  error_t rv = coinbase::convert(blob, in);
  if (rv) return rv;
  if (blob.version != private_share_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported private share blob");

  const auto cid = static_cast<curve_id>(blob.curve);
  if (cid == curve_id::ed25519) return coinbase::error(E_FORMAT, "invalid private share curve");
  const auto curve = to_internal_curve(cid);
  if (!curve.valid()) return coinbase::error(E_FORMAT, "invalid private share curve");

  if (blob.rid <= 0) return coinbase::error(E_FORMAT, "invalid private share blob");
  if (curve.order().is_in_range(blob.x) == false) return coinbase::error(E_FORMAT, "invalid private share blob");

  if (const error_t pk_rv = validate_public_key(blob.pub_key)) return pk_rv;
  if (blob.pub_key.Q.get_curve() != curve) return coinbase::error(E_FORMAT, "invalid private share blob");

  out.rid = blob.rid;
  out.x = blob.x;
  out.pub_key = blob.pub_key;
  return SUCCESS;
}

static error_t serialize_public_outputs(const coinbase::api::job_mp_t& job,
                                        const coinbase::mpc::eckey::key_share_mp_t& key, const buf_t& sid,
                                        buf_t& public_key, std::vector<buf_t>& public_shares) {
  const coinbase::crypto::tdh2::public_key_t pk(key.Q, sid);
  public_key = pk.to_bin();

  public_shares.clear();
  public_shares.reserve(job.party_names.size());
  for (const auto& name_view : job.party_names) {
    const std::string name(name_view);
    const auto it = key.Qis.find(name);
    if (it == key.Qis.end()) return coinbase::error(E_FORMAT, "DKG output missing public share");
    public_shares.emplace_back(it->second.to_compressed_bin());
  }
  return SUCCESS;
}

static error_t serialize_private_share(curve_id curve, coinbase::api::party_idx_t self,
                                       const coinbase::crypto::bn_t& x_share,
                                       const coinbase::crypto::tdh2::public_key_t& pk, buf_t& private_share) {
  private_share_blob_v1_t blob;
  blob.curve = static_cast<uint32_t>(curve);
  blob.rid = static_cast<int>(self) + 1;  // 1..n (aligned with job.party_names order)
  blob.x = x_share;
  blob.pub_key = pk;
  private_share = coinbase::convert(blob);
  return SUCCESS;
}

}  // namespace

error_t dkg_additive(const coinbase::api::job_mp_t& job, curve_id curve, buf_t& public_key,
                     std::vector<buf_t>& public_shares, buf_t& private_share, buf_t& sid) {
  error_t rv = validate_job_mp(job);
  if (rv) return rv;

  if (curve == curve_id::ed25519) return coinbase::error(E_BADARG, "unsupported curve");
  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  coinbase::mpc::job_mp_t mpc_job = to_internal_job(job);
  coinbase::mpc::eckey::key_share_mp_t key;

  public_key.free();
  private_share.free();
  sid.free();
  public_shares.clear();

  rv = coinbase::mpc::eckey::key_share_mp_t::dkg(mpc_job, icurve, key, sid);
  if (rv) return rv;

  rv = serialize_public_outputs(job, key, sid, public_key, public_shares);
  if (rv) return rv;

  // Deserialize the public key blob so we can embed the exact serialized form in the private share.
  coinbase::crypto::tdh2::public_key_t pk;
  rv = pk.from_bin(public_key);
  if (rv) return rv;

  return serialize_private_share(curve, job.self, key.x_share, pk, private_share);
}

error_t dkg_ac(const coinbase::api::job_mp_t& job, curve_id curve, buf_t& sid,
               const access_structure_t& access_structure, const std::vector<std::string_view>& quorum_party_names,
               buf_t& public_key, std::vector<buf_t>& public_shares, buf_t& private_share) {
  error_t rv = validate_job_mp(job);
  if (rv) return rv;

  if (curve == curve_id::ed25519) return coinbase::error(E_BADARG, "unsupported curve");
  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  coinbase::crypto::ss::ac_owned_t ac;
  rv = coinbase::api::detail::to_internal_access_structure(access_structure, job.party_names, icurve, ac);
  if (rv) return rv;

  coinbase::mpc::party_set_t quorum_party_set;
  rv = coinbase::api::detail::to_internal_party_set(job.party_names, quorum_party_names, quorum_party_set);
  if (rv) return rv;

  coinbase::mpc::job_mp_t mpc_job = to_internal_job(job);
  coinbase::mpc::eckey::key_share_mp_t key;

  public_key.free();
  private_share.free();
  public_shares.clear();

  rv = coinbase::mpc::eckey::key_share_mp_t::dkg_ac(mpc_job, icurve, sid, ac, quorum_party_set, key);
  if (rv) return rv;

  rv = serialize_public_outputs(job, key, sid, public_key, public_shares);
  if (rv) return rv;

  coinbase::crypto::tdh2::public_key_t pk;
  rv = pk.from_bin(public_key);
  if (rv) return rv;

  return serialize_private_share(curve, job.self, key.x_share, pk, private_share);
}

error_t encrypt(mem_t public_key, mem_t plaintext, mem_t label, buf_t& ciphertext) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(public_key, "public_key",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(plaintext, "plaintext")) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;

  coinbase::crypto::tdh2::public_key_t pk;
  error_t rv = pk.from_bin(public_key);
  if (rv) return rv;
  if (rv = validate_public_key(pk)) return rv;

  const auto ct = pk.encrypt(plaintext, label);
  ciphertext = coinbase::convert(ct);
  return SUCCESS;
}

error_t verify(mem_t public_key, mem_t ciphertext, mem_t label) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(public_key, "public_key",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;

  coinbase::crypto::tdh2::public_key_t pk;
  error_t rv = pk.from_bin(public_key);
  if (rv) return rv;
  if (rv = validate_public_key(pk)) return rv;

  coinbase::crypto::tdh2::ciphertext_t ct;
  rv = coinbase::convert(ct, ciphertext);
  if (rv) return rv;
  return ct.verify(pk, label);
}

error_t partial_decrypt(mem_t private_share, mem_t ciphertext, mem_t label, buf_t& partial_decryption) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(private_share, "private_share",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;

  coinbase::crypto::tdh2::private_share_t share;
  error_t rv = deserialize_private_share(private_share, share);
  if (rv) return rv;

  coinbase::crypto::tdh2::ciphertext_t ct;
  rv = coinbase::convert(ct, ciphertext);
  if (rv) return rv;

  coinbase::crypto::tdh2::partial_decryption_t partial;
  rv = share.decrypt(ct, label, partial);
  if (rv) return rv;

  partial_decryption = coinbase::convert(partial);
  return SUCCESS;
}

error_t combine_additive(mem_t public_key, const std::vector<mem_t>& public_shares, mem_t label,
                         const std::vector<mem_t>& partial_decryptions, mem_t ciphertext, buf_t& plaintext) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(public_key, "public_key",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_vec_arg_max_size(
          public_shares, "public_shares", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_vec_arg_max_size(
          partial_decryptions, "partial_decryptions", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;

  if (public_shares.size() != partial_decryptions.size())
    return coinbase::error(E_BADARG, "public_shares and partial_decryptions size mismatch");

  coinbase::crypto::tdh2::public_key_t pk;
  error_t rv = pk.from_bin(public_key);
  if (rv) return rv;
  if (rv = validate_public_key(pk)) return rv;

  coinbase::crypto::tdh2::ciphertext_t ct;
  rv = coinbase::convert(ct, ciphertext);
  if (rv) return rv;

  auto curve = pk.Q.get_curve();
  if (!curve.valid()) return coinbase::error(E_FORMAT, "public key missing curve");

  coinbase::crypto::tdh2::pub_shares_t Qi;
  Qi.reserve(public_shares.size());
  for (const auto& m : public_shares) {
    coinbase::crypto::ecc_point_t P;
    rv = P.from_bin(curve, m);
    if (rv) return rv;
    Qi.emplace_back(std::move(P));
  }

  coinbase::crypto::tdh2::partial_decryptions_t partials;
  partials.resize(partial_decryptions.size());
  for (size_t i = 0; i < partial_decryptions.size(); i++) {
    rv = coinbase::convert(partials[i], partial_decryptions[i]);
    if (rv) return rv;
  }

  return coinbase::crypto::tdh2::combine_additive(pk, Qi, label, partials, ct, plaintext);
}

error_t combine_ac(const access_structure_t& access_structure, mem_t public_key,
                   const std::vector<std::string_view>& party_names, const std::vector<mem_t>& public_shares,
                   mem_t label, const std::vector<std::string_view>& partial_decryption_party_names,
                   const std::vector<mem_t>& partial_decryptions, mem_t ciphertext, buf_t& plaintext) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(public_key, "public_key",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_vec_arg_max_size(
          public_shares, "public_shares", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_vec_arg_max_size(
          partial_decryptions, "partial_decryptions", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;

  if (party_names.size() != public_shares.size())
    return coinbase::error(E_BADARG, "party_names and public_shares size mismatch");
  if (partial_decryption_party_names.size() != partial_decryptions.size())
    return coinbase::error(E_BADARG, "partial_decryption_party_names and partial_decryptions size mismatch");

  coinbase::crypto::tdh2::public_key_t pk;
  error_t rv = pk.from_bin(public_key);
  if (rv) return rv;
  if (rv = validate_public_key(pk)) return rv;

  const auto curve = pk.Q.get_curve();
  if (!curve.valid()) return coinbase::error(E_FORMAT, "public key missing curve");

  // Convert access structure to internal representation and validate that leaf names match `party_names`.
  coinbase::crypto::ss::ac_owned_t ac;
  rv = coinbase::api::detail::to_internal_access_structure(access_structure, party_names, curve, ac);
  if (rv) return rv;

  coinbase::crypto::tdh2::ciphertext_t ct;
  rv = coinbase::convert(ct, ciphertext);
  if (rv) return rv;

  coinbase::crypto::ss::ac_pub_shares_t pub_shares_map;
  pub_shares_map.clear();
  std::set<std::string_view> seen_party_names;
  for (size_t i = 0; i < party_names.size(); i++) {
    const auto name_view = party_names[i];
    if (name_view.empty()) return coinbase::error(E_BADARG, "party name must be non-empty");
    if (!seen_party_names.insert(name_view).second) return coinbase::error(E_BADARG, "duplicate party name");
    coinbase::crypto::ecc_point_t Qi;
    rv = Qi.from_bin(curve, public_shares[i]);
    if (rv) return rv;
    pub_shares_map.emplace(std::string(name_view), std::move(Qi));
  }

  coinbase::crypto::ss::party_map_t<coinbase::crypto::tdh2::partial_decryption_t> partials_map;
  partials_map.clear();
  std::set<std::string_view> seen_partial_names;
  for (size_t i = 0; i < partial_decryptions.size(); i++) {
    const auto name_view = partial_decryption_party_names[i];
    if (name_view.empty()) return coinbase::error(E_BADARG, "partial decryption party name must be non-empty");
    if (!seen_partial_names.insert(name_view).second)
      return coinbase::error(E_BADARG, "duplicate partial decryption party name");

    const std::string name(name_view);
    if (pub_shares_map.find(name) == pub_shares_map.end())
      return coinbase::error(E_BADARG, "partial decryption party name not in party_names");

    coinbase::crypto::tdh2::partial_decryption_t partial;
    rv = coinbase::convert(partial, partial_decryptions[i]);
    if (rv) return rv;
    partials_map[name] = std::move(partial);
  }

  return coinbase::crypto::tdh2::combine(ac, pk, pub_shares_map, label, partials_map, ct, plaintext);
}

}  // namespace coinbase::api::tdh2
