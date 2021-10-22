#include <iostream>
#include <qs/optional.hpp>

int main() {
  qs::optional<int> opt(5);

  qs::optional<int> nullopt;

  std::cout << "Primitive optional with value: " << opt.get_or(0) << "\n";
  std::cout << "Primitive optional without value: " << nullopt.is_empty()
            << "\n";

  int *int_ptr = new int(5);
  qs::optional<int *> ptr_opt(int_ptr);
  qs::optional<int *> ptr_nullopt;

  std::cout << "Pointer optional with value: " << *ptr_opt.get_or(0) << "\n";
  std::cout << "Pointer optional without value: " << ptr_nullopt.is_empty()
            << "\n";
}
