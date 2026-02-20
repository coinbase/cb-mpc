#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <cbmpc/core/error.h>

#include "mpc_job_session.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;

[[noreturn]] void die(const std::string& msg) {
  std::cerr << "parallel_transport demo failure: " << msg << std::endl;
  std::exit(1);
}

void require(bool ok, const std::string& msg) {
  if (!ok) die(msg);
}

void require_rv(error_t got, error_t want, const std::string& msg) {
  if (got != want) {
    die(msg + " (got=" + std::to_string(int(got)) + ", want=" + std::to_string(int(want)) + ")");
  }
}

// A minimal in-memory transport implementation that demonstrates how users can
// bring their own transport without touching protocol/job definitions.
struct channel_t {
  std::mutex m;
  std::condition_variable cv;
  std::deque<buf_t> q;
};

struct in_memory_network_t {
  explicit in_memory_network_t(int n) : n(n), ch(n, std::vector<std::shared_ptr<channel_t>>(n)) {
    for (int from = 0; from < n; ++from) {
      for (int to = 0; to < n; ++to) {
        if (from == to) continue;
        ch[from][to] = std::make_shared<channel_t>();
      }
    }
  }

  const int n;
  // ch[from][to] is the message queue from `from` to `to`.
  std::vector<std::vector<std::shared_ptr<channel_t>>> ch;
};

class in_memory_transport_t final : public coinbase::api::data_transport_i {
 public:
  in_memory_transport_t(int self, std::shared_ptr<in_memory_network_t> net) : self_(self), net_(std::move(net)) {}

  error_t send(party_idx_t receiver, mem_t msg) override {
    if (!net_ || receiver < 0 || receiver >= net_->n || receiver == self_) return E_BADARG;
    auto c = net_->ch[self_][receiver];
    if (!c) return E_GENERAL;
    {
      std::lock_guard<std::mutex> lk(c->m);
      c->q.emplace_back(msg);
    }
    c->cv.notify_one();
    return SUCCESS;
  }

  error_t receive(party_idx_t sender, buf_t& msg) override {
    if (!net_ || sender < 0 || sender >= net_->n || sender == self_) return E_BADARG;
    auto c = net_->ch[sender][self_];
    if (!c) return E_GENERAL;
    std::unique_lock<std::mutex> lk(c->m);
    c->cv.wait(lk, [&] { return !c->q.empty(); });
    msg = std::move(c->q.front());
    c->q.pop_front();
    return SUCCESS;
  }

  error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& msgs) override {
    msgs.clear();
    msgs.resize(senders.size());
    for (size_t i = 0; i < senders.size(); ++i) {
      error_t rv = receive(senders[i], msgs[i]);
      if (rv) return rv;
    }
    return SUCCESS;
  }

 private:
  const int self_;
  std::shared_ptr<in_memory_network_t> net_;
};

class fixed_buf_transport_t final : public coinbase::api::data_transport_i {
 public:
  explicit fixed_buf_transport_t(buf_t malicious) : malicious_buf_(std::move(malicious)) {}

  error_t send(party_idx_t /*receiver*/, mem_t /*msg*/) override { return SUCCESS; }

  error_t receive(party_idx_t /*sender*/, buf_t& msg) override {
    msg = malicious_buf_;
    return SUCCESS;
  }

  error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& msgs) override {
    msgs.assign(senders.size(), malicious_buf_);
    return SUCCESS;
  }

 private:
  buf_t malicious_buf_;
};

struct scoped_log_sink_t {
  scoped_log_sink_t() : prev_(coinbase::out_log_fun) { coinbase::out_log_fun = &scoped_log_sink_t::discard; }
  ~scoped_log_sink_t() { coinbase::out_log_fun = prev_; }
  scoped_log_sink_t(const scoped_log_sink_t&) = delete;
  scoped_log_sink_t& operator=(const scoped_log_sink_t&) = delete;

 private:
  static void discard(int /*mode*/, const char* /*str*/) {}
  coinbase::out_log_str_f prev_;
};

