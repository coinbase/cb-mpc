add_library(cbmpc_api OBJECT "")

target_sources(cbmpc_api PRIVATE
  eddsa_mp.cpp
  eddsa2pc.cpp
  ecdsa_mp.cpp
  ecdsa2pc.cpp
  hd_keyset_eddsa_2p.cpp
  hd_keyset_ecdsa_2p.cpp
  pve.cpp
  pve_ac.cpp
  pve_batch.cpp
  schnorr_mp.cpp
  schnorr2pc.cpp
  tdh2.cpp
)

target_link_libraries(cbmpc_api cbmpc_core cbmpc_crypto cbmpc_protocol)

