#include <qs/distances.hpp>

#include <qs/core.h>
#include <qs/string_view.h>

namespace qs {

int hamming_distance(qs::string_view s1, qs::string_view s2, int max) {
  if (s1.size() != s2.size()) {
    // TODO(spyros): This should be an assertion. Change this when the assertion
    // system is in place
    throw std::runtime_error("cannot find hamming distance between two strings "
                             "of different lengths");
  }
  int dist = 0;
  std::size_t len = s1.size();
  auto *c1 = s1.data();
  auto *c2 = s2.data();
  for (std::size_t i = 0; i < len && max >= dist; i++) {
    dist += c1[i] != c2[i];
  }
  return dist;
}

static QS_FORCE_INLINE void init_edit_buffer(int *buffer, int len) {
  for (int i = 0; i < len; i++) {
    buffer[i] = i;
  }
}

#ifndef EDIT_BUFFER_SIZE
#define EDIT_BUFFER_SIZE 64ul
#endif

int edit_distance(qs::string_view s1, qs::string_view s2, int max) {
  static int d[EDIT_BUFFER_SIZE];
  int max_len = (int)s1.size();
  auto *max_str = s1.data();
  int min_len = (int)s2.size();
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

  for (int i = 1; i <= min_len; i++) {
    d[0] = i;
    int prev = i - 1;
    int best = i;
    for (int j = 1; j <= max_len; j++) {
      int sub = prev + (int)(min_str[i - 1] != max_str[j - 1]);
      int del = d[j - 1];
      int ins = d[j];
      prev = d[j];
      d[j] = functions::min(sub, functions::min(del, ins) + 1);
      best = functions::min(best, d[j]);
    }
    if (best > max) {
      return best;
    }
  }

  return d[max_len];
}
} // namespace qs
