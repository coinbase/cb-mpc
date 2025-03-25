#include "tdh2.h"

#include <cbmpc/crypto/secret_sharing.h>

namespace coinbase::crypto::tdh2 {

constexpr int aes_bits = 256;
constexpr int tag_size = 16;

ciphertext_t public_key_t::encrypt(mem_t plain, mem_t label) const {
  const auto& curve = Q.get_curve();
  const mod_t& q = curve.order();

  buf_t iv = gen_random(iv_size);
  bn_t r = bn_t::rand(q);
  bn_t s = bn_t::rand(q);
  return encrypt(plain, label, r, s, iv);
}

ciphertext_t public_key_t::encrypt(mem_t plain, mem_t label, const bn_t& r, const bn_t& s, mem_t iv) const {
  ciphertext_t ciphertext;
  const auto& G = Q.get_curve().generator();
  const mod_t& q = Q.get_curve().order();

  ecc_point_t P = r * Q;
  buf_t key;

  key = ro::hash_string(P).bitlen(aes_bits);
  ciphertext.iv = iv;
  aes_gcm_t::encrypt(key, ciphertext.iv, label, tag_size, plain, ciphertext.c);

  ciphertext.R1 = r * G;
  ecc_point_t W1 = s * G;
  ciphertext.R2 = r * Gamma;
  ecc_point_t W2 = s * Gamma;

  ciphertext.e = ro::hash_number(ciphertext.c, label, ciphertext.R1, W1, ciphertext.R2, W2, iv).mod(q);
  MODULO(q) ciphertext.f = s + r * ciphertext.e;

  ciphertext.L = label;

  return ciphertext;
}

error_t ciphertext_t::verify(const public_key_t& pub_key, mem_t label) const {
  error_t rv = UNINITIALIZED_ERROR;
  const ecc_point_t& Gamma = pub_key.Gamma;
  const ecc_point_t& Q = pub_key.Q;
  const auto& curve = Gamma.get_curve();
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  if (label != L) return coinbase::error(E_CRYPTO, "ciphertext_t::verify: label mismatch");
  if (rv = curve.check(R1)) return coinbase::error(rv, "ciphertext_t::verify: check R1 failed");
  if (rv = curve.check(R2)) return coinbase::error(rv, "ciphertext_t::verify: check R2 failed");

  if (Gamma != ro::hash_curve(mem_t("TDH2-Gamma"), Q).curve(Q.get_curve()))
    return coinbase::error(E_CRYPTO, "ciphertext_t::verify: Gamma mismatch");

  ecc_point_t W1 = f * G - e * R1;
  ecc_point_t W2 = f * Gamma - e * R2;

  bn_t e_test = ro::hash_number(c, label, R1, W1, R2, W2, iv).mod(q);
  if (e_test != e) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

error_t private_share_t::decrypt(const ciphertext_t& ciphertext, mem_t label,
                                 partial_decryption_t& partial_decryption) const {
  error_t rv = UNINITIALIZED_ERROR;
  const auto& curve = pub_key.Q.get_curve();
  if (rv = ciphertext.verify(pub_key, label)) return rv;
  const ecc_point_t& R1 = ciphertext.R1;

  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  ecc_point_t& Xi = partial_decryption.Xi;
  bn_t& ei = partial_decryption.ei;
  bn_t& fi = partial_decryption.fi;

  partial_decryption.pid = pid;
  Xi = x * R1;

  bn_t si = curve.get_random_value();
  ecc_point_t Yi = si * R1;
  ecc_point_t Zi = si * G;

  ei = ro::hash_number(Xi, Yi, Zi).mod(q);
  MODULO(q) fi = si + x * ei;
  return SUCCESS;
}

error_t ciphertext_t::decrypt(const ecc_point_t& V, buf_t& dec, mem_t label) const {
  error_t rv = UNINITIALIZED_ERROR;
  buf_t key;
  ecurve_t curve = V.get_curve();

  key = ro::hash_string(V).bitlen(aes_bits);
  if (rv = aes_gcm_t::decrypt(key, iv, label, tag_size, c, dec)) return rv;
  return SUCCESS;
}

error_t partial_decryption_t::check_partial_decryption_helper(const ecc_point_t& Qi, const ciphertext_t& ciphertext,
                                                              ecurve_t curve) const {
  error_t rv = UNINITIALIZED_ERROR;

  if (rv = curve.check(Qi))
    return coinbase::error(rv, "partial_decryption_t::check_partial_decryption_helper: check Qi failed");
  if (rv = curve.check(Xi))
    return coinbase::error(rv, "partial_decryption_t::check_partial_decryption_helper: check Xi failed");

  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  const ecc_point_t& R1 = ciphertext.R1;
  ecc_point_t Yi = fi * R1 - ei * Xi;
  ecc_point_t Zi = fi * G - ei * Qi;

  bn_t ei_test = ro::hash_number(Xi, Yi, Zi).mod(q);
  if (ei != ei_test) return coinbase::error(E_CRYPTO);

  return SUCCESS;
}

error_t combine_additive(const public_key_t& pub_key, const pub_shares_t& Qi, mem_t label,
                         const partial_decryptions_t& partial_decryptions, const ciphertext_t& ciphertext,
                         buf_t& plain) {
  error_t rv = UNINITIALIZED_ERROR;
  const auto& curve = pub_key.Q.get_curve();
  int n = int(Qi.size());
  for (const auto& _Qi : Qi) {
    if (rv = curve.check(_Qi)) return coinbase::error(rv, "combine_additive: check Qi failed");
  }
  if ((int)partial_decryptions.size() != n) return coinbase::error(E_CRYPTO);

  if (rv = ciphertext.verify(pub_key, label)) return rv;

  ecc_point_t V = curve.infinity();
  for (int i = 0; i < n; i++) {
    const partial_decryption_t& partial_decryption = partial_decryptions[i];

    int pid = partial_decryption.pid;
    if (pid < 1 || pid > n) return coinbase::error(E_CRYPTO);
    if (rv = partial_decryption.check_partial_decryption_helper(Qi[pid - 1], ciphertext, curve)) return rv;

    V += partial_decryption.Xi;
  }

  if (rv = ciphertext.decrypt(V, plain, label)) return rv;
  return SUCCESS;
}

error_t combine(const ss::ac_t& ac, const public_key_t& pub_key, ss::ac_pub_shares_t& pub_shares, mem_t label,
                const ss::party_map_t<partial_decryption_t> partial_decryptions, const ciphertext_t& ciphertext,
                buf_t& plain) {
  error_t rv = UNINITIALIZED_ERROR;

  if (!ac.enough_for_quorum(partial_decryptions)) return coinbase::error(E_CRYPTO);

  if (rv = ciphertext.verify(pub_key, label)) return rv;

  ss::ac_pub_shares_t Vs;
  for (const auto& [name, partial_decryption] : partial_decryptions) {
    if (rv = partial_decryption.check_partial_decryption_helper(pub_shares[name], ciphertext, pub_key.Q.get_curve()))
      return rv;
    if (rv = pub_key.Q.get_curve().check(partial_decryption.Xi)) return rv;

    Vs[name] = partial_decryption.Xi;
  }

  ecc_point_t V;
  if (rv = ac.reconstruct_exponent(Vs, V)) return rv;

  if (rv = ciphertext.decrypt(V, plain, label)) return rv;
  return SUCCESS;
}

}  // namespace coinbase::crypto::tdh2
