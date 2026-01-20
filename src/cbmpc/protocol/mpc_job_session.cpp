#include "mpc_job_session.h"

#include <algorithm>

namespace coinbase::mpc {

error_t parallel_data_transport_t::send(const party_idx_t receiver, const parallel_id_t parallel_id, const mem_t msg) {
  {  // Wait for senders to finish sending the previous message
    std::unique_lock<std::mutex> lk(is_send_active_mtx);
    send_active_cv.wait(lk, [this] { return is_send_active == 0; });
  }

  error_t rv = UNINITIALIZED_ERROR;
  {  // store the messages to be sent
    std::lock_guard<std::mutex> lk(send_msg_mutex);
    send_msg[parallel_id] = msg;
  }

  {  // Notify the master (parallel_id == 0) to start once we have all messages from threads.
    std::lock_guard<std::mutex> lk(send_ready_mtx);
    send_ready++;
    if (send_ready >= parallel_count) send_start_cv.notify_all();
  }

  if (parallel_id == 0) {
    {  // Wait for all threads joining
      std::unique_lock<std::mutex> lk(send_ready_mtx);
      send_start_cv.wait(lk, [this] { return send_ready >= parallel_count; });
      is_send_active = parallel_count;
    }

    // Send the collected messages
    buf_t bundled_msg;
    {
      std::lock_guard<std::mutex> lk(send_msg_mutex);
      bundled_msg = ser(send_msg);
      send_msg = std::vector<buf_t>(parallel_count);
    }
    rv = data_transport_ptr->send(receiver, bundled_msg);

    {  // Notify all threads that the send is done
      std::lock_guard<std::mutex> lk(send_ready_mtx);
      send_ready = 0;
    }
    send_done_cv.notify_all();
  } else {  // Wait for the master to finish sending
    std::unique_lock<std::mutex> lk(send_ready_mtx);
    send_done_cv.wait(lk, [this] { return send_ready == 0; });
  }

  {  // Reset is_send_active to notify the next message sending
    std::lock_guard<std::mutex> lk(is_send_active_mtx);
    is_send_active--;
    if (is_send_active == 0) send_active_cv.notify_all();
  }

  return SUCCESS;
}

error_t parallel_data_transport_t::receive(const party_idx_t sender, const parallel_id_t parallel_id, buf_t& msg) {
  {  // Wait for receivers to finish receiving the previous message
    std::unique_lock<std::mutex> lk(is_receive_active_mtx);
    receive_active_cv.wait(lk, [this] { return is_receive_active == 0; });
  }

  error_t rv = UNINITIALIZED_ERROR;
  {  // Notify the master (parallel_id == 0) to start once all receivers are ready
     // TODO(optimization): master thread should not have to wait for this. Following the same paradigm as send.
    std::lock_guard<std::mutex> lk(receive_ready_mtx);
    receive_ready++;
    if (receive_ready >= parallel_count) receive_start_cv.notify_all();
  }

  error_t local_rv = SUCCESS;
  if (parallel_id == 0) {
    {  // Wait for all threads joining
      std::unique_lock<std::mutex> lk(receive_ready_mtx);
      receive_start_cv.wait(lk, [this] { return receive_ready >= parallel_count; });
      is_receive_active = parallel_count;
    }

    // Pre-initialize receive buffers so non-master threads can safely read on error
    {
      std::lock_guard<std::mutex> lk(receive_msg_mutex);
      receive_msg = std::vector<buf_t>(parallel_count);
    }

    // Common error cleanup helper
    auto cleanup_receive_error = [&](error_t err) -> error_t {
      {
        std::lock_guard<std::mutex> lk(receive_ready_mtx);
        last_receive_rv = err;
        receive_ready = 0;
      }
      receive_done_cv.notify_all();

      {
        std::lock_guard<std::mutex> lk(is_receive_active_mtx);
        is_receive_active--;
        if (is_receive_active == 0) receive_active_cv.notify_all();
      }
      return err;
    };

    // Store the received messages
    buf_t buf;
    if (rv = data_transport_ptr->receive(sender, buf)) return cleanup_receive_error(rv);
    {
      // Deserialize into a temporary container first. `deser()` is allowed to resize vectors
      // based on attacker-controlled length prefixes, so never deserialize directly into
      // shared state that later gets indexed by `parallel_id`.
      std::vector<buf_t> decoded;
      if (rv = deser(buf, decoded)) return cleanup_receive_error(rv);
      if (int(decoded.size()) != parallel_count) {
        return cleanup_receive_error(
            coinbase::error(E_FORMAT, "parallel_data_transport_t::receive: unexpected bundled vector size"));
      }
      std::lock_guard<std::mutex> lk(receive_msg_mutex);
      receive_msg = std::move(decoded);
    }

    {  // Notify all threads that the receive is done
      std::lock_guard<std::mutex> lk(receive_ready_mtx);
      last_receive_rv = SUCCESS;
      receive_ready = 0;
    }
    receive_done_cv.notify_all();
    local_rv = last_receive_rv;
  } else {
    std::unique_lock<std::mutex> lk(receive_ready_mtx);
    receive_done_cv.wait(lk, [this] { return receive_ready == 0; });
    local_rv = last_receive_rv;
  }

  if (local_rv == SUCCESS) {
    // Getting the received message for each thread
    std::lock_guard<std::mutex> lk(receive_msg_mutex);
    msg = receive_msg[parallel_id];
  } else {
    // Ensure we don't return partially-initialized data on error.
    msg = buf_t();
  }

  {  // Reset is_receive_active to notify the next message receiving
    std::lock_guard<std::mutex> lk(is_receive_active_mtx);
    is_receive_active--;
    if (is_receive_active == 0) receive_active_cv.notify_all();
  }
  return local_rv;
}

error_t parallel_data_transport_t::receive_all(const std::vector<party_idx_t>& senders, const parallel_id_t parallel_id,
                                               std::vector<buf_t>& out_msgs) {
  error_t rv = UNINITIALIZED_ERROR;

  {
    std::unique_lock<std::mutex> lk(is_receive_all_mtx);
    receive_all_active_cv.wait(lk, [this] { return is_receive_all_active == 0; });
  }

  {
    std::lock_guard<std::mutex> lk(receive_all_ready_mtx);
    receive_all_ready++;
    if (receive_all_ready >= parallel_count) receive_all_start_cv.notify_all();
  }

  if (parallel_id == 0) {
    {
      std::unique_lock<std::mutex> lk(receive_all_ready_mtx);
      receive_all_start_cv.wait(lk, [this] { return receive_all_ready >= parallel_count; });
      is_receive_all_active = parallel_count;
    }

    // Pre-initialize receive_all buffers so non-master threads can safely read on error
    {
      std::lock_guard<std::mutex> lk(receive_all_msgs_mutex);
      for (auto s : senders) {
        receive_all_msgs[s] = std::vector<buf_t>(parallel_count);
      }
    }

    // Common error cleanup helper for receive_all
    auto cleanup_receive_all_error = [&](error_t err) -> error_t {
      {
        std::lock_guard<std::mutex> lk(receive_all_ready_mtx);
        last_receive_all_rv = err;
        receive_all_ready = 0;
        receive_all_done_cv.notify_all();
      }
      {
        std::lock_guard<std::mutex> lk2(is_receive_all_mtx);
        is_receive_all_active--;
        if (is_receive_all_active == 0) receive_all_active_cv.notify_all();
      }
      return err;
    };

    std::vector<buf_t> bufs(senders.size());
    if (rv = data_transport_ptr->receive_all(senders, bufs)) return cleanup_receive_all_error(rv);

    {
      std::lock_guard<std::mutex> lk(receive_all_msgs_mutex);
      for (size_t i = 0; i < bufs.size(); i++) {
        std::vector<buf_t> decoded;
        if (rv = deser(bufs[i], decoded)) return cleanup_receive_all_error(rv);
        if (int(decoded.size()) != parallel_count) {
          return cleanup_receive_all_error(
              coinbase::error(E_FORMAT, "parallel_data_transport_t::receive_all: unexpected bundled vector size"));
        }
        receive_all_msgs[senders[i]] = std::move(decoded);
      }
    }

    {
      std::lock_guard<std::mutex> lk(receive_all_ready_mtx);
      last_receive_all_rv = SUCCESS;
      receive_all_ready = 0;
      receive_all_done_cv.notify_all();
    }
  } else {
    std::unique_lock<std::mutex> lk(receive_all_ready_mtx);
    receive_all_done_cv.wait(lk, [this] { return receive_all_ready == 0; });
  }

  error_t local_rv = SUCCESS;
  {
    std::lock_guard<std::mutex> lk(receive_all_ready_mtx);
    local_rv = last_receive_all_rv;
  }
  if (local_rv == SUCCESS) {
    std::lock_guard<std::mutex> lk(receive_all_msgs_mutex);
    const size_t n = std::min(out_msgs.size(), senders.size());
    for (size_t i = 0; i < n; i++) {
      out_msgs[i] = receive_all_msgs[senders[i]][parallel_id];
    }
    for (size_t i = n; i < out_msgs.size(); i++) out_msgs[i] = buf_t();
  } else {
    for (auto& m : out_msgs) m = buf_t();
  }

  {
    std::lock_guard<std::mutex> lk(is_receive_all_mtx);
    is_receive_all_active--;
    if (is_receive_all_active == 0) receive_all_active_cv.notify_all();
  }
  return local_rv;
}

void parallel_data_transport_t::set_parallel(int _parallel_count) {
  {
    std::unique_lock<std::mutex> lk1(is_send_active_mtx, std::defer_lock);
    std::unique_lock<std::mutex> lk2(is_receive_active_mtx, std::defer_lock);
    std::unique_lock<std::mutex> lk3(is_receive_all_mtx, std::defer_lock);

    // Lock all mutexes together, avoiding deadlock
    std::lock(lk1, lk2, lk3);

    send_active_cv.wait(lk1, [this] { return is_send_active == 0; });
    receive_active_cv.wait(lk2, [this] { return is_receive_active == 0; });
    receive_all_active_cv.wait(lk3, [this] { return is_receive_all_active == 0; });
  }
  parallel_count = _parallel_count;
  {
    std::lock_guard<std::mutex> lk(send_msg_mutex);
    send_msg = std::vector<buf_t>(parallel_count);
  }
}

}  // namespace coinbase::mpc
