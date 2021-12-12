#include "catch_amalgamated.hpp"

#include <qs/functions.hpp>
#include <qs/hash_set.hpp>
#include <qs/parser.hpp>

TEST_CASE("Deduplication", "[deduplication]") {
  GIVEN("A file containing 10 strings with 5 of them being unique") {
    const char *filepath = "./src/test/resources/5unique_test.txt";
    FILE *f = std::fopen(filepath, "r");
    REQUIRE(f);
    WHEN("We insert entries into the hash set") {
      qs::hash_set<qs::string> set;
      qs::parse_file(f, '\n',
                     [&set](const qs::string &entry) { set.insert(entry); });
      THEN("Hash set should contains only 5 entries") {
        REQUIRE(set.get_size() == 5);
      }
      std::fclose(f);
    }
  }
}
