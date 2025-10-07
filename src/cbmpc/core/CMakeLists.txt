add_library(cbmpc_core OBJECT "")

# Link OpenSSL before precompiled headers to ensure headers are available
link_openssl(cbmpc_core)

target_precompile_headers(cbmpc_core PUBLIC "precompiled.h")

target_sources(cbmpc_core PRIVATE
  buf.cpp
  buf128.cpp
  buf256.cpp
  convert.cpp
  error.cpp
  strext.cpp
  extended_uint.cpp
)
