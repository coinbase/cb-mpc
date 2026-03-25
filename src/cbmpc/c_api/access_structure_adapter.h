#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include <cbmpc/c_api/access_structure.h>
#include <cbmpc/c_api/common.h>
#include <cbmpc/core/access_structure.h>
#include <cbmpc/core/error.h>

namespace coinbase::capi::detail {

inline cbmpc_error_t to_cpp_quorum_party_names(const char* const* names, int names_count,
                                               std::vector<std::string_view>& out) {
  out.clear();
  if (names_count < 0) return E_BADARG;
  if (names_count == 0) return CBMPC_SUCCESS;
  if (!names) return E_BADARG;

  out.reserve(static_cast<size_t>(names_count));
  for (int i = 0; i < names_count; i++) {
    const char* s = names[i];
    if (!s) return E_BADARG;
    if (s[0] == '\0') return E_BADARG;
    out.emplace_back(s);
  }
  return CBMPC_SUCCESS;
}

inline cbmpc_error_t to_cpp_access_structure(const cbmpc_access_structure_t* in,
                                             coinbase::api::access_structure_t& out) {
  if (!in) return E_BADARG;
  if (in->nodes_count < 0 || in->child_indices_count < 0) return E_BADARG;
  if (in->nodes_count == 0) return E_BADARG;
  if (!in->nodes) return E_BADARG;
  if (in->child_indices_count > 0 && !in->child_indices) return E_BADARG;
  if (in->root_index < 0 || in->root_index >= in->nodes_count) return E_BADARG;

  // state: 0 = unvisited, 1 = visiting, 2 = done
  std::vector<uint8_t> state(static_cast<size_t>(in->nodes_count), 0);

  struct builder_t {
    static cbmpc_error_t build(const cbmpc_access_structure_t* in, int32_t idx, bool is_root,
                               std::vector<uint8_t>& state, coinbase::api::access_structure_t& out) {
      if (idx < 0 || idx >= in->nodes_count) return E_BADARG;

      const auto uidx = static_cast<size_t>(idx);
      if (state[uidx] == 1) return E_BADARG;  // cycle
      if (state[uidx] == 2) return E_BADARG;  // node reuse (DAG)
      state[uidx] = 1;

      const cbmpc_access_structure_node_t& n = in->nodes[uidx];

      if (n.child_indices_offset < 0 || n.child_indices_count < 0) return E_BADARG;
      if (n.child_indices_count > 0) {
        if (n.child_indices_offset > in->child_indices_count - n.child_indices_count) return E_BADARG;
      }

      auto node = coinbase::api::access_structure_t{};

      switch (n.type) {
        case CBMPC_ACCESS_STRUCTURE_NODE_LEAF: {
          if (is_root) return E_BADARG;
          if (!n.leaf_name || n.leaf_name[0] == '\0') return E_BADARG;
          if (n.threshold_k != 0) return E_BADARG;
          if (n.child_indices_count != 0) return E_BADARG;
          node.type = coinbase::api::access_structure_t::node_type::leaf;
          node.leaf_name = std::string_view(n.leaf_name);
        } break;

        case CBMPC_ACCESS_STRUCTURE_NODE_AND:
        case CBMPC_ACCESS_STRUCTURE_NODE_OR:
        case CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD: {
          if (n.leaf_name) return E_BADARG;
          if (n.child_indices_count <= 0) return E_BADARG;

          if (n.type == CBMPC_ACCESS_STRUCTURE_NODE_AND) {
            if (n.threshold_k != 0) return E_BADARG;
            node.type = coinbase::api::access_structure_t::node_type::and_node;
          } else if (n.type == CBMPC_ACCESS_STRUCTURE_NODE_OR) {
            if (n.threshold_k != 0) return E_BADARG;
            node.type = coinbase::api::access_structure_t::node_type::or_node;
          } else {
            if (n.threshold_k < 1) return E_BADARG;
            if (n.threshold_k > n.child_indices_count) return E_BADARG;
            node.type = coinbase::api::access_structure_t::node_type::threshold;
            node.threshold_k = n.threshold_k;
          }

          node.children.reserve(static_cast<size_t>(n.child_indices_count));
          const int32_t off = n.child_indices_offset;
          for (int32_t i = 0; i < n.child_indices_count; i++) {
            const int32_t child_idx = in->child_indices[static_cast<size_t>(off + i)];
            coinbase::api::access_structure_t child;
            const cbmpc_error_t rv = build(in, child_idx, /*is_root=*/false, state, child);
            if (rv) return rv;
            node.children.emplace_back(std::move(child));
          }
        } break;

        default:
          return E_BADARG;
      }

      out = std::move(node);
      state[uidx] = 2;
      return CBMPC_SUCCESS;
    }
  };

  const cbmpc_error_t rv = builder_t::build(in, in->root_index, /*is_root=*/true, state, out);
  if (rv) return rv;

  // Reject unreachable nodes (must be a single rooted tree).
  for (size_t i = 0; i < state.size(); i++) {
    if (state[i] != 2) return E_BADARG;
  }

  return CBMPC_SUCCESS;
}

}  // namespace coinbase::capi::detail
