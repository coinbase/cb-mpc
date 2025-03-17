#pragma once

#include <cbmpc/core/macros.h>

#define _FUNCION_LOG_FORMAT_ __PRETTY_FUNCTION__

class log_string_buf_t {
 public:
  log_string_buf_t() : size(1) { buffer[0] = 0; }
  void reset() {
    size = 1;
    buffer[0] = 0;
  }
  const_char_ptr get() const { return buffer; }
  void put(const_char_ptr ptr);
  void put(int value);
  void put(uint64_t value);
  void put_hex(int value);
  void put_hex(uint64_t value);
  void begin_line();
  void end_line();

  log_string_buf_t& operator<<(const_char_ptr ptr) {
    put(ptr);
    return *this;
  }
  log_string_buf_t& operator<<(int value) {
    put(value);
    return *this;
  }
  log_string_buf_t& operator<<(const void* ptr) {
    put_hex(uint64_t(uintptr_t(ptr)));
    return *this;
  }

 private:
  enum { buf_size = 2048 };
  int size;
  char buffer[buf_size];
};

class log_data_t {
 public:
  template <typename T>
  log_data_t(const char* _name, T param) : name(_name) {
    init(uintptr_t(param));
  }
  log_data_t(const char* _name, const std::string& param) : name(_name) { init(param); }
  log_data_t() {}

  void print(log_string_buf_t& ss) const;

 private:
  enum {
    log_int = 2,
    log_long = 3,
    log_ptr = 4,
    log_string = 5,
  };
  uintptr_t data;
  unsigned flags;
  const char* name;

  void init(uintptr_t param) {
    data = param;
    flags = log_int;
  }

  void init(const std::string& param) {
    data = uintptr_t(&param);
    flags = log_string;
  }
};

class log_frame_t {
 public:
  log_frame_t(const char* _func_name, int) : func_name(_func_name) { init_thread_local_storage(); }

  template <typename... ARGS>
  explicit log_frame_t(const char* _func_name, const ARGS&... args) : func_name(_func_name) {
    init_thread_local_storage();
    init(args...);
  }

  ~log_frame_t();

  void print_frames(log_string_buf_t& ss) const;

 private:
  template <typename FIRST, typename... LAST>
  void init(const FIRST& first, const LAST&... last) {
    init(first);
    init(last...);
  }
  void init(const log_data_t& param) { params[params_count++] = param; }
  void init() {}
  void print(log_string_buf_t& ss) const;
  void init_thread_local_storage();

  enum { max_param = 16 };

  const char* func_name;
  log_frame_t* up;
  int params_count = 0;
  log_data_t params[max_param];
};

#define LOG(x) log_data_t(#x, x)
#define LOGSTR(x) log_data_t(#x, x)

struct dylog_disable_scope_t {
  dylog_disable_scope_t(bool enabled = false);
  ~dylog_disable_scope_t();

 private:
  int ref_counter;
};
