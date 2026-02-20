#pragma once

#include <cstdint>
#include <utility>

#include <cbmpc/api/pve_base_pke.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/protocol/pve_base.h>

namespace coinbase::api::pve::detail {

// Bridge from the public `coinbase::api::pve::base_pke_i` to the internal
// `coinbase::mpc::pve_base_pke_i`.
class base_pke_bridge_t final : public coinbase::mpc::pve_base_pke_i {
 public:
  explicit base_pke_bridge_t(const base_pke_i& base_pke) : base_pke_(base_pke) {}

  error_t encrypt(coinbase::mpc::pve_keyref_t ek, mem_t label, mem_t plain, mem_t rho, buf_t& out_ct) const override {
    const auto* ek_mem = ek.get<coinbase::mem_t>();
    if (!ek_mem) return coinbase::error(E_BADARG, "invalid encryption key");
    return base_pke_.encrypt(*ek_mem, label, plain, rho, out_ct);
  }

  error_t decrypt(coinbase::mpc::pve_keyref_t dk, mem_t label, mem_t ct, buf_t& out_plain) const override {
    const auto* dk_mem = dk.get<coinbase::mem_t>();
    if (!dk_mem) return coinbase::error(E_BADARG, "invalid decryption key");
    return base_pke_.decrypt(*dk_mem, label, ct, out_plain);
  }

 private:
  const base_pke_i& base_pke_;
};

// ---------------------------------------------------------------------------
// Built-in base PKE key blob format (internal implementation detail)
// ---------------------------------------------------------------------------

constexpr uint32_t base_pke_key_blob_version_v1 = 1;

enum class base_pke_key_type_v1 : uint32_t {
  rsa_oaep_2048 = 1,
  ecies_p256 = 2,
};

struct base_pke_ek_blob_v1_t {
  uint32_t version = base_pke_key_blob_version_v1;
  uint32_t key_type = static_cast<uint32_t>(base_pke_key_type_v1::rsa_oaep_2048);

  coinbase::crypto::rsa_pub_key_t rsa_ek;
  coinbase::crypto::ecc_pub_key_t ecies_ek;

  void convert(coinbase::converter_t& c) {
    c.convert(version, key_type);
    switch (static_cast<base_pke_key_type_v1>(key_type)) {
      case base_pke_key_type_v1::rsa_oaep_2048:
        c.convert(rsa_ek);
        return;
      case base_pke_key_type_v1::ecies_p256:
        c.convert(ecies_ek);
        return;
      default:
        c.set_error();
        return;
    }
  }
};

struct base_pke_dk_blob_v1_t {
  uint32_t version = base_pke_key_blob_version_v1;
  uint32_t key_type = static_cast<uint32_t>(base_pke_key_type_v1::rsa_oaep_2048);

  coinbase::crypto::rsa_prv_key_t rsa_dk;
  coinbase::crypto::ecc_prv_key_t ecies_dk;

  void convert(coinbase::converter_t& c) {
    c.convert(version, key_type);
    switch (static_cast<base_pke_key_type_v1>(key_type)) {
      case base_pke_key_type_v1::rsa_oaep_2048:
        c.convert(rsa_dk);
        return;
      case base_pke_key_type_v1::ecies_p256:
        c.convert(ecies_dk);
        return;
      default:
        c.set_error();
        return;
    }
  }
};

inline error_t parse_ek_blob(mem_t ek, base_pke_ek_blob_v1_t& out) {
  error_t rv = coinbase::convert(out, ek);
  if (rv) return rv;
  if (out.version != base_pke_key_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported base PKE key version");
  return SUCCESS;
}

inline error_t parse_dk_blob(mem_t dk, base_pke_dk_blob_v1_t& out) {
  error_t rv = coinbase::convert(out, dk);
  if (rv) return rv;
  if (out.version != base_pke_key_blob_version_v1) return coinbase::error(E_FORMAT, "unsupported base PKE key version");
  return SUCCESS;
}

// ---------------------------------------------------------------------------
// Built-in HSM base PKE adapters (KEM decapsulation callback only)
// ---------------------------------------------------------------------------

class rsa_oaep_hsm_base_pke_t final : public base_pke_i {
 public:
  explicit rsa_oaep_hsm_base_pke_t(const rsa_oaep_hsm_decap_cb_t& cb) : cb_(cb) {}

  error_t encrypt(mem_t ek, mem_t label, mem_t plain, mem_t rho, buf_t& out_ct) const override {
    base_pke_ek_blob_v1_t blob;
    error_t rv = parse_ek_blob(ek, blob);
    if (rv) return rv;
    if (static_cast<base_pke_key_type_v1>(blob.key_type) != base_pke_key_type_v1::rsa_oaep_2048)
      return coinbase::error(E_BADARG, "RSA-OAEP HSM decrypt requires an RSA base PKE public key");
    return coinbase::mpc::pve_base_pke_rsa().encrypt(coinbase::mpc::pve_keyref(blob.rsa_ek), label, plain, rho, out_ct);
  }

  error_t decrypt(mem_t dk_handle, mem_t label, mem_t ct, buf_t& out_plain) const override {
    if (!cb_.decap) return coinbase::error(E_BADARG, "missing HSM RSA decap callback");
    coinbase::mpc::pve_rsa_oaep_hsm_dk_t dk;
    dk.dk_handle = dk_handle;
    dk.ctx = cb_.ctx;
    dk.decap = cb_.decap;
    return coinbase::mpc::pve_base_pke_rsa_oaep_hsm().decrypt(coinbase::mpc::pve_keyref(dk), label, ct, out_plain);
  }

 private:
  const rsa_oaep_hsm_decap_cb_t& cb_;
};

class ecies_p256_hsm_base_pke_t final : public base_pke_i {
 public:
  ecies_p256_hsm_base_pke_t(buf_t pub_key_oct, const ecies_p256_hsm_ecdh_cb_t& cb)
      : pub_key_oct_(std::move(pub_key_oct)), cb_(cb) {}

  error_t encrypt(mem_t ek, mem_t label, mem_t plain, mem_t rho, buf_t& out_ct) const override {
    base_pke_ek_blob_v1_t blob;
    error_t rv = parse_ek_blob(ek, blob);
    if (rv) return rv;
    if (static_cast<base_pke_key_type_v1>(blob.key_type) != base_pke_key_type_v1::ecies_p256)
      return coinbase::error(E_BADARG, "ECIES(P-256) HSM decrypt requires an ECIES base PKE public key");
    if (blob.ecies_ek.get_curve() != coinbase::crypto::curve_p256)
      return coinbase::error(E_BADARG, "ECIES base PKE key must be on P-256");
    return coinbase::mpc::pve_base_pke_ecies().encrypt(coinbase::mpc::pve_keyref(blob.ecies_ek), label, plain, rho,
                                                       out_ct);
  }

  error_t decrypt(mem_t dk_handle, mem_t label, mem_t ct, buf_t& out_plain) const override {
    if (!cb_.ecdh) return coinbase::error(E_BADARG, "missing HSM ECIES ECDH callback");
    coinbase::mpc::pve_ecies_p256_hsm_dk_t dk;
    dk.dk_handle = dk_handle;
    dk.ctx = cb_.ctx;
    dk.ecdh = cb_.ecdh;
    dk.pub_key_oct = pub_key_oct_;
    return coinbase::mpc::pve_base_pke_ecies_p256_hsm().decrypt(coinbase::mpc::pve_keyref(dk), label, ct, out_plain);
  }

 private:
  buf_t pub_key_oct_;
  const ecies_p256_hsm_ecdh_cb_t& cb_;
};

}  // namespace coinbase::api::pve::detail
