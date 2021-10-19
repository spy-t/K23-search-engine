#include <iostream>
#include <plain/vector.hpp>
#include <plain/fs.h>

int main() {
	pl::vector<int> v(400);

	v.push(1);

	std::cout << v.get_size() << "\n";
	std::cout << pl::get_basename("/src/basename") << "\n";
}
