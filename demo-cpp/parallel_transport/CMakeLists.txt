cmake_minimum_required(VERSION 3.16)

project(mpc-demo-parallel_transport LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(REPO_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../cmake)

include(${REPO_CMAKE_DIR}/macros.cmake)
include(${REPO_CMAKE_DIR}/arch.cmake)
include(${REPO_CMAKE_DIR}/openssl.cmake)
include(${REPO_CMAKE_DIR}/compilation_flags.cmake)

if(NOT DEFINED CBMPC_SOURCE_DIR)
  if(DEFINED ENV{CBMPC_PREFIX})
    set(CBMPC_SOURCE_DIR "$ENV{CBMPC_PREFIX}")
  else()
    get_filename_component(_cbmpc_repo_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    if(EXISTS "${_cbmpc_repo_root}/build/install/full")
      set(CBMPC_SOURCE_DIR "${_cbmpc_repo_root}/build/install/full")
    else()
      set(CBMPC_SOURCE_DIR /usr/local/opt/cbmpc/)
    endif()
  endif()
endif()

set(CBMPC_LIB_DIR "${CBMPC_SOURCE_DIR}/lib")
if(EXISTS "${CBMPC_SOURCE_DIR}/lib/Release/libcbmpc.a")
  set(CBMPC_LIB_DIR "${CBMPC_SOURCE_DIR}/lib/Release")
elseif(EXISTS "${CBMPC_SOURCE_DIR}/lib/Debug/libcbmpc.a")
  set(CBMPC_LIB_DIR "${CBMPC_SOURCE_DIR}/lib/Debug")
endif()

add_executable(
  mpc-demo-parallel_transport
  main.cpp
  ../common/mpc_job_session.cpp
)

# The demo helper header lives in demo-cpp/common.
target_include_directories(mpc-demo-parallel_transport PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common)

target_include_directories(mpc-demo-parallel_transport PRIVATE ${CBMPC_SOURCE_DIR}/include)
if(EXISTS "${CBMPC_SOURCE_DIR}/include-internal")
  target_include_directories(mpc-demo-parallel_transport PRIVATE ${CBMPC_SOURCE_DIR}/include-internal)
endif()

target_link_directories(mpc-demo-parallel_transport PRIVATE ${CBMPC_LIB_DIR})
target_link_libraries(mpc-demo-parallel_transport PRIVATE cbmpc)
link_openssl(mpc-demo-parallel_transport)

if(IS_LINUX)
  link_openssl(mpc-demo-parallel_transport)
endif()

