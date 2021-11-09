#include "catch_amalgamated.hpp"
#include <qs/hash_table.hpp>
#include <qs/memory.hpp>
#include <qs/string.h>

TEST_CASE("hash table behaves as expected") {
  SECTION("hash table of primitives") {
    qs::hash_table<int> ht;
    const int max = 100;
    for (int i = 0; i < max; ++i) {
      ht.insert((const uint8_t *)qs::string(i).get_buffer(), i);
    }
    REQUIRE(ht.get_size() == max);

    for (int i = 0; i < max; ++i) {
      REQUIRE(ht.lookup((const uint8_t *)qs::string(i).get_buffer()).get() ==
              i);
    }

    ht.remove((const uint8_t *)"3");
    REQUIRE(ht.lookup((const uint8_t *)"3").is_empty());
  }

  // TODO(spyros): for the time being this does not work because
  // qs::unique_pointer does not support copy operations. We should investigate
  // an API similar to that of STL which uses iterators for insertion/find
  // operations (even though it sucks and I hate it...)

  //   SECTION("hash table of smart pointers") {
  //     qs::hash_table<qs::unique_pointer<int>> ht;
  //     ht.insert((const uint8_t *)"1", qs::make_unique<int>(1));
  //     ht.insert((const uint8_t *)"2", qs::make_unique<int>(2));
  //     ht.insert((const uint8_t *)"3", qs::make_unique<int>(3));
  //     REQUIRE(ht.get_size() == 3);
  //
  //     REQUIRE(*(ht.lookup((const uint8_t *)"1").get()) == 1);
  //     REQUIRE(*(ht.lookup((const uint8_t *)"2").get()) == 2);
  //     REQUIRE(*(ht.lookup((const uint8_t *)"3").get()) == 3);
  //
  //     ht.remove((const uint8_t *)"3");
  //     REQUIRE(ht.lookup((const uint8_t *)"3").is_empty());
  //   }
}
