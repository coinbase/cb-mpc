#include <openssl/core_names.h>

#include <cbmpc/internal/crypto/mlkem768.h>

namespace coinbase::crypto {

void mlkem768_prv_key_t::generate() {
  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new_from_name(NULL, "ML-KEM-768", NULL);
  cb_assert(ctx);
  auto res = EVP_PKEY_keygen_init(ctx);
  cb_assert(res > 0);
  EVP_PKEY* pk = nullptr;
  res = EVP_PKEY_keygen(ctx, &pk);
  cb_assert(res > 0);
  pkey = pk;
}

mlkem768_pub_key_t mlkem768_prv_key_t::pub() const {
  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new(pkey, NULL);
  cb_assert(ctx);
  auto res = EVP_PKEY_fromdata_init(ctx);
  cb_assert(res > 0);

  OSSL_PARAM* params = NULL;
  res = EVP_PKEY_todata(pkey, EVP_PKEY_PUBLIC_KEY, &params);
  cb_assert(res > 0);
  EVP_PKEY* pk = nullptr;
  res = EVP_PKEY_fromdata(ctx, &pk, EVP_PKEY_PUBLIC_KEY, params);
  cb_assert(res > 0);
  OSSL_PARAM_free(params);

  mlkem768_pub_key_t p;
  p.pkey = pk;
  return p;
}

void mlkem768_pub_key_t::encapsulate(buf_t& secret, buf_t& encapsulated) const {
  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new(pkey, NULL);
  cb_assert(ctx);
  auto res = EVP_PKEY_encapsulate_init(ctx, NULL);
  cb_assert(res > 0);

  size_t secret_len, encapsulated_len;
  res = EVP_PKEY_encapsulate(ctx, NULL, &encapsulated_len, NULL, &secret_len);
  cb_assert(res > 0);
  secret.resize(int(secret_len));
  encapsulated.resize(int(encapsulated_len));

  res = EVP_PKEY_encapsulate(ctx, encapsulated.data(), &encapsulated_len, secret.data(), &secret_len);
  cb_assert(res > 0);
}

void mlkem768_pub_key_t::encapsulate(mem_t seed, buf_t& secret, buf_t& encapsulated) const {
  cb_assert(seed.size >= 32);
  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new(pkey, NULL);
  cb_assert(ctx);
  auto res = EVP_PKEY_encapsulate_init(ctx, NULL);
  cb_assert(res > 0);

  OSSL_PARAM params[2];
  params[0] = OSSL_PARAM_construct_octet_string(OSSL_KEM_PARAM_IKME, byte_ptr(seed.data), seed.size);
  params[1] = OSSL_PARAM_construct_end();
  cb_assert(0 < EVP_PKEY_CTX_set_params(ctx, params));

  size_t secret_len, encapsulated_len;
  res = EVP_PKEY_encapsulate(ctx, NULL, &encapsulated_len, NULL, &secret_len);
  cb_assert(res > 0);
  secret.resize(int(secret_len));
  encapsulated.resize(int(encapsulated_len));

  res = EVP_PKEY_encapsulate(ctx, encapsulated.data(), &encapsulated_len, secret.data(), &secret_len);
  cb_assert(res > 0);
}

error_t mlkem768_prv_key_t::decapsulate(mem_t encapsulated, buf_t& secret) const {
  scoped_ptr_t<EVP_PKEY_CTX> ctx = EVP_PKEY_CTX_new_from_pkey(NULL, pkey, NULL);
  cb_assert(ctx);
  auto res = EVP_PKEY_decapsulate_init(ctx, NULL);
  cb_assert(res > 0);

  size_t secret_len = 0;
  if (0 >= EVP_PKEY_decapsulate(ctx, NULL, &secret_len, encapsulated.data, encapsulated.size))
    return coinbase::error(E_CRYPTO);
  secret.resize(int(secret_len));
  if (0 >= EVP_PKEY_decapsulate(ctx, secret.data(), &secret_len, encapsulated.data, encapsulated.size)) {
    secret.free();
    return coinbase::error(E_CRYPTO);
  }
  return 0;
}

}  // namespace coinbase::crypto
