#include <cbmpc/api/ecdsa_2p.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/protocol/ecdsa_2p.h>

#include "curve_util.h"
#include "job_util.h"
#include "mem_util.h"

namespace coinbase::api::ecdsa_2p {

namespace {

constexpr uint32_t key_blob_version_v1 = 1;

using coinbase::api::detail::from_internal_curve;
using coinbase::api::detail::to_internal_curve;
using coinbase::api::detail::to_internal_job;
using coinbase::api::detail::to_internal_party;
using coinbase::api::detail::validate_job_2p;

struct key_blob_v1_t {
  uint32_t version = key_blob_version_v1;
  uint32_t role = 0;   // 0=p1, 1=p2
  uint32_t curve = 0;  // coinbase::api::curve_id

  buf_t Q_compressed;
  coinbase::crypto::bn_t x_share;
  coinbase::crypto::bn_t c_key;
  coinbase::crypto::paillier_t paillier;

  void convert(coinbase::converter_t& c) { c.convert(version, role, curve, Q_compressed, x_share, c_key, paillier); }
};

error_t blob_to_key(const key_blob_v1_t& blob, coinbase::mpc::ecdsa2pc::key_t& key) {
  if (blob.role > 1) return coinbase::error(E_FORMAT, "invalid key blob role");

  const auto cid = static_cast<curve_id>(blob.curve);
  if (cid == curve_id::ed25519) return coinbase::error(E_FORMAT, "invalid key blob curve");
  auto curve = to_internal_curve(cid);
  if (!curve.valid()) return coinbase::error(E_FORMAT, "invalid key blob curve");

  key.role = static_cast<coinbase::mpc::party_t>(static_cast<int32_t>(blob.role));
  key.curve = curve;

  // Defensive validation at the opaque blob boundary.
  //
  // In our ECDSA-2PC protocol, party P1 owns the Paillier private key, and `c_key` is an encryption of P1's share under
  // that key. Reject malformed / tampered blobs early.
  const bool paillier_has_private = blob.paillier.has_private_key();
  if ((blob.role == 0) != paillier_has_private) return coinbase::error(E_FORMAT, "invalid key blob");

  const auto& N = blob.paillier.get_N();
  if (N.value().get_bits_count() < coinbase::crypto::paillier_t::bit_size) {
    return coinbase::error(E_FORMAT, "invalid key blob");
  }

  // Intentionally do not enforce `x_share in [0, q)` here:
  // in ECDSA-2PC this share is maintained as a Paillier-compatible integer representative and can be
  // unreduced after refresh, so rejecting non-reduced values would break valid refreshed key blobs.
  //
  // However, `x_share` must remain in Z_N so that Paillier-related operations are well-defined and to avoid
  // attacker-controlled bignum blowups.
  if (!N.is_in_range(blob.x_share)) return coinbase::error(E_FORMAT, "invalid key blob");

  // Ensure `c_key` is a well-formed Paillier ciphertext under this key.
  {
    coinbase::crypto::vartime_scope_t vartime_scope;
    if (blob.paillier.verify_cipher(blob.c_key)) return coinbase::error(E_FORMAT, "invalid key blob");
  }

  // If we have the private key (P1), bind the share to its Paillier encryption.
  if (paillier_has_private) {
    const coinbase::crypto::bn_t plain = blob.paillier.decrypt(blob.c_key);
    if (plain != N.mod(blob.x_share)) return coinbase::error(E_FORMAT, "invalid key blob");
  }

  key.x_share = blob.x_share;
  key.c_key = blob.c_key;
  key.paillier = blob.paillier;

  if (const error_t rv = key.Q.from_bin(curve, blob.Q_compressed)) return rv;
  if (curve.check(key.Q)) return coinbase::error(E_FORMAT, "invalid key blob");
  return SUCCESS;
}

error_t serialize_key_blob(const coinbase::mpc::ecdsa2pc::key_t& key, buf_t& out) {
  curve_id cid;
  if (!from_internal_curve(key.curve, cid)) return coinbase::error(E_BADARG, "unsupported curve");
  if (cid == curve_id::ed25519) return coinbase::error(E_BADARG, "unsupported curve");

  key_blob_v1_t blob;
  blob.role = static_cast<uint32_t>(key.role);
  blob.curve = static_cast<uint32_t>(cid);
  blob.Q_compressed = key.Q.to_compressed_bin();
  blob.x_share = key.x_share;
  blob.c_key = key.c_key;
  blob.paillier = key.paillier;
  out = coinbase::convert(blob);
  return SUCCESS;
}

error_t deserialize_key_blob(mem_t in, coinbase::mpc::ecdsa2pc::key_t& key) {
  // Reject unsupported versions before attempting to parse variable-length fields.
  // This prevents mis-parsing newer blob versions with incompatible encodings.
  if (in.size < 4) return coinbase::error(E_FORMAT, "invalid key blob");
  const uint32_t version = coinbase::be_get_4(in.data);
  if (version != key_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported key blob version");

  key_blob_v1_t blob;
  const error_t rv = coinbase::convert(blob, in);
  if (rv) return rv;
  if (blob.version != key_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported key blob version");
  return blob_to_key(blob, key);
}

}  // namespace

error_t dkg(const coinbase::api::job_2p_t& job, curve_id curve, buf_t& key_blob) {
  if (const error_t rv = validate_job_2p(job)) return rv;
  if (curve == curve_id::ed25519) return coinbase::error(E_BADARG, "unsupported curve");
  auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  coinbase::mpc::job_2p_t mpc_job = to_internal_job(job);

  coinbase::mpc::ecdsa2pc::key_t key;
  const error_t rv = coinbase::mpc::ecdsa2pc::dkg(mpc_job, icurve, key);
  if (rv) return rv;

  return serialize_key_blob(key, key_blob);
}

error_t refresh(const coinbase::api::job_2p_t& job, mem_t key_blob, buf_t& new_key_blob) {
  if (const error_t rv = validate_job_2p(job)) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::ecdsa2pc::key_t key;
  error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;

  const auto self = to_internal_party(job.self);
  if (key.role != self) return coinbase::error(E_BADARG, "job.self mismatch key blob role");

  coinbase::mpc::job_2p_t mpc_job = to_internal_job(job);

  coinbase::mpc::ecdsa2pc::key_t new_key;
  rv = coinbase::mpc::ecdsa2pc::refresh(mpc_job, key, new_key);
  if (rv) return rv;

  return serialize_key_blob(new_key, new_key_blob);
}

using internal_sign_fn_t = error_t (*)(coinbase::mpc::job_2p_t&, buf_t& /*sid*/, const coinbase::mpc::ecdsa2pc::key_t&,
                                       const mem_t /*msg*/, buf_t& /*sig_der*/);

static error_t sign_common(internal_sign_fn_t fn, const coinbase::api::job_2p_t& job, mem_t key_blob, mem_t msg_hash,
                           buf_t& sid, buf_t& sig_der) {
  if (const error_t rv = validate_job_2p(job)) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          msg_hash, "msg_hash", coinbase::api::detail::MAX_MESSAGE_DIGEST_SIZE))
    return rv;
  coinbase::mpc::ecdsa2pc::key_t key;
  error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;

