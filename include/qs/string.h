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
  std::size_t size; // The size of the string in bytes without the NULL byte
  bool is_alloced;

public:
  explicit string();
  explicit string(std::size_t capacity);
  explicit string(char *source);

  string(const string &other);
  string &operator=(const string &other);

  string(string &&other);
  string &operator=(string &&other);

  ~string();

  // Pure addition operation. The result is a new instance of qs::string
  string operator+(const string &other);

  friend bool operator==(const string &first, const string &second);
  friend std::ostream &operator<<(std::ostream &out, const string &str);

  std::size_t get_size() const;
};

} // namespace qs

#endif
