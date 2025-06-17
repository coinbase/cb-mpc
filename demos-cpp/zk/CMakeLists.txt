cmake_minimum_required(VERSION 3.16)

project(mpc-demo-zk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(REPO_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../cmake)

include(${REPO_CMAKE_DIR}/macros.cmake)
include(${REPO_CMAKE_DIR}/arch.cmake)
include(${REPO_CMAKE_DIR}/openssl.cmake)
include(${REPO_CMAKE_DIR}/compilation_flags.cmake)

set(CBMPC_SOURCE_DIR /usr/local/opt/cbmpc/)

add_executable(mpc-demo-zk main.cpp)

link_openssl(mpc-demo-zk)
target_include_directories(mpc-demo-zk PRIVATE ${CBMPC_SOURCE_DIR}/include)
target_link_directories(mpc-demo-zk PRIVATE ${CBMPC_SOURCE_DIR}/lib)
target_link_libraries(mpc-demo-zk PRIVATE cbmpc)

if(IS_LINUX)
  link_openssl(mpc-demo-zk)
endif()
