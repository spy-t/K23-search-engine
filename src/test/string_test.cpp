#include "catch_amalgamated.hpp"

#include <qs/string.h>

SCENARIO("String sizes work when creating and concatenating") {
  GIVEN("Two strings") {
    qs::string s1((char *)"String 1");
    qs::string s2((char *)"String 2");
    REQUIRE(s1.get_size() == 8);
    REQUIRE(s2.get_size() == 8);

    WHEN("They are concatenated") {
      auto s3 = s1 + s2;

      THEN("The resulting string has their summed up size") {
        REQUIRE(s3.get_size() == s1.get_size() + s2.get_size());
      }
    }
  }
}
