#include "catch_amalgamated.hpp"

#include <qs/bk_tree.hpp>
#include <qs/distances.hpp>
#include <qs/list.hpp>
#include <qs/string.h>
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

TEST_CASE("BK-Tree construction completes without errors and inserts nodes "
          "correctly") {
  GIVEN(
      "A linked list of strings: [hell, help, fall, felt, fell, small, melt]") {
    auto strings = new qs::linked_list<qs::string>();
    strings->append(qs::string("hell"));
    strings->append(qs::string("help"));
    strings->append(qs::string("fall"));
    strings->append(qs::string("felt"));
    strings->append(qs::string("fell"));
    strings->append(qs::string("small"));
    strings->append(qs::string("melt"));

    auto tree = qs::bk_tree<qs::string>(qs::hamming_distance, strings);
    auto r = tree.get_root();
    REQUIRE(!std::strcmp(*(r->get()), "hell"));

    const char *children_strings[3] = {"help", "fall", "small"};

    check_children(r, children_strings, 3);
    auto &children = r->get_children();

    auto help_node_iter = const_cast<sl &>(children).begin();
    const char *help_children_strings[1] = {"fell"};
    check_children(*help_node_iter, help_children_strings, 1);

    auto fall_node_iter = ++help_node_iter;
    const char *fall_children_strings[2] = {"felt", "melt"};
    check_children(*fall_node_iter, fall_children_strings, 2);

    auto small_node_iter = ++fall_node_iter;
    auto &small_children = (*small_node_iter)->get_children();
    REQUIRE(const_cast<sl &>(small_children).get_size() == 0);

    delete strings;
  }
}
