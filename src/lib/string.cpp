#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <qs/error.h>
#include <qs/functions.hpp>
#include <qs/string.h>
#include <stdexcept>
#include <utility>

namespace qs {

string::string() : str(nullptr), capacity(0), length(0), is_alloced(false) {}

string string::with_size(std::size_t cap) {
  qs::string s;

  s.str = new char[cap + 1];
  s.str[0] = '\0';
  s.capacity = cap + 1;
  s.is_alloced = true;
  s.length = 0;

  return s;
}

string string::from_allocated_string(char *str) {
  qs::string s;

  s.str = str;
  s.length = strlen(str);
  s.capacity = s.length;
  s.is_alloced = true;

  return s;
}

string::string(char *source) : string((const char *)source) {}

string::string(const char *source) : string(source, strlen(source)) {}

string::string(const char *source, size_t length) {
  this->length = length;
  str = new char[length + 1];
  is_alloced = true;
  std::memcpy(str, source, length);
  str[length] = '\0';
  capacity = length + 1;
}
string::string(int num) {
  // Use the snprintf hack
  length = snprintf(0, 0, "%d", num);

  str = new char[length + 1];
  is_alloced = true;
  snprintf(str, length + 1, "%d", num);
  capacity = length + 1;
}

string::string(const string &other)
    : str(nullptr), length(other.length), is_alloced(true) {
  str = new char[other.length + 1];
  std::memcpy(str, other.str, other.length + 1);
  capacity = other.length + 1;
}

string::string(string &&other) { *this = std::move(other); }

string &string::operator=(const string &other) {
  if (this != &other) {
    this->str = new char[other.length + 1];
    std::memcpy(this->str, other.str, other.length + 1);
    this->length = other.length;
    this->is_alloced = true;
    this->capacity = other.length + 1;
  }
  return *this;
}

string &string::operator=(string &&other) {
  if (this != &other) {
    this->str = other.str;
    this->length = other.length;
    this->capacity = other.capacity;
    this->is_alloced = other.is_alloced;
    other.str = nullptr;
    other.length = 0;
    other.capacity = 0;
    other.is_alloced = false;
  }
  return *this;
}

string::~string() {
  if (is_alloced) {
    delete[] str;
  }
}

string string::cat(const string &other) {
  auto s = string::with_size(this->length + other.length);
  std::memcpy(s.str, this->str, this->length);
  std::memcpy(s.str + this->length, other.str, other.length);
  s.length = this->length + other.length;
  s.str[s.length] = '\0';

  return s;
}

string &string::operator+(const string &other) {
  auto new_length = this->length + other.length;

  // No resizing needed
  if (this->capacity > new_length) {
    std::memcpy(this->str + this->length, other.str, other.length);
    this->str[new_length] = '\0';
  } else {
    auto new_str = new char[this->length + other.length + 1];
    std::memcpy(new_str, this->str, this->length);
    std::memcpy(new_str + this->length, other.str, other.length);
    if (this->is_alloced) {
      delete[] this->str;
    }
    this->str = new_str;
    this->is_alloced = true;
    this->capacity = this->length + other.length + 1;
  }
  this->length = new_length;

  return *this;
}

char string::operator[](std::size_t index) {
  if (this->length == 0) {
    throw std::runtime_error("Invalid index. String is empty");
  }
  if (index >= this->length) {
    throw std::runtime_error("Invalid index. Buffer overflow");
  }

  return this->str[index];
}

char *string::operator*() { return this->str; }

bool operator==(const string &first, const string &second) {
  return first.length == second.length &&
         std::memcmp(first.str, second.str, first.length) == 0;
}

bool operator==(const string &first, const char *second) {
  return first.length == std::strlen(second) &&
         std::memcmp(first.str, second, first.length) == 0;
}

bool operator!=(const string &first, const string &second) {
  return !(first == second);
}

bool operator!=(const string &first, const char *second) {
  return !(first == second);
}

std::ostream &operator<<(std::ostream &out, const string &str) {
  out << str.str;

  return out;
}

std::size_t string::get_length() const { return this->length; }

const char *string::get_buffer() const { return this->str; }

} // namespace qs
