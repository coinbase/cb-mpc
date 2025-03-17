#include "error.h"

#include <cbmpc/core/log.h>
#include <cbmpc/core/macros.h>
#include <cbmpc/core/strext.h>

#if !defined(_DEBUG)
// #define JSON_ERR
#endif

typedef log_frame_t* log_frame_ptr_t;

static thread_local log_frame_ptr_t thread_local_storage_log_frame = nullptr;

static thread_local int thread_local_storage_log_disabled = 0;

namespace coinbase {

// Define the global test mode flag here (default false)
bool test_error_storing_mode = false;

out_log_str_f out_log_fun = nullptr;
std::string g_test_log_str = "";
out_log_str_f test_log_fun = [](int mode, const char* str) { g_test_log_str += "; " + std::string(str); };

#define LogItemError 6
void out_error(const std::string& s) {
  if (out_log_fun) {
    out_log_fun(LogItemError, s.c_str());
    return;
  }
  std::cerr << s;
}

error_t error(error_t rv, int category, const std::string& text, bool to_print_stack_trace) {
#if !defined(DY_NO_LOG)
  if (!thread_local_storage_log_disabled && category != ECATEGORY_CONTROL_FLOW) {
    if (to_print_stack_trace) print_stack_trace();

    if (test_error_storing_mode) {
      test_log_fun(0, text.c_str());
    }

    log_string_buf_t ss;
    if (thread_local_storage_log_frame) {
      thread_local_storage_log_frame->print_frames(ss);
    }

    ss.begin_line();
    ss.put("Error ");
    ss.put_hex(rv);
    if (!text.empty()) {
      ss.put(": ");
      ss.put(text.c_str());
    }
    ss.end_line();
    out_error(ss.get());
  }

#endif

  return rv;
}

error_t error(error_t rv, const std::string& text, bool to_print_stack_trace) {
  return error(rv, (rv >> 16) & 0x0f, text, to_print_stack_trace);
}

error_t error(error_t rv, const std::string& text) { return error(rv, text, true); }

error_t error(error_t rv) { return error(rv, ""); }

struct BacktraceState {
  void** current;
  void** end;
};

static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg) {
  BacktraceState* state = static_cast<BacktraceState*>(arg);
  uintptr_t pc = _Unwind_GetIP(context);
  if (pc) {
    if (state->current == state->end)
      return _URC_END_OF_STACK;
    else
      *state->current++ = void_ptr(pc);
  }
  return _URC_NO_REASON;
}

static void str_replace_with_smaller(char* string, const char* substr, const char* replacement) {
  int len = int(strlen(string));
  int substr_len = int(strlen(substr));
  int replacement_len = int(strlen(replacement));
  char* tok = NULL;

  while (tok = strstr(string, substr)) {
    memmove(tok, replacement, replacement_len);
    memmove(tok + replacement_len, tok + substr_len, len - (tok - string) + 1);
    len -= substr_len - replacement_len;
  }
}

static void purify_cpp_symbol(char* symbol) {
  if (!symbol) return;
  str_replace_with_smaller(
      symbol, "std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>", "string");
  str_replace_with_smaller(symbol, "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >",
                           "string");

  str_replace_with_smaller(symbol, "coinbase::", "cb::");
  str_replace_with_smaller(symbol, "cb::mpc", "mpc");
  str_replace_with_smaller(symbol, "cb::zk", "zk");
  str_replace_with_smaller(symbol, "cb::buf_t", "buf_t");
  str_replace_with_smaller(symbol, "cb::mem_t", "mem_t");
  str_replace_with_smaller(symbol, "cb::crypto::bn_t", "bn_t");
  str_replace_with_smaller(symbol, "cb::crypto::mod_t", "mod_t");
  str_replace_with_smaller(symbol, "cb::crypto::ecc_point_t", "ecc_point_t");
  str_replace_with_smaller(symbol, "cb::crypto::paillier_t", "paillier_t");
  str_replace_with_smaller(symbol, "std::__1::map", "map");
  str_replace_with_smaller(symbol, "std::__1::pair", "pair");
  str_replace_with_smaller(symbol, "std::__1::tuple", "tuple");
  str_replace_with_smaller(symbol, "std::__1::vector", "vector");

  str_replace_with_smaller(symbol, "std::__1::allocator", "alloc");
  str_replace_with_smaller(symbol, "std::allocator", "alloc");
  str_replace_with_smaller(symbol, "std::__1", "std::");
}

