add_library(cbmpc_c_api OBJECT "")

target_sources(cbmpc_c_api PRIVATE
  common.cpp
  eddsa_mp.cpp
  eddsa2pc.cpp
  ecdsa_mp.cpp
  ecdsa2pc.cpp
  pve_base_pke.cpp
  pve_batch_ac.cpp
  pve_batch_single_recipient.cpp
  schnorr_mp.cpp
  schnorr2pc.cpp
  tdh2.cpp
)

target_link_libraries(cbmpc_c_api cbmpc_core cbmpc_api)

