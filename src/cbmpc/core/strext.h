#pragma once

#include <cbmpc/core/buf.h>

namespace coinbase {

struct insensitive_hasher_t {
  size_t operator()(const std::string& key) const;
};
struct insensitive_equ_t {
  bool operator()(const std::string& left, const std::string& right) const;
};
template <typename T>
class insensitive_map_t : public unordered_map_t<std::string, T, insensitive_hasher_t, insensitive_equ_t> {};

}  // namespace coinbase

struct strext {
  static char_ptr buffer(std::string& s) { return &s[0]; }
  static mem_t mem(const std::string& s) { return mem_t((const_byte_ptr)s.c_str(), (int)s.length()); }

  static int compare_nocase(const_char_ptr str1, const std::string& str2);
  static int compare_nocase(const std::string& str1, const_char_ptr str2);
  static int compare_nocase(const std::string& str1, const std::string& str2);
  static bool equal_nocase(const_char_ptr str1, const std::string& str2) { return 0 == compare_nocase(str1, str2); }
  static bool equal_nocase(const std::string& str1, const_char_ptr str2) { return 0 == compare_nocase(str1, str2); }
  static bool equal_nocase(const std::string& str1, const std::string& str2) { return 0 == compare_nocase(str1, str2); }

  static int find_nocase(const std::string& str, const std::string& what) { return find_nocase(str, what.c_str()); }
  static int find_nocase(const std::string& str, const_char_ptr what);
  static int find_nocase(const std::string& str, char c);
  static int rfind_nocase(const std::string& str, char c);

  // DT part of the AIX log fix
  static std::vector<std::string> split_to_words(const std::string& str);

  static std::vector<std::string> tokenize(const std::string& str, const std::string& delim = " ");

  static std::string from_char_ptr(const_char_ptr ptr);

  static std::string to_upper(const std::string& str);
  static std::string to_lower(const std::string& str);
  static void make_upper(std::string& str);
  static void make_lower(std::string& str);

  static void trim_left(std::string& str);
  static void trim_right(std::string& str);
  static void trim(std::string& str) {
    trim_left(str);
    trim_right(str);
  }

  static std::string left(const std::string& str, int count) { return str.substr(0, count); }
  static std::string right(const std::string& str, int count) { return str.substr(str.length() - count, count); }

  static bool starts_with(const std::string& str, const std::string& start);
  static bool ends_with(const std::string& str, const std::string& end);

  static std::string utoa(uint64_t value);
  static std::string itoa(int value);
  static int atoi(const std::string& str) { return (int)::strtol(str.c_str(), 0, 10); }
  static double atod(const std::string& str) { return (double)::strtod(str.c_str(), 0); }
  static std::string to_hex(mem_t hex);
  static std::string to_hex(uint8_t src);
  static std::string to_hex(uint16_t src);
  static std::string to_hex(uint32_t src);
  static std::string to_hex(uint64_t src);
  static bool from_hex(buf_t& dst, const std::string& src);
  static bool from_hex(uint8_t& dst, const std::string& src);
  static bool from_hex(uint16_t& dst, const std::string& src);
  static bool from_hex(uint32_t& dst, const std::string& src);
  static bool from_hex(uint64_t& dst, const std::string& src);

  static int scan_hex_byte(const_char_ptr str);
  static void print_hex_byte(char_ptr str, uint8_t value);
};
