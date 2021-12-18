#ifndef QS_STRING_VIEW_H
#define QS_STRING_VIEW_H

#include <qs/core.h>
#include <qs/string.h>

namespace qs {
class string_view {

  const char *startp;
  const char *endp;

public:
  static string_view empty;

  string_view();
  string_view(const char *start, const char *end);
  string_view(const char *start);

  string_view(const string_view &other) = default;
  string_view &operator=(const string_view &other) = default;
  string_view(string_view &&other) = default;
  string_view &operator=(string_view &&other) = default;

  // Assumes that the characters are byte wide

  QS_FORCE_INLINE std::size_t size() const {
    return startp && endp ? (endp - startp) + 1 : 0;
  }

  QS_FORCE_INLINE bool operator==(const string_view &other) const {
    auto this_length = this->size();
    if (this_length != other.size()) {
      return false;
    }

    return std::memcmp(startp, other.startp, this_length) == 0;
  }

  QS_FORCE_INLINE bool operator==(const string &other) const {
    auto this_length = this->size();
    if (this_length != other.length()) {
      return false;
    }

    return std::memcmp(startp, other.data(), this_length) == 0;
  }

  QS_FORCE_INLINE bool operator==(const char *other) const {
    auto this_length = this->size();
    auto other_length = strlen(other);
    if (this_length != other_length) {
      return false;
    }

    return std::memcmp(startp, other, this_length) == 0;
  }

  QS_FORCE_INLINE bool operator!=(const string_view &other) const {
    return !(*this == other);
  }

  QS_FORCE_INLINE bool operator!=(const string &other) const {
    return !(*this == other);
  }

  QS_FORCE_INLINE bool operator!=(const char *other) const {
    return !(*this == other);
  }

  QS_FORCE_INLINE char operator*() { return *this->startp; }

  QS_FORCE_INLINE char operator[](std::size_t index) {
    return *(this->startp + index);
  }

  QS_FORCE_INLINE char at(std::size_t index) {
    if (this->size() <= index) {
      throw std::runtime_error("index out of bounds");
    }
    return *(this->startp + index);
  }

  QS_FORCE_INLINE string copy() { return string{startp, size()}; }

  QS_FORCE_INLINE string_view substr(u32 from, u32 to) {
    return string_view{startp + from, startp + to};
  }

  string_view split(char needle, u32 from = 0);

  QS_FORCE_INLINE const char *data() const { return this->startp; }

  QS_FORCE_INLINE const char *begin() const { return this->startp; }

  QS_FORCE_INLINE const char *end() const { return this->endp + 1; }

  QS_FORCE_INLINE const char *last() const { return this->endp; }

  QS_FORCE_INLINE string_view get_string_view() const { return *this; }
};

} // namespace qs

template <> struct std::hash<qs::string_view> {

  std::size_t operator()(qs::string_view const &s) const noexcept {
    u64 hash = 5381;
    auto str = s.data();
    i32 c;
    while (str != s.last()) {
      c = *str++;
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
  }
};

#endif
