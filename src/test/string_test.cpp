#include "catch_amalgamated.hpp"

#include <qs/string.h>

TEST_CASE("String concatenation") {
  qs::string s1((char *)"String 1");
  qs::string s2((char *)"String 2");

  auto s3 = s1 + s2;

  REQUIRE(s3.get_size() == 16);
}
