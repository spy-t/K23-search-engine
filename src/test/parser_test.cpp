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

TEST_CASE("Parse string token by token") {
  SECTION("Small string") {
    const char *t = "First/";
    size_t length = strlen(t);
    char *str = new char[length + 1];
    std::memcpy(str, t, length + 1);
    qs::parse_string(str, "/", [&](const qs::string &entry) {
      REQUIRE(entry == qs::string("First"));
    });
    delete[] str;
  }
  SECTION("Medium string") {
    qs::string s("s");
    const char *t = "s1/s2/s3/s4/s5/s6/s7/s8/s9/s10";
    size_t length = strlen(t);
    char *str = new char[length + 1];
    std::memcpy(str, t, length + 1);
    int index = 1;
    qs::parse_string(str, "/", [&](const qs::string &entry) {
      REQUIRE(entry == s.cat(qs::string(index++)));
    });
    delete[] str;
  }
}