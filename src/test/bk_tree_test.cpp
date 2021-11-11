#include "catch_amalgamated.hpp"

#include <qs/bk_tree.hpp>
#include <qs/distances.hpp>
#include <qs/list.hpp>
#include <qs/string.h>
#include <qs/vector.hpp>
#include <type_traits>

using sl =
    qs::skip_list<qs::bk_tree_node<qs::string> *, QS_BK_TREE_SKIP_LIST_LEVELS>;

void check_children(qs::bk_tree_node<qs::string> *node, const char *strings[],
                    int num_children) {
  auto &children = node->get_children();
  int counter = 0;
  qs::functions::for_each(
      const_cast<sl &>(children).begin(), const_cast<sl &>(children).end(),
      [&counter, num_children, strings](qs::bk_tree_node<qs::string> *curr) {
        REQUIRE(counter < num_children);
        REQUIRE(!std::strcmp(*(curr->get()), strings[counter]));
        counter++;
      });
}

SCENARIO("BK-Tree correct construction and matching") {
  GIVEN("The strings: hell, help, fall, felt, fell, small, melt and using"
        " hamming distance") {
    auto tree = qs::bk_tree<qs::string>(qs::hamming_distance);
    tree.insert(qs::string("hell"));
    tree.insert(qs::string("help"));
    tree.insert(qs::string("fall"));
    tree.insert(qs::string("felt"));
    tree.insert(qs::string("fell"));
    tree.insert(qs::string("small"));
    tree.insert(qs::string("melt"));

    THEN("'hell' is the root") {
      auto r = tree.get_root();
      REQUIRE(!std::strcmp(*(r->get()), "hell"));

      THEN("'hell' has 3 children: 'help', 'fall' and 'small'") {
        const char *children_strings[3] = {"help", "fall", "small"};

        check_children(r, children_strings, 3);

        THEN("'help' has 1 child: 'fell'") {
          auto &children = r->get_children();

          auto help_node_iter = const_cast<sl &>(children).begin();
          const char *help_children_strings[1] = {"fell"};
          check_children(*help_node_iter, help_children_strings, 1);

          THEN("'fall' has 2 children: 'felt' and 'melt'") {
            auto fall_node_iter = ++help_node_iter;
            const char *fall_children_strings[2] = {"felt", "melt"};
            check_children(*fall_node_iter, fall_children_strings, 2);

            THEN("'small' has no children") {
              auto small_node_iter = ++fall_node_iter;
              auto &small_children = (*small_node_iter)->get_children();
              REQUIRE(const_cast<sl &>(small_children).get_size() == 0);
            }
          }
        }
      }
    }

    WHEN("Looking up words near 'henn' with threshold 2") {
      THEN("'hell' and 'help' are found") {
        auto words =
            tree.match(2, qs::string("henn"));
        REQUIRE(words.get_size() == 2);
        auto matched_words = words.get_data();
        REQUIRE(!std::strcmp(*(matched_words[0]), "hell"));
        REQUIRE(!std::strcmp(*(matched_words[1]), "help"));
      }
    }
  }
}
