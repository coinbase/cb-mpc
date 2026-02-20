#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <cbmpc/c_api/access_structure.h>
#include <cbmpc/c_api/tdh2.h>
#include <cbmpc/core/error.h>

#include "test_transport_harness.h"

namespace {

using coinbase::testutils::mpc_net_context_t;
using coinbase::testutils::capi_harness::make_transport;
using coinbase::testutils::capi_harness::run_mp;
using coinbase::testutils::capi_harness::transport_ctx_t;

static void expect_eq(cmem_t a, cmem_t b) {
  ASSERT_EQ(a.size, b.size);
  if (a.size > 0) {
    ASSERT_NE(a.data, nullptr);
    ASSERT_NE(b.data, nullptr);
    ASSERT_EQ(std::memcmp(a.data, b.data, static_cast<size_t>(a.size)), 0);
  }
}

static void expect_eq_cmems(cmems_t a, cmems_t b) {
  ASSERT_EQ(a.count, b.count);
  if (a.count == 0) return;
  ASSERT_NE(a.sizes, nullptr);
  ASSERT_NE(b.sizes, nullptr);
  int total_a = 0;
  int total_b = 0;
  for (int i = 0; i < a.count; i++) {
    ASSERT_EQ(a.sizes[i], b.sizes[i]);
    total_a += a.sizes[i];
    total_b += b.sizes[i];
  }
  ASSERT_EQ(total_a, total_b);
  if (total_a > 0) {
    ASSERT_NE(a.data, nullptr);
    ASSERT_NE(b.data, nullptr);
    ASSERT_EQ(std::memcmp(a.data, b.data, static_cast<size_t>(total_a)), 0);
  }
}

static cmems_t pack_cmems_copy(const std::vector<cmem_t>& mems) {
  cmems_t out{0, nullptr, nullptr};
  if (mems.empty()) return out;
  if (mems.size() > static_cast<size_t>(INT_MAX)) return out;

  int total = 0;
  for (const auto& m : mems) {
    if (m.size < 0) return cmems_t{0, nullptr, nullptr};
    if (m.size > INT_MAX - total) return cmems_t{0, nullptr, nullptr};
    total += m.size;
  }

  out.count = static_cast<int>(mems.size());
  out.sizes = static_cast<int*>(cbmpc_malloc(sizeof(int) * mems.size()));
  if (!out.sizes) return cmems_t{0, nullptr, nullptr};
  out.data = (total > 0) ? static_cast<uint8_t*>(cbmpc_malloc(static_cast<size_t>(total))) : nullptr;
  if (total > 0 && !out.data) {
    cbmpc_free(out.sizes);
    return cmems_t{0, nullptr, nullptr};
  }

  int offset = 0;
  for (int i = 0; i < out.count; i++) {
    out.sizes[i] = mems[i].size;
    if (mems[i].size) {
      std::memmove(out.data + offset, mems[i].data, static_cast<size_t>(mems[i].size));
      offset += mems[i].size;
    }
  }

  return out;
}

}  // namespace

