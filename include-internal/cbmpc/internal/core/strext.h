#pragma once

#include <cbmpc/core/buf.h>

struct strext {
  static coinbase::mem_t mem(const std::string& s) {
    return coinbase::mem_t((const_byte_ptr)s.c_str(), (int)s.length());
  }

  static std::vector<std::string> tokenize(const std::string& str, const std::string& delim = " ");

  static std::string itoa(int value);
  static std::string to_hex(coinbase::mem_t hex);
  static std::string to_hex(uint8_t src);
  static std::string to_hex(uint16_t src);
  static std::string to_hex(uint32_t src);
  static std::string to_hex(uint64_t src);
  static bool from_hex(coinbase::buf_t& dst, const std::string& src);
  static bool from_hex(uint8_t& dst, const std::string& src);
  static bool from_hex(uint16_t& dst, const std::string& src);
  static bool from_hex(uint32_t& dst, const std::string& src);
  static bool from_hex(uint64_t& dst, const std::string& src);
};
