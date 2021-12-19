#include <iostream>
#include <qs/sstream.h>

namespace qs {

stringstream::stringstream() : size(0) {}

qs::string stringstream::str() {
  auto out = qs::string::with_size(size);
  for (std::size_t i = 0; i < strings.get_size(); ++i) {
    out = out + strings[i];
  }

  return out;
}

stringstream &operator<<(stringstream &out, const qs::string &str) {
  out.strings.push(str);
  out.size += str.length();

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
