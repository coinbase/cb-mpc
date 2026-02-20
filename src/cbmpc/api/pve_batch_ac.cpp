#include <cbmpc/api/pve_batch_ac.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/protocol/pve_ac.h>

#include "access_structure_util.h"
#include "curve_util.h"
#include "mem_util.h"
#include "pve_internal.h"

namespace coinbase::api::pve {

namespace {

constexpr uint32_t pve_ac_ciphertext_version_v1 = 1;
// Defensive limit for untrusted inputs. This bounds allocations/work when ciphertexts come from the network.
static constexpr int MAX_BATCH_COUNT = 100000;

using coinbase::api::detail::to_internal_curve;

struct pve_ac_ciphertext_blob_v1_t {
  uint32_t version = pve_ac_ciphertext_version_v1;
  uint32_t batch_count = 0;
  buf_t ct;  // serialized `coinbase::mpc::ec_pve_ac_t`

  void convert(coinbase::converter_t& c) { c.convert(version, batch_count, ct); }
};

static error_t parse_ac_ciphertext(mem_t ciphertext, pve_ac_ciphertext_blob_v1_t& out_blob) {
  error_t rv = coinbase::convert(out_blob, ciphertext);
  if (rv) return rv;
  if (out_blob.version != pve_ac_ciphertext_version_v1)
    return coinbase::error(E_FORMAT, "unsupported ciphertext version");
  if (out_blob.batch_count == 0) return coinbase::error(E_FORMAT, "invalid batch count");
  if (out_blob.batch_count > static_cast<uint32_t>(MAX_BATCH_COUNT)) return coinbase::error(E_RANGE, "batch too large");
  return SUCCESS;
}

static error_t to_internal_ac_and_leaves(const access_structure_t& ac_in, const coinbase::crypto::ecurve_t& curve,
                                         coinbase::crypto::ss::ac_owned_t& out_ac,
                                         std::set<std::string>& out_leaf_names) {
  out_leaf_names.clear();
  error_t rv = coinbase::api::detail::collect_leaf_names(ac_in, out_leaf_names);
  if (rv) return rv;
  if (out_leaf_names.empty()) return coinbase::error(E_BADARG, "access_structure: missing leaves");

  std::vector<std::string_view> party_names;
  party_names.reserve(out_leaf_names.size());
  for (const auto& s : out_leaf_names) party_names.emplace_back(s);

  return coinbase::api::detail::to_internal_access_structure(ac_in, party_names, curve, out_ac);
}

static error_t validate_leaf_keys_exact(const std::set<std::string>& leaf_names, const leaf_keys_t& keys,
                                        const char* what) {
  if (leaf_names.empty()) return coinbase::error(E_BADARG, "access_structure: missing leaves");
  if (keys.size() != leaf_names.size()) return coinbase::error(E_BADARG, std::string(what) + ": key set mismatch");

  for (const auto& [name_view, key] : keys) {
    if (name_view.empty()) return coinbase::error(E_BADARG, std::string(what) + ": leaf name must be non-empty");
    if (key.size < 0) return coinbase::error(E_BADARG, std::string(what) + ": invalid key size");
    if (key.size > 0 && !key.data) return coinbase::error(E_BADARG, std::string(what) + ": missing key bytes");
    if (key.size > coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE)
      return coinbase::error(E_RANGE, std::string(what) + ": key too large");

    if (leaf_names.count(std::string(name_view)) == 0)
      return coinbase::error(E_BADARG, std::string(what) + ": unknown leaf name");
  }

  for (const auto& leaf : leaf_names) {
    if (keys.find(std::string_view(leaf)) == keys.end())
      return coinbase::error(E_BADARG, std::string(what) + ": missing key for leaf " + leaf);
  }

  return SUCCESS;
}

static error_t validate_quorum_shares_subset(const std::set<std::string>& leaf_names, const leaf_shares_t& shares,
                                             const char* what) {
  if (shares.empty()) return coinbase::error(E_BADARG, std::string(what) + ": quorum_shares must be non-empty");

  for (const auto& [name_view, share] : shares) {
    if (name_view.empty()) return coinbase::error(E_BADARG, std::string(what) + ": leaf name must be non-empty");
    if (share.size < 0) return coinbase::error(E_BADARG, std::string(what) + ": invalid share size");
    if (share.size > 0 && !share.data) return coinbase::error(E_BADARG, std::string(what) + ": missing share bytes");
    if (share.size > coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE)
      return coinbase::error(E_RANGE, std::string(what) + ": share too large");
    if (leaf_names.count(std::string(name_view)) == 0)
      return coinbase::error(E_BADARG, std::string(what) + ": unknown leaf name");
  }
  return SUCCESS;
}

static void build_internal_leaf_key_ptrs(const leaf_keys_t& in, std::map<std::string, coinbase::mem_t>& out_key_mems,
                                         coinbase::mpc::ec_pve_ac_t::pks_t& out_ptrs) {
  out_key_mems.clear();
  out_ptrs.clear();
  for (const auto& [name_view, key] : in) {
    out_key_mems.emplace(std::string(name_view), coinbase::mem_t(key.data, key.size));
  }
  for (auto& [name, key_mem] : out_key_mems) out_ptrs.emplace(name, coinbase::mpc::pve_keyref(key_mem));
}

}  // namespace

error_t encrypt_ac(const base_pke_i& base_pke, curve_id curve, const access_structure_t& ac, const leaf_keys_t& ac_pks,
                   mem_t label, const std::vector<mem_t>& xs, buf_t& out_ciphertext) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_vec_arg(xs, "xs")) return rv;
  error_t rv = UNINITIALIZED_ERROR;

  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  if (xs.empty()) return coinbase::error(E_BADARG, "batch_count must be positive");
  if (xs.size() > static_cast<size_t>(MAX_BATCH_COUNT)) return coinbase::error(E_RANGE, "batch too large");

  // Defensive check: avoid large attacker-controlled allocations when converting `xs[i]` into bignums.
  const int max_x_size = icurve.order().get_bin_size();
  for (const auto& x : xs) {
    if (x.size > max_x_size) return coinbase::error(E_RANGE, "xs element too large", /*to_print_stack_trace=*/false);
  }

  coinbase::crypto::ss::ac_owned_t ac_internal;
  std::set<std::string> leaf_names;
  if (rv = to_internal_ac_and_leaves(ac, icurve, ac_internal, leaf_names)) return rv;
  if (rv = validate_leaf_keys_exact(leaf_names, ac_pks, "ac_pks")) return rv;

  std::vector<coinbase::crypto::bn_t> xs_bn;
  xs_bn.reserve(xs.size());
  for (const auto& x : xs) xs_bn.push_back(coinbase::crypto::bn_t::from_bin(x));

  detail::base_pke_bridge_t bridge(base_pke);

  std::map<std::string, coinbase::mem_t> key_mems;
  coinbase::mpc::ec_pve_ac_t::pks_t pk_ptrs;
  build_internal_leaf_key_ptrs(ac_pks, key_mems, pk_ptrs);

  coinbase::mpc::ec_pve_ac_t pve_ct;

  out_ciphertext.free();
  rv = pve_ct.encrypt(bridge, ac_internal, pk_ptrs, label, icurve, xs_bn);
  if (rv) return rv;

  pve_ac_ciphertext_blob_v1_t blob;
  blob.batch_count = static_cast<uint32_t>(xs.size());
  blob.ct = coinbase::convert(pve_ct);

  out_ciphertext = coinbase::convert(blob);
  return SUCCESS;
}

