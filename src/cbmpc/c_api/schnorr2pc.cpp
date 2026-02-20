#include <new>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/schnorr_2p.h>
#include <cbmpc/c_api/schnorr_2p.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>

#include "util.h"

using namespace coinbase::capi::detail;

extern "C" {

cbmpc_error_t cbmpc_schnorr_2p_dkg(const cbmpc_2pc_job_t* job, cbmpc_curve_id_t curve, cmem_t* out_key_blob) {
  try {
    if (!out_key_blob) return E_BADARG;
    *out_key_blob = cmem_t{nullptr, 0};
    const auto vjob = validate_2pc_job(job);
    if (vjob) return vjob;

    coinbase::api::party_2p_t self_cpp;
    const auto pconv = to_cpp_party(job->self, self_cpp);
    if (pconv) return pconv;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    job_2p_cpp_ctx_t ctx(job, self_cpp);
    coinbase::buf_t key_blob;
    const coinbase::error_t rv = coinbase::api::schnorr_2p::dkg(ctx.job, curve_cpp, key_blob);
    if (rv) return rv;

    return alloc_cmem_from_buf(key_blob, out_key_blob);
  } catch (const std::bad_alloc&) {
    if (out_key_blob) *out_key_blob = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_key_blob) *out_key_blob = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_schnorr_2p_refresh(const cbmpc_2pc_job_t* job, cmem_t key_blob, cmem_t* out_new_key_blob) {
  try {
    if (!out_new_key_blob) return E_BADARG;
    *out_new_key_blob = cmem_t{nullptr, 0};
    const auto vjob = validate_2pc_job(job);
    if (vjob) return vjob;
    const auto vkb = validate_cmem(key_blob);
    if (vkb) return vkb;

    coinbase::api::party_2p_t self_cpp;
    const auto pconv = to_cpp_party(job->self, self_cpp);
    if (pconv) return pconv;

    job_2p_cpp_ctx_t ctx(job, self_cpp);
    coinbase::buf_t new_key;
    const coinbase::error_t rv = coinbase::api::schnorr_2p::refresh(ctx.job, view_cmem(key_blob), new_key);
    if (rv) return rv;

    return alloc_cmem_from_buf(new_key, out_new_key_blob);
  } catch (const std::bad_alloc&) {
    if (out_new_key_blob) *out_new_key_blob = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_new_key_blob) *out_new_key_blob = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_schnorr_2p_sign(const cbmpc_2pc_job_t* job, cmem_t key_blob, cmem_t msg, cmem_t* sig_out) {
  try {
    if (!sig_out) return E_BADARG;
    *sig_out = cmem_t{nullptr, 0};
    const auto vjob = validate_2pc_job(job);
    if (vjob) return vjob;
    const auto vkb = validate_cmem(key_blob);
    if (vkb) return vkb;
    const auto vm = validate_cmem(msg);
    if (vm) return vm;

    coinbase::api::party_2p_t self_cpp;
    const auto pconv = to_cpp_party(job->self, self_cpp);
    if (pconv) return pconv;

    job_2p_cpp_ctx_t ctx(job, self_cpp);
    coinbase::buf_t sig;
    const coinbase::error_t rv = coinbase::api::schnorr_2p::sign(ctx.job, view_cmem(key_blob), view_cmem(msg), sig);
    if (rv) return rv;

    return alloc_cmem_from_buf(sig, sig_out);
  } catch (const std::bad_alloc&) {
    if (sig_out) *sig_out = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (sig_out) *sig_out = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_schnorr_2p_get_public_key_compressed(cmem_t key_blob, cmem_t* out_pub_key) {
  try {
    if (!out_pub_key) return E_BADARG;
    *out_pub_key = cmem_t{nullptr, 0};
    const auto vkb = validate_cmem(key_blob);
    if (vkb) return vkb;

    coinbase::buf_t pk;
    const coinbase::error_t rv = coinbase::api::schnorr_2p::get_public_key_compressed(view_cmem(key_blob), pk);
    if (rv) return rv;

    return alloc_cmem_from_buf(pk, out_pub_key);
  } catch (const std::bad_alloc&) {
    if (out_pub_key) *out_pub_key = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_pub_key) *out_pub_key = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_schnorr_2p_extract_public_key_xonly(cmem_t key_blob, cmem_t* out_pub_key) {
  try {
    if (!out_pub_key) return E_BADARG;
    *out_pub_key = cmem_t{nullptr, 0};
    const auto vkb = validate_cmem(key_blob);
    if (vkb) return vkb;

    coinbase::buf_t pk;
    const coinbase::error_t rv = coinbase::api::schnorr_2p::extract_public_key_xonly(view_cmem(key_blob), pk);
    if (rv) return rv;

    return alloc_cmem_from_buf(pk, out_pub_key);
  } catch (const std::bad_alloc&) {
    if (out_pub_key) *out_pub_key = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_pub_key) *out_pub_key = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_schnorr_2p_get_public_share_compressed(cmem_t key_blob, cmem_t* out_public_share) {
  try {
    if (!out_public_share) return E_BADARG;
    *out_public_share = cmem_t{nullptr, 0};
    const auto vkb = validate_cmem(key_blob);
    if (vkb) return vkb;

    coinbase::buf_t Qi;
    const coinbase::error_t rv = coinbase::api::schnorr_2p::get_public_share_compressed(view_cmem(key_blob), Qi);
    if (rv) return rv;
    return alloc_cmem_from_buf(Qi, out_public_share);
  } catch (const std::bad_alloc&) {
    if (out_public_share) *out_public_share = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_public_share) *out_public_share = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_schnorr_2p_detach_private_scalar(cmem_t key_blob, cmem_t* out_public_key_blob,
                                                     cmem_t* out_private_scalar_fixed) {
  try {
    if (!out_public_key_blob || !out_private_scalar_fixed) return E_BADARG;
    *out_public_key_blob = cmem_t{nullptr, 0};
    *out_private_scalar_fixed = cmem_t{nullptr, 0};
    const auto vkb = validate_cmem(key_blob);
    if (vkb) return vkb;

    coinbase::buf_t public_blob;
    coinbase::buf_t private_scalar_fixed;
    const coinbase::error_t rv =
        coinbase::api::schnorr_2p::detach_private_scalar(view_cmem(key_blob), public_blob, private_scalar_fixed);
    if (rv) return rv;

    const auto r1 = alloc_cmem_from_buf(public_blob, out_public_key_blob);
    if (r1) return r1;
    const auto r2 = alloc_cmem_from_buf(private_scalar_fixed, out_private_scalar_fixed);
    if (r2) {
      cbmpc_cmem_free(*out_public_key_blob);
      *out_public_key_blob = cmem_t{nullptr, 0};
      return r2;
    }
    return CBMPC_SUCCESS;
  } catch (const std::bad_alloc&) {
    if (out_public_key_blob) {
      cbmpc_cmem_free(*out_public_key_blob);
      *out_public_key_blob = cmem_t{nullptr, 0};
    }
    if (out_private_scalar_fixed) {
      cbmpc_cmem_free(*out_private_scalar_fixed);
      *out_private_scalar_fixed = cmem_t{nullptr, 0};
    }
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_public_key_blob) {
      cbmpc_cmem_free(*out_public_key_blob);
      *out_public_key_blob = cmem_t{nullptr, 0};
    }
    if (out_private_scalar_fixed) {
      cbmpc_cmem_free(*out_private_scalar_fixed);
      *out_private_scalar_fixed = cmem_t{nullptr, 0};
    }
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_schnorr_2p_attach_private_scalar(cmem_t public_key_blob, cmem_t private_scalar_fixed,
                                                     cmem_t public_share_compressed, cmem_t* out_key_blob) {
  try {
    if (!out_key_blob) return E_BADARG;
    *out_key_blob = cmem_t{nullptr, 0};
    const auto vpb = validate_cmem(public_key_blob);
    if (vpb) return vpb;
    const auto vx = validate_cmem(private_scalar_fixed);
    if (vx) return vx;
    const auto vq = validate_cmem(public_share_compressed);
    if (vq) return vq;

    coinbase::buf_t merged;
    const coinbase::error_t rv = coinbase::api::schnorr_2p::attach_private_scalar(
        view_cmem(public_key_blob), view_cmem(private_scalar_fixed), view_cmem(public_share_compressed), merged);
    if (rv) return rv;
    return alloc_cmem_from_buf(merged, out_key_blob);
  } catch (const std::bad_alloc&) {
    if (out_key_blob) *out_key_blob = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_key_blob) *out_key_blob = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

}  // extern "C"
