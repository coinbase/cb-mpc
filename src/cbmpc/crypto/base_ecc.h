#pragma once

#include "base_bn.h"

#ifndef NID_ED25519
#define NID_ED25519 1087
#endif

namespace coinbase::crypto {
class ecc_generator_point_t;
class ecc_pub_key_t;
class ecc_prv_key_t;
class ecc_point_t;

namespace secp256k1 {
typedef struct point_t* point_ptr_t;
}

namespace ec25519_core {
class point_t;
}

enum class ecurve_type_e {
  ossl = 1,
  ed25519 = 2,
  bitcoin = 4,
};

class ecurve_interface_t {
 public:
  virtual ~ecurve_interface_t() {}

  ecurve_type_e type;
  const_char_ptr name;
  int bits;
  uint16_t openssl_code;
  const EC_GROUP* group = nullptr;

  int size() const { return coinbase::bits_to_bytes(bits); }
  virtual void get_params(bn_t& p, bn_t& a, bn_t& b) const = 0;
  virtual void mul_to_generator(const bn_t& val, ecc_point_t& P) const = 0;
  virtual void mul_to_generator_vartime(const bn_t& val, ecc_point_t& P) const = 0;
  virtual void init_point(ecc_point_t& P) const = 0;
  virtual void set_ossl_point(ecc_point_t& P, const EC_POINT* point) const {}
  virtual void free_point(ecc_point_t& P) const = 0;
  virtual void invert_point(ecc_point_t& P) const = 0;
  virtual void copy_point(ecc_point_t& Dst, const ecc_point_t& Src) const = 0;
  virtual bool equ_points(const ecc_point_t& P1, const ecc_point_t& P2) const = 0;
  virtual bool is_on_curve(const ecc_point_t& P) const = 0;
  virtual bool is_in_subgroup(const ecc_point_t& P) const = 0;
  virtual bool is_infinity(const ecc_point_t& P) const = 0;
  virtual void set_infinity(ecc_point_t& P) const = 0;
  virtual void add(const ecc_point_t& P1, const ecc_point_t& P2, ecc_point_t& R) const = 0;
  virtual void add_consttime(const ecc_point_t& P, const ecc_point_t& x, ecc_point_t& R) const = 0;
  virtual void mul(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const = 0;
  virtual void mul_vartime(const ecc_point_t& P, const bn_t& x, ecc_point_t& R) const = 0;
  virtual void mul_add(const bn_t& n, const ecc_point_t& P, const bn_t& m, ecc_point_t& R) const;  // R = G*n + P*m
  virtual int to_compressed_bin(const ecc_point_t& P, byte_ptr out) const = 0;
  virtual int to_bin(const ecc_point_t& P, byte_ptr out) const { return to_compressed_bin(P, out); }
  virtual error_t from_bin(ecc_point_t& P, mem_t bin) const = 0;
  virtual void get_coordinates(const ecc_point_t& P, bn_t& x, bn_t& y) const = 0;
  virtual void set_coordinates(ecc_point_t& P, const bn_t& x, const bn_t& y) const = 0;
  virtual bool hash_to_point(mem_t bin, ecc_point_t& Q) const = 0;
  virtual const mod_t& order() const = 0;
  virtual const mod_t& p() const = 0;
  virtual const ecc_generator_point_t& generator() const = 0;

  virtual buf_t pub_to_der(const ecc_pub_key_t& P) const = 0;
  virtual buf_t prv_to_der(const ecc_prv_key_t& K) const = 0;
  virtual error_t pub_from_der(ecc_pub_key_t& P, mem_t der) const { return coinbase::error(E_NOT_SUPPORTED); }
  virtual error_t prv_from_der(ecc_prv_key_t& K, mem_t der) const { return coinbase::error(E_NOT_SUPPORTED); }
  virtual error_t verify(const ecc_pub_key_t& P, mem_t hash, mem_t sig) const = 0;
  virtual buf_t sign(const ecc_prv_key_t& K, mem_t hash) const = 0;
};

class ecurve_t {
  friend class ecc_point_t;
  friend class ecc_prv_key_t;
  friend class ecc_pub_key_t;

