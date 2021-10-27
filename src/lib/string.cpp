#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <qs/error.h>
#include <qs/functions.hpp>
#include <qs/string.h>
#include <stdexcept>
#include <utility>

namespace qs {

string::string() : str(nullptr), size(0), is_alloced(false) {}
string::string(std::size_t cap) : str(nullptr), size(cap) {
  str = new char[cap + 1];
  str[cap] = '\0';
  is_alloced = true;
}
string::string(char *source)
    : str(source), size(std::strlen(source)), is_alloced(false) {}

string::string(const string &other)
    : str(nullptr), size(other.size), is_alloced(true) {
  str = new char[other.size + 1];
  std::memcpy(str, other.str, other.size + 1);
}

string::string(string &&other) { *this = std::move(other); }

string &string::operator=(const string &other) {
  if (this != &other) {
    this->str = new char[other.size + 1];
    std::memcpy(this->str, other.str, other.size + 1);
    this->size = other.size;
    this->is_alloced = true;
  }
  return *this;
}

string &string::operator=(string &&other) {
  if (this != &other) {
    this->str = other.str;
    this->size = other.size;
    this->is_alloced = other.is_alloced;
    other.str = nullptr;
    other.size = 0;
  }
  return *this;
}

string::~string() {
  if (is_alloced) {
    delete[] str;
  }
}

string string::operator+(const string &other) {
  string s(this->size + other.size);
  std::memcpy(s.str, this->str, this->size);
  std::memcpy(s.str + this->size, other.str, other.size);

  return s;
}

std::ostream &operator<<(std::ostream &out, const string &str) {
  out << str.str;

  return out;
}

std::size_t string::get_size() const { return this->size; }

} // namespace qs
