#include <cstdint>
#include <gtest/gtest.h>
#include <vector>

#include <cbmpc/c_api/access_structure.h>
#include <cbmpc/c_api/access_structure_adapter.h>
#include <cbmpc/core/access_structure.h>

namespace {

struct access_structure_storage_t {
  std::vector<cbmpc_access_structure_node_t> nodes;
  std::vector<int32_t> child_indices;

  cbmpc_access_structure_t view() const {
    return cbmpc_access_structure_t{
        nodes.data(),
        static_cast<int32_t>(nodes.size()),
        child_indices.empty() ? nullptr : child_indices.data(),
        static_cast<int32_t>(child_indices.size()),
        0,
    };
  }
};

access_structure_storage_t make_chain(size_t depth) {
  access_structure_storage_t storage;
  storage.nodes.resize(depth + 1);
  storage.child_indices.resize(depth);

  for (size_t i = 0; i < depth; i++) {
    storage.nodes[i] = cbmpc_access_structure_node_t{
        CBMPC_ACCESS_STRUCTURE_NODE_AND, nullptr, 0, static_cast<int32_t>(i), 1,
    };
    storage.child_indices[i] = static_cast<int32_t>(i + 1);
  }

  storage.nodes[depth] = cbmpc_access_structure_node_t{
      CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0,
  };
  return storage;
}

access_structure_storage_t make_wide_tree(size_t nodes_count) {
  access_structure_storage_t storage;
  storage.nodes.resize(nodes_count);
  storage.child_indices.resize(nodes_count - 1);

  storage.nodes[0] = cbmpc_access_structure_node_t{
      CBMPC_ACCESS_STRUCTURE_NODE_OR, nullptr, 0, 0, static_cast<int32_t>(storage.child_indices.size()),
  };

  for (size_t i = 1; i < nodes_count; i++) {
    storage.nodes[i] = cbmpc_access_structure_node_t{
        CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0,
    };
    storage.child_indices[i - 1] = static_cast<int32_t>(i);
  }
  return storage;
}

}  // namespace

TEST(CApiAccessStructureAdapter, AcceptsMaximumDepth) {
  const auto storage = make_chain(CBMPC_ACCESS_STRUCTURE_MAX_DEPTH);
  const cbmpc_access_structure_t input = storage.view();
  coinbase::api::access_structure_t output;

  EXPECT_EQ(coinbase::capi::detail::to_cpp_access_structure(&input, output), CBMPC_SUCCESS);
}

TEST(CApiAccessStructureAdapter, RejectsDepthAboveLimitBeforeRecursing) {
  const auto storage = make_chain(CBMPC_ACCESS_STRUCTURE_MAX_DEPTH + 1);
  const cbmpc_access_structure_t input = storage.view();
  coinbase::api::access_structure_t output;

  EXPECT_EQ(coinbase::capi::detail::to_cpp_access_structure(&input, output), E_RANGE);
}

TEST(CApiAccessStructureAdapter, AcceptsMaximumNodeCount) {
  const auto storage = make_wide_tree(CBMPC_ACCESS_STRUCTURE_MAX_NODES);
  const cbmpc_access_structure_t input = storage.view();
  coinbase::api::access_structure_t output;

  EXPECT_EQ(coinbase::capi::detail::to_cpp_access_structure(&input, output), CBMPC_SUCCESS);
}

TEST(CApiAccessStructureAdapter, RejectsNodeCountAboveLimitBeforeAllocating) {
  const cbmpc_access_structure_node_t dummy_node = {
      CBMPC_ACCESS_STRUCTURE_NODE_LEAF, "p0", 0, 0, 0,
  };
  const cbmpc_access_structure_t input = {
      &dummy_node, CBMPC_ACCESS_STRUCTURE_MAX_NODES + 1, nullptr, 0, 0,
  };
  coinbase::api::access_structure_t output;

  EXPECT_EQ(coinbase::capi::detail::to_cpp_access_structure(&input, output), E_RANGE);
}
