#pragma once

#define ASSERT_OK(rv) ASSERT_EQ(rv, 0)
#define EXPECT_OK(rv) EXPECT_EQ(rv, 0)
#define ASSERT_ER(er) ASSERT_NE(er, 0)
#define EXPECT_ER(er) EXPECT_NE(er, 0)
#define ASSERT_ER_MSG(er, msg) \
  ASSERT_ER(er);               \
  ASSERT_NE(std::string::npos, coinbase::g_test_log_str.find(msg))
#define EXPECT_ER_MSG(er, msg)                 \
  coinbase::set_test_error_storing_mode(true); \
  EXPECT_ER(er);                               \
  EXPECT_NE(std::string::npos, coinbase::g_test_log_str.find(msg))

#define ASSERT_THROW_MSG(statement, expected_exception, expected_what)                      \
  try {                                                                                     \
    statement;                                                                              \
    FAIL() << "Expected: " #statement " throws an exception of type " #expected_exception   \
              ".\n"                                                                         \
              "  Actual: it throws nothing.";                                               \
  } catch (const expected_exception& e) {                                                   \
    if (strstr(e.what(), expected_what) == static_cast<const char*>(NULL))                  \
      FAIL() << "Exception message is incorrect. Expected it to contain '" << expected_what \
             << "', whereas the text is '" << e.what() << "'.\n";                           \
  } catch (...) {                                                                           \
    FAIL() << "Expected: " #statement " throws an exception of type " #expected_exception   \
              ".\n"                                                                         \
              "  Actual: it throws a different type.";                                      \
  }

#define ASSERT_CB_ASSERT(statement, msg) ASSERT_THROW_MSG(statement, coinbase::assertion_failed_t, msg)

#define EXPECT_THROW_MSG(statement, expected_exception, expected_what)                             \
  try {                                                                                            \
    statement;                                                                                     \
    ADD_FAILURE() << "Expected: " #statement " throws an exception of type " #expected_exception   \
                     ".\n"                                                                         \
                     "  Actual: it throws nothing.";                                               \
  } catch (const expected_exception& e) {                                                          \
    if (strstr(e.what(), expected_what) == static_cast<const char*>(NULL))                         \
      ADD_FAILURE() << "Exception message is incorrect. Expected it to contain '" << expected_what \
                    << "', whereas the text is '" << e.what() << "'.\n";                           \
  } catch (...) {                                                                                  \
    ADD_FAILURE() << "Expected: " #statement " throws an exception of type " #expected_exception   \
                     ".\n"                                                                         \
                     "  Actual: it throws a different type.";                                      \
  }

#define EXPECT_CB_ASSERT(statement, msg) EXPECT_THROW_MSG(statement, coinbase::assertion_failed_t, msg)
