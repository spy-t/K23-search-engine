#include "catch_amalgamated.hpp"

#include <qs/crit_bit_tree.hpp>


TEST_CASE("Crit-bit tree insertion") {
  GIVEN("A crit bit tree and a key value pair"){
    qs::string key("key");
    qs::string value("value");
    qs::crit_bit_tree<qs::string> cbt;
    WHEN("A key value pair is inserted"){
      cbt.insert(key,value);
      THEN("They must be inside the tree"){
        auto result = cbt.get(key);
        REQUIRE(result.get() == value);
      }
    }
  }
}