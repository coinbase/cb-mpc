#include "base.h"

/*
 * Written by Ulf Moeller. This software is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied.
 */

/* EME-OAEP as defined in RFC 2437 (PKCS #1 v2.0) */

/*
 * See Victor Shoup, "OAEP reconsidered," Nov. 2000, <URL:
 * http://www.shoup.net/papers/oaep.ps.Z> for problems with the security
 * proof for the original OAEP scheme, which EME-OAEP is based on. A new
 * proof can be found in E. Fujisaki, T. Okamoto, D. Pointcheval, J. Stern,
 * "RSA-OEAP is Still Alive!", Dec. 2000, <URL:
 * http://eprint.iacr.org/2000/061/>. The new proof has stronger requirements
 * for the underlying permutation: "partial-one-wayness" instead of
 * one-wayness.  For the RSA function, this is an equivalent notion.
 */

namespace coinbase::crypto {

static int mgf1_xor(unsigned char *out, size_t outlen, const unsigned char *seed, size_t seedlen, const EVP_MD *md,
                    OSSL_LIB_CTX *libctx, const char *propq) {
  unsigned char dig[EVP_MAX_MD_SIZE];
  unsigned int counter = 0;
  size_t done = 0;
  unsigned int mdsize = 0;
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  if (ctx == NULL) return -1;

  mdsize = EVP_MD_get_size(md);
  if (mdsize == 0 || mdsize > EVP_MAX_MD_SIZE) {
    EVP_MD_CTX_free(ctx);
    return -1;
  }

  while (done < outlen) {
    unsigned char c[4];
    c[0] = (unsigned char)((counter >> 24) & 0xFF);
    c[1] = (unsigned char)((counter >> 16) & 0xFF);
    c[2] = (unsigned char)((counter >> 8) & 0xFF);
    c[3] = (unsigned char)(counter & 0xFF);

    if (!EVP_DigestInit_ex(ctx, md, NULL)) goto err;
    if (!EVP_DigestUpdate(ctx, seed, seedlen)) goto err;
    if (!EVP_DigestUpdate(ctx, c, sizeof(c))) goto err;
    if (!EVP_DigestFinal_ex(ctx, dig, NULL)) goto err;

    size_t to_copy = (done + mdsize > outlen) ? (outlen - done) : mdsize;
    for (size_t i = 0; i < to_copy; i++) {
      out[done + i] ^= dig[i];
    }

    done += to_copy;
    counter++;
  }

  EVP_MD_CTX_free(ctx);
  return 0;

err:
  EVP_MD_CTX_free(ctx);
  return -1;
}

/*
 * Perform the padding as per NIST 800-56B 7.2.2.3
 *      from (K) is the key material.
 *      param (A) is the additional input.
 * Step numbers are included here but not in the constant time inverse below
 * to avoid complicating an already difficult enough function.
 */
// NOLINTBEGIN
static int ossl_rsa_padding_add_PKCS1_OAEP_mgf1_ex(OSSL_LIB_CTX *libctx, unsigned char *to, int tlen,
                                                   const unsigned char *from, int flen, const unsigned char *param,
                                                   int plen, const EVP_MD *md, const EVP_MD *mgf1md,
                                                   const unsigned char *seed_data, int seedlen) {
  int rv = 0;
  int emlen = tlen - 1;
  unsigned char *db, *seed;
  int mdlen, dbmask_len = 0;

  if (md == NULL) {
#ifndef FIPS_MODULE
    md = EVP_sha1();
#else
    ERR_raise(ERR_LIB_RSA, ERR_R_PASSED_NULL_PARAMETER);
    return 0;
#endif
  }
  if (mgf1md == NULL) mgf1md = md;

  mdlen = EVP_MD_get_size(md);
  if (mdlen <= 0) {
    ERR_raise(ERR_LIB_RSA, RSA_R_INVALID_LENGTH);
    return 0;
  }
  cb_assert(mdlen == seedlen);

  /* step 2b: check KLen > nLen - 2 HLen - 2 */
  if (flen > emlen - 2 * mdlen - 1) {
    ERR_raise(ERR_LIB_RSA, RSA_R_DATA_TOO_LARGE_FOR_KEY_SIZE);
    return 0;
  }

  if (emlen < 2 * mdlen + 1) {
    ERR_raise(ERR_LIB_RSA, RSA_R_KEY_SIZE_TOO_SMALL);
    return 0;
  }

  /* step 3i: EM = 00000000 || maskedMGF || maskedDB */
  to[0] = 0;
  seed = to + 1;
  db = to + mdlen + 1;

  /* step 3a: hash the additional input */
  if (!EVP_Digest((void *)param, plen, db, NULL, md, NULL)) goto err;
  /* step 3b: zero bytes array of length nLen - KLen - 2 HLen -2 */
  memset(db + mdlen, 0, emlen - flen - 2 * mdlen - 1);
  /* step 3c: DB = HA || PS || 00000001 || K */
  db[emlen - flen - mdlen - 1] = 0x01;
  memcpy(db + emlen - flen - mdlen, from, (unsigned int)flen);
  /* step 3d: copy random byte string */
  memmove(seed, seed_data, mdlen);

  dbmask_len = emlen - mdlen;

  if (mgf1_xor(db, dbmask_len, seed, mdlen, mgf1md, libctx, NULL) < 0) goto err;

  if (mgf1_xor(seed, mdlen, db, dbmask_len, mgf1md, libctx, NULL) < 0) goto err;

  rv = 1;

err:
  return rv;
}
// NOLINTEND

error_t rsa_pub_key_t::pad_oaep_with_seed(int bits, mem_t in, hash_e hash_alg, hash_e mgf_alg, mem_t label, mem_t seed,
                                          buf_t &out)  // static
{
  int key_size = coinbase::bits_to_bytes(bits);
  if (0 >= ossl_rsa_padding_add_PKCS1_OAEP_mgf1_ex(NULL, out.alloc(key_size), key_size, in.data, in.size, label.data,
                                                   label.size, crypto::hash_alg_t::get(hash_alg).md,
                                                   crypto::hash_alg_t::get(mgf_alg).md, seed.data, seed.size))
    return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

error_t rsa_pub_key_t::pad_oaep(int bits, mem_t in, hash_e hash_alg, hash_e mgf_alg, mem_t label, buf_t &out)  // static
{
  int seed_size = hash_alg_t::get(hash_alg).size;
  return pad_oaep_with_seed(bits, in, hash_alg, mgf_alg, label, gen_random(seed_size), out);
}

error_t rsa_prv_key_t::decrypt_oaep(mem_t in, hash_e hash_alg, hash_e mgf_alg, mem_t label, buf_t &out) const {
  int n_size = size();
  if (in.size != n_size) return coinbase::error(E_CRYPTO);

  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new(ptr, NULL);
  if (EVP_PKEY_decrypt_init(ctx) <= 0) return openssl_error("RSA decrypt OAEP error");
  if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) return openssl_error("RSA decrypt OAEP error");
  if (EVP_PKEY_CTX_set_rsa_oaep_md(ctx, hash_alg_t::get(hash_alg).md) <= 0)
    return openssl_error("RSA decrypt OAEP error");
  if (EVP_PKEY_CTX_set_rsa_oaep_md(ctx, hash_alg_t::get(mgf_alg).md) <= 0)
    return openssl_error("RSA decrypt OAEP error");
  if (EVP_PKEY_CTX_set0_rsa_oaep_label(ctx, label.data, label.size) <= 0)
    return openssl_error("RSA decrypt OAEP error");

  size_t outlen = n_size;
  if (EVP_PKEY_decrypt(ctx, out.alloc(n_size), &outlen, in.data, in.size) <= 0)
    return openssl_error("RSA decrypt OAEP error");
  out.resize(int(outlen));
  return SUCCESS;
}

error_t rsa_pub_key_t::encrypt_oaep_with_seed(mem_t in, hash_e hash_alg, hash_e mgf_alg, mem_t label, mem_t seed,
                                              buf_t &out) const {
  error_t rv = UNINITIALIZED_ERROR;
  buf_t padded;
  if (rv = pad_oaep_with_seed(size() * 8, in, hash_alg, mgf_alg, label, seed, padded)) return rv;
  return rv = encrypt_raw(padded, out);
}

error_t rsa_pub_key_t::encrypt_oaep(mem_t in, hash_e hash_alg, hash_e mgf_alg, mem_t label, buf_t &out) const {
  error_t rv = UNINITIALIZED_ERROR;
  buf_t padded;
  if (rv = pad_oaep(size() * 8, in, hash_alg, mgf_alg, label, padded)) return rv;
  return rv = encrypt_raw(padded, out);
}

}  // namespace coinbase::crypto
