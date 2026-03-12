#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cbmpc/core/access_structure.h>
#include <cbmpc/core/error.h>
#include <cbmpc/internal/crypto/base_ecc.h>
#include <cbmpc/internal/crypto/secret_sharing.h>
#include <cbmpc/internal/protocol/mpc_job.h>

namespace coinbase::api::detail {

inline constexpr size_t MAX_ACCESS_STRUCTURE_DEPTH = 128;
inline constexpr size_t MAX_ACCESS_STRUCTURE_NODES = 4096;

inline error_t validate_access_structure_node_impl(const access_structure_t& n, size_t depth, size_t& nodes_seen) {
  if (++nodes_seen > MAX_ACCESS_STRUCTURE_NODES) return coinbase::error(E_RANGE, "access_structure: too many nodes");
  if (depth > MAX_ACCESS_STRUCTURE_DEPTH) return coinbase::error(E_RANGE, "access_structure: too deep");

  switch (n.type) {
    case access_structure_t::node_type::leaf:
      if (n.leaf_name.empty()) return coinbase::error(E_BADARG, "access_structure: leaf name must be non-empty");
      if (!n.children.empty()) return coinbase::error(E_BADARG, "access_structure: leaf node must not have children");
      if (n.threshold_k != 0) return coinbase::error(E_BADARG, "access_structure: leaf node must not set threshold_k");
      return SUCCESS;

    case access_structure_t::node_type::and_node:
    case access_structure_t::node_type::or_node:
      if (n.children.empty()) return coinbase::error(E_BADARG, "access_structure: AND/OR node must have children");
      if (!n.leaf_name.empty())
        return coinbase::error(E_BADARG, "access_structure: internal node must not set leaf_name");
      if (n.threshold_k != 0)
        return coinbase::error(E_BADARG, "access_structure: AND/OR node must not set threshold_k");
      break;

    case access_structure_t::node_type::threshold:
      if (n.children.empty()) return coinbase::error(E_BADARG, "access_structure: THRESHOLD node must have children");
      if (!n.leaf_name.empty())
        return coinbase::error(E_BADARG, "access_structure: internal node must not set leaf_name");
      if (n.threshold_k < 1) return coinbase::error(E_BADARG, "access_structure: invalid threshold_k");
      if (static_cast<size_t>(n.threshold_k) > n.children.size())
        return coinbase::error(E_BADARG, "access_structure: threshold_k > children.size()");
      break;
    default:
      return coinbase::error(E_BADARG, "invalid node type");
  }

  for (const auto& ch : n.children) {
    const error_t rv = validate_access_structure_node_impl(ch, depth + 1, nodes_seen);
    if (rv) return rv;
  }
  return SUCCESS;
}

inline error_t validate_access_structure_node(const access_structure_t& n) {
  size_t nodes_seen = 0;
  return validate_access_structure_node_impl(n, /*depth=*/0, nodes_seen);
}

inline error_t collect_leaf_names(const access_structure_t& n, std::set<std::string>& out) {
  struct frame_t {
    const access_structure_t* node = nullptr;
    size_t depth = 0;
  };

  size_t nodes_seen = 0;
  std::vector<frame_t> stack;
  stack.reserve(64);
  stack.push_back(frame_t{&n, 0});

  while (!stack.empty()) {
    const frame_t frame = stack.back();
    stack.pop_back();

    if (!frame.node) return coinbase::error(E_BADARG, "access_structure: invalid node");
    if (++nodes_seen > MAX_ACCESS_STRUCTURE_NODES) return coinbase::error(E_RANGE, "access_structure: too many nodes");
    if (frame.depth > MAX_ACCESS_STRUCTURE_DEPTH) return coinbase::error(E_RANGE, "access_structure: too deep");

    if (frame.node->type == access_structure_t::node_type::leaf) {
      out.insert(std::string(frame.node->leaf_name));
      continue;
    }

    for (const auto& ch : frame.node->children) {
      stack.push_back(frame_t{&ch, frame.depth + 1});
    }
  }

  return SUCCESS;
}

inline std::string gen_internal_node_name(std::unordered_set<std::string>& used, uint64_t& counter) {
  // Generate deterministic unique names that do not collide with leaf names.
  while (true) {
    counter++;
    std::string name = "__cbmpc_ac_node_" + std::to_string(counter);
    if (used.insert(name).second) return name;
  }
}

inline error_t build_internal_ac_node(const access_structure_t& in, size_t depth, bool is_root,
                                      std::unordered_set<std::string>& used_names, uint64_t& name_counter,
                                      coinbase::crypto::ss::node_t*& out) {
  out = nullptr;
  if (depth > MAX_ACCESS_STRUCTURE_DEPTH) return coinbase::error(E_RANGE, "access_structure: too deep");

  using coinbase::crypto::ss::node_e;
  using coinbase::crypto::ss::node_t;

  switch (in.type) {
    case access_structure_t::node_type::leaf: {
      // Leaves must be named. (Root leaf is not supported; root is unnamed.)
      if (is_root) return coinbase::error(E_BADARG, "access_structure: root cannot be a leaf node");
      auto* node = new node_t(node_e::LEAF, std::string(in.leaf_name));
      out = node;
      return SUCCESS;
    }

    case access_structure_t::node_type::and_node:
    case access_structure_t::node_type::or_node:
    case access_structure_t::node_type::threshold: {
      if (in.children.empty()) return coinbase::error(E_BADARG, "access_structure: internal node missing children");

      const node_e t = (in.type == access_structure_t::node_type::and_node)  ? node_e::AND
                       : (in.type == access_structure_t::node_type::or_node) ? node_e::OR
                                                                             : node_e::THRESHOLD;

      const int k = (in.type == access_structure_t::node_type::threshold) ? in.threshold_k : 0;

      const std::string node_name = is_root ? std::string() : gen_internal_node_name(used_names, name_counter);
      auto* node = new node_t(t, node_name, k);

      for (const auto& ch : in.children) {
        node_t* child = nullptr;
        const error_t rv = build_internal_ac_node(ch, depth + 1, /*is_root=*/false, used_names, name_counter, child);
        if (rv) {
          delete node;  // deletes any already-added children
          return rv;
        }
        node->add_child_node(child);
      }

      out = node;
      return SUCCESS;
    }
  }

  return coinbase::error(E_BADARG, "access_structure: invalid node type");
}

inline error_t to_internal_access_structure(const access_structure_t& in,
                                            const std::vector<std::string_view>& party_names,
                                            coinbase::crypto::ecurve_t curve, coinbase::crypto::ss::ac_owned_t& out) {
  // Clear any existing tree.
  delete out.root;
  out.root = nullptr;

  if (!curve.valid()) return coinbase::error(E_BADARG, "access_structure: invalid curve");

  // Basic shape validation (independent of job).
  error_t rv = validate_access_structure_node(in);
  if (rv) return rv;
  if (in.type == access_structure_t::node_type::leaf)
    return coinbase::error(E_BADARG, "access_structure: root cannot be leaf");

  // Validate that leaf set matches job.party_names exactly.
  std::set<std::string> leaf_names;
  rv = collect_leaf_names(in, leaf_names);
  if (rv) return rv;

  std::set<std::string> party_set;
  for (const auto& name_view : party_names) party_set.insert(std::string(name_view));

  if (leaf_names != party_set)
    return coinbase::error(E_BADARG, "access_structure: leaf names must match job.party_names exactly");

  // Build internal node tree with generated internal node names.
  std::unordered_set<std::string> used;
  used.reserve(leaf_names.size() * 2 + 8);
  used.insert(std::string());  // root name
  for (const auto& name : leaf_names) used.insert(name);

  uint64_t counter = 0;
  coinbase::crypto::ss::node_t* root = nullptr;
  rv = build_internal_ac_node(in, /*depth=*/0, /*is_root=*/true, used, counter, root);
  if (rv) return rv;

  out.curve = curve;
  out.root = root;

  rv = out.validate_tree();
  if (rv) {
    delete out.root;
    out.root = nullptr;
    return rv;
  }

  return SUCCESS;
}

inline error_t to_internal_party_set(const std::vector<std::string_view>& party_names,
                                     const std::vector<std::string_view>& quorum_party_names,
                                     coinbase::mpc::party_set_t& out) {
  out = coinbase::mpc::party_set_t::empty();
  if (quorum_party_names.empty()) return coinbase::error(E_BADARG, "quorum_party_names must be non-empty");

  std::unordered_map<std::string_view, int32_t> index_by_name;
  index_by_name.reserve(party_names.size());
  for (size_t i = 0; i < party_names.size(); i++) index_by_name.emplace(party_names[i], static_cast<int32_t>(i));

  std::unordered_set<std::string_view> seen;
  seen.reserve(quorum_party_names.size());
  for (const auto& qn : quorum_party_names) {
    if (!seen.insert(qn).second) return coinbase::error(E_BADARG, "duplicate quorum party name");
    const auto it = index_by_name.find(qn);
    if (it == index_by_name.end()) return coinbase::error(E_BADARG, "unknown quorum party name");
    out.add(it->second);
  }

  return SUCCESS;
}

}  // namespace coinbase::api::detail
