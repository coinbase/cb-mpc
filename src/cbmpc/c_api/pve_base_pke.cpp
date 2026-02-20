#include <new>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/pve_base_pke.h>
#include <cbmpc/c_api/pve_base_pke.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/protocol/pve_base.h>

#include "pve_internal.h"
#include "util.h"

using namespace coinbase::capi::detail;

namespace {
using coinbase::capi::pve_detail::c_base_pke_adapter_t;
using coinbase::capi::pve_detail::ecies_p256_hsm_ecdh_cpp;
using coinbase::capi::pve_detail::rsa_oaep_hsm_decap_cpp;

static coinbase::error_t kem_encap_shim(void* ctx, coinbase::mem_t ek_bytes, coinbase::mem_t rho32,
                                        coinbase::buf_t& out_kem_ct, coinbase::buf_t& out_kem_ss) {
  auto* kem = static_cast<const cbmpc_pve_base_kem_t*>(ctx);
  if (!kem || !kem->encap) return E_BADARG;
  if (rho32.size != 32) return E_BADARG;

  cmem_t kem_ct{nullptr, 0};
  cmem_t kem_ss{nullptr, 0};
  const cbmpc_error_t rv = kem->encap(kem->ctx, cmem_t{const_cast<byte_ptr>(ek_bytes.data), ek_bytes.size},
                                      cmem_t{const_cast<byte_ptr>(rho32.data), rho32.size}, &kem_ct, &kem_ss);
  if (rv) {
    if (kem_ct.data) cbmpc_cmem_free(kem_ct);
    if (kem_ss.data) cbmpc_cmem_free(kem_ss);
    return rv;
  }

  if (kem_ct.size < 0 || (kem_ct.size > 0 && !kem_ct.data)) {
    cbmpc_free(kem_ct.data);
    if (kem_ss.data) cbmpc_cmem_free(kem_ss);
    return E_FORMAT;
  }
  if (kem_ss.size < 0 || (kem_ss.size > 0 && !kem_ss.data)) {
    cbmpc_free(kem_ss.data);
    if (kem_ct.data) cbmpc_cmem_free(kem_ct);
    return E_FORMAT;
  }
  out_kem_ct = coinbase::buf_t(kem_ct.data, kem_ct.size);
  out_kem_ss = coinbase::buf_t(kem_ss.data, kem_ss.size);
  cbmpc_cmem_free(kem_ct);
  cbmpc_cmem_free(kem_ss);
  return CBMPC_SUCCESS;
}

static coinbase::error_t kem_decap_shim(void* ctx, const void* dk_handle, coinbase::mem_t kem_ct,
                                        coinbase::buf_t& out_kem_ss) {
  auto* kem = static_cast<const cbmpc_pve_base_kem_t*>(ctx);
  if (!kem || !kem->decap) return E_BADARG;
  if (!dk_handle) return E_BADARG;
  const auto* dk_mem = static_cast<const coinbase::mem_t*>(dk_handle);

  cmem_t kem_ss{nullptr, 0};
  const cbmpc_error_t rv = kem->decap(kem->ctx, cmem_t{const_cast<byte_ptr>(dk_mem->data), dk_mem->size},
                                      cmem_t{const_cast<byte_ptr>(kem_ct.data), kem_ct.size}, &kem_ss);
  if (rv) {
    if (kem_ss.data) cbmpc_cmem_free(kem_ss);
    return rv;
  }

  if (kem_ss.size < 0 || (kem_ss.size > 0 && !kem_ss.data)) {
    cbmpc_free(kem_ss.data);
    return E_FORMAT;
  }
  out_kem_ss = coinbase::buf_t(kem_ss.data, kem_ss.size);
  cbmpc_cmem_free(kem_ss);
  return CBMPC_SUCCESS;
}

class c_base_kem_adapter_t final : public coinbase::api::pve::base_pke_i {
 public:
  explicit c_base_kem_adapter_t(const cbmpc_pve_base_kem_t* kem) : kem_(kem) {}

  coinbase::error_t encrypt(coinbase::mem_t ek, coinbase::mem_t label, coinbase::mem_t plain, coinbase::mem_t rho,
                            coinbase::buf_t& out_ct) const override {
    if (!kem_ || !kem_->encap) return E_BADARG;

    coinbase::mpc::pve_runtime_kem_callbacks_t callbacks;
    callbacks.ctx = const_cast<cbmpc_pve_base_kem_t*>(kem_);
    callbacks.encap = kem_encap_shim;
    callbacks.decap = kem_decap_shim;

    coinbase::mpc::pve_runtime_kem_ek_t ek_i;
    ek_i.ek_bytes = ek;
    ek_i.callbacks = &callbacks;

    return coinbase::mpc::pve_base_pke_runtime_kem().encrypt(coinbase::mpc::pve_keyref(ek_i), label, plain, rho,
                                                             out_ct);
  }

