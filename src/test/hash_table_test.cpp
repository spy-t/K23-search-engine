#include "catch_amalgamated.hpp"

#include <qs/hash_table.hpp>
#include <qs/memory.hpp>
#include <qs/string.h>

int remove_value(int *a, std::size_t size, int what) {
  for (std::size_t i = 0; i < size; ++i) {
    if (a[i] == what) {
      a[i] = -1;
      return i;
    }
  }
  return -1;
}

TEST_CASE("open addressing hash table behaves as expected", "[hash_table]") {
  SECTION("open addressing hash table of primitives") {
    qs::hash_table<qs::string, int> ht;
    constexpr int max = 100;
    int vals[max] = {0};
    for (int i = 0; i < max; ++i) {
      ht.insert(qs::string(i), i);
      vals[i] = i;
    }
    REQUIRE(ht.get_size() == max);

    for (int i = 0; i < max; ++i) {
      REQUIRE(*ht.lookup(qs::string(i)) == i);
    }

    ht.remove(qs::string("3"));
    REQUIRE(ht.lookup(qs::string("3")) == ht.end());

    for (auto n = ht.begin(); n != ht.end(); ++n) {
      int remove_index = remove_value(vals, max, *n);
      REQUIRE(remove_index != -1);
    }
    /* for (auto &n : ht) { */
    /* } */
  }

  SECTION("open addressing hash table of smart pointers") {
    qs::hash_table<qs::string, qs::unique_pointer<int>> ht;
    ht.insert(qs::string("1"), qs::make_unique<int>(1));
    ht.insert(qs::string("2"), qs::make_unique<int>(2));
    ht.insert(qs::string("3"), qs::make_unique<int>(3));
    REQUIRE(ht.get_size() == 3);

    REQUIRE(**ht.lookup(qs::string("1")) == 1);
    REQUIRE(**ht.lookup(qs::string("2")) == 2);
    REQUIRE(**ht.lookup(qs::string("3")) == 3);
    REQUIRE(**ht.lookup(qs::string("3")) == 3);

    ht.remove(qs::string("3"));
    REQUIRE(ht.lookup(qs::string("3")) == ht.end());
  }

  SECTION("empty open addressing hash table behaviour") {
    auto ht = qs::hash_table<const char *, int>();
    REQUIRE(ht.get_size() == 0);
    REQUIRE(ht.begin() == ht.end());
    ht.remove("non-existent key");
    REQUIRE(ht.get_size() == 0);
  }

  SECTION("open addressing hash table insert with the same key") {
    auto key = qs::string("key1");
    int value = 1;
    auto ht = qs::hash_table<qs::string, int>();
    ht.insert(key, value);
    REQUIRE(ht.get_size() == 1);
    REQUIRE(*ht.lookup(key) == value);
    ht.insert(key, 2);
    REQUIRE(ht.get_size() == 1);
    REQUIRE(*ht.lookup(key) == 1);
  }
}