error_t encrypt_ac(curve_id curve, const access_structure_t& ac, const leaf_keys_t& ac_pks, mem_t label,
                   const std::vector<mem_t>& xs, buf_t& out_ciphertext) {
  return encrypt_ac(base_pke_default(), curve, ac, ac_pks, label, xs, out_ciphertext);
}

error_t verify_ac(const base_pke_i& base_pke, curve_id curve, const access_structure_t& ac, const leaf_keys_t& ac_pks,
                  mem_t ciphertext, const std::vector<mem_t>& Qs_compressed, mem_t label) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_vec_arg(Qs_compressed, "Qs_compressed")) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  error_t rv = UNINITIALIZED_ERROR;

  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  coinbase::crypto::ss::ac_owned_t ac_internal;
  std::set<std::string> leaf_names;
  if (rv = to_internal_ac_and_leaves(ac, icurve, ac_internal, leaf_names)) return rv;
  if (rv = validate_leaf_keys_exact(leaf_names, ac_pks, "ac_pks")) return rv;

  pve_ac_ciphertext_blob_v1_t blob;
  if (rv = parse_ac_ciphertext(ciphertext, blob)) return rv;

  if (Qs_compressed.size() != static_cast<size_t>(blob.batch_count))
    return coinbase::error(E_BADARG, "Q count mismatch");

  coinbase::mpc::ec_pve_ac_t pve_ct;
  if (rv = coinbase::convert(pve_ct, blob.ct)) return rv;

  // Validate ciphertext curve.
  for (const auto& q : pve_ct.get_Q()) {
    if (q.get_curve() != icurve) return coinbase::error(E_BADARG, "ciphertext curve mismatch");
  }

  std::vector<coinbase::crypto::ecc_point_t> Q_expected;
  Q_expected.resize(Qs_compressed.size());
  for (size_t i = 0; i < Qs_compressed.size(); i++) {
    if (rv = Q_expected[i].from_bin(icurve, Qs_compressed[i])) return coinbase::error(rv, "invalid Q");
    if (rv = icurve.check(Q_expected[i])) return coinbase::error(rv, "invalid Q");
  }

  detail::base_pke_bridge_t bridge(base_pke);

  std::map<std::string, coinbase::mem_t> key_mems;
  coinbase::mpc::ec_pve_ac_t::pks_t pk_ptrs;
  build_internal_leaf_key_ptrs(ac_pks, key_mems, pk_ptrs);

  return pve_ct.verify(bridge, ac_internal, pk_ptrs, Q_expected, label);
}

