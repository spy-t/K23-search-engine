#include "catch_amalgamated.hpp"
#include <qs/distances.hpp>
#include <qs/string.h>

TEST_CASE("Hamming distance", "[distances]") {
  SECTION("Two strings: 'hell' and 'felt'") {
    REQUIRE(qs::hamming_distance(qs::string("hell"), qs::string("felt")) == 2);
  };

  SECTION("Two strings: 'when' and 'whenever'") {
    REQUIRE_THROWS(
        qs::hamming_distance(qs::string("when"), qs::string("whenever")));
  }
}

TEST_CASE("Edit distance", "[distances]") {
  SECTION("'hell' and 'felt'") {
    REQUIRE(qs::fast_distance(qs::string("hell"), qs::string("felt")) == 2);
  };

  SECTION("'ac' and 'abc'") {
    REQUIRE(qs::edit_distance(qs::string("ac"), qs::string("abc")) == 1);
  };

  SECTION("'abc' and 'bc'") {
    REQUIRE(qs::edit_distance(qs::string("abc"), qs::string("bc")) == 1);
  }

  SECTION("'hell' and 'help'") {
    REQUIRE(qs::edit_distance(qs::string("hell"), qs::string("help")) == 1);
  }

  SECTION("'help' and 'troop'") {
    REQUIRE(qs::edit_distance(qs::string("help"), qs::string("troop")) == 4);
  }
}
