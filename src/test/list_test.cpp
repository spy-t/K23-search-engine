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
    WHEN("We iterate forward 1 step we reverse back 1 step") {
      auto it = l.begin();
      it++;
      it--;
      THEN("We must see the  value of the first node of our list(0 in our "
           "case)") {
        REQUIRE(it->get() == 0);
      }
    }

    WHEN("We iterate forwards until the end") {
      auto it = l.begin();
      auto end = l.end();
      for (; it != end; ++it) {
      }
      WHEN("We go backwards multiple times") {
        it--;
        REQUIRE(it->get() == maxi - 1);
        it--;
        REQUIRE(it->get() == maxi - 2);

        THEN("We should be able to go forwards again") {
          it++;
          REQUIRE(it->get() == maxi - 1);
        }
      }

      WHEN("We iterate backwads until the first node") {
        for (int i = maxi; i > 0; --i) {
          it--;
        }
        THEN("We should be able to move forward again") {
          it++;
          REQUIRE(it->get() == 1);
        }
      }
    }
  }
}