void demo_parallel_transport_oob_receive() {
  scoped_log_sink_t logs;

  // A single byte `0x00` decodes to vector length = 0 (via convert_len).
  buf_t malicious(1);
  malicious[0] = 0x00;

  auto transport = std::make_shared<fixed_buf_transport_t>(malicious);
  parallel_data_transport_t network(transport, /*_parallel_count=*/2);

  error_t rv0 = UNINITIALIZED_ERROR;
  error_t rv1 = UNINITIALIZED_ERROR;
  buf_t out0, out1;

  std::thread t0([&] { rv0 = network.receive(/*sender=*/0, /*parallel_id=*/0, out0); });
  std::thread t1([&] { rv1 = network.receive(/*sender=*/0, /*parallel_id=*/1, out1); });
  t0.join();
  t1.join();

  require_rv(rv0, E_FORMAT, "receive: expected E_FORMAT for parallel_id=0");
  require_rv(rv1, E_FORMAT, "receive: expected E_FORMAT for parallel_id=1");
  require(out0.empty(), "receive: expected empty out0 on error");
  require(out1.empty(), "receive: expected empty out1 on error");
}

void demo_parallel_transport_oob_receive_all() {
  scoped_log_sink_t logs;

  buf_t malicious(1);
  malicious[0] = 0x00;

  auto transport = std::make_shared<fixed_buf_transport_t>(malicious);
  parallel_data_transport_t network(transport, /*_parallel_count=*/2);

  const std::vector<party_idx_t> senders = {0, 1, 2};

  error_t rv0 = UNINITIALIZED_ERROR;
  error_t rv1 = UNINITIALIZED_ERROR;
  std::vector<buf_t> outs0(senders.size());
  std::vector<buf_t> outs1(senders.size());

  std::thread t0([&] { rv0 = network.receive_all(senders, /*parallel_id=*/0, outs0); });
  std::thread t1([&] { rv1 = network.receive_all(senders, /*parallel_id=*/1, outs1); });
  t0.join();
  t1.join();

  require_rv(rv0, E_FORMAT, "receive_all: expected E_FORMAT for parallel_id=0");
  require_rv(rv1, E_FORMAT, "receive_all: expected E_FORMAT for parallel_id=1");
  for (const auto& m : outs0) require(m.empty(), "receive_all: expected empty output on error (outs0)");
  for (const auto& m : outs1) require(m.empty(), "receive_all: expected empty output on error (outs1)");
}

void demo_parallel_2pc_messaging() {
  const int parallel_count = 16;

  auto net = std::make_shared<in_memory_network_t>(/*n=*/2);
  auto t_p1 = std::make_shared<in_memory_transport_t>(/*self=*/0, net);
  auto t_p2 = std::make_shared<in_memory_transport_t>(/*self=*/1, net);

  auto n_p1 = std::make_shared<parallel_data_transport_t>(t_p1, parallel_count);
  auto n_p2 = std::make_shared<parallel_data_transport_t>(t_p2, parallel_count);

  std::atomic<int> finished{0};
  std::vector<std::thread> threads;
  threads.reserve(size_t(parallel_count) * 2);

  for (int th_i = 0; th_i < parallel_count; ++th_i) {
    threads.emplace_back([&, th_i] {
      job_parallel_2p_t job(party_t::p1, /*pname1=*/"p1", /*pname2=*/"p2", n_p1, parallel_id_t(th_i));
      buf_t data("msg:" + std::to_string(th_i));
      require_rv(job.p1_to_p2(data), SUCCESS, "2pc parallel: p1_to_p2 failed (p1)");
      finished++;
    });
  }

  for (int th_i = 0; th_i < parallel_count; ++th_i) {
    threads.emplace_back([&, th_i] {
      job_parallel_2p_t job(party_t::p2, /*pname1=*/"p1", /*pname2=*/"p2", n_p2, parallel_id_t(th_i));
      buf_t data;
      require_rv(job.p1_to_p2(data), SUCCESS, "2pc parallel: p1_to_p2 failed (p2)");
      require(data == buf_t("msg:" + std::to_string(th_i)), "2pc parallel: received message mismatch");
      finished++;
    });
  }

  for (auto& t : threads) t.join();
  require(finished == parallel_count * 2, "2pc parallel: not all threads finished");
}

}  // namespace

int main() {
  demo_parallel_transport_oob_receive();
  demo_parallel_transport_oob_receive_all();
  demo_parallel_2pc_messaging();

  std::cout << "parallel_transport demo: OK" << std::endl;
  return 0;
}

