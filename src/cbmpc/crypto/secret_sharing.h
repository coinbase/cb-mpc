#pragma once

#include <cbmpc/crypto/base.h>
#include <cbmpc/zk/zk_ec.h>

namespace coinbase::crypto::ss {

template <typename T>
using party_map_t = std::map<pname_t, T>;

std::vector<bn_t> share_and(const mod_t &q, const bn_t &x, const int n, crypto::drbg_aes_ctr_t *drbg = nullptr);
std::pair<std::vector<bn_t>, std::vector<bn_t>> share_threshold(const mod_t &q, const bn_t &a, const int threshold,
                                                                const int n, const std::vector<bn_t> &pids,
                                                                crypto::drbg_aes_ctr_t *drbg = nullptr);

enum class node_e {
  NONE = 0,
  LEAF = 1,
  AND = 2,
  OR = 3,
  THRESHOLD = 4,
};

class node_t;

typedef party_map_t<bn_t> ac_shares_t;
typedef party_map_t<bn_t> ac_internal_shares_t;
typedef party_map_t<ecc_point_t> ac_pub_shares_t;
typedef party_map_t<ecc_point_t> ac_internal_pub_shares_t;

class ac_t;
class ac_owned_t;

struct node_t {
  friend class ac_t;
  friend class ac_owned_t;

  node_e type;
  pname_t name;
  int threshold;
  std::vector<node_t *> children;
  node_t *parent = nullptr;

  node_t(node_e _type, pname_t _name, int _threshold = 0) : type(_type), name(_name), threshold(_threshold) {}

  node_t(node_e _type, pname_t _name, int _threshold, std::initializer_list<node_t *> nodes)
      : type(_type), name(_name), threshold(_threshold), children(nodes) {
    for (auto child : nodes) {
      child->parent = this;
    }
  }

  ~node_t();
  node_t *clone() const;

  int get_n() const { return int(children.size()); }
  std::string get_path() const;

  static bn_t pid_from_path(const std::string &path);
  bn_t get_pid() const;

  std::vector<std::string> list_leaf_paths() const;
  std::set<pname_t> list_leaf_names() const;
  const node_t *find(const pname_t &path) const;
  void add_child_node(node_t *node);
  void remove_and_delete();

  error_t validate_tree() const {
    std::set<pname_t> names;
    return validate_tree(names);
  }
  error_t validate_tree(std::set<pname_t> &names) const;
  bool enough_for_quorum(const std::set<pname_t> &names) const;

  std::vector<node_t *> get_sorted_children() const;

 private:
  node_t() {}
  void convert_node(coinbase::converter_t &c);
};

static std::string get_node_path(const std::string &parent_path, const node_t *node) {
  if (!node->parent) return "";
  return parent_path + "/" + node->name;
}

class ac_t {
 public:
  explicit ac_t() {}
  explicit ac_t(const node_t *_root) : root(_root) {}

  const node_t *get_root() const { return root; }
  bool has_root() const { return root != nullptr; }

  error_t validate_tree() const { return root->validate_tree(); }

  const node_t *find(const pname_t &name) const { return root->find(name); }
  std::set<pname_t> list_leaf_names() const { return root->list_leaf_names(); }
  std::set<const node_t *> list_pub_data_nodes() const;
  int get_pub_data_size(const node_t *node) const {
    if (node->type == node_e::AND)
      return node->get_n();
    else if (node->type == node_e::THRESHOLD)
      return node->threshold;
    else
      return 0;
  }

  bool enough_for_quorum(const std::set<pname_t> names) const { return root->enough_for_quorum(names); }
  template <typename T>
  bool enough_for_quorum(const party_map_t<T> &map) const {
    std::set<pname_t> names;
    for (const auto &[name, value] : map) names.insert(name);
    return root->enough_for_quorum(names);
  }

  /**
   * @specs:
   * - basic-primitives-spec | ac-Share-1P
   */
  ac_shares_t share(const mod_t &q, const bn_t &x, drbg_aes_ctr_t *drbg = nullptr) const;
  error_t share_with_internals(const mod_t &q, const bn_t &x, ac_shares_t &shares,
                               ac_internal_shares_t &ac_internal_shares,
                               ac_internal_pub_shares_t &ac_internal_pub_shares, drbg_aes_ctr_t *drbg = nullptr) const;
  error_t verify_share_against_ancestors_pub_data(const ecc_point_t &Q, const bn_t &si,
                                                  const ac_internal_pub_shares_t &pub_data, const pname_t &leaf) const;

  /**
   * @specs:
   * - basic-primitives-spec | ac-Reconstruct-1P
   */
  error_t reconstruct(const mod_t &q, const ac_shares_t &shares, bn_t &x) const;

  /**
   * @specs:
   * - basic-primitives-spec | ac-Reconstruct-Exponent-1P
   */
  error_t reconstruct_exponent(const ac_pub_shares_t &shares, ecc_point_t &P) const;

  const node_t *root = nullptr;
  ecc_point_t G;
};

class ac_owned_t : public ac_t {
 public:
  ac_owned_t() = default;
  explicit ac_owned_t(const node_t *_root) { assign(_root); }
  explicit ac_owned_t(const ac_t &ac) { assign(ac.root); }
  ~ac_owned_t() { delete root; }
  void assign(const node_t *_root) {
    delete root;
    root = _root->clone();
  }
  ac_owned_t(const ac_owned_t &src) : ac_t() { assign(src.root); }
  ac_owned_t(ac_owned_t &&src) : ac_t() {
    root = src.root;
    src.root = nullptr;
  }
  ac_owned_t &operator=(const ac_owned_t &src) {
    if (&src != this) assign(src.root);
    return *this;
  }
  ac_owned_t &operator=(ac_owned_t &&src) {
    if (&src != this) {
      root = src.root;
      src.root = nullptr;
    }
    return *this;
  }
  void convert(coinbase::converter_t &c);
};

}  // namespace coinbase::crypto::ss