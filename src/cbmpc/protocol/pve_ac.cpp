#include "pve_ac.h"

using namespace coinbase::crypto;

namespace coinbase::mpc {

static buf_t batch_to_bin(ecurve_t curve, const std::vector<bn_t>& x) {
  int batch_size = int(x.size());
  int curve_size = curve.size();
  buf_t bin(batch_size * curve_size);
  for (int j = 0; j < batch_size; j++) x[j].to_bin(bin.range(j * curve_size, curve_size));
  return bin;
}

static error_t batch_from_bin(ecurve_t curve, int batch_size, mem_t bin, std::vector<bn_t>& x) {
  int curve_size = curve.size();
  if (bin.size != batch_size * curve_size) return coinbase::error(E_BADARG);
  x.resize(batch_size);
  for (int j = 0; j < batch_size; j++) x[j] = bn_t::from_bin(bin.range(j * curve_size, curve_size));
  return SUCCESS;
}

template <class PKI_T>
void ec_pve_ac_t<PKI_T>::encrypt_row(const ss::ac_t& ac, const pks_t& ac_pks, mem_t L, ecurve_t curve, mem_t seed,
                                     mem_t plain, buf_t& c, std::vector<CT_T>& quorum_c) {
  const mod_t& q = curve.order();
  crypto::drbg_aes_ctr_t drbg(seed);
  bn_t K = drbg.gen_bn(q);

  std::map<std::string, bn_t> K_shares = ac.share(curve.order(), K, &drbg);
  for (const auto& [path, pub_key] : ac_pks) {
    CT_T c;
    c.encrypt(pub_key, L, K_shares[path].to_bin(), &drbg);
    quorum_c.push_back(c);
  }

  buf_t k_and_iv = crypto::ro::hash_string(K, L).bitlen(256 + iv_bitlen);
  mem_t k_aes = k_and_iv.take(32);
  mem_t iv = k_and_iv.skip(32);

  crypto::aes_gcm_t::encrypt(k_aes, iv, L, tag_size, plain, c);
}

template <class PKI_T>
void ec_pve_ac_t<PKI_T>::encrypt_row0(const ss::ac_t& ac, const pks_t& ac_pks, mem_t L, ecurve_t curve, mem_t r0_1,
                                      mem_t r0_2, int batch_size,
                                      std::vector<bn_t>& x0,        // output
                                      buf_t& c0,                    // output
                                      std::vector<CT_T>& quorum_c0  // output
) {
  const mod_t& q = curve.order();
  x0.resize(batch_size);
  crypto::drbg_aes_ctr_t drbg(r0_1);
  for (int j = 0; j < batch_size; j++) x0[j] = drbg.gen_bn(q);
  encrypt_row(ac, ac_pks, L, curve,
              r0_2,      // seed
              r0_1,      // plain
              c0,        // output
              quorum_c0  // output
  );
}

template <class PKI_T>
void ec_pve_ac_t<PKI_T>::encrypt_row1(const ss::ac_t& ac, const pks_t& ac_pks, mem_t L, ecurve_t curve, mem_t r1,
                                      mem_t x1_bin,
                                      buf_t& c1,                    // output
                                      std::vector<CT_T>& quorum_c1  // output
) {
  encrypt_row(ac, ac_pks, L, curve,
              r1,        // seed
              x1_bin,    // plain
              c1,        // output
              quorum_c1  // output
  );
}

template <class PKI_T>
void ec_pve_ac_t<PKI_T>::encrypt(const ss::ac_t& ac, const pks_t& ac_pks, mem_t label, ecurve_t curve,
                                 const std::vector<bn_t>& _x) {
  int batch_size = int(_x.size());
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  std::vector<bn_t> x(batch_size);
  Q.resize(batch_size);
  for (int i = 0; i < batch_size; i++) {
    x[i] = _x[i] % q;
    Q[i] = x[i] * G;
  }
  L = crypto::sha256_t::hash(label, Q);

  std::vector<std::vector<ecc_point_t>> X0(kappa);
  std::vector<std::vector<ecc_point_t>> X1(kappa);
  std::vector<buf_t> c0(kappa);
  std::vector<buf_t> c1(kappa);
  std::vector<std::vector<CT_T>> quorum_c0(kappa);
  std::vector<std::vector<CT_T>> quorum_c1(kappa);
  std::vector<buf_t> r0_1(kappa);
  std::vector<buf_t> r0_2(kappa);
  std::vector<buf_t> r1(kappa);

  for (int i = 0; i < kappa; i++) {
    X0[i].resize(batch_size);
    X1[i].resize(batch_size);

    r0_1[i] = crypto::gen_random_bitlen(SEC_P_COM);
    r0_2[i] = crypto::gen_random_bitlen(SEC_P_COM);
    r1[i] = crypto::gen_random_bitlen(SEC_P_COM);

    std::vector<bn_t> x0;
    encrypt_row0(ac, ac_pks, L, curve, r0_1[i], r0_2[i], batch_size, x0, c0[i], quorum_c0[i]);

    std::vector<bn_t> x1(batch_size);
    for (int j = 0; j < batch_size; j++) MODULO(q) x1[j] = x[j] - x0[j];

    row_t& row = rows[i];
    row.x_bin = batch_to_bin(curve, x1);
    encrypt_row1(ac, ac_pks, L, curve, r1[i], row.x_bin, c1[i], quorum_c1[i]);

    for (int j = 0; j < batch_size; j++) {
      X0[i][j] = x0[j] * G;
      X1[i][j] = Q[j] - X0[i][j];
    }
  }

  b = crypto::ro::hash_string(Q, label, c0, c1, quorum_c0, quorum_c1, X0, X1).bitlen(kappa);

  for (int i = 0; i < kappa; i++) {
    bool bit = b.get_bit(i);
    rows[i].r = bit ? r1[i] : (r0_1[i] + r0_2[i]);  // concat
    rows[i].c = bit ? c0[i] : c1[i];
    rows[i].quorum_c = bit ? quorum_c0[i] : quorum_c1[i];
    if (!bit) rows[i].x_bin.free();  // clear output
  }
}

template <class PKI_T>
error_t ec_pve_ac_t<PKI_T>::verify(const ss::ac_t& ac, const pks_t& ac_pks, const std::vector<ecc_point_t>& Q,
                                   mem_t label) const {
  error_t rv = UNINITIALIZED_ERROR;
  int batch_size = int(Q.size());
  if (batch_size == 0) return coinbase::error(E_BADARG);

  ecurve_t curve = Q[0].get_curve();
  const auto& G = curve.generator();
  if (Q.size() != this->Q.size()) return coinbase::error(E_CRYPTO);
  for (int i = 0; i < batch_size; i++) {
    if (Q[i] != this->Q[i]) return coinbase::error(E_CRYPTO);
  }

  if (Q != this->Q) return coinbase::error(E_CRYPTO);
  buf_t L = crypto::sha256_t::hash(label, Q);
  if (L != this->L) return coinbase::error(E_CRYPTO);

  std::vector<std::vector<ecc_point_t>> X0(kappa);
  std::vector<std::vector<ecc_point_t>> X1(kappa);
  std::vector<buf_t> c0(kappa);
  std::vector<buf_t> c1(kappa);
  std::vector<std::vector<CT_T>> quorum_c0(kappa);
  std::vector<std::vector<CT_T>> quorum_c1(kappa);

  for (int i = 0; i < kappa; i++) {
    X0[i].resize(batch_size);
    X1[i].resize(batch_size);
    bool bit = b.get_bit(i);
    const row_t& row = rows[i];

    std::vector<bn_t> xb;
    if (bit) {
      c0[i] = row.c;
      quorum_c0[i] = row.quorum_c;
      if (rv = batch_from_bin(curve, batch_size, row.x_bin, xb)) return rv;
      mem_t r1 = row.r;
      encrypt_row1(ac, ac_pks, L, curve, r1, row.x_bin, c1[i], quorum_c1[i]);
    } else {
      c1[i] = row.c;
      quorum_c1[i] = row.quorum_c;
      mem_t r0_1 = row.r.take(16);
      mem_t r0_2 = row.r.skip(16);
      encrypt_row0(ac, ac_pks, L, curve, r0_1, r0_2, batch_size, xb, c0[i], quorum_c0[i]);
    }

    for (int j = 0; j < batch_size; j++) {
      ecc_point_t Xb = xb[j] * G;
      X1[i][j] = bit ? Xb : Q[j] - Xb;
      X0[i][j] = bit ? Q[j] - Xb : Xb;
    }
  }

  buf128_t b_tag;
  b_tag = crypto::ro::hash_string(Q, label, c0, c1, quorum_c0, quorum_c1, X0, X1).bitlen(SEC_P_COM);
  if (b_tag != b) return coinbase::error(E_CRYPTO);
  return SUCCESS;
}

template <class PKI_T>
error_t ec_pve_ac_t<PKI_T>::find_quorum_ciphertext(const std::vector<std::string>& sorted_leaves,
                                                   const std::string& path, const row_t& row, const CT_T*& c) {
  auto it = std::find(sorted_leaves.begin(), sorted_leaves.end(), path);
  if (it == sorted_leaves.end()) return coinbase::error(E_NOT_FOUND, "path not found");
  auto index = it - sorted_leaves.begin();

  const auto& quorum_c = row.quorum_c;
  if (index >= quorum_c.size()) return coinbase::error(E_NOT_FOUND, "path not found");
  c = &quorum_c[index];

  return SUCCESS;
}

template <class PKI_T>
error_t ec_pve_ac_t<PKI_T>::get_row_to_decrypt(const ss::ac_t& ac, int row_index, const std::string& path,
                                               buf_t& out) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (row_index < 0 || row_index >= kappa) return coinbase::error(E_RANGE);