#ifdef __linux__
class symbols_t {
 public:
  void load() {
    if (sym) return;
    Dl_info info = {0};
    dladdr((const void*)&unwindCallback, &info);

    int fd = open(info.dli_fname, O_RDONLY);
    struct stat statbuf;
    int err = fstat(fd, &statbuf);
    sym_base = (const uint8_t*)mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
    const Elf64_Ehdr* elf_hdr = (const Elf64_Ehdr*)sym_base;

    const Elf64_Shdr* first_section = (const Elf64_Shdr*)(sym_base + elf_hdr->e_shoff);
    for (int i = 0; i < elf_hdr->e_shnum; i++) {
      const Elf64_Shdr* section = first_section + i;
      if (section->sh_type == SHT_SYMTAB) {
        sym = (const Elf64_Sym*)(sym_base + section->sh_offset);
        sym_cnt = section->sh_size / sizeof(Elf64_Sym);
        strings = (char*)(sym_base + first_section[section->sh_link].sh_offset);
        break;
      }
    }
    void* executable_handle = dlopen(0, RTLD_LAZY);
    struct link_map* map = nullptr;
    dlinfo(executable_handle, RTLD_DI_LINKMAP, &map);
    offset = map->l_addr;
  }
  const char* find(const void* addr) {
    for (auto sym_index = 0; sym_index < sym_cnt; sym_index++) {
      const auto& s = sym[sym_index];
      auto val = offset + s.st_value;
      auto end = val + s.st_size;
      if (val <= uint64_t(addr) && end > uint64_t(addr)) {
        return strings + s.st_name;
      }
    }
    return nullptr;
  }

 private:
  const uint8_t* sym_base = nullptr;
  const char* strings = nullptr;
  const Elf64_Sym* sym = nullptr;
  int sym_cnt = 0;
  uint64_t offset = 0;
};

static symbols_t symbols;
#endif

static std::string get_func_name_from_full_name(const std::string& full_func_name) {
  size_t len = full_func_name.length();
#ifdef __APPLE__
  // Special handling for Objective-C function format: -[ClassName methodName]
  if (len > 4 && full_func_name[0] == '-' && full_func_name[1] == '[' && full_func_name[len - 1] == ']') {
    // Drop the leading '-' and return everything except the trailing ']'
    return full_func_name.substr(1, len - 2);
  }
#endif

  // Stop at the first '(' if it exists
  size_t e = full_func_name.find('(');
  if (e == std::string::npos) e = len;
  // Take everything up to '('
  std::string temp(full_func_name.c_str(), e);

  // Find the last space; function name is typically after the last space
  auto b = temp.rfind(' ');
  if (b == std::string::npos) {
    return temp;
  }
  // Return everything after the last space
  return std::string(temp.c_str() + b + 1, temp.size() - b - 1);
}

// We'll define a helper function to color only the function name.
static std::string color_func_name(const char* symbol) {
  if (!symbol || !symbol[0]) return "";
  // Convert to std::string.
  std::string symbol_str = symbol;
  // Extract just the function name portion (the logic is in get_func_name_from_full_name).
  std::string func_name = get_func_name_from_full_name(symbol_str);
  if (func_name.empty()) {
    return symbol_str;  // Nothing to color if the function name is empty.
  }

  // Build the colored function name.
  std::string colored_name = "\x1B[33m" + func_name + "\x1B[0m";

  // Replace the first occurrence of func_name with the colored version.
  size_t pos = symbol_str.find(func_name);
  if (pos != std::string::npos) {
    symbol_str.replace(pos, func_name.size(), colored_name);
  }

  return symbol_str;
}

void print_stack_trace() {
#ifdef __linux__
  symbols.load();
#endif

  void* buffer[64];
  BacktraceState state = {buffer, buffer + 64};
  _Unwind_Backtrace(unwindCallback, &state);
  int count = state.current - buffer;
  for (int idx = 0; idx < count; ++idx) {
    const void* addr = buffer[idx];
    const char* symbol = "";
    const char* module = "";
    char name_buf[2048];

    Dl_info info = {0};
    dladdr(addr, &info);
    symbol = info.dli_sname;

#ifdef __linux__
    if (!symbol || symbol[0] == 0) symbol = symbols.find(addr);
#endif

    if (symbol && symbol[0]) {
      size_t len = sizeof(name_buf);
      int status = 0;
      const char* cpp_symbol = abi::__cxa_demangle(symbol, name_buf, &len, &status);
      if (cpp_symbol) {
        purify_cpp_symbol(name_buf);
        symbol = name_buf;
      }
    }
    if (!symbol) symbol = "";

    if (info.dli_fname) {
      module = strrchr(info.dli_fname, '/');
      if (module)
        module++;
      else
        module = info.dli_fname;
    }

    // Use our new helper to color only the function name portion:
    std::string final_symbol = color_func_name(symbol);

    log_string_buf_t ss;
    ss.begin_line();
    ss << "##" << idx << " " << module << " " << addr << " " << final_symbol.c_str();
    ss.end_line();
    out_error(ss.get());
  }
}

