#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

namespace coinbase::api {

// Access structure used by threshold protocols (e.g., threshold DKG / refresh).
//
// This is a lightweight view type:
// - Leaf party names are represented as `std::string_view` and must outlive the
//   protocol call that consumes the access structure.
//   (For example, do not construct leaves from temporary `std::string`s.)
// - Internal node names are not exposed; the library assigns deterministic,
//   unique internal names when converting to the internal representation.
//
// Notes:
// - Leaf names are expected to match the `job_mp_t::party_names` values.
// - The root node is unnamed in the internal representation; therefore the root
//   of an access structure cannot be a leaf node.
struct access_structure_t {
  enum class node_type : uint8_t {
    leaf = 1,
    and_node = 2,
    or_node = 3,
    threshold = 4,
  };

  node_type type = node_type::leaf;

  // Leaf party name (only meaningful when `type == node_type::leaf`).
  std::string_view leaf_name;

  // Threshold parameter k (only meaningful when `type == node_type::threshold`).
  // Must satisfy: 1 <= k <= children.size().
  int32_t threshold_k = 0;

  // Child nodes (only meaningful when `type != node_type::leaf`).
  std::vector<access_structure_t> children;

  static access_structure_t leaf(std::string_view party_name) {
    access_structure_t n;
    n.type = node_type::leaf;
    n.leaf_name = party_name;
    return n;
  }

  static access_structure_t And(std::vector<access_structure_t> ch) {
    access_structure_t n;
    n.type = node_type::and_node;
    n.children = std::move(ch);
    return n;
  }

  static access_structure_t Or(std::vector<access_structure_t> ch) {
    access_structure_t n;
    n.type = node_type::or_node;
    n.children = std::move(ch);
    return n;
  }

  static access_structure_t Threshold(int32_t k, std::vector<access_structure_t> ch) {
    access_structure_t n;
    n.type = node_type::threshold;
    n.threshold_k = k;
    n.children = std::move(ch);
    return n;
  }
};

}  // namespace coinbase::api
