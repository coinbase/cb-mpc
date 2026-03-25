#include <array>
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

#include <cbmpc/api/pve_base_pke.h>
#include <cbmpc/api/pve_batch_single_recipient.h>
#include <cbmpc/api/schnorr_2p.h>
#include <cbmpc/core/job.h>
#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>

namespace {

using namespace coinbase;

[[noreturn]] void die(const std::string& msg) {
  std::cerr << "schnorr_2p_pve_batch_backup demo failure: " << msg << "\n";
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

static buf_t make_msg32(uint8_t seed) {
  std::array<uint8_t, 32> msg{};
  for (size_t i = 0; i < msg.size(); i++) msg[i] = static_cast<uint8_t>(seed + i);
  return buf_t(msg.data(), static_cast<int>(msg.size()));
}

}  // namespace

int main() {
  using coinbase::api::curve_id;
  using coinbase::api::schnorr_2p::party_t;

  std::cout << "=== Schnorr-2P (api) + PVE batch backup (5x DKG) ===\n";

  constexpr int batch_count = 5;
  const curve_id curve = curve_id::secp256k1;
  const mem_t label("schnorr-2p-demo:pve-batch-backup");

  // Base-PKE keypair (used to encrypt and decrypt the batch ciphertext).
  buf_t ek;
  buf_t dk;
  require_rv(coinbase::api::pve::generate_base_pke_rsa_keypair(ek, dk), SUCCESS, "generate_base_pke_rsa_keypair");

  std::vector<buf_t> key_p2(batch_count);
  std::vector<buf_t> public_p1(batch_count);
  std::vector<buf_t> x_fixed(batch_count);
  std::vector<buf_t> Qi_p1(batch_count);
  std::vector<buf_t> pubkeys(batch_count);

  // Run DKG 5 times and detach p1's scalar share each time.
  for (int k = 0; k < batch_count; k++) {
    auto net = std::make_shared<in_memory_network_t>();
    in_memory_transport_t t1(/*self=*/0, net);
    in_memory_transport_t t2(/*self=*/1, net);

    const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
    const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};

    buf_t key1;
    buf_t key2;
    error_t rv1 = UNINITIALIZED_ERROR;
    error_t rv2 = UNINITIALIZED_ERROR;
    run_2pc(net.get(), [&] { return coinbase::api::schnorr_2p::dkg(job1, curve, key1); },
            [&] { return coinbase::api::schnorr_2p::dkg(job2, curve, key2); }, rv1, rv2);
    require_rv(rv1, SUCCESS, "dkg p1 (k=" + std::to_string(k) + ")");
    require_rv(rv2, SUCCESS, "dkg p2 (k=" + std::to_string(k) + ")");

    // Extract and sanity-check global public key.
    buf_t pub1;
    buf_t pub2;
    require_rv(coinbase::api::schnorr_2p::get_public_key_compressed(key1, pub1), SUCCESS, "get_public_key_compressed p1");
    require_rv(coinbase::api::schnorr_2p::get_public_key_compressed(key2, pub2), SUCCESS, "get_public_key_compressed p2");
    require(pub1 == pub2, "public key mismatch (k=" + std::to_string(k) + ")");
    pubkeys[static_cast<size_t>(k)] = pub1;

    // Capture p1's share public point before detaching (public blobs do not carry it).
    require_rv(coinbase::api::schnorr_2p::get_public_share_compressed(key1, Qi_p1[static_cast<size_t>(k)]), SUCCESS,
               "get_public_share_compressed p1");

    // Detach p1 scalar share into (public blob, scalar).
    require_rv(coinbase::api::schnorr_2p::detach_private_scalar(key1, public_p1[static_cast<size_t>(k)],
                                                                x_fixed[static_cast<size_t>(k)]),
               SUCCESS, "detach_private_scalar p1");
    require(x_fixed[static_cast<size_t>(k)].size() == 32, "unexpected scalar size");

    // Keep p2's full key blob for later signing.
    key_p2[static_cast<size_t>(k)] = key2;
  }

