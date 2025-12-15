add_library(cbmpc_ffi OBJECT
  cmem_adapter.cpp
  pki.cpp
)

target_include_directories(cbmpc_ffi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Ensure OpenSSL headers/libs are available (matches other components)
link_openssl(cbmpc_ffi)


