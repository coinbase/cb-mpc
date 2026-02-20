#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/param_build.h>
#include <openssl/params.h>

#include <cbmpc/core/access_structure.h>
#include <cbmpc/api/curve.h>
#include <cbmpc/api/ecdsa_mp.h>
#include <cbmpc/core/job.h>
#include <cbmpc/api/pve.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

namespace {

using namespace coinbase;

[[noreturn]] void die(const std::string& msg) {
  std::cerr << "ecdsa_mp_pve_backup demo failure: " << msg << "\n";
  std::exit(1);
}

void require(bool ok, const std::string& msg) {
  if (!ok) die(msg);
}

void require_rv(error_t got, error_t want, const std::string& msg) {
  if (got != want) die(msg + " (got=0x" + std::to_string(uint32_t(got)) + ")");
}

// -------------------------
// Minimal in-memory MP transport
// -------------------------

struct channel_t {
  std::mutex m;
  std::condition_variable cv;
  std::deque<buf_t> q;
};

struct in_memory_network_t {
  const int n;
  std::vector<std::vector<std::shared_ptr<channel_t>>> ch;
  std::atomic<bool> aborted{false};

  explicit in_memory_network_t(int n_) : n(n_), ch(static_cast<size_t>(n_), std::vector<std::shared_ptr<channel_t>>(static_cast<size_t>(n_))) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (i == j) continue;
        ch[static_cast<size_t>(i)][static_cast<size_t>(j)] = std::make_shared<channel_t>();
      }
    }
  }

  void abort() {
    aborted.store(true);
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        auto& c = ch[static_cast<size_t>(i)][static_cast<size_t>(j)];
        if (c) c->cv.notify_all();
      }
    }
  }
};

class in_memory_transport_t final : public coinbase::api::data_transport_i {
 public:
  in_memory_transport_t(int self, std::shared_ptr<in_memory_network_t> net) : self_(self), net_(std::move(net)) {}

  error_t send(coinbase::api::party_idx_t receiver, mem_t msg) override {
    if (!net_) return E_GENERAL;
    if (net_->aborted.load()) return E_NET_GENERAL;
    if (receiver < 0 || receiver >= net_->n || receiver == self_) return E_BADARG;
    auto c = net_->ch[static_cast<size_t>(self_)][static_cast<size_t>(receiver)];
    if (!c) return E_GENERAL;
    {
      std::lock_guard<std::mutex> lk(c->m);
      c->q.emplace_back(msg);
    }
    c->cv.notify_one();
    return SUCCESS;
  }

  error_t receive(coinbase::api::party_idx_t sender, buf_t& msg) override {
    if (!net_ || sender < 0 || sender >= net_->n || sender == self_) return E_BADARG;
    auto c = net_->ch[static_cast<size_t>(sender)][static_cast<size_t>(self_)];
    if (!c) return E_GENERAL;
    std::unique_lock<std::mutex> lk(c->m);
    c->cv.wait(lk, [&] { return net_->aborted.load() || !c->q.empty(); });
    if (net_->aborted.load() && c->q.empty()) return E_NET_GENERAL;
    msg = std::move(c->q.front());
    c->q.pop_front();
    return SUCCESS;
  }

  error_t receive_all(const std::vector<coinbase::api::party_idx_t>& senders, std::vector<buf_t>& msgs) override {
    msgs.clear();
    msgs.resize(senders.size());
    for (size_t i = 0; i < senders.size(); i++) {
      const error_t rv = receive(senders[i], msgs[i]);
      if (rv) return rv;
    }
    return SUCCESS;
  }

 private:
  const int self_;
  std::shared_ptr<in_memory_network_t> net_;
};

template <typename F>
static void run_mp(const std::shared_ptr<in_memory_network_t>& net, int n, F&& f, std::vector<error_t>& out_rv) {
  out_rv.assign(static_cast<size_t>(n), UNINITIALIZED_ERROR);
  std::atomic<bool> aborted{false};
  std::vector<std::thread> threads;
  threads.reserve(static_cast<size_t>(n));

  for (int i = 0; i < n; i++) {
    threads.emplace_back([&, i] {
      out_rv[static_cast<size_t>(i)] = f(i);
      if (out_rv[static_cast<size_t>(i)] && !aborted.exchange(true)) {
        net->abort();
      }
    });
  }
  for (auto& t : threads) t.join();
}

// -------------------------
// OpenSSL verify helper (DER ECDSA, secp256k1/p256)
// -------------------------