  const auto self = to_internal_party(job.self);
  if (key.role != self) return coinbase::error(E_BADARG, "job.self mismatch key blob role");

  coinbase::mpc::job_2p_t mpc_job = to_internal_job(job);

  sig_der.free();
  return fn(mpc_job, sid, key, msg_hash, sig_der);
}

error_t sign(const coinbase::api::job_2p_t& job, mem_t key_blob, mem_t msg_hash, buf_t& sid, buf_t& sig_der) {
  return sign_common(&coinbase::mpc::ecdsa2pc::sign, job, key_blob, msg_hash, sid, sig_der);
}

error_t get_public_key_compressed(mem_t key_blob, buf_t& pub_key) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::ecdsa2pc::key_t key;
  const error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;
  pub_key = key.Q.to_compressed_bin();
  return SUCCESS;
}

error_t get_public_share_compressed(mem_t key_blob, buf_t& out_public_share_compressed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::ecdsa2pc::key_t key;
  error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;

  const auto& q = key.curve.order();
  const coinbase::crypto::bn_t x_mod_q = key.x_share % q;
  out_public_share_compressed = (x_mod_q * key.curve.generator()).to_compressed_bin();
  return SUCCESS;
}

error_t detach_private_scalar(mem_t key_blob, buf_t& out_public_key_blob, buf_t& out_private_scalar) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(key_blob, "key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  coinbase::mpc::ecdsa2pc::key_t key;
  error_t rv = deserialize_key_blob(key_blob, key);
  if (rv) return rv;

  curve_id cid;
  if (!from_internal_curve(key.curve, cid)) return coinbase::error(E_BADARG, "unsupported curve");
  if (cid == curve_id::ed25519) return coinbase::error(E_BADARG, "unsupported curve");

  // Variable-length big-endian encoding (may grow after refresh).
  out_private_scalar = key.x_share.to_bin();

  // Produce a v1-format blob with an invalid (out-of-range) scalar share so it is
  // rejected by sign/refresh APIs.
  key_blob_v1_t pub;
  pub.role = static_cast<uint32_t>(key.role);
  pub.curve = static_cast<uint32_t>(cid);
  pub.Q_compressed = key.Q.to_compressed_bin();
  pub.x_share = key.paillier.get_N().value();  // x_share == N is out of range
  pub.c_key = key.c_key;
  pub.paillier = key.paillier;
  out_public_key_blob = coinbase::convert(pub);
  return SUCCESS;
}