  std::set<std::string> leaves = ac.list_leaf_names();
  std::vector<std::string> sorted_leaves(leaves.begin(), leaves.end());
  const CT_T* c;
  if (rv = find_quorum_ciphertext(sorted_leaves, path, rows[row_index], c)) return rv;
  if (rv = c->decrypt_begin(out)) return rv;

  return SUCCESS;
}

template <class PKI_T>
error_t ec_pve_ac_t<PKI_T>::restore_row(const ss::ac_t& ac, int row_index,
                                        const std::map<std::string, buf_t>& decrypted, mem_t label,
                                        std::vector<bn_t>& x) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (row_index < 0 || row_index >= kappa) return coinbase::error(E_RANGE);
  const row_t& row = rows[row_index];

  int batch_size = int(Q.size());
  if (batch_size == 0) return coinbase::error(E_BADARG);

  ecurve_t curve = Q[0].get_curve();
  int curve_size = curve.size();
  const auto& G = curve.generator();
  const mod_t& q = curve.order();

  buf_t L = crypto::sha256_t::hash(label, Q);

  std::set<std::string> leaves = ac.list_leaf_names();
  std::vector<std::string> sorted_leaves(leaves.begin(), leaves.end());

  std::map<std::string, bn_t> quorum_decrypted;
  for (const auto& [path, dec] : decrypted) {
    const CT_T* c;
    if (rv = find_quorum_ciphertext(sorted_leaves, path, row, c)) return rv;
    buf_t plain;
    if (rv = c->decrypt_end(L, dec, plain)) return rv;

    quorum_decrypted[path] = bn_t::from_bin(plain);
  }

  bn_t K;
  if (rv = ac.reconstruct(q, quorum_decrypted, K)) return rv;

  buf_t k_and_iv = crypto::ro::hash_string(K, L).bitlen(256 + iv_bitlen);
  mem_t k_aes = k_and_iv.take(32);
  mem_t iv = k_and_iv.skip(32);

  buf_t decrypted_data;
  if (rv = crypto::aes_gcm_t::decrypt(k_aes, iv, L, tag_size, row.c, decrypted_data)) return rv;

  mem_t seed;
  mem_t x_bin;

  bool bit = b.get_bit(row_index);
  if (bit) {
    x_bin = row.x_bin;
    seed = decrypted_data;
  } else {
    x_bin = decrypted_data;
    seed = row.r.take(16);
  }

  if (x_bin.size != batch_size * curve_size) return coinbase::error(E_CRYPTO);
  crypto::drbg_aes_ctr_t drbg(seed);
  x.resize(batch_size);
  for (int j = 0; j < batch_size; j++) {
    bn_t x0 = drbg.gen_bn(q);
    bn_t x1 = bn_t::from_bin(x_bin.range(j * curve_size, curve_size));
    MODULO(q) x[j] = x0 + x1;
    if (x[j] * G != Q[j]) return coinbase::error(E_CRYPTO);
  }

  return SUCCESS;
}

template <class PKI_T>
error_t ec_pve_ac_t<PKI_T>::decrypt(const crypto::ss::ac_t& ac, const sks_t& quorum_ac_sks, const pks_t& all_ac_pks,
                                    mem_t label, std::vector<bn_t>& x, bool skip_verify) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (!skip_verify && (rv = verify(ac, all_ac_pks, Q, label))) return rv;

  for (int row_index = 0; row_index < kappa; row_index++) {
    std::map<std::string, buf_t> dec_infos;
    for (const auto& [path, prv_key] : quorum_ac_sks) {
      buf_t enc_info;
      if (rv = get_row_to_decrypt(ac, row_index, path, enc_info)) continue;

      if (rv = prv_key.execute(enc_info, dec_infos[path])) continue;
    }

    rv = restore_row(ac, row_index, dec_infos, label, x);
    if (rv == SUCCESS) return SUCCESS;
  }
  return SUCCESS;
}

template class ec_pve_ac_t<hybrid_cipher_t>;
template class ec_pve_ac_t<ecies_t>;
template class ec_pve_ac_t<rsa_kem_t>;

}  // namespace coinbase::mpc