TEST(CApiTdh2, DkgRoundTripEncryptDecrypt) {
  constexpr int n = 3;

  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  transport_ctx_t ctx[n];
  cbmpc_transport_t transports[n];
  for (int i = 0; i < n; i++) {
    ctx[i] = transport_ctx_t{peers[static_cast<size_t>(i)], /*free_calls=*/nullptr};
    transports[i] = make_transport(&ctx[i]);
  }

  const char* party_names[n] = {"p0", "p1", "p2"};

  std::vector<cmem_t> public_keys(n, cmem_t{nullptr, 0});
  std::vector<cmems_t> public_shares(n, cmems_t{0, nullptr, nullptr});
  std::vector<cmem_t> private_shares(n, cmem_t{nullptr, 0});
  std::vector<cmem_t> sids(n, cmem_t{nullptr, 0});
  std::vector<cbmpc_error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        const cbmpc_mp_job_t job = {
            /*self=*/i,
            /*party_names=*/party_names,
            /*party_names_count=*/n,
            /*transport=*/&transports[i],
        };
        return cbmpc_tdh2_dkg_additive(&job, CBMPC_CURVE_SECP256K1, &public_keys[static_cast<size_t>(i)],
                                       &public_shares[static_cast<size_t>(i)], &private_shares[static_cast<size_t>(i)],
                                       &sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  for (int i = 0; i < n; i++) {
    ASSERT_GT(public_keys[static_cast<size_t>(i)].size, 0);
    ASSERT_EQ(public_shares[static_cast<size_t>(i)].count, n);
    ASSERT_GT(private_shares[static_cast<size_t>(i)].size, 0);
    ASSERT_GT(sids[static_cast<size_t>(i)].size, 0);
  }

  for (int i = 1; i < n; i++) {
    expect_eq(public_keys[0], public_keys[static_cast<size_t>(i)]);
    expect_eq_cmems(public_shares[0], public_shares[static_cast<size_t>(i)]);
    expect_eq(sids[0], sids[static_cast<size_t>(i)]);
  }

  uint8_t plaintext_bytes[32];
  for (int i = 0; i < 32; i++) plaintext_bytes[i] = static_cast<uint8_t>(0xA5 ^ i);
  const cmem_t plaintext = {plaintext_bytes, 32};
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("tdh2-label")), 9};

  cmem_t ciphertext{nullptr, 0};
  ASSERT_EQ(cbmpc_tdh2_encrypt(public_keys[0], plaintext, label, &ciphertext), CBMPC_SUCCESS);
  ASSERT_GT(ciphertext.size, 0);
  ASSERT_EQ(cbmpc_tdh2_verify(public_keys[0], ciphertext, label), CBMPC_SUCCESS);

  std::vector<cmem_t> partials(n, cmem_t{nullptr, 0});
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(cbmpc_tdh2_partial_decrypt(private_shares[static_cast<size_t>(i)], ciphertext, label,
                                         &partials[static_cast<size_t>(i)]),
              CBMPC_SUCCESS);
    ASSERT_GT(partials[static_cast<size_t>(i)].size, 0);
  }

  const cmems_t partials_flat = pack_cmems_copy(partials);
  ASSERT_EQ(partials_flat.count, n);

  cmem_t decrypted{nullptr, 0};
  ASSERT_EQ(cbmpc_tdh2_combine_additive(public_keys[0], public_shares[0], label, partials_flat, ciphertext, &decrypted),
            CBMPC_SUCCESS);
  ASSERT_EQ(decrypted.size, 32);
  ASSERT_NE(decrypted.data, nullptr);
  EXPECT_EQ(std::memcmp(decrypted.data, plaintext_bytes, 32), 0);

  // Wrong label should fail verification.
  const cmem_t wrong_label = {reinterpret_cast<uint8_t*>(const_cast<char*>("wrong-label")), 11};
  EXPECT_NE(cbmpc_tdh2_verify(public_keys[0], ciphertext, wrong_label), CBMPC_SUCCESS);

  cbmpc_cmem_free(decrypted);
  cbmpc_cmems_free(partials_flat);
  for (auto p : partials) cbmpc_cmem_free(p);
  cbmpc_cmem_free(ciphertext);
  for (auto m : sids) cbmpc_cmem_free(m);
  for (auto m : private_shares) cbmpc_cmem_free(m);
  for (auto m : public_shares) cbmpc_cmems_free(m);
  for (auto m : public_keys) cbmpc_cmem_free(m);
}

