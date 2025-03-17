#pragma once

#include <cbmpc/core/convert.h>
#include <cbmpc/core/log.h>
#include <cbmpc/crypto/base_pki.h>
#include <cbmpc/crypto/ro.h>

#include "util.h"

namespace coinbase::mpc {

typedef int32_t party_idx_t;

enum class party_t : party_idx_t { p1 = 0, p2 = 1 };

class party_set_t {
 public:
  party_set_t(uint64_t p = 0) : peers(p) {}
  bool has(int party_index) const { return (peers & mask_of_party(party_index)) != 0; }
  static party_set_t of(int party_index) { return party_set_t(mask_of_party(party_index)); }
  bool is_empty() const { return peers == 0; }
  void add(int party_index) { peers |= mask_of_party(party_index); }
  void remove(int party_index) { peers &= ~mask_of_party(party_index); }
  static party_set_t all() { return party_set_t(0xffffffffffffffff); }
  static party_set_t empty() { return party_set_t(0); }
  uint64_t peers;

 private:
  static uint64_t mask_of_party(int party_index) { return uint64_t(1) << party_index; }
};

class job_mp_t {
  /* Helper funcitons for serializing/deserializing messages for multi parties */

  template <typename... MSGS>
  std::vector<buf_t> pack_msgs(party_set_t set, MSGS&... msgs) {
    std::vector<buf_t> out(n_parties);
    for (int i = 0; i < n_parties; i++) {
      if (!set.has(i)) continue;
      out[i] = pack_msgs_for_party(i, msgs...);
    }
    return out;
  }

  template <typename... MSGS>
  error_t unpack_msgs(party_set_t set, const std::vector<buf_t>& received, MSGS&... msgs) {
    error_t rv = UNINITIALIZED_ERROR;
    for (int i = 0; i < n_parties; i++) {
      if (!set.has(i) || i == party_index) continue;
      if (rv = unpack_msgs_for_party(i, received[i], msgs...)) return rv;
    }
    return SUCCESS;
  }

  template <typename... MSGS>
  static buf_t pack_msgs_for_party(int index, MSGS&... msgs) {
    int n;
    {
      coinbase::converter_t converter(true);
      pack_msgs_converter_helper(converter, index, msgs...);
      n = converter.get_offset();
    }

    buf_t out(n);

    {
      coinbase::converter_t converter(out.data());
      pack_msgs_converter_helper(converter, index, msgs...);
    }
    return out;
  }

  template <typename... MSGS>
  static error_t unpack_msgs_for_party(int party_index, mem_t mem, MSGS&... msgs) {
    error_t rv = UNINITIALIZED_ERROR;
    coinbase::converter_t converter(mem);
    (
        [&](auto& arg) {
          if (!converter.is_error()) arg.unpack(converter, party_index);
        }(msgs),
        ...);
    if (rv = converter.get_rv()) return rv;
    if (converter.get_offset() != converter.get_size()) return coinbase::error(E_FORMAT);
    return SUCCESS;
  }

  template <typename... MSGS>
  static void pack_msgs_converter_helper(coinbase::converter_t& converter, int index, MSGS&... msgs) {
    ([&](auto& msg) { msg.pack(converter, index); }(msgs), ...);
  }

  /* Helper functions for group messaging between for multiple groups */

  template <typename MSG>
  std::vector<buf_t> pack_multi_sets_msgs(std::tuple<party_set_t&, party_set_t&, MSG&> msg_ctx) {
    std::vector<buf_t> out(n_parties);
    for (int i = 0; i < n_parties; i++) {
      if (!std::get<0>(msg_ctx).has(i)) continue;
      out[i] = pack_msgs_for_party(i, std::get<2>(msg_ctx));
    }
    return out;
  }

  // Helper for `combine_packed_msgs` to apply the pack function at a specific index across all vectors
  template <typename Tuple, std::size_t... Is>
  buf_t apply_bundle_at_index(const Tuple& tuple, std::size_t index, std::index_sequence<Is...>) {
    return coinbase::ser(std::get<Is>(tuple)[index]...);
  }

