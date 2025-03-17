#include "eddsa.h"

namespace coinbase::mpc::eddsa2pc {

error_t sign(job_2p_t& job, key_t& key, const mem_t& msg, buf_t& sig) {
  return schnorr2p::sign(job, key, msg, sig, schnorr2p::variant_e::EdDSA);
}

error_t sign_batch(job_2p_t& job, key_t& key, const std::vector<mem_t>& msgs, std::vector<buf_t>& sigs) {
  return schnorr2p::sign_batch(job, key, msgs, sigs, schnorr2p::variant_e::EdDSA);
}

}  // namespace coinbase::mpc::eddsa2pc

namespace coinbase::mpc::eddsampc {

error_t sign(job_mp_t& job, key_t& key, const mem_t& msg, party_idx_t sig_receiver, buf_t& sig) {
  return schnorrmp::sign(job, key, msg, sig_receiver, sig, schnorrmp::variant_e::EdDSA);
}

error_t sign_batch(job_mp_t& job, key_t& key, const std::vector<mem_t>& msgs, party_idx_t sig_receiver,
                   std::vector<buf_t>& sigs) {
  return schnorrmp::sign_batch(job, key, msgs, sig_receiver, sigs, schnorrmp::variant_e::EdDSA);
}

}  // namespace coinbase::mpc::eddsampc