  coinbase::error_t decrypt(coinbase::mem_t dk, coinbase::mem_t label, coinbase::mem_t ct,
                            coinbase::buf_t& out_plain) const override {
    if (!kem_ || !kem_->decap) return E_BADARG;

    coinbase::mpc::pve_runtime_kem_callbacks_t callbacks;
    callbacks.ctx = const_cast<cbmpc_pve_base_kem_t*>(kem_);
    callbacks.encap = kem_encap_shim;
    callbacks.decap = kem_decap_shim;

    const coinbase::mem_t dk_mem(dk.data, dk.size);
    coinbase::mpc::pve_runtime_kem_dk_t dk_i;
    dk_i.dk_handle = &dk_mem;
    dk_i.callbacks = &callbacks;

    return coinbase::mpc::pve_base_pke_runtime_kem().decrypt(coinbase::mpc::pve_keyref(dk_i), label, ct, out_plain);
  }

 private:
  const cbmpc_pve_base_kem_t* kem_ = nullptr;
};

}  // namespace

extern "C" {

cbmpc_error_t cbmpc_pve_generate_base_pke_rsa_keypair(cmem_t* out_ek, cmem_t* out_dk) {
  try {
    if (!out_ek || !out_dk) return E_BADARG;
    *out_ek = cmem_t{nullptr, 0};
    *out_dk = cmem_t{nullptr, 0};

    coinbase::buf_t ek_blob;
    coinbase::buf_t dk_blob;
    const coinbase::error_t rv = coinbase::api::pve::generate_base_pke_rsa_keypair(ek_blob, dk_blob);
    if (rv) return rv;

    const cbmpc_error_t rv_ek = alloc_cmem_from_buf(ek_blob, out_ek);
    if (rv_ek) return rv_ek;
    const cbmpc_error_t rv_dk = alloc_cmem_from_buf(dk_blob, out_dk);
    if (rv_dk) {
      cbmpc_cmem_free(*out_ek);
      *out_ek = cmem_t{nullptr, 0};
      return rv_dk;
    }
    return CBMPC_SUCCESS;
  } catch (const std::bad_alloc&) {
    if (out_ek && out_ek->data) cbmpc_cmem_free(*out_ek);
    if (out_dk && out_dk->data) cbmpc_cmem_free(*out_dk);
    if (out_ek) *out_ek = cmem_t{nullptr, 0};
    if (out_dk) *out_dk = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_ek && out_ek->data) cbmpc_cmem_free(*out_ek);
    if (out_dk && out_dk->data) cbmpc_cmem_free(*out_dk);
    if (out_ek) *out_ek = cmem_t{nullptr, 0};
    if (out_dk) *out_dk = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_generate_base_pke_ecies_p256_keypair(cmem_t* out_ek, cmem_t* out_dk) {
  try {
    if (!out_ek || !out_dk) return E_BADARG;
    *out_ek = cmem_t{nullptr, 0};
    *out_dk = cmem_t{nullptr, 0};

    coinbase::buf_t ek_blob;
    coinbase::buf_t dk_blob;
    const coinbase::error_t rv = coinbase::api::pve::generate_base_pke_ecies_p256_keypair(ek_blob, dk_blob);
    if (rv) return rv;

    const cbmpc_error_t rv_ek = alloc_cmem_from_buf(ek_blob, out_ek);
    if (rv_ek) return rv_ek;
    const cbmpc_error_t rv_dk = alloc_cmem_from_buf(dk_blob, out_dk);
    if (rv_dk) {
      cbmpc_cmem_free(*out_ek);
      *out_ek = cmem_t{nullptr, 0};
      return rv_dk;
    }
    return CBMPC_SUCCESS;
  } catch (const std::bad_alloc&) {
    if (out_ek && out_ek->data) cbmpc_cmem_free(*out_ek);
    if (out_dk && out_dk->data) cbmpc_cmem_free(*out_dk);
    if (out_ek) *out_ek = cmem_t{nullptr, 0};
    if (out_dk) *out_dk = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_ek && out_ek->data) cbmpc_cmem_free(*out_ek);
    if (out_dk && out_dk->data) cbmpc_cmem_free(*out_dk);
    if (out_ek) *out_ek = cmem_t{nullptr, 0};
    if (out_dk) *out_dk = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_base_pke_ecies_p256_ek_from_oct(cmem_t pub_key_oct, cmem_t* out_ek) {
  try {
    if (!out_ek) return E_BADARG;
    *out_ek = cmem_t{nullptr, 0};

    const auto vpk = validate_cmem(pub_key_oct);
    if (vpk) return vpk;

    coinbase::buf_t ek_blob;
    const coinbase::error_t rv = coinbase::api::pve::base_pke_ecies_p256_ek_from_oct(view_cmem(pub_key_oct), ek_blob);
    if (rv) return rv;

    return alloc_cmem_from_buf(ek_blob, out_ek);
  } catch (const std::bad_alloc&) {
    if (out_ek) *out_ek = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_ek) *out_ek = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_base_pke_rsa_ek_from_modulus(cmem_t modulus, cmem_t* out_ek) {
  try {
    if (!out_ek) return E_BADARG;
    *out_ek = cmem_t{nullptr, 0};

    const auto vm = validate_cmem(modulus);
    if (vm) return vm;

    coinbase::buf_t ek_blob;
    const coinbase::error_t rv = coinbase::api::pve::base_pke_rsa_ek_from_modulus(view_cmem(modulus), ek_blob);
    if (rv) return rv;

    return alloc_cmem_from_buf(ek_blob, out_ek);
  } catch (const std::bad_alloc&) {
    if (out_ek) *out_ek = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_ek) *out_ek = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_decrypt_rsa_oaep_hsm(cbmpc_curve_id_t curve, cmem_t dk_handle, cmem_t ek, cmem_t ciphertext,
                                             cmem_t label, const cbmpc_pve_rsa_oaep_hsm_decap_t* cb, cmem_t* out_x) {
  try {
    if (!out_x) return E_BADARG;
    *out_x = cmem_t{nullptr, 0};
    if (!cb || !cb->decap) return E_BADARG;

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

    coinbase::buf_t x_out;
    const coinbase::error_t rv = coinbase::api::pve::decrypt_rsa_oaep_hsm(
        curve_cpp, view_cmem(dk_handle), view_cmem(ek), view_cmem(ciphertext), view_cmem(label), cb_cpp, x_out);
    if (rv) return rv;

    return alloc_cmem_from_buf(x_out, out_x);
  } catch (const std::bad_alloc&) {
    if (out_x) *out_x = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_x) *out_x = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_decrypt_ecies_p256_hsm(cbmpc_curve_id_t curve, cmem_t dk_handle, cmem_t ek, cmem_t ciphertext,
                                               cmem_t label, const cbmpc_pve_ecies_p256_hsm_ecdh_t* cb, cmem_t* out_x) {
  try {
    if (!out_x) return E_BADARG;
    *out_x = cmem_t{nullptr, 0};
    if (!cb || !cb->ecdh) return E_BADARG;

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

    coinbase::buf_t x_out;
    const coinbase::error_t rv = coinbase::api::pve::decrypt_ecies_p256_hsm(
        curve_cpp, view_cmem(dk_handle), view_cmem(ek), view_cmem(ciphertext), view_cmem(label), cb_cpp, x_out);
    if (rv) return rv;

    return alloc_cmem_from_buf(x_out, out_x);
  } catch (const std::bad_alloc&) {
    if (out_x) *out_x = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_x) *out_x = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_encrypt_with_kem(const cbmpc_pve_base_kem_t* kem, cbmpc_curve_id_t curve, cmem_t ek,
                                         cmem_t label, cmem_t x, cmem_t* out_ciphertext) {
  try {
    if (!out_ciphertext) return E_BADARG;
    *out_ciphertext = cmem_t{nullptr, 0};
    if (!kem || !kem->encap) return E_BADARG;

    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vl = validate_cmem(label);
    if (vl) return vl;
    const auto vx = validate_cmem(x);
    if (vx) return vx;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    c_base_kem_adapter_t adapter(kem);
    coinbase::buf_t ct;
    const coinbase::error_t rv =
        coinbase::api::pve::encrypt(adapter, curve_cpp, view_cmem(ek), view_cmem(label), view_cmem(x), ct);
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

cbmpc_error_t cbmpc_pve_verify_with_kem(const cbmpc_pve_base_kem_t* kem, cbmpc_curve_id_t curve, cmem_t ek,
                                        cmem_t ciphertext, cmem_t Q_compressed, cmem_t label) {
  try {
    if (!kem || !kem->encap) return E_BADARG;

    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vq = validate_cmem(Q_compressed);
    if (vq) return vq;
    const auto vl = validate_cmem(label);
    if (vl) return vl;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    c_base_kem_adapter_t adapter(kem);
    return coinbase::api::pve::verify(adapter, curve_cpp, view_cmem(ek), view_cmem(ciphertext), view_cmem(Q_compressed),
                                      view_cmem(label));
  } catch (const std::bad_alloc&) {
    return E_INSUFFICIENT;
  } catch (...) {
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_decrypt_with_kem(const cbmpc_pve_base_kem_t* kem, cbmpc_curve_id_t curve, cmem_t dk, cmem_t ek,
                                         cmem_t ciphertext, cmem_t label, cmem_t* out_x) {
  try {
    if (!out_x) return E_BADARG;
    *out_x = cmem_t{nullptr, 0};
    if (!kem || !kem->decap) return E_BADARG;

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

    c_base_kem_adapter_t adapter(kem);
    coinbase::buf_t x_out;
    const coinbase::error_t rv = coinbase::api::pve::decrypt(adapter, curve_cpp, view_cmem(dk), view_cmem(ek),
                                                             view_cmem(ciphertext), view_cmem(label), x_out);
    if (rv) return rv;

    return alloc_cmem_from_buf(x_out, out_x);
  } catch (const std::bad_alloc&) {
    if (out_x) *out_x = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_x) *out_x = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_encrypt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t ek, cmem_t label,
                                cmem_t x, cmem_t* out_ciphertext) {
  try {
    if (!out_ciphertext) return E_BADARG;
    *out_ciphertext = cmem_t{nullptr, 0};

    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vl = validate_cmem(label);
    if (vl) return vl;
    const auto vx = validate_cmem(x);
    if (vx) return vx;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::buf_t ct;
    coinbase::error_t rv = UNINITIALIZED_ERROR;
    if (base_pke) {
      c_base_pke_adapter_t adapter(base_pke);
      rv = coinbase::api::pve::encrypt(adapter, curve_cpp, view_cmem(ek), view_cmem(label), view_cmem(x), ct);
    } else {
      rv = coinbase::api::pve::encrypt(curve_cpp, view_cmem(ek), view_cmem(label), view_cmem(x), ct);
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

cbmpc_error_t cbmpc_pve_verify(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t ek,
                               cmem_t ciphertext, cmem_t Q_compressed, cmem_t label) {
  try {
    const auto vek = validate_cmem(ek);
    if (vek) return vek;
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vq = validate_cmem(Q_compressed);
    if (vq) return vq;
    const auto vl = validate_cmem(label);
    if (vl) return vl;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    if (base_pke) {
      c_base_pke_adapter_t adapter(base_pke);
      return coinbase::api::pve::verify(adapter, curve_cpp, view_cmem(ek), view_cmem(ciphertext),
                                        view_cmem(Q_compressed), view_cmem(label));
    }
    return coinbase::api::pve::verify(curve_cpp, view_cmem(ek), view_cmem(ciphertext), view_cmem(Q_compressed),
                                      view_cmem(label));
  } catch (const std::bad_alloc&) {
    return E_INSUFFICIENT;
  } catch (...) {
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_decrypt(const cbmpc_pve_base_pke_t* base_pke, cbmpc_curve_id_t curve, cmem_t dk, cmem_t ek,
                                cmem_t ciphertext, cmem_t label, cmem_t* out_x) {
  try {
    if (!out_x) return E_BADARG;
    *out_x = cmem_t{nullptr, 0};

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

    coinbase::buf_t x_out;
    coinbase::error_t rv = UNINITIALIZED_ERROR;
    if (base_pke) {
      c_base_pke_adapter_t adapter(base_pke);
      rv = coinbase::api::pve::decrypt(adapter, curve_cpp, view_cmem(dk), view_cmem(ek), view_cmem(ciphertext),
                                       view_cmem(label), x_out);
    } else {
      rv = coinbase::api::pve::decrypt(curve_cpp, view_cmem(dk), view_cmem(ek), view_cmem(ciphertext), view_cmem(label),
                                       x_out);
    }
    if (rv) return rv;

    return alloc_cmem_from_buf(x_out, out_x);
  } catch (const std::bad_alloc&) {
    if (out_x) *out_x = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_x) *out_x = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_get_Q(cmem_t ciphertext, cmem_t* out_Q_compressed) {
  try {
    if (!out_Q_compressed) return E_BADARG;
    *out_Q_compressed = cmem_t{nullptr, 0};

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;

    coinbase::buf_t Q;
    const coinbase::error_t rv = coinbase::api::pve::get_public_key_compressed(view_cmem(ciphertext), Q);
    if (rv) return rv;

    return alloc_cmem_from_buf(Q, out_Q_compressed);
  } catch (const std::bad_alloc&) {
    if (out_Q_compressed) *out_Q_compressed = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_Q_compressed) *out_Q_compressed = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_pve_get_Label(cmem_t ciphertext, cmem_t* out_label) {
  try {
    if (!out_label) return E_BADARG;
    *out_label = cmem_t{nullptr, 0};

    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;

    coinbase::buf_t L;
    const coinbase::error_t rv = coinbase::api::pve::get_Label(view_cmem(ciphertext), L);
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
