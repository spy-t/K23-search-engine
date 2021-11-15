#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include <cstring>
#include <qs/sstream.h>

TEST_CASE("Components can be appended to a stringstream") {
  qs::stringstream ss;
  SECTION("appending qs::string") {
    qs::string s("A string");
    ss << s;
    auto result = ss.str();

    REQUIRE(result == s);
  }

  SECTION("appending const char *") {
    ss << "this is a static string";

    auto result = ss.str();

    REQUIRE(std::strcmp(*result, "this is a static string") == 0);
  }

  SECTION("appending an int") {
    ss << 5;

    auto result = ss.str();

    REQUIRE(**result == '5');
  }

  SECTION("appending multiple components") {
    qs::string s("a component");
    ss << 5 << " " << s;

    auto result = ss.str();

    REQUIRE(std::strcmp(*result, "5 a component") == 0);
  }
}
