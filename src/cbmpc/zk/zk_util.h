#pragma once

#include <cbmpc/crypto/base.h>

namespace coinbase::zk {

enum class zk_flag { unverified, verified, skip };

// read-only memory buffer of short integers
// for example, if only 13 bits used from each 16 bits block (uint16_t),
// then 16 bits are used for simpler splitting using uint16_t
template <int item_bitlen>
class uint_mem_array_t {
 private:
  const_byte_ptr ptr;

 public:
  uint_mem_array_t(mem_t mem) : ptr(mem.data) {}
  unsigned operator[](int index) const {
    constexpr int byte_len = (item_bitlen + CHAR_BIT - 1) / CHAR_BIT;
    static_assert(byte_len == 2, "unsupported bitlen");
    constexpr unsigned mask = unsigned(-1) >> ((sizeof(unsigned) * CHAR_BIT) - item_bitlen);
    return coinbase::be_get_2(ptr + index * byte_len) & mask;
  }
};

struct param_t {
  inline static constexpr int log_alpha = 13;
  inline static constexpr int padded_log_alpha = 16;  // rounded up multiple of 8 for byte alignment
  inline static constexpr int alpha = 1 << log_alpha;
  inline static constexpr int alpha_bits_mask = alpha - 1;

  static uint16_t get_log_alpha_bits(mem_t e, int index) {
    uint_mem_array_t<log_alpha> e_array(e);
    return e_array[index];
  }
};

struct paillier_interactive_param_t : public param_t {
  inline static constexpr int secp = SEC_P_STAT_SHORT;
  inline static constexpr int t = coinbase::crypto::div_ceil(secp, log_alpha);
  inline static constexpr int lambda = t * log_alpha;
};

struct paillier_non_interactive_param_t : public param_t {
  inline static constexpr int secp = SEC_P_COM;
  inline static constexpr int t = coinbase::crypto::div_ceil(secp, log_alpha);
  inline static constexpr int lambda = t * log_alpha;
};

}  // namespace coinbase::zk