 public:
  ecurve_t(const ecurve_interface_t* _ptr = nullptr) noexcept(true) : ptr(_ptr) {}
  bool operator==(const ecurve_t& src) const { return ptr == src.ptr; }
  bool operator!=(const ecurve_t& src) const { return ptr != src.ptr; }
  bool operator!() const { return ptr == nullptr; }

  bool operator==(std::nullptr_t) const { return ptr == nullptr; }
  bool operator!=(std::nullptr_t) const { return ptr != nullptr; }
  operator bool() const { return ptr != nullptr; }

  int get_openssl_code() const;
  int bits() const;
  const_char_ptr get_name() const;
  ecurve_type_e type() const;

  static ecurve_t find(int openssl_id);
  static ecurve_t find(const EC_GROUP* group);

  int size() const;
  int point_bin_size() const;
  int compressed_point_bin_size() const;

  const ecc_generator_point_t& generator() const;
  ecc_point_t mul_to_generator(const bn_t& val) const;
  ecc_point_t mul_add(const bn_t& n, const ecc_point_t& P, const bn_t& m) const;  // G*n + P*m

  const mod_t& order() const;

  void get_params(bn_t& p, bn_t& a, bn_t& b) const;
  const mod_t& p() const;
  ecc_point_t infinity() const;
  bool hash_to_point(mem_t mem, ecc_point_t& Q) const;

  bn_t get_random_value() const;
  error_t check(const ecc_point_t& point) const;
  const EC_GROUP* get_group() const;

  void convert(coinbase::converter_t& converter);
  bool valid() const { return ptr != nullptr; }

 private:
  const ecurve_interface_t* ptr;
};

typedef ecurve_t ecc_curve_ptr;

std::ostream& operator<<(std::ostream& os, ecurve_t curve);

extern const ecurve_t curve_p256;
extern const ecurve_t curve_p384;
extern const ecurve_t curve_p521;
extern const ecurve_t curve_secp256k1;
extern const ecurve_t curve_ed25519;

class ecurve_ed_t;
class ecurve_ossl_t;
class ecurve_secp256k1_t;

class ecc_point_t {
  friend class ecc_key_t;
  friend class certificate_t;
  friend class ecurve_t;
  friend class ecurve_ed_t;
  friend class ecurve_ossl_t;
  friend class ecurve_secp256k1_t;

 public:
  operator const EC_POINT*() const { return ptr; }
  operator EC_POINT*() { return ptr; }

  ecc_point_t() : curve(nullptr), ptr(nullptr) {}
  explicit ecc_point_t(ecurve_t curve);
  ecc_point_t(ecurve_t curve, const EC_POINT* ptr);
  explicit ecc_point_t(const ec25519_core::point_t& ed);
  explicit ecc_point_t(const secp256k1::point_ptr_t p);

  ecc_point_t& operator=(const ecc_point_t& src);
  ecc_point_t& operator=(ecc_point_t&& src);  // move assignment

  ~ecc_point_t() { free(); }
  bool valid() const { return ptr != nullptr; }
  void free();

  ecc_point_t(const ecc_point_t& src);
  ecc_point_t(ecc_point_t&& src);  // move constructor
  ecurve_t get_curve() const { return curve; }

  int to_compressed_bin(byte_ptr bin) const;
  int to_compressed_oct(byte_ptr out) const { return to_compressed_bin(out); }
  buf_t to_compressed_bin() const;
  buf_t to_compressed_oct() const { return to_compressed_bin(); }
  int to_bin(byte_ptr bin) const;
  int to_oct(byte_ptr out) const { return to_bin(out); }
  buf_t to_bin() const;
  buf_t to_oct() const { return to_bin(); }

  error_t from_bin(ecurve_t curve, mem_t in);
  error_t from_oct(ecurve_t curve, mem_t in) { return from_bin(curve, in); }

  void get_coordinates(bn_t& x, bn_t& y) const;
  void get_x(bn_t& x) const;
  void get_y(bn_t& y) const;

  bn_t get_x() const;
  bn_t get_y() const;

