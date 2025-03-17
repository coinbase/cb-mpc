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

set(CBMPC_SOURCE_DIR /usr/local/opt/cbmpc/)

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

target_include_directories(cbmpc_benchmark PRIVATE ${REPO_ROOT}/tests/utils)
target_include_directories(cbmpc_benchmark PRIVATE ${CBMPC_SOURCE_DIR}/include)

target_link_directories(cbmpc_benchmark PUBLIC ${CBMPC_SOURCE_DIR}/lib)
link_openssl(cbmpc_benchmark)
target_link_libraries(cbmpc_benchmark PUBLIC benchmark::benchmark PRIVATE cbmpc)
target_link_libraries(cbmpc_benchmark
                      PRIVATE ${REPO_ROOT}/lib/Release/libcbmpc_test_util.a)

if(IS_LINUX)
  link_openssl(cbmpc_benchmark)
endif()
