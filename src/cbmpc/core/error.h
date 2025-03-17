#pragma once
#include <cbmpc/core/precompiled.h>

typedef int error_t;

#define ERRCODE(category, code) (0xff000000 | (uint32_t(category) << 16) | uint32_t(code))
#define ECATEGORY(code) (((code) >> 16) & 0x00ff)

// clang-format off
enum {
  ECATEGORY_GENERIC      = 0x01,
  ECATEGORY_NETWORK      = 0x03,
  ECATEGORY_CRYPTO       = 0x04,
  ECATEGORY_OPENSSL      = 0x06,
  ECATEGORY_CONTROL_FLOW = 0x0a,
};

enum {
  SUCCESS = 0,
  UNINITIALIZED_ERROR = ERRCODE(ECATEGORY_GENERIC, 0x0000), // our function should never return this error
  E_GENERAL           = ERRCODE(ECATEGORY_GENERIC, 0x0001),
  E_BADARG            = ERRCODE(ECATEGORY_GENERIC, 0x0002),
  E_FORMAT            = ERRCODE(ECATEGORY_GENERIC, 0x0003),
  E_NOT_SUPPORTED     = ERRCODE(ECATEGORY_GENERIC, 0x0005),
  E_NOT_FOUND         = ERRCODE(ECATEGORY_GENERIC, 0x0006),
  E_INSUFFICIENT      = ERRCODE(ECATEGORY_GENERIC, 0x000c),
  E_RANGE             = ERRCODE(ECATEGORY_GENERIC, 0x0012),

  E_NET_GENERAL      = ERRCODE(ECATEGORY_NETWORK, 0x0001),

  E_CF_MPC_BENCHMARK = ERRCODE(ECATEGORY_CONTROL_FLOW, 0x0001),
};
// clang-format on
namespace coinbase {

error_t error(error_t rv, int category, const std::string& text, bool to_print_stack_trace);
error_t error(error_t rv, const std::string& text, bool to_print_stack_trace);
error_t error(error_t rv, const std::string& text);
error_t error(error_t rv);

typedef void (*out_log_str_f)(int mode, const char* str);
extern out_log_str_f out_log_fun;
extern bool test_error_storing_mode;
extern std::string g_test_log_str;
extern out_log_str_f test_log_fun;

struct error_message_t {
  int category;
  int code;
  std::string message;
};

void print_stack_trace();

void assert_failed(const char* msg, const char* file, int line);

class assertion_failed_t : public std::logic_error {
 public:
  assertion_failed_t(const std::string& msg) : std::logic_error(msg) {}
};

inline void set_test_error_storing_mode(bool enabled) {
  test_error_storing_mode = enabled;
  g_test_log_str = "test error log";
}

}  // namespace coinbase

#define cb_assert(expr)                                                                   \
  do {                                                                                    \
    if (__builtin_expect(!(expr), 0)) coinbase::assert_failed(#expr, __FILE__, __LINE__); \
  } while (0)
