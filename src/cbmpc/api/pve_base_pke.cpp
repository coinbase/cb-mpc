#include <cbmpc/api/pve_base_pke.h>
#include <cbmpc/internal/core/convert.h>
#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/protocol/pve.h>
#include <cbmpc/internal/protocol/pve_base.h>

#include "curve_util.h"
#include "mem_util.h"
#include "pve_internal.h"

namespace coinbase::api::pve {

namespace {

constexpr uint32_t pve_ciphertext_version_v1 = 1;

using coinbase::api::detail::to_internal_curve;

struct pve_ciphertext_blob_v1_t {
  uint32_t version = pve_ciphertext_version_v1;
  buf_t ct;  // serialized `coinbase::mpc::ec_pve_t`

  void convert(coinbase::converter_t& c) { c.convert(version, ct); }
};

using detail::base_pke_bridge_t;
using detail::base_pke_dk_blob_v1_t;
using detail::base_pke_ek_blob_v1_t;
using detail::base_pke_key_type_v1;
using detail::ecies_p256_hsm_base_pke_t;
using detail::parse_dk_blob;
using detail::parse_ek_blob;
using detail::rsa_oaep_hsm_base_pke_t;

class unified_key_blob_base_pke_t final : public base_pke_i {
 public:
  error_t encrypt(mem_t ek, mem_t label, mem_t plain, mem_t rho, buf_t& out_ct) const override {
    base_pke_ek_blob_v1_t blob;
    error_t rv = parse_ek_blob(ek, blob);
    if (rv) return rv;

    switch (static_cast<base_pke_key_type_v1>(blob.key_type)) {
      case base_pke_key_type_v1::rsa_oaep_2048:
        return coinbase::mpc::pve_base_pke_rsa().encrypt(coinbase::mpc::pve_keyref(blob.rsa_ek), label, plain, rho,
                                                         out_ct);
      case base_pke_key_type_v1::ecies_p256:
        if (blob.ecies_ek.get_curve() != coinbase::crypto::curve_p256)
          return coinbase::error(E_BADARG, "ECIES base PKE key must be on P-256");
        return coinbase::mpc::pve_base_pke_ecies().encrypt(coinbase::mpc::pve_keyref(blob.ecies_ek), label, plain, rho,
                                                           out_ct);
      default:
        return coinbase::error(E_FORMAT, "unsupported base PKE key type");
    }
  }

