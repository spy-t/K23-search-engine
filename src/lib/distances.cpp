#include <qs/distances.hpp>

#include <qs/core.h>

namespace qs {

int hamming_distance(qs::string s1, qs::string s2) {
  int dist = 0;
  std::size_t len1 = s1.get_length();
  std::size_t len2 = s2.get_length();
  if (len1 != len2) {
    // TODO(spyros): This should be an assertion. Change this when the assertion
    // system is in place
    throw std::runtime_error("cannot find hamming distance between two strings "
                             "of differenct lengths");
  }
  std::size_t min_len = qs::functions::min(len1, len2);

  for (std::size_t i = 0; i < min_len; i++) {
    if (s1[i] != s2[i]) {
      dist++;
    }
  }
  dist += int(len1 < len2 ? len2 - len1 : len1 - len2);
  return dist;
}

static QS_FORCE_INLINE int get(int *arr, int row, int col, int width) {
  return arr[row * width + col];
}

static QS_FORCE_INLINE void set(int *arr, int row, int col, int width,
                                int val) {
  arr[row * width + col] = val;
}

int edit_distance(qs::string s1, qs::string s2) {
  int l1 = (int)s1.get_length();
  int l2 = (int)s2.get_length();

  int height = l1 + 1;
  int width = l2 + 1;

  int *d = new int[width * height];

  for (int i = 0; i <= l1; i++) {
    set(d, i, 0, width, i);
  }

  for (int j = 0; j <= l2; j++) {
    set(d, 0, j, width, j);
  }

  for (int i = 1; i <= l1; i++) {
    for (int j = 1; j <= l2; j++) {
      int substitution_cost = s1[i - 1] == s2[j - 1] ? 0 : 1;
      int deletion = get(d, i - 1, j, width) + 1;
      int insertion = get(d, i, j - 1, width) + 1;
      int substitution = get(d, i - 1, j - 1, width) + substitution_cost;

      int min = qs::functions::min(deletion,
                                   qs::functions::min(insertion, substitution));
      set(d, i, j, width, min);
    }
  }

  int ret = get(d, l1, l2, width);
  delete[] d;

  return ret;
}

#ifndef EDIT_BUFFER_SIZE
#define EDIT_BUFFER_SIZE 512ull
#endif

class StrippedStrings {
public:
  qs::string s1;
  qs::string s2;

  explicit StrippedStrings(qs::string &&s1, qs::string &&s2) : s1(s1), s2(s2) {}

  StrippedStrings(StrippedStrings &&other) noexcept
      : s1(std::move(other.s1)), s2(std::move(other.s2)) {}

  StrippedStrings &operator=(StrippedStrings &&other) noexcept {
    if (this != &other) {
      s1 = other.s1;
      s2 = other.s2;
    }

    return *this;
  }
};

static QS_FORCE_INLINE StrippedStrings
skip_common_prefix(const qs::string &s1, const qs::string &s2) {
  auto s1i = ((qs::string)s1).begin();
  auto s1e = ((qs::string)s1).end();
  auto s2i = ((qs::string)s2).begin();
  auto s2e = ((qs::string)s2).end();

  std::size_t offset = 0;

  while (s1i != s1e && s2i != s2e && *s1i == *s2i) {
    s1i++;
    s2i++;
    offset++;
  }

  return StrippedStrings(qs::string(s1i.operator->()),
                         qs::string(s2i.operator->()));
}

static QS_FORCE_INLINE StrippedStrings
skip_common_suffix(const qs::string &s1, const qs::string &s2) {
  auto s1i = ((qs::string)s1).rbegin();
  auto s1e = ((qs::string)s1).rend();
  auto s2i = ((qs::string)s2).rbegin();
  auto s2e = ((qs::string)s2).rend();

  auto s1len = (long long)s1.get_length();
  auto s2len = (long long)s2.get_length();

  while (s1i != s1e && s2i != s2e && *s1i == *s2i) {
    s1len--;
    s2len--;
  }
  if (s1len == -1) {
    return StrippedStrings{qs::string(), qs::string(s2i.operator->(), s2len)};
  } else if (s2len == -1) {
    return StrippedStrings{
        qs::string(s1i.operator->(), s1len),
        qs::string(),
    };
  }

  return StrippedStrings{
      qs::string(s1i.operator->(), s1len),
      qs::string(s2i.operator->(), s2len),
  };
}

static void init_edit_buffer(std::size_t *buffer, std::size_t len) {
  for (std::size_t i = 0; i < len; i++) {
    buffer[i] = i;
  }
}

int fast_distance_optimized(const qs::string &s1, const qs::string &s2) {
  if (s1.get_length() == 0 || s2.get_length() == 0) {
    return (int)functions::max(s1.get_length(), s2.get_length());
  }

  // skip common prefix
  auto pss = skip_common_prefix(s1, (qs::string)s2);

  /**
   * if a string A is the prefix of a string B,
   * then the edit distance between them is
   * equal to string B's length without the
   * prefix (A)
   * e.g. hello | helloworld
   */
  if (pss.s1.get_length() == 0) {
    return (int)pss.s2.get_length();
  } else if (pss.s2.get_length() == 0) {
    return (int)pss.s1.get_length();
  }

  auto tmp1 = std::move(pss.s1);
  auto tmp2 = std::move(pss.s2);

  pss = skip_common_suffix(tmp1, tmp2);

  /**
   * if a string A is the suffix of a string B,
   * then the edit distance between them is
   * equal to string B's length
   * e.g. world | helloworld
   */
  if (tmp1.get_length() == 0) {
    return (int)tmp2.get_length();
  } else if (tmp2.get_length() == 0) {
    return (int)tmp1.get_length();
  }

  auto s1s =
      std::move(pss.s1.get_length() < pss.s2.get_length() ? pss.s1 : pss.s2);
  auto s2s =
      std::move(pss.s1.get_length() < pss.s2.get_length() ? pss.s2 : pss.s1);

  static std::size_t buffer[EDIT_BUFFER_SIZE];
  init_edit_buffer(buffer, s2s.get_length() + 1);
  std::size_t end_j;
  for (std::size_t i = 1; i <= s1s.get_length(); i++) {
    std::size_t cost = buffer[0]++;

    std::size_t start_j =
        functions::max(1ll, (long long)(i + 1 - EDIT_BUFFER_SIZE / 2));
    end_j = functions::min((long long)(s2s.get_length() + 1),
                           (long long)(i + 1 + EDIT_BUFFER_SIZE / 2));

    std::size_t col_min = EDIT_BUFFER_SIZE;

    for (auto j = start_j; j < end_j; j++) {
      auto insertion = cost;
      auto deletion = buffer[j];
      auto substitution = insertion + (s1s[i - 1] != s2s[j - 1]);
      cost =
          functions::min(functions::min(insertion, deletion) + 1, substitution);

      col_min = functions::min(col_min, cost);
      functions::swap(buffer[j], cost);
    }
    if (col_min >= EDIT_BUFFER_SIZE) {
      return EDIT_BUFFER_SIZE;
    }
  }
  return (int)buffer[end_j - 1];
}

} // namespace qs
