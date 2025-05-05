#pragma once

#include "base.h"

namespace coinbase::crypto {

static const bool ec_vartime = true;

class booth_wnaf_t {
 public:
  booth_wnaf_t(int win, const bn_t& x, int bits, bool back = false);
  booth_wnaf_t(int _win, const uint64_t x[4], int _bits, bool _back);
  ~booth_wnaf_t();
  bool get(unsigned& value, bool& neg);

 private:
  int win, bits, index;
  bool back;
  byte_t data[33];
};

#ifdef __x86_64__
void ct_get2(__m128i* dst, const __m128i* precomp, int line_size, unsigned index);
void ct_get3(__m128i* dst, const __m128i* precomp, int line_size, unsigned index);
#endif

enum {
  mulg_win = 6,
  mulg_line = 1 << (mulg_win - 1),
};

template <typename FE, int a_coeff>
struct edwards_projective_t {
  static_assert(a_coeff == -1, "a_coeff must be -1");

  using fe_t = FE;
  static fe_t get_d();
  struct precomp_t  // affine
  {
    fe_t y_minus_x, y_plus_x, kt;

    void set_xy(const fe_t& x, const fe_t& y) {
      y_minus_x = y - x;
      y_plus_x = y + x;
      fe_t t = y * x;
      fe_t d = get_d();
      kt = (d + d) * t;
    }

    void cnd_neg(bool flag) {
      fe_t neg_y_minus_x = y_plus_x;
      fe_t neg_y_plus_x = y_minus_x;
      fe_t neg_kt = -kt;
      y_minus_x.cnd_assign(flag, neg_y_minus_x);
      y_plus_x.cnd_assign(flag, neg_y_plus_x);
      kt.cnd_assign(flag, neg_kt);
    }

    template <bool vartime = false>
    static precomp_t ct_get(const precomp_t* precomp, byte_t index) {
      if constexpr (vartime) return precomp[index];

      precomp_t R;
      precomp++;
      index--;

#ifdef INTEL_X64
      ct_get3((__m128i*)&R, (const __m128i*)precomp, mulg_line, index);
#else
      for (unsigned i = 0; i < mulg_line; i++, precomp++) {
        bool flag = index == i;
        R.y_minus_x.cnd_assign(flag, precomp->y_minus_x);
        R.y_plus_x.cnd_assign(flag, precomp->y_plus_x);
        R.kt.cnd_assign(flag, precomp->kt);
      }
#endif
      return R;
    }
  };

  struct mulg_point_t {
    fe_t x, y, z, t;

    void get_xyz(fe_t& x, fe_t& y, fe_t& z) {
      x = this->x;
      y = this->y;
      z = this->z;
    }

    void set_infinity() {
      x = fe_t::zero();
      y = fe_t::one();
      z = fe_t::one();
      t = fe_t::zero();
    }

    void cnd_assign(bool flag, const precomp_t& p) {
      // nothing to do
    }

    void cnd_assign(bool flag, const mulg_point_t& p) {
      x.cnd_assign(flag, p.x);
      y.cnd_assign(flag, p.y);
      z.cnd_assign(flag, p.z);
      t.cnd_assign(flag, p.t);
    }
  };

  static void add_precomp_inplace(mulg_point_t& r, const precomp_t& p) {
    fe_t a, b, c, d, e, f, g, h;

    fe_t::sub(a, r.y, r.x);
    fe_t::mul(a, a, p.y_minus_x);

    fe_t::add(b, r.y, r.x);
    fe_t::mul(b, b, p.y_plus_x);

    fe_t::mul(c, r.t, p.kt);

    fe_t::add(d, r.z, r.z);
    fe_t::sub(e, b, a);
    fe_t::sub(f, d, c);
    fe_t::add(g, d, c);
    fe_t::add(h, b, a);

    fe_t::mul(r.x, e, f);
    fe_t::mul(r.y, g, h);
    fe_t::mul(r.t, e, h);
    fe_t::mul(r.z, f, g);
  }

  static bool equ(const fe_t& ax, const fe_t& ay, const fe_t& az, const fe_t& bx, const fe_t& by, const fe_t& bz) {
    fe_t ta, tb;
    fe_t::mul(ta, ax, bz);
    fe_t::mul(tb, bx, az);
    if (ta != tb) return false;

    fe_t::mul(ta, ay, bz);
    fe_t::mul(tb, by, az);
    if (ta != tb) return false;

    return true;
  }

  static void get_xy(const fe_t& x, const fe_t& y, const fe_t& z, fe_t& affine_x, fe_t& affine_y) {
    fe_t zi = z.inv();
    affine_x = x * zi;
    affine_y = y * zi;
  }

