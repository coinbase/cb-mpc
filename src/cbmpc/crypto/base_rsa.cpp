#include <openssl/core_names.h>

#include <cbmpc/core/log.h>
#include <cbmpc/crypto/base.h>

#include "scope.h"

namespace coinbase::crypto {

// --------------------- tools -------------

enum {
  part_e = 1 << 0,
  part_n = 1 << 1,
  part_d = 1 << 2,
  part_p = 1 << 3,
  part_q = 1 << 4,
  part_dp = 1 << 5,
  part_dq = 1 << 6,
  part_qinv = 1 << 7,
};

static void_ptr *find_ptr(void_ptr buffer, void_ptr pointer) {
  byte_ptr buf = byte_ptr(buffer);
  for (;;) {
    void_ptr *b = (void_ptr *)buf++;
    if (pointer == *b) return b;
  }
  return nullptr;
}

static buf_t prepend_oid(hash_e hash_alg, mem_t data) {
  mem_t oid = hash_alg_t::get(hash_alg).oid;
  buf_t out(oid.size + data.size);
  memmove(out.data(), oid.data, oid.size);
  memmove(out.data() + oid.size, data.data, data.size);
  return out;
}

// ------------------------------ rsa_pub_key_t -------------------------

error_t rsa_pub_key_t::encrypt_raw(mem_t in, buf_t &out) const {
  int n_size = size();
  if (n_size != in.size) return coinbase::error(E_CRYPTO);

  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new(ptr, NULL);
  if (EVP_PKEY_encrypt_init(ctx) <= 0) return openssl_error("RSA encrypt RAW error");
  if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_NO_PADDING) <= 0) return openssl_error("RSA encrypt RAW error");
  size_t outlen = n_size;
  if (EVP_PKEY_encrypt(ctx, out.alloc(n_size), &outlen, in.data, in.size) <= 0)
    return openssl_error("RSA encrypt RAW error");
  return SUCCESS;
}

error_t rsa_pub_key_t::verify_pkcs1(mem_t in, hash_e hash_alg, mem_t signature) const {
  buf_t buf;
  int n_size = size();
  if (n_size != signature.size) return coinbase::error(E_CRYPTO);

  cb_assert(hash_alg != hash_e::none);
  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new(ptr, NULL);
  if (EVP_PKEY_verify_init(ctx) <= 0) return openssl_error("RSA verify error");
  if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) return openssl_error("RSA verify error");
  if (EVP_PKEY_CTX_set_signature_md(ctx, hash_alg_t::get(hash_alg).md) <= 0) return openssl_error("RSA verify error");
  if (EVP_PKEY_verify(ctx, signature.data, signature.size, in.data, in.size) != 1)
    return openssl_error("RSA verify error");
  return SUCCESS;
}

void rsa_pub_key_t::create() { free(); }

int rsa_pub_key_t::size() const {
  if (!ptr) return 0;
  return EVP_PKEY_get_size(ptr);
}

void rsa_pub_key_t::set(RSA_BASE *&rsa, const BIGNUM *n, const BIGNUM *e) {
  cb_assert(n && e);
  OSSL_PARAM_BLD *param_bld = OSSL_PARAM_BLD_new();
  OSSL_PARAM_BLD_push_BN(param_bld, "n", n);
  OSSL_PARAM_BLD_push_BN(param_bld, "e", e);
  OSSL_PARAM *params = OSSL_PARAM_BLD_to_param(param_bld);

  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new_from_name(NULL, "RSA", NULL);
  cb_assert(EVP_PKEY_fromdata_init(ctx) > 0);
  cb_assert(EVP_PKEY_fromdata(ctx, &rsa, EVP_PKEY_PUBLIC_KEY, params) > 0);

  OSSL_PARAM_free(params);
  OSSL_PARAM_BLD_free(param_bld);
}

rsa_pub_key_t::data_t rsa_pub_key_t::get(const EVP_PKEY *pkey) {
  data_t data;
  data.n = NULL;
  data.e = NULL;

  if (EVP_PKEY_id(pkey) != EVP_PKEY_RSA) {
    // Handle error: Not an RSA key
    return data;
  }

  if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_N, &data.n) <= 0) {
    // Handle error
    return data;
  }

  if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_RSA_E, &data.e) <= 0) {
    // Handle error
    BN_free(data.n);
    return data;
  }

  return data;
}

void rsa_pub_key_t::convert(coinbase::converter_t &converter) {
  uint8_t parts = 0;
  bn_t e, n;

  if (converter.is_write()) {
    data_t data = get();

    if (data.e) {
      parts |= part_e;
      e = bn_t(data.e);
    }
    if (data.n) {
      parts |= part_n;
      n = bn_t(data.n);
    }
  }

  converter.convert(parts);

  if (converter.is_error()) return;
  if (parts & part_e) converter.convert(e);
  if (parts & part_n) converter.convert(n);

  if (!converter.is_write() && !converter.is_error()) {
    create();
    switch (parts) {
      case 0:
        break;
      case part_e | part_n:
        set(n, e);
        break;
      default:
        converter.set_error();
        free();
        return;
    }
  }
}

