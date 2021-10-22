#include <iostream>
#include <qs/fs.h>
#include <qs/vector.hpp>

int main() {
  qs::vector<int> v(400);

  v.push(1);

  std::cout << v.get_size() << "\n";
  std::cout << qs::get_basename("/src/basename") << "\n";
}
