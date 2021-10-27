#include <qs/sstream.h>
#include <qs/vector.hpp>

namespace qs {

stringstream::stringstream() {}

qs::string stringstream::str() {
  qs::string out;
  for (std::size_t i = 0; i < strings.get_size(); ++i) {
    out = out + strings[i];
  }

  return out;
}

stringstream &operator<<(stringstream &out, const qs::string &str) {
  out.strings.push(str);

  return out;
}

stringstream &operator<<(stringstream &out, const char *str) {
  qs::string s(str);

  out << s;

  return out;
}

stringstream &operator<<(stringstream &out, int num) {
  qs::string s(num);

  out << s;

  return out;
}

} // namespace qs
