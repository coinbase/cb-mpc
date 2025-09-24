#pragma once
#include <cbmpc/crypto/base.h>

namespace coinbase::testutils {

struct test_message_buffer_t {
  int size;

  byte_ptr data() { return byte_ptr(this + 1); }
  static test_message_buffer_t* allocate(int size);
  static void free(test_message_buffer_t* test_message_buffer);
};

struct test_channel_sync_t {
  std::mutex mutex;
  std::condition_variable cond;
};

class test_channel_t {
 public:
  void send(test_channel_sync_t& sync, mem_t msg);
  error_t receive(test_channel_sync_t& sync, bool& abort, buf_t& result);
  buf_t receive();              // no-sync
  bool queue_is_empty() const;  // no-sync

  static std::atomic<int> msg_counter;
  static bool fuzzing;
  static int fuzzing_msg_counter;
  static crypto::drbg_aes_ctr_t fuzzing_drbg;
  void reset();

 private:
  test_message_buffer_t* sending = nullptr;
  test_message_buffer_t* receiving = nullptr;
  std::queue<test_message_buffer_t*> queue;
};

}  // namespace coinbase::testutils