#pragma once

#include <cbmpc/crypto/secret_sharing.h>

namespace coinbase::testutils {

// Add this helper function outside of the test fixture classes
inline crypto::ss::node_t* getTestRoot() {
  // Allocate once and reuse.
  static crypto::ss::node_t* singleton = new crypto::ss::node_t(
      crypto::ss::node_e::AND, "", 0,
      {
          new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf1"),
          new crypto::ss::node_t(
              crypto::ss::node_e::OR, "or2", 0,
              {
                  new crypto::ss::node_t(crypto::ss::node_e::AND, "and21", 0,
                                         {
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf211"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf212"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf213"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf214"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf215"),
                                         }),
                  new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf22"),
                  new crypto::ss::node_t(crypto::ss::node_e::THRESHOLD, "th23", 4,
                                         {
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf231"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf232"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf233"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf234"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf235"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf236"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf237"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf238"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf239"),
                                         }),
              }),
          new crypto::ss::node_t(
              crypto::ss::node_e::THRESHOLD, "th3", 2,
              {
                  new crypto::ss::node_t(crypto::ss::node_e::AND, "and31", 0,
                                         {
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf311"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf312"),
                                         }),
                  new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf32"),
                  new crypto::ss::node_t(crypto::ss::node_e::OR, "or33", 0,
                                         {
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf331"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf332"),
                                         }),
                  new crypto::ss::node_t(crypto::ss::node_e::THRESHOLD, "th34", 2,
                                         {
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf341"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf342"),
                                             new crypto::ss::node_t(crypto::ss::node_e::LEAF, "leaf343"),
                                         }),
              }),
      });
  return singleton;
}

}  // namespace coinbase::testutils
