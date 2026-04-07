#include <map>
#include <set>
#include <string>
#include <string_view>

#include <cbmpc/api/schnorr_mp.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/protocol/schnorr_mp.h>

#include "access_structure_util.h"
#include "job_util.h"
#include "mem_util.h"

namespace coinbase::api::schnorr_mp {

namespace {

constexpr uint32_t key_blob_version_v1 = 1;
constexpr uint32_t ac_key_blob_version_v1 = 2;

using coinbase::api::detail::to_internal_job;
using coinbase::api::detail::validate_job_mp;

struct key_blob_v1_t {
  uint32_t version = key_blob_version_v1;
  uint32_t curve = 0;  // coinbase::api::curve_id

  std::string party_name;  // self identity (name-bound, not index-bound)

  buf_t Q_compressed;
  std::map<std::string, buf_t> Qis_compressed;  // name -> compressed Qi

  coinbase::crypto::bn_t x_share;

  void convert(coinbase::converter_t& c) {
    c.convert(version, curve, party_name, Q_compressed, Qis_compressed, x_share);
  }
};

static error_t validate_aggregate_public_key(
    const coinbase::crypto::ecurve_t& curve, const coinbase::crypto::ecc_point_t& Q,
    const coinbase::crypto::ss::party_map_t<coinbase::crypto::ecc_point_t>& Qis) {
  coinbase::crypto::ecc_point_t Q_sum = curve.infinity();
  for (const auto& kv : Qis) Q_sum += kv.second;
  if (Q != Q_sum) return coinbase::error(E_FORMAT, "invalid key blob");
  return SUCCESS;
}

static error_t parse_Qis(const std::map<std::string, buf_t>& Qis_compressed, const coinbase::crypto::ecurve_t& curve,
                         coinbase::crypto::ss::party_map_t<coinbase::crypto::ecc_point_t>& Qis) {
  error_t rv = UNINITIALIZED_ERROR;
  Qis.clear();
  for (const auto& kv : Qis_compressed) {
    coinbase::crypto::ecc_point_t Qi;
    if (rv = Qi.from_bin(curve, kv.second)) return coinbase::error(rv, "invalid key blob");
    Qis[kv.first] = std::move(Qi);
  }
  return SUCCESS;
}

static error_t extract_Q_from_key_blob(mem_t in, coinbase::crypto::ecc_point_t& Q) {
  key_blob_v1_t blob;
  error_t rv = coinbase::convert(blob, in);
  if (rv) return rv;
  if (blob.version != key_blob_version_v1 && blob.version != ac_key_blob_version_v1)
    return coinbase::error(E_FORMAT, "unsupported key blob version");
  if (static_cast<curve_id>(blob.curve) != curve_id::secp256k1)
    return coinbase::error(E_FORMAT, "invalid key blob curve");
  if (blob.Q_compressed.empty()) return coinbase::error(E_FORMAT, "invalid key blob");
  const auto curve = coinbase::crypto::curve_secp256k1;
  if (rv = Q.from_bin(curve, blob.Q_compressed)) return coinbase::error(rv, "invalid key blob");

  coinbase::crypto::ss::party_map_t<coinbase::crypto::ecc_point_t> Qis;
  if (rv = parse_Qis(blob.Qis_compressed, curve, Qis)) return rv;
  return validate_aggregate_public_key(curve, Q, Qis);
}

static error_t serialize_key_blob_for_party_names(const std::vector<std::string_view>& party_names,
                                                  const std::string& self_name,
                                                  const coinbase::mpc::schnorrmp::key_t& key, uint32_t version,
                                                  buf_t& out) {
  if (key.curve != coinbase::crypto::curve_secp256k1) return coinbase::error(E_BADARG, "unsupported curve");

  const std::string_view self_sv(self_name);
  bool self_in_party_names = false;
  for (const auto& name_view : party_names) {
    if (name_view == self_sv) {
      self_in_party_names = true;
      break;
    }
  }
  if (!self_in_party_names) return coinbase::error(E_BADARG, "self_name not in party_names");
  if (key.party_name != self_name) return coinbase::error(E_BADARG, "job.self mismatch key");

  key_blob_v1_t blob;
  blob.version = version;
  blob.curve = static_cast<uint32_t>(curve_id::secp256k1);
  blob.party_name = key.party_name;
  blob.Q_compressed = key.Q.to_compressed_bin();
  blob.x_share = key.x_share;

  for (const auto& name_view : party_names) {
    const std::string name(name_view);
    const auto it = key.Qis.find(name);
    if (it == key.Qis.end()) return coinbase::error(E_FORMAT, "key missing Qi");
    blob.Qis_compressed[name] = it->second.to_compressed_bin();
  }

  out = coinbase::convert(blob);
  return SUCCESS;
}

static error_t serialize_key_blob(const coinbase::api::job_mp_t& job, const coinbase::mpc::schnorrmp::key_t& key,
                                  buf_t& out) {
  if (job.self < 0 || static_cast<size_t>(job.self) >= job.party_names.size())
    return coinbase::error(E_BADARG, "invalid job.self");

  const std::string self_name(job.party_names[static_cast<size_t>(job.self)]);
  return serialize_key_blob_for_party_names(job.party_names, self_name, key, key_blob_version_v1, out);
}

static error_t serialize_ac_key_blob(const coinbase::api::job_mp_t& job, const coinbase::mpc::schnorrmp::key_t& key,
                                     buf_t& out) {
  if (job.self < 0 || static_cast<size_t>(job.self) >= job.party_names.size())
    return coinbase::error(E_BADARG, "invalid job.self");

  const std::string self_name(job.party_names[static_cast<size_t>(job.self)]);
  return serialize_key_blob_for_party_names(job.party_names, self_name, key, ac_key_blob_version_v1, out);
}

static error_t deserialize_key_blob(const coinbase::api::job_mp_t& job, mem_t in,
                                    coinbase::mpc::schnorrmp::key_t& key) {
  error_t rv = UNINITIALIZED_ERROR;

  if (job.self < 0 || static_cast<size_t>(job.self) >= job.party_names.size())
    return coinbase::error(E_BADARG, "invalid job.self");
  const std::string self_name(job.party_names[static_cast<size_t>(job.self)]);

  key_blob_v1_t blob;
  if (rv = coinbase::convert(blob, in)) return rv;
  if (blob.version != key_blob_version_v1)
    return coinbase::error(E_FORMAT, "unsupported key blob version: " + std::to_string(blob.version));
  if (static_cast<curve_id>(blob.curve) != curve_id::secp256k1)
    return coinbase::error(E_FORMAT, "invalid key blob curve");
  if (blob.party_name.empty()) return coinbase::error(E_FORMAT, "invalid key blob");
  if (blob.party_name != self_name) return coinbase::error(E_BADARG, "job.self mismatch key blob");
  if (blob.Qis_compressed.size() != job.party_names.size()) return coinbase::error(E_BADARG, "invalid key blob");

  // Ensure the party name set matches the job (order can differ).
  for (const auto& name_view : job.party_names) {
    const std::string name(name_view);
    if (blob.Qis_compressed.find(name) == blob.Qis_compressed.end())
      return coinbase::error(E_BADARG, "job.party_names mismatch key blob");
  }

  const auto curve = coinbase::crypto::curve_secp256k1;
  const coinbase::crypto::mod_t& q = curve.order();
  if (!q.is_in_range(blob.x_share)) return coinbase::error(E_FORMAT, "invalid key blob");

  coinbase::crypto::ecc_point_t Q;
  if (rv = Q.from_bin(curve, blob.Q_compressed)) return coinbase::error(rv, "invalid key blob");

  coinbase::crypto::ss::party_map_t<coinbase::crypto::ecc_point_t> Qis;
  for (const auto& name_view : job.party_names) {
    const std::string name(name_view);
    const auto it = blob.Qis_compressed.find(name);
    if (it == blob.Qis_compressed.end()) return coinbase::error(E_BADARG, "job.party_names mismatch key blob");
  }
  if (rv = parse_Qis(blob.Qis_compressed, curve, Qis)) return rv;
  if (rv = validate_aggregate_public_key(curve, Q, Qis)) return rv;

  const auto& G = curve.generator();
  const auto it_self = Qis.find(blob.party_name);
  if (it_self == Qis.end()) return coinbase::error(E_FORMAT, "invalid key blob");
  if (blob.x_share * G != it_self->second) return coinbase::error(E_FORMAT, "invalid key blob");

  key.party_name = blob.party_name;
  key.curve = curve;
  key.x_share = blob.x_share;
  key.Qis = std::move(Qis);
  key.Q = std::move(Q);
  return SUCCESS;
}

static error_t deserialize_ac_key_blob(const coinbase::api::job_mp_t& job, mem_t in,
                                       coinbase::mpc::schnorrmp::key_t& key) {
  error_t rv = UNINITIALIZED_ERROR;

  if (job.self < 0 || static_cast<size_t>(job.self) >= job.party_names.size())
    return coinbase::error(E_BADARG, "invalid job.self");
  const std::string self_name(job.party_names[static_cast<size_t>(job.self)]);

  key_blob_v1_t blob;
  if (rv = coinbase::convert(blob, in)) return rv;
  if (blob.version != ac_key_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported key blob version");
  if (static_cast<curve_id>(blob.curve) != curve_id::secp256k1)
    return coinbase::error(E_FORMAT, "invalid key blob curve");
  if (blob.party_name.empty()) return coinbase::error(E_FORMAT, "invalid key blob");
  if (blob.party_name != self_name) return coinbase::error(E_BADARG, "job.self mismatch key blob");
  if (blob.Qis_compressed.size() != job.party_names.size()) return coinbase::error(E_BADARG, "invalid key blob");

  // Ensure the party name set matches the job (order can differ).
  for (const auto& name_view : job.party_names) {
    const std::string name(name_view);
    if (blob.Qis_compressed.find(name) == blob.Qis_compressed.end())
      return coinbase::error(E_BADARG, "job.party_names mismatch key blob");
  }

  const auto curve = coinbase::crypto::curve_secp256k1;
  const coinbase::crypto::mod_t& q = curve.order();
  if (!q.is_in_range(blob.x_share)) return coinbase::error(E_FORMAT, "invalid key blob");

  coinbase::crypto::ecc_point_t Q;
  if (rv = Q.from_bin(curve, blob.Q_compressed)) return coinbase::error(rv, "invalid key blob");

  coinbase::crypto::ss::party_map_t<coinbase::crypto::ecc_point_t> Qis;
  for (const auto& name_view : job.party_names) {
    const std::string name(name_view);
    const auto it = blob.Qis_compressed.find(name);
    if (it == blob.Qis_compressed.end()) return coinbase::error(E_BADARG, "job.party_names mismatch key blob");
  }
  if (rv = parse_Qis(blob.Qis_compressed, curve, Qis)) return rv;
  if (rv = validate_aggregate_public_key(curve, Q, Qis)) return rv;

  const auto& G = curve.generator();
  const auto it_self = Qis.find(blob.party_name);
  if (it_self == Qis.end()) return coinbase::error(E_FORMAT, "invalid key blob");
  if (blob.x_share * G != it_self->second) return coinbase::error(E_FORMAT, "invalid key blob");

  key.party_name = blob.party_name;
  key.curve = curve;
  key.x_share = blob.x_share;
  key.Qis = std::move(Qis);
  key.Q = std::move(Q);
  return SUCCESS;
}

static error_t deserialize_ac_key_blob(mem_t in, coinbase::mpc::schnorrmp::key_t& key) {
  error_t rv = UNINITIALIZED_ERROR;

  key_blob_v1_t blob;
  if (rv = coinbase::convert(blob, in)) return rv;
  if (blob.version != ac_key_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported key blob version");
  if (static_cast<curve_id>(blob.curve) != curve_id::secp256k1)
    return coinbase::error(E_FORMAT, "invalid key blob curve");
  if (blob.party_name.empty()) return coinbase::error(E_FORMAT, "invalid key blob");
  if (blob.Qis_compressed.empty()) return coinbase::error(E_FORMAT, "invalid key blob");

  const auto curve = coinbase::crypto::curve_secp256k1;
  const coinbase::crypto::mod_t& q = curve.order();
  if (!q.is_in_range(blob.x_share)) return coinbase::error(E_FORMAT, "invalid key blob");

  coinbase::crypto::ecc_point_t Q;
  if (rv = Q.from_bin(curve, blob.Q_compressed)) return coinbase::error(rv, "invalid key blob");

  coinbase::crypto::ss::party_map_t<coinbase::crypto::ecc_point_t> Qis;
  if (rv = parse_Qis(blob.Qis_compressed, curve, Qis)) return rv;
  if (rv = validate_aggregate_public_key(curve, Q, Qis)) return rv;

  const auto& G = curve.generator();
  const auto it_self = Qis.find(blob.party_name);
  if (it_self == Qis.end()) return coinbase::error(E_FORMAT, "invalid key blob");
  if (blob.x_share * G != it_self->second) return coinbase::error(E_FORMAT, "invalid key blob");

  key.party_name = blob.party_name;
  key.curve = curve;
  key.x_share = blob.x_share;
  key.Qis = std::move(Qis);
  key.Q = std::move(Q);
  return SUCCESS;
}

}  // namespace

error_t dkg_additive(const coinbase::api::job_mp_t& job, curve_id curve, buf_t& key_blob, buf_t& sid) {
  error_t rv = validate_job_mp(job);
  if (rv) return rv;
  if (curve != curve_id::secp256k1) return coinbase::error(E_BADARG, "unsupported curve");

  coinbase::mpc::job_mp_t mpc_job = to_internal_job(job);

  coinbase::mpc::schnorrmp::key_t key;
  sid.free();
  key_blob.free();
  rv = coinbase::mpc::schnorrmp::dkg(mpc_job, coinbase::crypto::curve_secp256k1, key, sid);
  if (rv) return rv;

  return serialize_key_blob(job, key, key_blob);
}

error_t dkg_ac(const coinbase::api::job_mp_t& job, curve_id curve, buf_t& sid,
               const access_structure_t& access_structure, const std::vector<std::string_view>& quorum_party_names,
               buf_t& key_blob) {
  error_t rv = validate_job_mp(job);
  if (rv) return rv;
  if (curve != curve_id::secp256k1) return coinbase::error(E_BADARG, "unsupported curve");

  coinbase::crypto::ss::ac_owned_t ac;
  rv = coinbase::api::detail::to_internal_access_structure(access_structure, job.party_names,
                                                           coinbase::crypto::curve_secp256k1, ac);
  if (rv) return rv;

  coinbase::mpc::party_set_t quorum_party_set;
  rv = coinbase::api::detail::to_internal_party_set(job.party_names, quorum_party_names, quorum_party_set);
  if (rv) return rv;

  coinbase::mpc::job_mp_t mpc_job = to_internal_job(job);

  coinbase::mpc::schnorrmp::key_t key;
  key_blob.free();

  rv = coinbase::mpc::schnorrmp::dkg_ac(mpc_job, coinbase::crypto::curve_secp256k1, sid, ac, quorum_party_set, key);
  if (rv) return rv;

  return serialize_ac_key_blob(job, key, key_blob);
}

error_t refresh_additive(const coinbase::api::job_mp_t& job, buf_t& sid, mem_t key_blob, buf_t& new_key_blob) {
  error_t rv = validate_job_mp(job);
  if (rv) return rv;
  if (rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                            coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;

  coinbase::mpc::schnorrmp::key_t key;
  rv = deserialize_key_blob(job, key_blob, key);
  if (rv) return rv;

  coinbase::mpc::job_mp_t mpc_job = to_internal_job(job);

  coinbase::mpc::schnorrmp::key_t new_key;
  new_key_blob.free();
  rv = coinbase::mpc::schnorrmp::refresh(mpc_job, sid, key, new_key);
  if (rv) return rv;

  return serialize_key_blob(job, new_key, new_key_blob);
}

error_t refresh_ac(const coinbase::api::job_mp_t& job, buf_t& sid, mem_t key_blob,
                   const access_structure_t& access_structure, const std::vector<std::string_view>& quorum_party_names,
                   buf_t& new_key_blob) {
  error_t rv = validate_job_mp(job);
  if (rv) return rv;
  if (rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                            coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;

  coinbase::mpc::schnorrmp::key_t key;
  rv = deserialize_ac_key_blob(job, key_blob, key);
  if (rv) return rv;

  coinbase::crypto::ss::ac_owned_t ac;
  rv = coinbase::api::detail::to_internal_access_structure(access_structure, job.party_names, key.curve, ac);
  if (rv) return rv;

  coinbase::mpc::party_set_t quorum_party_set;
  rv = coinbase::api::detail::to_internal_party_set(job.party_names, quorum_party_names, quorum_party_set);
  if (rv) return rv;

  coinbase::mpc::job_mp_t mpc_job = to_internal_job(job);

  coinbase::mpc::schnorrmp::key_t new_key;
  new_key_blob.free();
  rv = coinbase::mpc::schnorrmp::refresh_ac(mpc_job, key.curve, sid, ac, quorum_party_set, key, new_key);
  if (rv) return rv;

  return serialize_ac_key_blob(job, new_key, new_key_blob);
}

error_t sign_ac(const coinbase::api::job_mp_t& job, mem_t ac_key_blob, const access_structure_t& access_structure,
                mem_t msg, party_idx_t sig_receiver, buf_t& sig) {
  error_t rv = validate_job_mp(job);
  if (rv) return rv;
  if (rv = coinbase::api::detail::validate_mem_arg_max_size(ac_key_blob, "ac_key_blob",
                                                            coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (rv = coinbase::api::detail::validate_mem_arg(msg, "msg")) return rv;
  if (msg.size != 32) return coinbase::error(E_BADARG, "BIP340 requires a 32-byte message");
  if (sig_receiver < 0 || static_cast<size_t>(sig_receiver) >= job.party_names.size())
    return coinbase::error(E_BADARG, "invalid sig_receiver");

  coinbase::mpc::schnorrmp::key_t ac_key;
  rv = deserialize_ac_key_blob(ac_key_blob, ac_key);
  if (rv) return rv;

  // Bind the key share to the local party identity in the job.
  const std::string_view self_name_sv(job.party_names[static_cast<size_t>(job.self)]);
  if (ac_key.party_name != self_name_sv) return coinbase::error(E_BADARG, "job.self mismatch key blob");

  // Full party set is the key's Qis key set.
  std::vector<std::string_view> all_party_names;
  all_party_names.reserve(ac_key.Qis.size());
  for (const auto& kv : ac_key.Qis) all_party_names.emplace_back(kv.first);

  // Validate that the signing party set (`job.party_names`) is a subset of the key's party set.
  coinbase::mpc::party_set_t _unused;
  rv = coinbase::api::detail::to_internal_party_set(all_party_names, job.party_names, _unused);
  if (rv) return rv;

  // Convert access structure to internal and validate it matches the key party set.
  coinbase::crypto::ss::ac_owned_t ac;
  rv = coinbase::api::detail::to_internal_access_structure(access_structure, all_party_names, ac_key.curve, ac);
  if (rv) return rv;

  // Convert signing party list to internal set of names.
  std::set<coinbase::crypto::pname_t> quorum_names;
  for (const auto& name : job.party_names) quorum_names.insert(std::string(name));

  coinbase::mpc::schnorrmp::key_t additive_key;
  rv = ac_key.to_additive_share(ac, quorum_names, additive_key);
  if (rv) return rv;

  coinbase::mpc::job_mp_t mpc_job = to_internal_job(job);

  sig.free();
  return coinbase::mpc::schnorrmp::sign(mpc_job, additive_key, msg, sig_receiver, sig,
                                        coinbase::mpc::schnorrmp::variant_e::BIP340);
}

error_t sign_additive(const coinbase::api::job_mp_t& job, mem_t key_blob, mem_t msg, party_idx_t sig_receiver,
                      buf_t& sig) {
  error_t rv = validate_job_mp(job);
  if (rv) return rv;
  if (rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                            coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (rv = coinbase::api::detail::validate_mem_arg(msg, "msg")) return rv;
  if (msg.size != 32) return coinbase::error(E_BADARG, "BIP340 requires a 32-byte message");
  if (sig_receiver < 0 || static_cast<size_t>(sig_receiver) >= job.party_names.size())
    return coinbase::error(E_BADARG, "invalid sig_receiver");

  coinbase::mpc::schnorrmp::key_t key;
  rv = deserialize_key_blob(job, key_blob, key);
  if (rv) return rv;

  coinbase::mpc::job_mp_t mpc_job = to_internal_job(job);

  sig.free();
  return coinbase::mpc::schnorrmp::sign(mpc_job, key, msg, sig_receiver, sig,
                                        coinbase::mpc::schnorrmp::variant_e::BIP340);
}

error_t get_public_key_compressed(mem_t key_blob, buf_t& pub_key_compressed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::crypto::ecc_point_t Q(coinbase::crypto::curve_secp256k1);
  const error_t rv = extract_Q_from_key_blob(key_blob, Q);
  if (rv) return rv;
  pub_key_compressed = Q.to_compressed_bin();
  return SUCCESS;
}

error_t extract_public_key_xonly(mem_t key_blob, buf_t& pub_key_xonly) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::crypto::ecc_point_t Q(coinbase::crypto::curve_secp256k1);
  const error_t rv = extract_Q_from_key_blob(key_blob, Q);
  if (rv) return rv;
  pub_key_xonly = Q.get_x().to_bin(/*size=*/32);
  return SUCCESS;
}

error_t get_public_share_compressed(mem_t key_blob, buf_t& out_public_share_compressed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  key_blob_v1_t blob;
  error_t rv = coinbase::convert(blob, key_blob);
  if (rv) return rv;
  if (blob.version != key_blob_version_v1 && blob.version != ac_key_blob_version_v1)
    return coinbase::error(E_FORMAT, "unsupported key blob version");
  if (static_cast<curve_id>(blob.curve) != curve_id::secp256k1)
    return coinbase::error(E_FORMAT, "invalid key blob curve");
  if (blob.party_name.empty()) return coinbase::error(E_FORMAT, "invalid key blob");

  const auto it = blob.Qis_compressed.find(blob.party_name);
  if (it == blob.Qis_compressed.end()) return coinbase::error(E_FORMAT, "key blob missing self Qi");
  out_public_share_compressed = it->second;
  return SUCCESS;
}

error_t detach_private_scalar(mem_t key_blob, buf_t& out_public_key_blob, buf_t& out_private_scalar_fixed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  key_blob_v1_t blob;
  error_t rv = coinbase::convert(blob, key_blob);
  if (rv) return rv;
  if (blob.version != key_blob_version_v1 && blob.version != ac_key_blob_version_v1)
    return coinbase::error(E_FORMAT, "unsupported key blob version");
  if (static_cast<curve_id>(blob.curve) != curve_id::secp256k1)
    return coinbase::error(E_FORMAT, "invalid key blob curve");

  const auto curve = coinbase::crypto::curve_secp256k1;
  const coinbase::crypto::mod_t& q = curve.order();
  if (!q.is_in_range(blob.x_share)) return coinbase::error(E_FORMAT, "invalid key blob");
  const int order_size = q.get_bin_size();
  if (order_size <= 0) return coinbase::error(E_GENERAL, "invalid curve order size");

  coinbase::crypto::ecc_point_t Q;
  if (rv = Q.from_bin(curve, blob.Q_compressed)) return coinbase::error(rv, "invalid key blob");
  coinbase::crypto::ss::party_map_t<coinbase::crypto::ecc_point_t> Qis;
  if (rv = parse_Qis(blob.Qis_compressed, curve, Qis)) return rv;
  if (rv = validate_aggregate_public_key(curve, Q, Qis)) return rv;

  out_private_scalar_fixed = blob.x_share.to_bin(order_size);

  // Wipe private scalar share.
  blob.x_share = 0;
  out_public_key_blob = coinbase::convert(blob);
  return SUCCESS;
}

error_t attach_private_scalar(mem_t public_key_blob, mem_t private_scalar_fixed, mem_t public_share_compressed,
                              buf_t& out_key_blob) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(public_key_blob, "public_key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  key_blob_v1_t blob;
  error_t rv = coinbase::convert(blob, public_key_blob);
  if (rv) return rv;
  if (blob.version != key_blob_version_v1 && blob.version != ac_key_blob_version_v1)
    return coinbase::error(E_FORMAT, "unsupported key blob version");
  if (static_cast<curve_id>(blob.curve) != curve_id::secp256k1)
    return coinbase::error(E_FORMAT, "invalid key blob curve");
  if (blob.party_name.empty()) return coinbase::error(E_FORMAT, "invalid key blob");

  const auto curve = coinbase::crypto::curve_secp256k1;
  const coinbase::crypto::mod_t& q = curve.order();
  const int order_size = q.get_bin_size();
  if (order_size <= 0) return coinbase::error(E_GENERAL, "invalid curve order size");

  coinbase::crypto::ecc_point_t Q;
  if (rv = Q.from_bin(curve, blob.Q_compressed)) return coinbase::error(rv, "invalid key blob");
  coinbase::crypto::ss::party_map_t<coinbase::crypto::ecc_point_t> Qis;
  if (rv = parse_Qis(blob.Qis_compressed, curve, Qis)) return rv;
  if (rv = validate_aggregate_public_key(curve, Q, Qis)) return rv;

  if (const error_t rvm = coinbase::api::detail::validate_mem_arg(private_scalar_fixed, "private_scalar_fixed"))
    return rvm;
  if (private_scalar_fixed.size != order_size) return coinbase::error(E_BADARG, "private_scalar_fixed wrong size");
  if (const error_t rvp = coinbase::api::detail::validate_mem_arg(public_share_compressed, "public_share_compressed"))
    return rvp;

  const auto it = blob.Qis_compressed.find(blob.party_name);
  if (it == blob.Qis_compressed.end()) return coinbase::error(E_FORMAT, "key blob missing self Qi");
  const buf_t& Qi_self_compressed = it->second;

  if (public_share_compressed != mem_t(Qi_self_compressed))
    return coinbase::error(E_BADARG, "public_share_compressed mismatch key blob");

  coinbase::crypto::ecc_point_t Qi_self(curve);
  if (rv = Qi_self.from_bin(curve, Qi_self_compressed)) return coinbase::error(rv, "invalid key blob");
  if (rv = curve.check(Qi_self)) return coinbase::error(rv, "invalid key blob");

  coinbase::crypto::bn_t x = coinbase::crypto::bn_t::from_bin(private_scalar_fixed) % q;
  if (!q.is_in_range(x)) return coinbase::error(E_FORMAT, "invalid private_scalar_fixed");

  const auto& G = curve.generator();
  if (x * G != Qi_self) return coinbase::error(E_FORMAT, "x_share mismatch key blob");

  blob.x_share = std::move(x);
  out_key_blob = coinbase::convert(blob);
  return SUCCESS;
}

}  // namespace coinbase::api::schnorr_mp