  static bool is_on_curve(const fe_t& x, const fe_t& y) {
    fe_t xx = x * x;
    fe_t yy = y * y;

    fe_t t = yy;
    if constexpr (a_coeff == -1)
      t -= yy;
    else if constexpr (a_coeff == 1)
      t += yy;
    else
      return false;

    fe_t d = get_d();
    return t == fe_t::one() + d * xx * yy;
  }

  static bool is_on_curve(const fe_t& x, const fe_t& y, const fe_t& z) {
    fe_t xx = x * x;
    fe_t yy = y * y;
    fe_t zz = z * z;

    fe_t t = yy;
    if constexpr (a_coeff == -1)
      t -= xx;
    else if constexpr (a_coeff == 1)
      t += xx;
    else
      return false;

    fe_t d = get_d();
    return t * zz == zz * zz + d * xx * yy;
  }

  static bool get_y_from_x(const fe_t& x, fe_t& y) {
    // y = sqrt((a * xx - 1) / (d * xx - 1))

    auto yy = -fe_t::one();
    fe_t xx = x * x;

    if constexpr (a_coeff == -1)
      yy -= xx;
    else if constexpr (a_coeff == 1)
      yy += xx;
    else
      return false;

    fe_t d = get_d();
    yy /= d * xx - fe_t::one();

    vartime_scope_t vartime_scope;
    return yy.sqrt(y);
  }

  static void neg(fe_t& rx, fe_t& ry, fe_t& rz) { rx = -rx; }

  static void cnd_neg_affine(bool flag, fe_t& rx, fe_t& ry) {
    fe_t neg = -rx;
    rx.cnd_assign(flag, neg);
  }

  static void cnd_neg(bool flag, fe_t& rx, fe_t& ry, fe_t& rz) {
    fe_t neg = -rx;
    rx.cnd_assign(flag, neg);
  }

  static void dbl(fe_t& rx, fe_t& ry, fe_t& rz, const fe_t& x, const fe_t& y, const fe_t& z) {
    fe_t tc;
    fe_t::sqr(tc, x);  // C = X_1^2
    fe_t td;
    fe_t::sqr(td, y);  // D = Y_1^2

    fe_t tf;

    if constexpr (a_coeff == -1) fe_t::sub(tf, td, tc);  // F = E + D

    fe_t te;
    fe_t::sqr(te, z);  // H = Z_1^2
    fe_t tb;
    fe_t::sub(tb, tf, te);
    fe_t::sub(tb, te);  // J = F - 2H
    fe_t::add(rx, x, y);
    fe_t::sqr(rx, rx);  // B = (X_1 + Y_1)^2
    fe_t::sub(rx, tc);
    fe_t::sub(rx, td);
    fe_t::mul(rx, tb);  // X_3 = (B - C - D) * J

    if constexpr (a_coeff == -1) te = -tc;  // E = aC

    fe_t::sub(ry, te, td);
    fe_t::mul(ry, tf);      // Y_3 = F * (aC - D)
    fe_t::mul(rz, tf, tb);  // Z_3 = F * J
  }

  static void add(fe_t& rx, fe_t& ry, fe_t& rz, const fe_t& ax, const fe_t& ay, const fe_t& az, const fe_t& bx,
                  const fe_t& by, const fe_t& bz) {
    bool a_is_inf = ax.is_zero();
    bool b_is_inf = bx.is_zero();

    fe_t save_ax = ax;
    fe_t save_ay = ay;
    fe_t save_az = az;

    fe_t ta;
    fe_t::mul(ta, az, bz);  // A = Z1 * Z2
    fe_t tb;
    fe_t::sqr(tb, ta);  // B = A^2
    fe_t tc;
    fe_t::mul(tc, ax, bx);  // C = X1 * X2
    fe_t td;
    fe_t::mul(td, ay, by);  // D = Y1 * Y2

    fe_t te;
    static const fe_t d = get_d();
    fe_t::mul(te, d, tc);
    fe_t::mul(te, td);  // E = d * C * D

    fe_t tf;
    fe_t::sub(tf, tb, te);  // F = B - E
    fe_t::add(te, tb);      // G = B + E

    fe_t::add(tb, ax, ay);
    fe_t::add(rx, bx, by);
    fe_t::mul(rx, tb);
    fe_t::sub(rx, tc);
    fe_t::sub(rx, td);
    fe_t::mul(rx, tf);
    fe_t::mul(rx, ta);  // X_3 = A * F * ((X_1 + Y_1) * (X_2 + Y_2) - C - D)

    if constexpr (a_coeff == -1) fe_t::add(ry, td, tc);

    fe_t::mul(ry, te);
    fe_t::mul(ry, ta);  // Y_3 = A * G * (D - aC)

    fe_t::mul(rz, tf, te);  // Z_3 = F * G

    rx.cnd_assign(a_is_inf, bx);
    ry.cnd_assign(a_is_inf, by);
    rz.cnd_assign(a_is_inf, bz);

    rx.cnd_assign(b_is_inf, save_ax);
    ry.cnd_assign(b_is_inf, save_ay);
    rz.cnd_assign(b_is_inf, save_az);
  }

