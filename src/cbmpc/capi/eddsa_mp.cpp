#include <new>
#include <string_view>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/eddsa_mp.h>
#include <cbmpc/capi/eddsa_mp.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/job.h>

#include "access_structure_adapter.h"
#include "util.h"

using namespace coinbase::capi::detail;

extern "C" {

cbmpc_error_t cbmpc_eddsa_mp_dkg_additive(const cbmpc_mp_job_t* job, cbmpc_curve_id_t curve, cmem_t* out_key_blob,
                                          cmem_t* out_sid) {
  try {
    if (!out_key_blob || !out_sid) return E_BADARG;
    *out_key_blob = cmem_t{nullptr, 0};
    *out_sid = cmem_t{nullptr, 0};

    const auto vjob = validate_mp_job(job);
    if (vjob) return vjob;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    job_mp_cpp_ctx_t ctx(job);

    coinbase::buf_t key_blob;
    coinbase::buf_t sid;
    const coinbase::error_t rv = coinbase::api::eddsa_mp::dkg_additive(ctx.job, curve_cpp, key_blob, sid);
    if (rv) return rv;

    const auto r_key = alloc_cmem_from_buf(key_blob, out_key_blob);
    if (r_key) return r_key;

    const auto r_sid = alloc_cmem_from_buf(sid, out_sid);
    if (r_sid) {
      cbmpc_cmem_free(*out_key_blob);
      *out_key_blob = cmem_t{nullptr, 0};
      return r_sid;
    }

    return CBMPC_SUCCESS;
  } catch (const std::bad_alloc&) {
    if (out_key_blob) {
      cbmpc_cmem_free(*out_key_blob);
      *out_key_blob = cmem_t{nullptr, 0};
    }
    if (out_sid) {
      cbmpc_cmem_free(*out_sid);
      *out_sid = cmem_t{nullptr, 0};
    }
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_key_blob) {
      cbmpc_cmem_free(*out_key_blob);
      *out_key_blob = cmem_t{nullptr, 0};
    }
    if (out_sid) {
      cbmpc_cmem_free(*out_sid);
      *out_sid = cmem_t{nullptr, 0};
    }
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_eddsa_mp_dkg_ac(const cbmpc_mp_job_t* job, cbmpc_curve_id_t curve, cmem_t sid_in,
                                    const cbmpc_access_structure_t* access_structure,
                                    const char* const* quorum_party_names, int quorum_party_names_count,
                                    cmem_t* out_ac_key_blob, cmem_t* out_sid) {
  try {
    if (!out_ac_key_blob || !out_sid) return E_BADARG;
    *out_ac_key_blob = cmem_t{nullptr, 0};
    *out_sid = cmem_t{nullptr, 0};

    const auto vjob = validate_mp_job(job);
    if (vjob) return vjob;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    const auto vsi = validate_cmem(sid_in);
    if (vsi) return vsi;

    std::vector<std::string_view> quorum_names;
    const auto vqn = to_cpp_quorum_party_names(quorum_party_names, quorum_party_names_count, quorum_names);
    if (vqn) return vqn;

    coinbase::api::access_structure_t ac_cpp;
    const auto vac = to_cpp_access_structure(access_structure, ac_cpp);
    if (vac) return vac;

    job_mp_cpp_ctx_t ctx(job);

    coinbase::buf_t sid(sid_in.data, sid_in.size);
    coinbase::buf_t key_blob;
    const coinbase::error_t rv =
        coinbase::api::eddsa_mp::dkg_ac(ctx.job, curve_cpp, sid, ac_cpp, quorum_names, key_blob);
    if (rv) return rv;

    const auto r_key = alloc_cmem_from_buf(key_blob, out_ac_key_blob);
    if (r_key) return r_key;

    const auto r_sid = alloc_cmem_from_buf(sid, out_sid);
    if (r_sid) {
      cbmpc_cmem_free(*out_ac_key_blob);
      *out_ac_key_blob = cmem_t{nullptr, 0};
      return r_sid;
    }

    return CBMPC_SUCCESS;
  } catch (const std::bad_alloc&) {
    if (out_ac_key_blob) {
      cbmpc_cmem_free(*out_ac_key_blob);
      *out_ac_key_blob = cmem_t{nullptr, 0};
    }
    if (out_sid) {
      cbmpc_cmem_free(*out_sid);
      *out_sid = cmem_t{nullptr, 0};
    }
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_ac_key_blob) {
      cbmpc_cmem_free(*out_ac_key_blob);
      *out_ac_key_blob = cmem_t{nullptr, 0};
    }
    if (out_sid) {
      cbmpc_cmem_free(*out_sid);
      *out_sid = cmem_t{nullptr, 0};
    }
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_eddsa_mp_refresh_additive(const cbmpc_mp_job_t* job, cmem_t sid_in, cmem_t key_blob,
                                              cmem_t* sid_out, cmem_t* out_new_key_blob) {
  try {
    if (sid_out) *sid_out = cmem_t{nullptr, 0};
    if (!out_new_key_blob) return E_BADARG;
    *out_new_key_blob = cmem_t{nullptr, 0};

    const auto vjob = validate_mp_job(job);
    if (vjob) return vjob;

    const auto vsi = validate_cmem(sid_in);
    if (vsi) return vsi;
    const auto vkb = validate_cmem(key_blob);
    if (vkb) return vkb;

    job_mp_cpp_ctx_t ctx(job);

    coinbase::buf_t sid(sid_in.data, sid_in.size);
    coinbase::buf_t new_key;
    const coinbase::error_t rv = coinbase::api::eddsa_mp::refresh_additive(ctx.job, sid, view_cmem(key_blob), new_key);
    if (rv) return rv;

    const auto r_key = alloc_cmem_from_buf(new_key, out_new_key_blob);
    if (r_key) return r_key;

    if (sid_out) {
      const auto r_sid = alloc_cmem_from_buf(sid, sid_out);
      if (r_sid) {
        cbmpc_cmem_free(*out_new_key_blob);
        *out_new_key_blob = cmem_t{nullptr, 0};
        return r_sid;
      }
    }

    return CBMPC_SUCCESS;
  } catch (const std::bad_alloc&) {
    if (sid_out) *sid_out = cmem_t{nullptr, 0};
    if (out_new_key_blob) {
      cbmpc_cmem_free(*out_new_key_blob);
      *out_new_key_blob = cmem_t{nullptr, 0};
    }
    return E_INSUFFICIENT;
  } catch (...) {
    if (sid_out) *sid_out = cmem_t{nullptr, 0};
    if (out_new_key_blob) {
      cbmpc_cmem_free(*out_new_key_blob);
      *out_new_key_blob = cmem_t{nullptr, 0};
    }
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_eddsa_mp_refresh_ac(const cbmpc_mp_job_t* job, cmem_t sid_in, cmem_t ac_key_blob,
                                        const cbmpc_access_structure_t* access_structure,
                                        const char* const* quorum_party_names, int quorum_party_names_count,
                                        cmem_t* sid_out, cmem_t* out_new_ac_key_blob) {
  try {
    if (sid_out) *sid_out = cmem_t{nullptr, 0};
    if (!out_new_ac_key_blob) return E_BADARG;
    *out_new_ac_key_blob = cmem_t{nullptr, 0};

    const auto vjob = validate_mp_job(job);
    if (vjob) return vjob;

    const auto vsi = validate_cmem(sid_in);
    if (vsi) return vsi;
    const auto vkb = validate_cmem(ac_key_blob);
    if (vkb) return vkb;

    std::vector<std::string_view> quorum_names;
    const auto vqn = to_cpp_quorum_party_names(quorum_party_names, quorum_party_names_count, quorum_names);
    if (vqn) return vqn;

    coinbase::api::access_structure_t ac_cpp;
    const auto vac = to_cpp_access_structure(access_structure, ac_cpp);
    if (vac) return vac;

    job_mp_cpp_ctx_t ctx(job);

    coinbase::buf_t sid(sid_in.data, sid_in.size);
    coinbase::buf_t new_key;
    const coinbase::error_t rv =
        coinbase::api::eddsa_mp::refresh_ac(ctx.job, sid, view_cmem(ac_key_blob), ac_cpp, quorum_names, new_key);
    if (rv) return rv;

    const auto r_key = alloc_cmem_from_buf(new_key, out_new_ac_key_blob);
    if (r_key) return r_key;

    if (sid_out) {
      const auto r_sid = alloc_cmem_from_buf(sid, sid_out);
      if (r_sid) {
        cbmpc_cmem_free(*out_new_ac_key_blob);
        *out_new_ac_key_blob = cmem_t{nullptr, 0};
        return r_sid;
      }
    }

    return CBMPC_SUCCESS;
  } catch (const std::bad_alloc&) {
    if (sid_out) *sid_out = cmem_t{nullptr, 0};
    if (out_new_ac_key_blob) {
      cbmpc_cmem_free(*out_new_ac_key_blob);
      *out_new_ac_key_blob = cmem_t{nullptr, 0};
    }
    return E_INSUFFICIENT;
  } catch (...) {
    if (sid_out) *sid_out = cmem_t{nullptr, 0};
    if (out_new_ac_key_blob) {
      cbmpc_cmem_free(*out_new_ac_key_blob);
      *out_new_ac_key_blob = cmem_t{nullptr, 0};
    }
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_eddsa_mp_sign_additive(const cbmpc_mp_job_t* job, cmem_t key_blob, cmem_t msg, int32_t sig_receiver,
                                           cmem_t* sig_out) {
  try {
    if (!sig_out) return E_BADARG;
    *sig_out = cmem_t{nullptr, 0};

    const auto vjob = validate_mp_job(job);
    if (vjob) return vjob;

    const auto vkb = validate_cmem(key_blob);
    if (vkb) return vkb;
    const auto vm = validate_cmem(msg);
    if (vm) return vm;

    job_mp_cpp_ctx_t ctx(job);

    coinbase::buf_t sig;
    const coinbase::error_t rv =
        coinbase::api::eddsa_mp::sign_additive(ctx.job, view_cmem(key_blob), view_cmem(msg), sig_receiver, sig);
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

cbmpc_error_t cbmpc_eddsa_mp_sign_ac(const cbmpc_mp_job_t* job, cmem_t ac_key_blob,
                                     const cbmpc_access_structure_t* access_structure, cmem_t msg, int32_t sig_receiver,
                                     cmem_t* sig_out) {
  try {
    if (!sig_out) return E_BADARG;
    *sig_out = cmem_t{nullptr, 0};

    const auto vjob = validate_mp_job(job);
    if (vjob) return vjob;

    const auto vkb = validate_cmem(ac_key_blob);
    if (vkb) return vkb;
    const auto vm = validate_cmem(msg);
    if (vm) return vm;

    coinbase::api::access_structure_t ac_cpp;
    const auto vac = to_cpp_access_structure(access_structure, ac_cpp);
    if (vac) return vac;

    job_mp_cpp_ctx_t ctx(job);

    coinbase::buf_t sig;
    const coinbase::error_t rv =
        coinbase::api::eddsa_mp::sign_ac(ctx.job, view_cmem(ac_key_blob), ac_cpp, view_cmem(msg), sig_receiver, sig);
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

cbmpc_error_t cbmpc_eddsa_mp_get_public_key_compressed(cmem_t key_blob, cmem_t* out_pub_key) {
  try {
    if (!out_pub_key) return E_BADARG;
    *out_pub_key = cmem_t{nullptr, 0};
    const auto vkb = validate_cmem(key_blob);
    if (vkb) return vkb;

    coinbase::buf_t pk;
    const coinbase::error_t rv = coinbase::api::eddsa_mp::get_public_key_compressed(view_cmem(key_blob), pk);
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

cbmpc_error_t cbmpc_eddsa_mp_get_public_share_compressed(cmem_t key_blob, cmem_t* out_public_share) {
  try {
    if (!out_public_share) return E_BADARG;
    *out_public_share = cmem_t{nullptr, 0};
    const auto vkb = validate_cmem(key_blob);
    if (vkb) return vkb;

    coinbase::buf_t Qi;
    const coinbase::error_t rv = coinbase::api::eddsa_mp::get_public_share_compressed(view_cmem(key_blob), Qi);
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

cbmpc_error_t cbmpc_eddsa_mp_detach_private_scalar(cmem_t key_blob, cmem_t* out_public_key_blob,
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
        coinbase::api::eddsa_mp::detach_private_scalar(view_cmem(key_blob), public_blob, private_scalar_fixed);
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

cbmpc_error_t cbmpc_eddsa_mp_attach_private_scalar(cmem_t public_key_blob, cmem_t private_scalar_fixed,
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
    const coinbase::error_t rv = coinbase::api::eddsa_mp::attach_private_scalar(
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
