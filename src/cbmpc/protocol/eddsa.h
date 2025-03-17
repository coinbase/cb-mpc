#pragma once

#include <cbmpc/protocol/mpc_job.h>
#include <cbmpc/protocol/schnorr_2p.h>
#include <cbmpc/protocol/schnorr_mp.h>
namespace coinbase::mpc::eddsa2pc {

typedef schnorr2p::key_t key_t;

error_t sign(job_2p_t& job, key_t& key, const mem_t& msg, buf_t& sig);

error_t sign_batch(job_2p_t& job, key_t& key, const std::vector<mem_t>& msgs, std::vector<buf_t>& sigs);

}  // namespace coinbase::mpc::eddsa2pc

namespace coinbase::mpc::eddsampc {

typedef schnorrmp::key_t key_t;

error_t sign(job_mp_t& job, key_t& key, const mem_t& msg, party_idx_t sig_receiver, buf_t& sig);

error_t sign_batch(job_mp_t& job, key_t& key, const std::vector<mem_t>& msgs, party_idx_t sig_receiver,
                   std::vector<buf_t>& sigs);

}  // namespace coinbase::mpc::eddsampc