TEST(CApiTdh2, ThresholdDkg_Combine2of3) {
  constexpr int n = 3;

  std::vector<std::shared_ptr<mpc_net_context_t>> peers;
  peers.reserve(n);
  for (int i = 0; i < n; i++) peers.push_back(std::make_shared<mpc_net_context_t>(i));
  for (const auto& p : peers) p->init_with_peers(peers);

  transport_ctx_t ctx[n];
  cbmpc_transport_t transports[n];
  for (int i = 0; i < n; i++) {
    ctx[i] = transport_ctx_t{peers[static_cast<size_t>(i)], /*free_calls=*/nullptr};
    transports[i] = make_transport(&ctx[i]);
  }

  const char* party_names[n] = {"p0", "p1", "p2"};

  // Access structure: THRESHOLD[2](p0, p1, p2)
  const int32_t child_indices[] = {1, 2, 3};
  const cbmpc_access_structure_node_t nodes[] = {
      {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, /*leaf_name=*/nullptr, /*k=*/2, /*off=*/0, /*cnt=*/3},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p0", /*k=*/0, /*off=*/0, /*cnt=*/0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p1", /*k=*/0, /*off=*/0, /*cnt=*/0},
      {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, /*leaf_name=*/"p2", /*k=*/0, /*off=*/0, /*cnt=*/0},
  };
  const cbmpc_access_structure_t ac = {
      /*nodes=*/nodes,
      /*nodes_count=*/static_cast<int32_t>(sizeof(nodes) / sizeof(nodes[0])),
      /*child_indices=*/child_indices,
      /*child_indices_count=*/static_cast<int32_t>(sizeof(child_indices) / sizeof(child_indices[0])),
      /*root_index=*/0,
  };

  const char* dkg_quorum[] = {"p0", "p1"};

  std::vector<cmem_t> public_keys(n, cmem_t{nullptr, 0});
  std::vector<cmems_t> public_shares(n, cmems_t{0, nullptr, nullptr});
  std::vector<cmem_t> private_shares(n, cmem_t{nullptr, 0});
  std::vector<cmem_t> sids(n, cmem_t{nullptr, 0});
  std::vector<cbmpc_error_t> rvs;

  run_mp(
      peers,
      [&](int i) {
        const cbmpc_mp_job_t job = {
            /*self=*/i,
            /*party_names=*/party_names,
            /*party_names_count=*/n,
            /*transport=*/&transports[i],
        };
        return cbmpc_tdh2_dkg_ac(&job, CBMPC_CURVE_P256,
                                 /*sid_in=*/cmem_t{nullptr, 0}, &ac, dkg_quorum,
                                 /*quorum_party_names_count=*/2, &public_keys[static_cast<size_t>(i)],
                                 &public_shares[static_cast<size_t>(i)], &private_shares[static_cast<size_t>(i)],
                                 &sids[static_cast<size_t>(i)]);
      },
      rvs);
  for (auto rv : rvs) ASSERT_EQ(rv, CBMPC_SUCCESS);
  for (int i = 1; i < n; i++) {
    expect_eq(public_keys[0], public_keys[static_cast<size_t>(i)]);
    expect_eq_cmems(public_shares[0], public_shares[static_cast<size_t>(i)]);
    expect_eq(sids[0], sids[static_cast<size_t>(i)]);
  }

  uint8_t plaintext_bytes[32];
  for (int i = 0; i < 32; i++) plaintext_bytes[i] = static_cast<uint8_t>(0x5A ^ i);
  const cmem_t plaintext = {plaintext_bytes, 32};
  const cmem_t label = {reinterpret_cast<uint8_t*>(const_cast<char*>("tdh2-label")), 9};

  cmem_t ciphertext{nullptr, 0};
  ASSERT_EQ(cbmpc_tdh2_encrypt(public_keys[0], plaintext, label, &ciphertext), CBMPC_SUCCESS);
  ASSERT_EQ(cbmpc_tdh2_verify(public_keys[0], ciphertext, label), CBMPC_SUCCESS);

  cmem_t partial0{nullptr, 0};
  cmem_t partial1{nullptr, 0};
  ASSERT_EQ(cbmpc_tdh2_partial_decrypt(private_shares[0], ciphertext, label, &partial0), CBMPC_SUCCESS);
  ASSERT_EQ(cbmpc_tdh2_partial_decrypt(private_shares[1], ciphertext, label, &partial1), CBMPC_SUCCESS);

  const char* partial_names[] = {"p0", "p1"};
  const std::vector<cmem_t> partial_vec = {partial0, partial1};
  const cmems_t partials_flat = pack_cmems_copy(partial_vec);

  cmem_t decrypted{nullptr, 0};
  ASSERT_EQ(cbmpc_tdh2_combine_ac(&ac, public_keys[0], party_names, n, public_shares[0], label, partial_names, 2,
                                  partials_flat, ciphertext, &decrypted),
            CBMPC_SUCCESS);
  ASSERT_EQ(decrypted.size, 32);
  EXPECT_EQ(std::memcmp(decrypted.data, plaintext_bytes, 32), 0);

  // Not enough partial decryptions should fail.
  const char* one_name[] = {"p0"};
  const std::vector<cmem_t> one_partial_vec = {partial0};
  const cmems_t one_partials = pack_cmems_copy(one_partial_vec);
  cmem_t decrypted2{nullptr, 0};
  EXPECT_NE(cbmpc_tdh2_combine_ac(&ac, public_keys[0], party_names, n, public_shares[0], label, one_name, 1,
                                  one_partials, ciphertext, &decrypted2),
            CBMPC_SUCCESS);

  cbmpc_cmem_free(decrypted2);
  cbmpc_cmems_free(one_partials);
  cbmpc_cmem_free(decrypted);
  cbmpc_cmems_free(partials_flat);
  cbmpc_cmem_free(partial0);
  cbmpc_cmem_free(partial1);
  cbmpc_cmem_free(ciphertext);
  for (auto m : sids) cbmpc_cmem_free(m);
  for (auto m : private_shares) cbmpc_cmem_free(m);
  for (auto m : public_shares) cbmpc_cmems_free(m);
  for (auto m : public_keys) cbmpc_cmem_free(m);
}

