#ifndef QS_STRING_H
#define QS_STRING_H

#include <cstdint>
#include <cstring>
#include <iostream>

namespace qs {

// A wrapper around NULL terminated ASCII strings
class string {
private:
  char *str;

  // The size of the underlying buffer. Can be used for efficient concatenating
  std::size_t capacity;

  // The size of the string in bytes without the NULL byte
  std::size_t length;

  bool is_alloced;

public:
  explicit string();
  static string with_size(std::size_t capacity);
  static string from_allocated_string(char *str);
  explicit string(char *source);
  explicit string(const char *source, size_t length);
  explicit string(const char *source);
  explicit string(int num);

  string(const string &other);
  string &operator=(const string &other);

  string(string &&other);
  string &operator=(string &&other);

  ~string();

  // Pure addition operation. The result is a new instance of qs::string
  string cat(const string &other);

  // Impure addition operation. Mutates the string in place
  string &operator+(const string &other);

  // Checked dereference operation
  char operator[](std::size_t index);
  // Unchecked dereference operation
  char *operator*();

  friend bool operator==(const string &first, const string &second);
  friend bool operator==(const string &first, const char *second);

  friend bool operator!=(const string &first, const string &second);
  friend bool operator!=(const string &first, const char *second);

  friend std::ostream &operator<<(std::ostream &out, const string &str);

  std::size_t get_length() const;
  const char *get_buffer() const;
};

} // namespace qs

#endif
