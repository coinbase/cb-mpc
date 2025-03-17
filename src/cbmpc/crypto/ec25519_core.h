#pragma once

#include <cbmpc/crypto/base.h>

namespace coinbase::crypto::ec25519_core {

class point_t;

point_t* new_point(const point_t* src);
point_t* new_point();
void free_point(point_t* a);
void copy(point_t* r, const point_t* a);
bool equ(const point_t* a, const point_t* b);
void add(point_t* r, const point_t* a, const point_t* b);
void sub(point_t* r, const point_t* a, const point_t* b);
void neg(point_t* r, const point_t* a);
int to_bin(const point_t* r, uint8_t* out);
error_t from_bin(point_t* r, mem_t in);
void mul_to_generator(point_t* r, const bn_t& x);
void mul(point_t* r, const point_t* a, const bn_t& x);
void mul_add(point_t* r, const point_t* P, const bn_t& x, const bn_t& y);  // r = x * P + y * G
void set_infinity(point_t* r);
bool is_infinity(const point_t* a);
void get_xy(const point_t* a, bn_t& x, bn_t& y);
bool set_xy(point_t* r, const bn_t& x, const bn_t& y);
bool is_on_curve(const point_t* a);
bool is_in_subgroup(const point_t* a);
const mod_t& get_order();
const point_t& get_generator();

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
