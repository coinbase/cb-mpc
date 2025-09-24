#include <gtest/gtest.h>
#include <string>

#include <cbmpc/core/error.h>

#include "utils/test_macros.h"

namespace {

error_t inner_func() { return coinbase::error(E_BADARG, "inner error msg"); }

error_t outer_func() {
  error_t rv = UNINITIALIZED_ERROR;
  if (rv = inner_func()) return coinbase::error(rv, "outer error msg", false);
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

}  // namespace