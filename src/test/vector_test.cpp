#include "catch_amalgamated.hpp"

#include <qs/vector.hpp>
#include <utility>

SCENARIO("Vector adds items in place") {
  GIVEN("A sorted vector with all integers in range [0, 3)UNION(0, 6]") {
    auto v = qs::vector<int>();
    for (int i = 0; i < 3; i++) {
      v.push(i);
    }
    for (int i = 4; i < 7; i++) {
      v.push(i);
    }

    REQUIRE(v.get_size() == 3 + 3);

    WHEN("Inserting 3 in place with index == 3") {
      v.insert_in_place(3, 3);

      THEN("The vector contains the ordered set of all integers in [0, 6]") {
        bool right_vector = true;
        int i = 0;
        while (i < int(v.get_size()) && right_vector) {
          right_vector = v[i] == i;
          i++;
        }
        REQUIRE(right_vector);
        REQUIRE(v.get_size() == 7);
      }
    }
  }

  GIVEN("A vector with all integers in range [0, 6]") {
    auto v = qs::vector<int>();
    for (int i = 0; i < 7; i++) {
      v.push(i);
    }

    WHEN("Inserting 7 in place with index == 7") {
      v.insert_in_place(7, 7);
      REQUIRE(v.get_size() == 8);

      THEN("The vector contains the ordered set of all integers in [0, 7]") {
        bool right_vector = true;
        int i = 0;
        while (i < int(v.get_size()) && right_vector) {
          right_vector = v[i] == i;
          i++;
        }
        REQUIRE(right_vector);
      }
    }
  }

  GIVEN("A vector with all integers in range [1, 6]") {
    auto v = qs::vector<int>();
    for (int i = 1; i < 7; i++) {
      v.push(i);
    }

    WHEN("Inserting 0 in place with index == 0") {
      v.insert_in_place(0, 0);
      REQUIRE(v.get_size() == 7);

      THEN("The vector contains the ordered set of all integers in [0, 6]") {
        bool right_vector = true;
        int i = 0;
        while (i < int(v.get_size()) && right_vector) {
          right_vector = v[i] == i;
          i++;
        }
        REQUIRE(right_vector);
      }
    }
  }

  GIVEN("A vector with all integers in range [0, 1)UNION(1,6]") {
    auto v = qs::vector<int>();
    for (int i = 0; i < 1; i++) {
      v.push(i);
    }
    for (int i = 2; i < 7; i++) {
      v.push(i);
    }

    WHEN("Inserting 1 in place with index == 1") {
      v.insert_in_place(1, 1);
      REQUIRE(v.get_size() == 7);

      THEN("The vector contains the ordered set of all integers in [0, 6]") {
        bool right_vector = true;
        int i = 0;
        while (i < int(v.get_size()) && right_vector) {
          right_vector = v[i] == i;
          i++;
        }
        REQUIRE(right_vector);
      }
    }
  }
}
