#include "catch_amalgamated.hpp"

#include <qs/memory.hpp>
#include <qs/vector.hpp>
#include <utility>

struct obj {
  int x, y;
  explicit obj(int x, int y) : x(x), y(y) {}
  bool operator==(const obj &other) const {
    return this->x == other.x && this->y == other.y;
  }
};

template <typename T, typename C>
qs::vector<T> construct_vector(std::size_t number_of_elements, C constructor) {
  auto v = qs::vector<T>();
  for (std::size_t i = 0; i < number_of_elements; ++i) {
    v.push(constructor(i));
  }

  return v;
}

int construct_int(std::size_t i) { return (int)i; }
obj construct_obj(std::size_t i) { return obj(i, i); }
qs::unique_pointer<obj> construct_pointer_obj(std::size_t i) {
  return qs::make_unique<obj>(i, i);
}

TEST_CASE("vector push behaves correctly") {
  std::size_t size = 5;

  SECTION("vector of primitives") {
    auto v = construct_vector<int>(size, construct_int);

    REQUIRE(v.get_size() == size);
    for (std::size_t i = 0; i < size; ++i) {
      REQUIRE(v[i] == (int)i);
    }
  }

  SECTION("a vector of owned objects") {

    auto v = construct_vector<obj>(size, construct_obj);

    for (std::size_t i = 0; i < size; ++i) {
      obj o(i, i);
      REQUIRE(v[i] == o);
    }
  }

  SECTION("a vector of unique pointers") {
    auto v =
        construct_vector<qs::unique_pointer<obj>>(size, construct_pointer_obj);

    for (std::size_t i = 0; i < size; ++i) {
      obj o(i, i);
      REQUIRE(*v[i] == o);
    }
  }
}

TEST_CASE("vector resizing behaves correctly") {
  std::size_t initial_size = 5;
  std::size_t size_that_triggers_resize = 5;

  SECTION("vector of primitives") {
    auto v = construct_vector<int>(initial_size, construct_int);

    for (std::size_t i = initial_size;
         i < initial_size + size_that_triggers_resize; ++i) {
      v.push(construct_int(i));
    }

    REQUIRE(v.get_size() == initial_size + size_that_triggers_resize);
    for (std::size_t i = 0; i < initial_size + size_that_triggers_resize; ++i) {
      REQUIRE(v[i] == (int)i);
    }
  }

  SECTION("a vector of owned objects") {

    auto v = construct_vector<obj>(initial_size, construct_obj);

    for (std::size_t i = initial_size;
         i < initial_size + size_that_triggers_resize; ++i) {
      obj o(i, i);
      v.push(o);
    }
    REQUIRE(v.get_size() == initial_size + size_that_triggers_resize);

    for (std::size_t i = 0; i < initial_size + size_that_triggers_resize; ++i) {
      obj o(i, i);
      REQUIRE(v[i] == o);
    }
  }

  SECTION("a vector of unique pointers") {

    auto v = construct_vector<qs::unique_pointer<obj>>(initial_size,
                                                       construct_pointer_obj);

    for (std::size_t i = initial_size;
         i < initial_size + size_that_triggers_resize; ++i) {
      auto o = construct_pointer_obj(i);
      v.push(std::move(o));
    }
    REQUIRE(v.get_size() == initial_size + size_that_triggers_resize);

    for (std::size_t i = 0; i < initial_size + size_that_triggers_resize; ++i) {
      obj o(i, i);
      REQUIRE(*v[i] == o);
    }
  }
}
