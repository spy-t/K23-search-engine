#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include <qs/functions.hpp>
#include <qs/hash_set.h>
#include <qs/parser.hpp>

TEST_CASE("Deduplication") {
  GIVEN("A file containing 10 strings with 5 of them being unique") {
    const char *filepath = "../src/test/resources/5unique_test.txt";
    FILE *f = std::fopen(filepath, "r");
    WHEN("We insert entries into the hash set") {
      qs::hash_set set;
      qs::parse_file(f, '\n',
                     [&set](const qs::string &entry) { set.insert(entry); });
      THEN("Hash set should contains only 5 entries") {
        auto entries = set.get_all();
        REQUIRE(entries.get_size() == 5);
      }
      std::fclose(f);
    }
  }
}
