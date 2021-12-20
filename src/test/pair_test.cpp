#include "catch_amalgamated.hpp"

#include <qs/pair.hpp>
#include <qs/string.h>
#include <utility>

SCENARIO("Hashing a pair of qs::string", "[pair]") {
  GIVEN("Two strings pair with the same strings but in another order") {
    auto s1 = qs::string("first");
    auto s2 = qs::string("second");
    auto p1 = qs::pair<qs::string>(s1, s2);
    auto p2 = qs::pair<qs::string>(s2, s1);
    WHEN("We hash those 2 pairs") {
      auto h1 = std::hash<qs::pair<qs::string>>{}(p1);
      auto h2 = std::hash<qs::pair<qs::string>>{}(p2);
      THEN("The resulting hash value must be the same") { REQUIRE(h1 == h2); }
    }
  }
}