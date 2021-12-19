#include "catch_amalgamated.hpp"

#include <functional>
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
  REQUIRE(s.split(' ') == "string");
  REQUIRE(s.split(' ') == qs::string_view::empty);

  qs::string_view s1{
      "http dbpedia resource sokol airport http dbpedia ontology abstract "
      "sokol airport arport sokol magadan oblast russia airport located north "
      "magadan city center irport sometimes confused with dolinsk sokol base "
      "which home fighters that shot down korean flight town gained exposure "
      "western world with inauguration alaska airlinef flights united states "
      "using mcdonnell douglas jets according anecdotal story published york "
      "times first alaska airliues flight needed deicing services which were "
      "unavailable flight crew acquired quantity vodka sprayed onto wings "
      "airline threatened discontinue russian service difficulties with "
      "contract workers alaska airlines flights into magadan elsewhere russia "
      "were halted october shortly after russian financial crisis which "
      "rendered routes unprofitable aeroflot suspended flights sokol airpfrt "
      "february planned removal from service aircraft aeroflot cited lack "
      "certification airpoirt acceptance servicing more modern aircraft such "
      "airbus airbus primary reason suspension flights resumed service march "
      "http wikipedia wiki sokol arport"};
  auto spl = s1.split(' ');
  REQUIRE(spl == "http");

  SECTION("complex split") {
    qs::string_view s{"a big separated"};
    auto a = s.split(' ');
    auto b = s.split(' ');
    auto c = s.split(' ');

    REQUIRE(a == "a");
    REQUIRE(b == "big");
    REQUIRE(c == "separated");
  }
}

TEST_CASE("String view hash works", "[string_view]") {
  qs::string_view sv{"edit"};
  qs::string s{"edit"};

  REQUIRE(std::hash<qs::string_view>{}(sv) == std::hash<qs::string>{}(s));
}

TEST_CASE("string_view vs string comparison works as expected",
          "[string_view]") {
  qs::string_view s1{"str1"};
  qs::string s2{"str11"};
  REQUIRE_FALSE(s1 == s2);
}

TEST_CASE("String view iteration works as expected", "[string_view]") {
  qs::string_view s{"string"};

  auto begin = s.begin();
  auto end = s.end();

  REQUIRE(*begin == 's');
  REQUIRE(*end == 0);
  begin++;
  REQUIRE(*begin == 't');
  end--;
  REQUIRE(*end == 'g');
}
