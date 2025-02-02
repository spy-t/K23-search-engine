#include "catch_amalgamated.hpp"

#include <qs/functions.hpp>
#include <qs/hash_set.hpp>

TEST_CASE("Hash set insertion and removal", "[hash_set]") {
  GIVEN("A hash set and a key (qs::string)") {
    qs::hash_set<qs::string> set;
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

TEST_CASE("Hash set retrieve all keys", "[hash_set]") {
  GIVEN("A hash set ") {
    qs::hash_set<qs::string> set;
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
        for (auto &v : arr) {
          bool found = false;
          for (auto &k : set) {
            if (v == k) {
              found = true;
            }
          }
          REQUIRE(found == true);
        }
      }
    }
  }
}
