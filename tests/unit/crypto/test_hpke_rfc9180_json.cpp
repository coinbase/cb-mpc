#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/base_pki.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

static bool read_file_to_string(const std::string& path, std::string& out) {
  std::ifstream f(path);
  if (!f.is_open()) return false;
  std::ostringstream ss;
  ss << f.rdbuf();
  out = ss.str();
  return true;
}

static bool find_hex_field(const std::string& js, const std::string& key, std::string& hex_out) {
  const std::string needle = '"' + key + '"';
  size_t p = js.find(needle);
  if (p == std::string::npos) return false;
  p = js.find(':', p);
  if (p == std::string::npos) return false;
  p = js.find('"', p);
  if (p == std::string::npos) return false;
  size_t q = js.find('"', p + 1);
  if (q == std::string::npos) return false;
  hex_out.assign(js.begin() + p + 1, js.begin() + q);
  return true;
}

static buf_t from_hex(const std::string& hex) {
  size_t n = hex.size();
  if ((n % 2) != 0) {
    ADD_FAILURE() << "Hex string has odd length";
    return buf_t();
  }
  buf_t out(int(n / 2));
  for (size_t i = 0; i < n; i += 2) {
    unsigned int byte = 0;
    std::stringstream ss;
    ss << std::hex << hex.substr(i, 2);
    ss >> byte;
    out[int(i / 2)] = static_cast<uint8_t>(byte);
  }
  return out;
}

TEST(HPKE_RFC9180_JSON, DHKEM_P256_KDF_SHA256_KEM_Only) {
  // Try a couple of common locations for vectors
  std::string json;
  if (!read_file_to_string("tests/data/hpke-vectors.json", json) &&
      !read_file_to_string("tests/data/hpke_vectors.json", json)) {
    GTEST_SKIP() << "HPKE JSON vectors not found (tests/data/hpke-vectors.json)";
  }

  // Look for an object with: mode=0, kem_id=16 (0x0010), kdf_id=1 (HKDF-SHA256)
  bool found = false;
  for (size_t pos = json.find("\"kem_id\":16"); pos != std::string::npos; pos = json.find("\"kem_id\":16", pos + 1)) {
    // Try to bound this object by nearest braces around kem_id occurrence
    size_t start = json.rfind('{', pos);
    size_t end = json.find('}', pos);
    if (start == std::string::npos || end == std::string::npos || end <= start) continue;
    std::string block = json.substr(start, end - start + 1);
    if (block.find("\"mode\":0") == std::string::npos && block.find("\"mode\": 0") == std::string::npos) continue;
    if (block.find("\"kdf_id\":1") == std::string::npos && block.find("\"kdf_id\": 1") == std::string::npos) continue;
    {
      // Extract fields: skRm, pkRm, skEm, enc, shared_secret
      std::string skRm_hex, pkRm_hex, skEm_hex, enc_hex, ss_hex;
      ASSERT_TRUE(find_hex_field(block, "skRm", skRm_hex));
      ASSERT_TRUE(find_hex_field(block, "pkRm", pkRm_hex));
      ASSERT_TRUE(find_hex_field(block, "skEm", skEm_hex));
      ASSERT_TRUE(find_hex_field(block, "enc", enc_hex));
      ASSERT_TRUE(find_hex_field(block, "shared_secret", ss_hex));

      buf_t skRm_b = from_hex(skRm_hex);
      buf_t skEm_b = from_hex(skEm_hex);
      buf_t pkRm_b = from_hex(pkRm_hex);
      buf_t enc_b = from_hex(enc_hex);
      buf_t ss_b = from_hex(ss_hex);

      // Only accept uncompressed P-256 points
      if (pkRm_b.size() != 65 || pkRm_b[0] != 0x04) continue;
      if (enc_b.size() != 65 || enc_b[0] != 0x04) continue;

      // Build recipient keys
      ecc_prv_key_t skR;
      skR.set(curve_p256, bn_t::from_bin(skRm_b));
      ecc_pub_key_t pkR;
      {
        ecc_point_t P;
        ASSERT_OK(P.from_oct(curve_p256, pkRm_b));
        pkR = ecc_pub_key_t(P);
      }

      // Check enc = skEm * G
      {
        ecc_point_t E = bn_t::from_bin(skEm_b) * curve_p256.generator();
        buf_t enc2 = E.to_oct();
        EXPECT_EQ(enc2, enc_b);
      }

      // Decapsulation should reproduce shared_secret
      buf_t ss2;
      ASSERT_OK(kem_policy_ecdh_p256_t::decapsulate(skR, enc_b, ss2));
      EXPECT_EQ(ss2, ss_b);

      found = true;
      break;
    }
  }

  if (!found) {
    GTEST_SKIP() << "No matching DHKEM(P-256,HKDF-SHA256) vector found in JSON";
  }
}

}  // namespace
