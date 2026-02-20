cmake_minimum_required(VERSION 3.16)

project(mpc-demo-api-ecdsa_mp_pve_backup LANGUAGES CXX)

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

add_executable(mpc-demo-api-ecdsa_mp_pve_backup main.cpp)

target_include_directories(mpc-demo-api-ecdsa_mp_pve_backup PRIVATE ${CBMPC_SOURCE_DIR}/include)
target_link_directories(mpc-demo-api-ecdsa_mp_pve_backup PRIVATE ${CBMPC_LIB_DIR})
target_link_libraries(mpc-demo-api-ecdsa_mp_pve_backup PRIVATE cbmpc)

# Important for static linking on Linux: ensure libcbmpc.a appears before
# libcrypto.a on the final link line so libcrypto symbols resolve correctly.
link_openssl(mpc-demo-api-ecdsa_mp_pve_backup)

