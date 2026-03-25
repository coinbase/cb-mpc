#include <cbmpc/api/schnorr_2p.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/protocol/schnorr_2p.h>

#include "job_util.h"
#include "mem_util.h"

namespace coinbase::api::schnorr_2p {

namespace {

constexpr uint32_t key_blob_version_v1 = 1;

using coinbase::api::detail::to_internal_job;
using coinbase::api::detail::to_internal_party;
using coinbase::api::detail::validate_job_2p;

struct key_blob_v1_t {
  uint32_t version = key_blob_version_v1;
  uint32_t role = 0;   // 0=p1, 1=p2
  uint32_t curve = 0;  // coinbase::api::curve_id

  buf_t Q_compressed;
  coinbase::crypto::bn_t x_share;

  void convert(coinbase::converter_t& c) { c.convert(version, role, curve, Q_compressed, x_share); }
};

static error_t blob_to_key(const key_blob_v1_t& blob, coinbase::mpc::schnorr2p::key_t& key) {
  if (blob.role > 1) return coinbase::error(E_FORMAT, "invalid key blob role");
  if (static_cast<curve_id>(blob.curve) != curve_id::secp256k1)
    return coinbase::error(E_FORMAT, "invalid key blob curve");

  key.role = static_cast<coinbase::mpc::party_t>(static_cast<int32_t>(blob.role));
  key.curve = coinbase::crypto::curve_secp256k1;
  const auto& q = key.curve.order();
  if (!q.is_in_range(blob.x_share)) return coinbase::error(E_FORMAT, "invalid key blob");
  key.x_share = blob.x_share;

  return key.Q.from_bin(key.curve, blob.Q_compressed);
}

static error_t serialize_key_blob(const coinbase::mpc::schnorr2p::key_t& key, buf_t& out) {
  if (key.curve != coinbase::crypto::curve_secp256k1) return coinbase::error(E_BADARG, "unsupported curve");

  key_blob_v1_t blob;
  blob.role = static_cast<uint32_t>(key.role);
  blob.curve = static_cast<uint32_t>(curve_id::secp256k1);
  blob.Q_compressed = key.Q.to_compressed_bin();
  blob.x_share = key.x_share;
  out = coinbase::convert(blob);
  return SUCCESS;
}

static error_t deserialize_key_blob(mem_t in, coinbase::mpc::schnorr2p::key_t& key) {
  key_blob_v1_t blob;
  const error_t rv = coinbase::convert(blob, in);
  if (rv) return rv;
  if (blob.version != key_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported key blob version");
  return blob_to_key(blob, key);
}

}  // namespace

error_t dkg(const coinbase::api::job_2p_t& job, curve_id curve, buf_t& key_blob) {
  if (const error_t rv = validate_job_2p(job)) return rv;
  if (curve != curve_id::secp256k1) return coinbase::error(E_BADARG, "unsupported curve");

  coinbase::mpc::job_2p_t mpc_job = to_internal_job(job);

  coinbase::mpc::schnorr2p::key_t key;
  buf_t sid;  // unused by this API
  const error_t rv = coinbase::mpc::eckey::key_share_2p_t::dkg(mpc_job, coinbase::crypto::curve_secp256k1, key, sid);
  if (rv) return rv;

  return serialize_key_blob(key, key_blob);
}

error_t refresh(const coinbase::api::job_2p_t& job, mem_t key_blob, buf_t& new_key_blob) {
  if (const error_t rv = validate_job_2p(job)) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::schnorr2p::key_t key;
  error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;

  const auto self = to_internal_party(job.self);
  if (key.role != self) return coinbase::error(E_BADARG, "job.self mismatch key blob role");

  coinbase::mpc::job_2p_t mpc_job = to_internal_job(job);

  coinbase::mpc::schnorr2p::key_t new_key;
  new_key_blob.free();
  rv = coinbase::mpc::eckey::key_share_2p_t::refresh(mpc_job, key, new_key);
  if (rv) return rv;

  return serialize_key_blob(new_key, new_key_blob);
}

error_t sign(const coinbase::api::job_2p_t& job, mem_t key_blob, mem_t msg, buf_t& sig) {
  if (const error_t rv = validate_job_2p(job)) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(msg, "msg")) return rv;
  if (msg.size != 32) return coinbase::error(E_BADARG, "BIP340 requires a 32-byte message");

  coinbase::mpc::schnorr2p::key_t key;
  error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;

  const auto self = to_internal_party(job.self);
  if (key.role != self) return coinbase::error(E_BADARG, "job.self mismatch key blob role");

  coinbase::mpc::job_2p_t mpc_job = to_internal_job(job);

