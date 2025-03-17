#pragma once

#include <cbmpc/crypto/base.h>

namespace coinbase::zk {

enum class zk_flag { unverified, verified, skip };

struct param_t {
  inline static constexpr int log_alpha = 13;
  inline static constexpr int padded_log_alpha = 16;  // rounded up multiple of 8 for byte alignment
  inline static constexpr int alpha = 1 << log_alpha;
  inline static constexpr int alpha_bits_mask = alpha - 1;

  static uint16_t get_13_bits(mem_t e, int index) {
    static_assert(log_alpha <= 16);  // We assume alpha bits can be stored in 16 bits.
    uint16_t ei_tag = coinbase::be_get_2(e.data + index * 2);
    return ei_tag & alpha_bits_mask;  // 13 bits
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