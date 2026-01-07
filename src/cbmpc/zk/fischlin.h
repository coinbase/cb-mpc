#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

namespace coinbase::zk {

// Variadic template function to serialize all bn_t objects and update the SHA256 context
inline void sha256_update_zs(EVP_MD_CTX* ctx) {}
template <typename... REST>
void sha256_update_zs(EVP_MD_CTX* ctx, const bn_t& first, REST&... rest) {
  alignas(64) byte_t temp[256];

  cb_assert(first.get_bin_size() <= 256);  // prevent stack overflow

  int len = first.to_bin(temp);
  EVP_DigestUpdate(ctx, temp, len);

  sha256_update_zs(ctx, rest...);
}

// Hard-coded to hash32, since `b` is at most 32
template <typename... BN_TS>
uint32_t hash32bit_for_zk_fischlin(mem_t common_hash, int i, int j, BN_TS&... zs) {
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  EVP_DigestInit(ctx, EVP_sha256());
  EVP_DigestUpdate(ctx, common_hash.data, common_hash.size);

  byte_t temp[32];
  coinbase::be_set_4(temp + 0, i);
  coinbase::be_set_4(temp + 4, j);
  EVP_DigestUpdate(ctx, temp, 8);

  sha256_update_zs(ctx, zs...);

  unsigned int hash_len = 0;
  EVP_DigestFinal(ctx, temp, &hash_len);
  EVP_MD_CTX_free(ctx);
  return coinbase::be_get_4(temp);
}

struct fischlin_params_t {
  int rho, b, t;

  int e_max() const {
    cb_assert(t < 32);
    return 1 << t;
  }
  uint32_t b_mask() const {
    cb_assert(b < 32);
    return (1 << b) - 1;
  }
  error_t check() const {
    if (rho <= 0) return coinbase::error(E_CRYPTO, "rho <= 0");
    if (b <= 0) return coinbase::error(E_CRYPTO, "b <= 0");
    if (b >= 32) return coinbase::error(E_CRYPTO, "b >= 32");
    if (int64_t(b) * int64_t(rho) < SEC_P_COM) return coinbase::error(E_CRYPTO, "b * rho < SEC_P_COM");
    return SUCCESS;
  }

  error_t check_with_effective_b(int effective_b) const {
    if (rho <= 0) return coinbase::error(E_CRYPTO, "rho <= 0");
    if (b <= 0) return coinbase::error(E_CRYPTO, "b <= 0");
    if (b >= 32) return coinbase::error(E_CRYPTO, "b >= 32");

    if (effective_b <= 0) return coinbase::error(E_CRYPTO, "effective_b <= 0");
    if (int64_t(rho) * int64_t(effective_b) < SEC_P_COM)
      return coinbase::error(E_CRYPTO, "rho * effective_b < SEC_P_COM");
    return SUCCESS;
  }
  void convert(coinbase::converter_t& c) { c.convert(rho, b); }  // t is not sent
};

/**
 * @specs:
 * - zk-proofs-spec | Prove-ZK-Fischlin-1P
 *
 * @notes:
 * - The corresponding verify function is defined for each ZKP separately.
 *   The main reason for this is to allow for optimizations that can be done on the verify function (e.g., see ZK-DL
 * optimization in the spec)
 */
void fischlin_prove(const fischlin_params_t& params, std::function<void()> restart,
                    std::function<void(int index)> begin, std::function<uint32_t(int index, int e_tag)> hash,
                    std::function<void(int index, int e_tag)> save, std::function<void(int e_tag)> next);

}  // namespace coinbase::zk
