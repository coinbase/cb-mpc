#pragma once

#include <cbmpc/crypto/base.h>

namespace coinbase::crypto::ro {  // random oracle
struct hmac_state_t {
  crypto::hmac_sha256_t hmac;

  hmac_state_t();
  explicit hmac_state_t(mem_t key) : hmac(key) {}

  template <typename FIRST, typename... LAST>
  void encode_and_update(const FIRST& first, const LAST&... last) {
    encode_and_update(first);
    encode_and_update(last...);
  }

  template <typename T>
  void encode_and_update(const T& v) {
    hmac.update(crypto::get_bin_size(v));
    hmac.update(v);
  }

  template <typename T, size_t s>
  void encode_and_update(const std::array<T, s>& v) {
    hmac.update(int(s));
    hmac.update(v);
  }

  template <typename V, std::size_t N>
  void encode_and_update(const V (&v)[N]) {
    hmac.update(int(N));
    for (std::size_t i = 0; i < N; i++) encode_and_update(v[i]);
  }

  template <typename V>
  void encode_and_update(const std::vector<V>& v) {
    hmac.update(int(v.size()));
    for (std::size_t i = 0; i < v.size(); i++) encode_and_update(v[i]);
  }

  template <typename V>
  void encode_and_update(const coinbase::array_view_t<V>& v) {
    hmac.update(v.count);
    for (int i = 0; i < v.count; i++) encode_and_update(v.ptr[i]);
  }

  template <typename V>
  void update(const V& v) {
    hmac.update(v);
  }

  buf_t final();
};

class hash_string_t : public hmac_state_t {
 public:
  hash_string_t() {}

  template <typename... ARGS>
  hash_string_t(const ARGS&... args) {
    encode_and_update(args...);
  }

  buf_t bitlen(int bits);
  buf128_t bitlen128();
  buf256_t bitlen256();
};

/**
 * @specs:
 * - basic-primitives-spec | ro-hash-string-1P
 */
template <typename... ARGS>
hash_string_t hash_string(const ARGS&... args) {
  return hash_string_t(args...);
}

class hash_number_t : public hmac_state_t {
 public:
  template <typename... ARGS>
  hash_number_t(const ARGS&... args) {
    encode_and_update(args...);
  }

  bn_t mod(const mod_t& q);
};

/**
 * @specs:
 * - basic-primitives-spec | ro-hash-number-1P
 *
 * @deviations:
 * - Code uses 64 bit statistical security parameter by default
 */
template <typename... ARGS>
hash_number_t hash_number(const ARGS&... args) {
  return hash_number_t(args...);
}

class hash_numbers_t : public hmac_state_t {
 public:
  template <typename... ARGS>
  hash_numbers_t(const ARGS&... args) {
    encode_and_update(args...);
  }

  hash_numbers_t& count(int l) {
    this->l = l;
    return *this;
  }
  std::vector<bn_t> mod(const mod_t& q);

 private:
  int l;
};

/**
 * @specs:
 * - basic-primitives-spec | ro-hash-numbers-1P
 *
 * @deviations:
 * - Code uses 64 bit statistical security parameter by default
 */
template <typename... ARGS>
hash_numbers_t hash_numbers(const ARGS&... args) {
  return hash_numbers_t(args...);
}

class hash_curve_t : public hmac_state_t {
 public:
  template <typename... ARGS>
  hash_curve_t(const ARGS&... args) {
    encode_and_update(args...);
  }

  ecc_point_t curve(ecurve_t curve);
};

/**
 * @specs:
 * - basic-primitives-spec | ro-hash-curve-1P
 */
template <typename... ARGS>
hash_curve_t hash_curve(const ARGS&... args) {
  return hash_curve_t(args...);
}

buf_t drbg_sample_string(mem_t seed, int bits);
bn_t drbg_sample_number(mem_t seed, const mod_t& p);  // modulo p
ecc_point_t drbg_sample_curve(mem_t seed, const crypto::ecurve_t& curve);

}  // namespace coinbase::crypto::ro
