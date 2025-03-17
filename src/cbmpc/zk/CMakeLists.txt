add_library(cbmpc_zk OBJECT "")

target_sources(cbmpc_zk  PRIVATE
  small_primes.cpp
  fischlin.cpp

  zk_ec.cpp
  zk_elgamal_com.cpp
  zk_paillier.cpp
  zk_pedersen.cpp
  zk_unknown_order.cpp
)

target_link_libraries(cbmpc_zk cbmpc_crypto)
