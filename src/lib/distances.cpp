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
static QS_FORCE_INLINE void set(int *arr, int row, int col, int width, int val) {
  arr[row * width + col] = val;
}

int edit_distance(qs::string s1, qs::string s2) {
  int l1 = (int)s1.get_length();
  int l2 = (int)s2.get_length();

  int height = l1 + 1;
  int width = l2 + 1;

  int *d = new int [width * height];

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

} // namespace qs