  // Combine a tuple of packed_msgs (vector<buf_t>) into a single packed_msgs
  template <typename... BUFS>
  std::vector<buf_t> combine_packed_msgs(const std::tuple<BUFS...>& tuple) {
    std::vector<buf_t> result(n_parties);

    for (std::size_t i = 0; i < n_parties; ++i) {
      result[i] = apply_bundle_at_index(tuple, i, std::index_sequence_for<BUFS...>{});
    }

    return result;
  }

  // Helper for `split_packed_msgs` to apply the unpack function at a specific index across all vectors
  template <typename Tuple, std::size_t... Is>
  error_t apply_unbundle_at_index(mem_t out, Tuple& tuple, std::size_t index, std::index_sequence<Is...>) {
    return coinbase::deser(out, std::get<Is>(tuple)[index]...);
  }

  // Reverse of `combine_packed_msgs`
  template <typename... Ts>
  error_t split_packed_msgs(const std::vector<buf_t>& bufs, std::tuple<Ts...>& tuple) {
    error_t rv = UNINITIALIZED_ERROR;

    for (std::size_t i = 0; i < n_parties; ++i) {
      mem_t mem = bufs[i];
      if (rv = apply_unbundle_at_index(mem, tuple, i, std::index_sequence_for<Ts...>{})) return rv;
    }

    return SUCCESS;
  }

  template <typename MSG>
  error_t unpack_msg_ctx(const std::vector<buf_t>& bufs, std::tuple<party_set_t&, party_set_t&, MSG&> msg_ctx) {
    return unpack_msgs(std::get<1>(msg_ctx), bufs, std::get<2>(msg_ctx));
  }
  template <typename Tuple1, typename Tuple2, std::size_t... Is>
  void unpack_multi_sets_msgs_helper(Tuple1& t1, Tuple2& t2, std::index_sequence<Is...>) {
    // Use the index sequence to access elements from both tuples
    (unpack_msg_ctx(std::get<Is>(t1), std::get<Is>(t2)), ...);
  }
  template <typename... Ts1, typename... Ts2>
  void unpack_multi_sets_tupled_msgs(std::tuple<Ts1...>& t1, std::tuple<Ts2&...> t2) {
    static_assert(sizeof...(Ts1) == sizeof...(Ts2), "Tuples must have the same length to unpack");
    unpack_multi_sets_msgs_helper(t1, t2, std::index_sequence_for<Ts1...>{});
  }

  /* functions to send and received serialized multi-party messages */

  error_t send_to_parties(party_set_t set, const std::vector<buf_t>& in);
  error_t receive_from_parties(party_set_t set, std::vector<buf_t>& v);

 protected:
  bool message_sending = false;
  party_idx_t party_index;
  int n_parties;
  std::vector<crypto::mpc_pid_t> pids;

  job_mp_t(int index, std::vector<crypto::mpc_pid_t> pids) : party_index(index), n_parties(pids.size()) {
    if (party_index < 0 || party_index >= n_parties) coinbase::error(E_BADARG, "invalid party_index");
    cb_assert(pids.size() >= 2 && "at least 2 parties are required");
    cb_assert(pids.size() <= 64 && "at most 64 parties are supported");
    this->pids = pids;
  }

  virtual error_t send_impl(party_idx_t to, mem_t msg) = 0;
  virtual error_t receive_impl(party_idx_t from, mem_t& msg) = 0;
  virtual error_t receive_many_impl(std::vector<party_idx_t> from_set, std::vector<mem_t>& outs);

 public:
  /* MPC Properties */

  int get_n_parties() const { return n_parties; }
  party_idx_t get_party_idx() const { return party_index; }
  virtual bool is_party_idx(party_idx_t i) const { return i == party_index; }

  const crypto::mpc_pid_t& get_pid() const { return pids[get_party_idx()]; }
  const crypto::mpc_pid_t& get_pid(party_idx_t index) const { return pids[index]; }
  const std::vector<crypto::mpc_pid_t>& get_pids() const { return pids; }

  /* MPC messaging */

  error_t mpc_abort(error_t rv, const std::string& message = "") { return coinbase::error(rv); }

  template <typename... MSGS>
  error_t send(party_idx_t to, MSGS&... msgs) {
    return send_impl(to, ser(msgs...));
  }

  template <typename... MSGS>
  error_t receive(party_idx_t from, MSGS&... msgs) {
    error_t rv = UNINITIALIZED_ERROR;

    mem_t mem;
    if (rv = receive_impl(from, mem)) return rv;
    if (mem.size <= 0) return coinbase::error(E_NET_GENERAL);  // deserialization length validation

    if (rv = deser(mem, msgs...)) return rv;

    return SUCCESS;
  }

