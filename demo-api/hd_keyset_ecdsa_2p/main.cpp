#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/evp.h>
#include <openssl/param_build.h>
#include <openssl/params.h>

#include <cbmpc/api/curve.h>
#include <cbmpc/api/ecdsa_2p.h>
#include <cbmpc/api/hd_keyset_ecdsa_2p.h>
#include <cbmpc/core/job.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

namespace {

using namespace coinbase;

[[noreturn]] void die(const std::string& msg) {
  std::cerr << "hd_keyset_ecdsa_2p demo failure: " << msg << "\n";
  std::exit(1);
}

void require(bool ok, const std::string& msg) {
  if (!ok) die(msg);
}

void require_rv(error_t got, error_t want, const std::string& msg) {
  if (got != want) die(msg + " (got=0x" + std::to_string(uint32_t(got)) + ")");
}

// Minimal in-memory 2-party transport.
struct channel_t {
  std::mutex m;
  std::condition_variable cv;
  std::deque<buf_t> q;
};

struct in_memory_network_t {
  std::shared_ptr<channel_t> ch[2][2];
  std::atomic<bool> aborted{false};
  in_memory_network_t() {
    ch[0][1] = std::make_shared<channel_t>();
    ch[1][0] = std::make_shared<channel_t>();
  }

  void abort() {
    aborted.store(true);
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        if (ch[i][j]) ch[i][j]->cv.notify_all();
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
    if (receiver < 0 || receiver > 1 || receiver == self_) return E_BADARG;
    auto c = net_->ch[self_][receiver];
    if (!c) return E_GENERAL;
    {
      std::lock_guard<std::mutex> lk(c->m);
      c->q.emplace_back(msg);
    }
    c->cv.notify_one();
    return SUCCESS;
  }

  error_t receive(coinbase::api::party_idx_t sender, buf_t& msg) override {
    if (!net_ || sender < 0 || sender > 1 || sender == self_) return E_BADARG;
    auto c = net_->ch[sender][self_];
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
      error_t rv = receive(senders[i], msgs[i]);
      if (rv) return rv;
    }
    return SUCCESS;
  }

 private:
  const int self_;
  std::shared_ptr<in_memory_network_t> net_;
};

template <typename F1, typename F2>
void run_2pc(in_memory_network_t* net, F1&& f1, F2&& f2, error_t& out_rv1, error_t& out_rv2) {
  std::thread t1([&] {
    out_rv1 = f1();
    if (out_rv1 && net) net->abort();
  });
  std::thread t2([&] {
    out_rv2 = f2();
    if (out_rv2 && net) net->abort();
  });
  t1.join();
  t2.join();
}

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

