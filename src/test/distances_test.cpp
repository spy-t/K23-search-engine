#include "catch_amalgamated.hpp"

#include <qs/distances.hpp>
#include <qs/string.h>

TEST_CASE("Edit distance") {
  GIVEN("Two strings: 'hell' and 'felt'") {
    REQUIRE(qs::hamming_distance(qs::string("hell"), qs::string("felt")) == 2);
  };

  GIVEN("Two strings: 'when' and 'whenever'") {
    REQUIRE(qs::hamming_distance(qs::string("when"), qs::string("whenever")) ==
            4);
  }
}
