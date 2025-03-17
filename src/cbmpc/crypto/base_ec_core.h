#pragma once

#include "base.h"

namespace coinbase::crypto {

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

}  // namespace coinbase::crypto
