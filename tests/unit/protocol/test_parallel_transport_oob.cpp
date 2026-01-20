#include <gtest/gtest.h>
#include <thread>

#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/protocol/mpc_job_session.h>

namespace {

using namespace coinbase;
using namespace coinbase::mpc;

class fixed_buf_transport_t final : public data_transport_interface_t {
 public:
  explicit fixed_buf_transport_t(buf_t malicious) : malicious_buf_(std::move(malicious)) {}

  error_t send(party_idx_t /*receiver*/, mem_t /*msg*/) override { return SUCCESS; }

  error_t receive(party_idx_t /*sender*/, buf_t& msg) override {
    msg = malicious_buf_;
    return SUCCESS;
  }

  error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<buf_t>& message) override {
    message.assign(senders.size(), malicious_buf_);
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

TEST(ParallelDataTransportOOB, MaliciousVectorLenZeroReceive) {
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

  EXPECT_EQ(rv0, E_FORMAT);
  EXPECT_EQ(rv1, E_FORMAT);
  EXPECT_TRUE(out0.empty());
  EXPECT_TRUE(out1.empty());
}

TEST(ParallelDataTransportOOB, MaliciousVectorLenZeroReceiveAll) {
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

  EXPECT_EQ(rv0, E_FORMAT);
  EXPECT_EQ(rv1, E_FORMAT);
  for (const auto& m : outs0) EXPECT_TRUE(m.empty());
  for (const auto& m : outs1) EXPECT_TRUE(m.empty());
}

}  // namespace
