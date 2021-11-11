#ifndef QS_DISTANCES_HPP
#define QS_DISTANCES_HPP

#include <qs/string.h>

namespace qs {

int hamming_distance(qs::string s1, qs::string s2) {
  int dist = 0;
  std::size_t len1 = s1.get_length();
  std::size_t len2 = s2.get_length();
  std::size_t min_len = len1 < len2 ? len1 : len2;

  for (std::size_t i = 0; i < min_len; i++) {
    if (s1[i] != s2[i]) {
      dist++;
    }
  }
  dist += int(len1 < len2 ? len2 - len1 : len1 - len2);
  return dist;
}

int edit_distance(qs::string s1, qs::string s2) {
  int l1 = (int)s1.get_length();
  int l2 = (int)s2.get_length();

  int **d = new int *[l1 + 1];
  for (int i = 0; i <= l1; i++) {
    d[i] = new int[l2 + 1];
  }

  for (int i = 0; i <= l1; i++) {
    d[i][0] = i;
  }

  for (int i = 0; i <= l2; i++) {
    d[0][i] = i;
  }

  for (int j = 0; j < l2; j++) {
    for (int i = 0; i < l1; i++) {
      int substitution_cost = s1[i] == s2[j];
      int deletion = d[i][j + 1] + 1;
      int insertion = d[i + 1][j] + 1;
      int substitution = d[i][j] + substitution_cost;

      int min = deletion;
      if (insertion < min) {
        min = insertion;
      }
      if (substitution < min) {
        min = substitution;
      }
      d[i + 1][j + 1] = min;
    }
  }

  int ret = d[l1][l2];
  for (int i = 0; i <= l1; i++) {
    delete[] d[i];
  }
  delete[] d;

  return ret;
}

} // namespace qs

#endif // QS_DISTANCES_HPP