// ------------------------------ rsa_prv_key_t -------------------------

error_t rsa_prv_key_t::execute(mem_t enc_info, buf_t &dec_info) const {
  return rsa_oaep_t(*this).execute(hash_e::sha256, hash_e::sha256, mem_t(), enc_info, dec_info);
}

error_t rsa_prv_key_t::sign_pkcs1(mem_t in, hash_e hash_alg, buf_t &signature) const {
  buf_t buf;

  unsigned int signature_size = size();
  signature.alloc(signature_size);

  cb_assert(hash_alg != hash_e::none);
  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new(ptr, NULL);
  if (EVP_PKEY_sign_init(ctx) <= 0) return openssl_error("RSA sign error");
  if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) return openssl_error("RSA sign error");
  if (EVP_PKEY_CTX_set_signature_md(ctx, hash_alg_t::get(hash_alg).md) <= 0) return openssl_error("RSA sign error");
  size_t sig_len = signature_size;
  if (EVP_PKEY_sign(ctx, signature.data(), &sig_len, in.data, in.size) <= 0) return openssl_error("RSA sign error");
  return SUCCESS;
}

error_t rsa_prv_key_t::decrypt_raw(mem_t in, buf_t &out) const {
  int n_size = size();
  if (in.size != n_size) return coinbase::error(E_CRYPTO);

  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new(ptr, NULL);
  if (EVP_PKEY_decrypt_init(ctx) <= 0) return openssl_error("RSA decrypt RAW error");
  if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_NO_PADDING) <= 0) return openssl_error("RSA decrypt RAW error");
  size_t outlen = n_size;
  if (EVP_PKEY_decrypt(ctx, out.alloc(n_size), &outlen, in.data, in.size) <= 0)
    return openssl_error("RSA decrypt RAW error");
  return SUCCESS;
}

void rsa_prv_key_t::create() { free(); }

void rsa_prv_key_t::generate(int bits, const bn_t &e) {
  create();
  ptr = EVP_RSA_gen(bits);
}

void rsa_prv_key_t::generate(int bits, int e) {
  if (e == 0) e = 65537;
  bn_t pub_exp(e);
  generate(bits, pub_exp);
}

int rsa_prv_key_t::size() const {
  if (!ptr) return 0;
  return EVP_PKEY_get_size(ptr);
}

rsa_prv_key_t::data_t rsa_prv_key_t::get(const RSA_BASE *rsa) {
  data_t data;

  OSSL_PARAM *params = NULL;
  cb_assert(EVP_PKEY_todata(rsa, EVP_PKEY_PUBLIC_KEY, &params));
  const OSSL_PARAM *param_e = OSSL_PARAM_locate_const(params, "e");
  cb_assert(param_e);
  BIGNUM *e_ptr = data.e;
  const OSSL_PARAM *param_n = OSSL_PARAM_locate_const(params, "n");
  cb_assert(param_n);
  BIGNUM *n_ptr = data.n;
  cb_assert(OSSL_PARAM_get_BN(param_e, &e_ptr) > 0);
  cb_assert(OSSL_PARAM_get_BN(param_n, &n_ptr) > 0);
  OSSL_PARAM_free(params);

  params = NULL;
  cb_assert(EVP_PKEY_todata(rsa, EVP_PKEY_PRIVATE_KEY, &params));
  const OSSL_PARAM *param_p = OSSL_PARAM_locate_const(params, "rsa-factor1");
  cb_assert(param_p);
  BIGNUM *p_ptr = data.p;
  const OSSL_PARAM *param_q = OSSL_PARAM_locate_const(params, "rsa-factor2");
  cb_assert(param_q);
  BIGNUM *q_ptr = data.q;
  cb_assert(OSSL_PARAM_get_BN(param_p, &p_ptr) > 0);
  cb_assert(OSSL_PARAM_get_BN(param_q, &q_ptr) > 0);
  OSSL_PARAM_free(params);

  return data;
}

rsa_pub_key_t rsa_prv_key_t::pub() const {
  rsa_pub_key_t pub_key;
  pub_key.set(get_n(), get_e());
  return pub_key;
}

