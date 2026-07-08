#include <functional>
#include <gtest/gtest.h>

#include <cbmpc/internal/protocol/hd_tree_bip32.h>

#include "utils/test_macros.h"

namespace {

using namespace coinbase;
using namespace coinbase::crypto;
using namespace coinbase::mpc;

TEST(HDTreeBip32, RootSerializationAndKShareHelpers) {
  vartime_scope_t vartime_scope;
  const ecurve_t curve = curve_secp256k1;
  const ecc_generator_point_t& G = curve.generator();

  hd_root_t root;
  root.x_share = bn_t(11);
  root.k_share = bn_t(17);
  root.Q = root.x_share * G;
  root.K = root.k_share * G + bn_t(23) * G;

  EXPECT_EQ(root.get_K_share(), bn_t(17) * G);
  EXPECT_EQ(root.get_other_K_share(), bn_t(23) * G);

  hd_root_t decoded;
  ASSERT_OK(coinbase::deser(coinbase::ser(root), decoded));
  EXPECT_EQ(decoded.x_share, root.x_share);
  EXPECT_EQ(decoded.k_share, root.k_share);
  EXPECT_EQ(decoded.Q, root.Q);
  EXPECT_EQ(decoded.K, root.K);
}

TEST(HDTreeBip32, PathConstructionSerializationAndHashing) {
  const uint32_t raw[] = {1, 2, 0x80000003};
  bip32_path_t path(raw, 3);

  ASSERT_FALSE(path.empty());
  ASSERT_EQ(path.count(), 3);
  EXPECT_EQ(path[0], raw[0]);
  EXPECT_EQ(path[1], raw[1]);
  EXPECT_EQ(path[2], raw[2]);
  EXPECT_EQ(path.get_indices()[2], raw[2]);
  EXPECT_EQ(path.get(), (std::vector<uint32_t>{raw[0], raw[1], raw[2]}));

  bip32_path_t same;
  same.append(raw[0]);
  same.append(raw[1]);
  same.append(raw[2]);
  EXPECT_TRUE(path == same);
  EXPECT_EQ(path.hash(), same.hash());
  EXPECT_EQ(std::hash<bip32_path_t>{}(path), path.hash());

  bip32_path_t decoded;
  ASSERT_OK(coinbase::deser(coinbase::ser(path), decoded));
  EXPECT_TRUE(decoded == path);
  EXPECT_EQ(decoded.hash(), path.hash());
}

TEST(HDTreeBip32, EmptyPathHasStableHash) {
  bip32_path_t empty;
  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(empty.count(), 0);
  EXPECT_EQ(empty.hash(), 0);
  EXPECT_EQ(std::hash<bip32_path_t>{}(empty), 0);
}

TEST(HDTreeBip32, DetectsDuplicateDerivationPaths) {
  const uint32_t first[] = {1, 2, 3};
  const uint32_t duplicate[] = {1, 2, 3};
  const uint32_t sibling[] = {1, 2, 4};

  EXPECT_FALSE(bip32_path_t::has_duplicate({bip32_path_t(first, 3), bip32_path_t(sibling, 3)}));
  EXPECT_TRUE(
      bip32_path_t::has_duplicate({bip32_path_t(first, 3), bip32_path_t(sibling, 3), bip32_path_t(duplicate, 3)}));
}

}  // namespace
