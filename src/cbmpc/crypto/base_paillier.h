#pragma once

#include "base_rsa.h"

namespace coinbase::crypto {

class paillier_t {
 public:
  enum class rerand_e { off = 0, on = 1 };

  class rerand_scope_t {
   public:
    rerand_scope_t(rerand_e mode) : save(get()) { set(mode); }
    ~rerand_scope_t() { set(save); }
    static rerand_e get();

   private:
    rerand_e save;
    static void set(rerand_e mode);
  };

  static const int bit_size = 2048;
  paillier_t() : has_private(false) {}
  ~paillier_t() {}

  /**
   * @specs:
   * - basic-primitives-spec | Paillier-KeyGen-1P
   */
  void generate();
  void create_prv(const bn_t& N, const bn_t& p, const bn_t& q);
  void create_pub(const bn_t& N);

  /**
   * @specs:
   * - basic-primitives-spec | Paillier-Encrypt-1P
   */
  bn_t encrypt(const bn_t& src) const;
  bn_t encrypt(const bn_t& src, const bn_t& rand) const;

  /**
   * @specs:
   * - basic-primitives-spec | Paillier-Decrypt-1P
   */
  bn_t decrypt(const bn_t& src) const;
  bn_t add_ciphers(const bn_t& src1, const bn_t& src2, rerand_e rerand_mode = rerand_e::on) const;
  bn_t sub_ciphers(const bn_t& src1, const bn_t& src2, rerand_e rerand_mode = rerand_e::on) const;
  bn_t mul_scalar(const bn_t& cipher, const bn_t& scalar, rerand_e rerand_mode = rerand_e::on) const;
  bn_t add_scalar(const bn_t& cipher, const bn_t& scalar, rerand_e rerand_mode = rerand_e::on) const;
  bn_t sub_scalar(const bn_t& cipher, const bn_t& scalar, rerand_e rerand_mode = rerand_e::on) const;
  bn_t sub_cipher_scalar(const bn_t& scalar, const bn_t& cipher, rerand_e rerand_mode = rerand_e::on) const;
  bn_t rerand(const bn_t& cipher) const;

  bn_t get_cipher_randomness(const bn_t& plain, const bn_t& cipher) const;

  void convert(coinbase::converter_t& converter);

  bool has_private_key() const { return has_private; }
  const mod_t& get_NN() const { return NN; }
  const mod_t& get_N() const { return N; }
  const bn_t& get_p() const { return p; }
  const bn_t& get_q() const { return q; }
  const bn_t& get_phi_N() const { return phi_N; }
  const bn_t& get_inv_phi_N() const { return inv_phi_N; }

  class elem_t {
    friend class paillier_t;

   public:
    elem_t() : paillier(nullptr) {}
    elem_t(const paillier_t& _paillier, const bn_t& _bn) : paillier(&_paillier), bn(_bn) {}
    elem_t(const elem_t& src) : paillier(src.paillier), bn(src.bn) {}
    elem_t(elem_t&& src) : paillier(src.paillier), bn(std::move(src.bn)) {}

    elem_t& operator=(const elem_t& src) {
      if (this != &src) {
        paillier = src.paillier;
        bn = src.bn;
      }
      return *this;
    }
    elem_t& operator=(elem_t&& src) {
      if (this != &src) {
        paillier = src.paillier;
        bn = std::move(src.bn);
      }
      return *this;
    }

    bool operator==(const elem_t& src) { return bn == src.bn; }
    bool operator!=(const elem_t& src) { return bn != src.bn; }

    elem_t& operator*=(const bn_t& src) { return *this = *this * src; }
    elem_t& operator+=(const bn_t& src) { return *this = *this + src; }
    elem_t& operator-=(const bn_t& src) { return *this = *this - src; }

    elem_t operator*(const bn_t& src) const {
      return elem_t(*paillier, paillier->mul_scalar(bn, src, rerand_scope_t::get()));
    }
    elem_t operator+(const bn_t& src) const {
      return elem_t(*paillier, paillier->add_scalar(bn, src, rerand_scope_t::get()));
    }
    elem_t operator-(const bn_t& src) const {
      return elem_t(*paillier, paillier->sub_scalar(bn, src, rerand_scope_t::get()));
    }
    elem_t operator+(const elem_t& src) const {
      return elem_t(*paillier, paillier->add_ciphers(bn, src.bn, rerand_scope_t::get()));
    }
    elem_t operator-(const elem_t& src) const {
      return elem_t(*paillier, paillier->sub_ciphers(bn, src.bn, rerand_scope_t::get()));
    }
    elem_t& operator+=(const elem_t& src) { return *this = *this + src; }
    elem_t& operator-=(const elem_t& src) { return *this = *this - src; }

    friend elem_t operator-(const bn_t& src1, const elem_t& src2);

    buf_t to_bin() const { return bn.to_bin(); }
    const bn_t& to_bn() const { return bn; }

    int get_bin_size() const { return bn.get_bin_size(); }
    void rerand() { bn = paillier->rerand(bn); }

   private:
    const paillier_t* paillier;
    bn_t bn;
  };

  elem_t enc(const bn_t& src) const { return elem_t(*this, encrypt(src)); }
  elem_t enc(const bn_t& src, const bn_t& rand) const { return elem_t(*this, encrypt(src, rand)); }
  bn_t decrypt(const elem_t& src) const { return decrypt(src.bn); }
  elem_t elem(const bn_t& src) const { return elem_t(*this, src); }

  error_t verify_cipher(const bn_t& cipher) const { return verify_cipher(N, NN, cipher); }
  error_t verify_cipher(const elem_t& cipher) const { return verify_cipher(cipher.bn); }
  template <class... Values>
  error_t verify_ciphers(Values... ciphers) const {
    std::array<bn_t, sizeof...(ciphers)> arr{ciphers...};
    return batch_verify_ciphers(&arr[0], sizeof...(ciphers));
  }
  error_t batch_verify_ciphers(const bn_t* ciphers, int n) const;

 private:
  bool has_private;
  mod_t N;
  mod_t NN;  // cached
  bn_t p;
  bn_t q;
  bn_t phi_N;      // cached
  bn_t inv_phi_N;  // cached

  struct crt_t {
    mod_t p, q;
    bn_t dp, dq, qinv;
    bn_t compute_power(const bn_t& x, const mod_t& NN) const;
  };
  crt_t crt_enc, crt_dec;

  static error_t verify_cipher(const mod_t& N, const mod_t& NN, const bn_t& cipher);
  void update_public();
  void update_private();
};

inline paillier_t::elem_t operator*(const bn_t& src1, const paillier_t::elem_t& src2) { return src2 * src1; }

inline paillier_t::elem_t operator-(const bn_t& src1, const paillier_t::elem_t& src2) {
  return paillier_t::elem_t(*src2.paillier,
                            src2.paillier->sub_cipher_scalar(src1, src2.bn, paillier_t::rerand_scope_t::get()));
}

template <class T>
T& update_state(T& state, const paillier_t::elem_t& v) {
  return update_state(state, v.to_bin());
}
inline int get_bin_size(const paillier_t::elem_t& v) { return v.get_bin_size(); }

}  // namespace coinbase::crypto
