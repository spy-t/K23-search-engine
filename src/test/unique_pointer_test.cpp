#include "catch_amalgamated.hpp"

#include <qs/memory.hpp>

class int_wrapper {
private:
  int a;

public:
  int_wrapper(int a) : a(a) {}
  int get() { return a; }
};

TEST_CASE("unique pointer works and correctly deallocates memory") {
  qs::unique_pointer<int> intptr = qs::make_unique<int>(5);
  REQUIRE(*intptr == 5);

  auto other = std::move(intptr);

  REQUIRE(*other == 5);
  REQUIRE_THROWS(*intptr);

  SECTION("can call methods of a complex type") {
    qs::unique_pointer<int_wrapper> intwrapperptr =
        qs::make_unique<int_wrapper>(10);

    REQUIRE(intwrapperptr->get() == 10);
  }
}