TEST(CApiTdh2, ValidatesArgs) {
  cmem_t pk{reinterpret_cast<uint8_t*>(0x1), 123};
  cmem_t priv{reinterpret_cast<uint8_t*>(0x1), 123};
  cmem_t sid{reinterpret_cast<uint8_t*>(0x1), 123};
  cmems_t pub{123, reinterpret_cast<uint8_t*>(0x1), reinterpret_cast<int*>(0x1)};

  const cbmpc_transport_t bad_transport = {/*ctx=*/nullptr, /*send=*/nullptr, /*receive=*/nullptr,
                                           /*receive_all=*/nullptr,
                                           /*free=*/nullptr};
  const char* names[2] = {"p0", "p1"};
  const cbmpc_mp_job_t bad_job = {/*self=*/0, /*party_names=*/names, /*party_names_count=*/2,
                                  /*transport=*/&bad_transport};

  EXPECT_EQ(cbmpc_tdh2_dkg_additive(&bad_job, CBMPC_CURVE_SECP256K1, &pk, &pub, &priv, &sid), E_BADARG);
  EXPECT_EQ(pk.data, nullptr);
  EXPECT_EQ(pk.size, 0);
  EXPECT_EQ(pub.count, 0);
  EXPECT_EQ(priv.data, nullptr);
  EXPECT_EQ(priv.size, 0);
  EXPECT_EQ(sid.data, nullptr);
  EXPECT_EQ(sid.size, 0);

  EXPECT_EQ(cbmpc_tdh2_encrypt(cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, cmem_t{nullptr, 0}, nullptr), E_BADARG);
}

// ------------ Disclaimer: All the following tests have been generated by AI ------------

// ---------------------------------------------------------------------------
// Negative tests
// ---------------------------------------------------------------------------

#include <cbmpc/internal/core/log.h>

namespace {

const cmem_t empty_cmem{nullptr, 0};
const cmems_t empty_cmems{0, nullptr, nullptr};

uint8_t g_garbage[] = {0xDE, 0xAD, 0xBE, 0xEF};
const cmem_t garbage_cmem{g_garbage, 4};

const char* g_names[] = {"p0", "p1", "p2"};
const cbmpc_mp_job_t g_bad_job = {0, g_names, 3, nullptr};

const int32_t g_child_indices[] = {1, 2, 3};
const cbmpc_access_structure_node_t g_nodes[] = {
    {CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD, nullptr, 2, 0, 3},
    {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0},
    {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p1", 0, 0, 0},
    {CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p2", 0, 0, 0},
};
const cbmpc_access_structure_t g_ac = {g_nodes, 4, g_child_indices, 3, 0};

}  // namespace

// --- dkg_additive ---

TEST(CApiTdh2Neg, DkgAdditiveNullOutPk) {
  dylog_disable_scope_t no_log;
  cmems_t pub = empty_cmems;
  cmem_t priv = empty_cmem;
  cmem_t sid = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_additive(&g_bad_job, CBMPC_CURVE_SECP256K1, nullptr, &pub, &priv, &sid), E_BADARG);
}

TEST(CApiTdh2Neg, DkgAdditiveNullOutPubShares) {
  dylog_disable_scope_t no_log;
  cmem_t pk = empty_cmem;
  cmem_t priv = empty_cmem;
  cmem_t sid = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_additive(&g_bad_job, CBMPC_CURVE_SECP256K1, &pk, nullptr, &priv, &sid), E_BADARG);
}