struct ec_group_deleter_t {
  void operator()(EC_GROUP* g) const { EC_GROUP_free(g); }
};
struct ec_point_deleter_t {
  void operator()(EC_POINT* p) const { EC_POINT_free(p); }
};
struct ossl_param_bld_deleter_t {
  void operator()(OSSL_PARAM_BLD* bld) const { OSSL_PARAM_BLD_free(bld); }
};
struct ossl_param_deleter_t {
  void operator()(OSSL_PARAM* p) const { OSSL_PARAM_free(p); }
};
struct evp_pkey_ctx_deleter_t {
  void operator()(EVP_PKEY_CTX* ctx) const { EVP_PKEY_CTX_free(ctx); }
};
struct evp_pkey_deleter_t {
  void operator()(EVP_PKEY* pkey) const { EVP_PKEY_free(pkey); }
};

static int curve_to_nid(coinbase::api::curve_id curve) {
  switch (curve) {
    case coinbase::api::curve_id::secp256k1:
      return NID_secp256k1;
    case coinbase::api::curve_id::p256:
      return NID_X9_62_prime256v1;
    case coinbase::api::curve_id::ed25519:
      return NID_undef;
  }
  return NID_undef;
}

static const char* nid_to_group_name(int nid) {
  switch (nid) {
    case NID_secp256k1:
      return SN_secp256k1;
    case NID_X9_62_prime256v1:
      return SN_X9_62_prime256v1;
  }
  return nullptr;
}

static bool verify_ecdsa_sig_der(coinbase::api::curve_id curve, mem_t pub_key_compressed, mem_t msg_hash, mem_t sig_der) {
  const int nid = curve_to_nid(curve);
  if (nid == NID_undef) return false;
  const char* group_name = nid_to_group_name(nid);
  if (!group_name) return false;

  std::unique_ptr<EC_GROUP, ec_group_deleter_t> group(EC_GROUP_new_by_curve_name(nid));
  if (!group) return false;
  std::unique_ptr<EC_POINT, ec_point_deleter_t> Q(EC_POINT_new(group.get()));
  if (!Q) return false;

  if (EC_POINT_oct2point(group.get(), Q.get(), pub_key_compressed.data, static_cast<size_t>(pub_key_compressed.size),
                         /*ctx=*/nullptr) != 1) {
    return false;
  }

  const size_t oct_len = EC_POINT_point2oct(group.get(), Q.get(), POINT_CONVERSION_UNCOMPRESSED, /*buf=*/nullptr,
                                            /*len=*/0, /*ctx=*/nullptr);
  if (oct_len == 0) return false;
  std::vector<uint8_t> pub_oct(oct_len);
  if (EC_POINT_point2oct(group.get(), Q.get(), POINT_CONVERSION_UNCOMPRESSED, pub_oct.data(), pub_oct.size(),
                         /*ctx=*/nullptr) != oct_len) {
    return false;
  }

  std::unique_ptr<OSSL_PARAM_BLD, ossl_param_bld_deleter_t> bld(OSSL_PARAM_BLD_new());
  if (!bld) return false;
  if (OSSL_PARAM_BLD_push_utf8_string(bld.get(), "group", group_name, 0) <= 0) return false;
  if (OSSL_PARAM_BLD_push_octet_string(bld.get(), "pub", pub_oct.data(), pub_oct.size()) <= 0) return false;
  std::unique_ptr<OSSL_PARAM, ossl_param_deleter_t> params(OSSL_PARAM_BLD_to_param(bld.get()));
  if (!params) return false;

  std::unique_ptr<EVP_PKEY_CTX, evp_pkey_ctx_deleter_t> fromdata_ctx(EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr));
  if (!fromdata_ctx) return false;
  if (EVP_PKEY_fromdata_init(fromdata_ctx.get()) <= 0) return false;
  EVP_PKEY* pkey_raw = nullptr;
  if (EVP_PKEY_fromdata(fromdata_ctx.get(), &pkey_raw, EVP_PKEY_PUBLIC_KEY, params.get()) <= 0) return false;
  std::unique_ptr<EVP_PKEY, evp_pkey_deleter_t> pkey(pkey_raw);

  std::unique_ptr<EVP_PKEY_CTX, evp_pkey_ctx_deleter_t> verify_ctx(EVP_PKEY_CTX_new(pkey.get(), nullptr));
  if (!verify_ctx) return false;
  if (EVP_PKEY_verify_init(verify_ctx.get()) <= 0) return false;
  const int v = EVP_PKEY_verify(verify_ctx.get(), sig_der.data, static_cast<size_t>(sig_der.size), msg_hash.data,
                                static_cast<size_t>(msg_hash.size));
  return v == 1;
}

