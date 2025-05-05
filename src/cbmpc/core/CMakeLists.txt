add_library(cbmpc_core OBJECT "")

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
