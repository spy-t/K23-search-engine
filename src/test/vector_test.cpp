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
    v.set(v.get_size(), -1);
    REQUIRE(v.at(v.get_size() - 1) == -1);
    int x = -2;
    v.set(v.get_size(), x);
    REQUIRE(v.at(v.get_size() - 1) == -2);
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

    v.set(v.get_size(), obj(-1, -1));
    REQUIRE(v.at(v.get_size() - 1) == obj(-1, -1));
    obj o2(-2, -2);
    v.set(v.get_size(), o2);
    REQUIRE(v.at(v.get_size() - 1) == obj(-2, -2));
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
    v.set(v.get_size(), construct_pointer_obj(-1));
    REQUIRE(*v.at(v.get_size() - 1) == obj(-1, -1));
    // no copy is allowed on unique_ptr
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

TEST_CASE("vector runtime exceptions behave correctly") {
  auto v = construct_vector<int>(5, construct_int);

  SECTION("out of bounds set (rvalue)") { REQUIRE_THROWS(v.set(100, 10)); }
  SECTION("out of bounds set (reference)") { auto x = 10; REQUIRE_THROWS(v.set(100, x)); }
  SECTION("out of bounds dereference") { REQUIRE_THROWS(v.at(6)); }
}

TEST_CASE("vector copying and moving behaves correctly") {
  auto v = construct_vector<int>(5, construct_int);
  SECTION("assignment") {
    SECTION("copy") {
      auto v_copy = v;
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

TEST_CASE("vector iterator behaves correctly") {
  auto v1 = construct_vector<int>(5, construct_int);
  auto v2 = construct_vector<obj>(5, construct_obj);
  auto v3 = construct_vector<qs::unique_pointer<obj>>(5, construct_pointer_obj);
  SECTION("range loop forward") {
    int n1 = 0;
    for (auto n : v1) {
      REQUIRE(n1++ == n);
    }

    int n2 = 0;
    for (auto n : v2) {
      REQUIRE(obj(n2, n2) == n);
      n2++;
    }

    int n3 = 0;
    for (auto &n : v3) {
      REQUIRE(obj(n3, n3) == *n);
      n3++;
    }
  }

  SECTION("conventional loop forward") {
    int n1 = 0;
    for (auto begin = v1.begin(), end = v1.end(); begin != end; begin++) {
      REQUIRE(n1++ == *begin);
    }

    int n2 = 0;
    for (auto begin = v2.begin(), end = v2.end(); begin != end; begin++) {
      REQUIRE(obj(n2, n2) == *begin);
      n2++;
    }

    int n3 = 0;
    for (auto begin = v3.begin(), end = v3.end(); begin != end; begin++) {
      REQUIRE(obj(n3, n3) == **begin);
      n3++;
    }
  }

  SECTION("conventional loop backwards") {
    int n1 = 4;
    for (auto rbegin = v1.rbegin(), rend = v1.rend(); rbegin != rend;
         rbegin++) {
      REQUIRE(*rbegin == n1--);
    }

    int n2 = 4;
    for (auto rbegin = v2.rbegin(), rend = v2.rend(); rbegin != rend;
         rbegin++) {
      REQUIRE(*rbegin == obj(n2, n2));
      n2--;
    }

    int n3 = 4;
    for (auto rbegin = v3.rbegin(), rend = v3.rend(); rbegin != rend;
         rbegin++) {
      REQUIRE(**rbegin == obj(n3, n3));
      n3--;
    }
  }
}