  static void add_affine_inplace(fe_t& X3, fe_t& Y3, fe_t& Z3, const fe_t& X2, const fe_t& Y2) {
    const fe_t& X1 = X3;
    const fe_t& Y1 = Y3;
    const fe_t& Z1 = Z3;
    bool a_is_inf = X1.is_zero();
    static const fe_t d = get_d();

    fe_t B, C, D, E, F, G, H;
    fe_t::sqr(B, Z1);      // B = Z1^2
    fe_t::mul(C, X1, X2);  // C = X1*X2
    fe_t::mul(D, Y1, Y2);  // D = Y1*Y2
    fe_t::mul(E, d, C);
    fe_t::mul(E, D);       // E = d*C*D
    fe_t::sub(F, B, E);    // F = B-E
    fe_t::add(G, B, E);    // G = B+E
    fe_t::add(H, X1, Y1);  // H = X1+Y1
    fe_t::add(X3, X2, Y2);
    fe_t::mul(X3, H);
    fe_t::sub(X3, C);
    fe_t::sub(X3, D);
    fe_t::mul(X3, Z1);
    fe_t::mul(X3, F);  // X3 = Z1*F*((X1+Y1)*(X2+Y2)-C-D)
    if constexpr (a_coeff == -1) {
      fe_t::add(Y3, D, C);
    }
    fe_t::mul(Y3, Z1);
    fe_t::mul(Y3, G);     // Y3 = Z1*G*(D-a*C)
    fe_t::mul(Z3, F, G);  // Z3 = F*G

    X3.cnd_assign(a_is_inf, X2);
    Y3.cnd_assign(a_is_inf, Y2);
    Z3.cnd_assign(a_is_inf, fe_t::one());
  }
};

template <typename FORMULA, bool USE_GLV = false>
struct ecurve_core_t {
  using fe_t = typename FORMULA::fe_t;

  struct point_t {
    using curve = ecurve_core_t<FORMULA, USE_GLV>;

    static point_t affine(const bn_t& x, const bn_t& y) {
      point_t P;
      P.x = fe_t::from_bn(x);
      P.y = fe_t::from_bn(y);
      P.z = fe_t::one();
      return P;
    }

    fe_t x, y, z;

    bool is_infinity() const { return z.is_zero(); }
    void set_infinity() { x = y = z = fe_t::zero(); }

    void get_xy(fe_t& affine_x, fe_t& affine_y) const { FORMULA::get_xy(x, y, z, affine_x, affine_y); }

    void get_xy(bn_t& out_x, bn_t& out_y) const {
      fe_t affine_x, affine_y;
      get_xy(affine_x, affine_y);
      out_x = affine_x.to_bn();
      out_y = affine_y.to_bn();
    }

    bool is_on_curve() const { return FORMULA::is_on_curve(x, y, z); }

    point_t operator+(const point_t& P) const {
      point_t R;
      curve::add(R, *this, P);
      return R;
    }

    point_t& operator+=(const point_t& P) {
      curve::add(*this, *this, P);
      return *this;
    }

    point_t operator-(const point_t& P) const {
      point_t R;
      curve::add(R, *this, -P);
      return R;
    }

    point_t& operator-=(const point_t& P) {
      curve::add(*this, *this, -P);
      return *this;
    }

    point_t operator-() const {
      point_t R = *this;
      FORMULA::neg(R.x, R.y, R.z);
      return R;
    }

    void cnd_negate(bool flag) { FORMULA::cnd_neg(flag, x, y, z); }

    bool operator==(const point_t& P) const { return FORMULA::equ(x, y, z, P.x, P.y, P.z); }
    bool operator!=(const point_t& P) const { return !(*this == P); }
  };

  struct generator_point_t : public point_t {
    generator_point_t(const point_t& G) : point_t(G) {}
  };

  static const generator_point_t& generator() {
    static const generator_point_t G = generator_point();
    return G;
  }

