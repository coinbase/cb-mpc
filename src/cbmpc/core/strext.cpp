#include <cbmpc/core/buf.h>
#include <cbmpc/core/macros.h>
#include <cbmpc/core/strext.h>

namespace coinbase {
size_t insensitive_hasher_t::operator()(const std::string& key) const {
  size_t hash = 2166136261;
  const_char_ptr s = key.c_str();
  while (*s) {
    hash *= 16777619;
    hash ^= byte_t(tolower(*s++));
  }
  return hash;
}

bool insensitive_equ_t::operator()(const std::string& left, const std::string& right) const {
  return strext::equal_nocase(left, right);
}

}  // namespace coinbase

int strext::compare_nocase(const_char_ptr str1, const std::string& str2) {
  if (!str1) str1 = "";
  return strcasecmp(str1, str2.c_str());
}

int strext::compare_nocase(const std::string& str1, const_char_ptr str2) {
  if (!str2) str2 = "";
  return strcasecmp(str1.c_str(), str2);
}

int strext::compare_nocase(const std::string& str1, const std::string& str2) {
  return strcasecmp(str1.c_str(), str2.c_str());
}

int strext::find_nocase(const std::string& str, const_char_ptr what) {
  const_char_ptr s = str.c_str();
  const_char_ptr w = strcasestr(str.c_str(), what);
  if (!w) return -1;
  return (int)(w - s);
}

int strext::find_nocase(const std::string& str, char what) {
  char temp[] = {what, 0};
  return find_nocase(str, temp);
}

int strext::rfind_nocase(const std::string& str, char what) {
  int u = (int)str.rfind((char)toupper(what));
  int l = (int)str.rfind((char)toupper(what));
  return std::max(u, l);
}

// DT part of the AIX log fix
std::vector<std::string> strext::split_to_words(const std::string& str) {
  std::string buf;                  // Have a buffer string
  std::stringstream ss(str);        // Insert the string into a stream
  std::vector<std::string> tokens;  // Create vector to hold our words

  while (ss >> buf) tokens.push_back(buf);

  return tokens;
}

std::vector<std::string> strext::tokenize(const std::string& str, const std::string& delim) {  // static
  std::vector<std::string> out;
  buf_t buf(const_byte_ptr(str.c_str()), int(str.length()) + 1);
  char_ptr dup = char_ptr(buf.data());
  char* save = nullptr;
  const_char_ptr token = strtok_r(dup, delim.c_str(), &save);

  while (token) {
    std::string t = token;
    trim(t);
    out.push_back(t);
    token = strtok_r(NULL, delim.c_str(), &save);
  }

  return out;
}

std::string strext::from_char_ptr(const_char_ptr ptr) {
  if (!ptr) return "";
  return ptr;
}

void strext::make_upper(std::string& str) { std::transform(str.begin(), str.end(), str.begin(), ::toupper); }
void strext::make_lower(std::string& str) { std::transform(str.begin(), str.end(), str.begin(), ::tolower); }

std::string strext::to_upper(const std::string& str) {
  std::string dst = str;
  make_upper(dst);
  return dst;
}

std::string strext::to_lower(const std::string& str) {
  std::string dst = str;
  make_lower(dst);
  return dst;
}

int strext::scan_hex_byte(const_char_ptr str) {
  unsigned result = 0;
  for (int i = 0; i < 2; i++) {
    unsigned x = 0;
    char c = *str++;
    if (c >= '0' && c <= '9')
      x = c - '0';
    else if (c >= 'a' && c <= 'f')
      x = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
      x = c - 'A' + 10;
    else
      return -1;
    result <<= 4;
    result |= x;
  }
  return result;
}

void strext::print_hex_byte(char_ptr str, uint8_t value) {
  const char hex[] = "0123456789abcdef";
  *str++ = hex[value >> 4];
  *str++ = hex[value & 15];
}

std::string strext::to_hex(mem_t mem) {
  std::string out(mem.size * 2, char(0));
  char_ptr s = buffer(out);
  for (int i = 0; i < mem.size; i++, s += 2) print_hex_byte(s, mem.data[i]);
  return out;
}

static std::string print_hex(uint64_t src, int dst_size) {
  std::string out(dst_size * 2, char(0));
  char_ptr s = strext::buffer(out) + dst_size * 2 - 2;
  for (int i = 0; i < dst_size; i++, s -= 2) strext::print_hex_byte(s, uint8_t(src >> (i * 8)));
  return out;
}

std::string strext::to_hex(uint8_t src) { return print_hex(src, 1); }

std::string strext::to_hex(uint16_t src) { return print_hex(src, 2); }

std::string strext::to_hex(uint32_t src) { return print_hex(src, 4); }

std::string strext::to_hex(uint64_t src) { return print_hex(src, 8); }

bool strext::from_hex(buf_t& dst, const std::string& src) {
  int length = (int)src.length();
  if (length & 1) return false;
  int dst_size = length / 2;
  const_char_ptr hex = src.c_str();
  byte_ptr d = dst.alloc(dst_size);

  for (int i = 0; i < dst_size; i++, hex += 2) {
    int v = strext::scan_hex_byte(hex);
    if (v < 0) return false;
    *d++ = v;
  }
  return true;
}

static bool scan_hex_bytes(uint64_t& dst, const std::string& src, int dst_size) {
  int length = (int)src.length();
  if (length < dst_size * 2) return false;
  const_char_ptr hex = src.c_str();
  uint64_t result = 0;
  for (int i = 0; i < dst_size; i++, hex += 2) {
    int v = strext::scan_hex_byte(hex);
    if (v < 0) return false;
    result = (result << 8) | v;
  }
  dst = result;
  return true;
}

bool strext::from_hex(uint8_t& dst, const std::string& src) {
  uint64_t v = 0;
  if (!scan_hex_bytes(v, src, 1)) return false;
  dst = uint8_t(v);
  return true;
}

bool strext::from_hex(uint16_t& dst, const std::string& src) {
  uint64_t v = 0;
  if (!scan_hex_bytes(v, src, 2)) return false;
  dst = uint16_t(v);
  return true;
}

bool strext::from_hex(uint32_t& dst, const std::string& src) {
  uint64_t v = 0;
  if (!scan_hex_bytes(v, src, 4)) return false;
  dst = uint32_t(v);
  return true;
}

bool strext::from_hex(uint64_t& dst, const std::string& src) { return scan_hex_bytes(dst, src, 8); }

void strext::trim_left(std::string& str) {
  int n = 0, len = int(str.length());
  const_char_ptr s = str.c_str();
  while (n < len && s[n] <= ' ') n++;
  if (n > 0) str.assign(s + n, len - n);
}

void strext::trim_right(std::string& str) {
  int len = int(str.length());
  int n = len;
  const_char_ptr s = str.c_str();
  while (n > 0 && s[n - 1] <= ' ') n--;
  if (n < len) str.resize(n);
}

std::string strext::utoa(uint64_t value) { return std::to_string(value); }

std::string strext::itoa(int value) { return std::to_string(value); }

bool strext::starts_with(const std::string& str, const std::string& start) {
  return str.length() >= start.length() && 0 == memcmp(str.c_str(), start.c_str(), start.length());
}

bool strext::ends_with(const std::string& str, const std::string& end) {
  return str.length() >= end.length() &&
         0 == memcmp(str.c_str() + str.length() - end.length(), end.c_str(), end.length());
}
