# Link OpenSSL to a target
#
# This macro links the custom OpenSSL build to a CMake target.
# The OpenSSL path can be customized via:
#   1. CMake variable: -DCBMPC_OPENSSL_ROOT=/path/to/openssl
#   2. Environment variable: export CBMPC_OPENSSL_ROOT=/path/to/openssl
#   3. Default: /usr/local/opt/openssl@3.2.0
#
# To build the custom OpenSSL, run the appropriate script:
#   - macOS (x86_64): scripts/openssl/build-static-openssl-macos.sh
#   - macOS (ARM64):  scripts/openssl/build-static-openssl-macos-m1.sh
#   - Linux:          scripts/openssl/build-static-openssl-linux.sh
#
macro(link_openssl TARGET_NAME)
  if(NOT DEFINED CBMPC_OPENSSL_ROOT)
    if(DEFINED ENV{CBMPC_OPENSSL_ROOT})
      set(CBMPC_OPENSSL_ROOT $ENV{CBMPC_OPENSSL_ROOT})
    else()
      set(CBMPC_OPENSSL_ROOT "/usr/local/opt/openssl@3.2.0")
    endif()
  endif()

  if(IS_LINUX)
    set(_cbmpc_openssl_include "${CBMPC_OPENSSL_ROOT}/include")
    set(_cbmpc_openssl_lib "${CBMPC_OPENSSL_ROOT}/lib64/libcrypto.a")
    if(NOT EXISTS "${_cbmpc_openssl_lib}")
      set(_cbmpc_openssl_lib "${CBMPC_OPENSSL_ROOT}/lib/libcrypto.a")
    endif()
  elseif(IS_MACOS)
    set(_cbmpc_openssl_include "${CBMPC_OPENSSL_ROOT}/include")
    set(_cbmpc_openssl_lib "${CBMPC_OPENSSL_ROOT}/lib/libcrypto.a")
  else()
    message(STATUS "link_openssl: skipping (unsupported platform)")
    return()
  endif()

  # Note: Do not hard-fail on missing OpenSSL here to keep compatibility with
  # external security workflows that may configure without building.

  target_include_directories(${TARGET_NAME} PUBLIC "${_cbmpc_openssl_include}")
  target_link_libraries(${TARGET_NAME} PUBLIC "${_cbmpc_openssl_lib}")
endmacro(link_openssl)