  struct affine_point_t {
    fe_t x, y;
  };

  static const point_t& generator_point();
  static const mod_t& order();

  template <bool vartime = false>
  static point_t ct_get(const point_t* table, int line_size, unsigned index) {
    if constexpr (vartime) return table[index];

    point_t R;

#ifdef INTEL_X64
    ct_get3((__m128i*)&R, (const __m128i*)table, line_size, index);
#else
    R.set_infinity();
    for (unsigned i = 1; i < line_size; i++) {
      table++;
      bool flag = index == i;
      R.x.cnd_assign(flag, table->x);
      R.y.cnd_assign(flag, table->y);
      R.z.cnd_assign(flag, table->z);
    }
#endif
    return R;
  }

  static void dbl(point_t& r, const point_t& a) { FORMULA::dbl(r.x, r.y, r.z, a.x, a.y, a.z); };
  static void dbl(point_t& r) { FORMULA::dbl(r.x, r.y, r.z, r.x, r.y, r.z); };
  static void add(point_t& r, const point_t& a, const point_t& b) {
    FORMULA::add(r.x, r.y, r.z, a.x, a.y, a.z, b.x, b.y, b.z);
  };
  static void add(point_t& r, const point_t& a) { FORMULA::add(r.x, r.y, r.z, r.x, r.y, r.z, a.x, a.y, a.z); };

  template <bool vartime = false>
  static void mul(const point_t& P, const bn_t& v, point_t& R) {
#define tab_size 17
    point_t tab[tab_size];
    tab[0] = {fe_t::zero(), fe_t::zero(), fe_t::zero()};
    tab[1] = P;

    dbl(tab[2], tab[1]);
    add(tab[3], tab[2], P);
    dbl(tab[4], tab[2]);
    add(tab[5], tab[4], P);
    dbl(tab[6], tab[3]);
    add(tab[7], tab[6], P);
    dbl(tab[8], tab[4]);
    add(tab[9], tab[8], P);
    dbl(tab[10], tab[5]);
    add(tab[11], tab[10], P);
    dbl(tab[12], tab[6]);
    add(tab[13], tab[12], P);
    dbl(tab[14], tab[7]);
    add(tab[15], tab[14], P);

#if tab_size == 17
    dbl(tab[16], tab[8]);
    bool first = true;
    const int win = 5;
    unsigned value;
    bool neg;

    if constexpr (ecurve_core_t::use_glv) {
      point_t tab2[tab_size];
      tab2[0] = tab[0];
      for (int i = 1; i < tab_size; i++) tab2[i] = ecurve_core_t::endomorphism(tab[i]);

      bn_t v1, v2;
      ecurve_core_t::glv_decompose(v, v1, v2);
      bool v1_is_neg = v1.sign() < 0;
      bool v2_is_neg = v2.sign() < 0;
      v1.set_sign(+1);
      booth_wnaf_t wnaf1(win, v1, 128, true);
      v2.set_sign(+1);
      booth_wnaf_t wnaf2(win, v2, 128, true);

      while (wnaf1.get(value, neg)) {
        if (first) {
          first = false;
          R = ct_get<vartime>(tab, tab_size, value);
          R.cnd_negate(neg ^ v1_is_neg);
        } else {
          for (int i = 0; i < win; i++) dbl(R);
          auto A = ct_get<vartime>(tab, tab_size, value);
          A.cnd_negate(neg ^ v1_is_neg);
          add(R, A);
        }

        wnaf2.get(value, neg);
        auto A = ct_get<vartime>(tab2, tab_size, value);
        A.cnd_negate(neg ^ v2_is_neg);
        add(R, A);
      }
    } else {
      booth_wnaf_t wnaf(win, v, 256, true);
      while (wnaf.get(value, neg)) {
        if (first) {
          first = false;
          R = ct_get<vartime>(tab, tab_size, value);
          R.cnd_negate(neg);
        } else {
          for (int i = 0; i < win; i++) dbl(R);
          auto A = ct_get<vartime>(tab, tab_size, value);
          A.cnd_negate(neg);
          add(R, A);
        }
      }
    }

#else
    if constexpr (ecurve_core_t::use_glv) {
      bn_t v1, v2;
      ecurve_core_t::glv_decompose(v, v1, v2);
      bool v1_is_neg = v1.sign() < 0;
      bool v2_is_neg = v2.sign() < 0;
      v1.set_sign(+1);
      buf_t scalar1 = v1.to_bin(16);
      v2.set_sign(+1);
      buf_t scalar2 = v2.to_bin(16);

      point_t tab2[tab_size];
      tab2[0] = tab[0];
      for (int i = 1; i < tab_size; i++) tab2[i] = ecurve_core_t::endomorphism(tab[i]);

      for (int i = 1; i < tab_size; i++) tab[i].cnd_negate(v1_is_neg);
      for (int i = 1; i < tab_size; i++) tab2[i].cnd_negate(v2_is_neg);

      R = ct_get<vartime>(tab, tab_size, scalar1[0] >> 4);
      add(R, ct_get<vartime>(tab2, tab_size, scalar2[0] >> 4));

      dbl(R);
      dbl(R);
      dbl(R);
      dbl(R);
      add(R, ct_get<vartime>(tab, tab_size, scalar1[0] & 0x0f));
      add(R, ct_get<vartime>(tab2, tab_size, scalar2[0] & 0x0f));

      for (int i = 1; i < 16; i++) {
        dbl(R);
        dbl(R);
        dbl(R);
        dbl(R);
        add(R, ct_get<vartime>(tab, tab_size, scalar1[i] >> 4));
        add(R, ct_get<vartime>(tab2, tab_size, scalar2[i] >> 4));

        dbl(R);
        dbl(R);
        dbl(R);
        dbl(R);
        add(R, ct_get<vartime>(tab, tab_size, scalar1[i] & 0x0f));
        add(R, ct_get<vartime>(tab2, tab_size, scalar2[i] & 0x0f));
      }
    } else {
      buf_t scalar = v.to_bin(32);

      R = ct_get<vartime>(tab, tab_size, scalar[0] >> 4);

      dbl(R);
      dbl(R);
      dbl(R);
      dbl(R);
      add(R, ct_get<vartime>(tab, tab_size, scalar[0] & 0x0f));

      for (int i = 1; i < 32; i++) {
        dbl(R);
        dbl(R);
        dbl(R);
        dbl(R);
        add(R, ct_get<vartime>(tab, tab_size, scalar[i] >> 4));

        dbl(R);
        dbl(R);
        dbl(R);
        dbl(R);
        add(R, ct_get<vartime>(tab, tab_size, scalar[i] & 0x0f));
      }
    }
#endif
  }