  template <typename... MSGS>
  error_t send_receive_message(party_idx_t from, party_idx_t to, MSGS&... msgs) {
    error_t rv = UNINITIALIZED_ERROR;

    message_sending = true;

    if (is_party_idx(from)) {
      if (rv = send(to, msgs...)) return rv;
    }
    if (is_party_idx(to)) {
      if (rv = receive(from, msgs...)) return rv;
    }

    message_sending = false;

    return SUCCESS;
  }

  template <typename... MSGS>
  error_t group_message(party_set_t to_set, party_set_t from_set, MSGS&... msgs) {
    error_t rv = UNINITIALIZED_ERROR;

    if (!to_set.is_empty()) {
      std::vector<buf_t> send = pack_msgs(to_set, msgs...);
      if (rv = send_to_parties(to_set, send)) return rv;
    }
    if (!from_set.is_empty()) {
      std::vector<buf_t> receive;
      if (rv = receive_from_parties(from_set, receive)) return rv;
      unpack_msgs(from_set, receive, msgs...);
    }

    return SUCCESS;
  }

  // Group messaging for messages that involve different group of sender and receivers. It will bundle the messages for
  // each pairs of parties, which is different from calling the group_message for a fixed set of sender and receiver
  // multiple times.
  template <typename... MSG_TUPLES>
  error_t group_message(const MSG_TUPLES&... msg_tuples) {
    error_t rv = UNINITIALIZED_ERROR;
    auto packed_msgs_tuple = std::make_tuple(pack_multi_sets_msgs(msg_tuples)...);
    std::vector<buf_t> packed_msgs = combine_packed_msgs(packed_msgs_tuple);

    if (rv = send_to_parties(party_set_t::all(), packed_msgs)) return rv;
    if (rv = receive_from_parties(party_set_t::all(), packed_msgs)) return rv;

    if (rv = split_packed_msgs(packed_msgs, packed_msgs_tuple)) return rv;
    unpack_multi_sets_tupled_msgs(packed_msgs_tuple, std::tie(msg_tuples...));

    return SUCCESS;
  }

  template <typename... MSGS>
  error_t plain_broadcast(MSGS&... msgs) {
    return group_message(party_set_t::all(), party_set_t::all(), msgs...);
  }

  template <typename... MSGS>
  error_t send_message_all_to_one(party_idx_t to, MSGS&... msgs) {
    error_t rv = UNINITIALIZED_ERROR;

    if (party_index == to) {
      if (rv = group_message(party_set_t::empty(), party_set_t::all(), msgs...)) return rv;
    } else {
      if (rv = group_message(party_set_t::of(to), party_set_t::empty(), msgs...)) return rv;
    }
    return SUCCESS;
  }

  // ----- MPC message containers ---------

  // uniform message is for sending identical contents to other parties
  template <typename T>
  class uniform_msg_t : public T {
    friend class job_mp_t;
    void pack(coinbase::converter_t& c, int index) { c.convert(msg); }
    void unpack(coinbase::converter_t& c, int index) { c.convert(*receptacle[index]); }

    job_mp_t* job;
    std::vector<std::shared_ptr<T>> receptacle;

   public:
    void init_receptacle() {
      receptacle.resize(job->get_n_parties());
      for (int i = 0; i < job->get_n_parties(); i++) {
        if (i == job->get_party_idx())
          receptacle[job->get_party_idx()].reset(&msg, [](T*) {});
        else
          receptacle[i] = std::make_shared<T>();
      }
    }

    uniform_msg_t(job_mp_t* job) : job(job) { init_receptacle(); }
    uniform_msg_t(job_mp_t* job, const T& src) : T(src), job(job) { init_receptacle(); }

    operator T&() { return *this; }
    operator const T&() const { return *this; }

    void convert(coinbase::converter_t& c) { c.convert(msg); }

    T& msg = *this;
    T& received(int index) { return *receptacle[index]; }
    std::vector<T> all_received_values() { return extract_values(receptacle); }
    std::vector<std::reference_wrapper<T>> all_received_refs() { return extract_refs(receptacle); }
  };

