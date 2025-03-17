#include <cbmpc/crypto/base.h>

// NOLINTBEGIN(*magic-number*)
namespace coinbase::crypto {

// http://oid-info.com/get/2.16.840.1.101.3.4.2.1
static const uint8_t SHA256_oid[] = {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                     0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20};

// http://oid-info.com/get/2.16.840.1.101.3.4.2.2
static const uint8_t SHA384_oid[] = {0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                     0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30};

// http://oid-info.com/get/2.16.840.1.101.3.4.2.3
static const uint8_t SHA512_oid[] = {0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                     0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40};

// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf 5.3.3 SHA-256
static const uint8_t SHA256_init[] = {0x6a, 0x09, 0xe6, 0x67, 0xbb, 0x67, 0xae, 0x85, 0x3c, 0x6e, 0xf3,
                                      0x72, 0xa5, 0x4f, 0xf5, 0x3a, 0x51, 0x0e, 0x52, 0x7f, 0x9b, 0x05,
                                      0x68, 0x8c, 0x1f, 0x83, 0xd9, 0xab, 0x5b, 0xe0, 0xcd, 0x19};

// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf 5.3.4 SHA-384
static const uint8_t SHA384_init[] = {0xcb, 0xbb, 0x9d, 0x5d, 0xc1, 0x05, 0x9e, 0xd8, 0x62, 0x9a, 0x29, 0x2a, 0x36,
                                      0x7c, 0xd5, 0x07, 0x91, 0x59, 0x01, 0x5a, 0x30, 0x70, 0xdd, 0x17, 0x15, 0x2f,
                                      0xec, 0xd8, 0xf7, 0x0e, 0x59, 0x39, 0x67, 0x33, 0x26, 0x67, 0xff, 0xc0, 0x0b,
                                      0x31, 0x8e, 0xb4, 0x4a, 0x87, 0x68, 0x58, 0x15, 0x11, 0xdb, 0x0c, 0x2e, 0x0d,
                                      0x64, 0xf9, 0x8f, 0xa7, 0x47, 0xb5, 0x48, 0x1d, 0xbe, 0xfa, 0x4f, 0xa4};

// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf 5.3.5 SHA-512
static const uint8_t SHA512_init[] = {0x6a, 0x09, 0xe6, 0x67, 0xf3, 0xbc, 0xc9, 0x08, 0xbb, 0x67, 0xae, 0x85, 0x84,
                                      0xca, 0xa7, 0x3b, 0x3c, 0x6e, 0xf3, 0x72, 0xfe, 0x94, 0xf8, 0x2b, 0xa5, 0x4f,
                                      0xf5, 0x3a, 0x5f, 0x1d, 0x36, 0xf1, 0x51, 0x0e, 0x52, 0x7f, 0xad, 0xe6, 0x82,
                                      0xd1, 0x9b, 0x05, 0x68, 0x8c, 0x2b, 0x3e, 0x6c, 0x1f, 0x1f, 0x83, 0xd9, 0xab,
                                      0xfb, 0x41, 0xbd, 0x6b, 0x5b, 0xe0, 0xcd, 0x19, 0x13, 0x7e, 0x21, 0x79};

// http://oid-info.com/get/2.16.840.1.101.3.4.2.8
static const uint8_t SHA3_256_oid[] = {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                       0x65, 0x03, 0x04, 0x02, 0x08, 0x05, 0x00, 0x04, 0x20};

// http://oid-info.com/get/2.16.840.1.101.3.4.2.9
static const uint8_t SHA3_384_oid[] = {0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                       0x65, 0x03, 0x04, 0x02, 0x09, 0x05, 0x00, 0x04, 0x30};

// http://oid-info.com/get/2.16.840.1.101.3.4.2.10
static const uint8_t SHA3_512_oid[] = {0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                       0x65, 0x03, 0x04, 0x02, 0x0a, 0x05, 0x00, 0x04, 0x40};

static const EVP_MD *evp_sha256() noexcept(true) { return EVP_sha256(); }
static const EVP_MD *evp_sha384() noexcept(true) { return EVP_sha384(); }
static const EVP_MD *evp_sha512() noexcept(true) { return EVP_sha512(); }
static const EVP_MD *evp_sha3_256() noexcept(true) { return EVP_sha3_256(); }
static const EVP_MD *evp_sha3_384() noexcept(true) { return EVP_sha3_384(); }
static const EVP_MD *evp_sha3_512() noexcept(true) { return EVP_sha3_512(); }
static const EVP_MD *evp_blake2s256() noexcept(true) { return EVP_blake2s256(); }
static const EVP_MD *evp_blake2b512() noexcept(true) { return EVP_blake2b512(); }
static const EVP_MD *evp_ripemd160() noexcept(true) { return EVP_ripemd160(); }

static const hash_alg_t alg_nohash = {hash_e::none, 0, 0, 0, 0, mem_t(), mem_t(), nullptr};
static const hash_alg_t alg_sha256 = {
    hash_e::sha256, 32, 64, 32, 8, mem_t(SHA256_oid, sizeof(SHA256_oid)), mem_t(SHA256_init, sizeof(SHA256_init)),
    evp_sha256()};
static const hash_alg_t alg_sha384 = {
    hash_e::sha384, 48, 128, 64, 16, mem_t(SHA384_oid, sizeof(SHA384_oid)), mem_t(SHA384_init, sizeof(SHA384_init)),
    evp_sha384()};
static const hash_alg_t alg_sha512 = {
    hash_e::sha512, 64, 128, 64, 16, mem_t(SHA512_oid, sizeof(SHA512_oid)), mem_t(SHA512_init, sizeof(SHA512_init)),
    evp_sha512()};
static const hash_alg_t alg_sha3_256 = {
    hash_e::sha3_256, 32, 136, 200, 0, mem_t(SHA3_256_oid, sizeof(SHA3_256_oid)), mem_t(), evp_sha3_256()};
static const hash_alg_t alg_sha3_384 = {
    hash_e::sha3_384, 48, 104, 200, 0, mem_t(SHA3_384_oid, sizeof(SHA3_384_oid)), mem_t(), evp_sha3_384()};
static const hash_alg_t alg_sha3_512 = {
    hash_e::sha3_512, 64, 72, 200, 0, mem_t(SHA3_512_oid, sizeof(SHA3_512_oid)), mem_t(), evp_sha3_512()};
static const hash_alg_t alg_shake128 = {hash_e::shake128, 0, 168, 200, 0, mem_t(), mem_t(), nullptr};
static const hash_alg_t alg_shake256 = {hash_e::shake256, 0, 136, 200, 0, mem_t(), mem_t(), nullptr};
static const hash_alg_t alg_blake2s = {hash_e::blake2s, 32, 64, 0, 0, mem_t(), mem_t(), evp_blake2s256()};
static const hash_alg_t alg_blake2b = {hash_e::blake2b, 64, 128, 0, 0, mem_t(), mem_t(), evp_blake2b512()};
static const hash_alg_t alg_ripemd160 = {hash_e::ripemd160, 20, 64, 20, 8, mem_t(), mem_t(), evp_ripemd160()};

const hash_alg_t &hash_alg_t::get(hash_e type)  // static
{
  switch (type) {
    case hash_e::sha256:
      return alg_sha256;
    case hash_e::sha384:
      return alg_sha384;
    case hash_e::sha512:
      return alg_sha512;
    case hash_e::sha3_256:
      return alg_sha3_256;
    case hash_e::sha3_384:
      return alg_sha3_384;
    case hash_e::sha3_512:
      return alg_sha3_512;
    case hash_e::shake128:
      return alg_shake128;
    case hash_e::shake256:
      return alg_shake256;
    case hash_e::blake2s:
      return alg_blake2s;
    case hash_e::blake2b:
      return alg_blake2b;
    case hash_e::ripemd160:
      return alg_ripemd160;
    case hash_e::none:
      break;
  }
  return alg_nohash;
}

// ----------------------------------------- hash_t ----------------------------------------

hash_t::hash_t(hash_e type) : alg(hash_alg_t::get(type)) {}

void hash_t::free() {
  if (ctx_ptr) ::EVP_MD_CTX_free(ctx_ptr);
  ctx_ptr = nullptr;
}

hash_t &hash_t::init() {
  if (!ctx_ptr) ctx_ptr = ::EVP_MD_CTX_new();
  ::EVP_DigestInit(ctx_ptr, alg.md);
  return *this;
}

hash_t &hash_t::update(const_byte_ptr ptr, int size) {
  ::EVP_DigestUpdate(ctx_ptr, ptr, size);
  return *this;
}

void hash_t::final(byte_ptr out) { ::EVP_DigestFinal(ctx_ptr, out, NULL); }

void hash_t::copy_state(hash_t &dst) { EVP_MD_CTX_copy(dst.ctx_ptr, ctx_ptr); }

buf_t hash_t::final() {
  buf_t out(alg.size);
  final(out.data());
  return out;
}

hmac_t::hmac_t(hash_e type) : alg(hash_alg_t::get(type)) {}

hmac_t::~hmac_t() {
  if (ctx_ptr) {
    EVP_MAC_CTX_free(ctx_ptr);
  }
}

buf_t hmac_t::final() {
  buf_t out(alg.size);
  final(out.data());
  return out;
}

hmac_t &hmac_t::init(mem_t key) {
  if (!ctx_ptr) {
    EVP_MAC *mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
    ctx_ptr = EVP_MAC_CTX_new(mac);
    EVP_MAC_free(mac);
  }
  OSSL_PARAM params[2];
  params[0] = OSSL_PARAM_construct_utf8_string("digest", (char *)EVP_MD_name(alg.md), 0);
  params[1] = OSSL_PARAM_construct_end();
  EVP_MAC_init(ctx_ptr, key.data, key.size, params);
  return *this;
}

hmac_t &hmac_t::update(const_byte_ptr ptr, int size) {
  EVP_MAC_update(ctx_ptr, ptr, size);
  return *this;
}

void hmac_t::final(byte_ptr out) {
  size_t outl;
  EVP_MAC_final(ctx_ptr, out, &outl, alg.size);
  EVP_MAC_CTX_free(ctx_ptr);
  ctx_ptr = nullptr;
}

void hmac_t::copy_state(hmac_t &dst) {
  if (dst.ctx_ptr) EVP_MAC_CTX_free(dst.ctx_ptr);
  dst.ctx_ptr = EVP_MAC_CTX_dup(ctx_ptr);
}

// -------------------------- SHA256 -----------------------------------

// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf 4.2.2 SHA-224 and SHA-256 Constants
const uint32_t sha256_k[64] = {  // UL = uint32
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

// -------------------------- SHA512 -----------------------------------

// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf 4.2.3 SHA-384, SHA-512, SHA-512/224 and SHA-512/256
// Constants
const uint64_t sha512_k[80] = {  // ULL = uint64
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538,
    0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe,
    0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
    0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab,
    0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
    0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed,
    0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
    0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
    0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373,
    0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c,
    0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6,
    0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
    0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817};

buf_t pbkdf2(hash_e type, mem_t password, mem_t salt, int iter, int out_size) {
  buf_t out(out_size);
  PKCS5_PBKDF2_HMAC(const_char_ptr(password.data), password.size, salt.data, salt.size, iter, hash_alg_t::get(type).md,
                    out_size, out.data());
  return out;
}

}  // namespace coinbase::crypto
// NOLINTEND(*magic-number*)
