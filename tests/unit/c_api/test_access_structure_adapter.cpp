#include <gtest/gtest.h>

#include <vector>

#include <cbmpc/c_api/access_structure.h>
#include <cbmpc/core/error.h>

#include <cbmpc/c_api/access_structure_adapter.h>

namespace {

using coinbase::api::detail::MAX_ACCESS_STRUCTURE_DEPTH;
using coinbase::api::detail::MAX_ACCESS_STRUCTURE_NODES;
using coinbase::capi::detail::to_cpp_access_structure;

TEST(CApiAccessStructureAdapter, RejectsTooDeepTree) {
  constexpr size_t kDepth = MAX_ACCESS_STRUCTURE_DEPTH + 1;

  std::vector<cbmpc_access_structure_node_t> nodes(kDepth + 1);
  std::vector<int32_t> child_indices(kDepth);
  const char leaf_name[] = "leaf";

  for (size_t i = 0; i < kDepth; i++) {
    child_indices[i] = static_cast<int32_t>(i + 1);
    nodes[i] = cbmpc_access_structure_node_t{
        CBMPC_ACCESS_STRUCTURE_NODE_AND,
        /*leaf_name=*/nullptr,
        /*threshold_k=*/0,
        /*child_indices_offset=*/static_cast<int32_t>(i),
        /*child_indices_count=*/1,
    };
  }
  nodes[kDepth] = cbmpc_access_structure_node_t{
      CBMPC_ACCESS_STRUCTURE_NODE_LEAF,
      leaf_name,
      /*threshold_k=*/0,
      /*child_indices_offset=*/0,
      /*child_indices_count=*/0,
  };

  const cbmpc_access_structure_t ac = {
      nodes.data(),
      static_cast<int32_t>(nodes.size()),
      child_indices.data(),
      static_cast<int32_t>(child_indices.size()),
      /*root_index=*/0,
  };

  coinbase::api::access_structure_t out;
  EXPECT_EQ(to_cpp_access_structure(&ac, out), E_RANGE);
}

TEST(CApiAccessStructureAdapter, RejectsTooManyNodes) {
  constexpr size_t kLeafCount = MAX_ACCESS_STRUCTURE_NODES;

  std::vector<cbmpc_access_structure_node_t> nodes(kLeafCount + 1);
  std::vector<int32_t> child_indices(kLeafCount);
  const char leaf_name[] = "leaf";

  nodes[0] = cbmpc_access_structure_node_t{
      CBMPC_ACCESS_STRUCTURE_NODE_AND,
      /*leaf_name=*/nullptr,
      /*threshold_k=*/0,
      /*child_indices_offset=*/0,
      /*child_indices_count=*/static_cast<int32_t>(kLeafCount),
  };

  for (size_t i = 0; i < kLeafCount; i++) {
    child_indices[i] = static_cast<int32_t>(i + 1);
    nodes[i + 1] = cbmpc_access_structure_node_t{
        CBMPC_ACCESS_STRUCTURE_NODE_LEAF,
        leaf_name,
        /*threshold_k=*/0,
        /*child_indices_offset=*/0,
        /*child_indices_count=*/0,
    };
  }

  const cbmpc_access_structure_t ac = {
      nodes.data(),
      static_cast<int32_t>(nodes.size()),
      child_indices.data(),
      static_cast<int32_t>(child_indices.size()),
      /*root_index=*/0,
  };

  coinbase::api::access_structure_t out;
  EXPECT_EQ(to_cpp_access_structure(&ac, out), E_RANGE);
}

}  // namespace
