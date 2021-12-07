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

#ifndef MAX_EDIT_DIST
#define MAX_EDIT_DIST EDIT_BUFFER_SIZE
#endif

static void init_edit_buffer(std::size_t *buffer, std::size_t len) {
  for (std::size_t i = 0; i < len; i++) {
    buffer[i] = i;
  }
}

int fast_distance(const qs::string &s1, const qs::string &s2) {
  if (s1.get_length() == 0 || s2.get_length() == 0) {
    return (int)functions::max(s1.get_length(), s2.get_length());
  }

  const char *start1 = s1.get_buffer();
  const char *start2 = s2.get_buffer();
  std::size_t len1 = s1.get_length();
  std::size_t len2 = s2.get_length();
  const char *end1 = start1 + len1;
  const char *end2 = start2 + len2;

  // skip common prefix
  while (*start1++ == *start2++ && start1 != end1 && start2 != end2) {
    len1--;
    len2--;
  }

  /**
   * if a string A is the prefix of a string B,
   * then the edit distance between them is
   * equal to string B's length without the
   * prefix (A)
   * e.g. hello | helloworld
   */
  if (len1 == 0) {
    return (int)len2;
  } else if (len2 == 0) {
    return (int)len1;
  }

  // skip common suffix
  std::size_t common_suffix_len = 0;
  while (*end1-- == *end2-- && start1 != end1 && start2 != end2) {
    len1--;
    len2--;
    common_suffix_len++;
  }

  /**
   * if a string A is the suffix of a string B,
   * then the edit distance between them is
   * equal to the length of the common suffix
   * and the rest of the characters of the
   * bigger word
   * e.g. world | helloworld
   */
  if (len1 == 0) {
    return (int)(len2 + common_suffix_len);
  } else if (len2 == 0) {
    return (int)(len1 + common_suffix_len);
  }

  const char *small;
  std::size_t small_len;
  const char *big;
  std::size_t big_len;

  if (len1 < len2) {
    small = start1;
    small_len = len1;
    big = start2;
    big_len = len2;
  } else {
    small = start2;
    small_len = len2;
    big = start1;
    big_len = len1;
  }

  static std::size_t buffer[EDIT_BUFFER_SIZE];
  init_edit_buffer(buffer, big_len + 1);
  std::size_t end_j;
  for (std::size_t i = 1; i <= small_len; i++) {
    std::size_t cost = buffer[0]++;

    std::size_t start_j =
        functions::max(1ll, (long long)(i - MAX_EDIT_DIST / 2));
    end_j = functions::min(big_len + 1,
                           (std::size_t)(i + MAX_EDIT_DIST / 2));

    std::size_t col_min = MAX_EDIT_DIST;

    for (auto j = start_j; j < end_j; j++) {
      auto insertion = buffer[j];
      auto deletion = cost;
      auto substitution = cost + (small[i - 1] != big[j - 1]);
      cost =
          functions::min(functions::min(insertion, deletion) + 1, substitution);

      col_min = functions::min(col_min, cost);
      functions::swap(buffer[j], cost);
    }
    if (col_min >= MAX_EDIT_DIST) {
      return MAX_EDIT_DIST;
    }
  }
  return (int)buffer[end_j - 1];
}

} // namespace qs
