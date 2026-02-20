cmake_minimum_required(VERSION 3.16)

project(cbmpc_benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(REPO_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)
set(REPO_CMAKE_DIR ${REPO_ROOT}/cmake)

include(${REPO_CMAKE_DIR}/macros.cmake)
include(${REPO_CMAKE_DIR}/arch.cmake)
include(${REPO_CMAKE_DIR}/openssl.cmake)
include(${REPO_CMAKE_DIR}/compilation_flags.cmake)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable benchmark tests" FORCE)

add_subdirectory(${REPO_ROOT}/vendors/google-benchmark
                 ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/.benchmark)

if(NOT DEFINED CBMPC_SOURCE_DIR)
  if(DEFINED ENV{CBMPC_PREFIX})
    set(CBMPC_SOURCE_DIR "$ENV{CBMPC_PREFIX}")
  elseif(DEFINED ENV{CBMPC_PREFIX_FULL})
    set(CBMPC_SOURCE_DIR "$ENV{CBMPC_PREFIX_FULL}")
  else()
    # Default to the repo-local full install prefix (no sudo).
    set(CBMPC_SOURCE_DIR "${REPO_ROOT}/build/install/full")
  endif()
endif()

set(CBMPC_LIB_DIR "${CBMPC_SOURCE_DIR}/lib")
if(EXISTS "${CBMPC_SOURCE_DIR}/lib/Release/libcbmpc.a")
  set(CBMPC_LIB_DIR "${CBMPC_SOURCE_DIR}/lib/Release")
elseif(EXISTS "${CBMPC_SOURCE_DIR}/lib/Debug/libcbmpc.a")
  set(CBMPC_LIB_DIR "${CBMPC_SOURCE_DIR}/lib/Debug")
endif()

add_executable(
  cbmpc_benchmark
  bm_main.cpp
  bm_core_bn.cpp
  bm_elliptic_curve.cpp
  bm_hash.cpp
  bm_drbg.cpp
  bm_tdh2.cpp
  bm_commitment.cpp
  bm_elgamal.cpp
  bm_paillier.cpp
  bm_share.cpp
  bm_zk.cpp
  bm_pve.cpp
  bm_ot.cpp
  bm_agree_random.cpp
  bm_sid.cpp
  bm_eddsa.cpp
  bm_ecdsa.cpp
  # bm_test.cpp
  )

add_subdirectory(${REPO_ROOT}/tests/utils ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/.cbmpc_test_util)
target_include_directories(cbmpc_test_util PRIVATE ${CBMPC_SOURCE_DIR}/include)
if(EXISTS "${CBMPC_SOURCE_DIR}/include-internal")
  target_include_directories(cbmpc_test_util PRIVATE ${CBMPC_SOURCE_DIR}/include-internal)
endif()
# `cbmpc_test_util` (local_network runner) historically used the parallel transport
# demo helpers. Keep the implementation linked here so clean Linux builds don't
# depend on core library internals.
target_sources(cbmpc_test_util PRIVATE ${REPO_ROOT}/demo-cpp/common/mpc_job_session.cpp)

target_include_directories(cbmpc_benchmark PRIVATE ${CBMPC_SOURCE_DIR}/include)
if(EXISTS "${CBMPC_SOURCE_DIR}/include-internal")
  target_include_directories(cbmpc_benchmark PRIVATE ${CBMPC_SOURCE_DIR}/include-internal)
endif()
target_include_directories(cbmpc_benchmark PRIVATE ${REPO_ROOT}/demo-cpp/common)

target_link_directories(cbmpc_benchmark PUBLIC ${CBMPC_LIB_DIR})
link_openssl(cbmpc_benchmark)
target_link_libraries(cbmpc_benchmark PUBLIC benchmark::benchmark PRIVATE cbmpc)
target_link_libraries(cbmpc_benchmark PRIVATE cbmpc_test_util)

if(IS_LINUX)
  link_openssl(cbmpc_benchmark)
endif()
