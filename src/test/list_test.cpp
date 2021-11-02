#include "catch_amalgamated.hpp"
#include "qs/list.hpp"

TEST_CASE("List iteration through iterator") {
  GIVEN("A list with some nodes") {
    const int maxi = 10;
    qs::linked_list<int> l;
    for (int i = 0; i < maxi; i++) {
      l.append(i);
    }
    WHEN("We iterate forward with prefix iterator") {
      THEN("We must see all the nodes with the right values") {
        int i = 0;
        for (auto n : l) {
          REQUIRE(i == n.get());
          i++;
        }
        REQUIRE(i - 1 == maxi - 1);
      }
    }
    WHEN("We iterate forward with postfix iterator") {
      int i = 0;
      THEN("We must see all the nodes with the right values") {
        for (auto it = l.begin(), end = l.end(); it != end; it++) {
          const auto tit = *it;
          REQUIRE(i == tit.get());
          i++;
        }
        REQUIRE(i - 1 == maxi - 1);
      }
    }
  }
}
