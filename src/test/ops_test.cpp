#include "catch_amalgamated.hpp"

#include <qs/functions/ops.hpp>
#include <qs/vector.hpp>

TEST_CASE("qs::functions::find", "[ops]") {
  SECTION("Nothing found") {
    auto v = qs::vector<const char *>();
    v.push("word1");
    v.push("word2");
    auto result = qs::functions::find(v.begin(), v.end(), "word3");

    REQUIRE(*result == *(v.end()));
  }
}

TEST_CASE("qs::functions::find_if", "[ops]") {
  SECTION("Nothing found") {
    auto v = qs::vector<const char>();
    v.push('a');
    v.push('b');

    auto result = qs::functions::find_if(v.begin(), v.end(),
                                         [](const char c) { return c == 'c'; });
    REQUIRE(result == v.end());
  }
}

static int count = 0;
struct TestStruct {
  int value;
  TestStruct() {
    value = 1;
    count++;
  }
  TestStruct(const TestStruct &other) {
    value = other.value;
    count++;
    if (count > 2) {
      throw std::runtime_error("Count gt 2");
    }
  }
};

TEST_CASE(
    "qs::functions::copy_uninitialized does not leak when an allocation fails",
    "[ops]") {
  auto v1 = qs::vector<TestStruct>();
  v1.push(TestStruct());
  count = 0;
  v1.push(TestStruct());
  count = 0;
  v1.push(TestStruct());
  count = 0;
  auto v2 = qs::vector<TestStruct>();
  REQUIRE_THROWS(
      qs::functions::copy_uninitialized(v1.begin(), v1.end(), v2.begin()));
}
