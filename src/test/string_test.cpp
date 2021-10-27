#include "catch_amalgamated.hpp"

#include <qs/string.h>
#include <utility>

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

SCENARIO("String copy & move semantics work") {
  GIVEN("A string") {
    qs::string s1((char *)"String 1");
    auto size = s1.get_size();
    WHEN("It is assigned to another variable") {
      auto s2 = s1;

      THEN("They are the same string") { REQUIRE(s1 == s2); }
    }

    WHEN("It is move to another variable") {
      qs::string s2(std::move(s1));

      REQUIRE(s2.get_size() == size);

      THEN("The old variable is no longer valid") {
        REQUIRE(s1.get_size() == 0);
      }
    }
  }
}
