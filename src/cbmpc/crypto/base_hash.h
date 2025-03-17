#pragma once

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include <cbmpc/core/strext.h>

#include "base_bn.h"

namespace coinbase::crypto {
class bn_t;

extern const uint32_t sha256_k[64];
extern const uint64_t sha512_k[80];

enum { max_hash_size = EVP_MAX_MD_SIZE };

enum class hash_e {
  none = NID_undef,
  sha256 = NID_sha256,
  sha384 = NID_sha384,
  sha512 = NID_sha512,
  sha3_256 = NID_sha3_256,
  sha3_384 = NID_sha3_384,
  sha3_512 = NID_sha3_512,
  blake2b = NID_blake2b512,
  blake2s = NID_blake2s256,
  shake128 = NID_shake128,
  shake256 = NID_shake256,
  ripemd160 = NID_ripemd160,
};

class hash_alg_t {
 public:
  hash_e type;
  int size;
  int block_size;
  int state_size;
  int length_size;
  mem_t oid;
  mem_t initial_state;
  const EVP_MD* md;

  bool valid() const { return type != hash_e::none; }

  static const hash_alg_t& get(hash_e type);
};

class ecc_point_t;
class ecc_generator_point_t;

inline int get_bin_size(cmem_t mem) { return mem.size; }
inline int get_bin_size(mem_t mem) { return mem.size; }
inline int get_bin_size(const buf_t& buf) { return buf.size(); }
inline int get_bin_size(const buf256_t& v) { return 32; }
inline int get_bin_size(const buf128_t& v) { return 16; }
inline int get_bin_size(byte_t v) { return 1; }
inline int get_bin_size(bool v) { return 1; }
inline int get_bin_size(uint16_t v) { return 2; }
inline int get_bin_size(int16_t v) { return 2; }
inline int get_bin_size(uint32_t v) { return 4; }
inline int get_bin_size(int32_t v) { return 4; }
inline int get_bin_size(uint64_t v) { return 8; }
inline int get_bin_size(int64_t v) { return 8; }
inline int get_bin_size(const std::string& v) { return (int)v.length(); }
inline int get_bin_size(const coinbase::bits_t& v) { return v.to_bin().size; }
inline int get_bin_size(const bn_t& v) { return v.get_bin_size(); }
inline int get_bin_size(const mod_t& v) { return v.get_bin_size(); }
inline int get_bin_size(const ecc_point_t& v) { return v.to_compressed_bin(nullptr); }
inline int get_bin_size(const ecc_generator_point_t& v) { return v.to_compressed_bin(nullptr); }
inline int get_bin_size(const coinbase::bufs128_t& v) { return mem_t(v).size; }
template <std::size_t N>
int get_bin_size(const byte_t (&v)[N]) {
  return N;
}
template <typename V>
int get_bin_size(const V& v) {
  return (int)coinbase::converter_t::convert_write(v, nullptr);
}

template <class T>
T& update_state(T& state, const buf_t& v) {
  return state.update(v.data(), v.size());
}
template <class T>
T& update_state(T& state, cmem_t v) {
  return state.update(v.data, v.size);
}
template <class T>
T& update_state(T& state, mem_t v) {
  return state.update(v.data, v.size);
}
template <class T>
T& update_state(T& state, bool v) {
  return update_state(state, byte_t(v ? 1 : 0));
}
template <class T>
T& update_state(T& state, byte_t v) {
  return state.update(&v, 1);
}
template <class T>
T& update_state(T& state, uint16_t v) {
  byte_t temp[2];
  coinbase::be_set_2(temp, v);
  return state.update(temp, 2);
}
template <class T>
T& update_state(T& state, int16_t v) {
  return update_state(state, uint16_t(v));
}
template <class T>
T& update_state(T& state, uint32_t v) {
  byte_t temp[4];
  coinbase::be_set_4(temp, v);
  return state.update(temp, 4);
}
template <class T>
T& update_state(T& state, int32_t v) {
  return update_state(state, uint32_t(v));
}
template <class T>
T& update_state(T& state, uint64_t v) {
  byte_t temp[8];
  coinbase::be_set_8(temp, v);
  return state.update(temp, 8);
}
template <class T>
T& update_state(T& state, int64_t v) {
  return update_state(state, uint64_t(v));
}
template <class T>
T& update_state(T& state, const std::string& v) {
  return update_state(state, strext::mem(v));
}
template <class T>
T& update_state(T& state, const coinbase::bits_t& v) {
  return update_state(state, v.to_bin());
}
template <class T>
T& update_state(T& state, const bn_t& v) {
  return update_state(state, v.to_bin());
}
template <class T>
T& update_state(T& state, const mod_t& v) {
  return update_state(state, bn_t(v).to_bin());
}
template <class T>
T& update_state(T& state, const ecc_point_t& v) {
  return update_state(state, v.to_compressed_bin());
}
template <class T>
T& update_state(T& state, const ecc_generator_point_t& v) {
  return update_state(state, v.to_compressed_bin());
}
template <class T>
T& update_state(T& state, const buf256_t& v) {
  return state.update(v, 32);
}
template <class T>
T& update_state(T& state, const buf128_t& v) {
  return state.update(v, 16);
}
template <class T>
T& update_state(T& state, const coinbase::bufs128_t& v) {
  return state.update(mem_t(v));
}
template <class T, std::size_t N>
T& update_state(T& state, const uint8_t (&v)[N]) {
  return state.update(v, N);
}
template <class T, typename V, std::size_t N>
T& update_state(T& state, const V (&v)[N]) {
  for (std::size_t i = 0; i < N; i++) update_state(state, v[i]);
  return state;
}
template <class T, typename V>
T& update_state(T& state, const std::vector<V>& v) {
  for (std::size_t i = 0; i < (int)v.size(); i++) update_state(state, v[i]);
  return state;
}
template <class T, typename V>
T& update_state(T& state, const V& v) {
  buf_t buf = coinbase::convert(v);
  return state.update(buf.data(), buf.size());
}

template <class T, typename V>
T& update_state(T& state, const coinbase::array_view_t<V>& v) {
  for (int i = 0; i < v.count; i++) update_state(state, v.ptr[i]);
  return state;
}

class hash_t {
 public:
  explicit hash_t(hash_e type);
  ~hash_t() { free(); }

