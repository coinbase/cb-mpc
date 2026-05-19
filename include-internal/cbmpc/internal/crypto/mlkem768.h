#pragma once

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/scope.h>

namespace coinbase::crypto {

class mlkem768_pub_key_t;
class mlkem768_prv_key_t;

class mlkem768_pub_key_t {
  friend class mlkem768_prv_key_t;

 public:
  void encapsulate(buf_t& secret, buf_t& encapsulated) const;
  void encapsulate(mem_t seed, buf_t& secret, buf_t& encapsulated) const;

 private:
  scoped_ptr_t<EVP_PKEY> pkey;
};

class mlkem768_prv_key_t {
  friend class mlkem768_pub_key_t;

 public:
  void generate();
  mlkem768_pub_key_t pub() const;
  error_t decapsulate(mem_t encapsulated, buf_t& secret) const;

 private:
  scoped_ptr_t<EVP_PKEY> pkey;
};

}  // namespace coinbase::crypto
