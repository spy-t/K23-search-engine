#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <qs/core.h>
#include <qs/error.h>
#include <qs/functions.hpp>
#include <qs/string.h>
#include <qs/vector.hpp>
#include <stdexcept>
#include <utility>

namespace qs {

string::string() : str(nullptr), cap(0), len(0) {}

string string::with_size(std::size_t cap) {
  qs::string s;

  s.str = new char[cap + 1];
  s.str[0] = '\0';
  s.cap = cap + 1;
  s.len = 0;

  return s;
}

string::string(char *source) : string((const char *)source) {}

string::string(const char *source) : string(source, strlen(source)) {}

string::string(const char *source, size_t length) {
  this->len = length;
  str = new char[length + 1];
  std::memcpy(str, source, length);
  str[length] = '\0';
  cap = length + 1;
}

string::string(int num) {
  // Use the snprintf hack
  len = snprintf(0, 0, "%d", num);

  str = new char[len + 1];
  snprintf(str, len + 1, "%d", num);
  cap = len + 1;
}

string::string(const string &other) : str(nullptr), len(other.len) {
  str = new char[other.len + 1];
  std::memcpy(str, other.str, other.len + 1);
  cap = other.len + 1;
}

string::string(string &&other)
    : str(other.str), cap(other.cap), len(other.len) {
  other.str = nullptr;
  other.len = 0;
  other.cap = 0;
}

string &string::operator=(const string &other) {
  if (this != &other) {
    this->str = new char[other.len + 1];
    std::memcpy(this->str, other.str, other.len + 1);
    this->len = other.len;
    this->cap = other.len + 1;
  }
  return *this;
}

string &string::operator=(string &&other) noexcept {
  if (this != &other) {
    if (str != nullptr) {
      delete[] str;
    }
    this->str = other.str;
    this->len = other.len;
    this->cap = other.cap;
    other.str = nullptr;
    other.len = 0;
    other.cap = 0;
  }
  return *this;
}

string::~string() {
  if (str != nullptr) {
    delete[] str;
  }
}

string &string::cat(const string &other) {
  auto new_length = this->len + other.len;

  // No resizing needed
  if (this->cap > new_length) {
    std::memcpy(this->str + this->len, other.str, other.len);
    this->str[new_length] = '\0';
  } else {
    auto new_str = new char[this->len + other.len + 1];
    std::memcpy(new_str, this->str, this->len);
    std::memcpy(new_str + this->len, other.str, other.len);
    delete[] this->str;
    this->str = new_str;
    this->cap = this->len + other.len + 1;
  }
  this->len = new_length;

  return *this;
}

string string::operator+(const string &other) {
  auto s = string::with_size(this->len + other.len);
  std::memcpy(s.str, this->str, this->len);
  std::memcpy(s.str + this->len, other.str, other.len);
  s.len = this->len + other.len;
  s.str[s.len] = '\0';

  return s;
}

string &string::sanitize(const string &remove_set) {
  for (size_t i = 0; i < remove_set.len; i++) {
    char *p;
    while ((p = const_cast<char *>(strchr(this->str, remove_set.data()[i]))) !=
           nullptr) {
      memmove(p, p + 1, (this->str + this->len) - (p));
      this->len--;
    }
  }
  return *this;
}
string string::pure_sanitize(const string &remove_set) {
  string s(*this);
  return s.sanitize(remove_set);
}

char string::at(std::size_t index) {
  if (this->len == 0) {
    throw std::runtime_error("Invalid index. String is empty");
  }
  if (index >= this->len) {
    throw std::runtime_error("Invalid index. Buffer overflow");
  }

  return this->str[index];
}

char *string::operator*() { return this->str; }

std::ostream &operator<<(std::ostream &out, const string &str) {
  out << str.str;

  return out;
}

std::size_t string::length() const { return this->len; }

const char *string::data() const { return this->str; }

} // namespace qs
