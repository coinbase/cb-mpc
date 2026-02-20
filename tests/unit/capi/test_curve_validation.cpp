#include <gtest/gtest.h>

#include <cbmpc/capi/ecdsa_2p.h>
#include <cbmpc/capi/job.h>
#include <cbmpc/capi/tdh2.h>
#include <cbmpc/core/error.h>

namespace {

static cbmpc_error_t dummy_send(void* /*ctx*/, int32_t /*receiver*/, const uint8_t* /*data*/, int /*size*/) {
  return E_GENERAL;
}

static cbmpc_error_t dummy_receive(void* /*ctx*/, int32_t /*sender*/, cmem_t* /*out_msg*/) { return E_GENERAL; }

static cbmpc_error_t dummy_receive_all(void* /*ctx*/, const int32_t* /*senders*/, int /*senders_count*/,
                                       cmems_t* /*out_msgs*/) {
  return E_GENERAL;
}

}  // namespace

TEST(CApiCurveValidation, Ecdsa2pcRejectsInvalidCurve) {
  const cbmpc_transport_t t = {
      /*ctx=*/nullptr,
      /*send=*/dummy_send,
      /*receive=*/dummy_receive,
      /*receive_all=*/nullptr,
      /*free=*/nullptr,
  };

  cmem_t key_blob = {reinterpret_cast<uint8_t*>(0x1), 123};
  const cbmpc_2pc_job_t job = {CBMPC_2PC_P1, "p1", "p2", &t};
  const cbmpc_error_t rv = cbmpc_ecdsa_2p_dkg(&job, static_cast<cbmpc_curve_id_t>(42), &key_blob);
  EXPECT_EQ(rv, E_BADARG);
  EXPECT_EQ(key_blob.data, nullptr);
  EXPECT_EQ(key_blob.size, 0);
}

TEST(CApiCurveValidation, Tdh2RejectsInvalidCurve) {
  const cbmpc_transport_t t = {
      /*ctx=*/nullptr,
      /*send=*/dummy_send,
      /*receive=*/dummy_receive,
      /*receive_all=*/dummy_receive_all,
      /*free=*/nullptr,
  };

  const char* names[2] = {"p0", "p1"};
  const cbmpc_mp_job_t job = {/*self=*/0, /*party_names=*/names, /*party_names_count=*/2, /*transport=*/&t};

  cmem_t pk = {reinterpret_cast<uint8_t*>(0x1), 123};
  cmems_t pub_shares = {123, reinterpret_cast<uint8_t*>(0x1), reinterpret_cast<int*>(0x1)};
  cmem_t priv_share = {reinterpret_cast<uint8_t*>(0x1), 123};
  cmem_t sid = {reinterpret_cast<uint8_t*>(0x1), 123};

  const cbmpc_error_t rv =
      cbmpc_tdh2_dkg_additive(&job, static_cast<cbmpc_curve_id_t>(42), &pk, &pub_shares, &priv_share, &sid);
  EXPECT_EQ(rv, E_BADARG);
  EXPECT_EQ(pk.data, nullptr);
  EXPECT_EQ(pk.size, 0);
  EXPECT_EQ(pub_shares.count, 0);
  EXPECT_EQ(pub_shares.data, nullptr);
  EXPECT_EQ(pub_shares.sizes, nullptr);
  EXPECT_EQ(priv_share.data, nullptr);
  EXPECT_EQ(priv_share.size, 0);
  EXPECT_EQ(sid.data, nullptr);
  EXPECT_EQ(sid.size, 0);
}
