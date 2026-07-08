#include <gtest/gtest.h>

#include <cbmpc/internal/core/log.h>
#include <cbmpc/internal/protocol/ecdsa_mp.h>

#include "utils/local_network/mpc_tester.h"

namespace {

using namespace coinbase;
using namespace coinbase::mpc;
using namespace coinbase::testutils;

class noop_transport_t final : public coinbase::api::data_transport_i {
 public:
  error_t send(coinbase::api::party_idx_t receiver, mem_t msg) override {
    last_receiver = receiver;
    last_msg = buf_t(msg);
    return SUCCESS;
  }

  error_t receive(coinbase::api::party_idx_t sender, buf_t& msg) override {
    last_sender = sender;
    msg = last_msg.empty() ? buf_t("noop") : last_msg;
    return SUCCESS;
  }

  error_t receive_all(const std::vector<coinbase::api::party_idx_t>& senders, std::vector<buf_t>& msgs) override {
    msgs.assign(senders.size(), last_msg.empty() ? buf_t("noop") : last_msg);
    return SUCCESS;
  }

  coinbase::api::party_idx_t last_receiver = -1;
  coinbase::api::party_idx_t last_sender = -1;
  buf_t last_msg;
};

TEST(MPCJob, PartySetAndAccessors) {
  party_set_t empty = party_set_t::empty();
  EXPECT_TRUE(empty.is_empty());
  EXPECT_FALSE(empty.has(1));

  party_set_t set = party_set_t::of(1);
  EXPECT_FALSE(set.is_empty());
  EXPECT_TRUE(set.has(1));
  EXPECT_FALSE(set.has(2));
  set.add(2);
  EXPECT_TRUE(set.has(2));
  set.remove(1);
  EXPECT_FALSE(set.has(1));
  EXPECT_TRUE(set.has(2));

  noop_transport_t transport;
  job_mp_t job(1, {"alice", "bob", "carol"}, transport);
  EXPECT_EQ(job.get_party_idx(), 1);
  EXPECT_EQ(job.get_n_parties(), 3);
  EXPECT_EQ(job.get_name(), "bob");
  EXPECT_EQ(job.get_name(0), "alice");
  EXPECT_EQ(job.get_names(), (std::vector<crypto::pname_t>{"alice", "bob", "carol"}));
  EXPECT_EQ(job.get_pid(), crypto::pid_from_name("bob"));
  EXPECT_EQ(job.get_pid(2), crypto::pid_from_name("carol"));
  EXPECT_EQ(job.get_pids().size(), 3);

  job.set_transport(2, transport);
  EXPECT_EQ(job.get_party_idx(), 2);
  EXPECT_EQ(job.get_name(), "carol");

  dylog_disable_scope_t no_log_err;
  EXPECT_EQ(job.mpc_abort(E_CRYPTO), E_CRYPTO);
}

TEST(MPCJob, TwoPartyReferenceTransportAccessors) {
  noop_transport_t transport;
  job_2p_t job(party_t::p1, "alice", "bob", transport);

  EXPECT_TRUE(job.is_p1());
  EXPECT_FALSE(job.is_p2());
  EXPECT_TRUE(job.is_party(party_t::p1));
  EXPECT_FALSE(job.is_party(party_t::p2));
  EXPECT_EQ(job.get_party(), party_t::p1);
  EXPECT_EQ(job.get_name(), "alice");
  EXPECT_EQ(job.get_name(party_t::p2), "bob");
  EXPECT_EQ(job.get_pid(), crypto::pid_from_name("alice"));
  EXPECT_EQ(job.get_pid(party_t::p2), crypto::pid_from_name("bob"));
}

TEST_F(Network2PC, BasicMessaging) {
  mpc_runner->run_2pc([](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    buf_t data;
    buf_t want(mem_t("test_string"));

    if (job.is_p1()) data = want;
    if (job.is_p2()) EXPECT_NE(data, want);
    rv = job.p1_to_p2(data);
    ASSERT_EQ(rv, 0);

    EXPECT_EQ(data, want);
  });

  mpc_runner->run_2pc([](job_2p_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    buf_t data;
    buf_t want(mem_t("test_string"));

    if (job.is_p2()) data = want;
    if (job.is_p1()) EXPECT_NE(data, want);
    rv = job.p2_to_p1(data);
    ASSERT_EQ(rv, 0);

    EXPECT_EQ(data, want);
  });
}

TEST_F(Network4PC, BasicBroadcast) {
  mpc_runner->run_mpc([](job_mp_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    auto data = job.uniform_msg<buf_t>(buf_t("test_data:" + std::to_string(party_index)));
    rv = job.plain_broadcast(data);
    ASSERT_EQ(rv, 0);

    for (int j = 0; j < 4; j++) {
      EXPECT_EQ(data.received(j), buf_t("test_data:" + std::to_string(j)));
      EXPECT_EQ(data.all_received()[j], buf_t("test_data:" + std::to_string(j)));
    }
    EXPECT_EQ(data.msg, buf_t("test_data:" + std::to_string(party_index)));
  });
}

TEST_F(Network4PC, MessageWrapperCopySafety) {
  mpc_runner->run_mpc([](job_mp_t& job) {
    // nonuniform_msg_t copy then use-after-source-destruction should be safe
    coinbase::buf_t sentinel("x");
    auto copy_nu = job.nonuniform_msg<coinbase::buf_t>();
    {
      auto src = job.nonuniform_msg<coinbase::buf_t>();
      int n = job.get_n_parties();
      for (int i = 0; i < n; ++i) src[i] = sentinel;
      copy_nu = src;  // deep copy
    }
    // Write through received() on the copy; should not crash or UAF
    for (int i = 0; i < job.get_n_parties(); ++i) {
      copy_nu.received(i) = sentinel;
      EXPECT_EQ(copy_nu.received(i), sentinel);
    }

    // uniform_msg_t copy then use-after-source-destruction should be safe
    auto copy_u = job.uniform_msg<coinbase::buf_t>();
    {
      auto src = job.uniform_msg<coinbase::buf_t>(coinbase::buf_t("self"));
      copy_u = src;  // deep copy
    }
    for (int i = 0; i < job.get_n_parties(); ++i) {
      copy_u.received(i) = sentinel;
      EXPECT_EQ(copy_u.received(i), sentinel);
    }
  });
}

TEST_F(Network4PC, MessageWrapperReallocSafety) {
  mpc_runner->run_mpc([](job_mp_t& job) {
    auto w = job.nonuniform_msg<coinbase::buf_t>();
    auto cap0 = w.msgs.capacity();
    // Force reallocation of msgs
    while (w.msgs.capacity() == cap0) {
      w.msgs.push_back(coinbase::buf_t());
      if (w.msgs.size() > 1000) break;  // safety guard
    }
    // Using received() after reallocation should be safe
    for (int i = 0; i < job.get_n_parties(); ++i) {
      w.received(i) = coinbase::buf_t("ok");
      EXPECT_EQ(w.received(i), coinbase::buf_t("ok"));
    }
  });
}

TEST_P(NetworkMPC, PairwiseAndBroadcast) {
  const int m = GetParam();
  // This is a special case only used in ecdsa mpc to send both OT messages (pairwise) and a common message (broadcast).
  std::vector<std::vector<int>> ot_role_map(m, std::vector<int>(m));
  for (int i = 0; i <= m - 1; i++) {
    for (int j = i + 1; j < m; j++) {
      ot_role_map[i][j] = ecdsampc::ot_sender;
      ot_role_map[j][i] = ecdsampc::ot_receiver;
    }
  }

  mpc_runner->run_mpc([&ot_role_map, m](job_mp_t& job) {
    error_t rv = UNINITIALIZED_ERROR;
    auto party_index = job.get_party_idx();
    auto data = job.uniform_msg<buf_t>(buf_t("test_data:" + std::to_string(party_index)));
    party_set_t ot_receivers = ecdsampc::ot_receivers_for(party_index, m, ot_role_map);
    auto ot_msg = job.inplace_msg<buf_t>([&party_index](int j) -> auto {
      return buf_t("test_data:" + std::to_string(party_index) + std::to_string(j));
    });
    rv = ecdsampc::plain_broadcast_and_pairwise_message(job, ot_receivers, ot_msg, data);
    ASSERT_EQ(rv, 0);

    for (int j = 0; j < m; j++) {
      EXPECT_EQ(data.received(j), buf_t("test_data:" + std::to_string(j)));
      EXPECT_EQ(data.all_received()[j], buf_t("test_data:" + std::to_string(j)));

      if (ot_role_map[j][party_index] == ecdsampc::ot_sender) {
        EXPECT_EQ(ot_msg.received(j), buf_t("test_data:" + std::to_string(j) + std::to_string(party_index)));
      } else if (ot_role_map[party_index][j] == ecdsampc::ot_receiver) {
        EXPECT_EQ(ot_msg.received(j), buf_t());
      }
    }
    EXPECT_EQ(data.msg, buf_t("test_data:" + std::to_string(party_index)));
  });
}
INSTANTIATE_TEST_SUITE_P(, NetworkMPC, testing::Values(2, 4, 5, 10, 32, 64));

}  // namespace
