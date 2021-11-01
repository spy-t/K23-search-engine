#include "catch_amalgamated.hpp"

#include <qs/hash_set.h>

TEST_CASE("Hash insertion") {
  GIVEN("A hash set and a key (qs::string)") {
    qs::hash_set set;
    qs::string key("key");

    WHEN("We insert the key") {
      set.insert(key);
      THEN("The key must be inside the hash set") {
        REQUIRE(true == set.contains(key));
      }
    }
  }
}
