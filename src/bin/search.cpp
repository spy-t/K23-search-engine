#include <iostream>
#include <qs/optional.hpp>

int main() {
  qs::optional<int> opt(5);

  qs::optional<int> nullopt;

  std::cout << "Primitive optional with value: " << opt.get() << "\n";
  std::cout << "Primitive optional without value: " << nullopt.get_or(9999)
            << "\n";

  int *int_ptr = new int(5);
  qs::optional<int *> ptr_opt(int_ptr);
  qs::optional<int *> ptr_nullopt;

  std::cout << "Pointer optional with value: " << *ptr_opt.get() << "\n";
  std::cout << "Pointer optional without value: " << *ptr_nullopt.get_or(9999)
            << "\n";
}
