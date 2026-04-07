#include <cbmpc/api/pve_batch_single_recipient.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/protocol/pve_base.h>
#include <cbmpc/internal/protocol/pve_batch.h>

#include "curve_util.h"
#include "mem_util.h"
#include "pve_internal.h"

namespace coinbase::api::pve {

namespace {

constexpr uint32_t pve_batch_ciphertext_version_v1 = 1;

using coinbase::api::detail::to_internal_curve;

struct pve_batch_ciphertext_blob_v1_t {
  uint32_t version = pve_batch_ciphertext_version_v1;
  uint32_t batch_count = 0;
  buf_t ct;  // serialized `coinbase::mpc::ec_pve_batch_t`

  void convert(coinbase::converter_t& c) { c.convert(version, batch_count, ct); }
};

using detail::base_pke_bridge_t;
using detail::base_pke_ek_blob_v1_t;
using detail::base_pke_key_type_v1;
using detail::ecies_p256_hsm_base_pke_t;
using detail::parse_ek_blob;
using detail::rsa_oaep_hsm_base_pke_t;

static error_t parse_batch_ciphertext(mem_t ciphertext, pve_batch_ciphertext_blob_v1_t& out_blob) {
  error_t rv = coinbase::deser(ciphertext, out_blob);
  if (rv) return rv;
  if (out_blob.version != pve_batch_ciphertext_version_v1)
    return coinbase::error(E_FORMAT, "unsupported ciphertext version");
  if (out_blob.batch_count == 0) return coinbase::error(E_FORMAT, "invalid batch count");
  if (out_blob.batch_count > static_cast<uint32_t>(coinbase::mpc::ec_pve_batch_t::MAX_BATCH_COUNT))
    return coinbase::error(E_RANGE, "batch too large");
  return SUCCESS;
}

static error_t deserialize_batch_ciphertext(mem_t ciphertext, int n, coinbase::mpc::ec_pve_batch_t& out_ct) {
  if (n <= 0 || n > coinbase::mpc::ec_pve_batch_t::MAX_BATCH_COUNT)
    return coinbase::error(E_BADARG, "invalid batch count");
  return coinbase::deser(ciphertext, out_ct);
}

}  // namespace

error_t encrypt_batch(const base_pke_i& base_pke, curve_id curve, mem_t ek, mem_t label, const std::vector<mem_t>& xs,
                      buf_t& out_ciphertext) {
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(ek, "ek", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_vec_arg(xs, "xs")) return rv;
  error_t rv = UNINITIALIZED_ERROR;

  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  if (xs.empty()) return coinbase::error(E_BADARG, "batch_count must be positive");
  if (xs.size() > static_cast<size_t>(coinbase::mpc::ec_pve_batch_t::MAX_BATCH_COUNT))
    return coinbase::error(E_RANGE, "batch too large");

  // Defensive check: avoid large attacker-controlled allocations when converting `xs[i]` into bignums.
  const int max_x_size = icurve.order().get_bin_size();
  for (const auto& x : xs) {
    if (x.size > max_x_size) return coinbase::error(E_RANGE, "xs element too large", /*to_print_stack_trace=*/false);
  }

  const int n = static_cast<int>(xs.size());
  std::vector<coinbase::crypto::bn_t> x_bn;
  x_bn.reserve(static_cast<size_t>(n));
  for (int i = 0; i < n; i++) x_bn.push_back(coinbase::crypto::bn_t::from_bin(xs[static_cast<size_t>(i)]));

  base_pke_bridge_t bridge(base_pke);
  const coinbase::mem_t ek_mem(ek.data, ek.size);

  coinbase::mpc::ec_pve_batch_t pve_ct(n);

  out_ciphertext.free();
  rv = pve_ct.encrypt(bridge, coinbase::mpc::pve_keyref(ek_mem), label, icurve, x_bn);
  if (rv) return rv;

  pve_batch_ciphertext_blob_v1_t blob;
  blob.batch_count = static_cast<uint32_t>(n);
  blob.ct = coinbase::convert(pve_ct);
  out_ciphertext = coinbase::convert(blob);
  return SUCCESS;
}

error_t encrypt_batch(curve_id curve, mem_t ek, mem_t label, const std::vector<mem_t>& xs, buf_t& out_ciphertext) {
  return encrypt_batch(base_pke_default(), curve, ek, label, xs, out_ciphertext);
}

error_t verify_batch(const base_pke_i& base_pke, curve_id curve, mem_t ek, mem_t ciphertext,
                     const std::vector<mem_t>& Qs_compressed, mem_t label) {
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(ek, "ek", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_vec_arg(Qs_compressed, "Qs_compressed")) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  error_t rv = UNINITIALIZED_ERROR;

  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  pve_batch_ciphertext_blob_v1_t blob;
  if (rv = parse_batch_ciphertext(ciphertext, blob)) return rv;

  const int n = static_cast<int>(blob.batch_count);
  if (static_cast<size_t>(n) != Qs_compressed.size()) return coinbase::error(E_BADARG, "Q count mismatch");

  base_pke_bridge_t bridge(base_pke);
  coinbase::mpc::ec_pve_batch_t pve_ct(n);
  if (rv = deserialize_batch_ciphertext(blob.ct, n, pve_ct)) return rv;

  for (const auto& q : pve_ct.get_Qs()) {
    if (q.get_curve() != icurve) return coinbase::error(E_BADARG, "ciphertext curve mismatch");
  }

  std::vector<coinbase::crypto::ecc_point_t> Q_expected;
  Q_expected.resize(static_cast<size_t>(n));
  for (int i = 0; i < n; i++) {
    if (rv = Q_expected[static_cast<size_t>(i)].from_bin(icurve, Qs_compressed[static_cast<size_t>(i)]))
      return coinbase::error(rv, "invalid Q");
    if (rv = icurve.check(Q_expected[static_cast<size_t>(i)])) return coinbase::error(rv, "invalid Q");
  }

  const coinbase::mem_t ek_mem(ek.data, ek.size);
  return pve_ct.verify(bridge, coinbase::mpc::pve_keyref(ek_mem), Q_expected, label);
}

error_t verify_batch(curve_id curve, mem_t ek, mem_t ciphertext, const std::vector<mem_t>& Qs_compressed, mem_t label) {
  return verify_batch(base_pke_default(), curve, ek, ciphertext, Qs_compressed, label);
}

error_t decrypt_batch(const base_pke_i& base_pke, curve_id curve, mem_t dk, mem_t ek, mem_t ciphertext, mem_t label,
                      std::vector<buf_t>& out_xs) {
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(dk, "dk", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(ek, "ek", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  error_t rv = UNINITIALIZED_ERROR;

  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  pve_batch_ciphertext_blob_v1_t blob;
  if (rv = parse_batch_ciphertext(ciphertext, blob)) return rv;

  const int n = static_cast<int>(blob.batch_count);

  base_pke_bridge_t bridge(base_pke);
  coinbase::mpc::ec_pve_batch_t pve_ct(n);
  if (rv = deserialize_batch_ciphertext(blob.ct, n, pve_ct)) return rv;

  for (const auto& q : pve_ct.get_Qs()) {
    if (q.get_curve() != icurve) return coinbase::error(E_BADARG, "ciphertext curve mismatch");
  }

  const coinbase::mem_t dk_mem(dk.data, dk.size);
  const coinbase::mem_t ek_mem(ek.data, ek.size);

  std::vector<coinbase::crypto::bn_t> xs_bn;
  rv = pve_ct.decrypt(bridge, coinbase::mpc::pve_keyref(dk_mem), coinbase::mpc::pve_keyref(ek_mem), label, icurve,
                      xs_bn, /*skip_verify=*/false);
  if (rv) {
    out_xs.clear();
    return rv;
  }

  std::vector<buf_t> out_local;
  out_local.resize(static_cast<size_t>(n));
  const int out_len = icurve.order().get_bin_size();
  for (int i = 0; i < n; i++) out_local[static_cast<size_t>(i)] = xs_bn[static_cast<size_t>(i)].to_bin(out_len);

  out_xs = std::move(out_local);
  return SUCCESS;
}

error_t decrypt_batch(curve_id curve, mem_t dk, mem_t ek, mem_t ciphertext, mem_t label, std::vector<buf_t>& out_xs) {
  return decrypt_batch(base_pke_default(), curve, dk, ek, ciphertext, label, out_xs);
}

error_t decrypt_batch_rsa_oaep_hsm(curve_id curve, mem_t dk_handle, mem_t ek, mem_t ciphertext, mem_t label,
                                   const rsa_oaep_hsm_decap_cb_t& cb, std::vector<buf_t>& out_xs) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(dk_handle, "dk_handle")) return rv;
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(ek, "ek", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  if (!cb.decap) return coinbase::error(E_BADARG, "missing HSM RSA decap callback");

  base_pke_ek_blob_v1_t ek_blob;
  error_t rv = parse_ek_blob(ek, ek_blob);
  if (rv) return rv;
  if (static_cast<base_pke_key_type_v1>(ek_blob.key_type) != base_pke_key_type_v1::rsa_oaep_2048)
    return coinbase::error(E_BADARG, "expected RSA base PKE public key");

  rsa_oaep_hsm_base_pke_t base_pke(cb);
  return decrypt_batch(base_pke, curve, dk_handle, ek, ciphertext, label, out_xs);
}

error_t decrypt_batch_ecies_p256_hsm(curve_id curve, mem_t dk_handle, mem_t ek, mem_t ciphertext, mem_t label,
                                     const ecies_p256_hsm_ecdh_cb_t& cb, std::vector<buf_t>& out_xs) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(dk_handle, "dk_handle")) return rv;
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(ek, "ek", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  if (!cb.ecdh) return coinbase::error(E_BADARG, "missing HSM ECIES ECDH callback");

  base_pke_ek_blob_v1_t ek_blob;
  error_t rv = parse_ek_blob(ek, ek_blob);
  if (rv) return rv;
  if (static_cast<base_pke_key_type_v1>(ek_blob.key_type) != base_pke_key_type_v1::ecies_p256)
    return coinbase::error(E_BADARG, "expected ECIES(P-256) base PKE public key");
  if (ek_blob.ecies_ek.get_curve() != coinbase::crypto::curve_p256)
    return coinbase::error(E_BADARG, "ECIES base PKE key must be on P-256");

  ecies_p256_hsm_base_pke_t base_pke(ek_blob.ecies_ek.to_oct(), cb);
  return decrypt_batch(base_pke, curve, dk_handle, ek, ciphertext, label, out_xs);
}

error_t get_batch_count(mem_t ciphertext, int& out_batch_count) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  pve_batch_ciphertext_blob_v1_t blob;
  error_t rv = parse_batch_ciphertext(ciphertext, blob);
  if (rv) return rv;
  out_batch_count = static_cast<int>(blob.batch_count);
  return SUCCESS;
}