error_t verify_ac(curve_id curve, const access_structure_t& ac, const leaf_keys_t& ac_pks, mem_t ciphertext,
                  const std::vector<mem_t>& Qs_compressed, mem_t label) {
  return verify_ac(base_pke_default(), curve, ac, ac_pks, ciphertext, Qs_compressed, label);
}

error_t partial_decrypt_ac_attempt(const base_pke_i& base_pke, curve_id curve, const access_structure_t& ac,
                                   mem_t ciphertext, int attempt_index, std::string_view leaf_name, mem_t dk,
                                   mem_t label, buf_t& out_share) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(dk, "dk", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  error_t rv = UNINITIALIZED_ERROR;

  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");
  if (leaf_name.empty()) return coinbase::error(E_BADARG, "leaf_name must be non-empty");

  coinbase::crypto::ss::ac_owned_t ac_internal;
  std::set<std::string> leaf_names;
  if (rv = to_internal_ac_and_leaves(ac, icurve, ac_internal, leaf_names)) return rv;
  if (leaf_names.count(std::string(leaf_name)) == 0) return coinbase::error(E_BADARG, "unknown leaf_name");

  pve_ac_ciphertext_blob_v1_t blob;
  if (rv = parse_ac_ciphertext(ciphertext, blob)) return rv;

  coinbase::mpc::ec_pve_ac_t pve_ct;
  if (rv = coinbase::convert(pve_ct, blob.ct)) return rv;

  for (const auto& q : pve_ct.get_Q()) {
    if (q.get_curve() != icurve) return coinbase::error(E_BADARG, "ciphertext curve mismatch");
  }

  detail::base_pke_bridge_t bridge(base_pke);

  const coinbase::mem_t dk_mem(dk.data, dk.size);
  coinbase::crypto::bn_t share_bn;
  rv = pve_ct.party_decrypt_row(bridge, ac_internal, attempt_index, std::string(leaf_name),
                                coinbase::mpc::pve_keyref(dk_mem), label, share_bn);
  if (rv) {
    out_share.free();
    return rv;
  }

  out_share = share_bn.to_bin(icurve.order().get_bin_size());
  return SUCCESS;
}

error_t partial_decrypt_ac_attempt(curve_id curve, const access_structure_t& ac, mem_t ciphertext, int attempt_index,
                                   std::string_view leaf_name, mem_t dk, mem_t label, buf_t& out_share) {
  return partial_decrypt_ac_attempt(base_pke_default(), curve, ac, ciphertext, attempt_index, leaf_name, dk, label,
                                    out_share);
}

error_t partial_decrypt_ac_attempt_rsa_oaep_hsm(curve_id curve, const access_structure_t& ac, mem_t ciphertext,
                                                int attempt_index, std::string_view leaf_name, mem_t dk_handle,
                                                mem_t ek, mem_t label, const rsa_oaep_hsm_decap_cb_t& cb,
                                                buf_t& out_share) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(dk_handle, "dk_handle")) return rv;
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(ek, "ek", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  if (!cb.decap) return coinbase::error(E_BADARG, "missing HSM RSA decap callback");

  detail::base_pke_ek_blob_v1_t ek_blob;
  error_t rv = detail::parse_ek_blob(ek, ek_blob);
  if (rv) return rv;
  if (static_cast<detail::base_pke_key_type_v1>(ek_blob.key_type) != detail::base_pke_key_type_v1::rsa_oaep_2048)
    return coinbase::error(E_BADARG, "expected RSA base PKE public key");

  detail::rsa_oaep_hsm_base_pke_t base_pke(cb);
  return partial_decrypt_ac_attempt(base_pke, curve, ac, ciphertext, attempt_index, leaf_name, dk_handle, label,
                                    out_share);
}

