#include <gtest/gtest.h>
#include <string>
#include <vector>

#include <cbmpc/core/error.h>

#include "utils/test_macros.h"

namespace {

using coinbase::error_t;

std::vector<std::string> g_captured_logs;

void capture_log(int, const char* str) { g_captured_logs.emplace_back(str ? str : ""); }

std::string captured_log_string() {
  std::string out;
  for (const std::string& log : g_captured_logs) out += log;
  return out;
}

struct error_log_capture_scope_t {
  error_log_capture_scope_t() {
    previous_log_fun = coinbase::out_log_fun;
    previous_test_mode = coinbase::test_error_storing_mode;
    previous_test_log = coinbase::g_test_log_str;

    g_captured_logs.clear();
    coinbase::out_log_fun = capture_log;
    coinbase::test_error_storing_mode = false;
  }

  ~error_log_capture_scope_t() {
    coinbase::out_log_fun = previous_log_fun;
    coinbase::test_error_storing_mode = previous_test_mode;
    coinbase::g_test_log_str = previous_test_log;
  }

  coinbase::out_log_str_f previous_log_fun;
  bool previous_test_mode;
  std::string previous_test_log;
};

error_t inner_func() { return coinbase::error(E_BADARG, "inner error msg"); }

error_t outer_func() {
  error_t rv = UNINITIALIZED_ERROR;
  if (rv = inner_func()) return coinbase::error(rv, "outer error msg");
  return SUCCESS;
}

TEST(ErrorTest, TestErrorLogsWithCallback) {
  coinbase::set_test_error_storing_mode(true);

  coinbase::error(E_BADARG, "This is a test of E_BADARG");

  EXPECT_FALSE(coinbase::g_test_log_str.empty());
  EXPECT_NE(std::string::npos, coinbase::g_test_log_str.find("BADARG"));
  EXPECT_NE(std::string::npos, coinbase::g_test_log_str.find("This is a test of E_BADARG"));
}

TEST(ErrorTest, TestErrorNoMessage) {
  coinbase::set_test_error_storing_mode(true);

  coinbase::error(E_CF_MPC_BENCHMARK);
  EXPECT_EQ(coinbase::g_test_log_str, "test error log");
}

TEST(ErrorTest, TestLayeredErrorMsgs) {
  coinbase::set_test_error_storing_mode(true);

  EXPECT_ER_MSG(outer_func(), "inner error msg; outer error msg");
}

TEST(ErrorTest, DefaultErrorDoesNotPrintStackTrace) {
  error_log_capture_scope_t capture;

  coinbase::error(E_BADARG, "malformed protocol message");

  ASSERT_EQ(g_captured_logs.size(), 1);
  std::string logs = captured_log_string();
  EXPECT_NE(std::string::npos, logs.find("malformed protocol message"));
  EXPECT_EQ(std::string::npos, logs.find("Stack trace"));
  EXPECT_EQ(std::string::npos, logs.find("##"));
  EXPECT_EQ(std::string::npos, logs.find("0x"));
}

TEST(ErrorTest, AssertFailureLogIsConciseByDefault) {
  error_log_capture_scope_t capture;

  try {
    cb_assert(false && "assert log");
    FAIL() << "cb_assert should throw";
  } catch (const coinbase::assertion_failed_t&) {
  }

  ASSERT_EQ(g_captured_logs.size(), 1);
  std::string logs = captured_log_string();
  EXPECT_NE(std::string::npos, logs.find("Assertion failed:"));
  EXPECT_NE(std::string::npos, logs.find("assert log"));
  EXPECT_EQ(std::string::npos, logs.find("Stack trace"));
  EXPECT_EQ(std::string::npos, logs.find("##"));
  EXPECT_EQ(std::string::npos, logs.find("0x"));
  EXPECT_EQ(std::string::npos, logs.find("\x1B"));
  EXPECT_EQ(std::string::npos, logs.find("src/"));
  EXPECT_EQ(std::string::npos, logs.find("#L"));
}

}  // namespace