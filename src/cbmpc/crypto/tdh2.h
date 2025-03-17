#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/lagrange.h>
#include <cbmpc/crypto/secret_sharing.h>

namespace coinbase::crypto::tdh2 {

typedef std::vector<ecc_point_t> pub_shares_t;

static const int iv_size = 16;

class public_key_t;
struct ciphertext_t {
  buf_t c;
  buf_t iv;
  ecc_point_t R1, R2;
  bn_t e, f;
  buf_t L;

  void convert(coinbase::converter_t& converter) { converter.convert(c, R1, R2, e, f, iv); }

  /**
   * @specs:
   * - tdh2-spec | tdh2-combine-1P
   * @notes:
   * - This is a helper function used in the last step of tdh2-combine-1P such that given `V`, it performs the aes-gcm
   * decryption
   */
  error_t decrypt(const ecc_point_t& V, buf_t& dec, mem_t label) const;

  /**
   * @specs:
   * - tdh2-spec | tdh2-verify-1P
   */
  error_t verify(const public_key_t& pub_key, mem_t label) const;
};

template <class T>
T& update_state(T& state, const ciphertext_t& v) {
  update_state(state, v.c);
  update_state(state, v.R1);
  update_state(state, v.R2);
  update_state(state, v.e);
  update_state(state, v.f);
  return state;
}

struct public_key_t {
  ecc_point_t Q, Gamma;

  public_key_t() {}
  public_key_t(const ecc_point_t& _Q) : Q(_Q) { Gamma = ro::hash_curve(mem_t("TDH2-Gamma"), Q).curve(Q.get_curve()); }

  /**
   * @specs:
   * - tdh2-spec | tdh2-encrypt-1P
   * @notes:
   * - This function generates random r, s, iv and calls encrypt(plain, label, r, s, iv, curve)
   */
  ciphertext_t encrypt(mem_t plain, mem_t label) const;

  /**
   * @specs:
   * - tdh2-spec | tdh2-encrypt-1P
   */
  ciphertext_t encrypt(mem_t plain, mem_t label, const bn_t& r, const bn_t& s, mem_t iv) const;

  bool valid() const { return Q.valid(); }
  void convert(coinbase::converter_t& converter) { converter.convert(Q, Gamma); }
  buf_t to_bin() const { return coinbase::convert(*this); }
  error_t from_bin(mem_t bin) { return coinbase::convert(*this, bin); }
  bool operator==(const public_key_t& other) const { return Q == other.Q && Gamma == other.Gamma; }
  bool operator!=(const public_key_t& other) const { return Q != other.Q || Gamma != other.Gamma; }
};

struct private_key_t {
  bn_t x;

  public_key_t pub_key;
  void convert(coinbase::converter_t& c) { c.convert(x, pub_key); }
  public_key_t pub() const { return pub_key; }
  bool valid() const { return pub_key.Q.valid(); }
};

struct partial_decryption_t {
  int pid;
  ecc_point_t Xi;
  bn_t ei, fi;

  void convert(coinbase::converter_t& converter) { converter.convert(pid, Xi, ei, fi); }

  /**
   * @specs:
   * - tdh2-spec | tdh2-combine-1P
   * @notes:
   * - This is a helper function used in tdh2-combine-1P
   */
  error_t check_partial_decryption_helper(const ecc_point_t& Qi, const ciphertext_t& ciphertext, ecurve_t curve) const;
};

struct private_share_t {
  public_key_t pub_key;
  bn_t x;
  int pid = 0;

  /**
   * @specs:
   * - tdh2-spec | tdh2-local-decrypt-1P
   */
  error_t decrypt(const ciphertext_t& ciphertext, mem_t label, partial_decryption_t& partial_decryption) const;
};

typedef std::vector<partial_decryption_t> partial_decryptions_t;

/**
 * @specs:
 * - tdh2-spec | tdh2-combine-1P
 * @notes:
 * - This is the special case where the shares are additive shares
 */
error_t combine_additive(const public_key_t& pub, const pub_shares_t& Qi, mem_t label,
                         const partial_decryptions_t& partial_decryptions, const ciphertext_t& ciphertext,
                         buf_t& plain);

/**
 * @specs:
 * - tdh2-spec | tdh2-combine-1P
 * @notes:
 * - This is the general case where the shares are general access structure
 */
error_t combine(const ss::ac_t& ac, const public_key_t& pub, ss::ac_pub_shares_t& pub_shares, mem_t label,
                const ss::party_map_t<partial_decryption_t> partial_decryptions, const ciphertext_t& ciphertext,
                buf_t& plain);

}  // namespace coinbase::crypto::tdh2
