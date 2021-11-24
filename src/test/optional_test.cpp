#include "catch_amalgamated.hpp"
#include <qs/memory.hpp>
#include <qs/optional.hpp>

struct trivial {
  int x;
  explicit trivial(int x) : x(x) {}
};

struct non_trivial {
  int x;
  int copies = 0;
  explicit non_trivial(int x) : x(x) {}
  non_trivial(const non_trivial &other)
      : x(other.x), copies(other.copies + 1) {}
  non_trivial &operator=(const non_trivial &other) {
    this->x = other.x;
    this->copies = other.copies + 1;

    return *this;
  }
};

TEST_CASE("optional behaves correctly", "[optional]") {
  SECTION("for primitives") {
    qs::optional<int> opt(5);
    qs::optional<int> nullopt;

    REQUIRE(opt.get() == 5);
    REQUIRE(nullopt.get_or(10) == 10);
    REQUIRE_THROWS(nullopt.get());

    auto opt_copy = opt;
    REQUIRE(opt_copy.get() == 5);
    auto opt_moved = std::move(opt);
    REQUIRE(opt_moved.get() == 5);
    REQUIRE(opt.is_empty());

    REQUIRE(opt_moved.get_or(25) == 5);
  }

  SECTION("for trivially copyable types") {
    qs::optional<trivial> opt(5);
    qs::optional<trivial> nullopt;

    REQUIRE(opt.get().x == 5);
    REQUIRE(nullopt.get_or(trivial(10)).x == 10);
    REQUIRE_THROWS(nullopt.get());

    auto opt_copy = opt;
    REQUIRE(opt_copy.get().x == 5);
    auto opt_moved = std::move(opt);
    REQUIRE(opt_moved.get().x == 5);
    REQUIRE(opt.is_empty());

    REQUIRE(opt_moved.get_or(trivial(25)).x == 5);
  }

  SECTION("for non trivially copyable types") {
    qs::optional<non_trivial> opt(5);
    qs::optional<non_trivial> nullopt;

    REQUIRE(opt.get().x == 5);
    REQUIRE(nullopt.get_or(non_trivial(10)).x == 10);
    REQUIRE_THROWS(nullopt.get());

    auto opt_copy = opt;
    REQUIRE(opt_copy.get().x == 5);
    auto opt_moved = std::move(opt);
    REQUIRE(opt_moved.get().x == 5);
    REQUIRE(opt.is_empty());

    REQUIRE(opt_moved.get_or(non_trivial(25)).x == 5);
  }

  SECTION("for non copyable types") {
    qs::optional<qs::unique_pointer<trivial>> opt(qs::make_unique<trivial>(5));
    qs::optional<qs::unique_pointer<trivial>> nullopt;

    REQUIRE(opt.get()->x == 5);
    REQUIRE(nullopt.get_or(qs::make_unique<trivial>(10))->x == 10);
    REQUIRE_THROWS(nullopt.get());

    // No copies here. unique_pointer does not have copy semantics so we can not
    // copy the optional willy nilly
    auto opt_moved = std::move(opt);
    REQUIRE(opt_moved.get()->x == 5);
    REQUIRE(opt.is_empty());

    REQUIRE(opt_moved.get_or(qs::make_unique<trivial>(25))->x == 5);

    auto ptr_moved = std::move(opt_moved.get());
    REQUIRE(ptr_moved->x == 5);
  }

  SECTION("for pointers") {
    int *ptr = new int(5);
    qs::optional<int *> opt(ptr);
    qs::optional<int *> nullopt;

    REQUIRE(*opt.get() == 5);
    REQUIRE_THROWS(nullopt.get());

    auto opt_copy = opt;
    REQUIRE(*opt_copy.get() == 5);
    auto opt_moved = std::move(opt);
    REQUIRE(*opt_moved.get() == 5);
    REQUIRE(opt.is_empty());

    REQUIRE(*opt_moved.get_or(nullptr) == 5);

    delete ptr;
  }
}
