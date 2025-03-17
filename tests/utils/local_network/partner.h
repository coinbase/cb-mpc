#pragma once

#include "mpc_runner.h"

namespace coinbase::testutils {
class mpc_runner_t;

class partner_t {
 public:
  partner_t(mpc::party_idx_t _role_index) : party_index(_role_index) {}

  void add_runner(mpc_runner_t& runner);
  void start();
  void stop();
  void join();

 private:
  mpc::party_idx_t party_index;
  std::mutex mutex;
  std::condition_variable cond;
  std::thread* thread = nullptr;

  std::queue<mpc_runner_t*> runner_queue;
  bool end = false;
};

}  // namespace coinbase::testutils