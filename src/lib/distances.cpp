#include <qs/distances.hpp>

#include <qs/core.h>

namespace qs {

int hamming_distance(qs::string s1, qs::string s2) {
  int dist = 0;
  std::size_t len1 = s1.length();
  std::size_t len2 = s2.length();
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

#ifndef EDIT_BUFFER_SIZE
#define EDIT_BUFFER_SIZE 128ull
#endif

static QS_FORCE_INLINE void init_edit_buffer(int *buffer, int len) {
  int i;
  for (i = 0; i < len; i++) {
    buffer[i] = i;
  }
  buffer[i] = 1;
}

#define ROW (i % 2)
#define NEXT_ROW (!ROW)
#define DELETION (get(d, NEXT_ROW, j, width) + 1)
#define INSERTION (get(d, ROW, j - 1, width) + 1)
#define SUBST                                                                  \
  (get(d, NEXT_ROW, j - 1, width) + (min_str[i - 1] != max_str[j - 1]))
#define SAVE(i, j, v) (set(d, ROW, j, width, (v)))

int edit_distance(const qs::string &s1, const qs::string &s2) {
  static int d[EDIT_BUFFER_SIZE];
  int max_len = (int)s1.length();
  auto *max_str = s1.data();
  int min_len = (int)s2.length();
  auto *min_str = s2.data();
  if (max_len < min_len) {
    functions::swap(max_len, min_len);
    functions::swap(max_str, min_str);
  }

  // skip common prefix
  while (min_len > 0 && *max_str == *min_str) {
    max_str++;
    min_str++;
    max_len--;
    min_len--;
  }
  // skip common suffix
  while (min_len > 0 && max_str[max_len - 1] == min_str[min_len - 1]) {
    max_len--;
    min_len--;
  }

  // check if a string is the prefix/suffix of the other
  if (min_len == 0)
    return max_len;

  int width = max_len + 1;

  init_edit_buffer(d, width);

  int i;
  for (i = 1; i <= min_len; i++) {
    for (int j = 1; j <= max_len; j++) {
      int min =
          qs::functions::min(DELETION, qs::functions::min(INSERTION, SUBST));
      SAVE(ROW, j, min);
    }
    SAVE(NEXT_ROW, 0, i + 1);
  }
  i--;
  return get(d, ROW, max_len, width);
}
} // namespace qs