  void set_coordinates(const bn_t& x, const bn_t& y);
  bool is_on_curve() const;
  bool is_in_subgroup() const;
  bool is_infinity() const;
  void invert();

  ecc_point_t& operator+=(const ecc_point_t& val);
  ecc_point_t& operator-=(const ecc_point_t& val);
  ecc_point_t& operator*=(const bn_t& val);
  bool operator==(const ecc_point_t& val) const;
  bool operator!=(const ecc_point_t& val) const;

  friend ecc_point_t operator+(const ecc_point_t& val1, const ecc_point_t& val2);
  friend ecc_point_t operator-(const ecc_point_t& val1, const ecc_point_t& val2);

  static ecc_point_t add(const ecc_point_t& val1, const ecc_point_t& val2);
  static ecc_point_t add_consttime(const ecc_point_t& val1, const ecc_point_t& val2);
  static ecc_point_t sub(const ecc_point_t& val1, const ecc_point_t& val2);
  static ecc_point_t mul(const ecc_point_t& val1, const bn_t& val2);

  ecc_point_t operator-() const;

  void attach(ecurve_t curve, EC_POINT* value);
  EC_POINT* detach() {
    EC_POINT* value = ptr;
    ptr = nullptr;
    return value;
  }

  void convert(coinbase::converter_t& converter);
  void convert_fixed_curve(coinbase::converter_t& converter, ecurve_t curve);

 protected:
  ecurve_t curve;
  union {
    EC_POINT* ptr = nullptr;
    ec25519_core::point_t* ed;
    secp256k1::point_ptr_t secp256k1;
  };
};

ecc_point_t extended_ec_mul_add_ct(const bn_t& x0, const ecc_point_t& P0, const bn_t& x1, const ecc_point_t& P1);
std::ostream& operator<<(std::ostream& os, const ecc_point_t& p);

ecc_point_t operator+(const ecc_point_t& val1, const ecc_point_t& val2);
ecc_point_t operator-(const ecc_point_t& val1, const ecc_point_t& val2);

class ecc_generator_point_t : public ecc_point_t {
  friend struct ecurve_interface_t;
  friend ecc_point_t operator*(const bn_t& val1, const ecc_generator_point_t& val2);

 public:
  ecc_generator_point_t() {}
  ecc_generator_point_t(const ecc_point_t& point) : ecc_point_t(point) {}
};

ecc_point_t operator*(const bn_t& x, const ecc_point_t& X);
ecc_point_t operator*(const bn_t& x, const ecc_generator_point_t& X);

struct sig_with_pub_key_t {
  ecc_point_t Q;
  buf_t sig;
  void convert(coinbase::converter_t& c) { c.convert(Q, sig); }
  error_t verify(mem_t hash) const;
  static error_t verify_all(const ecc_point_t& Q, mem_t hash, const std::vector<sig_with_pub_key_t>& sigs);
};

class ecc_pub_key_t : public ecc_point_t {
  friend class ecurve_ed_t;
  friend class ecurve_ossl_t;

 public:
  ecc_pub_key_t() {}
  ecc_pub_key_t(const ecc_point_t& P) : ecc_point_t(P) {}
  ecc_pub_key_t(const ecc_pub_key_t& P) = default;
  ecc_pub_key_t(ecc_pub_key_t&& P) = default;
  ecc_pub_key_t& operator=(const ecc_pub_key_t& P) = default;
  ecc_pub_key_t& operator=(ecc_pub_key_t&& P) = default;

  error_t verify(mem_t hash, mem_t signature) const;
  buf_t to_der() const;
  error_t from_der(mem_t der);
  error_t from_der(ecurve_t curve, mem_t der);

  bool operator==(const ecc_pub_key_t& val) const { return ecc_point_t(*this) == ecc_point_t(val); }
  bool operator!=(const ecc_pub_key_t& val) const { return ecc_point_t(*this) != ecc_point_t(val); }

  error_t verify_schnorr(mem_t message, mem_t signature) const;
};

class ecc_prv_key_t {
  friend class ecurve_ed_t;
  friend class ecurve_ossl_t;
  friend class ecurve_secp256k1_t;