TEST(CApiTdh2Neg, DkgAdditiveNullOutPrivShare) {
  dylog_disable_scope_t no_log;
  cmem_t pk = empty_cmem;
  cmems_t pub = empty_cmems;
  cmem_t sid = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_additive(&g_bad_job, CBMPC_CURVE_SECP256K1, &pk, &pub, nullptr, &sid), E_BADARG);
}

TEST(CApiTdh2Neg, DkgAdditiveNullOutSid) {
  dylog_disable_scope_t no_log;
  cmem_t pk = empty_cmem;
  cmems_t pub = empty_cmems;
  cmem_t priv = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_additive(&g_bad_job, CBMPC_CURVE_SECP256K1, &pk, &pub, &priv, nullptr), E_BADARG);
}

TEST(CApiTdh2Neg, DkgAdditiveNullJob) {
  dylog_disable_scope_t no_log;
  cmem_t pk = empty_cmem;
  cmems_t pub = empty_cmems;
  cmem_t priv = empty_cmem;
  cmem_t sid = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_additive(nullptr, CBMPC_CURVE_SECP256K1, &pk, &pub, &priv, &sid), E_BADARG);
}

TEST(CApiTdh2Neg, DkgAdditiveInvalidCurve) {
  dylog_disable_scope_t no_log;
  cmem_t pk = empty_cmem;
  cmems_t pub = empty_cmems;
  cmem_t priv = empty_cmem;
  cmem_t sid = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_additive(&g_bad_job, static_cast<cbmpc_curve_id_t>(0), &pk, &pub, &priv, &sid), E_BADARG);
}

TEST(CApiTdh2Neg, DkgAdditiveEd25519Rejected) {
  dylog_disable_scope_t no_log;
  cmem_t pk = empty_cmem;
  cmems_t pub = empty_cmems;
  cmem_t priv = empty_cmem;
  cmem_t sid = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_additive(&g_bad_job, CBMPC_CURVE_ED25519, &pk, &pub, &priv, &sid), E_BADARG);
}

// --- dkg_ac ---

TEST(CApiTdh2Neg, DkgAcNullOutPk) {
  dylog_disable_scope_t no_log;
  const char* quorum[] = {"p0", "p1"};
  cmems_t pub = empty_cmems;
  cmem_t priv = empty_cmem;
  cmem_t sid = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_ac(&g_bad_job, CBMPC_CURVE_P256, empty_cmem, &g_ac, quorum, 2, nullptr, &pub, &priv, &sid),
            E_BADARG);
}

TEST(CApiTdh2Neg, DkgAcNullOutSid) {
  dylog_disable_scope_t no_log;
  const char* quorum[] = {"p0", "p1"};
  cmem_t pk = empty_cmem;
  cmems_t pub = empty_cmems;
  cmem_t priv = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_ac(&g_bad_job, CBMPC_CURVE_P256, empty_cmem, &g_ac, quorum, 2, &pk, &pub, &priv, nullptr),
            E_BADARG);
}

TEST(CApiTdh2Neg, DkgAcNullJob) {
  dylog_disable_scope_t no_log;
  const char* quorum[] = {"p0", "p1"};
  cmem_t pk = empty_cmem;
  cmems_t pub = empty_cmems;
  cmem_t priv = empty_cmem;
  cmem_t sid = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_ac(nullptr, CBMPC_CURVE_P256, empty_cmem, &g_ac, quorum, 2, &pk, &pub, &priv, &sid),
            E_BADARG);
}

TEST(CApiTdh2Neg, DkgAcInvalidCurve) {
  dylog_disable_scope_t no_log;
  const char* quorum[] = {"p0", "p1"};
  cmem_t pk = empty_cmem;
  cmems_t pub = empty_cmems;
  cmem_t priv = empty_cmem;
  cmem_t sid = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_ac(&g_bad_job, static_cast<cbmpc_curve_id_t>(0), empty_cmem, &g_ac, quorum, 2, &pk, &pub,
                              &priv, &sid),
            E_BADARG);
}