error_t partial_decrypt_ac_attempt_ecies_p256_hsm(curve_id curve, const access_structure_t& ac, mem_t ciphertext,
                                                  int attempt_index, std::string_view leaf_name, mem_t dk_handle,
                                                  mem_t ek, mem_t label, const ecies_p256_hsm_ecdh_cb_t& cb,
                                                  buf_t& out_share) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(dk_handle, "dk_handle")) return rv;
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(ek, "ek", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  if (!cb.ecdh) return coinbase::error(E_BADARG, "missing HSM ECIES ECDH callback");

  detail::base_pke_ek_blob_v1_t ek_blob;
  error_t rv = detail::parse_ek_blob(ek, ek_blob);
  if (rv) return rv;
  if (static_cast<detail::base_pke_key_type_v1>(ek_blob.key_type) != detail::base_pke_key_type_v1::ecies_p256)
    return coinbase::error(E_BADARG, "expected ECIES(P-256) base PKE public key");
  if (ek_blob.ecies_ek.get_curve() != coinbase::crypto::curve_p256)
    return coinbase::error(E_BADARG, "ECIES base PKE key must be on P-256");

  detail::ecies_p256_hsm_base_pke_t base_pke(ek_blob.ecies_ek.to_oct(), cb);
  return partial_decrypt_ac_attempt(base_pke, curve, ac, ciphertext, attempt_index, leaf_name, dk_handle, label,
                                    out_share);
}

error_t combine_ac(const base_pke_i& base_pke, curve_id curve, const access_structure_t& ac, mem_t ciphertext,
                   int attempt_index, mem_t label, const leaf_shares_t& quorum_shares, std::vector<buf_t>& out_xs) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  error_t rv = UNINITIALIZED_ERROR;

  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  coinbase::crypto::ss::ac_owned_t ac_internal;
  std::set<std::string> leaf_names;
  if (rv = to_internal_ac_and_leaves(ac, icurve, ac_internal, leaf_names)) return rv;
  if (rv = validate_quorum_shares_subset(leaf_names, quorum_shares, "quorum_shares")) return rv;

  pve_ac_ciphertext_blob_v1_t blob;
  if (rv = parse_ac_ciphertext(ciphertext, blob)) return rv;

  coinbase::mpc::ec_pve_ac_t pve_ct;
  if (rv = coinbase::convert(pve_ct, blob.ct)) return rv;

  for (const auto& q : pve_ct.get_Q()) {
    if (q.get_curve() != icurve) return coinbase::error(E_BADARG, "ciphertext curve mismatch");
  }

  const int expected_share_size = icurve.order().get_bin_size();
  for (const auto& [name_view, share_bytes] : quorum_shares) {
    if (share_bytes.size != expected_share_size) return coinbase::error(E_BADARG, "quorum_shares: invalid share size");
  }

  std::map<std::string, coinbase::crypto::bn_t> quorum_bn;
  for (const auto& [name_view, share_bytes] : quorum_shares) {
    quorum_bn.emplace(std::string(name_view), coinbase::crypto::bn_t::from_bin(share_bytes));
  }

  detail::base_pke_bridge_t bridge(base_pke);
  coinbase::mpc::ec_pve_ac_t::pks_t pk_ptrs;

  std::vector<coinbase::crypto::bn_t> xs_bn;
  rv = pve_ct.aggregate_to_restore_row(bridge, ac_internal, attempt_index, label, quorum_bn, xs_bn,
                                       /*skip_verify=*/true, pk_ptrs);
  if (rv) {
    out_xs.clear();
    return rv;
  }

  std::vector<buf_t> out_local;
  out_local.resize(xs_bn.size());
  const int out_len = icurve.order().get_bin_size();
  for (size_t i = 0; i < xs_bn.size(); i++) out_local[i] = xs_bn[i].to_bin(out_len);
  out_xs = std::move(out_local);
  return SUCCESS;
}

error_t combine_ac(curve_id curve, const access_structure_t& ac, mem_t ciphertext, int attempt_index, mem_t label,
                   const leaf_shares_t& quorum_shares, std::vector<buf_t>& out_xs) {
  return combine_ac(base_pke_default(), curve, ac, ciphertext, attempt_index, label, quorum_shares, out_xs);
}

error_t get_ac_batch_count(mem_t ciphertext, int& out_batch_count) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  pve_ac_ciphertext_blob_v1_t blob;
  error_t rv = parse_ac_ciphertext(ciphertext, blob);
  if (rv) return rv;
  out_batch_count = static_cast<int>(blob.batch_count);
  return SUCCESS;
}

error_t get_public_keys_compressed_ac(mem_t ciphertext, std::vector<buf_t>& out_Qs_compressed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  error_t rv = UNINITIALIZED_ERROR;

  pve_ac_ciphertext_blob_v1_t blob;
  if (rv = parse_ac_ciphertext(ciphertext, blob)) return rv;

  coinbase::mpc::ec_pve_ac_t pve_ct;  // base PKE not used for extraction
  if (rv = coinbase::convert(pve_ct, blob.ct)) return rv;

  std::vector<buf_t> out_local;
  out_local.reserve(pve_ct.get_Q().size());
  for (const auto& q : pve_ct.get_Q()) out_local.push_back(q.to_compressed_bin());

  out_Qs_compressed = std::move(out_local);
  return SUCCESS;
}

}  // namespace coinbase::api::pve