error_t get_public_keys_compressed_batch(mem_t ciphertext, std::vector<buf_t>& out_Qs_compressed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  error_t rv = UNINITIALIZED_ERROR;

  pve_batch_ciphertext_blob_v1_t blob;
  if (rv = parse_batch_ciphertext(ciphertext, blob)) return rv;

  const int n = static_cast<int>(blob.batch_count);
  coinbase::mpc::ec_pve_batch_t pve_ct(n);  // base PKE not used for extraction
  if (rv = deserialize_batch_ciphertext(blob.ct, n, pve_ct)) return rv;

  std::vector<buf_t> out_local;
  out_local.reserve(static_cast<size_t>(n));
  for (const auto& q : pve_ct.get_Qs()) out_local.push_back(q.to_compressed_bin());

  out_Qs_compressed = std::move(out_local);
  return SUCCESS;
}

error_t get_Label_batch(mem_t ciphertext, buf_t& out_label) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  error_t rv = UNINITIALIZED_ERROR;

  pve_batch_ciphertext_blob_v1_t blob;
  if (rv = parse_batch_ciphertext(ciphertext, blob)) return rv;

  const int n = static_cast<int>(blob.batch_count);
  coinbase::mpc::ec_pve_batch_t pve_ct(n);  // base PKE not used for extraction
  if (rv = deserialize_batch_ciphertext(blob.ct, n, pve_ct)) return rv;

  out_label = pve_ct.get_Label();
  return SUCCESS;
}

}  // namespace coinbase::api::pve