TEST(CApiTdh2Neg, DkgAcNullAc) {
  dylog_disable_scope_t no_log;
  const char* quorum[] = {"p0", "p1"};
  cmem_t pk = empty_cmem;
  cmems_t pub = empty_cmems;
  cmem_t priv = empty_cmem;
  cmem_t sid = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_dkg_ac(&g_bad_job, CBMPC_CURVE_P256, empty_cmem, nullptr, quorum, 2, &pk, &pub, &priv, &sid),
            E_BADARG);
}

// --- encrypt ---

TEST(CApiTdh2Neg, EncryptNullOutput) {
  dylog_disable_scope_t no_log;
  EXPECT_EQ(cbmpc_tdh2_encrypt(garbage_cmem, garbage_cmem, garbage_cmem, nullptr), E_BADARG);
}

TEST(CApiTdh2Neg, EncryptEmptyPk) {
  dylog_disable_scope_t no_log;
  cmem_t ct = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_encrypt(empty_cmem, garbage_cmem, garbage_cmem, &ct), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, EncryptGarbagePk) {
  dylog_disable_scope_t no_log;
  cmem_t ct = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_encrypt(garbage_cmem, garbage_cmem, garbage_cmem, &ct), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, EncryptEmptyPlaintext) {
  dylog_disable_scope_t no_log;
  cmem_t ct = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_encrypt(garbage_cmem, empty_cmem, garbage_cmem, &ct), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, EncryptEmptyLabel) {
  dylog_disable_scope_t no_log;
  cmem_t ct = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_encrypt(garbage_cmem, garbage_cmem, empty_cmem, &ct), CBMPC_SUCCESS);
}

// --- verify ---

TEST(CApiTdh2Neg, VerifyEmptyPk) {
  dylog_disable_scope_t no_log;
  EXPECT_NE(cbmpc_tdh2_verify(empty_cmem, garbage_cmem, garbage_cmem), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, VerifyGarbagePk) {
  dylog_disable_scope_t no_log;
  EXPECT_NE(cbmpc_tdh2_verify(garbage_cmem, garbage_cmem, garbage_cmem), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, VerifyEmptyCt) {
  dylog_disable_scope_t no_log;
  EXPECT_NE(cbmpc_tdh2_verify(garbage_cmem, empty_cmem, garbage_cmem), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, VerifyGarbageCt) {
  dylog_disable_scope_t no_log;
  EXPECT_NE(cbmpc_tdh2_verify(garbage_cmem, garbage_cmem, garbage_cmem), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, VerifyEmptyLabel) {
  dylog_disable_scope_t no_log;
  EXPECT_NE(cbmpc_tdh2_verify(garbage_cmem, garbage_cmem, empty_cmem), CBMPC_SUCCESS);
}

// --- partial_decrypt ---

TEST(CApiTdh2Neg, PartialDecryptNullOutput) {
  dylog_disable_scope_t no_log;
  EXPECT_EQ(cbmpc_tdh2_partial_decrypt(garbage_cmem, garbage_cmem, garbage_cmem, nullptr), E_BADARG);
}

TEST(CApiTdh2Neg, PartialDecryptEmptyPrivShare) {
  dylog_disable_scope_t no_log;
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_partial_decrypt(empty_cmem, garbage_cmem, garbage_cmem, &out), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, PartialDecryptGarbagePrivShare) {
  dylog_disable_scope_t no_log;
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_partial_decrypt(garbage_cmem, garbage_cmem, garbage_cmem, &out), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, PartialDecryptEmptyCt) {
  dylog_disable_scope_t no_log;
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_partial_decrypt(garbage_cmem, empty_cmem, garbage_cmem, &out), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, PartialDecryptGarbageCt) {
  dylog_disable_scope_t no_log;
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_partial_decrypt(garbage_cmem, garbage_cmem, garbage_cmem, &out), CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, PartialDecryptEmptyLabel) {
  dylog_disable_scope_t no_log;
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_partial_decrypt(garbage_cmem, garbage_cmem, empty_cmem, &out), CBMPC_SUCCESS);
}

// --- combine_additive ---

TEST(CApiTdh2Neg, CombineAdditiveNullOutput) {
  dylog_disable_scope_t no_log;
  EXPECT_EQ(cbmpc_tdh2_combine_additive(garbage_cmem, empty_cmems, garbage_cmem, empty_cmems, garbage_cmem, nullptr),
            E_BADARG);
}

TEST(CApiTdh2Neg, CombineAdditiveEmptyPk) {
  dylog_disable_scope_t no_log;
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_combine_additive(empty_cmem, empty_cmems, garbage_cmem, empty_cmems, garbage_cmem, &out),
            CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, CombineAdditiveGarbagePk) {
  dylog_disable_scope_t no_log;
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_combine_additive(garbage_cmem, empty_cmems, garbage_cmem, empty_cmems, garbage_cmem, &out),
            CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, CombineAdditiveEmptyCt) {
  dylog_disable_scope_t no_log;
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_combine_additive(garbage_cmem, empty_cmems, garbage_cmem, empty_cmems, empty_cmem, &out),
            CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, CombineAdditiveEmptyLabel) {
  dylog_disable_scope_t no_log;
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_combine_additive(garbage_cmem, empty_cmems, empty_cmem, empty_cmems, garbage_cmem, &out),
            CBMPC_SUCCESS);
}

// --- combine_ac ---

TEST(CApiTdh2Neg, CombineAcNullOutput) {
  dylog_disable_scope_t no_log;
  const char* party_names[] = {"p0", "p1", "p2"};
  const char* partial_names[] = {"p0", "p1"};
  EXPECT_EQ(cbmpc_tdh2_combine_ac(&g_ac, garbage_cmem, party_names, 3, empty_cmems, garbage_cmem, partial_names, 2,
                                  empty_cmems, garbage_cmem, nullptr),
            E_BADARG);
}

TEST(CApiTdh2Neg, CombineAcNullAc) {
  dylog_disable_scope_t no_log;
  const char* party_names[] = {"p0", "p1", "p2"};
  const char* partial_names[] = {"p0", "p1"};
  cmem_t out = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_combine_ac(nullptr, garbage_cmem, party_names, 3, empty_cmems, garbage_cmem, partial_names, 2,
                                  empty_cmems, garbage_cmem, &out),
            E_BADARG);
}

TEST(CApiTdh2Neg, CombineAcNullPartyNames) {
  dylog_disable_scope_t no_log;
  const char* partial_names[] = {"p0", "p1"};
  cmem_t out = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_combine_ac(&g_ac, garbage_cmem, nullptr, 3, empty_cmems, garbage_cmem, partial_names, 2,
                                  empty_cmems, garbage_cmem, &out),
            E_BADARG);
}

