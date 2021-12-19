#include "catch_amalgamated.hpp"

#include <qs/entry.hpp>
#include <qs/string_view.h>

TEST_CASE("Entry single-argument constructor works", "[entry]") {
  auto input = qs::string_view("teststring");
  WHEN("an entry is constructed using only a string_view and no payload") {
    auto en = qs::entry<int>(input);

    THEN("entry::get_string_view() returns the correct result") {
      REQUIRE(en.get_string_view() == input);
    }
  }
}