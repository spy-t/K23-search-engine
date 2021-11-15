#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include <qs/functions/ops.hpp>
#include <qs/vector.hpp>

TEST_CASE("qs::functions::find") {
  SECTION("Nothing found") {
    auto v = qs::vector<const char *>();
    v.push("word1");
    v.push("word2");
    auto result = qs::functions::find(v.begin(), v.end(), "word3");

    REQUIRE(*result == *(v.end()));
  }
}
