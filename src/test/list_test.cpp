#include "catch_amalgamated.hpp"
#include <qs/list.hpp>

TEST_CASE("Linked list behaves as expected") {
  GIVEN("A linked list") {
    const int maxi = 10;
    qs::linked_list<int> l;
    for (int i = 0; i < maxi; ++i) {
      l.append(i);
    }
    REQUIRE(l.get_size() == maxi);

    WHEN("we append") {
      l.append(10);
      THEN("the value has been appended") {
        REQUIRE(l.get_size() == maxi + 1);
        REQUIRE(**l.tail == 10);
      }
    }
    WHEN("we remove") {
      l.remove(l.head);
      THEN("the node has been removed") {
        REQUIRE(l.get_size() == maxi - 1);
        REQUIRE(**l.head == 1);
      }
    }

    WHEN("we remove the third node") {
      l.remove(l.head->next()->next());
      THEN("the node has been removed") {
        REQUIRE(l.get_size() == maxi - 1);
        REQUIRE(l.head->next()->get() == 1);
        REQUIRE(l.head->next()->next()->get() == 3);
      }
    }

    WHEN("we append after") {
      l.append(-1, *l.head);
      THEN("the node has been appended after the given node") {
        REQUIRE(l.get_size() == maxi + 1);
        REQUIRE(**l.head->next() == -1);
      }
    }

    WHEN("we append at the end") {
      l.append(-1, *l.tail);
      THEN("-1 is found in the end of the list") {
        REQUIRE(l.get_size() == maxi + 1);
        REQUIRE(l.tail->get() == -1);
        REQUIRE(**l.tail->prev() == 9);
      }
    }

    WHEN("we prepend") {
      auto prev_head = l.head;
      l.prepend(-1, *l.head);
      THEN("the node has been prepended before the given node") {
        REQUIRE(l.get_size() == maxi + 1);
        REQUIRE(**l.head == -1);
        REQUIRE(l.head->next() == prev_head);
      }
    }
    WHEN("we prepend on the second node") {
      auto prev_node = l.head->next();
      l.prepend(-1, *prev_node);
      THEN("the node has been prepended after the head and before the second "
           "node") {
        REQUIRE(l.get_size() == maxi + 1);
        auto n = l.head;
        REQUIRE(n->get() == 0);
        n = n->next();
        REQUIRE(n->get() == -1);
        n = n->next();
        REQUIRE(n->get() == 1);
      }
    }
  }
}

TEST_CASE("Linked list iterator behaves as expected") {
  GIVEN("A linked list") {
    const int maxi = 10;
    qs::linked_list<int> l;
    for (int i = 0; i < maxi; i++) {
      l.append(i);
    }
    WHEN("We iterate forward with prefix iterator") {
      THEN("We find the expected values") {
        int i = 0;
        for (auto &n : l) {
          REQUIRE(i++ == n);
        }
        REQUIRE(i - 1 == maxi - 1);
      }
    }
    WHEN("We iterate forward with postfix iterator") {
      int i = 0;
      THEN("We find the expected values") {
        for (auto it = l.begin(), end = l.end(); it != end; it++) {
          REQUIRE(i++ == *it);
        }
        REQUIRE(i - 1 == maxi - 1);
      }
    }

    WHEN("We iterate backwards") {
      int i = 9;
      THEN("We find the expected values") {
        for (auto it = l.rbegin(), end = l.rend(); it != end; it++) {
          REQUIRE(i-- == *it);
        }
      }
    }
  }
}