  sig.free();
  return coinbase::mpc::schnorr2p::sign(mpc_job, key, msg, sig, coinbase::mpc::schnorr2p::variant_e::BIP340);
}

error_t get_public_key_compressed(mem_t key_blob, buf_t& pub_key_compressed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::schnorr2p::key_t key;
  const error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;
  pub_key_compressed = key.Q.to_compressed_bin();
  return SUCCESS;
}

error_t extract_public_key_xonly(mem_t key_blob, buf_t& pub_key_xonly) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::schnorr2p::key_t key;
  const error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;
  pub_key_xonly = key.Q.get_x().to_bin(/*size=*/32);
  return SUCCESS;
}

error_t get_public_share_compressed(mem_t key_blob, buf_t& out_public_share_compressed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::schnorr2p::key_t key;
  error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;

  const auto curve = coinbase::crypto::curve_secp256k1;
  const coinbase::crypto::mod_t& q = curve.order();
  const auto& G = curve.generator();
  const coinbase::crypto::bn_t x = key.x_share % q;
  out_public_share_compressed = (x * G).to_compressed_bin();
  return SUCCESS;
}

error_t detach_private_scalar(mem_t key_blob, buf_t& out_public_key_blob, buf_t& out_private_scalar_fixed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::schnorr2p::key_t key;
  const error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;

  const auto curve = coinbase::crypto::curve_secp256k1;
  const coinbase::crypto::mod_t& q = curve.order();
  const int order_size = q.get_bin_size();
  if (order_size <= 0) return coinbase::error(E_GENERAL, "invalid curve order size");

  out_private_scalar_fixed = key.x_share.to_bin(order_size);

  // Produce a v1-format blob with an invalid (out-of-range) scalar share so it is
  // rejected by sign/refresh APIs.
  key_blob_v1_t pub;
  pub.role = static_cast<uint32_t>(key.role);
  pub.curve = static_cast<uint32_t>(curve_id::secp256k1);
  pub.Q_compressed = key.Q.to_compressed_bin();
  pub.x_share = q;  // x_share == q is out of range
  out_public_key_blob = coinbase::convert(pub);
  return SUCCESS;
}

error_t attach_private_scalar(mem_t public_key_blob, mem_t private_scalar_fixed, mem_t public_share_compressed,
                              buf_t& out_key_blob) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(public_key_blob, "public_key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  key_blob_v1_t pub;
  error_t rv = coinbase::convert(pub, public_key_blob);
  if (rv) return rv;
  if (pub.version != key_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported key blob version");
  if (pub.role > 1) return coinbase::error(E_FORMAT, "invalid key blob role");
  if (static_cast<curve_id>(pub.curve) != curve_id::secp256k1)
    return coinbase::error(E_FORMAT, "invalid key blob curve");
  if (pub.Q_compressed.empty()) return coinbase::error(E_FORMAT, "invalid key blob");

  const auto curve = coinbase::crypto::curve_secp256k1;
  const coinbase::crypto::mod_t& q = curve.order();
  const int order_size = q.get_bin_size();
  if (order_size <= 0) return coinbase::error(E_GENERAL, "invalid curve order size");

  if (const error_t rvm = coinbase::api::detail::validate_mem_arg(private_scalar_fixed, "private_scalar_fixed"))
    return rvm;
  if (private_scalar_fixed.size != order_size) return coinbase::error(E_BADARG, "private_scalar_fixed wrong size");

  if (const error_t rvp = coinbase::api::detail::validate_mem_arg(public_share_compressed, "public_share_compressed"))
    return rvp;

  coinbase::crypto::ecc_point_t Qi_self(curve);
  if (rv = Qi_self.from_bin(curve, public_share_compressed))
    return coinbase::error(rv, "invalid public_share_compressed");
  if (rv = curve.check(Qi_self)) return coinbase::error(rv, "invalid public_share_compressed");

  const coinbase::crypto::bn_t x = coinbase::crypto::bn_t::from_bin(private_scalar_fixed) % q;
  if (!q.is_in_range(x)) return coinbase::error(E_FORMAT, "invalid private_scalar_fixed");

  const auto& G = curve.generator();
  if (x * G != Qi_self) return coinbase::error(E_FORMAT, "x_share mismatch key blob");

  // Validate and normalize global public key encoding.
  coinbase::crypto::ecc_point_t Q(curve);
  if (rv = Q.from_bin(curve, pub.Q_compressed)) return coinbase::error(rv, "invalid key blob");
  if (rv = curve.check(Q)) return coinbase::error(rv, "invalid key blob");

  pub.x_share = x;
  pub.Q_compressed = Q.to_compressed_bin();
  out_key_blob = coinbase::convert(pub);
  return SUCCESS;
}

}  // namespace coinbase::api::schnorr_2p