 public:
  void generate(ecurve_t curve);
  void set(ecurve_t curve, const bn_t& val);
  void set_ed_bin(mem_t ed_bin);

  ecc_pub_key_t pub() const;

  bn_t value() const;
  buf_t get_ed_bin() const { return ed_bin; }

  buf_t sign(mem_t hash) const;
  buf_t ecdh(const ecc_point_t& P) const;
  buf_t to_der() const;
  error_t from_der(mem_t der);
  error_t from_der(ecurve_t curve, mem_t der);
  ecurve_t get_curve() const { return curve; }
  bool valid() const { return curve != nullptr; }
  void convert(coinbase::converter_t& converter);

  error_t execute(mem_t enc_info, buf_t& dec_info) const;

  buf_t sign_schnorr(mem_t hash) const;
  sig_with_pub_key_t sign_and_output_pub_key(mem_t hash) const;

 private:
  ecurve_t curve;
  bn_t val;
  buf_t ed_bin;  // for EDDSA
};

class ecdsa_signature_t {
  friend class ecc_key_t;

 public:
  ecdsa_signature_t() : curve(nullptr) {}
  ecdsa_signature_t(ecurve_t src_curve, const bn_t& src_r, const bn_t& src_s) : curve(src_curve), r(src_r), s(src_s) {}

  error_t from_der(ecurve_t curve, mem_t mem);

  buf_t to_der() const;

  int to_der(byte_ptr out) const;

  bn_t get_r() const { return r; };
  bn_t get_s() const { return s; };
  ecurve_t get_curve() const { return curve; };

  bool valid() const { return r != 0; }

  void convert(coinbase::converter_t& converter);

  error_t get_recovery_code(mem_t in, const ecc_point_t& pub_key, int& recovery_code);
  error_t recover_pub_key(mem_t in, int recovery_code, ecc_point_t& pub_key);

 private:
  ecurve_t curve;
  bn_t r, s;
};

class ecdh_t {
 public:
  typedef error_t (*exec_t)(void* ctx, cmem_t pub_key, cmem_t out_secret);

  ecdh_t(const ecc_prv_key_t& _key) : key(&_key), exec(nullptr), ctx(nullptr) {}
  ecdh_t(exec_t _exec, void* _ctx) : key(nullptr), exec(_exec), ctx(_ctx) {}

  error_t execute(const ecc_point_t& P, buf_t& out) const;
  static error_t execute(void* ctx, cmem_t pub_key, cmem_t out_secret);

 private:
  exec_t exec;
  void* ctx;
  const ecc_prv_key_t* key;
};

struct ecies_ciphertext_t {
  enum { iv_size = 12, tag_size = 12 };

  ecc_point_t E;
  uint8_t iv[iv_size];
  buf_t encrypted;
  void convert(coinbase::converter_t& converter);
  buf_t to_bin() const { return coinbase::convert(*this); }

  static int get_bin_size(int plaintext_size);

  error_t encrypt(const ecc_point_t& pub_key, mem_t aad, const bn_t& e, mem_t iv, mem_t plain);
  error_t encrypt(const ecc_point_t& pub_key, mem_t aad, mem_t plain, drbg_aes_ctr_t* drbg = nullptr);

  error_t decrypt(const ecdh_t& ecdh, mem_t aad, buf_t& decrypted);
  static error_t decrypt(const ecdh_t& ecdh, mem_t encrypted, mem_t aad, buf_t& decrypted);

  void add_password_encryption(mem_t password, mem_t salt);
  void remove_password_encryption(mem_t password, mem_t salt);
  void change_password_encryption(mem_t old_password, mem_t old_salt, mem_t new_password, mem_t new_salt);

  error_t decrypt_begin(buf_t& enc_info) const;
  error_t decrypt_end(mem_t aad, mem_t shared_secret, buf_t& out) const;
  error_t from_bin(mem_t mem) const;
};

struct allow_ecc_infinity_t {
  allow_ecc_infinity_t();
  ~allow_ecc_infinity_t();
};

}  // namespace coinbase::crypto
