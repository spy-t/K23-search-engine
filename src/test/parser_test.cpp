#include "catch_amalgamated.hpp"
#include <cstdio>
#include <qs/parser.hpp>
#include <qs/string.h>
TEST_CASE("Parse file line by line") {
  SECTION("SMALL TEST") {
    const char *filepath = "../src/test/resources/small_test.txt";
    FILE *f = std::fopen(filepath, "r");
    qs::parse_file(f, '\n', [](const qs::string &entry) {
      REQUIRE(entry == "test_entry");
    });
    std::fclose(f);
  }

  SECTION("MEDIUM TEST") {
    const char *filepath = "../src/test/resources/med_test.txt";
    FILE *f = std::fopen(filepath, "r");
    qs::string test_v("entry");
    int index = 1;
    qs::parse_file(f, '\n', [&](const qs::string &entry) {
      REQUIRE(entry == test_v.cat(qs::string(index++)));
    });
    std::fclose(f);
  }
}