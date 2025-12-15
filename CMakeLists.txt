cmake_minimum_required(VERSION 3.16)

project(
  CBMPC
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------- Available options ---------------
# * CMAKE_BUILD_TYPE (Release|Debug|RelWithDebInfo)
option(CBMPC_PLATFORM_DEP_OUTPUT_DIR
       "library output directory will be platform-dependent" OFF)

option(BUILD_TESTS "Also build the unit and integration tests" ON)
option(BUILD_DUDECT "Also build the dudect tests" OFF)


# ------------- Setup pathes --------------------

set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SRC_DIR ${ROOT_DIR}/src/)
set(LIB_DIR ${ROOT_DIR}/lib/)
set(TEST_DIR ${ROOT_DIR}/tests/)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin/)

include(cmake/macros.cmake)
include(cmake/arch.cmake)

# ------------- Build types ---------------------
# Following the convention in
# https://llvm.org/docs/CMake.html#frequently-used-cmake-variables
if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(ENABLE_O3 true)
  set(ENABLE_DEBUG true)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(ENABLE_O3 true)
  set(ENABLE_DEBUG false)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(ENABLE_O3 false)
  set(ENABLE_DEBUG true)
else()
  message(FATAL_ERROR "Unsupported build type: ${CMAKE_BUILD_TYPE}")
endif()

if(ENABLE_DEBUG)
  add_definitions("-D_DEBUG")
  if(NOT IS_WASM)
    set_cxx_flags("-g")
  endif()
else()
  add_definitions("-DNDEBUG")
endif()

if(ENABLE_O3)
  set_cxx_flags("-O3")
endif()

if(ENABLE_VALIDATION)
  add_definitions("-DENABLE_VALIDATION")
endif()

message("======= Build Mode: ${CMAKE_BUILD_TYPE} =======")
message("ENABLE_DEBUG:                   ${ENABLE_DEBUG}")
message("ENABLE_O3:                      ${ENABLE_O3}")
message("ENABLE_VALIDATION:              ${ENABLE_VALIDATION}")
message("BUILD_TESTS:                    ${BUILD_TESTS}")
message("BUILD_DUDECT:                   ${BUILD_DUDECT}")

include(cmake/compilation_flags.cmake)
include(cmake/openssl.cmake)

if(CBMPC_PLATFORM_DEP_OUTPUT_DIR)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
      ${LIB_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_OS}-${CMAKE_ARCH})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      ${LIB_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_OS}-${CMAKE_ARCH})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      ${BIN_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_OS}-${CMAKE_ARCH})
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR}/${CMAKE_BUILD_TYPE})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}/${CMAKE_BUILD_TYPE})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/${CMAKE_BUILD_TYPE})
endif()

include_directories(${SRC_DIR})

add_subdirectory(src/cbmpc/core)
add_subdirectory(src/cbmpc/crypto)
add_subdirectory(src/cbmpc/zk)
add_subdirectory(src/cbmpc/protocol)
add_subdirectory(src/cbmpc/ffi)

add_library(
  cbmpc STATIC $<TARGET_OBJECTS:cbmpc_core> $<TARGET_OBJECTS:cbmpc_crypto>
               $<TARGET_OBJECTS:cbmpc_zk> $<TARGET_OBJECTS:cbmpc_protocol>
               $<TARGET_OBJECTS:cbmpc_ffi>)

link_openssl(cbmpc)

# ------------- Tests ---------------------------

if(NOT IS_MACOS)
  set(USE_FETCH_CONTENT true)
endif()

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
