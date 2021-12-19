#ifndef QS_PARSER_HPP
#define QS_PARSER_HPP
#include <cstdio>
#include <qs/string.h>
#include <qs/string_view.h>

namespace qs {
template <typename Fn> void parse_file(FILE *stream, char del, Fn f) {
  char *line = nullptr;
  size_t allocation_length = 0;
  while (getdelim(&line, &allocation_length, del, stream) != -1) {
    size_t length = std::strlen(line);
    if (line[length - 1] == del) {
      length = length - 1;
    }
    qs::string entry(line, length);
    f(entry);
  }
  std::free(line);
}

template <typename Fn>
void parse_string(const char *stream, const char del, Fn f) {
  qs::string_view token;
  qs::string_view stream_view{stream};
  while ((token = stream_view.split(del)) != qs::string_view::empty) {
    f(token);
  }
}
} // namespace qs
#endif // QS_PARSER_HPP
