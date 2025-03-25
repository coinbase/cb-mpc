#include "ot.h"

#include <cbmpc/crypto/ro.h>

namespace coinbase::mpc {

// ----------------------- base_ot_protocol_pvw_ctx_t -----------------------------------

error_t base_ot_protocol_pvw_ctx_t::step1_R2S(const coinbase::bits_t& b) {
  cb_assert(!sid.empty());

  const mod_t& q = curve.order();
  this->b = b;

  m = b.count();
  A.resize(m);
  B.resize(m);
  r.resize(m);
  ecc_point_t G[2];
  ecc_point_t H[2];
  G[0] = curve.generator();
  H[0] = crypto::ro::hash_curve(sid, 1).curve(curve);
  G[1] = crypto::ro::hash_curve(sid, 2).curve(curve);
  H[1] = crypto::ro::hash_curve(sid, 3).curve(curve);

  for (int i = 0; i < m; i++) {
    r[i] = bn_t::rand(q);
    int bi = b[i] ? 1 : 0;
    A[i] = r[i] * G[bi];
    B[i] = r[i] * H[bi];
  }

  return SUCCESS;
}

error_t base_ot_protocol_pvw_ctx_t::step2_S2R(const std::vector<buf_t>& x0, const std::vector<buf_t>& x1) {
  cb_assert(!sid.empty());

  error_t rv = UNINITIALIZED_ERROR;
  if (x0.size() != x1.size()) return coinbase::error(E_BADARG);
  m = int(x0.size());
  if (m != int(A.size())) return coinbase::error(E_BADARG);
  if (m != int(B.size())) return coinbase::error(E_BADARG);

  this->x0 = x0;
  this->x1 = x1;

  const mod_t& q = curve.order();
  ecc_point_t G0, G1, H0, H1;
  G0 = curve.generator();
  H0 = crypto::ro::hash_curve(sid, 1).curve(curve);
  G1 = crypto::ro::hash_curve(sid, 2).curve(curve);
  H1 = crypto::ro::hash_curve(sid, 3).curve(curve);

  U0.resize(m);
  V0.resize(m);
  U1.resize(m);
  V1.resize(m);

  for (int i = 0; i < m; i++) {
    if (rv = curve.check(A[i])) return coinbase::error(rv, "base_ot_protocol_pvw_ctx_t::step2_S2R: check A[i] failed");
    if (rv = curve.check(B[i])) return coinbase::error(rv, "base_ot_protocol_pvw_ctx_t::step2_S2R: check B[i] failed");

    bn_t s0 = bn_t::rand(q);
    bn_t t0 = bn_t::rand(q);
    U0[i] = curve.mul_add(s0, H0, t0);                           // U0[i] = s0 * G[0] + t0 * H[0];
    ecc_point_t X = extended_ec_mul_add_ct(s0, A[i], t0, B[i]);  // X     = s0 * A[i] + t0 * B[i];
    V0[i] = crypto::ro::hash_string(X).bitlen(l) ^ x0[i];

    bn_t s1 = bn_t::rand(q);
    bn_t t1 = bn_t::rand(q);
    U1[i] = extended_ec_mul_add_ct(s1, G1, t1, H1);  // U1[i] = s1 * G[1] + t1 * H[1];
    X = extended_ec_mul_add_ct(s1, A[i], t1, B[i]);  // X     = s1 * A[i] + t1 * B[i];
    V1[i] = crypto::ro::hash_string(X).bitlen(l) ^ x1[i];
  }

  return SUCCESS;
}

error_t base_ot_protocol_pvw_ctx_t::output_R(std::vector<buf_t>& x) {
  error_t rv = UNINITIALIZED_ERROR;
  if (m != int(U0.size())) return coinbase::error(E_BADARG);
  if (m != int(U1.size())) return coinbase::error(E_BADARG);
  if (m != int(V0.size())) return coinbase::error(E_BADARG);
  if (m != int(V1.size())) return coinbase::error(E_BADARG);

  x.resize(m);

  for (int i = 0; i < m; i++) {
    if (rv = curve.check(U0[i])) return coinbase::error(rv, "base_ot_protocol_pvw_ctx_t::output_R: check U0[i] failed");
    if (rv = curve.check(U1[i])) return coinbase::error(rv, "base_ot_protocol_pvw_ctx_t::output_R: check U1[i] failed");
    mem_t Vbi = b[i] ? V1[i] : V0[i];
    const ecc_point_t& Ubi = b[i] ? U1[i] : U0[i];

    x[i] = crypto::ro::hash_string(r[i] * Ubi).bitlen(l) ^ Vbi;
  }

  return SUCCESS;
}

// ------------------------- transpose ---------------------

namespace transpose {
// NOLINTBEGIN

typedef union {
  uint8_t b[16];
  uint64_t l[2];
} generic_v128_t;

static generic_v128_t lshift64x2(generic_v128_t v) {
  v.l[0] <<= 1;
  v.l[1] <<= 1;
  return v;
}

static uint16_t high16x8(generic_v128_t v) {
  uint16_t r = 0;
  for (int i = 0; i < 16; i++) r |= (v.b[i] >> 7) << i;
  return r;
}

#if defined(__x86_64__)
typedef union {
  __m128i x;
  uint8_t b[16];
  uint64_t l[2];
} intel_v128_t;
typedef intel_v128_t v128_t;
static intel_v128_t lshift64x2(intel_v128_t v) {
  v.x = _mm_slli_epi64(v.x, 1);
  return v;
}
static uint16_t high16x8(intel_v128_t v) { return _mm_movemask_epi8(v.x); }
// #elif defined(__aarch64__)
// typedef union { uint8x16_t x; uint8_t b[16]; uint64_t l[2]; } neon_v128_t;
#else
typedef generic_v128_t v128_t;
#endif

template <typename T>
static void matrix_transposition(uint8_t const* inp, uint8_t* out, int nrows, int ncols) {
#define INP_BYTE(x, y) inp[(x)*ncols / 8 + (y) / 8]
#define OUT_BYTE(x, y) out[(y)*nrows / 8 + (x) / 8]

  T tmp;
  cb_assert(nrows % 8 == 0 && ncols % 8 == 0);

  // Do the main body in 16x8 blocks:
  for (int rr = 0; rr < nrows; rr += 16) {
    for (int cc = 0; cc < ncols; cc += 8) {
      for (int i = 0; i < 16; ++i) tmp.b[i] = INP_BYTE(rr + i, cc);
      for (int i = 8; --i >= 0; tmp = lshift64x2(tmp)) *(uint16_t*)&OUT_BYTE(rr, cc + i) = high16x8(tmp);
    }
  }
}

static void matrix_transpose(uint8_t const* inp, uint8_t* out, int nrows, int ncols) {
  matrix_transposition<generic_v128_t>(inp, out, nrows, ncols);
}

// NOLINTEND
}  // namespace transpose

static void ot_matrix_transpose(const h_matrix_256rows_t& h_src, v_matrix_256cols_t& v_dst) {
  int n_blocks = h_src.cols() / 256;
  v_dst.alloc(h_src.cols());

  for (int i = 0; i < n_blocks; i++) {
    byte_t h_temp[256 * 32];
    for (int j = 0; j < 256; j++) memmove(&h_temp[j * 32], h_src.get_row(j).data + 32 * i, 32);
    transpose::matrix_transpose(h_temp, byte_ptr(&v_dst[i * 256]), 256, 256);
  }
}

// ------------------------- ot_ext_protocol_ctx_t ---------------------

error_t ot_ext_protocol_ctx_t::step1_R2S(mem_t sid, const std::vector<buf_t>& sigma0, const std::vector<buf_t>& sigma1,
                                         const coinbase::bits_t& rr, int l) {
  if (int(sigma0.size()) != u) return coinbase::error(E_BADARG);
  if (int(sigma1.size()) != u) return coinbase::error(E_BADARG);

  // make l a multiple of 8
  this->l = (l + 7) & ~7;
  int m = rr.count();
  // make m a multiple of 128 bits
  int pad = (m & 127) ? 128 - (m & 127) : 0;

  r = rr + crypto::gen_random_bits(kappa + pad);

  h_matrix_256rows_t T_tmp;
  T_tmp.alloc(m + kappa + pad);
  U.alloc(m + kappa + pad);

  std::vector<buf_t> sigma_tag0_table(u);

  for (int i = 0; i < u; i++) {
    buf_t sigma_tag0 = crypto::ro::drbg_sample_string(sigma0[i], m + kappa + pad);
    buf_t sigma_tag1 = crypto::ro::drbg_sample_string(sigma1[i], m + kappa + pad);

    buf_t Ui = sigma_tag0 ^ sigma_tag1 ^ r;

    U.set_row(i, Ui);
    T_tmp.set_row(i, sigma_tag0);

    sigma_tag0_table[i] = sigma_tag0;
  }

  ot_matrix_transpose(T_tmp, T);

  v0.resize(u * d);
  v1.resize(u * d);

  // Because u=256, each byte is a number between 0 and u-1. In addition, since u=256 is a power of 2, we don't need
  // extra statistical security parameter to sample a random value. Therefore instead of the ro-hash-numbers-1P,
  // we use the following.
  buf_t e = crypto::ro::hash_string(sid, U).bitlen(bytes_to_bits(u * d));

  for (int i = 0; i < u; i++) {
    for (int j = 0; j < d; j++) {
      int index = d * i + j;

      unsigned alpha = i;
      unsigned beta = e[index];

      v0[index] = crypto::ro::hash_string(sigma_tag0_table[alpha] ^ sigma_tag0_table[beta]).bitlen(kappa);
      v1[index] = crypto::ro::hash_string(sigma_tag0_table[alpha] ^ sigma_tag0_table[beta] ^ r).bitlen(kappa);
    }
  }

  return SUCCESS;
}

error_t ot_ext_protocol_ctx_t::step2_S2R(mem_t sid, const coinbase::bits_t& s, const std::vector<buf_t>& sigma,
                                         const std::vector<buf_t>& x0, const std::vector<buf_t>& x1) {
  std::vector<bn_t> dummy;
  bool sender_one_input_random_mode = false;
  return step2_S2R_helper(sid, s, sigma, sender_one_input_random_mode, x0, x1, dummy, mod_t(), dummy, dummy);
}

error_t ot_ext_protocol_ctx_t::step2_S2R_sender_one_input_random(mem_t sid, const coinbase::bits_t& s,
                                                                 const std::vector<buf_t>& sigma,
                                                                 const std::vector<bn_t>& delta, const mod_t& q,
                                                                 std::vector<bn_t>& x0_out, std::vector<bn_t>& x1_out) {
  std::vector<buf_t> dummy;
  bool sender_one_input_random_mode = true;
  return step2_S2R_helper(sid, s, sigma, sender_one_input_random_mode, dummy, dummy, delta, q, x0_out, x1_out);
}

static buf_t hash_matrix_line(int index, buf256_t line, int l) {
  if (l == 256)  // for efficiency reasons, we use sha256 for 256-bit lines
  {
    return crypto::sha256_t::hash(index, line);
  }

  return crypto::ro::hash_string(index, line).bitlen(l);
};

error_t ot_ext_protocol_ctx_t::step2_S2R_helper(mem_t sid, const coinbase::bits_t& s, const std::vector<buf_t>& sigma,
                                                const bool sender_one_input_random_mode, const std::vector<buf_t>& x0,
                                                const std::vector<buf_t>& x1, const std::vector<bn_t>& delta,
                                                const mod_t& q, std::vector<bn_t>& x0_out, std::vector<bn_t>& x1_out) {
  int l, m;
  if (sender_one_input_random_mode) {
    l = bytes_to_bits(q.get_bin_size());
    m = int(delta.size());
    x0_out.resize(m);
    x1_out.resize(m);
  } else {
    if (x0.empty()) return coinbase::error(E_BADARG);
    l = bytes_to_bits(int(x0[0].size()));
    m = int(x0.size());
    w0.resize(m);
  }
  w1.resize(m);

  if (int(v0.size()) != u * d) return coinbase::error(E_BADARG);
  if (int(v1.size()) != u * d) return coinbase::error(E_BADARG);

  cb_assert(s.count() == u);
  if (int(sigma.size()) != s.count()) return coinbase::error(E_BADARG);
  if (x0.size() != x1.size()) return coinbase::error(E_BADARG);

  int pad = (m & 127) ? 128 - (m & 127) : 0;

  int u_cols = U.cols();
  if (u_cols != m + kappa + pad) return coinbase::error(E_BADARG);

  h_matrix_256rows_t Q_tmp;
  Q_tmp.alloc(m + kappa + pad);
  for (int i = 0; i < u; i++) {
    buf_t sigma_tag = crypto::ro::drbg_sample_string(sigma[i], m + kappa + pad);

    uint8_t mask = -static_cast<uint8_t>(s[i]);  // 0xFF if s[i] is 1, 0x00 otherwise
    for (size_t j = 0; j < sigma_tag.size(); j++) {
      sigma_tag[j] ^= U.get_row(i)[j] & mask;
    }

    Q_tmp.set_row(i, sigma_tag);
  }

  v_matrix_256cols_t Q;
  ot_matrix_transpose(Q_tmp, Q);

  buf_t e = crypto::ro::hash_string(sid, U).bitlen(8 * u * d);
  for (int i = 0; i < u; i++) {
    for (int j = 0; j < d; j++) {
      int index = d * i + j;

      unsigned alpha = i;
      unsigned beta = e[index];
      bool b = s[alpha] ^ s[beta];
      buf128_t vbz = b ? v1[index] : v0[index];
      buf128_t t = crypto::ro::hash_string(Q_tmp.get_row(alpha) ^ Q_tmp.get_row(beta)).bitlen128();

      if (t != vbz) return coinbase::error(E_CRYPTO);
    }
  }

  buf256_t s_buf;
  for (int i = 0; i < 256; i++) s_buf.set_bit(i, s[i]);

  for (int i = 0; i < m; i++) {
    buf_t w0_bin = hash_matrix_line(i, Q[i], l);
    buf_t w1_bin = hash_matrix_line(i, Q[i] ^ s_buf, l);

    if (sender_one_input_random_mode) {
      x0_out[i] = bn_t::from_bin(w0_bin);
      MODULO(q) x1_out[i] = x0_out[i] + delta[i];
      w1[i] = w1_bin ^ x1_out[i].to_bin(coinbase::bits_to_bytes(l));
    } else {
      if (bytes_to_bits(x0[i].size()) != l) return coinbase::error(E_BADARG);
      if (bytes_to_bits(x1[i].size()) != l) return coinbase::error(E_BADARG);
      w0[i] = w0_bin ^ x0[i];
      w1[i] = w1_bin ^ x1[i];
    }
  }

  return SUCCESS;
}

error_t ot_ext_protocol_ctx_t::output_R(int m, std::vector<buf_t>& x) {
  bool sender_one_input_random_mode = w0.empty();
  if (!sender_one_input_random_mode) {
    if (m != int(w0.size())) return coinbase::error(E_FORMAT);
  }

  if (m != int(w1.size())) return coinbase::error(E_FORMAT);

  x.resize(m);
  for (int i = 0; i < m; i++) {
    x[i] = hash_matrix_line(i, T[i], l);

    if (sender_one_input_random_mode) {
      if (r[i]) {
        buf_t& w = w1[i];
        if (bytes_to_bits(w.size()) != l)
          return coinbase::error(E_BADARG, "sender_one_input_random_mode: w1[i] size mismatch");
        x[i] ^= w;
      }
    } else {
      buf_t& w = r[i] ? w1[i] : w0[i];
      if (bytes_to_bits(w.size()) != l)
        return coinbase::error(E_BADARG, "non-sender_one_input_random_mode: w1[i] size mismatch");
      x[i] ^= w;
    }
  }
  return SUCCESS;
}

static std::vector<buf_t> zeroes(int m, int l) {
  buf_t z(coinbase::bits_to_bytes(l));
  z.bzero();
  return std::vector<buf_t>(m, z);
}

error_t ot_ext_protocol_ctx_t::sender_random_step1_R2S(mem_t sid, const std::vector<buf_t>& sigma0,
                                                       const std::vector<buf_t>& sigma1, const coinbase::bits_t& r,
                                                       int l, std::vector<buf_t>& x) {
  error_t rv = UNINITIALIZED_ERROR;
  if (rv = step1_R2S(sid, sigma0, sigma1, r, l)) return rv;

  int m = r.count();
  w0 = w1 = zeroes(m, l);
  if (rv = output_R(m, x)) return rv;
  return SUCCESS;
}

error_t ot_ext_protocol_ctx_t::sender_random_output_S(mem_t sid, const coinbase::bits_t& s,
                                                      const std::vector<buf_t>& sigma, int m, int l,
                                                      std::vector<buf_t>& x0, std::vector<buf_t>& x1) {
  error_t rv = UNINITIALIZED_ERROR;

  x0 = x1 = zeroes(m, l);
  if (rv = step2_S2R(sid, s, sigma, x0, x1)) return rv;
  x0 = w0;
  x1 = w1;
  return SUCCESS;
}

// ------------------------------ ot_protocol_pvw_ctx_t ---------------------

error_t ot_protocol_pvw_ctx_t::step1_S2R() {
  coinbase::bits_t s = crypto::gen_random_bits(u);
  return base.step1_R2S(s);
}

error_t ot_protocol_pvw_ctx_t::step2_R2S(const coinbase::bits_t& r, int l) {
  error_t rv = UNINITIALIZED_ERROR;

  std::vector<buf_t> sigma0(u);
  std::vector<buf_t> sigma1(u);
  for (int i = 0; i < u; i++) {
    sigma0[i] = crypto::gen_random_bitlen(ext.kappa);
    sigma1[i] = crypto::gen_random_bitlen(ext.kappa);
  }

  if (rv = base.step2_S2R(sigma0, sigma1)) return rv;
  if (rv = ext.step1_R2S(base.sid, base.x0, base.x1, r, l)) return rv;
  return SUCCESS;
}

error_t ot_protocol_pvw_ctx_t::step3_S2R(const std::vector<buf_t>& x0, const std::vector<buf_t>& x1) {
  cb_assert(x0.size() == x1.size());
  error_t rv = UNINITIALIZED_ERROR;
  std::vector<buf_t> sigma;
  if (rv = base.output_R(sigma)) return rv;
  if (rv = ext.step2_S2R(base.sid, base.b, sigma, x0, x1)) return rv;
  return SUCCESS;
}

// This is a wrapper around the above function and all it does is convert the input from bn_t to buf_t.
error_t ot_protocol_pvw_ctx_t::step3_S2R(const std::vector<bn_t>& x0, const std::vector<bn_t>& x1, int l) {
  cb_assert(x0.size() == x1.size());
  std::vector<buf_t> x0_bin(x0.size()), x1_bin(x1.size());
  int n = coinbase::bits_to_bytes(l);

  for (int i = 0; i < int(x0.size()); i++) {
    x0_bin[i] = x0[i].to_bin(n);
    x1_bin[i] = x1[i].to_bin(n);
  }
  return step3_S2R(x0_bin, x1_bin);
}

error_t ot_protocol_pvw_ctx_t::step3_S2R(const std::vector<bn_t>& delta, const mod_t& q, std::vector<bn_t>& x0,
                                         std::vector<bn_t>& x1) {
  error_t rv = UNINITIALIZED_ERROR;
  std::vector<buf_t> sigma;
  if (rv = base.output_R(sigma)) return rv;
  if (rv = ext.step2_S2R_sender_one_input_random(base.sid, base.b, sigma, delta, q, x0, x1)) return rv;
  return SUCCESS;
}

error_t ot_protocol_pvw_ctx_t::output_R(int m, std::vector<bn_t>& x) {
  std::vector<buf_t> x_bin;
  error_t rv = ext.output_R(m, x_bin);
  if (rv) return rv;
  x.resize(x_bin.size());
  for (int i = 0; i < int(x.size()); i++) x[i] = bn_t::from_bin(x_bin[i]);
  return SUCCESS;
}

// This is a wrapper around the above function and all it does is convert the input from buf_t to bn_t.
error_t ot_protocol_pvw_ctx_t::output_R(int m, std::vector<buf_t>& x) { return ext.output_R(m, x); }
}  // namespace coinbase::mpc
