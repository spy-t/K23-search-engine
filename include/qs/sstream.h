#ifndef QS_SSTREAM_H
#define QS_SSTREAM_H

#include <qs/string.h>
#include <qs/vector.hpp>

namespace qs {
class stringstream {
private:
  qs::vector<qs::string> strings;

public:
  explicit stringstream();

  qs::string str();

  friend stringstream &operator<<(stringstream &out, const qs::string &str);
  friend stringstream &operator<<(stringstream &out, const char *str);
  friend stringstream &operator<<(stringstream &out, int str);
};
} // namespace qs

#endif
