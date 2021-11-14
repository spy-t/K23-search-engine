#include "catch_amalgamated.hpp"

#include <qs/string.h>
#include <utility>

SCENARIO("String concatenation works") {
  GIVEN("Two strings") {
    qs::string s1((char *)"String 1");
    qs::string s2((char *)"String 2");
    REQUIRE(s1.get_length() == 8);
    REQUIRE(s2.get_length() == 8);

    WHEN("They are purely concatenated") {
      auto s3 = s1.cat(s2);

      THEN("The resulting string is the concatenation") {
        REQUIRE(s3 == qs::string("String 1String 2"));
      }
    }

    WHEN("They are impurely concatenated") {
      s1 = s1 + s2;

      THEN("The resulting string is the concatenation") {
        REQUIRE(s1 == qs::string("String 1String 2"));
        REQUIRE(s1 != qs::string("String 1 String 2"));
      }
    }
  }

  GIVEN("A preallocated string") {
    auto s = qs::string::with_size(5);

    WHEN("It is impurely concatenated with the numbers from 0 to 4") {
      for (int i = 0; i < 5; ++i) {
        s = s + qs::string(i);
      }

      THEN("The resulting string is 01234 ") {
        REQUIRE(std::strcmp(*s, "01234") == 0);
      }
    }
  }
}

SCENARIO("String copy & move semantics work") {
  GIVEN("A string") {
    qs::string s1((char *)"String 1");
    auto size = s1.get_length();
    WHEN("It is assigned to another variable") {
      qs::string s2;
      s2 = s1;

      THEN("They are the same string") { REQUIRE(s1 == s2); }
    }

    WHEN("It is moved to another variable") {
      qs::string s2(std::move(s1));

      REQUIRE(s2.get_length() == size);

      THEN("The old variable is no longer valid") {
        REQUIRE(s1.get_length() == 0);
      }
    }

    WHEN("It is moved-assigned to another variable") {
      qs::string s2("irrelevant string");
      s2 = std::move(s1);

      REQUIRE(s2.get_length() == size);

      THEN("The old variable is no longer valid") {
        REQUIRE(s1.get_length() == 0);
      }
    }
  }
}

SCENARIO("String indexing works") {
  GIVEN("An empty string") {
    qs:: string s("");

    WHEN("It is dereferenced") {
      THEN("'String is empty' exception is thrown") {
        REQUIRE_THROWS_MATCHES(s[0], std::runtime_error, Catch::Matchers::Message("Invalid index. String is empty"));
      }
    }
  }
  GIVEN("A string") {
    qs::string s((char *)"String 1");

    WHEN("It is dereferenced using a valid index with checked dereference") {
      auto c = s[0];

      THEN("The character matches") { REQUIRE(c == 'S'); }
    }

    WHEN("It is dereferenced using an invalid index with checked dereference") {

      THEN("An exception is thrown") { REQUIRE_THROWS(s.operator[](9)); }
    }

    WHEN("It is dereferenced using a valid index with unchecked dereference") {
      auto c = *s;

      THEN("The character matches") { REQUIRE(*c == 'S'); }
    }
  }
}

SCENARIO("String comparisons work") {
  GIVEN("A string: 'String 1'") {
    qs::string s1("String 1");

    WHEN("It is compared with a different string: 'String 2'") {
      qs::string s2("String 2");

      THEN("They are not equal") {
        REQUIRE_FALSE(s1 == s2);
        REQUIRE(s1 != s2);
        REQUIRE_FALSE(s1 == "String 2");
        REQUIRE(s1 != "String 2");
      }

      THEN("The first string is lesser than the second") {
        REQUIRE(s1 < s2);
        REQUIRE_FALSE(s1 > s2);
        REQUIRE(s1 <= s2);
        REQUIRE_FALSE(s1 >= s2);
        REQUIRE(s1 < "String 2");
        REQUIRE(s1 <= "String2");
        REQUIRE_FALSE(s1 > "String 2");
        REQUIRE_FALSE(s1 >= "String 2");
      }
    }
  }
}
