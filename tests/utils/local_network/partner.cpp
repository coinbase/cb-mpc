#include "partner.h"

namespace coinbase::testutils {

void partner_t::add_runner(mpc_runner_t& runner) {
  std::unique_lock scoped(mutex);
  runner_queue.push(&runner);
  cond.notify_all();
}

void partner_t::start() {
  end = false;
  thread = new std::thread([this]() {
    for (;;) {
      mpc_runner_t* runner;
      {
        std::unique_lock scoped(mutex);
        while (!end && runner_queue.empty()) cond.wait(scoped);
        if (end) break;
        runner = runner_queue.back();
        runner_queue.pop();
      }

      runner->run_on_partner(party_index);
    }
  });
}

void partner_t::stop() {
  std::unique_lock scoped(mutex);
  end = true;
  cond.notify_all();
}

void partner_t::join() {
  if (thread) thread->join();
  delete thread;
}

}  // namespace coinbase::testutils