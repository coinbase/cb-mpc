#include "secret_sharing.h"

#include <cbmpc/core/log.h>
#include <cbmpc/core/utils.h>
#include <cbmpc/crypto/base_pki.h>
#include <cbmpc/crypto/lagrange.h>

namespace coinbase::crypto::ss {

std::vector<bn_t> share_and(const mod_t &q, const bn_t &x, const int n, crypto::drbg_aes_ctr_t *drbg) {
  cb_assert(n > 0);
  std::vector<bn_t> shares(n);
  bn_t sum = 0;
  for (int i = 1; i < n; i++) {
    if (drbg)
      shares[i] = drbg->gen_bn(q);
    else
      shares[i] = bn_t::rand(q);
    MODULO(q) sum += shares[i];
  }
  MODULO(q) shares[0] = x - sum;
  return shares;
}

std::pair<std::vector<bn_t>, std::vector<bn_t>> share_threshold(const mod_t &q, const bn_t &a, const int threshold,
                                                                const int n, const std::vector<bn_t> &pids,
                                                                crypto::drbg_aes_ctr_t *drbg) {
  std::vector<bn_t> shares(n);
  std::vector<bn_t> b(threshold);
  cb_assert(threshold > 0);
  shares.resize(n);
  b.resize(threshold);
  b[0] = a;
  for (int i = 1; i < threshold; i++) {
    if (drbg)
      b[i] = drbg->gen_bn(q);
    else
      b[i] = bn_t::rand(q);
  }
  for (int i = 0; i < n; i++) {
    cb_assert(pids[i] != 0);
    shares[i] = horner_poly(q, b, pids[i]);
  }
  return {shares, b};
}

node_t::~node_t() {
  for (auto node : children) delete node;
}

void node_t::add_child_node(node_t *node) {
  children.push_back(node);
  node->parent = this;
}

error_t node_t::validate_tree(std::set<pname_t> &names) const {
  error_t rv = UNINITIALIZED_ERROR;
  if (name.empty() && parent) return coinbase::error(E_BADARG, "unnamed node");
  if (!name.empty() && !parent) return coinbase::error(E_BADARG, "named root node");
  int n = int(children.size());

  switch (type) {
    case node_e::LEAF:
      if (threshold != 0) return coinbase::error(E_BADARG, "no threshold node");
      if (n != 0) return coinbase::error(E_BADARG, "leaf node must not have children");
      return SUCCESS;
    case node_e::AND:
      if (threshold != 0) return coinbase::error(E_BADARG, "no threshold node");
      if (n == 0) return coinbase::error(E_BADARG, "AND node must have children");
      break;
    case node_e::OR:
      if (threshold != 0) return coinbase::error(E_BADARG, "no threshold node");
      if (n == 0) return coinbase::error(E_BADARG, "OR node must have children");
      break;
    case node_e::THRESHOLD:
      if (threshold < 1) return coinbase::error(E_BADARG, "invalid threshold");
      if (threshold > n) return coinbase::error(E_BADARG, "invalid threshold");
      break;
    default:
      return coinbase::error(E_BADARG, "invalid node type");
  }

  if (names.count(name) > 0) return coinbase::error(E_BADARG, "name duplication");

  names.insert(name);

  for (const node_t *child : children)
    if (rv = child->validate_tree(names)) return rv;

  return SUCCESS;
}

void node_t::convert_node(coinbase::converter_t &c) {
  int temp = int(type);
  c.convert(temp);
  type = node_e(temp);
  c.convert(name, threshold);

  uint32_t n = get_n();
  c.convert_len(n);

  for (int i = 0; i < n; i++) {
    node_t *child = c.is_write() ? children[i] : new node_t();
    child->convert_node(c);

    if (c.is_error()) {
      if (!c.is_write()) delete child;
      break;
    }

    if (!c.is_write()) add_child_node(child);
  }
}

// ac stands for access structure
void ac_owned_t::convert(coinbase::converter_t &c)  // static
{
  bool exists = (root != nullptr);
  c.convert(exists);
  error_t rv = UNINITIALIZED_ERROR;

  if (exists) {
    if (!c.is_write()) {
      delete root;
      root = new node_t();
    }

    ((node_t *)root)->convert_node(c);
    if (c.is_write()) return;

    if (!c.is_error()) {
      rv = root->validate_tree();
      if (rv == 0) return;
    }
  }

  delete root;
  root = nullptr;
  if (rv) c.set_error(rv);
}

std::vector<node_t *> node_t::get_sorted_children() const {
  std::vector<node_t *> sorted = children;
  std::sort(
      sorted.begin(), sorted.end(), [](node_t * n1, node_t * n2) -> auto{ return n1->name < n2->name; });
  return sorted;
}

static int find_child_index(const node_t *node, const std::string &name) {
  int n = int(node->children.size());
  for (int i = 0; i < n; i++) {
    if (node->children[i]->name == name) return i;
  }
  return -1;
}

node_t *node_t::clone() const {
  node_t *node = new node_t(type, name, threshold);
  for (const node_t *child : children) {
    node->add_child_node(child->clone());
  }
  return node;
}

void node_t::remove_and_delete() {
  if (parent) {
    auto &parent_list = parent->children;
    auto it = std::find(parent_list.begin(), parent_list.end(), this);
    if (it != parent_list.end()) parent_list.erase(it);
  }
  delete this;
}

std::string node_t::get_path() const {
  std::string path;
  const node_t *node = this;
  while (node) {
    if (path.empty())
      path = node->name;
    else
      path = node->name + "/" + path;
    node = node->parent;
  }
  return path;
}

bn_t node_t::pid_from_path(const std::string &path) { return pid_from_name(strext::tokenize(path, "/").back()); }

bn_t node_t::get_pid() const { return pid_from_name(name); }

const node_t *node_t::find(const pname_t &name) const {
  if (this->name == name) return this;
  for (const auto child : children) {
    const node_t *res = child->find(name);
    if (res) return res;
  }
  return nullptr;
}

static void list_leaf_paths_recursive(const node_t *node, const std::string &parent_path,
                                      std::vector<std::string> &list) {
  std::string path = get_node_path(parent_path, node);

  if (node->type == node_e::LEAF) {
    list.push_back(path);
  } else {
    for (const node_t *child : node->children) list_leaf_paths_recursive(child, path, list);
  }
}

std::vector<std::string> node_t::list_leaf_paths() const {
  std::vector<std::string> list;
  list_leaf_paths_recursive(this, "", list);
  return list;
}

static void list_leaf_names_recursive(const node_t *node, std::set<pname_t> &list) {
  if (node->type == node_e::LEAF) {
    list.insert(node->name);
  } else {
    for (const node_t *child : node->children) list_leaf_names_recursive(child, list);
  }
}

std::set<pname_t> node_t::list_leaf_names() const {
  std::set<pname_t> list;
  list_leaf_names_recursive(this, list);
  return list;
}

bool node_t::enough_for_quorum(const std::set<pname_t> &names) const {
  int count = 0;

  switch (type) {
    case node_e::LEAF:
      return names.find(this->name) != names.end();
    case node_e::OR:
      for (int i = 0; i < get_n(); i++) {
        if (children[i]->enough_for_quorum(names)) return true;
      }
      return false;
    case node_e::AND:
      for (int i = 0; i < get_n(); i++) {
        if (!children[i]->enough_for_quorum(names)) return false;
      }
      return true;
    case node_e::THRESHOLD:
      for (int i = 0; i < get_n(); i++) {
        if (!children[i]->enough_for_quorum(names)) continue;
        count++;
        if (count >= threshold) return true;
      }
      return false;
    case node_e::NONE: {
      return false;
    } break;
  }

  cb_assert(false);
  return false;
}

static void share_recursive(const mod_t &q, const ecc_point_t &G, const node_t *node, const bn_t &a,
                            const bool output_additional_data, ac_shares_t &ac_shares,
                            ac_internal_shares_t &ac_internal_shares, ac_internal_pub_shares_t &ac_internal_pub_shares,
                            drbg_aes_ctr_t *drbg) {
  auto sorted_children = node->get_sorted_children();
  int n = int(sorted_children.size());

  std::vector<bn_t> children_a(n);
  std::vector<bn_t> b;

  if (output_additional_data) {
    ac_internal_shares[node->name] = a;
    ac_internal_pub_shares[node->name] = a * G;
  }

  switch (node->type) {
    case node_e::LEAF:
      ac_shares[node->name] = a;

      break;
    case node_e::OR:
      for (int i = 0; i < n; i++)
        share_recursive(q, G, sorted_children[i], a, output_additional_data, ac_shares, ac_internal_shares,
                        ac_internal_pub_shares, drbg);

      break;
    case node_e::AND:
      children_a = share_and(q, a, n, drbg);
      for (int i = 0; i < n; i++)
        share_recursive(q, G, sorted_children[i], children_a[i], output_additional_data, ac_shares, ac_internal_shares,
                        ac_internal_pub_shares, drbg);

      break;
    case node_e::THRESHOLD: {
      std::vector<bn_t> pids(n);
      for (int i = 0; i < n; i++) pids[i] = sorted_children[i]->get_pid();
      b.resize(node->threshold);
      std::tie(children_a, b) = share_threshold(q, a, node->threshold, n, pids, drbg);
      for (int i = 0; i < n; i++)
        share_recursive(q, G, sorted_children[i], children_a[i], output_additional_data, ac_shares, ac_internal_shares,
                        ac_internal_pub_shares, drbg);

    } break;
    case node_e::NONE: {
      return;
    } break;
  }
}

ac_shares_t ac_t::share(const mod_t &q, const bn_t &x, drbg_aes_ctr_t *drbg) const {
  ac_shares_t shares;
  ac_internal_shares_t dummy;
  ac_internal_pub_shares_t dummy_pub;

  bool output_additional_data = false;
  share_recursive(q, G, root, x, output_additional_data, shares, dummy, dummy_pub, drbg);
  return shares;
}

error_t ac_t::share_with_internals(const mod_t &q, const bn_t &x, ac_shares_t &shares,
                                   ac_internal_shares_t &ac_internal_shares,
                                   ac_internal_pub_shares_t &ac_internal_pub_shares, drbg_aes_ctr_t *drbg) const {
  bool output_additional_data = true;
  share_recursive(q, G, root, x, output_additional_data, shares, ac_internal_shares, ac_internal_pub_shares, drbg);
  return SUCCESS;
}

error_t ac_t::verify_share_against_ancestors_pub_data(const ecc_point_t &Q, const bn_t &si,
                                                      const ac_internal_pub_shares_t &pub_data,
                                                      const pname_t &leaf) const {
  vartime_scope_t vartime_scope;
  auto node = find(leaf);
  if (node == nullptr || node->type != node_e::LEAF) return coinbase::error(E_NOT_FOUND);

  ecc_point_t expected_pub_share = si * G;
  const node_t *child = nullptr;

  while (node != nullptr) {
    auto sorted_children = node->get_sorted_children();

    auto pub_shares = pub_data.at(node->name);
    ecc_point_t my_pub_share = pub_shares;

    if (node->type == node_e::LEAF || node->type == node_e::OR) {
      if (my_pub_share != expected_pub_share) {
        return coinbase::error(E_CRYPTO);
      }
    } else if (node->type == node_e::AND) {
      ecc_point_t expected_sum = Q.get_curve().infinity();
      for (int i = 0; i < sorted_children.size(); i++) {
        auto child_pub_shares = pub_data.at(sorted_children[i]->name);
        expected_sum += child_pub_shares;
      }
      if (expected_sum != my_pub_share) return coinbase::error(E_CRYPTO);
    } else if (node->type == node_e::THRESHOLD) {
      std::vector<ecc_point_t> quorum(node->threshold);
      std::vector<bn_t> quorum_pids(node->threshold);
      for (int i = 0; i < node->threshold; i++) {
        quorum[i] = pub_data.at(sorted_children[i]->name);
        quorum_pids[i] = sorted_children[i]->get_pid();
      }

      // NOTE: this is a less efficient implementation. More optimized implementation should store coefficients in the
      //       node and run `horner_poly` for each child. At the moment, the code is reconstructing the polynomial from
      //       scratch for itself and each of its children.
      if (my_pub_share != lagrange_interpolate_exponent(0, quorum, quorum_pids)) return coinbase::error(E_CRYPTO);

      for (int i = node->threshold; i < sorted_children.size(); i++) {
        if (pub_data.at(sorted_children[i]->name) !=
            lagrange_interpolate_exponent(sorted_children[i]->get_pid(), quorum, quorum_pids))
          return coinbase::error(E_CRYPTO);
      }
    } else {
      return coinbase::error(E_BADARG);
    }

    expected_pub_share = my_pub_share;
    child = node;
    node = node->parent;
  }

  if (Q != expected_pub_share) return coinbase::error(E_CRYPTO);

  return SUCCESS;
}

static error_t reconstruct_recursive(const mod_t &q, const node_t *node, const ac_shares_t &shares, bn_t &x) {
  error_t rv = UNINITIALIZED_ERROR;
  int n = node->get_n();

  switch (node->type) {
    case node_e::LEAF: {
      const auto &[found, share] = lookup(shares, node->name);
      if (!found) {
        return coinbase::error(E_INSUFFICIENT);
      }
      x = share;
    } break;
    case node_e::OR:
      for (int i = 0; i < n; i++) {
        rv = reconstruct_recursive(q, node->children[i], shares, x);
        if (rv == SUCCESS) break;
        if (rv != E_INSUFFICIENT) return rv;
      }
      if (rv != SUCCESS) return coinbase::error(E_INSUFFICIENT);
      break;
    case node_e::AND:
      x = 0;
      for (int i = 0; i < n; i++) {
        bn_t share;
        if (rv = reconstruct_recursive(q, node->children[i], shares, share)) return rv;
        MODULO(q) x += share;
      }
      break;

    case node_e::THRESHOLD: {
      std::vector<bn_t> pids(node->threshold);
      std::vector<bn_t> node_shares(node->threshold);
      int count = 0;

      for (int i = 0; i < n; i++) {
        bn_t share;
        rv = reconstruct_recursive(q, node->children[i], shares, share);
        if (rv == E_INSUFFICIENT) {
          rv = SUCCESS;
          continue;
        }
        if (rv) return rv;

        pids[count] = node->children[i]->get_pid();
        node_shares[count] = share;
        count++;
        if (count == node->threshold) break;
      }

      if (count < node->threshold) {
        dylog_disable_scope_t dylog_disable_scope;
        return coinbase::error(E_INSUFFICIENT);
      }

      x = lagrange_interpolate(0, node_shares, pids, q);
    } break;

    case node_e::NONE: {
      return coinbase::error(E_CRYPTO);
    } break;
  }

  return SUCCESS;
}

error_t ac_t::reconstruct(const mod_t &q, const ac_shares_t &shares, bn_t &x) const {
  return reconstruct_recursive(q, root, shares, x);
}

static error_t reconstruct_exponent_recursive(const node_t *node, const ac_pub_shares_t &shares, ecc_point_t &P) {
  error_t rv = UNINITIALIZED_ERROR;
  int n = node->get_n();
  const pname_t &name = node->name;

  switch (node->type) {
    case node_e::LEAF: {
      const auto &[found, share] = lookup(shares, name);
      if (!found) {
        dylog_disable_scope_t dylog_disable_scope;
        return coinbase::error(E_INSUFFICIENT, "missing share for leaf node " + name);
      }
      P = share;
    } break;

    case node_e::OR:
      for (int i = 0; i < n; i++) {
        rv = reconstruct_exponent_recursive(node->children[i], shares, P);
        if (rv == SUCCESS) break;
        if (rv != E_INSUFFICIENT) return coinbase::error(rv, "cannot reconstruct OR node " + name);
      }
      if (rv != SUCCESS) return coinbase::error(E_INSUFFICIENT);
      break;

    case node_e::AND:
      for (int i = 0; i < n; i++) {
        ecc_point_t Pi;
        if (rv = reconstruct_exponent_recursive(node->children[i], shares, Pi))
          return coinbase::error(rv, "cannot reconstruct AND node " + name);
        if (i == 0)
          P = Pi;
        else
          P = P + Pi;
      }
      break;

    case node_e::THRESHOLD: {
      std::vector<bn_t> pids(node->threshold);
      std::vector<ecc_point_t> node_shares(node->threshold);
      int count = 0;

      for (int i = 0; i < n; i++) {
        ecc_point_t Pi;
        rv = reconstruct_exponent_recursive(node->children[i], shares, Pi);
        if (rv == E_INSUFFICIENT) {
          rv = SUCCESS;
          continue;
        }
        if (rv) return coinbase::error(rv, "cannot reconstruct threshold node " + name);

        pids[count] = node->children[i]->get_pid();
        node_shares[count] = Pi;
        count++;
        if (count == node->threshold) break;
      }

      if (count < node->threshold) {
        dylog_disable_scope_t dylog_disable_scope;
        return coinbase::error(E_INSUFFICIENT, "missing share for threshold node " + name);
      }

      P = lagrange_interpolate_exponent(0, node_shares, pids);
    } break;

    case node_e::NONE: {
      return coinbase::error(E_CRYPTO);
    } break;
  }

  return SUCCESS;
}

error_t ac_t::reconstruct_exponent(const ac_pub_shares_t &shares, ecc_point_t &P) const {
  return reconstruct_exponent_recursive(root, shares, P);
}

static void list_pub_data_nodes_recursive(const node_t *node, std::set<const node_t *> &node_set) {
  if (node->type == node_e::LEAF) {
    return;
  }
  for (const node_t *child : node->children) {
    list_pub_data_nodes_recursive(child, node_set);
  }
  if (node->type == node_e::AND || node->type == node_e::THRESHOLD) {
    node_set.insert(node);
  }
}

std::set<const node_t *> ac_t::list_pub_data_nodes() const {
  std::set<const node_t *> nodes;
  list_pub_data_nodes_recursive(root, nodes);
  return nodes;
}

}  // namespace coinbase::crypto::ss