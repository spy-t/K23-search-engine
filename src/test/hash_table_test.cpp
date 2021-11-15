#define CATCH_CONFIG_MAIN
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

TEST_CASE("hash table behaves as expected") {
  SECTION("hash table of primitives") {
    qs::hash_table<int> ht;
    constexpr int max = 100;
    int vals[max] = {0};
    for (int i = 0; i < max; ++i) {
      ht.insert((const uint8_t *)qs::string(i).get_buffer(), i);
      vals[i] = i;
    }
    REQUIRE(ht.get_size() == max);

    for (int i = 0; i < max; ++i) {
      REQUIRE(**ht.lookup((const uint8_t *)qs::string(i).get_buffer()) == i);
    }

    ht.remove((const uint8_t *)"3");
    REQUIRE(ht.lookup((const uint8_t *)"3") == ht.end());

    for (auto &n : ht) {
      int remove_index = remove_value(vals, max, *n);
      REQUIRE(remove_index != -1);
    }
  }

  SECTION("hash table of smart pointers") {
    qs::hash_table<qs::unique_pointer<int>> ht;
    ht.insert((const uint8_t *)"1", qs::make_unique<int>(1));
    ht.insert((const uint8_t *)"2", qs::make_unique<int>(2));
    ht.insert((const uint8_t *)"3", qs::make_unique<int>(3));
    REQUIRE(ht.get_size() == 3);

    REQUIRE(***ht.lookup((const uint8_t *)"1") == 1);
    REQUIRE(***ht.lookup((const uint8_t *)"2") == 2);
    REQUIRE(***ht.lookup((const uint8_t *)"3") == 3);

    ht.remove((const uint8_t *)"3");
    REQUIRE(ht.lookup((const uint8_t *)"3") == ht.end());
  }

  SECTION("empty hash table behaviour") {
    auto ht = qs::hash_table<int>();
    REQUIRE(ht.get_size() == 0);
    REQUIRE(ht.begin() == ht.end());
    ht.remove((const uint8_t *)"non-existent key");
    REQUIRE(ht.get_size() == 0);
  }

  SECTION("hash table insert with the same key") {
    const auto *key = reinterpret_cast<const uint8_t *>("key1");
    int value = 1;
    auto ht = qs::hash_table<int>();
    ht.insert(key, value);
    REQUIRE(ht.get_size() == 1);
    REQUIRE((*ht.lookup(key)).get() == value);
    ht.insert(key, 2);
    REQUIRE(ht.get_size() == 1);
    REQUIRE((*ht.lookup(key)).get() == 1);
  }
}
