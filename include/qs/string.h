#ifndef QS_STRING_H
#define QS_STRING_H

#include <cstdint>
#include <cstring>
#include <iostream>

#include <qs/core.h>
#include <qs/functions.hpp>

namespace qs {

// A wrapper around NULL terminated ASCII strings
class string {
private:
  char *str;

  // The size of the underlying buffer. Can be used for efficient concatenating
  std::size_t capacity;

  // The size of the string in bytes without the NULL byte
  std::size_t length;

public:
  explicit string();
  static string with_size(std::size_t capacity);
  explicit string(char *source);
  explicit string(const char *source, size_t length);
  explicit string(const char *source);
  explicit string(int num);

  string(const string &other);
  string &operator=(const string &other);

  string(string &&other);
  string &operator=(string &&other) noexcept;

  ~string();

  // Pure addition operation. The result is a new instance of qs::string
  string cat(const string &other);

  // Impure addition operation. Mutates the string in place
  string &operator+(const string &other);

  string &sanitize(const string &remove_set);

  string pure_sanitize(const string &remove_set);

  // Unchecked index operation
  QS_FORCE_INLINE char operator[](std::size_t index) {
    return this->str[index];
  }
  // Checked index operation
  char at(std::size_t index);

  // Unchecked dereference operation
  char *operator*();

  friend QS_FORCE_INLINE bool operator==(const string &first,
                                         const string &second) {
    return first.length == second.length &&
           std::memcmp(first.str, second.str, first.length) == 0;
  }
  friend QS_FORCE_INLINE bool operator==(const string &first,
                                         const char *second) {
    return first.length == std::strlen(second) &&
           std::memcmp(first.str, second, first.length) == 0;
  }

  friend QS_FORCE_INLINE bool operator!=(const string &first,
                                         const string &second) {
    return !(first == second);
  }
  friend QS_FORCE_INLINE bool operator!=(const string &first,
                                         const char *second) {
    return !(first == second);
  }

  friend QS_FORCE_INLINE bool operator<(const string &first,
                                        const string &second) {
    return std::memcmp(
               first.get_buffer(), second.get_buffer(),
               qs::functions::min(first.get_length(), second.get_length())) < 0;
  }
  friend QS_FORCE_INLINE bool operator<(const string &first,
                                        const char *second) {
    return std::memcmp(
               first.get_buffer(), second,
               qs::functions::min(first.get_length(), std::strlen(second))) < 0;
  }
  friend QS_FORCE_INLINE bool operator<=(const string &first,
                                         const string &second) {
    return std::memcmp(first.get_buffer(), second.get_buffer(),
                       qs::functions::min(first.get_length(),
                                          second.get_length())) <= 0;
  }

  friend QS_FORCE_INLINE bool operator<=(const string &first,
                                         const char *second) {
    return std::memcmp(first.get_buffer(), second,
                       qs::functions::min(first.get_length(),
                                          std::strlen(second))) <= 0;
  }

  friend QS_FORCE_INLINE bool operator>(const string &first,
                                        const string &second) {
    return std::memcmp(
               first.get_buffer(), second.get_buffer(),
               qs::functions::min(first.get_length(), second.get_length())) > 0;
  }
  friend QS_FORCE_INLINE bool operator>(const string &first,
                                        const char *second) {
    return std::memcmp(
               first.get_buffer(), second,
               qs::functions::min(first.get_length(), std::strlen(second))) > 0;
  }
  friend QS_FORCE_INLINE bool operator>=(const string &first,
                                         const string &second) {
    return std::memcmp(first.get_buffer(), second.get_buffer(),
                       qs::functions::min(first.get_length(),
                                          second.get_length())) >= 0;
  }
  friend QS_FORCE_INLINE bool operator>=(const string &first,
                                         const char *second) {
    return std::memcmp(first.get_buffer(), second,
                       qs::functions::min(first.get_length(),
                                          std::strlen(second))) >= 0;
  }

  friend std::ostream &operator<<(std::ostream &out, const string &str);

  std::size_t get_length() const;
  const char *get_buffer() const;
};

} // namespace qs

#endif