TEST(CApiTdh2Neg, CombineAcNullPartialNames) {
  dylog_disable_scope_t no_log;
  const char* party_names[] = {"p0", "p1", "p2"};
  cmem_t out = empty_cmem;
  EXPECT_EQ(cbmpc_tdh2_combine_ac(&g_ac, garbage_cmem, party_names, 3, empty_cmems, garbage_cmem, nullptr, 2,
                                  empty_cmems, garbage_cmem, &out),
            E_BADARG);
}

TEST(CApiTdh2Neg, CombineAcEmptyPk) {
  dylog_disable_scope_t no_log;
  const char* party_names[] = {"p0", "p1", "p2"};
  const char* partial_names[] = {"p0", "p1"};
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_combine_ac(&g_ac, empty_cmem, party_names, 3, empty_cmems, garbage_cmem, partial_names, 2,
                                  empty_cmems, garbage_cmem, &out),
            CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, CombineAcEmptyCt) {
  dylog_disable_scope_t no_log;
  const char* party_names[] = {"p0", "p1", "p2"};
  const char* partial_names[] = {"p0", "p1"};
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_combine_ac(&g_ac, garbage_cmem, party_names, 3, empty_cmems, garbage_cmem, partial_names, 2,
                                  empty_cmems, empty_cmem, &out),
            CBMPC_SUCCESS);
}

TEST(CApiTdh2Neg, CombineAcEmptyLabel) {
  dylog_disable_scope_t no_log;
  const char* party_names[] = {"p0", "p1", "p2"};
  const char* partial_names[] = {"p0", "p1"};
  cmem_t out = empty_cmem;
  EXPECT_NE(cbmpc_tdh2_combine_ac(&g_ac, garbage_cmem, party_names, 3, empty_cmems, empty_cmem, partial_names, 2,
                                  empty_cmems, garbage_cmem, &out),
            CBMPC_SUCCESS);
}
