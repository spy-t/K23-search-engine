#include "catch_amalgamated.hpp"

#include <qs/bk_tree.hpp>
#include <qs/distances.hpp>
#include <qs/functions.hpp>
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
    qs::vector<qs::string> word_vec(7);
    word_vec.push(qs::string("hell"));
    word_vec.push(qs::string("help"));
    word_vec.push(qs::string("fall"));
    word_vec.push(qs::string("felt"));
    word_vec.push(qs::string("fell"));
    word_vec.push(qs::string("smal"));
    word_vec.push(qs::string("melt"));
    auto tree = qs::bk_tree<qs::string>(word_vec, qs::hamming_distance);

    THEN("'hell' is the root") {
      auto r = tree.get_root();
      REQUIRE(!std::strcmp(*(r->get()), "hell"));

      THEN("'hell' has 3 children: 'help', 'fall' and 'smal'") {
        const char *children_strings[3] = {"help", "fall", "smal"};

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

            THEN("'smal' has no children") {
              auto smal_node_iter = ++fall_node_iter;
              auto &small_children = (*smal_node_iter)->get_children();
              REQUIRE(const_cast<sl &>(small_children).get_size() == 0);
            }
          }
        }
      }
    }

    WHEN("Looking up words near 'henn' with threshold 2") {
      THEN("'hell' and 'help' are found") {
        auto words = tree.match(2, qs::string("henn"));
        REQUIRE(words.get_size() == 2);
        auto hell =
            qs::functions::find(words.begin(), words.end(), qs::string("hell"));
        auto help =
            qs::functions::find(words.begin(), words.end(), qs::string("help"));
        REQUIRE((hell != words.end() && help != words.end()));
      }
    }
  }

  GIVEN("The strings: help, hell, hello, loop, helps, shell, helper, troop and "
        "using edit distance") {
    auto tree = qs::bk_tree<qs::string>(qs::edit_distance);
    tree.insert(qs::string("help"));
    tree.insert(qs::string("hell"));
    tree.insert(qs::string("hello"));
    tree.insert(qs::string("loop"));
    tree.insert(qs::string("helps"));
    tree.insert(qs::string("shell"));
    tree.insert(qs::string("helper"));
    tree.insert(qs::string("cult"));
    tree.insert(qs::string("troop"));
    tree.insert(qs::string("helped"));

    THEN("'help' is the root") {
      auto r = tree.get_root();
      REQUIRE(!std::strcmp(*(r->get()), "help"));

      THEN("'help' has 4 children: 'hell', 'hello', 'loop' and 'troop'") {
        const char *children_strings[4] = {"hell", "hello", "loop", "troop"};
        check_children(r, children_strings, 4);

        THEN("'hell' has 1 child: 'helps'") {
          auto &children = r->get_children();

          auto hell_node_iter = const_cast<sl &>(children).begin();
          const char *hell_children_strings[1] = {"helps"};
          check_children(*hell_node_iter, hell_children_strings, 1);

          THEN("'hello' has 2 children: 'shell' and 'helper'") {
            auto hello_node_iter = ++hell_node_iter;
            const char *hello_children_strings[2] = {"shell", "helper"};
            check_children(*hello_node_iter, hello_children_strings, 2);

            THEN("'loop' has 1 child: 'cult'") {
              auto loop_node_iter = ++hello_node_iter;
              const char *loop_children_strings[1] = {"cult"};
              check_children(*loop_node_iter, loop_children_strings, 1);

              THEN("'troop' has no children") {
                auto troop_node_iter = ++loop_node_iter;
                auto &troop_children =
                    troop_node_iter.curr->operator*()->get_children();
                REQUIRE(const_cast<sl &>(troop_children).begin() ==
                        const_cast<sl &>(troop_children).end());
              }
            }
          }
        }
      }
    }

    WHEN("Looking up words near 'helper' with threshold 0") {
      THEN("'helper' is found") {
        auto words = tree.match(0, qs::string("helper"));
        REQUIRE(words.get_size() == 1);
        auto helper = qs::functions::find(words.begin(), words.end(),
                                          qs::string("helper"));
        REQUIRE(helper != words.end());
      }
    }

    WHEN("Looking up words near 'poor' with threshold 3") {
      THEN("'loop' and 'troop' are found") {
        auto words = tree.match(3, qs::string("poor"));
        REQUIRE(words.get_size() == 2);
        auto loop =
            qs::functions::find(words.begin(), words.end(), qs::string("loop"));
        auto troop = qs::functions::find(words.begin(), words.end(),
                                         qs::string("troop"));
        REQUIRE((loop != words.end() && troop != words.end()));
      }
    }

    WHEN("Looking up words near 'helped' with threshold 0") {
      THEN("'helped' is found") {
        auto words = tree.match(0, qs::string("helped"));
        REQUIRE(words.get_size() == 1);
        auto helped = qs::functions::find(words.begin(), words.end(),
                                          qs::string("helped"));
        REQUIRE(helped != words.end());
      }
    }
  }

  GIVEN("No strings") {
    auto tree = qs::bk_tree<qs::string>(qs::hamming_distance);
    REQUIRE(tree.get_root() == nullptr);
    THEN("Matching returns nothing") {
      auto words = tree.match(0, qs::string("str"));
      REQUIRE(words.get_size() == 0);
    }
  }
}
