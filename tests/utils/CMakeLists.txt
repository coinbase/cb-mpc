file(GLOB_RECURSE SOURCES "*.cpp")
add_library(cbmpc_test_util STATIC ${SOURCES})
target_include_directories(cbmpc_test_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
link_openssl(cbmpc_test_util)
