#include "catch_amalgamated.hpp"

#include <qs/hash_set.h>

TEST_CASE("Hash set insertion and removal") {
  GIVEN("A hash set and a key (qs::string)") {
    qs::hash_set set;
    qs::string key("key");

    WHEN("We insert the key") {
      set.insert(key);
      THEN("The key must be inside the hash set") {
        REQUIRE(true == set.contains(key));
      }
    }
    WHEN("We insert the key and then remove it.") {
      set.insert(key);
      set.remove(key);
      THEN("The key must not be inside the hash set") {
        REQUIRE(false == set.contains(key));
      }
    }
  }
}

TEST_CASE("Hash set retrieve all keys") {
  GIVEN("A hash set ") {
    qs::hash_set set;
    const int maxi = 10;
    qs::vector<qs::string> arr;
    WHEN("We insert some keys(qs::string)") {
      for (int i = 0; i < maxi; ++i) {
        qs::string key("key");
        auto skey = key.cat(qs::string(i));
        set.insert(skey);
        arr.push(skey);
      }
      THEN("We should be able to retrieve all the keys") {
        auto keys = set.get_all();
        arr.for_each([&keys](const qs::string &val) {
          bool found = false;
          keys.for_each([&val, &found](const qs::string &skey) {
            if (val == skey) {
              found = true;
            }
          });
          REQUIRE(true == found);
        });
      }
    }
  }
}