#pragma once

#include <cbmpc/crypto/secret_sharing.h>
#include <cbmpc/crypto/tdh2.h>

using namespace coinbase::crypto;

namespace coinbase::testutils {

void generate_additive_shares(int n, tdh2::public_key_t& enc_key, tdh2::pub_shares_t& pub_shares,
                              std::vector<tdh2::private_share_t>& dec_shares, ecurve_t curve) {
  const auto& G = curve.generator();

  bn_t x = curve.get_random_value();

  std::vector<bn_t> prv_shares;

  prv_shares = ss::share_and(curve.order(), x, n);
  pub_shares.resize(n);
  for (int i = 0; i < n; i++) {
    pub_shares[i] = prv_shares[i] * G;
  }
  enc_key.Q = x * G;
  enc_key.Gamma = ro::hash_curve(mem_t("TDH2-Gamma"), enc_key.Q).curve(curve);

  dec_shares.resize(n);
  for (int i = 0; i < n; i++) {
    dec_shares[i].x = prv_shares[i];
    dec_shares[i].pid = i + 1;
    dec_shares[i].pub_key = enc_key;
  }
}

void generate_ac_shares(const ss::ac_t& ac, tdh2::public_key_t& enc_key, ss::ac_pub_shares_t& pub_shares,
                        ss::party_map_t<tdh2::private_share_t>& dec_shares, ecurve_t curve) {
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  bn_t x = curve.get_random_value();
  enc_key.Q = x * G;
  enc_key.Gamma = ro::hash_curve(mem_t("TDH2-Gamma"), enc_key.Q).curve(curve);
  ss::ac_shares_t prv_shares = ac.share(q, x);

  pub_shares.clear();
  dec_shares.clear();
  for (const auto& [name, xi] : prv_shares) {
    pub_shares[name] = xi * G;
    dec_shares[name].x = xi;
    dec_shares[name].pid = ss::node_t::pid_from_path(name);
    dec_shares[name].pub_key = enc_key;
  }
}

}  // namespace coinbase::testutils