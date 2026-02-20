add_library(cbmpc_capi OBJECT "")

target_sources(cbmpc_capi PRIVATE
  common.cpp
  eddsa_mp.cpp
  eddsa2pc.cpp
  ecdsa_mp.cpp
  ecdsa2pc.cpp
  pve.cpp
  pve_ac.cpp
  pve_batch.cpp
  schnorr_mp.cpp
  schnorr2pc.cpp
  tdh2.cpp
)

target_link_libraries(cbmpc_capi cbmpc_core cbmpc_api)

