#include "ec_dkg.h"

#include <cbmpc/crypto/ro.h>
#include <cbmpc/protocol/agree_random.h>
#include <cbmpc/protocol/sid.h>
#include <cbmpc/zk/zk_elgamal_com.h>
#include <cbmpc/zk/zk_pedersen.h>

#include "util.h"

using namespace coinbase::crypto::ss;

#define _ij msgs[j]
#define _i msg
#define _j received(j)
#define _ji received(j)
#define _js all_received_refs()

namespace coinbase::mpc::eckey {
void dkg_2p_t::step1_p1_to_p2(const bn_t& x1) {
  this->x1 = x1;
  const auto& G = curve.generator();
  sid1 = crypto::gen_random_bitlen(SEC_P_COM);
  Q1 = x1 * G;

  com.id(sid1, p1_pid).gen(Q1);
}

void dkg_2p_t::step2_p2_to_p1(const bn_t& x2) {
  this->x2 = x2;
  const auto& G = curve.generator();
  sid2 = crypto::gen_random_bitlen(SEC_P_COM);
  sid = crypto::sha256_t::hash(sid1, sid2);

  Q2 = x2 * G;
  pi_2.prove(Q2, x2, sid, 2);
}

error_t dkg_2p_t::step3_p1_to_p2(ecc_point_t& Q) {
  error_t rv = UNINITIALIZED_ERROR;
  if (rv = curve.check(Q2)) return coinbase::error(E_CRYPTO, "dkg_2p_t::p1_verify: check Q2 failed");
  sid = crypto::sha256_t::hash(sid1, sid2);
  if (rv = pi_2.verify(Q2, sid, 2)) return rv;
  pi_1.prove(Q1, x1, sid, 1);
  Q = Q1 + Q2;
  return SUCCESS;
}

error_t dkg_2p_t::step4_output_p2(ecc_point_t& Q) {
  error_t rv = UNINITIALIZED_ERROR;
  if (rv = curve.check(Q1)) return coinbase::error(E_CRYPTO, "dkg_2p_t::p2_verify: check Q1 failed");
  if (rv = com.id(sid1, p1_pid).open(Q1)) return rv;
  if (rv = pi_1.verify(Q1, sid, 1)) return rv;
  Q = Q1 + Q2;
  return SUCCESS;
}

error_t key_share_2p_t::dkg(job_2p_t& job, ecurve_t curve, key_share_2p_t& key, buf_t& sid) {
  error_t rv = UNINITIALIZED_ERROR;
  key.curve = curve;
  const mod_t& q = curve.order();
  eckey::dkg_2p_t ec_dkg(curve, job.get_pid(party_t::p1));
  key.x_share = bn_t::rand(q);
  key.role = job.get_party();

  if (job.is_p1()) {
    ec_dkg.step1_p1_to_p2(key.x_share);
  }

  if (rv = job.p1_to_p2(ec_dkg.msg1)) return rv;

  if (job.is_p2()) {
    ec_dkg.step2_p2_to_p1(key.x_share);
  }

  if (rv = job.p2_to_p1(ec_dkg.msg2)) return rv;

  if (job.is_p1()) {
    if (rv = ec_dkg.step3_p1_to_p2(key.Q)) return rv;
  }

  if (rv = job.p1_to_p2(ec_dkg.msg3)) return rv;

  if (job.is_p2()) {
    if (rv = ec_dkg.step4_output_p2(key.Q)) return rv;
  }

  sid = std::move(ec_dkg.sid);
  return SUCCESS;
}

error_t key_share_2p_t::refresh(job_2p_t& job, const key_share_2p_t& key, key_share_2p_t& new_key) {
  error_t rv = UNINITIALIZED_ERROR;
  new_key.role = key.role;
  new_key.curve = key.curve;
  new_key.Q = key.Q;

  const mod_t& q = key.curve.order();
  buf_t rand_bits;
  if (rv = agree_random(job, q.get_bits_count() + SEC_P_STAT, rand_bits)) return rv;
  bn_t r = bn_t::from_bin(rand_bits) % q;

  if (job.is_p1()) {
    MODULO(q) { new_key.x_share = key.x_share + r; }
  }

  if (job.is_p2()) {
    MODULO(q) { new_key.x_share = key.x_share - r; }
  }

  return SUCCESS;
}

error_t key_share_mp_t::dkg(job_mp_t& job, ecurve_t curve, key_share_mp_t& key, buf_t& sid) {
  error_t rv = UNINITIALIZED_ERROR;
  int n = job.get_n_parties();
  int i = job.get_party_idx();
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  key.party_index = i;
  key.curve = curve;
  key.Qis.resize(n);
  auto h_consistency = job.uniform_msg<buf256_t>();
  h_consistency._i = crypto::sha256_t::hash(std::string(curve.get_name()));

  auto sid_i = job.uniform_msg<buf_t>(crypto::gen_random_bitlen(SEC_P_COM));
  key.x_share = bn_t::rand(q);
  auto Qi = job.uniform_msg<ecc_point_t>(key.x_share * G);

  coinbase::crypto::commitment_t com(sid_i, job.get_pid(i));

  com.gen(Qi.msg);
  auto c = job.uniform_msg<buf_t>(com.msg);
  if (rv = job.plain_broadcast(sid_i, c, h_consistency)) return rv;

  for (int j = 0; j < n; j++) {
    if (j == i) continue;
    if (h_consistency._j != h_consistency) return coinbase::error(E_CRYPTO);
  }

  sid = crypto::sha256_t::hash(sid_i._js);
  auto h = job.uniform_msg<buf256_t>(crypto::sha256_t::hash(c._js));
  auto pi = job.uniform_msg<zk::uc_dl_t>();
  pi.prove(Qi, key.x_share, sid, i);

  auto rho = job.uniform_msg<buf256_t>(com.rand);
  auto sid_msg = job.uniform_msg<buf_t>(sid);
  if (rv = job.plain_broadcast(sid_msg, h, Qi, rho, pi)) return rv;

  for (int j = 0; j < n; j++) {
    if (j == i) continue;

    if (sid_msg._j != sid) return coinbase::error(E_CRYPTO);
    if (h._j != h.msg) return coinbase::error(E_CRYPTO);

    if (rv = crypto::commitment_t(sid_i._j, job.get_pid(j)).set(rho._j, c._j).open(Qi._j)) return rv;

    // curve check of Qi._j is done inside the zk verify function
    if (rv = pi._j.verify(Qi._j, sid, j)) return rv;
  }

  key.Qis = Qi.all_received_values();
  key.Q = SUM(key.Qis);
  return SUCCESS;
}

error_t key_share_mp_t::refresh(job_mp_t& job, buf_t& sid, const key_share_mp_t& current_key, key_share_mp_t& new_key) {
  error_t rv = UNINITIALIZED_ERROR;

  if (sid.empty()) {
    if (rv = generate_sid_fixed_mp(job, sid)) return rv;
  }

  int n = job.get_n_parties();
  int i = job.get_party_idx();
  const crypto::mpc_pid_t& pid = job.get_pid();

  ecurve_t curve = current_key.curve;
  const mod_t& q = curve.order();
  const auto& G = curve.generator();

  if (current_key.party_index != i) return coinbase::error(E_BADARG, "Wrong role");
  if (current_key.Qis.size() != n) return coinbase::error(E_BADARG, "Wrong number of peers");
  if (current_key.x_share * G != current_key.Qis[i]) return coinbase::error(E_BADARG, "x_share does not match Qi");
  if (SUM(current_key.Qis) != current_key.Q) return coinbase::error(E_BADARG, "Q does not match the sum of Qis");
  auto h_consistency = job.uniform_msg<buf256_t>();
  h_consistency._i = crypto::sha256_t::hash(sid, current_key.Q, current_key.Qis);

  new_key = current_key;

  auto r = job.nonuniform_msg<bn_t>();
  auto R = job.uniform_msg<std::vector<ecc_point_t>>(std::vector<ecc_point_t>(n));
  auto pi_r = job.uniform_msg<std::vector<zk::uc_dl_t>>(std::vector<zk::uc_dl_t>(n));
  for (int j = 0; j < n; j++) {
    r._ij = bn_t::rand(q);
    R._i[j] = r._ij * G;
    pi_r._i[j].prove(R._i[j], r._ij, sid, i * n + j);
  }

  crypto::commitment_t com_R(sid, pid);
  auto c = job.uniform_msg<buf256_t>();
  auto rho = job.uniform_msg<buf256_t>();
  com_R.gen(R.msg, pi_r.msg);
  c._i = com_R.msg;     // c_i
  rho._i = com_R.rand;  // rho_i
  if (rv = job.plain_broadcast(c, h_consistency)) return rv;

  for (int j = 0; j < n; j++) {
    if (j == i) continue;
    if (h_consistency._j != h_consistency) return coinbase::error(E_CRYPTO);
  }
  auto h = job.uniform_msg<buf256_t>();
  h._i = crypto::sha256_t::hash(c.all_received_refs());

  if (rv = job.plain_broadcast(r, h, R, pi_r, rho)) return rv;

  for (int j = 0; j < n; j++) {
    if (j == i) continue;

    // Curve check of R._j[l] is done inside the zk verify function further below

    if (h._j != h) return coinbase::error(E_CRYPTO);

    if (rv = com_R.id(sid, job.get_pid(j)).set(rho._j, c._j).open(R._j, pi_r._j)) return rv;
    for (int l = 0; l < n; l++) {
      if (l == j) continue;
      if (rv = pi_r._j[l].verify(R._j[l], sid, j * n + l)) return rv;
    }
    if (r._ji * G != R._j[i]) return coinbase::error(E_CRYPTO);
  }

  for (int j = 0; j < n; j++) {
    if (j == i) continue;

    bn_t delta_x;
    MODULO(q) delta_x = r._ij + r._ji;
    if (j < i)
      MODULO(q) new_key.x_share += delta_x;
    else
      MODULO(q) new_key.x_share -= delta_x;
  }

  for (int j = 0; j < n; j++) {
    for (int l = 0; l < n; l++) {
      if (l == j) continue;
      ecc_point_t R_delta = R.received(j)[l] + R.received(l)[j];
      if (l < j)
        new_key.Qis[j] += R_delta;
      else
        new_key.Qis[j] -= R_delta;
    }
  }

  if (new_key.Qis[i] != new_key.x_share * G) return coinbase::error(E_CRYPTO);

  if (SUM(new_key.Qis) != current_key.Q) return coinbase::error(E_CRYPTO);
  new_key.Q = current_key.Q;
  return SUCCESS;
}

error_t dkg_mp_threshold_t::dkg_or_refresh(job_mp_t& job, const ecurve_t& curve, buf_t& sid, const crypto::ss::ac_t ac,
                                           const party_set_t& quorum_party_set, key_share_mp_t& key,
                                           key_share_mp_t& new_key, bool is_refresh) {
  error_t rv = UNINITIALIZED_ERROR;

  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  int n = job.get_n_parties();
  int quorum_count = 0;
  int i = job.get_party_idx();
  std::vector<crypto::mpc_pid_t> all_pids(n);
  std::map<party_idx_t, crypto::mpc_pid_t> quorum_pids;
  int representative_quorum_pid_index = -1;
  std::set<crypto::pname_t> quorum_pids_set;
  for (int j = 0; j < n; j++) {
    all_pids[j] = job.get_pid(j);
    if (quorum_party_set.has(j)) {
      quorum_pids[j] = job.get_pid(j);
      quorum_pids_set.insert(job.get_pid(j).to_string());
      quorum_count++;
      representative_quorum_pid_index = j;
    }
  }

  if (!ac.enough_for_quorum(quorum_pids_set)) {
    return coinbase::error(E_BADARG, "Not enough quorum parties");
  }

  if (sid.empty()) {
    if (rv = coinbase::mpc::generate_sid_fixed_mp(job, sid)) return coinbase::error(rv, "Failed to generate sid");
  }

  auto h_consistency = job.uniform_msg<buf256_t>();
  h_consistency._i = crypto::sha256_t::hash(std::string(curve.get_name()), all_pids, quorum_pids, sid);

  if (rv = job.plain_broadcast(h_consistency)) return coinbase::error(rv, "Failed to broadcast h_consistency");

  for (int j = 0; j < n; j++) {
    if (j == i) continue;
    if (h_consistency._j != h_consistency) return coinbase::error(E_CRYPTO, "h_consistency mismatch");
  }

  auto xij = job.nonuniform_msg<bn_t>();

  auto ac_pub_all = job.uniform_msg<ac_internal_pub_shares_t>();
  auto pi_r_all = job.uniform_msg<zk::uc_batch_dl_t>();
  auto c_all = job.uniform_msg<buf_t>();
  auto rho_all = job.uniform_msg<buf256_t>();

  crypto::commitment_t com_R(job.get_pid(i));

  if (quorum_party_set.has(i)) {
    std::vector<bn_t> rs;
    std::vector<ecc_point_t> Rs;
    bn_t r = 0;
    if (!is_refresh) {
      r = bn_t::rand(q);
    }

    ac_internal_shares_t ac_internal_shares;
    ac_shares_t shares;
    if (rv = ac.share_with_internals(q, r, shares, ac_internal_shares, ac_pub_all._i))
      return coinbase::error(rv, "Failed to share with internals");
    for (int j = 0; j < n; j++) {
      xij.msgs[j] = shares[job.get_pid(j).to_string()];
    }

    if (is_refresh) {
      // Since the root is the point at infinity
      ac_pub_all._i.erase(ac.root->name);
    }

    for (const auto& [node_name, internal_pub_shares] : ac_pub_all._i) {
      auto internal_share = ac_internal_shares.at(node_name);
      // NOTE: because of the less optimized implementation of the sharing, there are essentially duplicate data
      //       in `rs` and similarly in `Rs` which will cause us do more batch zk dl operations than necessary.
      rs.push_back(internal_share);
      Rs.push_back(internal_pub_shares);
    }

    pi_r_all._i = zk::uc_batch_dl_t();
    pi_r_all._i.prove(Rs, rs, sid, i);

    com_R.gen(Rs, pi_r_all._i);
    c_all._i = com_R.msg;
    rho_all._i = com_R.rand;
  }

  if (rv = job.plain_broadcast(c_all)) return coinbase::error(rv, "Failed to broadcast c_all");

  auto h_all = job.uniform_msg<buf256_t>();
  std::map<party_idx_t, buf_t> all_received_c_s;
  for (int j = 0; j < n; j++) {
    if (!quorum_party_set.has(j)) continue;

    all_received_c_s[j] = c_all._j;
  }
  h_all._i = crypto::sha256_t::hash(all_received_c_s, quorum_pids, sid);

  if (rv = job.plain_broadcast(h_all, ac_pub_all, pi_r_all, rho_all, xij))
    return coinbase::error(rv, "Failed to broadcast h_all, ac_pub_all, pi_r_all, rho_all, xij");

  std::map<party_idx_t, buf_t> cs;
  for (int j = 0; j < n; j++) {
    if (j == i) continue;
    if (!quorum_party_set.has(j)) continue;

    if (h_all._j != h_all.received(representative_quorum_pid_index)) return coinbase::error(E_CRYPTO, "h_all mismatch");

    crypto::commitment_t com_R_tag(quorum_pids[j]);
    // deviation from the spec: since we are sending `c` to all parties, we open them for all parties.
    // furthermore, later on we compute the hash and check if the hash with the cs is correct.
    std::vector<ecc_point_t> Rs;
    for (const auto& [node_name, internal_pub_shares] : ac_pub_all._j) {
      Rs.push_back(internal_pub_shares);
    }
    com_R_tag.set(rho_all._j, c_all._j);
    if (rv = com_R_tag.open(Rs, pi_r_all._j)) return coinbase::error(rv, "Failed to open com_R_tag");

    cs[j] = c_all._j;
    // Verifying that R values are on the curve and subgroup is done in the zk verify function
    if (rv = pi_r_all._j.verify(Rs, sid, j)) return coinbase::error(rv, "Failed to verify pi_r_all");
    if (is_refresh) {
      ac_pub_all._j[ac.root->name] = curve.infinity();
    }
    ecc_point_t Qj = ac_pub_all._j.at(ac.root->name);
    if (rv = ac.verify_share_against_ancestors_pub_data(Qj, xij._j, ac_pub_all._j, job.get_pid(i).to_string()))
      return coinbase::error(rv, "Failed to verify share against ancestors pub data");
  }

  if (!quorum_party_set.has(i)) {
    if (h_all.received(representative_quorum_pid_index) != crypto::sha256_t::hash(cs, quorum_pids, sid))
      return coinbase::error(E_CRYPTO, "h_all mismatch");
  }

  ecc_point_t Q = curve.infinity();
  bn_t x_i = 0;
  for (int j = 0; j < n; j++) {
    if (!quorum_party_set.has(j)) continue;
    if (!is_refresh) {
      crypto::vartime_scope_t vartime_scope;
      Q += ac_pub_all._j.at(ac.root->name);
    }
    MODULO(q) x_i += xij._j;
  }
  ac_pub_shares_t Qis;
  for (int l = 0; l < n; l++) {
    Qis[job.get_pid(l).to_string()] = curve.infinity();
  }
  for (int j = 0; j < n; j++) {
    if (!quorum_party_set.has(j)) continue;

    for (int l = 0; l < n; l++) {
      crypto::vartime_scope_t vartime_scope;
      Qis[job.get_pid(l).to_string()] += ac_pub_all._j.at(job.get_pid(l).to_string());
    }
  }

  {
    crypto::vartime_scope_t vartime_scope;
    ecc_point_t reconstructed_Q;
    if (rv = ac.reconstruct_exponent(Qis, reconstructed_Q))
      return coinbase::error(rv, "Failed to reconstruct exponent");
    if (reconstructed_Q != Q) return coinbase::error(E_CRYPTO, "Q mismatch");
  }
  if (x_i * G != Qis[job.get_pid(i).to_string()])
    return coinbase::error(E_CRYPTO, "x_i * G != Qis[job.get_pid(i).to_string()]");

  if (is_refresh) {
    new_key = key;

    MODULO(q) new_key.x_share += x_i;
    for (int j = 0; j < n; j++) {
      crypto::vartime_scope_t vartime_scope;
      new_key.Qis[j] += Qis[job.get_pid(j).to_string()];
    }
    new_key.party_index = i;
  } else {
    key.x_share = x_i;
    key.Q = Q;
    key.Qis = std::vector<ecc_point_t>(n);
    for (int j = 0; j < n; j++) {
      key.Qis[j] = Qis[job.get_pid(j).to_string()];
    }
    key.curve = curve;
    key.party_index = i;
  }

  return SUCCESS;
}

error_t dkg_mp_threshold_t::dkg(job_mp_t& job, const ecurve_t& curve, buf_t& sid, const crypto::ss::ac_t ac,
                                const party_set_t& quorum_party_set, key_share_mp_t& key) {
  key_share_mp_t dummy_new_key;
  bool is_refresh = false;
  return dkg_or_refresh(job, curve, sid, ac, quorum_party_set, key, dummy_new_key, is_refresh);
}

error_t dkg_mp_threshold_t::refresh(job_mp_t& job, const ecurve_t& curve, buf_t& sid, const crypto::ss::ac_t ac,
                                    const party_set_t& quorum_party_set, key_share_mp_t& key, key_share_mp_t& new_key) {
  bool is_refresh = true;
  return dkg_or_refresh(job, curve, sid, ac, quorum_party_set, key, new_key, is_refresh);
}

error_t key_share_mp_t::reconstruct_additive_share(const mod_t& q, const node_t* node,
                                                   const party_map_t<party_idx_t>& name_to_idx,
                                                   bn_t& additive_share) const {
  error_t rv = UNINITIALIZED_ERROR;
  int n = node->get_n();

  switch (node->type) {
    case node_e::LEAF: {
      const auto& [found, idx] = lookup(name_to_idx, node->name);
      if (!found) {
        return coinbase::error(E_INSUFFICIENT);
      }
      additive_share = 0;
      if (idx == party_index) {
        additive_share = x_share;
      }
    } break;

    case node_e::OR:
      for (int i = 0; i < n; i++) {
        bn_t additive_share_from_child;
        rv = reconstruct_additive_share(q, node->children[i], name_to_idx, additive_share_from_child);
        if (rv == E_INSUFFICIENT) {
          rv = SUCCESS;
          continue;
        }
        if (rv) return rv;
        additive_share = additive_share_from_child;
        break;
      }
      if (rv == E_INSUFFICIENT) {
        return coinbase::error(E_INSUFFICIENT);
      }
      break;
    case node_e::AND:
      for (int i = 0; i < n; i++) {
        bn_t additive_share_from_child;
        if (rv = reconstruct_additive_share(q, node->children[i], name_to_idx, additive_share_from_child)) {
          return rv;
        }
        if (additive_share_from_child != 0) {
          additive_share = additive_share_from_child;
          break;
        }
      }
      break;

    case node_e::THRESHOLD: {
      std::vector<bn_t> pids(node->threshold);
      bn_t share = 0;
      bn_t share_pid = 0;
      int count = 0;

      for (int i = 0; i < n; i++) {
        bn_t share_from_child;
        rv = reconstruct_additive_share(q, node->children[i], name_to_idx, share_from_child);
        if (rv == E_INSUFFICIENT) {
          continue;
        }
        if (rv) return rv;

        pids[count] = node->children[i]->get_pid();
        if (share_from_child != 0) {
          share_pid = pids[count];
          share = share_from_child;
        }
        count++;
        if (count == node->threshold) break;
      }

      if (count < node->threshold) {
        dylog_disable_scope_t dylog_disable_scope;
        return coinbase::error(E_INSUFFICIENT);
      }

      additive_share = crypto::lagrange_partial_interpolate(0, {share}, {share_pid}, pids, q);
    } break;
    case node_e::NONE: {
      return coinbase::error(E_CRYPTO, "key_share_mp_t::reconstruct_additive_share: none node");
    } break;
  }

  return SUCCESS;
}

error_t key_share_mp_t::reconstruct_pub_additive_shares(const crypto::ss::node_t* node,
                                                        const crypto::ss::party_map_t<party_idx_t>& name_to_idx,
                                                        party_idx_t target, ecc_point_t& pub_additive_shares) const {
  error_t rv = UNINITIALIZED_ERROR;
  int n = node->get_n();

  switch (node->type) {
    case node_e::LEAF: {
      const auto& [found, idx] = lookup(name_to_idx, node->name);
      if (!found) {
        return coinbase::error(E_INSUFFICIENT);
      }
      pub_additive_shares = curve.infinity();
      if (idx == target) {
        pub_additive_shares = Qis[idx];
      }
    } break;

    case node_e::OR:
      for (int i = 0; i < n; i++) {
        ecc_point_t additive_share_from_child;
        rv = reconstruct_pub_additive_shares(node->children[i], name_to_idx, target, additive_share_from_child);
        if (rv == E_INSUFFICIENT) {
          rv = SUCCESS;
          continue;
        }
        if (rv) return rv;
        pub_additive_shares = additive_share_from_child;
        break;
      }
      if (rv == E_INSUFFICIENT) {
        return coinbase::error(E_INSUFFICIENT);
      }
      break;
    case node_e::AND:
      for (int i = 0; i < n; i++) {
        ecc_point_t additive_share_from_child;
        if (rv = reconstruct_pub_additive_shares(node->children[i], name_to_idx, target, additive_share_from_child)) {
          return rv;
        }

        if (!additive_share_from_child.is_infinity()) {
          pub_additive_shares = additive_share_from_child;
          break;
        }
      }
      break;

    case node_e::THRESHOLD: {
      std::vector<bn_t> pids(node->threshold);
      ecc_point_t share = curve.infinity();
      bn_t share_pid = 0;
      int count = 0;

      for (int i = 0; i < n; i++) {
        ecc_point_t share_from_child;
        rv = reconstruct_pub_additive_shares(node->children[i], name_to_idx, target, share_from_child);
        if (rv == E_INSUFFICIENT) {
          continue;
        }
        if (rv) return rv;

        pids[count] = node->children[i]->get_pid();
        if (!share_from_child.is_infinity()) {
          share_pid = pids[count];
          share = share_from_child;
        }
        count++;
        if (count == node->threshold) break;
      }

      if (count < node->threshold) {
        dylog_disable_scope_t dylog_disable_scope;
        return coinbase::error(E_INSUFFICIENT);
      }

      pub_additive_shares = crypto::lagrange_partial_interpolate_exponent(0, {share}, {share_pid}, pids);
    } break;
    case node_e::NONE: {
      return coinbase::error(E_CRYPTO, "key_share_mp_t::reconstruct_pub_additive_shares: none node");
    } break;
  }

  return SUCCESS;
}

error_t key_share_mp_t::to_additive_share(const party_idx_t& party_new_index, const crypto::ss::ac_t ac,
                                          const int active_party_count, const party_map_t<party_idx_t>& name_to_idx,
                                          key_share_mp_t& additive_share) {
  error_t rv = UNINITIALIZED_ERROR;
  const mod_t& q = curve.order();
  bn_t new_x_share;
  if (rv = reconstruct_additive_share(q, ac.root, name_to_idx, new_x_share)) return rv;
  std::vector<ecc_point_t> new_Qis(active_party_count);
  for (int j = 0; j < active_party_count; j++) {
    crypto::vartime_scope_t vartime_scope;
    if (rv = reconstruct_pub_additive_shares(ac.root, name_to_idx, j, new_Qis[j])) return rv;
  }

  additive_share.x_share = new_x_share;
  additive_share.Q = Q;
  additive_share.Qis = new_Qis;
  additive_share.curve = curve;
  additive_share.party_index = party_new_index;

  return SUCCESS;
}

}  // namespace coinbase::mpc::eckey