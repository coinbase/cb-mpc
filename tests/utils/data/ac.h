#pragma once

#include <gtest/gtest.h>

#include <cbmpc/crypto/secret_sharing.h>

#include "test_node.h"

namespace coinbase::testutils {

/* The structure of test_root:
AND (Root)
├── leaf1
├── OR (or2)
│   ├── AND (and21)
│   │   ├── leaf211
│   │   ├── ...
│   │   └── leaf215
│   ├── leaf22
│   └── THRESHOLD-4-of-9 (th23)
│       ├── leaf231
│       ├── ...
│       └── leaf239
└── THRESHOLD-2-of-4 (th3)
    ├── AND (and31)
    │   ├── leaf311
    │   └── leaf312
    ├── leaf32
    ├── OR (or33)
    │   ├── leaf331
    │   └── leaf332
    └── THRESHOLD-2-of-3 (th34)
        ├── leaf341
        ├── leaf342
        └── leaf343
*/

using node_t = coinbase::crypto::ss::node_t;
using node_e = coinbase::crypto::ss::node_e;
using ac_t = coinbase::crypto::ss::ac_t;

class TestNodes : public ::testing::Test {
 protected:
  void SetUp() override {
    simple_and_node = new node_t(node_e::AND, "", 0,
                                 {
                                     new node_t(node_e::LEAF, "leaf1"),
                                     new node_t(node_e::LEAF, "leaf2"),
                                     new node_t(node_e::LEAF, "leaf3"),
                                 });
    simple_or_node = new node_t(node_e::OR, "", 0,
                                {
                                    new node_t(node_e::LEAF, "leaf1"),
                                    new node_t(node_e::LEAF, "leaf2"),
                                    new node_t(node_e::LEAF, "leaf3"),
                                });
    simple_threshold_node = new node_t(node_e::THRESHOLD, "", 2,
                                       {
                                           new node_t(node_e::LEAF, "leaf1"),
                                           new node_t(node_e::LEAF, "leaf2"),
                                           new node_t(node_e::LEAF, "leaf3"),
                                       });

    test_root = getTestRoot();

    all_roots = {simple_and_node, simple_or_node, simple_threshold_node, test_root};
  }

  void TearDown() override {
    delete simple_and_node;
    delete simple_or_node;
    delete simple_threshold_node;
  }

  node_t* simple_and_node;
  node_t* simple_or_node;
  node_t* simple_threshold_node;
  node_t* test_root;
  std::vector<node_t*> all_roots;
  // valid quorum for the test_root
  std::set<crypto::pname_t> valid_quorum = {"leaf1", "leaf22", "leaf32", "leaf331"};
};

class TestAC : public TestNodes {
 protected:
  void SetUp() override {
    TestNodes::SetUp();
    simple_and_ac.root = simple_and_node;
    simple_or_ac.root = simple_or_node;
    simple_threshold_ac.root = simple_threshold_node;
    test_ac.root = test_root;
    all_acs = {simple_and_ac, simple_or_ac, simple_threshold_ac, test_ac};
  }

  void TearDown() override { TestNodes::TearDown(); }

  ac_t simple_and_ac;
  ac_t simple_or_ac;
  ac_t simple_threshold_ac;
  ac_t test_ac;
  std::vector<ac_t> all_acs;
};

}  // namespace coinbase::testutils