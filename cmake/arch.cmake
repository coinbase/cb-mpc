if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(IS_LINUX true)
  set(CMAKE_OS "Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  string(TOLOWER "${CMAKE_OSX_SYSROOT}" _cbmpc_osx_sysroot)
  if(_cbmpc_osx_sysroot MATCHES "(iphone|appletv|watch|xros)")
    message(
      FATAL_ERROR
        "Unsupported Apple SDK '${CMAKE_OSX_SYSROOT}'. cb-mpc supports only macOS."
    )
  endif()
  set(IS_MACOS true)
  set(CMAKE_OS "Darwin")
else()
  message(
    FATAL_ERROR
      "Unsupported platform '${CMAKE_SYSTEM_NAME}'. cb-mpc supports only 64-bit macOS and Linux."
  )
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(
    FATAL_ERROR
      "Unsupported pointer size '${CMAKE_SIZEOF_VOID_P}'. cb-mpc supports only 64-bit targets."
  )
endif()

if(CMAKE_OSX_ARCHITECTURES MATCHES "arm64" OR
   CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
  set(IS_ARM64 true)
elseif(CMAKE_OSX_ARCHITECTURES MATCHES "x86_64" OR
       CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
  set(IS_X86_64 true)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(IS_CLANG true)
endif()

message("======= Build system: ${CMAKE_SYSTEM_NAME} =======")
message("IS_X86_64:                      ${IS_X86_64}")
message("IS_ARM64:                       ${IS_ARM64}")
message("IS_LINUX:                       ${IS_LINUX}")
message("IS_MACOS:                       ${IS_MACOS}")
message("IS_CLANG:                       ${IS_CLANG}")
message("CMAKE_OS:                       ${CMAKE_OS}")
message("CMAKE_ARCH:                     ${CMAKE_ARCH}")
