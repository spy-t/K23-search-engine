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

string::string(char *source) { string((const char *)source); }

string::string(const char *source) {
  size = std::strlen(source);
  str = new char[size + 1];
  is_alloced = true;
  std::memcpy(str, source, size);
  str[size] = '\0';
}

string::string(int num) {
  // Use the snprintf hack
  size = snprintf(0, 0, "%d", num);

  str = new char[size + 1];
  is_alloced = true;
  snprintf(str, size + 1, "%d", num);
}

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
    other.is_alloced = false;
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

char string::operator[](std::size_t index) {
  if (this->size == 0) {
    throw std::runtime_error("Invalid index. String is empty");
  }
  if (index >= this->size) {
    throw std::runtime_error("Invalid index. Buffer overflow");
  }

  return this->str[index];
}

char *string::operator*() { return this->str; }

bool operator==(const string &first, const string &second) {
  return first.size == second.size &&
         std::memcmp(first.str, second.str, first.size) == 0;
}

bool operator==(const string &first, const char *second) {
  return first.size == std::strlen(second) &&
         std::memcmp(first.str, second, first.size) == 0;
}

std::ostream &operator<<(std::ostream &out, const string &str) {
  out << str.str;

  return out;
}

std::size_t string::get_size() const { return this->size; }

const char *string::get_buffer() const { return this->str; }

} // namespace qs