  error_t decrypt(mem_t dk, mem_t label, mem_t ct, buf_t& out_plain) const override {
    base_pke_dk_blob_v1_t blob;
    error_t rv = parse_dk_blob(dk, blob);
    if (rv) return rv;

    switch (static_cast<base_pke_key_type_v1>(blob.key_type)) {
      case base_pke_key_type_v1::rsa_oaep_2048:
        return coinbase::mpc::pve_base_pke_rsa().decrypt(coinbase::mpc::pve_keyref(blob.rsa_dk), label, ct, out_plain);
      case base_pke_key_type_v1::ecies_p256:
        if (blob.ecies_dk.get_curve() != coinbase::crypto::curve_p256)
          return coinbase::error(E_BADARG, "ECIES base PKE key must be on P-256");
        return coinbase::mpc::pve_base_pke_ecies().decrypt(coinbase::mpc::pve_keyref(blob.ecies_dk), label, ct,
                                                           out_plain);
      default:
        return coinbase::error(E_FORMAT, "unsupported base PKE key type");
    }
  }
};

}  // namespace

const base_pke_i& base_pke_default() {
  static const unified_key_blob_base_pke_t pke;
  return pke;
}

error_t generate_base_pke_rsa_keypair(buf_t& out_ek, buf_t& out_dk) {
  coinbase::crypto::rsa_prv_key_t sk;
  sk.generate(coinbase::crypto::RSA_KEY_LENGTH);
  coinbase::crypto::rsa_pub_key_t pk = sk.pub();

  base_pke_ek_blob_v1_t ek_blob;
  ek_blob.key_type = static_cast<uint32_t>(base_pke_key_type_v1::rsa_oaep_2048);
  ek_blob.rsa_ek = pk;

  base_pke_dk_blob_v1_t dk_blob;
  dk_blob.key_type = static_cast<uint32_t>(base_pke_key_type_v1::rsa_oaep_2048);
  dk_blob.rsa_dk = std::move(sk);

  out_ek = coinbase::convert(ek_blob);
  out_dk = coinbase::convert(dk_blob);
  return SUCCESS;
}

error_t generate_base_pke_ecies_p256_keypair(buf_t& out_ek, buf_t& out_dk) {
  coinbase::crypto::ecc_prv_key_t sk;
  sk.generate(coinbase::crypto::curve_p256);
  coinbase::crypto::ecc_pub_key_t pk = sk.pub();

  base_pke_ek_blob_v1_t ek_blob;
  ek_blob.key_type = static_cast<uint32_t>(base_pke_key_type_v1::ecies_p256);
  ek_blob.ecies_ek = pk;

  base_pke_dk_blob_v1_t dk_blob;
  dk_blob.key_type = static_cast<uint32_t>(base_pke_key_type_v1::ecies_p256);
  dk_blob.ecies_dk = std::move(sk);

  out_ek = coinbase::convert(ek_blob);
  out_dk = coinbase::convert(dk_blob);
  return SUCCESS;
}

error_t base_pke_ecies_p256_ek_from_oct(mem_t pub_key_oct, buf_t& out_ek) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(pub_key_oct, "pub_key_oct",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  error_t rv = UNINITIALIZED_ERROR;

  coinbase::crypto::ecc_pub_key_t pk;
  if (rv = pk.from_oct(coinbase::crypto::curve_p256, pub_key_oct)) return rv;
  if (rv = coinbase::crypto::curve_p256.check(pk)) return rv;

  base_pke_ek_blob_v1_t ek_blob;
  ek_blob.key_type = static_cast<uint32_t>(base_pke_key_type_v1::ecies_p256);
  ek_blob.ecies_ek = std::move(pk);

  out_ek = coinbase::convert(ek_blob);
  return SUCCESS;
}

error_t base_pke_rsa_ek_from_modulus(mem_t modulus, buf_t& out_ek) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(modulus, "modulus",
                                                                          coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;

  constexpr int kExpectedModulusBytes = coinbase::crypto::RSA_KEY_LENGTH / 8;
  if (modulus.size != kExpectedModulusBytes)
    return coinbase::error(E_BADARG, "modulus must be exactly 256 bytes (RSA-2048)");

  constexpr unsigned long kDefaultExponent = 65537;

  BIGNUM* n_bn = BN_bin2bn(modulus.data, modulus.size, nullptr);
  if (!n_bn) return coinbase::error(E_GENERAL, "BN_bin2bn(modulus) failed");

  if (BN_is_zero(n_bn)) {
    BN_free(n_bn);
    return coinbase::error(E_BADARG, "modulus must not be zero");
  }

  BIGNUM* e_bn = BN_new();
  if (!e_bn) {
    BN_free(n_bn);
    return coinbase::error(E_GENERAL, "BN_new(e) failed");
  }
  BN_set_word(e_bn, kDefaultExponent);

  coinbase::crypto::rsa_pub_key_t pk;
  pk.set(n_bn, e_bn);
  BN_free(n_bn);
  BN_free(e_bn);

  base_pke_ek_blob_v1_t ek_blob;
  ek_blob.key_type = static_cast<uint32_t>(base_pke_key_type_v1::rsa_oaep_2048);
  ek_blob.rsa_ek = std::move(pk);

  out_ek = coinbase::convert(ek_blob);
  return SUCCESS;
}

error_t encrypt(const base_pke_i& base_pke, curve_id curve, mem_t ek, mem_t label, mem_t x, buf_t& out_ciphertext) {
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(ek, "ek", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(x, "x")) return rv;
  error_t rv = UNINITIALIZED_ERROR;

  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  // Defensive check: avoid large attacker-controlled allocations when converting `x` into a bignum.
  const int max_x_size = icurve.order().get_bin_size();
  if (x.size > max_x_size) return coinbase::error(E_RANGE, "x too large", /*to_print_stack_trace=*/false);

  base_pke_bridge_t bridge(base_pke);
  coinbase::mpc::ec_pve_t pve_ct;

  const coinbase::mem_t ek_mem(ek.data, ek.size);
  const coinbase::crypto::bn_t x_bn = coinbase::crypto::bn_t::from_bin(x);

  out_ciphertext.free();
  rv = pve_ct.encrypt(bridge, coinbase::mpc::pve_keyref(ek_mem), label, icurve, x_bn);
  if (rv) return rv;

  pve_ciphertext_blob_v1_t blob;
  blob.ct = coinbase::convert(pve_ct);
  out_ciphertext = coinbase::convert(blob);
  return SUCCESS;
}

error_t encrypt(curve_id curve, mem_t ek, mem_t label, mem_t x, buf_t& out_ciphertext) {
  return encrypt(base_pke_default(), curve, ek, label, x, out_ciphertext);
}

error_t verify(const base_pke_i& base_pke, curve_id curve, mem_t ek, mem_t ciphertext, mem_t Q_compressed,
               mem_t label) {
  if (const error_t rv =
          coinbase::api::detail::validate_mem_arg_max_size(ek, "ek", coinbase::api::detail::MAX_OPAQUE_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(Q_compressed, "Q_compressed")) return rv;
  if (const error_t rv = coinbase::api::detail::validate_mem_arg(label, "label")) return rv;
  error_t rv = UNINITIALIZED_ERROR;

  const auto icurve = to_internal_curve(curve);
  if (!icurve.valid()) return coinbase::error(E_BADARG, "unsupported curve");

  pve_ciphertext_blob_v1_t blob;
  if (rv = coinbase::convert(blob, ciphertext)) return rv;
  if (blob.version != pve_ciphertext_version_v1) return coinbase::error(E_FORMAT, "unsupported ciphertext version");

  base_pke_bridge_t bridge(base_pke);
  coinbase::mpc::ec_pve_t pve_ct;
  if (rv = coinbase::convert(pve_ct, blob.ct)) return rv;

  if (pve_ct.get_Q().get_curve() != icurve) return coinbase::error(E_BADARG, "ciphertext curve mismatch");

  coinbase::crypto::ecc_point_t Q_expected;
  if (rv = Q_expected.from_bin(icurve, Q_compressed)) return coinbase::error(rv, "invalid Q");
  if (rv = icurve.check(Q_expected)) return coinbase::error(rv, "invalid Q");

  const coinbase::mem_t ek_mem(ek.data, ek.size);
  return pve_ct.verify(bridge, coinbase::mpc::pve_keyref(ek_mem), Q_expected, label);
}

error_t verify(curve_id curve, mem_t ek, mem_t ciphertext, mem_t Q_compressed, mem_t label) {
  return verify(base_pke_default(), curve, ek, ciphertext, Q_compressed, label);
}

error_t decrypt(const base_pke_i& base_pke, curve_id curve, mem_t dk, mem_t ek, mem_t ciphertext, mem_t label,
                buf_t& out_x) {
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

  pve_ciphertext_blob_v1_t blob;
  if (rv = coinbase::convert(blob, ciphertext)) return rv;
  if (blob.version != pve_ciphertext_version_v1) return coinbase::error(E_FORMAT, "unsupported ciphertext version");

  base_pke_bridge_t bridge(base_pke);
  coinbase::mpc::ec_pve_t pve_ct;
  if (rv = coinbase::convert(pve_ct, blob.ct)) return rv;

  if (pve_ct.get_Q().get_curve() != icurve) return coinbase::error(E_BADARG, "ciphertext curve mismatch");

  const coinbase::mem_t dk_mem(dk.data, dk.size);
  const coinbase::mem_t ek_mem(ek.data, ek.size);

  coinbase::crypto::bn_t x_bn;
  rv = pve_ct.decrypt(bridge, coinbase::mpc::pve_keyref(dk_mem), coinbase::mpc::pve_keyref(ek_mem), label, icurve, x_bn,
                      /*skip_verify=*/true);
  if (rv) {
    out_x.free();
    return rv;
  }

  out_x = x_bn.to_bin(icurve.order().get_bin_size());
  return SUCCESS;
}

error_t decrypt(curve_id curve, mem_t dk, mem_t ek, mem_t ciphertext, mem_t label, buf_t& out_x) {
  return decrypt(base_pke_default(), curve, dk, ek, ciphertext, label, out_x);
}

error_t decrypt_rsa_oaep_hsm(curve_id curve, mem_t dk_handle, mem_t ek, mem_t ciphertext, mem_t label,
                             const rsa_oaep_hsm_decap_cb_t& cb, buf_t& out_x) {
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
  return decrypt(base_pke, curve, dk_handle, ek, ciphertext, label, out_x);
}

error_t decrypt_ecies_p256_hsm(curve_id curve, mem_t dk_handle, mem_t ek, mem_t ciphertext, mem_t label,
                               const ecies_p256_hsm_ecdh_cb_t& cb, buf_t& out_x) {
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
  return decrypt(base_pke, curve, dk_handle, ek, ciphertext, label, out_x);
}

error_t get_public_key_compressed(mem_t ciphertext, buf_t& out_Q_compressed) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  error_t rv = UNINITIALIZED_ERROR;

  pve_ciphertext_blob_v1_t blob;
  if (rv = coinbase::convert(blob, ciphertext)) return rv;
  if (blob.version != pve_ciphertext_version_v1) return coinbase::error(E_FORMAT, "unsupported ciphertext version");

  coinbase::mpc::ec_pve_t pve_ct;  // base PKE not used for extraction
  if (rv = coinbase::convert(pve_ct, blob.ct)) return rv;

  out_Q_compressed = pve_ct.get_Q().to_compressed_bin();
  return SUCCESS;
}

error_t get_Label(mem_t ciphertext, buf_t& out_label) {
  if (const error_t rv = coinbase::api::detail::validate_mem_arg_max_size(
          ciphertext, "ciphertext", coinbase::api::detail::MAX_CIPHERTEXT_BLOB_SIZE))
    return rv;
  error_t rv = UNINITIALIZED_ERROR;

  pve_ciphertext_blob_v1_t blob;
  if (rv = coinbase::convert(blob, ciphertext)) return rv;
  if (blob.version != pve_ciphertext_version_v1) return coinbase::error(E_FORMAT, "unsupported ciphertext version");

  coinbase::mpc::ec_pve_t pve_ct;  // base PKE not used for extraction
  if (rv = coinbase::convert(pve_ct, blob.ct)) return rv;

  out_label = pve_ct.get_Label();
  return SUCCESS;
}

}  // namespace coinbase::api::pve
