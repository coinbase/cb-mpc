#include "extended_uint.h"

#include <cbmpc/core/error.h>
#include <cbmpc/core/utils.h>

namespace coinbase {

void uint256_t::to_bin(byte_ptr bin) const {
  be_set_8(&bin[24], w0);
  be_set_8(&bin[16], w1);
  be_set_8(&bin[8], w2);
  be_set_8(&bin[0], w3);
}

buf_t uint256_t::to_bin() const {
  buf_t r(32);
  to_bin(r.data());
  return r;
}

uint256_t uint256_t::from_bin(mem_t bin) {
  cb_assert(bin.size == 32);
  uint256_t r;
  r.w0 = be_get_8(bin.data + 24);
  r.w1 = be_get_8(bin.data + 16);
  r.w2 = be_get_8(bin.data + 8);
  r.w3 = be_get_8(bin.data + 0);
  return r;
}

bool uint256_t::operator==(const uint256_t& b) const {
  uint64_t x = w0 ^ b.w0;
  x |= w1 ^ b.w1;
  x |= w2 ^ b.w2;
  x |= w3 ^ b.w3;
  return x == 0;
}

void uint256_t::cnd_assign(bool flag, const uint256_t& a) {
  uint64_t mask = constant_time_mask_64(flag);
  w0 = ((a.w0 ^ w0) & mask) ^ w0;
  w1 = ((a.w1 ^ w1) & mask) ^ w1;
  w2 = ((a.w2 ^ w2) & mask) ^ w2;
  w3 = ((a.w3 ^ w3) & mask) ^ w3;
}

}  // namespace coinbase
