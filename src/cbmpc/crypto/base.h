
#pragma once

#if OPENSSL_VERSION_NUMBER >= 0x10100000
#define OPENSSL_BN_PTR
#define OPENSSL_RSA_PTR
#define OPENSSL_ECDSA_SIG_PTR
#define OPENSSL_METHOD_PTR
#endif

#if !defined(X509_get_notBefore) && (OPENSSL_VERSION_NUMBER >= 0x10100000)
#define X509_get_notBefore X509_getm_notBefore
#define X509_get_notAfter X509_getm_notAfter
#endif

#include <cbmpc/core/convert.h>

enum { E_CRYPTO = ERRCODE(ECATEGORY_CRYPTO, 1), E_ECDSA_2P_BIT_LEAK = ERRCODE(ECATEGORY_CRYPTO, 2) };

constexpr int SEC_P_COM = 128;
constexpr int SEC_P_STAT = 64;
constexpr int SEC_P_STAT_SHORT = 50;

namespace coinbase::crypto {
class bn_t;
class mod_t;
class ecc_point_t;

error_t error(const std::string& text, bool print_stack = true);
error_t openssl_error(const std::string& text);
error_t openssl_error(int rv, const std::string& text);
std::string openssl_get_last_error_string();

class initializer_t {
 public:
  initializer_t();
};

#ifdef __x86_64__
bool is_rdrand_supported();
error_t seed_rd_rand_entropy(int size);
#endif

void seed_random(mem_t in);
void gen_random(byte_ptr output, int size);
void gen_random(mem_t out);
buf_t gen_random(int size);
buf_t gen_random_bitlen(int bitlen);

coinbase::bits_t gen_random_bits(int count);
coinbase::bufs128_t gen_random_bufs128(int count);
bool gen_random_bool();

template <typename T>
T gen_random_int() {
  T result;
  gen_random((byte_ptr)&result, sizeof(T));
  return result;
}

bool secure_equ(mem_t src1, mem_t src2);
bool secure_equ(const_byte_ptr src1, const_byte_ptr src2, int size);

class evp_cipher_ctx_t {
 public:
  explicit evp_cipher_ctx_t() : ctx(EVP_CIPHER_CTX_new()) {}
  void clear() { EVP_CIPHER_CTX_reset(ctx); }
  ~evp_cipher_ctx_t() { EVP_CIPHER_CTX_free(ctx); }

  int update(mem_t in, byte_ptr out) const;

 public:
  EVP_CIPHER_CTX* ctx = nullptr;
};

class aes_ctr_t : public evp_cipher_ctx_t {
 public:
  aes_ctr_t() : evp_cipher_ctx_t() {}
  void init(mem_t key, const_byte_ptr iv);
  void init(buf128_t key, buf128_t iv) { init(mem_t(key), const_byte_ptr(&iv)); }
  void init(buf256_t key, buf128_t iv) { init(mem_t(key), const_byte_ptr(&iv)); }
  static buf_t encrypt(mem_t key, const_byte_ptr iv, mem_t in);
  static buf_t decrypt(mem_t key, const_byte_ptr iv, mem_t in);
  static void encrypt(mem_t key, const_byte_ptr iv, mem_t in, byte_ptr out);
  static void decrypt(mem_t key, const_byte_ptr iv, mem_t in, byte_ptr out);
};

class drbg_aes_ctr_t {
 public:
  drbg_aes_ctr_t(mem_t seed);
  ~drbg_aes_ctr_t() {}

  /**
   * @notes:
   * - Note: this must be followed by a call to seed
   */
  void init();
  /**
   * @specs:
   * - basic-primitives-spec | drbg-init-1P
   */
  void init(mem_t seed);

  /**
   * @specs:
   * - basic-primitives-spec | drbg-add-seed-1P
   */
  void seed(mem_t in);

  /**
   * @specs:
   * - basic-primitives-spec | drbg-get-random-1P
   */
  void gen(mem_t out);
  void gen(byte_ptr out, int size) { gen(mem_t(out, size)); }
  buf_t gen(int size) {
    buf_t result(size);
    gen(result);
    return result;
  }
  buf_t gen_bitlen(int bitlen) { return gen(coinbase::bits_to_bytes(bitlen)); }
  bn_t gen_bn(int bits);
  bn_t gen_bn(const mod_t& mod);
  bn_t gen_bn(const bn_t& mod);

  bool gen_bool() { return (gen_byte() & 1) != 0; }
  uint32_t gen_int() {
    uint32_t result = 0;
    gen(byte_ptr(&result), sizeof(result));
    return result;
  }
  uint64_t gen_int64() {
    uint64_t result = 0;
    gen(byte_ptr(&result), sizeof(result));
    return result;
  }
  byte_t gen_byte() {
    byte_t result;
    gen(&result, 1);
    return result;
  }
  buf128_t gen_buf128() {
    buf128_t result;
    gen(byte_ptr(&result), sizeof(result));
    return result;
  }
  buf256_t gen_buf256() {
    buf256_t result;
    gen(byte_ptr(&result), sizeof(result));
    return result;
  }
  coinbase::bufs128_t gen_bufs128(int count);

 private:
  aes_ctr_t ctr;
};

template <typename T>
void random_shuffle(buf128_t key, T& v, int count) {
  std::vector<uint32_t> rnd(count);
  drbg_aes_ctr_t(key).gen(byte_ptr(rnd.data()), count * sizeof(uint32_t));

  for (uint32_t i = 0; i < (uint32_t)count - 1; i++) {
    unsigned k = rnd[i] % (count - i);
    if (k == 0) continue;
    std::swap(v[i], v[i + k]);
  }
}

class aes_gcm_t {
 private:
  evp_cipher_ctx_t cipher;

  aes_gcm_t() = default;
  void encrypt_init(mem_t key, mem_t iv, mem_t auth);
  void encrypt_final(mem_t tag);  // tag.data is output

  void decrypt_init(mem_t key, mem_t iv, mem_t auth);
  error_t decrypt_final(mem_t tag);

  void reinit(mem_t iv, mem_t auth);

 public:
  static void encrypt(mem_t key, mem_t iv, mem_t auth, int tag_size, mem_t in, buf_t& out);
  static error_t decrypt(mem_t key, mem_t iv, mem_t auth, int tag_size, mem_t in, buf_t& out);
};

class aes_gmac_t : public evp_cipher_ctx_t {
 public:
  aes_gmac_t() : evp_cipher_ctx_t() {}
  static void calculate(mem_t key, mem_t iv, mem_t in, mem_t out);
  static buf_t calculate(mem_t key, mem_t iv, mem_t in, int out_size);
  void init(mem_t key, mem_t iv);
  void update(mem_t in);
  void update(bool in);
  void update(const buf128_t& in);
  void final(mem_t out);
  buf_t final(int size);
  buf128_t final();
};

}  // namespace coinbase::crypto

// clang-format off
// Order matters here
#include "base_bn.h"
#include "base_mod.h"
#include "base_ecc.h"
#include "base_eddsa.h"
#include "base_hash.h"
#include "base_paillier.h"
#include "base_rsa.h"

// clang-format on
using coinbase::crypto::bn_t;
using coinbase::crypto::ecc_point_t;
using coinbase::crypto::ecurve_t;
using coinbase::crypto::mod_t;

namespace coinbase::crypto {

using pname_t = std::string;
using mpc_pid_t = bn_t;

}  // namespace coinbase::crypto
