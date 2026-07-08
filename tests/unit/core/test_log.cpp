#include <gtest/gtest.h>

#include <cbmpc/core/error.h>
#include <cbmpc/internal/core/log.h>

namespace {

std::vector<std::string> g_captured_logs;

void capture_log(int, const char* str) { g_captured_logs.emplace_back(str ? str : ""); }

struct log_capture_scope_t {
  log_capture_scope_t() {
    previous_log_fun = coinbase::out_log_fun;
    previous_test_mode = coinbase::test_error_storing_mode;
    previous_test_log = coinbase::g_test_log_str;
    g_captured_logs.clear();
    coinbase::out_log_fun = capture_log;
    coinbase::test_error_storing_mode = false;
  }

  ~log_capture_scope_t() {
    coinbase::out_log_fun = previous_log_fun;
    coinbase::test_error_storing_mode = previous_test_mode;
    coinbase::g_test_log_str = previous_test_log;
  }

  coinbase::out_log_str_f previous_log_fun;
  bool previous_test_mode;
  std::string previous_test_log;
};

TEST(CoreLog, StringBufferFormatsValuesAndCanReset) {
  log_string_buf_t ss;
  ss.begin_line();
  ss << "value=" << -12;
  ss.put(", count=");
  ss.put(uint64_t(1234567890123ULL));
  ss.put(", hex=");
  ss.put_hex(uint64_t(0xabcdef));
  ss.put(", hex32=");
  ss.put_hex(0x2a);
  ss.end_line();

  EXPECT_EQ(std::string(ss.get()), "value=-12, count=1234567890123, hex=0xabcdef, hex32=0x2a\n");

  ss.reset();
  EXPECT_STREQ(ss.get(), "");

  int marker = 0;
  ss << static_cast<const void*>(&marker);
  std::string pointer_text = ss.get();
  EXPECT_EQ(pointer_text.rfind("0x", 0), 0u);
  EXPECT_GT(pointer_text.size(), 2u);
}

TEST(CoreLog, StringBufferTruncatesLongMessagesAndKeepsTerminator) {
  log_string_buf_t ss;
  std::string long_message(4096, 'x');
  ss.put(long_message.c_str());

  std::string actual = ss.get();
  EXPECT_EQ(actual.size(), 2047);
  EXPECT_EQ(actual, std::string(2047, 'x'));
}

TEST(CoreLog, LogFramePrintsNestedContextWithParameters) {
  std::string phase = "sign";
  log_frame_t outer("void Outer(int)", log_data_t("round", 2), log_data_t("phase", phase));
  log_frame_t inner("int Inner()", log_data_t("step", 3));

  log_string_buf_t ss;
  inner.print_frames(ss);

  EXPECT_EQ(std::string(ss.get()), "Outer(round=2, phase=sign)\nInner(step=3)\n");
}

TEST(CoreLog, LogFramePrintsFramesWithoutParameters) {
  log_frame_t outer("void Outer()", 0);
  log_frame_t inner("void Inner()");

  log_string_buf_t ss;
  inner.print_frames(ss);

  EXPECT_EQ(std::string(ss.get()), "Outer()\nInner()\n");
}

#ifdef __APPLE__
TEST(CoreLog, LogFramePrintsObjectiveCStyleFunctionNames) {
  log_frame_t frame("-[Signer sign:]", 0);

  log_string_buf_t ss;
  frame.print_frames(ss);

  EXPECT_EQ(std::string(ss.get()), "[Signer sign:]()\n");
}
#endif

TEST(CoreLog, ErrorLogIncludesActiveFrameStack) {
  log_capture_scope_t capture;
  std::string phase = "prepare";
  log_frame_t frame("void Operation()", log_data_t("round", 7), log_data_t("phase", phase));

  coinbase::error(E_BADARG, "frame-visible");

  ASSERT_EQ(g_captured_logs.size(), 1);
  EXPECT_NE(g_captured_logs[0].find("Operation(round=7, phase=prepare)\n"), std::string::npos);
  EXPECT_NE(g_captured_logs[0].find("Error code "), std::string::npos);
  EXPECT_NE(g_captured_logs[0].find("frame-visible"), std::string::npos);
}

TEST(CoreLog, DisableScopeSuppressesErrorLogsAndRestoresPreviousState) {
  log_capture_scope_t capture;

  coinbase::error(E_BADARG, "visible before");
  ASSERT_EQ(g_captured_logs.size(), 1);
  EXPECT_NE(g_captured_logs[0].find("visible before"), std::string::npos);

  {
    dylog_disable_scope_t disable_logs;
    coinbase::error(E_BADARG, "hidden");
  }
  EXPECT_EQ(g_captured_logs.size(), 1);

  coinbase::error(E_BADARG, "visible after");
  ASSERT_EQ(g_captured_logs.size(), 2);
  EXPECT_NE(g_captured_logs[1].find("visible after"), std::string::npos);
}

TEST(CoreLog, EnabledDisableScopeLeavesLoggingEnabled) {
  log_capture_scope_t capture;

  {
    dylog_disable_scope_t keep_logs_enabled(true);
    coinbase::error(E_BADARG, "still visible");
  }

  ASSERT_EQ(g_captured_logs.size(), 1);
  EXPECT_NE(g_captured_logs[0].find("still visible"), std::string::npos);
}

}  // namespace
