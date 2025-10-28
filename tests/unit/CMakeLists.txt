file(GLOB UNIT_TEST_SOURCES "*/test_*.cpp")
add_executable(cbmpc_test_unit ${UNIT_TEST_SOURCES})
target_link_libraries(cbmpc_test_unit PRIVATE gtest_main cbmpc cbmpc_test_util)
gtest_discover_tests(cbmpc_test_unit PROPERTIES LABELS "unit" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
