#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include <qs/functions.hpp>
#include <qs/skip_list.hpp>
#include <qs/string.h>

constexpr std::size_t n = 10;
qs::string data[n] = {
    qs::string("some"), qs::string("random"),  qs::string("strings"),
    qs::string("that"), qs::string("are"),     qs::string("used"),
    qs::string("for"),  qs::string("testing"), qs::string("purposes"),
    qs::string("."),
};

SCENARIO("qs::skip_list behaves as expected") {

  GIVEN("a qs::skip_list") {
    qs::skip_list<qs::string, 10> sl(
        [](const qs::string &a, const qs::string &b) {
          return std::strcmp(a.get_buffer(), b.get_buffer());
        });

    WHEN("we insert some strings") {
      for (std::size_t i = 0; i < n; ++i) {
        sl.insert(data[i]);
      }

      THEN("all the strings can be found in the list") {
        for (auto &f : sl) {
          REQUIRE(qs::functions::find(data, data + n, f) != data + n);
        }
      }

      THEN("we can copy the skip list and both have the same data") {
        auto other = sl;
        for (std::size_t i = 0; i < n; ++i) {
          auto f = sl.find(data[i]);
          auto fother = other.find(data[i]);
          REQUIRE(f != sl.end());
          REQUIRE(fother != other.end());
          REQUIRE(*f == data[i]);
          REQUIRE(*fother == data[i]);
        }
      }
    }

    WHEN("we insert 2 strings that are the same") {
      sl.insert(data[0]);
      sl.insert(data[0]);

      THEN("only one instance of this string is found") {
        REQUIRE(sl.get_size() == 1);
        REQUIRE(!std::strcmp(**(sl.begin()), *(data[0])));
        REQUIRE(++sl.begin() == sl.end());
      }
    }
  }
}
