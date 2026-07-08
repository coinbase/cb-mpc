#include <cbmpc/core/buf.h>
#include <cbmpc/core/macros.h>
#include <cbmpc/internal/core/strext.h>

namespace {
void trim_left(std::string& str) {
  int n = 0, len = int(str.length());
  const_char_ptr s = str.c_str();
  while (n < len && s[n] <= ' ') n++;
  if (n > 0) str.assign(s + n, len - n);
}

void trim_right(std::string& str) {
  int len = int(str.length());
  int n = len;
  const_char_ptr s = str.c_str();
  while (n > 0 && s[n - 1] <= ' ') n--;
  if (n < len) str.resize(n);
}

void trim(std::string& str) {
  trim_left(str);
  trim_right(str);
}

int scan_hex_byte(const_char_ptr str) {
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

void print_hex_byte(char_ptr str, uint8_t value) {
  const char hex[] = "0123456789abcdef";
  *str++ = hex[value >> 4];
  *str++ = hex[value & 15];
}

}  // namespace

std::vector<std::string> strext::tokenize(const std::string& str, const std::string& delim) {  // static
  std::vector<std::string> out;
  coinbase::buf_t buf(const_byte_ptr(str.c_str()), int(str.length()) + 1);
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

std::string strext::to_hex(coinbase::mem_t mem) {
  if (mem.size <= 0 || !mem.data) return "";

  const size_t n = static_cast<size_t>(mem.size);
  std::string out(n * 2, char(0));
  char_ptr s = out.data();
  for (int i = 0; i < mem.size; i++, s += 2) print_hex_byte(s, mem.data[i]);
  return out;
}

static std::string print_hex(uint64_t src, int dst_size) {
  std::string out(dst_size * 2, char(0));
  char_ptr s = out.data() + dst_size * 2 - 2;
  for (int i = 0; i < dst_size; i++, s -= 2) print_hex_byte(s, uint8_t(src >> (i * 8)));
  return out;
}

std::string strext::to_hex(uint8_t src) { return print_hex(src, 1); }

std::string strext::to_hex(uint16_t src) { return print_hex(src, 2); }

std::string strext::to_hex(uint32_t src) { return print_hex(src, 4); }

std::string strext::to_hex(uint64_t src) { return print_hex(src, 8); }

bool strext::from_hex(coinbase::buf_t& dst, const std::string& src) {
  int length = (int)src.length();
  if (length & 1) return false;
  int dst_size = length / 2;
  const_char_ptr hex = src.c_str();
  byte_ptr d = dst.alloc(dst_size);

  for (int i = 0; i < dst_size; i++, hex += 2) {
    int v = scan_hex_byte(hex);
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
    int v = scan_hex_byte(hex);
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

std::string strext::itoa(int value) { return std::to_string(value); }