static buf_t make_msg_hash32(uint8_t seed) {
  buf_t msg_hash(32);
  for (int i = 0; i < msg_hash.size(); i++) msg_hash[i] = static_cast<uint8_t>(seed + i);
  return msg_hash;
}

}  // namespace

int main(int /*argc*/, const char* /*argv*/[]) {
  std::cout << std::boolalpha;
  std::cout << "============= ECDSA-MP + PVE backup demo (api-only) =============\n";

  const coinbase::api::curve_id curve = coinbase::api::curve_id::secp256k1;

  // Parties: p0..p4
  const int n = 5;
  std::vector<std::string> names = {"p0", "p1", "p2", "p3", "p4"};
  std::vector<std::string_view> name_views;
  name_views.reserve(names.size());
  for (const auto& name : names) name_views.emplace_back(name);

  // THRESHOLD[3](p0, p1, p2, p3, p4)
  const coinbase::api::access_structure_t ac = coinbase::api::access_structure_t::Threshold(
      3, {
             coinbase::api::access_structure_t::leaf(names[0]),
             coinbase::api::access_structure_t::leaf(names[1]),
             coinbase::api::access_structure_t::leaf(names[2]),
             coinbase::api::access_structure_t::leaf(names[3]),
             coinbase::api::access_structure_t::leaf(names[4]),
         });

  // Only 3 parties actively contribute to DKG/refresh (all 5 must be online to run).
  const std::vector<std::string_view> quorum_party_names = {names[0], names[1], names[2]};

  // -------------------------
  // Step 1: DKG (access-structure)
  // -------------------------
  std::cout << "\n[1] DKG(ac): 5 parties, threshold 3-of-5, quorum contributors: {p0,p1,p2}\n";

  auto net_dkg = std::make_shared<in_memory_network_t>(n);
  std::vector<std::unique_ptr<in_memory_transport_t>> transports;
  transports.reserve(static_cast<size_t>(n));
  for (int i = 0; i < n; i++) transports.emplace_back(std::make_unique<in_memory_transport_t>(i, net_dkg));

  std::vector<buf_t> key_blobs(n);
  std::vector<buf_t> sids(n);
  std::vector<error_t> rvs;

  run_mp(net_dkg, n, [&](int i) {
    coinbase::api::job_mp_t job{static_cast<coinbase::api::party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
    return coinbase::api::ecdsa_mp::dkg_ac(job, curve, sids[static_cast<size_t>(i)], ac, quorum_party_names,
                                           key_blobs[static_cast<size_t>(i)]);
  }, rvs);
  for (int i = 0; i < n; i++) require_rv(rvs[static_cast<size_t>(i)], SUCCESS, "dkg_ac p" + std::to_string(i));
  for (int i = 1; i < n; i++) require(sids[0] == sids[static_cast<size_t>(i)], "DKG sid mismatch");

  buf_t pub;
  require_rv(coinbase::api::ecdsa_mp::get_public_key_compressed(key_blobs[0], pub), SUCCESS, "get public key");
  for (int i = 1; i < n; i++) {
    buf_t pub_i;
    require_rv(coinbase::api::ecdsa_mp::get_public_key_compressed(key_blobs[static_cast<size_t>(i)], pub_i), SUCCESS,
               "get public key");
    require(pub_i == pub, "public key mismatch across parties");
  }
  std::cout << "public key bytes: " << pub.size() << "\n";

  // -------------------------
  // Step 2: Sign, then refresh
  // -------------------------
  std::cout << "\n[2] Sign(ac) with quorum {p0,p1,p2}\n";
  {
    const int qn = static_cast<int>(quorum_party_names.size());
    auto net_sign = std::make_shared<in_memory_network_t>(qn);
    std::vector<std::unique_ptr<in_memory_transport_t>> sign_transports;
    sign_transports.reserve(static_cast<size_t>(qn));
    for (int i = 0; i < qn; i++) sign_transports.emplace_back(std::make_unique<in_memory_transport_t>(i, net_sign));

    const buf_t msg_hash = make_msg_hash32(/*seed=*/0x11);
    std::vector<buf_t> sigs(static_cast<size_t>(qn));

    run_mp(net_sign, qn, [&](int i) {
      coinbase::api::job_mp_t job{static_cast<coinbase::api::party_idx_t>(i), quorum_party_names,
                                 *sign_transports[static_cast<size_t>(i)]};
      // Map quorum index -> original party index (p0,p1,p2 == 0,1,2)
      return coinbase::api::ecdsa_mp::sign_ac(job, key_blobs[static_cast<size_t>(i)], ac, msg_hash,
                                              /*sig_receiver=*/0, sigs[static_cast<size_t>(i)]);
    }, rvs);
    for (int i = 0; i < qn; i++) require_rv(rvs[static_cast<size_t>(i)], SUCCESS, "sign_ac");

    require(!sigs[0].empty(), "signature should be returned on receiver (p0)");
    require(verify_ecdsa_sig_der(curve, mem_t(pub.data(), pub.size()), mem_t(msg_hash.data(), msg_hash.size()),
                                 mem_t(sigs[0].data(), sigs[0].size())),
            "signature verify failed");
    std::cout << "sign/verify ok\n";
  }

  std::cout << "\n[2] Refresh(ac): 5 parties, quorum contributors: {p0,p1,p2}\n";
  std::vector<buf_t> refreshed(n);
  std::vector<buf_t> refresh_sids(n);
  {
    auto net_refresh = std::make_shared<in_memory_network_t>(n);
    // Reuse transport objects but point them at a fresh network by rebuilding.
    transports.clear();
    for (int i = 0; i < n; i++) transports.emplace_back(std::make_unique<in_memory_transport_t>(i, net_refresh));

    run_mp(net_refresh, n, [&](int i) {
      coinbase::api::job_mp_t job{static_cast<coinbase::api::party_idx_t>(i), name_views, *transports[static_cast<size_t>(i)]};
      return coinbase::api::ecdsa_mp::refresh_ac(job, refresh_sids[static_cast<size_t>(i)], key_blobs[static_cast<size_t>(i)],
                                                 ac, quorum_party_names, refreshed[static_cast<size_t>(i)]);
    }, rvs);
    for (int i = 0; i < n; i++) require_rv(rvs[static_cast<size_t>(i)], SUCCESS, "refresh_ac p" + std::to_string(i));
    for (int i = 1; i < n; i++) require(refresh_sids[0] == refresh_sids[static_cast<size_t>(i)], "refresh sid mismatch");

    for (int i = 0; i < n; i++) {
      buf_t pub_i;
      require_rv(coinbase::api::ecdsa_mp::get_public_key_compressed(refreshed[static_cast<size_t>(i)], pub_i), SUCCESS,
                 "get public key refreshed");
      require(pub_i == pub, "public key changed after refresh");
    }
    std::cout << "refresh ok (public key stable)\n";
  }

  // -------------------------
  // Step 3: PVE backup of private scalar shares (verifiable via Qi_self)
  // -------------------------
  std::cout << "\n[3] Backup each party's private scalar x_share using PVE (verifiable)\n";

  std::vector<buf_t> redacted(n);
  std::vector<buf_t> ct(n);
  std::vector<buf_t> ek(n);
  std::vector<buf_t> dk(n);
  std::vector<buf_t> Qi_selfs(n);
  std::vector<std::string> labels(n);

  for (int i = 0; i < n; i++) {
    require_rv(coinbase::api::ecdsa_mp::get_public_share_compressed(refreshed[static_cast<size_t>(i)],
                                                                    Qi_selfs[static_cast<size_t>(i)]),
               SUCCESS, "get_public_share_compressed");

    buf_t x_fixed;
    require_rv(coinbase::api::ecdsa_mp::detach_private_scalar(refreshed[static_cast<size_t>(i)],
                                                             redacted[static_cast<size_t>(i)], x_fixed),
               SUCCESS, "detach_private_scalar");

    require_rv(coinbase::api::pve::generate_base_pke_rsa_keypair(ek[static_cast<size_t>(i)], dk[static_cast<size_t>(i)]),
               SUCCESS, "generate_base_pke_rsa_keypair");

    labels[static_cast<size_t>(i)] = std::string("ecdsa-mp-demo:pve-backup:") + names[static_cast<size_t>(i)];
    const mem_t label_mem(labels[static_cast<size_t>(i)]);

    require_rv(coinbase::api::pve::encrypt(curve, mem_t(ek[static_cast<size_t>(i)].data(), ek[static_cast<size_t>(i)].size()),
                                          label_mem, mem_t(x_fixed.data(), x_fixed.size()),
                                          ct[static_cast<size_t>(i)]),
               SUCCESS, "pve::encrypt");

    require_rv(coinbase::api::pve::verify(curve, mem_t(ek[static_cast<size_t>(i)].data(), ek[static_cast<size_t>(i)].size()),
                                          mem_t(ct[static_cast<size_t>(i)].data(), ct[static_cast<size_t>(i)].size()),
                                          mem_t(Qi_selfs[static_cast<size_t>(i)].data(),
                                                Qi_selfs[static_cast<size_t>(i)].size()),
                                          label_mem),
               SUCCESS, "pve::verify");
  }
  std::cout << "PVE backup + verification ok\n";

  // -------------------------
  // Step 4: Simulate party p1 losing the private scalar share (restore from PVE)
  // Step 5: Sign again with quorum {p0,p1,p2}
  // -------------------------
  std::cout << "\n[4] Party p1 loses private scalar; restore from PVE and attach into public blob\n";

  auto restore_party = [&](int party_idx, buf_t& out_full_key_blob) {
    const mem_t label_mem(labels[static_cast<size_t>(party_idx)]);
    buf_t x_out;
    require_rv(coinbase::api::pve::decrypt(curve, mem_t(dk[static_cast<size_t>(party_idx)].data(), dk[static_cast<size_t>(party_idx)].size()),
                                          mem_t(ek[static_cast<size_t>(party_idx)].data(), ek[static_cast<size_t>(party_idx)].size()),
                                          mem_t(ct[static_cast<size_t>(party_idx)].data(), ct[static_cast<size_t>(party_idx)].size()),
                                          label_mem, x_out),
               SUCCESS, "pve::decrypt");
    require_rv(coinbase::api::ecdsa_mp::attach_private_scalar(redacted[static_cast<size_t>(party_idx)],
                                                             mem_t(x_out.data(), x_out.size()),
                                                             mem_t(Qi_selfs[static_cast<size_t>(party_idx)].data(),
                                                                   Qi_selfs[static_cast<size_t>(party_idx)].size()),
                                                             out_full_key_blob),
               SUCCESS, "attach_private_scalar");
  };

  // Restore quorum parties (p0, p1, p2). Only p1 is "lost", but restoring all three
  // keeps the demo uniform after redaction.
  std::vector<buf_t> restored_quorum(3);
  restore_party(/*p0=*/0, restored_quorum[0]);
  restore_party(/*p1=*/1, restored_quorum[1]);
  restore_party(/*p2=*/2, restored_quorum[2]);

  std::cout << "\n[5] Sign(ac) again with quorum {p0,p1,p2} using restored key blobs\n";
  {
    const int qn = 3;
    auto net_sign2 = std::make_shared<in_memory_network_t>(qn);
    std::vector<std::unique_ptr<in_memory_transport_t>> sign_transports;
    sign_transports.reserve(static_cast<size_t>(qn));
    for (int i = 0; i < qn; i++) sign_transports.emplace_back(std::make_unique<in_memory_transport_t>(i, net_sign2));

    const buf_t msg_hash = make_msg_hash32(/*seed=*/0x33);
    std::vector<buf_t> sigs(static_cast<size_t>(qn));

    run_mp(net_sign2, qn, [&](int i) {
      coinbase::api::job_mp_t job{static_cast<coinbase::api::party_idx_t>(i), quorum_party_names,
                                 *sign_transports[static_cast<size_t>(i)]};
      return coinbase::api::ecdsa_mp::sign_ac(job, restored_quorum[static_cast<size_t>(i)], ac, msg_hash,
                                              /*sig_receiver=*/0, sigs[static_cast<size_t>(i)]);
    }, rvs);
    for (int i = 0; i < qn; i++) require_rv(rvs[static_cast<size_t>(i)], SUCCESS, "sign_ac (restored)");

    require(!sigs[0].empty(), "signature should be returned on receiver (p0)");
    require(verify_ecdsa_sig_der(curve, mem_t(pub.data(), pub.size()), mem_t(msg_hash.data(), msg_hash.size()),
                                 mem_t(sigs[0].data(), sigs[0].size())),
            "restored signature verify failed");
    std::cout << "sign/verify after restore ok\n";
  }

  std::cout << "\nDone.\n";
  return 0;
}

