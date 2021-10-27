#ifndef QS_SSTREAM_H
#define QS_SSTREAM_H

#include <qs/string.h>
#include <qs/vector.hpp>

namespace qs {
// A basic string builder. When a string is appended to the builder it should
// not be mutated further
class stringstream {
private:
  qs::vector<qs::string> strings;
  std::size_t size;

public:
  explicit stringstream();

  qs::string str();

  friend stringstream &operator<<(stringstream &out, const qs::string &str);
  friend stringstream &operator<<(stringstream &out, const char *str);
  friend stringstream &operator<<(stringstream &out, int str);
};
} // namespace qs

#endif