  void free();

  hash_t& init();
  hash_t& update(const_byte_ptr ptr, int size);
  void final(byte_ptr out);
  buf_t final();
  void copy_state(hash_t& dst);

  template <typename T>
  hash_t& update(const T& v) {
    return update_state(*this, v);
  }

 private:
  const hash_alg_t& alg;
  EVP_MD_CTX* ctx_ptr = nullptr;
};

template <hash_e hash_type>
class hash_template_t {
 public:
  hash_template_t() : state(hash_type) { state.init(); }

  template <typename T>
  hash_template_t& update(const std::reference_wrapper<T> v) {
    state.update(v.get());
    return *this;
  }
  template <typename T>
  hash_template_t& update(const T& v) {
    state.update(v);
    return *this;
  }

  template <typename... ARGS>
  static buf_t hash(const ARGS&... args) {
    hash_template_t h;  // init
    h.update(args...);
    return h.final();
  }

  void final(byte_ptr out) { state.final(out); }
  buf_t final() { return state.final(); }
  void copy_state(hash_template_t& dst) { state.copy_state(dst.state); }

  template <typename... A>
  void update(std::tuple<A&...> tuple) {
    for_tuple(tuple, [this](const auto& item) { this->update(item); });
  }
  template <typename... A>
  void update(A&... args) {
    ([this](const auto& item) { this->update(item); }(args), ...);
  }
  template <typename... A>
  void update(A&&... args) {
    ([this](const auto& item) { this->update(item); }(args), ...);
  }

 protected:
  hash_t state;
};

class sha256_t : public hash_template_t<hash_e::sha256> {
 public:
  sha256_t() : hash_template_t<hash_e::sha256>() {}

  void final(byte_ptr out) { state.final(out); }
  buf256_t final() {
    buf256_t out;
    state.final(out);
    return out;
  }

  template <typename... ARGS>
  static buf256_t hash(const ARGS&... args) {
    crypto::sha256_t h;  // init
    h.update(args...);
    return h.final();
  }
};

typedef hash_template_t<hash_e::sha384> sha384_t;
typedef hash_template_t<hash_e::sha512> sha512_t;
typedef hash_template_t<hash_e::ripemd160> ripemd160_t;
typedef hash_template_t<hash_e::blake2b> blake2b_t;
typedef hash_template_t<hash_e::blake2s> blake2s_t;

class hmac_t {
 public:
  static const byte_t ipad_byte = 0x36;
  static const byte_t opad_byte = 0x5c;

  explicit hmac_t(hash_e type);
  ~hmac_t();

  hmac_t& init(mem_t key);
  hmac_t& update(const_byte_ptr ptr, int size);

  template <typename T>
  hmac_t& update(const T& v) {
    return update_state(*this, v);
  }

  void final(byte_ptr out);
  buf_t final();
  void copy_state(hmac_t& dst);

 private:
  const hash_alg_t& alg;
  EVP_MAC_CTX* ctx_ptr = nullptr;
};

template <hash_e type>
class hmac_template_t {
 public:
  hmac_template_t(mem_t key) : state(type) { state.init(key); }

  template <typename T>
  hmac_template_t& update(const T& v) {
    state.update(v);
    return *this;
  }

  template <typename... ARGS>
  buf_t calculate(const ARGS&... args) {
    update(args...);
    return final();
  }

  void final(byte_ptr out) { state.final(out); }
  buf_t final() { return state.final(); }
  void copy_state(hmac_template_t& dst) { state.copy_state(dst.state); }

 protected:
  hmac_t state;
  template <typename... A>
  void update(std::tuple<A&...>& tuple) {
    for_tuple(tuple, [this](const auto& item) { this->update(item); });
  }
  template <typename... A>
  void update(A&... args) {
    ([this](const auto& item) { this->update(item); }(args), ...);
  }
};

typedef hmac_template_t<hash_e::sha256> hmac_sha256_t;
typedef hmac_template_t<hash_e::sha384> hmac_sha384_t;
typedef hmac_template_t<hash_e::sha512> hmac_sha512_t;

/**
 * @specs:
 * - basic-primitives-spec | KDF-1P
 */
buf_t pbkdf2(hash_e type, mem_t password, mem_t salt, int iter, int out_size);

}  // namespace coinbase::crypto