  // Prepare batch scalars and corresponding share points.
  std::vector<mem_t> xs;
  xs.reserve(batch_count);
  std::vector<mem_t> Qs;
  Qs.reserve(batch_count);
  for (int k = 0; k < batch_count; k++) {
    xs.emplace_back(x_fixed[static_cast<size_t>(k)].data(), x_fixed[static_cast<size_t>(k)].size());
    Qs.emplace_back(Qi_p1[static_cast<size_t>(k)].data(), Qi_p1[static_cast<size_t>(k)].size());
  }

  // Encrypt + verify batch ciphertext.
  buf_t ct;
  require_rv(coinbase::api::pve::encrypt_batch(curve, mem_t(ek.data(), ek.size()), label, xs, ct), SUCCESS,
             "pve::encrypt_batch");

  int got_count = 0;
  require_rv(coinbase::api::pve::get_batch_count(mem_t(ct.data(), ct.size()), got_count), SUCCESS, "pve::get_batch_count");
  require(got_count == batch_count, "unexpected batch_count");

  buf_t label_out;
  require_rv(coinbase::api::pve::get_Label_batch(mem_t(ct.data(), ct.size()), label_out), SUCCESS, "pve::get_Label_batch");
  require(label_out == buf_t(label), "label mismatch");

  require_rv(coinbase::api::pve::verify_batch(curve, mem_t(ek.data(), ek.size()), mem_t(ct.data(), ct.size()), Qs, label),
             SUCCESS, "pve::verify_batch");

  // Decrypt batch ciphertext (simulate restoring from backup service).
  std::vector<buf_t> xs_out;
  require_rv(coinbase::api::pve::decrypt_batch(curve, mem_t(dk.data(), dk.size()), mem_t(ek.data(), ek.size()),
                                               mem_t(ct.data(), ct.size()), label, xs_out),
             SUCCESS, "pve::decrypt_batch");
  require(static_cast<int>(xs_out.size()) == batch_count, "decrypt_batch returned wrong count");

  // Restore p1 key blobs and sign once per key.
  for (int k = 0; k < batch_count; k++) {
    buf_t restored_p1;
    require_rv(coinbase::api::schnorr_2p::attach_private_scalar(public_p1[static_cast<size_t>(k)],
                                                                mem_t(xs_out[static_cast<size_t>(k)].data(),
                                                                      xs_out[static_cast<size_t>(k)].size()),
                                                                mem_t(Qi_p1[static_cast<size_t>(k)].data(),
                                                                      Qi_p1[static_cast<size_t>(k)].size()),
                                                                restored_p1),
               SUCCESS, "attach_private_scalar p1");

    // Sign with restored p1 + original p2.
    auto net = std::make_shared<in_memory_network_t>();
    in_memory_transport_t t1(/*self=*/0, net);
    in_memory_transport_t t2(/*self=*/1, net);

    const coinbase::api::job_2p_t job1{party_t::p1, "p1", "p2", t1};
    const coinbase::api::job_2p_t job2{party_t::p2, "p1", "p2", t2};

    const buf_t msg = make_msg32(static_cast<uint8_t>(0x11 + k));
    buf_t sig1;
    buf_t sig2;
    error_t rv1 = UNINITIALIZED_ERROR;
    error_t rv2 = UNINITIALIZED_ERROR;
    run_2pc(net.get(), [&] { return coinbase::api::schnorr_2p::sign(job1, restored_p1, msg, sig1); },
            [&] { return coinbase::api::schnorr_2p::sign(job2, key_p2[static_cast<size_t>(k)], msg, sig2); }, rv1, rv2);
    require_rv(rv1, SUCCESS, "sign p1 (k=" + std::to_string(k) + ")");
    require_rv(rv2, SUCCESS, "sign p2 (k=" + std::to_string(k) + ")");

    require(sig1.size() == 64, "unexpected signature size");
    require(sig2.empty(), "signature should be returned only on p1");

    // Public key should remain stable after restore.
    buf_t pub_restored;
    require_rv(coinbase::api::schnorr_2p::get_public_key_compressed(restored_p1, pub_restored), SUCCESS,
               "get_public_key_compressed restored p1");
    require(pub_restored == pubkeys[static_cast<size_t>(k)], "restored public key mismatch");
  }

  std::cout << "Done.\n";
  return 0;
}

