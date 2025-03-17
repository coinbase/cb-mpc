add_library(cbmpc_protocol OBJECT "")

target_precompile_headers(cbmpc_protocol PUBLIC "mpc_job.h")

target_sources(cbmpc_protocol  PRIVATE
  int_commitment.cpp

  mpc_job.cpp
  mpc_job_session.cpp

  agree_random.cpp
  ot.cpp

  pve.cpp
  pve_ac.cpp

  ec_dkg.cpp
  ecdsa_2p.cpp
  ecdsa_mp.cpp
  schnorr_2p.cpp
  schnorr_mp.cpp
  eddsa.cpp

  hd_tree_bip32.cpp
  hd_keyset_ecdsa_2p.cpp
  hd_keyset_eddsa_2p.cpp
)

target_link_libraries(cbmpc_protocol cbmpc_zk)
