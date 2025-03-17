#include "mpc_job_session.h"

namespace coinbase::mpc {

error_t network_t::send(const party_idx_t receiver, const jsid_t jsid, const mem_t msg) {
  {  // Wait for senders to finish sending the previous message
    std::unique_lock<std::mutex> lk(is_send_active_mtx);
    send_active_cv.wait(lk, [this] { return is_send_active == 0; });
  }

  error_t rv = UNINITIALIZED_ERROR;
  {  // store the messages to be sent
    std::lock_guard<std::mutex> lk(send_msg_mutex);
    send_msg[jsid] = msg;
  }

  {  // Notify the master (jsid == 0) to start once we have all messages from threads.
    std::lock_guard<std::mutex> lk(send_ready_mtx);
    send_ready++;
    if (send_ready >= parallel_count) send_start_cv.notify_all();
  }

  if (jsid == 0) {
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

error_t network_t::receive(const party_idx_t sender, const jsid_t jsid, mem_t& msg) {
  {  // Wait for receivers to finish receiving the previous message
    std::unique_lock<std::mutex> lk(is_receive_active_mtx);
    receive_active_cv.wait(lk, [this] { return is_receive_active == 0; });
  }

  error_t rv = UNINITIALIZED_ERROR;
  {  // Notify the master (jsid == 0) to start once all receivers are ready
     // TODO(optimization): master thread should not have to wait for this. Following the same paradigm as send.
    std::lock_guard<std::mutex> lk(receive_ready_mtx);
    receive_ready++;
    if (receive_ready >= parallel_count) receive_start_cv.notify_all();
  }

  if (jsid == 0) {
    {  // Wait for all threads joining
      std::unique_lock<std::mutex> lk(receive_ready_mtx);
      receive_start_cv.wait(lk, [this] { return receive_ready >= parallel_count; });
      is_receive_active = parallel_count;
    }

    // Store the received messages
    mem_t mem;
    if (rv = data_transport_ptr->receive(sender, mem)) return rv;
    {
      std::lock_guard<std::mutex> lk(receive_msg_mutex);

      receive_msg = std::vector<buf_t>(parallel_count);
      if (rv = deser(mem, receive_msg)) return rv;
    }

    {  // Notify all threads that the receive is done
      std::lock_guard<std::mutex> lk(receive_ready_mtx);
      receive_ready = 0;
    }
    receive_done_cv.notify_all();
  } else {
    std::unique_lock<std::mutex> lk(receive_ready_mtx);
    receive_done_cv.wait(lk, [this] { return receive_ready == 0; });
  }

  {  // Getting the received message for each thread
    std::lock_guard<std::mutex> lk(receive_msg_mutex);
    msg = receive_msg[jsid];
  }

  {  // Reset is_receive_active to notify the next message receiving
    std::lock_guard<std::mutex> lk(is_receive_active_mtx);
    is_receive_active--;
    if (is_receive_active == 0) receive_active_cv.notify_all();
  }
  return SUCCESS;
}

error_t network_t::receive_all(const std::vector<party_idx_t>& senders, const jsid_t jsid,
                               std::vector<mem_t>& out_msgs) {
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

  if (jsid == 0) {
    {
      std::unique_lock<std::mutex> lk(receive_all_ready_mtx);
      receive_all_start_cv.wait(lk, [this] { return receive_all_ready >= parallel_count; });
      is_receive_all_active = parallel_count;
    }

    std::vector<mem_t> mems(senders.size());
    if (rv = data_transport_ptr->receive_all(senders, mems)) return rv;

    {
      std::lock_guard<std::mutex> lk(receive_all_msgs_mutex);
      for (int i = 0; i < mems.size(); i++) {
        receive_all_msgs[senders[i]] = std::vector<buf_t>(parallel_count);
        if (rv = deser(mems[i], receive_all_msgs[senders[i]])) return rv;
      }
    }

    {
      std::lock_guard<std::mutex> lk(receive_all_ready_mtx);
      receive_all_ready = 0;
      receive_all_done_cv.notify_all();
    }
  } else {
    std::unique_lock<std::mutex> lk(receive_all_ready_mtx);
    receive_all_done_cv.wait(lk, [this] { return receive_all_ready == 0; });
  }

  {
    std::lock_guard<std::mutex> lk(receive_all_msgs_mutex);
    for (int i = 0; i < out_msgs.size(); i++) {
      out_msgs[i] = receive_all_msgs[senders[i]][jsid];
    }
  }

  {
    std::lock_guard<std::mutex> lk(is_receive_all_mtx);
    is_receive_all_active--;
    if (is_receive_all_active == 0) receive_all_active_cv.notify_all();
  }
  return SUCCESS;
}

void network_t::set_parallel(int _parallel_count) {
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