error_t rsa_oaep_t::execute(hash_e hash_alg, hash_e mgf_alg, mem_t label, mem_t in, buf_t &out) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (!hash_alg_t::get(hash_alg).valid()) return coinbase::error(E_BADARG);
  if (!hash_alg_t::get(mgf_alg).valid()) return coinbase::error(E_BADARG);

  if (key) {
    if (rv = key->decrypt_oaep(in, hash_alg, mgf_alg, label, out)) return rv;
    return SUCCESS;
  }

  cmem_t cmem;
  if (rv = exec(ctx, int(hash_alg), int(mgf_alg), cmem_t(label), cmem_t(in), &cmem)) return rv;

  out = buf_t::from_cmem(cmem);
  return SUCCESS;
}

error_t rsa_oaep_t::execute(void *ctx, int hash_alg, int mgf_alg, cmem_t label, cmem_t in, cmem_t *out) {
  error_t rv = UNINITIALIZED_ERROR;
  if (!hash_alg_t::get(hash_e(hash_alg)).valid()) return coinbase::error(E_BADARG);
  if (!hash_alg_t::get(hash_e(mgf_alg)).valid()) return coinbase::error(E_BADARG);

  buf_t buf;
  const rsa_prv_key_t *key = (const rsa_prv_key_t *)ctx;
  if (rv = key->decrypt_oaep(mem_t(in), hash_e(hash_alg), hash_e(mgf_alg), mem_t(label), buf)) return rv;

  *out = buf.to_cmem();
  return SUCCESS;
}

void rsa_kem_ciphertext_t::convert(coinbase::converter_t &converter) {
  converter.convert(rsa_enc);
  converter.convert(aes_enc);
}

error_t rsa_kem_ciphertext_t::encrypt(const rsa_pub_key_t &pub_key, mem_t label, mem_t plain, drbg_aes_ctr_t *drbg) {
  return encrypt(pub_key, hash_e::sha256, hash_e::sha256, label, plain, drbg);
}

error_t rsa_kem_ciphertext_t::encrypt(const rsa_pub_key_t &pub_key, hash_e hash_alg, hash_e mgf_alg, mem_t label,
                                      mem_t plain, drbg_aes_ctr_t *drbg) {
  aes_enc = buf_t();
  rsa_enc = buf_t();
  bool rsa_hybrid = true;

  buf_t bin;
  if (rsa_hybrid) {
    buf_t k, iv;
    if (drbg) {
      k = drbg->gen(32);
      iv = drbg->gen(12);
    } else {
      k = crypto::gen_random(32);
      iv = crypto::gen_random(12);
    }
    crypto::aes_gcm_t::encrypt(k, iv, label, 12, plain, aes_enc);

    bin = k + iv;
  } else {
    cb_assert(plain.size + 32 + 32 <= pub_key.size());
    bin = crypto::sha256_t::hash(label) + plain;
  }

  if (drbg) {
    buf_t seed = drbg->gen_bitlen(256);
    return pub_key.encrypt_oaep_with_seed(bin, hash_alg, mgf_alg, mem_t(), seed, rsa_enc);
  } else {
    return pub_key.encrypt_oaep(bin, hash_alg, mgf_alg, mem_t(), rsa_enc);
  }
}

error_t rsa_kem_ciphertext_t::decrypt(const rsa_oaep_t &oaep, mem_t label, buf_t &out) {
  return decrypt(oaep, crypto::hash_e::sha256, crypto::hash_e::sha256, label, out);
}

error_t rsa_kem_ciphertext_t::decrypt(const rsa_oaep_t &oaep, hash_e hash_alg, hash_e mgf_alg, mem_t label,
                                      buf_t &out) {
  error_t rv = UNINITIALIZED_ERROR;
  buf_t dec_info;
  if (rv = oaep.execute(crypto::hash_e::sha256, crypto::hash_e::sha256, mem_t(), rsa_enc, dec_info)) return rv;
  if (rv = decrypt_end(label, dec_info, out)) return rv;
  return SUCCESS;
}

error_t rsa_kem_ciphertext_t::decrypt_begin(buf_t &enc_info) const {
  enc_info = rsa_enc;
  return SUCCESS;
}

error_t rsa_kem_ciphertext_t::decrypt_end(mem_t label, mem_t dec_info, buf_t &out) const {
  error_t rv = UNINITIALIZED_ERROR;
  bool rsa_hybrid = !aes_enc.empty();
  if (rsa_hybrid) {
    if (dec_info.size != 32 + 12) return coinbase::error(E_CRYPTO);
    mem_t k = dec_info.take(32);
    mem_t iv = dec_info.skip(32);
    if (rv = crypto::aes_gcm_t::decrypt(k, iv, label, 12, aes_enc, out)) return rv;
  } else {
    if (dec_info.size < 32) return coinbase::error(E_CRYPTO);
    buf_t h = crypto::sha256_t::hash(label);
    if (h != dec_info.take(32)) return coinbase::error(E_CRYPTO);
    out = dec_info.skip(32);
  }
  return SUCCESS;
}

}  // namespace coinbase::crypto
