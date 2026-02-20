#pragma once

#include <cbmpc/api/pve.h>
#include <cbmpc/capi/pve.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

namespace coinbase::capi::pve_detail {

class c_base_pke_adapter_t final : public coinbase::api::pve::base_pke_i {
 public:
  explicit c_base_pke_adapter_t(const cbmpc_pve_base_pke_t* p) : p_(p) {}

  coinbase::error_t encrypt(coinbase::mem_t ek, coinbase::mem_t label, coinbase::mem_t plain, coinbase::mem_t rho,
                            coinbase::buf_t& out_ct) const override {
    if (!p_ || !p_->encrypt) return E_BADARG;

    cmem_t out{nullptr, 0};
    const cbmpc_error_t rv = p_->encrypt(
        p_->ctx, cmem_t{const_cast<byte_ptr>(ek.data), ek.size}, cmem_t{const_cast<byte_ptr>(label.data), label.size},
        cmem_t{const_cast<byte_ptr>(plain.data), plain.size}, cmem_t{const_cast<byte_ptr>(rho.data), rho.size}, &out);
    if (rv) {
      if (out.data) cbmpc_cmem_free(out);
      return rv;
    }

    if (out.size < 0 || (out.size > 0 && !out.data)) {
      // Callback violated the ABI contract; do not attempt to read.
      cbmpc_free(out.data);
      return E_FORMAT;
    }
    out_ct = coinbase::buf_t(out.data, out.size);
    cbmpc_cmem_free(out);
    return CBMPC_SUCCESS;
  }

  coinbase::error_t decrypt(coinbase::mem_t dk, coinbase::mem_t label, coinbase::mem_t ct,
                            coinbase::buf_t& out_plain) const override {
    if (!p_ || !p_->decrypt) return E_BADARG;

    cmem_t out{nullptr, 0};
    const cbmpc_error_t rv = p_->decrypt(p_->ctx, cmem_t{const_cast<byte_ptr>(dk.data), dk.size},
                                         cmem_t{const_cast<byte_ptr>(label.data), label.size},
                                         cmem_t{const_cast<byte_ptr>(ct.data), ct.size}, &out);
    if (rv) {
      if (out.data) cbmpc_cmem_free(out);
      return rv;
    }

    if (out.size < 0 || (out.size > 0 && !out.data)) {
      cbmpc_free(out.data);
      return E_FORMAT;
    }
    out_plain = coinbase::buf_t(out.data, out.size);
    cbmpc_cmem_free(out);
    return CBMPC_SUCCESS;
  }

 private:
  const cbmpc_pve_base_pke_t* p_ = nullptr;
};

inline coinbase::error_t rsa_oaep_hsm_decap_cpp(void* ctx, coinbase::mem_t dk_handle, coinbase::mem_t kem_ct,
                                                coinbase::buf_t& out_kem_ss) {
  const auto* cb = static_cast<const cbmpc_pve_rsa_oaep_hsm_decap_t*>(ctx);
  if (!cb || !cb->decap) return E_BADARG;

  cmem_t kem_ss{nullptr, 0};
  const cbmpc_error_t rv = cb->decap(cb->ctx, cmem_t{const_cast<byte_ptr>(dk_handle.data), dk_handle.size},
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

inline coinbase::error_t ecies_p256_hsm_ecdh_cpp(void* ctx, coinbase::mem_t dk_handle, coinbase::mem_t kem_ct,
                                                 coinbase::buf_t& out_dh_x32) {
  const auto* cb = static_cast<const cbmpc_pve_ecies_p256_hsm_ecdh_t*>(ctx);
  if (!cb || !cb->ecdh) return E_BADARG;

  cmem_t dh{nullptr, 0};
  const cbmpc_error_t rv = cb->ecdh(cb->ctx, cmem_t{const_cast<byte_ptr>(dk_handle.data), dk_handle.size},
                                    cmem_t{const_cast<byte_ptr>(kem_ct.data), kem_ct.size}, &dh);
  if (rv) {
    if (dh.data) cbmpc_cmem_free(dh);
    return rv;
  }
  if (dh.size < 0 || (dh.size > 0 && !dh.data)) {
    cbmpc_free(dh.data);
    return E_FORMAT;
  }
  if (dh.size != 32) {
    cbmpc_cmem_free(dh);
    return E_CRYPTO;
  }

  out_dh_x32 = coinbase::buf_t(dh.data, dh.size);
  cbmpc_cmem_free(dh);
  return CBMPC_SUCCESS;
}

}  // namespace coinbase::capi::pve_detail
