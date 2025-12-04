#include "channel.h"

namespace coinbase::testutils {
test_message_buffer_t* test_message_buffer_t::allocate(int size) {
  test_message_buffer_t* buf = (test_message_buffer_t*)malloc(sizeof(test_message_buffer_t) + size);
  buf->size = size;
  return buf;
}

void test_message_buffer_t::free(test_message_buffer_t* buf) { ::free(buf); }

std::atomic<int> test_channel_t::msg_counter = 0;
bool test_channel_t::fuzzing = false;
int test_channel_t::fuzzing_msg_counter = 0;
crypto::drbg_aes_ctr_t test_channel_t::fuzzing_drbg = crypto::drbg_aes_ctr_t(buf_t(16));

void test_channel_t::send(test_channel_sync_t& sync, mem_t msg) {
  std::unique_lock lock(sync.mutex);
  cb_assert(sending == nullptr);
  sending = test_message_buffer_t::allocate(msg.size);
  memmove(sending->data(), msg.data, msg.size);

  cb_assert(sending);
  queue.push(sending);

  int counter = msg_counter.fetch_add(1);

  if (fuzzing && counter == fuzzing_msg_counter) {
    int bit = fuzzing_drbg.gen_int() % (sending->size * 8);
    int byte_index = bit / 8;
    byte_t mask = 1 << (bit % 8);
    sending->data()[byte_index] ^= mask;
  }

  sending = nullptr;
  sync.cond.notify_all();
}

bool test_channel_t::queue_is_empty() const { return queue.empty(); }

error_t test_channel_t::receive(test_channel_sync_t& sync, bool& abort, buf_t& result) {
  std::unique_lock lock(sync.mutex);
  while (queue_is_empty() && !abort) sync.cond.wait(lock);
  if (abort) return E_NET_GENERAL;
  result = receive();
  return SUCCESS;
}

buf_t test_channel_t::receive()  // no-sync
{
  if (receiving) test_message_buffer_t::free(receiving);
  receiving = queue.front();
  queue.pop();
  return buf_t(receiving->data(), receiving->size);
}

void test_channel_t::reset() {
  if (receiving) test_message_buffer_t::free(receiving);
  sending = nullptr;
  receiving = nullptr;
  while (!queue_is_empty()) {
    delete queue.front();
    queue.pop();
  }
}

}  // namespace coinbase::testutils