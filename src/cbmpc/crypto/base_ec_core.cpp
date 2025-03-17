
#include "base_ec_core.h"

namespace coinbase::crypto {

booth_wnaf_t::booth_wnaf_t(int _win, const bn_t& x, int _bits, bool _back)
    : win(_win), bits(_bits), back(_back), index(0) {
  x.to_bin(data, 33);
  mem_t(data, 33).reverse();

  if (back) index = (((bits - 1) + (win - 1)) / win) * win;
}

booth_wnaf_t::booth_wnaf_t(int _win, const uint64_t x[4], int _bits, bool _back)
    : win(_win), bits(_bits), back(_back), index(0) {
  memmove(data, x, 32);
  data[32] = 0;

  if (back) index = (((bits - 1) + (win - 1)) / win) * win;
}

booth_wnaf_t::~booth_wnaf_t() { bzero(data); }

bool booth_wnaf_t::get(unsigned& value, bool& neg) {
  if (back) {
    if (index < 0) return false;
  } else {
    if (index >= bits) return false;
  }

  unsigned val;
  if (index == 0) {
    val = unsigned(data[0]) << 1;
  } else {
    int off = (index - 1) / 8;
    val = unsigned(data[off]);
    if (off + 1 < sizeof(data)) val |= unsigned(data[off + 1]) << 8;
    val = val >> ((index - 1) % 8);
  }

  const unsigned kMask = (1 << (win + 1)) - 1;
  val = val & kMask;

  unsigned s, d;
  s = ~((val >> win) - 1);
  d = (1 << (win + 1)) - val - 1;
  d = (d & s) | (val & ~s);
  d = (d >> 1) + (d & 1);
  neg = bool(s & 1);
  value = d;

  if (back)
    index -= win;
  else
    index += win;
  return true;
}

#ifdef __x86_64__
void ct_get2(__m128i* dst, const __m128i* precomp, int line_size, unsigned index) {
  __m128i lo1, hi1, lo2, hi2;
  lo1 = hi1 = lo2 = hi2 = _mm_setzero_si128();
  for (unsigned i = 0; i < line_size; i++) {
    __m128i mask = _mm_set1_epi32(-(index == i));
    lo1 = _mm_or_si128(lo1, _mm_and_si128(mask, _mm_load_si128(precomp + 0)));
    hi1 = _mm_or_si128(hi1, _mm_and_si128(mask, _mm_load_si128(precomp + 1)));
    lo2 = _mm_or_si128(lo2, _mm_and_si128(mask, _mm_load_si128(precomp + 2)));
    hi2 = _mm_or_si128(hi2, _mm_and_si128(mask, _mm_load_si128(precomp + 3)));
    precomp += 2 * 2;
  }
  _mm_storeu_si128(dst + 0, lo1);
  _mm_storeu_si128(dst + 1, hi1);
  _mm_storeu_si128(dst + 2, lo2);
  _mm_storeu_si128(dst + 3, hi2);
}

void ct_get3(__m128i* dst, const __m128i* precomp, int line_size, unsigned index) {
  __m128i lo1, hi1, lo2, hi2, lo3, hi3;
  lo1 = hi1 = lo2 = hi2 = lo3 = hi3 = _mm_setzero_si128();
  for (unsigned i = 0; i < line_size; i++) {
    __m128i mask = _mm_set1_epi32(-(index == i));
    lo1 = _mm_or_si128(lo1, _mm_and_si128(mask, _mm_load_si128(precomp + 0)));
    hi1 = _mm_or_si128(hi1, _mm_and_si128(mask, _mm_load_si128(precomp + 1)));
    lo2 = _mm_or_si128(lo2, _mm_and_si128(mask, _mm_load_si128(precomp + 2)));
    hi2 = _mm_or_si128(hi2, _mm_and_si128(mask, _mm_load_si128(precomp + 3)));
    lo3 = _mm_or_si128(lo3, _mm_and_si128(mask, _mm_load_si128(precomp + 4)));
    hi3 = _mm_or_si128(hi3, _mm_and_si128(mask, _mm_load_si128(precomp + 5)));
    precomp += 2 * 3;
  }
  _mm_storeu_si128(dst + 0, lo1);
  _mm_storeu_si128(dst + 1, hi1);
  _mm_storeu_si128(dst + 2, lo2);
  _mm_storeu_si128(dst + 3, hi2);
  _mm_storeu_si128(dst + 4, lo3);
  _mm_storeu_si128(dst + 5, hi3);
}

#endif

}  // namespace coinbase::crypto
