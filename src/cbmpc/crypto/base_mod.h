#pragma once

namespace coinbase::crypto {

// This is a dangerous function and should be used only if you know what are you doing!
class vartime_scope_t {
 public:
  vartime_scope_t();
  ~vartime_scope_t();
};

bool is_vartime_scope();

class mod_t {
 public:
  mod_t();
  ~mod_t();

  mod_t(const mod_t& src);
  mod_t(mod_t&& src);

  mod_t& operator=(const mod_t& src);
  mod_t& operator=(mod_t&& src);

  mod_t(const bn_t& m, bool multiplicative_dense = true) : multiplicative_dense(multiplicative_dense) { init(m); }

  void convert(coinbase::converter_t&);

  // clang-format off
  bn_t add(const bn_t& a, const bn_t& b) const { bn_t r; _add(r, a, b); return r; }
  bn_t sub(const bn_t& a, const bn_t& b) const { bn_t r; _sub(r, a, b); return r; }
  bn_t neg(const bn_t& a) const                { bn_t r; _neg(r, a);    return r; }
  bn_t mul(const bn_t& a, const bn_t& b) const { bn_t r; _mul(r, a, b); return r; }
  bn_t div(const bn_t& a, const bn_t& b) const;
  
  enum class inv_algo_e { SCR = 0, RandomMasking = 1 };
  bn_t inv(const bn_t& a) const { bn_t r; _inv(r, a, multiplicative_dense ? inv_algo_e::RandomMasking : inv_algo_e::SCR); return r; }
  bn_t inv(const bn_t& a, inv_algo_e alg) const { bn_t r; _inv(r, a, alg); return r; }
  
  bn_t pow(const bn_t& x, const bn_t& e) const { bn_t r; _pow(r, x, e); return r; }
  bn_t mod(const bn_t& a) const                { bn_t r; _mod(r, a);    return r; }
  bn_t mod(int a) const { return a < 0 ? neg(bn_t(-a)) : bn_t(a); }
  // clang-format on

  // only works with odd m
  static bn_t mod(const bn_t& a, const bn_t& m);

  static bool coprime(const bn_t& a, const mod_t& m);

  static bn_t N_inv_mod_phiN_2048(const bn_t& a, const bn_t& m);

  bn_t rand() const;

  operator const bn_t&() const { return m; }
  bool is_valid() const { return mont != nullptr; }
  int get_bin_size() const { return m.get_bin_size(); }
  int get_bits_count() const { return m.get_bits_count(); }

  bool operator==(const bn_t& val) const { return m == val; }
  bool operator!=(const bn_t& val) const { return m != val; }
  bool operator>(const bn_t& val) const { return m > val; }
  bool operator<(const bn_t& val) const { return m < val; }
  bool operator>=(const bn_t& val) const { return m >= val; }
  bool operator<=(const bn_t& val) const { return m <= val; }
  bool operator==(int val) const { return m == val; }
  bool operator!=(int val) const { return m != val; }
  bool operator>(int val) const { return m > val; }
  bool operator<(int val) const { return m < val; }
  bool operator>=(int val) const { return m >= val; }
  bool operator<=(int val) const { return m <= val; }
  bn_t operator<<(int val2) const { return m << val2; }
  bn_t operator>>(int val2) const { return m >> val2; }

  bn_t to_mont(const bn_t& x) const;
  bn_t from_mont(const bn_t& x) const;
  bn_t mul_mont(const bn_t& x, const bn_t& y) const;

  BN_MONT_CTX* get_mont_ctx() const { return mont; }
  const bn_t& value() const { return m; }

 private:
  bn_t m;
  bn_t mu;
  bn_t b_pow_k_plus1;
  BN_MONT_CTX* mont = nullptr;
  bool multiplicative_dense = false;

  void init(const bn_t& m);
  void check(const bn_t& a) const;

  void _add(bn_t& r, const bn_t& a, const bn_t& b) const;
  void _sub(bn_t& r, const bn_t& a, const bn_t& b) const;
  void _neg(bn_t& r, const bn_t& a) const;
  void _mul(bn_t& r, const bn_t& a, const bn_t& b) const;
  void random_masking_inv(bn_t& r, const bn_t& a) const;
  void scr_inv(bn_t& r, const bn_t& a) const;
  void _inv(bn_t& r, const bn_t& a, inv_algo_e alg) const;
  void _pow(bn_t& r, const bn_t& x, const bn_t& e) const;
  void _mod(BIGNUM& r, const BIGNUM& a) const;
  void _mod(bn_t& r, const bn_t& a) const;
};

// clang-format off
inline const mod_t LARGEST_PRIME_MOD_2048 = bn_t::from_string("64634012142622014601429753377339903920888205339430968064260690855049310277735781786394402823045826927377435921843796038988239118300981842190176304772896566241261754734601992183500395500779304213592115276768135136553584437285239512323676188676952340941163291704072610085775151783082131617215104798247860771043828666779336684841369949573129138989712352070652644116155611318662052385416920628300517185728354233451887207436923714715196702304603291808807395226466574462454251369421640419450314203453862646939357085161313395870091994536705997276431050332778874671087204270866459209290636957209904296387111707222119192459863");
// clang-format on

extern "C" int bn_mod_add_fixed_top(BIGNUM* r, const BIGNUM* a, const BIGNUM* b, const BIGNUM* m);
extern "C" int bn_mod_sub_fixed_top(BIGNUM* r, const BIGNUM* a, const BIGNUM* b, const BIGNUM* m);
extern "C" void bn_mul_normal(BN_ULONG* r, const BN_ULONG* a, int na, const BN_ULONG* b, int nb);
extern "C" BIGNUM* bn_wexpand(BIGNUM* a, int words);
extern "C" void bn_correct_top(BIGNUM* a);

}  // namespace coinbase::crypto
