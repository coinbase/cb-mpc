cmake_minimum_required(VERSION 3.16)

# IMPORTANT: Set gtest options BEFORE adding googletest to the build
# This prevents CRT/ABI mismatches that cause double-free errors under sanitizers
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)

# Disable Google Test's internal use of pthreads if it causes issues
set(gtest_disable_pthreads
    OFF
    CACHE BOOL "" FORCE)

if(USE_FETCH_CONTENT)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.0)
  FetchContent_MakeAvailable(googletest)
else()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendors/googletest
                   ${CMAKE_CURRENT_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)
endif()

# Google Test must be built with default visibility to avoid ODR violations
# when the main project uses -fvisibility=hidden.
# This is critical for sanitizer builds where such violations cause crashes.
if(TARGET gtest)
  set_target_properties(gtest PROPERTIES CXX_VISIBILITY_PRESET default)
  target_compile_options(gtest PRIVATE -fvisibility=default)
endif()
if(TARGET gtest_main)
  set_target_properties(gtest_main PROPERTIES CXX_VISIBILITY_PRESET default)
  target_compile_options(gtest_main PRIVATE -fvisibility=default)
endif()
if(TARGET gmock)
  set_target_properties(gmock PROPERTIES CXX_VISIBILITY_PRESET default)
  target_compile_options(gmock PRIVATE -fvisibility=default)
endif()
if(TARGET gmock_main)
  set_target_properties(gmock_main PROPERTIES CXX_VISIBILITY_PRESET default)
  target_compile_options(gmock_main PRIVATE -fvisibility=default)
endif()

enable_testing()

include(GoogleTest)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_subdirectory(utils)
add_subdirectory(unit)


if(BUILD_DUDECT)
  Message("Building dudect tests")
  add_subdirectory(dudect)
endif()
