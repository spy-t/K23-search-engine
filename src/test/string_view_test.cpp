#include "catch_amalgamated.hpp"

#include <qs/string_view.h>

TEST_CASE("string_view behaves as expected", "[string_view]") {
  qs::string_view s{"a string"};
  qs::string_view other{"a string"};
  REQUIRE(s.size() == 8);
  REQUIRE(s == "a string");
  REQUIRE(s == other);
  REQUIRE(s != "a different string");
  REQUIRE(s.substr(2, 5) == "stri");

  auto a = s.split(' ');
  REQUIRE(a == "a");
  REQUIRE(s == "string");
  REQUIRE(s.split(' ') == qs::string_view::empty);
}
