#include "catch_amalgamated.hpp"
#include <qs/distances.hpp>

TEST_CASE("Hamming distance", "[distances]") {
  SECTION("Two strings: 'hell' and 'felt'") {
    REQUIRE(qs::hamming_distance(qs::string_view("hell"),
                                 qs::string_view("felt")) == 2);
  };

  SECTION("Two strings: 'when' and 'whenever'") {
    REQUIRE_THROWS(qs::hamming_distance(qs::string_view("when"),
                                        qs::string_view("whenever")));
  }
  SECTION("'suasa' and 'alana'") {
    REQUIRE(qs::hamming_distance(qs::string_view("suasa"),
                                 qs::string_view("alana")) == 3);
  }
}

TEST_CASE("Edit distance", "[distances]") {
  SECTION("'hell' and 'felt'") {
    REQUIRE(qs::edit_distance(qs::string_view("hell"),
                              qs::string_view("felt")) == 2);
  };

  SECTION("'ac' and 'abc'") {
    REQUIRE(qs::edit_distance(qs::string_view("ac"), qs::string_view("abc")) ==
            1);
  };

  SECTION("'abc' and 'bc'") {
    REQUIRE(qs::edit_distance(qs::string_view("abc"), qs::string_view("bc")) ==
            1);
  }

  SECTION("'hell' and 'help'") {
    REQUIRE(qs::edit_distance(qs::string_view("hell"),
                              qs::string_view("help")) == 1);
  }

  SECTION("'help' and 'troop'") {
    REQUIRE(qs::edit_distance(qs::string_view("help"),
                              qs::string_view("troop")) == 4);
  }

  SECTION("'ahem' and 'mahemn'") {
    REQUIRE(qs::edit_distance(qs::string_view("ahem"),
                              qs::string_view("mahemn")) == 2);
  }
}
