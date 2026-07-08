#include <cbmpc/core/error.h>
#include <cbmpc/core/macros.h>
#include <cbmpc/internal/core/log.h>

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

error_t error(error_t rv, int category, const std::string& text) {
#if !defined(DY_NO_LOG)
  if (!thread_local_storage_log_disabled && category != ECATEGORY_CONTROL_FLOW) {
    if (test_error_storing_mode) {
      test_log_fun(0, text.c_str());
    }

    log_string_buf_t ss;
    if (thread_local_storage_log_frame) {
      thread_local_storage_log_frame->print_frames(ss);
    }

    ss.begin_line();
    ss.put("Error code ");
    ss.put(rv);
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

error_t error(error_t rv, const std::string& text) { return error(rv, (rv >> 16) & 0x0f, text); }

error_t error(error_t rv) { return error(rv, ""); }

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
    (void)file;
    (void)line;
    ss.begin_line();
    ss << "Assertion failed: " << msg;
    ss.end_line();
#endif
    out_error(ss.get());
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

  size_t count = frames.size();
  cb_assert(count <= INT_MAX);
  for (int i = (int)count - 1; i >= 0; i--) {
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
