#include <new>

#include <cbmpc/api/pve_batch.h>
#include <cbmpc/capi/pve_batch.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

#include "pve_internal.h"
#include "util.h"

using namespace coinbase::capi::detail;
using coinbase::capi::pve_detail::c_base_pke_adapter_t;
using coinbase::capi::pve_detail::ecies_p256_hsm_ecdh_cpp;
using coinbase::capi::pve_detail::rsa_oaep_hsm_decap_cpp;

extern "C" {

cbmpc_error_t cbmpc_pve_batch_encrypt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t ek,
                                      cmem_t label, cmems_t xs, cmem_t* out_ciphertext) {
  try {
    if (!out_ciphertext) return E_BADARG;
    *out_ciphertext = cmem_t{nullptr, 0};

    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vl = validate_cmem(label);
    if (vl) return vl;

    std::vector<coinbase::mem_t> xs_cpp;
    const auto vxs = view_cmems(xs, xs_cpp);
    if (vxs) return vxs;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::buf_t ct;
    coinbase::error_t rv = UNINITIALIZED_ERROR;
    if (base_pke) {
      c_base_pke_adapter_t adapter(base_pke);
      rv = coinbase::api::pve::encrypt_batch(adapter, curve_cpp, view_cmem(ek), view_cmem(label), xs_cpp, ct);
    } else {
      rv = coinbase::api::pve::encrypt_batch(curve_cpp, view_cmem(ek), view_cmem(label), xs_cpp, ct);
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

cbmpc_error_t cbmpc_pve_batch_verify(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t ek,
                                     cmem_t ciphertext, cmems_t Qs_compressed, cmem_t label) {
  try {
    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vl = validate_cmem(label);
    if (vl) return vl;

    std::vector<coinbase::mem_t> Qs_cpp;
    const auto vqs = view_cmems(Qs_compressed, Qs_cpp);
    if (vqs) return vqs;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    if (base_pke) {
      c_base_pke_adapter_t adapter(base_pke);
      return coinbase::api::pve::verify_batch(adapter, curve_cpp, view_cmem(ek), view_cmem(ciphertext), Qs_cpp,
                                              view_cmem(label));
    }
    return coinbase::api::pve::verify_batch(curve_cpp, view_cmem(ek), view_cmem(ciphertext), Qs_cpp, view_cmem(label));
  } catch (const std::bad_alloc&) {
    return E_INSUFFICIENT;
  } catch (...) {
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_batch_decrypt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t dk,
                                      cmem_t ek, cmem_t ciphertext, cmem_t label, cmems_t* out_xs) {
  try {
    if (!out_xs) return E_BADARG;
    *out_xs = cmems_t{0, nullptr, nullptr};

    const auto vdk = validate_cmem(dk);
    if (vdk) return vdk;
    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vl = validate_cmem(label);
    if (vl) return vl;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    std::vector<coinbase::buf_t> xs_cpp;
    coinbase::error_t rv = UNINITIALIZED_ERROR;
    if (base_pke) {
      c_base_pke_adapter_t adapter(base_pke);
      rv = coinbase::api::pve::decrypt_batch(adapter, curve_cpp, view_cmem(dk), view_cmem(ek), view_cmem(ciphertext),
                                             view_cmem(label), xs_cpp);
    } else {
      rv = coinbase::api::pve::decrypt_batch(curve_cpp, view_cmem(dk), view_cmem(ek), view_cmem(ciphertext),
                                             view_cmem(label), xs_cpp);
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

cbmpc_error_t cbmpc_pve_batch_decrypt_rsa_oaep_hsm(cbmpc_curve_id_t curve, cmem_t dk_handle, cmem_t ek,
                                                   cmem_t ciphertext, cmem_t label,
                                                   const cbmpc_pve_rsa_oaep_hsm_decap_t* cb, cmems_t* out_xs) {
  try {
    if (!out_xs) return E_BADARG;
    *out_xs = cmems_t{0, nullptr, nullptr};
    if (!cb) return E_BADARG;

    const auto vdk = validate_cmem(dk_handle);
    if (vdk) return vdk;
    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vl = validate_cmem(label);
    if (vl) return vl;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::api::pve::rsa_oaep_hsm_decap_cb_t cb_cpp;
    cb_cpp.ctx = const_cast<cbmpc_pve_rsa_oaep_hsm_decap_t*>(cb);
    cb_cpp.decap = rsa_oaep_hsm_decap_cpp;

    std::vector<coinbase::buf_t> xs_cpp;
    const coinbase::error_t rv = coinbase::api::pve::decrypt_batch_rsa_oaep_hsm(
        curve_cpp, view_cmem(dk_handle), view_cmem(ek), view_cmem(ciphertext), view_cmem(label), cb_cpp, xs_cpp);
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

cbmpc_error_t cbmpc_pve_batch_decrypt_ecies_p256_hsm(cbmpc_curve_id_t curve, cmem_t dk_handle, cmem_t ek,
                                                     cmem_t ciphertext, cmem_t label,
                                                     const cbmpc_pve_ecies_p256_hsm_ecdh_t* cb, cmems_t* out_xs) {
  try {
    if (!out_xs) return E_BADARG;
    *out_xs = cmems_t{0, nullptr, nullptr};
    if (!cb) return E_BADARG;

    const auto vdk = validate_cmem(dk_handle);
    if (vdk) return vdk;
    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vl = validate_cmem(label);
    if (vl) return vl;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::api::pve::ecies_p256_hsm_ecdh_cb_t cb_cpp;
    cb_cpp.ctx = const_cast<cbmpc_pve_ecies_p256_hsm_ecdh_t*>(cb);
    cb_cpp.ecdh = ecies_p256_hsm_ecdh_cpp;

    std::vector<coinbase::buf_t> xs_cpp;
    const coinbase::error_t rv = coinbase::api::pve::decrypt_batch_ecies_p256_hsm(
        curve_cpp, view_cmem(dk_handle), view_cmem(ek), view_cmem(ciphertext), view_cmem(label), cb_cpp, xs_cpp);
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

cbmpc_error_t cbmpc_pve_batch_get_count(cmem_t ciphertext, int* out_batch_count) {
  try {
    if (!out_batch_count) return E_BADARG;
    *out_batch_count = 0;

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;

    int n = 0;
    const coinbase::error_t rv = coinbase::api::pve::get_batch_count(view_cmem(ciphertext), n);
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

cbmpc_error_t cbmpc_pve_batch_get_Qs(cmem_t ciphertext, cmems_t* out_Qs_compressed) {
  try {
    if (!out_Qs_compressed) return E_BADARG;
    *out_Qs_compressed = cmems_t{0, nullptr, nullptr};

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;

    std::vector<coinbase::buf_t> Qs;
    const coinbase::error_t rv = coinbase::api::pve::get_Qs_batch(view_cmem(ciphertext), Qs);
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

cbmpc_error_t cbmpc_pve_batch_get_Label(cmem_t ciphertext, cmem_t* out_label) {
  try {
    if (!out_label) return E_BADARG;
    *out_label = cmem_t{nullptr, 0};

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;

    coinbase::buf_t L;
    const coinbase::error_t rv = coinbase::api::pve::get_Label_batch(view_cmem(ciphertext), L);
    if (rv) return rv;

    return alloc_cmem_from_buf(L, out_label);
  } catch (const std::bad_alloc&) {
    if (out_label) *out_label = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_label) *out_label = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

}  // extern "C"
