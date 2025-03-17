#pragma once

#include "base_bn.h"
#include "scope.h"

typedef EVP_PKEY RSA_BASE;

namespace coinbase::crypto {

const int RSA_KEY_LENGTH = 2048;
class rsa_pub_key_t : public scoped_ptr_t<RSA_BASE> {
 public:
  int size() const;

  static error_t pad_oaep(int bits, mem_t in, hash_e hash_alg, hash_e mgf_alg, mem_t label, buf_t &out);
  static error_t pad_oaep_with_seed(int bits, mem_t in, hash_e hash_alg, hash_e mgf_alg, mem_t label, mem_t seed,
                                    buf_t &out);

  error_t encrypt_raw(mem_t in, buf_t &out) const;
  error_t encrypt_oaep(mem_t in, hash_e hash_alg, hash_e mgf_alg, mem_t label, buf_t &out) const;
  error_t encrypt_oaep_with_seed(mem_t in, hash_e hash_alg, hash_e mgf_alg, mem_t label, mem_t seed, buf_t &out) const;
  error_t verify_pkcs1(mem_t data, hash_e hash_alg, mem_t signature) const;

  buf_t to_der() const;
  buf_t to_der_pkcs1() const;
  error_t from_der(mem_t der);

  bn_t get_e() const { return bn_t(get().e); }
  bn_t get_n() const { return bn_t(get().n); }
  void set(const BIGNUM *n, const BIGNUM *e) {
    create();
    set(ptr, n, e);
  }

  void convert(coinbase::converter_t &converter);

  bool operator==(const rsa_pub_key_t &val) const { return EVP_PKEY_eq(ptr, val.ptr); }
  bool operator!=(const rsa_pub_key_t &val) const { return !EVP_PKEY_eq(ptr, val.ptr); }

 private:
  struct data_t {
    BIGNUM *n = nullptr, *e = nullptr;
  };

  static data_t get(const RSA_BASE *ptr);
  static void set(RSA_BASE *&rsa, const BIGNUM *n, const BIGNUM *e);

  data_t get() const { return get(ptr); }
  void create();
};

class rsa_prv_key_t : public scoped_ptr_t<RSA_BASE> {
 public:
  error_t execute(mem_t enc_info, buf_t &dec_info) const;

  rsa_pub_key_t pub() const;
  int size() const;

  void generate(int bits, int e = 65537);
  void generate(int bits, const bn_t &e);

  error_t decrypt_raw(mem_t in, buf_t &out) const;
  error_t decrypt_oaep(mem_t in, hash_e hash_alg, hash_e mgf_alg, mem_t label, buf_t &out) const;
  error_t sign_pkcs1(mem_t data, hash_e hash_alg, buf_t &sig) const;

  buf_t to_der() const;
  error_t from_der(mem_t der);

  void convert(coinbase::converter_t &converter);

  bn_t get_e() const { return bn_t(get().e); }
  bn_t get_n() const { return bn_t(get().n); }
  bn_t get_p() const { return bn_t(get().p); }
  bn_t get_q() const { return bn_t(get().q); }

  void set(const BIGNUM *n, const BIGNUM *e, const BIGNUM *d) {
    create();
    set(ptr, n, e, d);
  }
  void set(const BIGNUM *n, const BIGNUM *e, const BIGNUM *d, const BIGNUM *p, const BIGNUM *q) {
    create();
    set(ptr, n, e, d, p, q);
  }
  void set(const BIGNUM *n, const BIGNUM *e, const BIGNUM *d, const BIGNUM *p, const BIGNUM *q, const BIGNUM *dp,
           const BIGNUM *dq, const BIGNUM *qinv) {
    create();
    set(ptr, n, e, d, p, q, dp, dq, qinv);
  }
  error_t recover_factors();
  void set_paillier(const BIGNUM *n, const BIGNUM *p, const BIGNUM *q, const BIGNUM *dp, const BIGNUM *dq,
                    const BIGNUM *qinv);

 private:
  struct data_t {
    bn_t n, e;
    bn_t p, q;
  };
  static data_t get(const RSA_BASE *ptr);
  static void set(RSA_BASE *rsa, const BIGNUM *n, const BIGNUM *e, const BIGNUM *d);
  static void set(RSA_BASE *rsa, const BIGNUM *n, const BIGNUM *e, const BIGNUM *d, const BIGNUM *p, const BIGNUM *q);
  static void set(RSA_BASE *rsa, const BIGNUM *n, const BIGNUM *e, const BIGNUM *d, const BIGNUM *p, const BIGNUM *q,
                  const BIGNUM *dp, const BIGNUM *dq, const BIGNUM *qinv);
  static void set(RSA_BASE *rsa, const data_t &data);

  data_t get() const { return get(ptr); }
  void create();
};

class rsa_oaep_t {
 public:
  typedef error_t (*exec_t)(void *ctx, int hash_alg, int mgf_alg, cmem_t label, cmem_t input, cmem_t *output);

  rsa_oaep_t(const rsa_prv_key_t &_key) : key(&_key), exec(nullptr), ctx(nullptr) {}
  rsa_oaep_t(exec_t _exec, void *_ctx) : key(nullptr), exec(_exec), ctx(_ctx) {}

  error_t execute(hash_e hash_alg, hash_e mgf_alg, mem_t label, mem_t in, buf_t &out) const;
  static error_t execute(void *ctx, int hash_alg, int mgf_alg, cmem_t label, cmem_t in, cmem_t *out);

 private:
  exec_t exec;
  void *ctx;
  const rsa_prv_key_t *key;
};

struct rsa_kem_ciphertext_t {
  buf_t rsa_enc, aes_enc;
  buf_t encrypted;
  void convert(coinbase::converter_t &converter);
  buf_t to_bin() const { return coinbase::convert(*this); }

  error_t encrypt(const rsa_pub_key_t &pub_key, mem_t label, mem_t plain, drbg_aes_ctr_t *drbg = nullptr);
  error_t encrypt(const rsa_pub_key_t &pub_key, hash_e hash_alg, hash_e mgf_alg, mem_t label, mem_t plain,
                  drbg_aes_ctr_t *drbg = nullptr);
  error_t decrypt(const rsa_oaep_t &prv_key, mem_t label, buf_t &out);
  error_t decrypt(const rsa_oaep_t &prv_key, hash_e hash_alg, hash_e mgf_alg, mem_t label, buf_t &out);
  error_t decrypt_begin(buf_t &enc_info) const;
  error_t decrypt_end(mem_t label, mem_t dec_info, buf_t &out) const;
};

static int evp_md_size(hash_e type) { return hash_alg_t::get(type).size; }
static int evp_digest_init_ex(hash_t &ctx, hash_e type, void *impl) {
  ctx.init();
  return 1;
}
static int evp_digest_update(hash_t &ctx, const void *d, size_t cnt) {
  ctx.update(const_byte_ptr(d), int(cnt));
  return 1;
}
static int evp_digest_final_ex(hash_t &ctx, unsigned char *md, unsigned int *s) {
  ctx.final(md);
  return 1;
}

}  // namespace coinbase::crypto
