#include <climits>
#include <cstdlib>
#include <cstring>
#include <new>
#include <type_traits>
#include <vector>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/tdh2.h>
#include <cbmpc/c_api/tdh2.h>
#include <cbmpc/core/access_structure.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

#include "access_structure_adapter.h"
#include "util.h"

namespace {

using namespace coinbase::capi::detail;

struct tdh2_dkg_out_guard_t {
  cmem_t* out_public_key = nullptr;
  cmems_t* out_public_shares = nullptr;
  cmem_t* out_private_share = nullptr;
  cmem_t* out_sid = nullptr;
  bool released = false;

  tdh2_dkg_out_guard_t(cmem_t* pk, cmems_t* pub, cmem_t* priv, cmem_t* sid)
      : out_public_key(pk), out_public_shares(pub), out_private_share(priv), out_sid(sid) {}

  tdh2_dkg_out_guard_t(const tdh2_dkg_out_guard_t&) = delete;
  tdh2_dkg_out_guard_t& operator=(const tdh2_dkg_out_guard_t&) = delete;
  tdh2_dkg_out_guard_t(tdh2_dkg_out_guard_t&&) = delete;
  tdh2_dkg_out_guard_t& operator=(tdh2_dkg_out_guard_t&&) = delete;

  void release() { released = true; }

  void cleanup() const {
    cbmpc_cmem_free(*out_public_key);
    cbmpc_cmems_free(*out_public_shares);
    cbmpc_cmem_free(*out_private_share);
    cbmpc_cmem_free(*out_sid);

    *out_public_key = cmem_t{nullptr, 0};
    *out_public_shares = cmems_t{0, nullptr, nullptr};
    *out_private_share = cmem_t{nullptr, 0};
    *out_sid = cmem_t{nullptr, 0};
  }

  ~tdh2_dkg_out_guard_t() {
    if (!released) cleanup();
  }
};

static_assert(!std::is_copy_constructible_v<tdh2_dkg_out_guard_t>);
static_assert(!std::is_copy_assignable_v<tdh2_dkg_out_guard_t>);
static_assert(!std::is_move_constructible_v<tdh2_dkg_out_guard_t>);
static_assert(!std::is_move_assignable_v<tdh2_dkg_out_guard_t>);

}  // namespace

