#include <new>

#include <cbmpc/api/pve_batch_ac.h>
#include <cbmpc/c_api/pve_batch_ac.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

#include "access_structure_adapter.h"
#include "pve_internal.h"
#include "util.h"

using namespace coinbase::capi::detail;
using coinbase::capi::pve_detail::c_base_pke_adapter_t;
using coinbase::capi::pve_detail::ecies_p256_hsm_ecdh_cpp;
using coinbase::capi::pve_detail::rsa_oaep_hsm_decap_cpp;

namespace {

static cbmpc_error_t validate_leaf_key_mapping(const char* const* leaf_names, const cmem_t* leaf_keys, int leaf_count,
                                               coinbase::api::pve::leaf_keys_t& out) {
  out.clear();
  if (leaf_count < 0) return E_BADARG;
  if (leaf_count == 0) return CBMPC_SUCCESS;
  if (!leaf_names || !leaf_keys) return E_BADARG;

  for (int i = 0; i < leaf_count; i++) {
    const char* name = leaf_names[i];
    if (!name || name[0] == '\0') return E_BADARG;
    const auto vk = validate_cmem(leaf_keys[i]);
    if (vk) return vk;

    const auto [it, inserted] =
        out.emplace(std::string_view(name), coinbase::mem_t(leaf_keys[i].data, leaf_keys[i].size));
    if (!inserted) return E_BADARG;  // duplicate leaf name
  }
  return CBMPC_SUCCESS;
}

static cbmpc_error_t validate_leaf_share_mapping(const char* const* leaf_names, const cmem_t* leaf_shares,
                                                 int leaf_count, coinbase::api::pve::leaf_shares_t& out) {
  out.clear();
  if (leaf_count < 0) return E_BADARG;
  if (leaf_count == 0) return CBMPC_SUCCESS;
  if (!leaf_names || !leaf_shares) return E_BADARG;

  for (int i = 0; i < leaf_count; i++) {
    const char* name = leaf_names[i];
    if (!name || name[0] == '\0') return E_BADARG;
    const auto vs = validate_cmem(leaf_shares[i]);
    if (vs) return vs;

    const auto [it, inserted] =
        out.emplace(std::string_view(name), coinbase::mem_t(leaf_shares[i].data, leaf_shares[i].size));
    if (!inserted) return E_BADARG;  // duplicate leaf name
  }
  return CBMPC_SUCCESS;
}

}  // namespace

