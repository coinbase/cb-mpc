#include <utility>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/scope.h>

namespace coinbase::crypto {
// clang-format off
template<> void scoped_ptr_t<BN_CTX>       ::free(BN_CTX* ptr)       { BN_CTX_free(ptr);              }
template<> void scoped_ptr_t<EC_POINT>     ::free(EC_POINT* ptr)     { EC_POINT_clear_free(ptr);      }
template<> void scoped_ptr_t<EVP_PKEY>     ::free(EVP_PKEY* ptr)     { EVP_PKEY_free(ptr);            }
template<> void scoped_ptr_t<EVP_PKEY_CTX> ::free(EVP_PKEY_CTX* ptr) { EVP_PKEY_CTX_free(ptr);        }

template<> EVP_PKEY* scoped_ptr_t<EVP_PKEY>::copy(EVP_PKEY* ptr) { EVP_PKEY_up_ref(ptr); return ptr; }
// clang-format on

#ifdef __x86_64__

bool is_rdrand_supported() {
  unsigned int eax, ebx, ecx, edx;
  __get_cpuid(1, &eax, &ebx, &ecx, &edx);

  return (ecx & bit_RDRND) != 0;
}

// This is called multiple times, since it might fail.
static error_t get_rd_rand(uint64_t& out) {
  for (int i = 0; i < 15; i++) {
    unsigned char ok = 0;
    asm volatile("rdrand %0; setc %1" : "=r"(out), "=qm"(ok));
    if (ok) return SUCCESS;
  }
  return coinbase::error(E_CRYPTO);
}

error_t seed_rd_rand_entropy(int size) {
  error_t rv = UNINITIALIZED_ERROR;
  int count = (size + 7) / 8;
  buf_t entropy(count * 8);
  uint64_t* p = (uint64_t*)entropy.data();
  for (int i = 0; i < count; i++) {
    if (rv = get_rd_rand(p[i])) return rv;
  }
  seed_random(entropy);
  return SUCCESS;
}
#endif

initializer_t::initializer_t() {
#ifdef __x86_64__
  if (is_rdrand_supported()) crypto::seed_rd_rand_entropy(32);
#endif
}

error_t error(const std::string& text) { return coinbase::error(E_CRYPTO, ECATEGORY_CRYPTO, text); }

error_t openssl_error(const std::string& text) { return openssl_error(E_CRYPTO, text); }

std::string openssl_get_last_error_string() {
  char ssl_message[1024] = "";
  int err = (int)ERR_get_error();
  ERR_error_string(err, ssl_message);
  return ssl_message;
}

error_t openssl_error(int rv, const std::string& text) {
  int err = (int)ERR_peek_error();
  std::string ssl_message = openssl_get_last_error_string();
  std::string message = text;
  if (message.empty()) message = "OPENSSL error: ";

  return coinbase::error(rv, ECATEGORY_OPENSSL, message + "(" + strext::itoa(err) + ") " + ssl_message);
}

/**
 * It seeds the global random number generator of openssl
 * Used primarily for testing
 */
void seed_random(mem_t in) { RAND_seed(in.data, in.size); }

void gen_random(byte_ptr output, int size) {
  int res = RAND_bytes(output, size);
  cb_assert(res > 0);
}

void gen_random(mem_t out) { gen_random(const_cast<byte_ptr>(out.data), out.size); }

bool gen_random_bool() {
  uint8_t temp = 0;
  gen_random(&temp, 1);
  return (temp & 1) == 0;
}

buf_t gen_random(int size) {
  buf_t output(size);
  gen_random(output.data(), size);
  return output;
}

buf_t gen_random_bitlen(int bitlen) {
  cb_assert(bitlen >= 0);
  return gen_random(coinbase::bits_to_bytes(bitlen));
}

coinbase::bits_t gen_random_bits(int count) {
  coinbase::bits_t out(count);
  gen_random(const_cast<byte_ptr>(mem_t(out).data), coinbase::bits_to_bytes(count));
  return out;
}

int evp_cipher_ctx_t::update(mem_t in, byte_ptr out) const {
  if (in.size == 0) return 0;

  int out_size = 0;
  if (0 < EVP_CipherUpdate(ctx, out, &out_size, in.data, in.size)) return out_size;
  return -1;
}

// ------------------------- AES-CTR ---------------------------

static const EVP_CIPHER* cipher_aes_ctr(int key_size) {
  switch (key_size) {
    case 16:
      return EVP_aes_128_ctr();
    case 24:
      return EVP_aes_192_ctr();
    case 32:
      return EVP_aes_256_ctr();
  }
  cb_assert(false);
  return NULL;
}

void aes_ctr_t::init(mem_t key, const_byte_ptr iv) {
  cb_assert(0 < EVP_EncryptInit(ctx, cipher_aes_ctr(key.size), key.data, iv));
  cb_assert(0 < EVP_CIPHER_CTX_set_padding(ctx, 0));
}

buf_t aes_ctr_t::encrypt(mem_t key, const_byte_ptr iv, mem_t in) {
  buf_t out(in.size);
  encrypt(key, iv, in, out.data());
  return out;
}

buf_t aes_ctr_t::decrypt(mem_t key, const_byte_ptr iv, mem_t in) { return encrypt(key, iv, in); }

void aes_ctr_t::encrypt(mem_t key, const_byte_ptr iv, mem_t in, byte_ptr out) {
  aes_ctr_t ctr;
  ctr.init(key, iv);
  ctr.update(in, out);
}

void aes_ctr_t::decrypt(mem_t key, const_byte_ptr iv, mem_t in, byte_ptr out) { encrypt(key, iv, in, out); }

static const EVP_CIPHER* cipher_aes_gcm(int key_size) {
  switch (key_size) {
    case 16:
      return EVP_aes_128_gcm();
    case 24:
      return EVP_aes_192_gcm();
    case 32:
      return EVP_aes_256_gcm();
  }
  cb_assert(false);
  return NULL;
}

void aes_gcm_t::encrypt(mem_t key, mem_t iv, mem_t auth, int tag_size, mem_t in, buf_t& out) {
  aes_gcm_t gcm;
  gcm.encrypt_init(key, iv, auth);
  gcm.cipher.update(in, out.alloc(in.size + tag_size));
  gcm.encrypt_final(mem_t(out.data() + in.size, tag_size));
}

error_t aes_gcm_t::decrypt(mem_t key, mem_t iv, mem_t auth, int tag_size, mem_t in, buf_t& out) {
  out.free();
  if (in.size < tag_size) return coinbase::error(E_CRYPTO);

  aes_gcm_t gcm;
  gcm.decrypt_init(key, iv, auth);
  int data_size = in.size - tag_size;
  buf_t candidate(data_size);
  if (gcm.cipher.update(mem_t(in.data, data_size), candidate.data()) != data_size) return coinbase::error(E_CRYPTO);
  if (error_t rv = gcm.decrypt_final(mem_t(in.data + data_size, tag_size))) return rv;

  out = std::move(candidate);
  return SUCCESS;
}

void aes_gcm_t::encrypt_init(mem_t key, mem_t iv, mem_t auth) {
  cb_assert(0 < EVP_EncryptInit(cipher.ctx, cipher_aes_gcm(key.size), NULL, NULL));
  cb_assert(0 < EVP_CIPHER_CTX_ctrl(cipher.ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size, NULL));
  cb_assert(0 < EVP_EncryptInit(cipher.ctx, NULL, key.data, iv.data));
  cb_assert(0 < EVP_CIPHER_CTX_set_padding(cipher.ctx, 0));
  if (auth.size > 0) {
    int out_size = 0;
    cb_assert(0 < EVP_CipherUpdate(cipher.ctx, NULL, &out_size, auth.data, auth.size));
  }
}

void aes_gcm_t::encrypt_final(mem_t tag)  // tag.data is output
{
  int out_size = 0;
  cb_assert(0 < EVP_EncryptFinal_ex(cipher.ctx, NULL, &out_size));
  cb_assert(out_size == 0);
  cb_assert(0 < EVP_CIPHER_CTX_ctrl(cipher.ctx, EVP_CTRL_GCM_GET_TAG, tag.size, const_cast<byte_ptr>(tag.data)));
}

void aes_gcm_t::decrypt_init(mem_t key, mem_t iv, mem_t auth) {
  cb_assert(0 < EVP_DecryptInit(cipher.ctx, cipher_aes_gcm(key.size), NULL, NULL));
  cb_assert(0 < EVP_CIPHER_CTX_ctrl(cipher.ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size, NULL));
  cb_assert(0 < EVP_DecryptInit(cipher.ctx, NULL, key.data, iv.data));
  cb_assert(0 < EVP_CIPHER_CTX_set_padding(cipher.ctx, 0));
  if (auth.size > 0) {
    int out_size = 0;
    cb_assert(0 < EVP_DecryptUpdate(cipher.ctx, NULL, &out_size, auth.data, auth.size));
  }
}

error_t aes_gcm_t::decrypt_final(mem_t tag) {
  cb_assert(0 < EVP_CIPHER_CTX_ctrl(cipher.ctx, EVP_CTRL_GCM_SET_TAG, tag.size, const_cast<byte_ptr>(tag.data)));
  int dummy = 0;
  if (0 >= EVP_DecryptFinal_ex(cipher.ctx, NULL, &dummy)) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

void aes_gmac_t::init(mem_t key, mem_t iv) {
  cb_assert(0 < EVP_EncryptInit(ctx, cipher_aes_gcm(key.size), NULL, NULL));
  cb_assert(0 < EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size, NULL));
  cb_assert(0 < EVP_EncryptInit(ctx, NULL, key.data, iv.data));
  cb_assert(0 < EVP_CIPHER_CTX_set_padding(ctx, 0));
}

void aes_gmac_t::update(bool in) {
  byte_t x = in ? 1 : 0;
  update(mem_t(&x, 1));
}

void aes_gmac_t::update(const buf128_t& in) { update(mem_t(in)); }

void aes_gmac_t::update(mem_t in) {
  if (in.size == 0) return;
  int out_size = 0;
  cb_assert(0 < EVP_EncryptUpdate(ctx, NULL, &out_size, in.data, in.size));
}

void aes_gmac_t::final(mem_t out) {
  int out_size = 0;
  byte_t dummy = 0;
  cb_assert(0 < EVP_EncryptUpdate(ctx, &dummy, &out_size, &dummy, 0));
  cb_assert(0 < EVP_EncryptFinal_ex(ctx, NULL, &out_size));
  cb_assert(out_size == 0);
  cb_assert(0 < EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, out.size, const_cast<byte_ptr>(out.data)));
}

buf_t aes_gmac_t::final(int size) {
  buf_t out(size);
  final(out);
  return out;
}

buf128_t aes_gmac_t::final() {
  buf128_t result{};
  final(mem_t(result));
  return result;
}

void aes_gmac_t::calculate(mem_t key, mem_t iv, mem_t in, mem_t out) {
  aes_gmac_t ghash_aes;
  ghash_aes.init(key, iv);
  ghash_aes.update(in);
  ghash_aes.final(out);
}

buf_t aes_gmac_t::calculate(mem_t key, mem_t iv, mem_t in, int out_size) {
  buf_t out(out_size);
  calculate(key, iv, in, out);
  return out;
}

}  // namespace coinbase::crypto