extern "C" {

cbmpc_error_t cbmpc_tdh2_dkg_additive(const cbmpc_mp_job_t* job, cbmpc_curve_id_t curve, cmem_t* out_public_key,
                                      cmems_t* out_public_shares, cmem_t* out_private_share, cmem_t* out_sid) {
  try {
    if (!out_public_key || !out_public_shares || !out_private_share || !out_sid) return E_BADARG;
    *out_public_key = cmem_t{nullptr, 0};
    *out_public_shares = cmems_t{0, nullptr, nullptr};
    *out_private_share = cmem_t{nullptr, 0};
    *out_sid = cmem_t{nullptr, 0};
    tdh2_dkg_out_guard_t out_guard(out_public_key, out_public_shares, out_private_share, out_sid);

    const auto vjob = validate_mp_job(job);
    if (vjob) return vjob;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::capi::detail::job_mp_cpp_ctx_t job_ctx(job);

    coinbase::buf_t pk;
    std::vector<coinbase::buf_t> pub_shares;
    coinbase::buf_t priv_share;
    coinbase::buf_t sid;

    const coinbase::error_t rv =
        coinbase::api::tdh2::dkg_additive(job_ctx.job, curve_cpp, pk, pub_shares, priv_share, sid);
    if (rv) return rv;

    auto r1 = alloc_cmem_from_buf(pk, out_public_key);
    if (r1) return r1;
    auto r2 = alloc_cmems_from_bufs(pub_shares, out_public_shares);
    if (r2) return r2;
    auto r3 = alloc_cmem_from_buf(priv_share, out_private_share);
    if (r3) return r3;

    auto r4 = alloc_cmem_from_buf(sid, out_sid);
    if (r4) return r4;

    out_guard.release();
    return CBMPC_SUCCESS;
  } catch (const std::bad_alloc&) {
    return E_INSUFFICIENT;
  } catch (...) {
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_tdh2_dkg_ac(const cbmpc_mp_job_t* job, cbmpc_curve_id_t curve, cmem_t sid_in,
                                const cbmpc_access_structure_t* access_structure, const char* const* quorum_party_names,
                                int quorum_party_names_count, cmem_t* out_public_key, cmems_t* out_public_shares,
                                cmem_t* out_private_share, cmem_t* out_sid) {
  try {
    if (!out_public_key || !out_public_shares || !out_private_share || !out_sid) return E_BADARG;
    *out_public_key = cmem_t{nullptr, 0};
    *out_public_shares = cmems_t{0, nullptr, nullptr};
    *out_private_share = cmem_t{nullptr, 0};
    *out_sid = cmem_t{nullptr, 0};
    tdh2_dkg_out_guard_t out_guard(out_public_key, out_public_shares, out_private_share, out_sid);

    const auto vjob = validate_mp_job(job);
    if (vjob) return vjob;
    const auto vsid = validate_cmem(sid_in);
    if (vsid) return vsid;
    if (!access_structure) return E_BADARG;

    coinbase::api::curve_id curve_cpp;
    const auto cconv = to_cpp_curve(curve, curve_cpp);
    if (cconv) return cconv;

    coinbase::api::access_structure_t ac_cpp;
    const auto ac_rv = to_cpp_access_structure(access_structure, ac_cpp);
    if (ac_rv) return ac_rv;

    std::vector<std::string_view> quorum_cpp;
    const auto qrv = to_cpp_quorum_party_names(quorum_party_names, quorum_party_names_count, quorum_cpp);
    if (qrv) return qrv;

    coinbase::capi::detail::job_mp_cpp_ctx_t job_ctx(job);

    coinbase::buf_t pk;
    std::vector<coinbase::buf_t> pub_shares;
    coinbase::buf_t priv_share;
    coinbase::buf_t sid(view_cmem(sid_in));

    const coinbase::error_t rv =
        coinbase::api::tdh2::dkg_ac(job_ctx.job, curve_cpp, sid, ac_cpp, quorum_cpp, pk, pub_shares, priv_share);
    if (rv) return rv;

    auto r1 = alloc_cmem_from_buf(pk, out_public_key);
    if (r1) return r1;
    auto r2 = alloc_cmems_from_bufs(pub_shares, out_public_shares);
    if (r2) return r2;
    auto r3 = alloc_cmem_from_buf(priv_share, out_private_share);
    if (r3) return r3;
    auto r4 = alloc_cmem_from_buf(sid, out_sid);
    if (r4) return r4;

    out_guard.release();
    return CBMPC_SUCCESS;
  } catch (const std::bad_alloc&) {
    return E_INSUFFICIENT;
  } catch (...) {
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_tdh2_encrypt(cmem_t public_key, cmem_t plaintext, cmem_t label, cmem_t* out_ciphertext) {
  try {
    if (!out_ciphertext) return E_BADARG;
    *out_ciphertext = cmem_t{nullptr, 0};
    const auto vpk = validate_cmem(public_key);
    if (vpk) return vpk;
    const auto vpt = validate_cmem(plaintext);
    if (vpt) return vpt;
    const auto vlb = validate_cmem(label);
    if (vlb) return vlb;

    coinbase::buf_t ct;
    const coinbase::error_t rv =
        coinbase::api::tdh2::encrypt(view_cmem(public_key), view_cmem(plaintext), view_cmem(label), ct);
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

cbmpc_error_t cbmpc_tdh2_verify(cmem_t public_key, cmem_t ciphertext, cmem_t label) {
  try {
    const auto vpk = validate_cmem(public_key);
    if (vpk) return vpk;
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vlb = validate_cmem(label);
    if (vlb) return vlb;
    return coinbase::api::tdh2::verify(view_cmem(public_key), view_cmem(ciphertext), view_cmem(label));
  } catch (const std::bad_alloc&) {
    return E_INSUFFICIENT;
  } catch (...) {
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_tdh2_partial_decrypt(cmem_t private_share, cmem_t ciphertext, cmem_t label,
                                         cmem_t* out_partial_decryption) {
  try {
    if (!out_partial_decryption) return E_BADARG;
    *out_partial_decryption = cmem_t{nullptr, 0};
    const auto vps = validate_cmem(private_share);
    if (vps) return vps;
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;
    const auto vlb = validate_cmem(label);
    if (vlb) return vlb;

    coinbase::buf_t partial;
    const coinbase::error_t rv = coinbase::api::tdh2::partial_decrypt(view_cmem(private_share), view_cmem(ciphertext),
                                                                      view_cmem(label), partial);
    if (rv) return rv;
    return alloc_cmem_from_buf(partial, out_partial_decryption);
  } catch (const std::bad_alloc&) {
    if (out_partial_decryption) *out_partial_decryption = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_partial_decryption) *out_partial_decryption = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_tdh2_combine_additive(cmem_t public_key, cmems_t public_shares, cmem_t label,
                                          cmems_t partial_decryptions, cmem_t ciphertext, cmem_t* out_plaintext) {
  try {
    if (!out_plaintext) return E_BADARG;
    *out_plaintext = cmem_t{nullptr, 0};
    const auto vpk = validate_cmem(public_key);
    if (vpk) return vpk;
    const auto vlb = validate_cmem(label);
    if (vlb) return vlb;
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;

    std::vector<coinbase::mem_t> pub_shares;
    std::vector<coinbase::mem_t> partials;
    auto rv = view_cmems(public_shares, pub_shares);
    if (rv) return rv;
    rv = view_cmems(partial_decryptions, partials);
    if (rv) return rv;

    coinbase::buf_t plain;
    const coinbase::error_t crv = coinbase::api::tdh2::combine_additive(
        view_cmem(public_key), pub_shares, view_cmem(label), partials, view_cmem(ciphertext), plain);
    if (crv) return crv;

    return alloc_cmem_from_buf(plain, out_plaintext);
  } catch (const std::bad_alloc&) {
    if (out_plaintext) *out_plaintext = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_plaintext) *out_plaintext = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

cbmpc_error_t cbmpc_tdh2_combine_ac(const cbmpc_access_structure_t* access_structure, cmem_t public_key,
                                    const char* const* party_names, int party_names_count, cmems_t public_shares,
                                    cmem_t label, const char* const* partial_decryption_party_names,
                                    int partial_decryption_party_names_count, cmems_t partial_decryptions,
                                    cmem_t ciphertext, cmem_t* out_plaintext) {
  try {
    if (!out_plaintext) return E_BADARG;
    *out_plaintext = cmem_t{nullptr, 0};
    if (!access_structure) return E_BADARG;

    const auto vpk = validate_cmem(public_key);
    if (vpk) return vpk;
    const auto vlb = validate_cmem(label);
    if (vlb) return vlb;
    const auto vct = validate_cmem(ciphertext);
    if (vct) return vct;

    // Convert access structure.
    coinbase::api::access_structure_t ac_cpp;
    const auto ac_rv = to_cpp_access_structure(access_structure, ac_cpp);
    if (ac_rv) return ac_rv;

    // Convert party names.
    std::vector<std::string_view> party_names_cpp;
    {
      if (party_names_count < 0) return E_BADARG;
      if (party_names_count == 0) return E_BADARG;
      if (!party_names) return E_BADARG;
      party_names_cpp.reserve(static_cast<size_t>(party_names_count));
      for (int i = 0; i < party_names_count; i++) {
        const char* s = party_names[i];
        if (!s) return E_BADARG;
        if (s[0] == '\0') return E_BADARG;
        party_names_cpp.emplace_back(s);
      }
    }

    // Convert public shares.
    std::vector<coinbase::mem_t> public_shares_cpp;
    auto rv = view_cmems(public_shares, public_shares_cpp);
    if (rv) return rv;
    if (public_shares_cpp.size() != static_cast<size_t>(party_names_count)) return E_BADARG;

    // Convert partial decryption party names.
    std::vector<std::string_view> partial_names_cpp;
    {
      if (partial_decryption_party_names_count < 0) return E_BADARG;
      if (partial_decryption_party_names_count == 0) return E_BADARG;
      if (!partial_decryption_party_names) return E_BADARG;
      partial_names_cpp.reserve(static_cast<size_t>(partial_decryption_party_names_count));
      for (int i = 0; i < partial_decryption_party_names_count; i++) {
        const char* s = partial_decryption_party_names[i];
        if (!s) return E_BADARG;
        if (s[0] == '\0') return E_BADARG;
        partial_names_cpp.emplace_back(s);
      }
    }

    // Convert partial decryptions.
    std::vector<coinbase::mem_t> partials_cpp;
    rv = view_cmems(partial_decryptions, partials_cpp);
    if (rv) return rv;
    if (partials_cpp.size() != static_cast<size_t>(partial_decryption_party_names_count)) return E_BADARG;

    coinbase::buf_t plain;
    const coinbase::error_t crv = coinbase::api::tdh2::combine_ac(
        ac_cpp, view_cmem(public_key), party_names_cpp, public_shares_cpp, view_cmem(label), partial_names_cpp,
        partials_cpp, view_cmem(ciphertext), plain);
    if (crv) return crv;

    return alloc_cmem_from_buf(plain, out_plaintext);
  } catch (const std::bad_alloc&) {
    if (out_plaintext) *out_plaintext = cmem_t{nullptr, 0};
    return E_INSUFFICIENT;
  } catch (...) {
    if (out_plaintext) *out_plaintext = cmem_t{nullptr, 0};
    return E_GENERAL;
  }
}

}  // extern "C"