static bool verify_ecdsa_sig_der(coinbase::api::curve_id curve, mem_t pub_key_compressed, mem_t msg_hash, mem_t sig_der) {
  const int nid = curve_to_nid(curve);
  if (nid == NID_undef) return false;
  const char* group_name = nid_to_group_name(nid);
  if (!group_name) return false;

  // Decode the compressed public key into an EC_POINT, then re-encode it as an
  // uncompressed point (the provider-based EVP_PKEY import expects an encoded
  // point blob).
  std::unique_ptr<EC_GROUP, ec_group_deleter_t> group(EC_GROUP_new_by_curve_name(nid));
  if (!group) return false;
  std::unique_ptr<EC_POINT, ec_point_deleter_t> Q(EC_POINT_new(group.get()));
  if (!Q) return false;

  if (EC_POINT_oct2point(group.get(), Q.get(), pub_key_compressed.data, static_cast<size_t>(pub_key_compressed.size),
                         /*ctx=*/nullptr) != 1) {
    return false;
  }

  const size_t oct_len =
      EC_POINT_point2oct(group.get(), Q.get(), POINT_CONVERSION_UNCOMPRESSED, /*buf=*/nullptr, /*len=*/0, /*ctx=*/nullptr);
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

void demo_curve(coinbase::api::curve_id curve) {
  std::cout << "\n=== HD keyset ECDSA-2P (api) curve=" << (curve == coinbase::api::curve_id::secp256k1 ? "secp256k1" : "p256")
            << " ===\n";

  auto net = std::make_shared<in_memory_network_t>();
  in_memory_transport_t t1(/*self=*/0, net);
  in_memory_transport_t t2(/*self=*/1, net);

  const coinbase::api::job_2p_t job1{coinbase::api::party_2p_t::p1, "p1", "p2", t1};
  const coinbase::api::job_2p_t job2{coinbase::api::party_2p_t::p2, "p1", "p2", t2};

  buf_t keyset1;
  buf_t keyset2;
  error_t rv1 = UNINITIALIZED_ERROR;
  error_t rv2 = UNINITIALIZED_ERROR;

  run_2pc(net.get(), [&] { return coinbase::api::hd_keyset_ecdsa_2p::dkg(job1, curve, keyset1); },
          [&] { return coinbase::api::hd_keyset_ecdsa_2p::dkg(job2, curve, keyset2); }, rv1, rv2);
  require_rv(rv1, SUCCESS, "dkg p1");
  require_rv(rv2, SUCCESS, "dkg p2");

  buf_t root_pub1;
  buf_t root_pub2;
  require_rv(coinbase::api::hd_keyset_ecdsa_2p::extract_root_public_key_compressed(keyset1, root_pub1), SUCCESS,
             "extract root pub p1");
  require_rv(coinbase::api::hd_keyset_ecdsa_2p::extract_root_public_key_compressed(keyset2, root_pub2), SUCCESS,
             "extract root pub p2");
  require(root_pub1 == root_pub2, "root pub keys must match");
  std::cout << "root public key bytes: " << root_pub1.size() << "\n";

  // Derivation paths.
  coinbase::api::hd_keyset_ecdsa_2p::bip32_path_t hard;
  hard.indices = {0x8000002c, 0x80000000, 0x80000000};

  std::vector<coinbase::api::hd_keyset_ecdsa_2p::bip32_path_t> non_hard;
  non_hard.push_back(coinbase::api::hd_keyset_ecdsa_2p::bip32_path_t{{0, 0}});
  non_hard.push_back(coinbase::api::hd_keyset_ecdsa_2p::bip32_path_t{{0, 1}});

  std::vector<buf_t> derived1;
  std::vector<buf_t> derived2;
  buf_t sid1;
  buf_t sid2;
  run_2pc(
      net.get(),
      [&] { return coinbase::api::hd_keyset_ecdsa_2p::derive_ecdsa_2p_keys(job1, keyset1, hard, non_hard, sid1, derived1); },
      [&] { return coinbase::api::hd_keyset_ecdsa_2p::derive_ecdsa_2p_keys(job2, keyset2, hard, non_hard, sid2, derived2); },
      rv1, rv2);
  require_rv(rv1, SUCCESS, "derive p1");
  require_rv(rv2, SUCCESS, "derive p2");
  require(sid1 == sid2, "sid must match");
  std::cout << "derived keys: " << derived1.size() << "\n";

  for (size_t i = 0; i < derived1.size(); i++) {
    buf_t pub_a;
    buf_t pub_b;
    require_rv(coinbase::api::ecdsa_2p::get_public_key_compressed(derived1[i], pub_a), SUCCESS,
               "extract derived pub p1");
    require_rv(coinbase::api::ecdsa_2p::get_public_key_compressed(derived2[i], pub_b), SUCCESS,
               "extract derived pub p2");
    require(pub_a == pub_b, "derived pubkeys must match across parties");
  }

  // Sign with derived key #0.
  buf_t msg_hash(32);
  for (int i = 0; i < msg_hash.size(); i++) msg_hash[i] = static_cast<uint8_t>(0x42 + i);

  buf_t sig1;
  buf_t sig2;
  buf_t sid3;
  buf_t sid4;
  run_2pc(net.get(), [&] { return coinbase::api::ecdsa_2p::sign(job1, derived1[0], msg_hash, sid3, sig1); },
          [&] { return coinbase::api::ecdsa_2p::sign(job2, derived2[0], msg_hash, sid4, sig2); }, rv1, rv2);
  require_rv(rv1, SUCCESS, "sign p1");
  require_rv(rv2, SUCCESS, "sign p2");
  require(sid3 == sid4, "sign sid mismatch");
  require(!sig1.empty(), "signature should be returned on p1");

  buf_t derived_pub;
  require_rv(coinbase::api::ecdsa_2p::get_public_key_compressed(derived1[0], derived_pub), SUCCESS,
             "extract pub for verify");
  require(verify_ecdsa_sig_der(curve, derived_pub, msg_hash, sig1), "signature verification failed");
  std::cout << "sign/verify ok\n";

  // Refresh keyset shares and derive again.
  buf_t keyset1_ref;
  buf_t keyset2_ref;
  run_2pc(net.get(), [&] { return coinbase::api::hd_keyset_ecdsa_2p::refresh(job1, keyset1, keyset1_ref); },
          [&] { return coinbase::api::hd_keyset_ecdsa_2p::refresh(job2, keyset2, keyset2_ref); }, rv1, rv2);
  require_rv(rv1, SUCCESS, "refresh p1");
  require_rv(rv2, SUCCESS, "refresh p2");

  std::vector<buf_t> derived1_ref;
  std::vector<buf_t> derived2_ref;
  buf_t sid5;
  buf_t sid6;
  run_2pc(
      net.get(),
      [&] {
        return coinbase::api::hd_keyset_ecdsa_2p::derive_ecdsa_2p_keys(job1, keyset1_ref, hard, non_hard, sid5,
                                                                       derived1_ref);
      },
      [&] {
        return coinbase::api::hd_keyset_ecdsa_2p::derive_ecdsa_2p_keys(job2, keyset2_ref, hard, non_hard, sid6,
                                                                       derived2_ref);
      },
      rv1, rv2);
  require_rv(rv1, SUCCESS, "derive after refresh p1");
  require_rv(rv2, SUCCESS, "derive after refresh p2");

  for (size_t i = 0; i < derived1.size(); i++) {
    buf_t pub_old;
    buf_t pub_new;
    require_rv(coinbase::api::ecdsa_2p::get_public_key_compressed(derived1[i], pub_old), SUCCESS, "extract pub old");
    require_rv(coinbase::api::ecdsa_2p::get_public_key_compressed(derived1_ref[i], pub_new), SUCCESS,
               "extract pub new");
    require(pub_old == pub_new, "derived pubkey changed across refresh (unexpected)");
  }
  std::cout << "refresh/derive stable ok\n";
}

}  // namespace

int main(int /*argc*/, const char* /*argv*/[]) {
  std::cout << "============= HD Keyset ECDSA 2P Demo (api-only) =============\n";
  demo_curve(coinbase::api::curve_id::secp256k1);
  demo_curve(coinbase::api::curve_id::p256);
  return 0;
}