void assert_failed(const char* msg, const char* file, int line) {
  if (!thread_local_storage_log_disabled) {
    log_string_buf_t ss;
#if defined(JSON_ERR)
    // JSON mode uses multiple lines
    ss.begin_line();
    ss << "[ASSERTION FAILED] " << msg;
    ss.end_line();

    ss.begin_line();
    ss << "File: " << file;
    ss.end_line();

    ss.begin_line();
    ss << "Line: " << line;
    ss.end_line();
#else
    // Plain text mode uses a single line
    // First strip out everything before "src/" so the file path is relative
    std::string relativeFile(file);
    auto pos = relativeFile.find("src/");
    if (pos != std::string::npos) {
      relativeFile.erase(0, pos);
    }
    ss.begin_line();
    ss << "[ASSERTION FAILED] "
       << "\x1B[1;33m" << msg << "\x1B[0m"
       << " (File: " << relativeFile.c_str() << "#L" << line << ")";
    ss.end_line();
#endif
    out_error(ss.get());
  }

  if (!thread_local_storage_log_disabled) {
    print_stack_trace();
  }
  throw assertion_failed_t(msg);
}

}  // namespace coinbase

static void get_function_name_from_full_name(const std::string& full_func_name, char* out) {
  out[0] = 0;
  size_t len = full_func_name.length();
  const_char_ptr begin = full_func_name.c_str();
  const_char_ptr end = begin + len;

#ifdef __APPLE__
  if (len > 4 && full_func_name[0] == '-' && full_func_name[1] == '[' && full_func_name[len - 1] == ']')  // obj-C
  {
    begin++;
  } else
#endif
  {
    size_t ef = full_func_name.find('(');
    if (ef != std::string::npos) end = begin + ef;
    const_char_ptr e = end;
    while (e > begin && e[-1] != ' ') e--;
    begin = e;  // if (e != begin) begin = e;
  }

  len = end - begin;
  if (len > 255) len = 255;
  memmove(out, begin, len);
  out[len] = 0;
}

void log_frame_t::print(log_string_buf_t& ss) const {
  char function_name[256];
  get_function_name_from_full_name(func_name, function_name);
  ss.put(function_name);

  ss.put("(");
  for (int i = 0; i < params_count; i++) {
    if (i > 0) ss.put(", ");
    params[i].print(ss);
  }
  ss.put(")");
}

void log_frame_t::print_frames(log_string_buf_t& ss) const {
  const log_frame_t* f = this;
  std::vector<const log_frame_t*> frames;
  while (f) {
    frames.push_back(f);
    f = f->up;
  }

  for (int i = (int)frames.size() - 1; i >= 0; i--) {
    ss.begin_line();
    frames[i]->print(ss);
    ss.end_line();
  }
}

void log_frame_t::init_thread_local_storage() {
  up = thread_local_storage_log_frame;
  thread_local_storage_log_frame = this;
}

log_frame_t::~log_frame_t() { thread_local_storage_log_frame = up; }

dylog_disable_scope_t::dylog_disable_scope_t(bool enabled) {
  ref_counter = thread_local_storage_log_disabled;
  if (!enabled) thread_local_storage_log_disabled++;
}
dylog_disable_scope_t::~dylog_disable_scope_t() { thread_local_storage_log_disabled = ref_counter; }
void disable_thread_local_storage_log() { thread_local_storage_log_disabled = 1; }

void log_string_buf_t::put(const_char_ptr ptr) {
  int len = strlen(ptr);
  if (size + len > buf_size) len = buf_size - size;
  memmove(buffer + size - 1, ptr, len);
  size += len;
  buffer[size - 1] = 0;
}

void log_data_t::print(log_string_buf_t& ss) const {
  ss.put(name);
  ss.put("=");

  switch (flags) {
    case log_int:
      ss.put(int(data));
      break;
    case log_long:
      ss.put(uint64_t(data));
      break;
    case log_ptr:
      ss.put(data ? 1 : 0);
      break;
    case log_string:
      ss.put((*(std::string*)data).c_str());
      break;
  }
}

#include <charconv>

void log_string_buf_t::put(int value) {
  char buf[32] = {0};
  std::to_chars(buf, buf + 31, value);
  put(buf);
}

void log_string_buf_t::put_hex(int value) {
  char buf[32] = {0};
  std::to_chars(buf, buf + 31, uint32_t(value), 16);
  put("0x");
  put(buf);
}

void log_string_buf_t::put_hex(uint64_t value) {
  char buf[32] = {0};
  std::to_chars(buf, buf + 31, value, 16);
  put("0x");
  put(buf);
}

void log_string_buf_t::put(uint64_t value) {
  char buf[32] = {0};
  std::to_chars(buf, buf + 31, value);
  put(buf);
}

void log_string_buf_t::begin_line() {
#if defined(JSON_ERR)
  double t = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
  put("{\"level\":\"error\",\"ts\":");

  char buf[32] = {0};
  snprintf(buf, 31, "%.6f", t);

  put(buf);
  put(",\"msg\":\"");
#else
  // Plain text mode: do nothing special here
#endif
}

void log_string_buf_t::end_line() {
#if defined(JSON_ERR)
  put("\"}");
#endif
  put("\n");
}
