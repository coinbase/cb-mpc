file(GLOB TEST_SOURCES "dudect_*.cpp")
add_executable(cbmpc_test_dudect ${TEST_SOURCES})
target_link_libraries(cbmpc_test_dudect PRIVATE gtest_main cbmpc
                                                     cbmpc_test_util cbmpc)
gtest_discover_tests(cbmpc_test_dudect PROPERTIES LABELS "dudect")