  // nonuniform message is for sending different contents to each other parties
  // Note that there are inplace and non-inplace variables for different use cases. See the constructor comments.
  template <typename T>
  class nonuniform_msg_t {
    friend class job_mp_t;
    void pack(coinbase::converter_t& c, int index) { c.convert(msgs[index]); }
    void unpack(coinbase::converter_t& c, int index) { c.convert(*receptacle[index]); }

    job_mp_t* job;
    std::vector<std::shared_ptr<T>> receptacle;

   public:
    nonuniform_msg_t(job_mp_t* job) : job(job) {
      int n = job->get_n_parties();
      int index = job->get_party_idx();
      msgs.resize(n);

      receptacle.resize(n);
      for (int i = 0; i < n; i++) {
        if (i == index)
          receptacle[index].reset(&msgs[index], [](T*) {});
        else
          receptacle[i] = std::make_shared<T>();
      }
    }
    // For inplace messages where sending and receiving use same slots.
    // It is suitable when in a round, each party is either sender or receiver but not at the same time.
    nonuniform_msg_t(job_mp_t* job, std::function<T(int i)>& f) : job(job) {
      int n = job->get_n_parties();
      msgs.reserve(n);
      for (int i = 0; i < n; i++) msgs.push_back(f(i));

      receptacle.resize(n);
      for (int i = 0; i < n; i++) receptacle[i].reset(&msgs[i], [](T*) {});
    }

    T& operator[](int index) { return msgs[index]; };
    const T& operator[](int index) const { return msgs[index]; };
    T& received(int index) { return *receptacle[index]; }

    void convert(coinbase::converter_t& c) { c.convert(msgs); }

    operator T&() { return msgs; }
    operator const T&() const { return msgs; }

    std::vector<T> msgs;
  };

  // To bundle MPC messages. Useful when we can only pass a single parameter between functions.
  template <typename T>
  class msg_tuple_t {
   public:
    msg_tuple_t(T&& src) : msg(src) {}
    void pack(coinbase::converter_t& c, int index) {
      for_tuple(msg, [&](auto& arg) { arg.pack(c, index); });
    }
    void unpack(coinbase::converter_t& c, int index) {
      for_tuple(msg, [&](auto& arg) {
        if (!c.is_error()) arg.unpack(c, index);
      });
    }

   private:
    T msg;
  };

  // Functions to create message containers
  template <typename T>
  uniform_msg_t<T> uniform_msg() {
    return uniform_msg_t<T>(this);
  }
  template <typename T>
  uniform_msg_t<T> uniform_msg(const T& src) {
    return uniform_msg_t<T>(this, src);
  }
  template <typename T>
  nonuniform_msg_t<T> nonuniform_msg() {
    return nonuniform_msg_t<T>(this);
  }
  template <typename T>
  nonuniform_msg_t<T> inplace_msg(std::function<T(int i)>&& f) {
    return nonuniform_msg_t<T>(this, f);
  }
  template <typename... ARGS>
  static auto tie_msgs(ARGS&... args) {
    return msg_tuple_t(std::tie(args...));
  }
};

class job_2p_t : public job_mp_t {
 public:
  job_2p_t(party_t index, crypto::mpc_pid_t pid1, crypto::mpc_pid_t pid2)
      : job_mp_t(party_idx_t(index), {pid1, pid2}) {}

  bool is_p1() const { return is_party_idx(party_idx_t(party_t::p1)); }
  bool is_p2() const { return is_party_idx(party_idx_t(party_t::p2)); }
  bool is_party(party_t party) const { return is_party_idx(party_idx_t(party)); }
  party_t get_party() { return party_t(party_index); }

  const crypto::mpc_pid_t& get_pid() const { return job_mp_t::get_pid(); }
  const crypto::mpc_pid_t& get_pid(party_t party) const { return job_mp_t::get_pid(party_idx_t(party)); }

  template <typename... ARGS>
  error_t p1_to_p2(ARGS&... args) {
    return send_receive_message(party_idx_t(party_t::p1), party_idx_t(party_t::p2), args...);
  }

  template <typename... ARGS>
  error_t p2_to_p1(ARGS&... args) {
    return send_receive_message(party_idx_t(party_t::p2), party_idx_t(party_t::p1), args...);
  }
};

}  // namespace coinbase::mpc