  static point_t mul(const bn_t& x, const point_t& P) {
    point_t R;
    mul(P, x, R);
    return R;
  }

  using precomp_t = typename FORMULA::precomp_t;
  using mulg_point_t = typename FORMULA::mulg_point_t;

  static const precomp_t* precompute() {
    point_t base = generator_point();
    int qbits = order().get_bits_count();
    const int n = (qbits + mulg_win - 1) / mulg_win;

    precomp_t* precomp = new precomp_t[1 + n * mulg_line];

    precomp_t* precomp_line = precomp + 1;
    for (int i = 0; i < n; i++, precomp_line += mulg_line) {
      point_t row = base;
      for (int j = 0; j < mulg_line; j++) {
        fe_t x, y;
        FORMULA::get_xy(row.x, row.y, row.z, x, y);
        precomp_line[j].set_xy(x, y);
        add(row, base);
      }

      for (int j = 0; j < mulg_win; j++) dbl(base);
    }
    return precomp;
  }

  template <bool vartime = false>
  static void mul_to_generator(const bn_t& x, point_t& R) {
    static const precomp_t* precomp = precompute();
    const precomp_t* precomp_line = precomp;

    int qbits = order().get_bits_count();
    booth_wnaf_t wnaf(mulg_win, x, qbits);

    mulg_point_t A;
    A.set_infinity();

    bool is_neg;
    unsigned ind;
    bool r_is_inf = true;

    while (wnaf.get(ind, is_neg)) {
      precomp_t pre = precomp_t::template ct_get<vartime>(precomp_line, ind);
      precomp_line += mulg_line;

      pre.cnd_neg(is_neg);

      mulg_point_t save = A;
      FORMULA::add_precomp_inplace(A, pre);

      A.cnd_assign(r_is_inf, pre);

      bool add_inf = ind == 0;
      A.cnd_assign(add_inf, save);

      r_is_inf &= add_inf;
    }

    A.get_xyz(R.x, R.y, R.z);
    R.z.cnd_assign(r_is_inf, fe_t::zero());
  }

  static point_t mul_to_generator(const bn_t& x) {
    point_t R;
    mul_to_generator(x, R);
    return R;
  }

  static constexpr bool use_glv = USE_GLV;
  static void glv_decompose(const bn_t& v, bn_t& v1, bn_t& v2);
  static point_t endomorphism(const point_t& P);
};

}  // namespace coinbase::crypto
