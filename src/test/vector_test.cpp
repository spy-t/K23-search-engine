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

qs::vector<int> f(qs::vector<int> v) { return v; }

TEST_CASE("vector push behaves correctly") {
  std::size_t size = 5;

  SECTION("vector of primitives") {
    auto v = construct_vector<int>(size, construct_int);

    REQUIRE(v.get_size() == size);
    for (std::size_t i = 0; i < size; ++i) {
      REQUIRE(v.at(i) == (int)i);
    }
    v.set(0, 10);
    REQUIRE(v.at(0) == 10);
  }

  SECTION("a vector of owned objects") {

    auto v = construct_vector<obj>(size, construct_obj);

    for (std::size_t i = 0; i < size; ++i) {
      obj o(i, i);
      REQUIRE(v.at(i) == o);
    }

    obj o(10, 10);
    v.set(0, o);
    REQUIRE(v.at(0) == o);
  }

  SECTION("a vector of unique pointers") {
    auto v =
        construct_vector<qs::unique_pointer<obj>>(size, construct_pointer_obj);

    for (std::size_t i = 0; i < size; ++i) {
      obj o(i, i);
      REQUIRE(*v.at(i) == o);
    }
    auto o = construct_pointer_obj(10);
    v.set(0, std::move(o));
    REQUIRE(*v.at(0) == obj(10, 10));
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
      REQUIRE(v.at(i) == (int)i);
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
      REQUIRE(v.at(i) == o);
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
      REQUIRE(*v.at(i) == o);
    }
  }
}

TEST_CASE("vector runtime excpetions behave correctly") {
  auto v = construct_vector<int>(5, construct_int);

  SECTION("out of bounds set") { REQUIRE_THROWS(v.set(100, 10)); }
  SECTION("out of bounds dereference") { REQUIRE_THROWS(v.at(6)); }
}

TEST_CASE("vector copying and moving behaves correctly") {
  auto v = construct_vector<int>(5, construct_int);
  SECTION("assignment") {
    SECTION("copy") {
      auto v_copy = v.operator=(v);
      REQUIRE(v.get_size() == v_copy.get_size());
      for (std::size_t i = 0; i < v.get_size(); ++i) {
        REQUIRE(v[i] == v_copy[i]);
      }
    }

    SECTION("move") {
      qs::vector<int> v_moved;
      v_moved = std::move(v);
      REQUIRE(v.get_size() == 0);
      REQUIRE(v_moved.get_size() == 5);
      REQUIRE(v.get_data() == nullptr);
    }
  }

  SECTION("construction") {
    SECTION("copy") {
      qs::vector<int> v_copy(v);
      REQUIRE(v.get_size() == v_copy.get_size());
      for (std::size_t i = 0; i < v.get_size(); ++i) {
        REQUIRE(v[i] == v_copy[i]);
      }
    }

    SECTION("move") {
      qs::vector<int> v_moved(
          std::move(construct_vector<int>(5, construct_int)));
      REQUIRE(v_moved.get_size() == 5);
    }
  }
}
