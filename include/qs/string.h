#ifndef QS_STRING_H
#define QS_STRING_H

#include <cstdint>
#include <cstring>
#include <iostream>

#include <qs/core.h>
#include <qs/functions.hpp>
#include <qs/hash.h>

namespace qs {

// A wrapper around NULL terminated ASCII strings
class string {
private:
  char *str;

  // The size of the underlying buffer. Can be used for efficient concatenating
  std::size_t cap;

  // The size of the string in bytes without the NULL byte
  std::size_t len;

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

  // Impure addition operation. Mutates the string in place
  string &cat(const string &other);

  // Pure addition operation. The result is a new instance of qs::string
  string operator+(const string &other);

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
    return first.len == second.len &&
           std::memcmp(first.str, second.str, first.len) == 0;
  }
  friend QS_FORCE_INLINE bool operator==(const string &first,
                                         const char *second) {
    return first.len == std::strlen(second) &&
           std::memcmp(first.str, second, first.len) == 0;
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
    return std::memcmp(first.data(), second.data(),
                       qs::functions::min(first.length(), second.length())) < 0;
  }
  friend QS_FORCE_INLINE bool operator<(const string &first,
                                        const char *second) {
    return std::memcmp(
               first.data(), second,
               qs::functions::min(first.length(), std::strlen(second))) < 0;
  }
  friend QS_FORCE_INLINE bool operator<=(const string &first,
                                         const string &second) {
    return std::memcmp(first.data(), second.data(),
                       qs::functions::min(first.length(), second.length())) <=
           0;
  }

  friend QS_FORCE_INLINE bool operator<=(const string &first,
                                         const char *second) {
    return std::memcmp(
               first.data(), second,
               qs::functions::min(first.length(), std::strlen(second))) <= 0;
  }

  friend QS_FORCE_INLINE bool operator>(const string &first,
                                        const string &second) {
    return std::memcmp(first.data(), second.data(),
                       qs::functions::min(first.length(), second.length())) > 0;
  }
  friend QS_FORCE_INLINE bool operator>(const string &first,
                                        const char *second) {
    return std::memcmp(
               first.data(), second,
               qs::functions::min(first.length(), std::strlen(second))) > 0;
  }
  friend QS_FORCE_INLINE bool operator>=(const string &first,
                                         const string &second) {
    return std::memcmp(first.data(), second.data(),
                       qs::functions::min(first.length(), second.length())) >=
           0;
  }
  friend QS_FORCE_INLINE bool operator>=(const string &first,
                                         const char *second) {
    return std::memcmp(
               first.data(), second,
               qs::functions::min(first.length(), std::strlen(second))) >= 0;
  }

  friend std::ostream &operator<<(std::ostream &out, const string &str);

  struct iterator {
    const char *p;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const char;
    using pointer = value_type *;
    using reference = value_type &;

    explicit iterator(const char *p) : p(p) {}
    reference operator*() { return *this->p; }
    pointer operator->() { return this->p; }

    iterator operator++() {
      p++;
      return *this;
    }
    iterator operator++(int) {
      auto tmp = *this;
      p++;
      return tmp;
    }
    iterator &operator--() {
      p--;
      return *this;
    }
    iterator operator--(int) {
      auto tmp = *this;
      p--;
      return tmp;
    }
    friend bool operator==(const iterator &a, const iterator &b) {
      return a.p == b.p;
    }
    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.p != b.p;
    }
  };

  iterator begin() { return iterator(this->str); }
  iterator end() { return iterator(&(this->str[this->len])); }

  auto rbegin() { return std::make_reverse_iterator(this->end()); }
  auto rend() { return std::make_reverse_iterator(this->begin()); }
  std::size_t length() const;
  const char *data() const;
};

} // namespace qs

template <> struct std::hash<qs::string> {

  std::size_t operator()(qs::string const &s) const noexcept {
    return qs::djb2((const uint8_t *)s.data());
  }
};

#endif
