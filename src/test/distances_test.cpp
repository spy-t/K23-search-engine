#include "catch_amalgamated.hpp"

#include <qs/distances.hpp>
#include <qs/string.h>

TEST_CASE("Hamming distance") {
  SECTION("Two strings: 'hell' and 'felt'") {
    REQUIRE(qs::hamming_distance(qs::string("hell"), qs::string("felt")) == 2);
  };

  SECTION("Two strings: 'when' and 'whenever'") {
    REQUIRE(qs::hamming_distance(qs::string("when"), qs::string("whenever")) ==
            4);
  }
}

TEST_CASE("Edit distance") {
  SECTION("Two strings: 'hell' and 'felt'") {
    REQUIRE(qs::edit_distance(qs::string("hell"), qs::string("felt")) == 2);
  };

  SECTION("Two strings: 'ac' and 'abc'") {
    REQUIRE(qs::edit_distance(qs::string("ac"), qs::string("abc")) == 2);
  };

  SECTION("Two strings: 'abc' and 'bc'") {
    REQUIRE(qs::edit_distance(qs::string("abc"), qs::string("bc")) == 1);
  }
}
