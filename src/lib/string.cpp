#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <qs/error.h>
#include <qs/functions.hpp>
#include <qs/string.h>
#include <qs/vector.hpp>
#include <stdexcept>
#include <utility>

namespace qs {

string::string() : str(nullptr), capacity(0), length(0) {}

string string::with_size(std::size_t cap) {
  qs::string s;

  s.str = new char[cap + 1];
  s.str[0] = '\0';
  s.capacity = cap + 1;
  s.length = 0;

  return s;
}

string::string(char *source) : string((const char *)source) {}

string::string(const char *source) : string(source, strlen(source)) {}

string::string(const char *source, size_t length) {
  this->length = length;
  str = new char[length + 1];
  std::memcpy(str, source, length);
  str[length] = '\0';
  capacity = length + 1;
}

string::string(int num) {
  // Use the snprintf hack
  length = snprintf(0, 0, "%d", num);

  str = new char[length + 1];
  snprintf(str, length + 1, "%d", num);
  capacity = length + 1;
}

string::string(const string &other) : str(nullptr), length(other.length) {
  str = new char[other.length + 1];
  std::memcpy(str, other.str, other.length + 1);
  capacity = other.length + 1;
}

string::string(string &&other)
    : str(other.str), capacity(other.capacity), length(other.length) {
  other.str = nullptr;
  other.length = 0;
  other.capacity = 0;
}

string &string::operator=(const string &other) {
  if (this != &other) {
    this->str = new char[other.length + 1];
    std::memcpy(this->str, other.str, other.length + 1);
    this->length = other.length;
    this->capacity = other.length + 1;
  }
  return *this;
}

string &string::operator=(string &&other) noexcept {
  if (this != &other) {
    if (str != nullptr) {
      delete[] str;
    }
    this->str = other.str;
    this->length = other.length;
    this->capacity = other.capacity;
    other.str = nullptr;
    other.length = 0;
    other.capacity = 0;
  }
  return *this;
}

string::~string() {
  if (str != nullptr) {
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
    delete[] this->str;
    this->str = new_str;
    this->capacity = this->length + other.length + 1;
  }
  this->length = new_length;

  return *this;
}

string &string::sanitize(const string &remove_set) {
  for (size_t i = 0; i < remove_set.length; i++) {
    char *p;
    while ((p = const_cast<char *>(
                strchr(this->str, remove_set.get_buffer()[i]))) != nullptr) {
      memmove(p, p + 1, (this->str + length) - (p));
      length--;
    }
  }
  return *this;
}
string string::pure_sanitize(const string &remove_set) {
  string s(*this);
  return s.sanitize(remove_set);
}

char string::operator[](std::size_t index) { return this->str[index]; }

char string::at(std::size_t index) {
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

bool operator<(const string &first, const string &second) {
  return std::memcmp(
             first.get_buffer(), second.get_buffer(),
             qs::functions::min(first.get_length(), second.get_length())) < 0;
}
bool operator<(const string &first, const char *second) {
  return std::memcmp(
             first.get_buffer(), second,
             qs::functions::min(first.get_length(), std::strlen(second))) < 0;
}
bool operator<=(const string &first, const string &second) {
  return std::memcmp(
             first.get_buffer(), second.get_buffer(),
             qs::functions::min(first.get_length(), second.get_length())) <= 0;
}
bool operator<=(const string &first, const char *second) {
  return std::memcmp(
             first.get_buffer(), second,
             qs::functions::min(first.get_length(), std::strlen(second))) <= 0;
}
bool operator>(const string &first, const string &second) {
  return std::memcmp(
             first.get_buffer(), second.get_buffer(),
             qs::functions::min(first.get_length(), second.get_length())) > 0;
}
bool operator>(const string &first, const char *second) {
  return std::memcmp(
             first.get_buffer(), second,
             qs::functions::min(first.get_length(), std::strlen(second))) > 0;
}
bool operator>=(const string &first, const string &second) {

  return std::memcmp(
             first.get_buffer(), second.get_buffer(),
             qs::functions::min(first.get_length(), second.get_length())) >= 0;
}
bool operator>=(const string &first, const char *second) {
  return std::memcmp(
             first.get_buffer(), second,
             qs::functions::min(first.get_length(), std::strlen(second))) >= 0;
}

std::ostream &operator<<(std::ostream &out, const string &str) {
  out << str.str;

  return out;
}

std::size_t string::get_length() const { return this->length; }

const char *string::get_buffer() const { return this->str; }

} // namespace qs
