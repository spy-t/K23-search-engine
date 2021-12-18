#include <qs/core.h>
#include <qs/string_view.h>

namespace qs {
string_view::string_view() : startp(nullptr), endp(nullptr) {}

string_view::string_view(const char *s, const char *e) : startp(s), endp(e) {}

string_view::string_view(const char *s) : startp(s) {
  auto ss = startp;
  endp = startp;
  while (*ss != '\0') {
    endp++;
    ss++;
  }
  // Drop the NUL byte
  endp--;
}

string_view string_view::split(char needle, u32 from) {
  if (*this == empty) {
    return empty;
  }
  auto new_start = startp + from;
  auto ss = new_start;

  while (*ss != needle && ss != endp) {
    ss++;
  }
  if (ss == endp && *endp != needle) {
    auto ret = string_view{new_start, endp};
    *this = empty;
    return ret;
  } else if (ss == endp) {
    *this = empty;
    return empty;
  } else {
    startp = ss + 1;
  }

  return string_view{new_start, ss - 1};
}

string_view string_view::empty = string_view{};

} // namespace qs
