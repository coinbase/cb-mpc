#pragma once

#include <cbmpc/crypto/base.h>

namespace coinbase::crypto::ec25519_core {

crypto::ecp_storage_t* new_point(const crypto::ecp_storage_t* src);
crypto::ecp_storage_t* new_point();
void free_point(crypto::ecp_storage_t* a);
void copy(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a);
bool equ(const crypto::ecp_storage_t* a, const crypto::ecp_storage_t* b);
void add(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a, const crypto::ecp_storage_t* b);
void sub(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a, const crypto::ecp_storage_t* b);
void neg(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a);
int to_bin(const crypto::ecp_storage_t* r, uint8_t* out);
error_t from_bin(crypto::ecp_storage_t* r, mem_t in);
void mul_to_generator(crypto::ecp_storage_t* r, const bn_t& x);
void mul_to_generator_vartime(crypto::ecp_storage_t* r, const bn_t& x);
void mul(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a, const bn_t& x);
void mul_vartime(crypto::ecp_storage_t* r, const crypto::ecp_storage_t* a, const bn_t& x);
void set_infinity(crypto::ecp_storage_t* r);
bool is_infinity(const crypto::ecp_storage_t* a);
void get_xy(const crypto::ecp_storage_t* a, bn_t& x, bn_t& y);
bool is_on_curve(const crypto::ecp_storage_t* a);
bool is_in_subgroup(const crypto::ecp_storage_t* a);
const crypto::ecp_storage_t& get_generator();

}  // namespace coinbase::crypto::ec25519_core

extern "C" {

int ED25519_verify(const uint8_t* message, size_t message_len, const uint8_t signature[64],
                   const uint8_t public_key[32]);
int ED25519_sign(uint8_t* out_sig, const uint8_t* message, size_t message_len, const uint8_t public_key[32],
                 const uint8_t private_key[32]);
int ED25519_sign_with_scalar(uint8_t* out_sig, const uint8_t* message, size_t message_len, const uint8_t public_key[32],
                             const uint8_t private_key[32]);
void ED25519_private_to_scalar(uint8_t out_scalar_bin[32], const uint8_t private_key[32]);
void ED25519_scalar_to_public(uint8_t out_public_key[32], const uint8_t scalar_bin[32]);
}
