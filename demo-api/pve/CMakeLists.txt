cmake_minimum_required(VERSION 3.16)

project(mpc-demo-api-pve LANGUAGES CXX)

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
    if(EXISTS "${_cbmpc_repo_root}/build/install/public")
      set(CBMPC_SOURCE_DIR "${_cbmpc_repo_root}/build/install/public")
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

# Optional explicit artifacts for testing against a local build or released headers.
set(CBMPC_INCLUDE_DIR "${CBMPC_SOURCE_DIR}/include" CACHE PATH "Public header directory")
set(CBMPC_LIBRARY "" CACHE FILEPATH "Explicit libcbmpc.a (optional)")

add_executable(mpc-demo-api-pve main.cpp)

target_include_directories(mpc-demo-api-pve PRIVATE ${CBMPC_INCLUDE_DIR})
if(CBMPC_LIBRARY)
  target_link_libraries(mpc-demo-api-pve PRIVATE "${CBMPC_LIBRARY}")
else()
  target_link_directories(mpc-demo-api-pve PRIVATE ${CBMPC_LIB_DIR})
  target_link_libraries(mpc-demo-api-pve PRIVATE cbmpc)
endif()
link_openssl(mpc-demo-api-pve)

enable_testing()
add_test(NAME pve-ac-recipient-protection COMMAND mpc-demo-api-pve --ac-only)
set_tests_properties(pve-ac-recipient-protection PROPERTIES TIMEOUT 120)