error_t attach_private_scalar(mem_t public_key_blob, mem_t private_scalar, mem_t public_share_compressed,
                              buf_t& out_key_blob) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(public_key_blob, "public_key_blob",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  key_blob_v1_t pub;
  error_t rv = coinbase::convert(pub, public_key_blob);
  if (rv) return rv;
  if (pub.version != key_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported key blob version");
  if (pub.role > 1) return coinbase::error(E_FORMAT, "invalid key blob role");

  const auto cid = static_cast<curve_id>(pub.curve);
  if (cid == curve_id::ed25519) return coinbase::error(E_FORMAT, "invalid key blob curve");
  auto curve = to_internal_curve(cid);
  if (!curve.valid()) return coinbase::error(E_FORMAT, "invalid key blob curve");

  if (const error_t rvm = coinbase::api::detail::validate_mem_arg(private_scalar, "private_scalar")) return rvm;
  if (private_scalar.size == 0) return coinbase::error(E_BADARG, "private_scalar must be non-empty");
  if (const error_t rvp = coinbase::api::detail::validate_mem_arg(public_share_compressed, "public_share_compressed"))
    return rvp;

  // Validate Paillier material (c_key + key) is well-formed.
  const bool paillier_has_private = pub.paillier.has_private_key();
  if ((pub.role == 0) != paillier_has_private) return coinbase::error(E_FORMAT, "invalid key blob");

  const auto& N = pub.paillier.get_N();
  if (N.value().get_bits_count() < coinbase::crypto::paillier_t::bit_size)
    return coinbase::error(E_FORMAT, "invalid key blob");
  {
    coinbase::crypto::vartime_scope_t vartime_scope;
    if (pub.paillier.verify_cipher(pub.c_key)) return coinbase::error(E_FORMAT, "invalid key blob");
  }

  // Interpret scalar and ensure it stays in Z_N (matching key blob invariants).
  coinbase::crypto::bn_t x_share = coinbase::crypto::bn_t::from_bin(private_scalar);
  if (!N.is_in_range(x_share)) return coinbase::error(E_FORMAT, "invalid private_scalar");

  // If we have the private key (P1), bind the share to its Paillier encryption.
  if (paillier_has_private) {
    coinbase::crypto::vartime_scope_t vartime_scope;
    const coinbase::crypto::bn_t plain = pub.paillier.decrypt(pub.c_key);
    if (plain != N.mod(x_share)) return coinbase::error(E_FORMAT, "x_share mismatch key blob");
  }

  // Verify scalar matches the provided self-share point.
  const coinbase::crypto::mod_t& q = curve.order();
  const coinbase::crypto::bn_t x_mod_q = x_share % q;
  if (!q.is_in_range(x_mod_q)) return coinbase::error(E_FORMAT, "invalid private_scalar");

  coinbase::crypto::ecc_point_t Qi_self(curve);
  if (rv = Qi_self.from_bin(curve, public_share_compressed))
    return coinbase::error(rv, "invalid public_share_compressed");
  if (rv = curve.check(Qi_self)) return coinbase::error(rv, "invalid public_share_compressed");
  if (x_mod_q * curve.generator() != Qi_self) return coinbase::error(E_FORMAT, "x_share mismatch key blob");

  // Validate and normalize global public key encoding.
  coinbase::crypto::ecc_point_t Q(curve);
  if (rv = Q.from_bin(curve, pub.Q_compressed)) return coinbase::error(rv, "invalid key blob");
  if (rv = curve.check(Q)) return coinbase::error(rv, "invalid key blob");

  pub.Q_compressed = Q.to_compressed_bin();
  pub.x_share = std::move(x_share);
  out_key_blob = coinbase::convert(pub);
  return SUCCESS;
}

}  // namespace coinbase::api::ecdsa_2p
