cmake_minimum_required(VERSION 3.16)

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

# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)

enable_testing()

include(GoogleTest)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_subdirectory(utils)
add_subdirectory(unit)


if(BUILD_DUDECT)
  Message("Building dudect tests")
  add_subdirectory(dudect)
endif()