extern "C" {

cbmpc_error_t cbmpc_pve_ac_encrypt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve,
                                   const cbmpc_access_structure_t* ac, const char* const* leaf_names,
                                   const cmem_t* leaf_eks, int leaf_count, cmem_t label, cmems_t xs,
                                   cmem_t* out_ciphertext) {
  try {
    if (!out_ciphertext) return E_BADARG;
    *out_ciphertext = cmem_t{nullptr, 0};

    const auto vl = validate_cmem(label);
    if (vl) return vl;

    std::vector<coinbase::mem_t> xs_cpp;
    const auto vxs = view_cmems(xs, xs_cpp);
    if (vxs) return vxs;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::api::access_structure_t ac_cpp;
    const auto aconv = coinbase::capi::detail::to_cpp_access_structure(ac, ac_cpp);
    if (aconv) return aconv;

    coinbase::api::pve::leaf_keys_t keys_cpp;
    const auto kmap = validate_leaf_key_mapping(leaf_names, leaf_eks, leaf_count, keys_cpp);
    if (kmap) return kmap;

    coinbase::buf_t ct;
    coinbase::error_t rv = UNINITIALIZED_ERROR;
    if (base_pke) {
      c_base_pke_adapter_t adapter(base_pke);
      rv = coinbase::api::pve::encrypt_ac(adapter, curve_cpp, ac_cpp, keys_cpp, view_cmem(label), xs_cpp, ct);
    } else {
      rv = coinbase::api::pve::encrypt_ac(curve_cpp, ac_cpp, keys_cpp, view_cmem(label), xs_cpp, ct);
    }
    if (rv) return rv;

    return alloc_cmem_from_buf(ct, out_ciphertext);
  } catch (const std::bad_alloc&) {
    if (out_ciphertext) *out_ciphertext = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_ciphertext) *out_ciphertext = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_ac_verify(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve,
                                  const cbmpc_access_structure_t* ac, const char* const* leaf_names,
                                  const cmem_t* leaf_eks, int leaf_count, cmem_t ciphertext, cmems_t Qs_compressed,
                                  cmem_t label) {
  try {
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vl = validate_cmem(label);
    if (vl) return vl;

    std::vector<coinbase::mem_t> qs_cpp;
    const auto vqs = view_cmems(Qs_compressed, qs_cpp);
    if (vqs) return vqs;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::api::access_structure_t ac_cpp;
    const auto aconv = coinbase::capi::detail::to_cpp_access_structure(ac, ac_cpp);
    if (aconv) return aconv;

    coinbase::api::pve::leaf_keys_t keys_cpp;
    const auto kmap = validate_leaf_key_mapping(leaf_names, leaf_eks, leaf_count, keys_cpp);
    if (kmap) return kmap;

    if (base_pke) {
      c_base_pke_adapter_t adapter(base_pke);
      return coinbase::api::pve::verify_ac(adapter, curve_cpp, ac_cpp, keys_cpp, view_cmem(ciphertext), qs_cpp,
                                           view_cmem(label));
    }
    return coinbase::api::pve::verify_ac(curve_cpp, ac_cpp, keys_cpp, view_cmem(ciphertext), qs_cpp, view_cmem(label));
  } catch (const std::bad_alloc&) {
    return E_INSUFFICIENT;
  } catch (...) {
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_ac_partial_decrypt_attempt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve,
                                                   const cbmpc_access_structure_t* ac, cmem_t ciphertext,
                                                   int attempt_index, const char* leaf_name, cmem_t dk, cmem_t label,
                                                   cmem_t* out_share) {
  try {
    if (!out_share) return E_BADARG;
    *out_share = cmem_t{nullptr, 0};

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vdk = validate_cmem(dk);
    if (vdk) return vdk;
    const auto vl = validate_cmem(label);
    if (vl) return vl;
    if (!leaf_name || leaf_name[0] == '\0') return E_BADARG;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::api::access_structure_t ac_cpp;
    const auto aconv = coinbase::capi::detail::to_cpp_access_structure(ac, ac_cpp);
    if (aconv) return aconv;

    coinbase::buf_t share;
    coinbase::error_t rv = UNINITIALIZED_ERROR;
    if (base_pke) {
      c_base_pke_adapter_t adapter(base_pke);
      rv = coinbase::api::pve::partial_decrypt_ac_attempt(adapter, curve_cpp, ac_cpp, view_cmem(ciphertext),
                                                          attempt_index, std::string_view(leaf_name), view_cmem(dk),
                                                          view_cmem(label), share);
    } else {
      rv = coinbase::api::pve::partial_decrypt_ac_attempt(curve_cpp, ac_cpp, view_cmem(ciphertext), attempt_index,
                                                          std::string_view(leaf_name), view_cmem(dk), view_cmem(label),
                                                          share);
    }
    if (rv) return rv;

    return alloc_cmem_from_buf(share, out_share);
  } catch (const std::bad_alloc&) {
    if (out_share) *out_share = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_share) *out_share = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_ac_partial_decrypt_attempt_rsa_oaep_hsm(cbmpc_curve_id_t curve,
                                                                const cbmpc_access_structure_t* ac, cmem_t ciphertext,
                                                                int attempt_index, const char* leaf_name,
                                                                cmem_t dk_handle, cmem_t ek, cmem_t label,
                                                                const cbmpc_pve_rsa_oaep_hsm_decap_t* cb,
                                                                cmem_t* out_share) {
  try {
    if (!out_share) return E_BADARG;
    *out_share = cmem_t{nullptr, 0};
    if (!cb || !cb->decap) return E_BADARG;

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vdk = validate_cmem(dk_handle);
    if (vdk) return vdk;
    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vl = validate_cmem(label);
    if (vl) return vl;
    if (!leaf_name || leaf_name[0] == '\0') return E_BADARG;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::api::access_structure_t ac_cpp;
    const auto aconv = coinbase::capi::detail::to_cpp_access_structure(ac, ac_cpp);
    if (aconv) return aconv;

    coinbase::api::pve::rsa_oaep_hsm_decap_cb_t cb_cpp;
    cb_cpp.ctx = const_cast<cbmpc_pve_rsa_oaep_hsm_decap_t*>(cb);
    cb_cpp.decap = rsa_oaep_hsm_decap_cpp;

    coinbase::buf_t share;
    const coinbase::error_t rv = coinbase::api::pve::partial_decrypt_ac_attempt_rsa_oaep_hsm(
        curve_cpp, ac_cpp, view_cmem(ciphertext), attempt_index, std::string_view(leaf_name), view_cmem(dk_handle),
        view_cmem(ek), view_cmem(label), cb_cpp, share);
    if (rv) return rv;

    return alloc_cmem_from_buf(share, out_share);
  } catch (const std::bad_alloc&) {
    if (out_share) *out_share = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_share) *out_share = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_ac_partial_decrypt_attempt_ecies_p256_hsm(cbmpc_curve_id_t curve,
                                                                  const cbmpc_access_structure_t* ac, cmem_t ciphertext,
                                                                  int attempt_index, const char* leaf_name,
                                                                  cmem_t dk_handle, cmem_t ek, cmem_t label,
                                                                  const cbmpc_pve_ecies_p256_hsm_ecdh_t* cb,
                                                                  cmem_t* out_share) {
  try {
    if (!out_share) return E_BADARG;
    *out_share = cmem_t{nullptr, 0};
    if (!cb || !cb->ecdh) return E_BADARG;

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vdk = validate_cmem(dk_handle);
    if (vdk) return vdk;
    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vl = validate_cmem(label);
    if (vl) return vl;
    if (!leaf_name || leaf_name[0] == '\0') return E_BADARG;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::api::access_structure_t ac_cpp;
    const auto aconv = coinbase::capi::detail::to_cpp_access_structure(ac, ac_cpp);
    if (aconv) return aconv;

    coinbase::api::pve::ecies_p256_hsm_ecdh_cb_t cb_cpp;
    cb_cpp.ctx = const_cast<cbmpc_pve_ecies_p256_hsm_ecdh_t*>(cb);
    cb_cpp.ecdh = ecies_p256_hsm_ecdh_cpp;

    coinbase::buf_t share;
    const coinbase::error_t rv = coinbase::api::pve::partial_decrypt_ac_attempt_ecies_p256_hsm(
        curve_cpp, ac_cpp, view_cmem(ciphertext), attempt_index, std::string_view(leaf_name), view_cmem(dk_handle),
        view_cmem(ek), view_cmem(label), cb_cpp, share);
    if (rv) return rv;

    return alloc_cmem_from_buf(share, out_share);
  } catch (const std::bad_alloc&) {
    if (out_share) *out_share = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_share) *out_share = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_ac_combine(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve,
                                   const cbmpc_access_structure_t* ac, const char* const* quorum_leaf_names,
                                   const cmem_t* quorum_shares, int quorum_count, cmem_t ciphertext, int attempt_index,
                                   cmem_t label, cmems_t* out_xs) {
  try {
    if (!out_xs) return E_BADARG;
    *out_xs = cmems_t{0, nullptr, nullptr};

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vl = validate_cmem(label);
    if (vl) return vl;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::api::access_structure_t ac_cpp;
    const auto aconv = coinbase::capi::detail::to_cpp_access_structure(ac, ac_cpp);
    if (aconv) return aconv;

    coinbase::api::pve::leaf_shares_t quorum_cpp;
    const auto qmap = validate_leaf_share_mapping(quorum_leaf_names, quorum_shares, quorum_count, quorum_cpp);
    if (qmap) return qmap;

    std::vector<coinbase::buf_t> xs_cpp;
    coinbase::error_t rv = UNINITIALIZED_ERROR;
    if (base_pke) {
      c_base_pke_adapter_t adapter(base_pke);
      rv = coinbase::api::pve::combine_ac(adapter, curve_cpp, ac_cpp, view_cmem(ciphertext), attempt_index,
                                          view_cmem(label), quorum_cpp, xs_cpp);
    } else {
      rv = coinbase::api::pve::combine_ac(curve_cpp, ac_cpp, view_cmem(ciphertext), attempt_index, view_cmem(label),
                                          quorum_cpp, xs_cpp);
    }
    if (rv) return rv;

    return alloc_cmems_from_bufs(xs_cpp, out_xs);
  } catch (const std::bad_alloc&) {
    if (out_xs) *out_xs = cmems_t{0, nullptr, nullptr};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_xs) *out_xs = cmems_t{0, nullptr, nullptr};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_ac_get_count(cmem_t ciphertext, int* out_batch_count) {
  try {
    if (!out_batch_count) return E_BADARG;
    *out_batch_count = 0;

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;

    int n = 0;
    const coinbase::error_t rv = coinbase::api::pve::get_ac_batch_count(view_cmem(ciphertext), n);
    if (rv) return rv;

    *out_batch_count = n;
    return CBMPC_SUCCESS;
  } catch (const std::bad_alloc&) {
    if (out_batch_count) *out_batch_count = 0;
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_batch_count) *out_batch_count = 0;
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_ac_get_Qs(cmem_t ciphertext, cmems_t* out_Qs_compressed) {
  try {
    if (!out_Qs_compressed) return E_BADARG;
    *out_Qs_compressed = cmems_t{0, nullptr, nullptr};

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;

    std::vector<coinbase::buf_t> Qs;
    const coinbase::error_t rv = coinbase::api::pve::get_public_keys_compressed_ac(view_cmem(ciphertext), Qs);
    if (rv) return rv;

    return alloc_cmems_from_bufs(Qs, out_Qs_compressed);
  } catch (const std::bad_alloc&) {
    if (out_Qs_compressed) *out_Qs_compressed = cmems_t{0, nullptr, nullptr};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_Qs_compressed) *out_Qs_compressed = cmems_t{0, nullptr, nullptr};
    return E_GENERAL;
  }
}

}  // extern "C"
