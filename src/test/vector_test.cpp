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
        bool right_vector = false;
        for (int i = 0; i < 7; i++) {
          right_vector = v[i] == i;
        }
        REQUIRE(right_vector);
      }
    }
  }
}
