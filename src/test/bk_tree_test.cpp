#include "catch_amalgamated.hpp"

#include <qs/bk_tree.hpp>
#include <qs/distances.hpp>
#include <qs/list.hpp>
#include <qs/string.h>
#include <utility>

TEST_CASE("BK-Tree construction completes successfully") {

  GIVEN("A linked list of strings: [hell, help, fall, felt, fell, small]") {
    auto strings = new qs::linked_list<qs::string>();
    strings->append(qs::string("hell"));
    strings->append(qs::string("help"));
    strings->append(qs::string("fall"));
    strings->append(qs::string("felt"));
    strings->append(qs::string("fell"));
    strings->append(qs::string("small"));

    qs::bk_tree<qs::string> tree =
        qs::bk_tree<qs::string>(qs::edit_distance, strings);
  }
}
