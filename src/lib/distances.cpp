#include <qs/distances.hpp>

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

  for (int j = 0; j <= l2; j++) {
    d[0][j] = j;
  }

  for (int i = 1; i <= l1; i++) {
    for (int j = 1; j <= l2; j++) {
      int substitution_cost = s1[i - 1] == s2[j - 1] ? 0 : 1;
      int deletion = d[i - 1][j] + 1;
      int insertion = d[i][j - 1] + 1;
      int substitution = d[i - 1][j - 1] + substitution_cost;

      int min = qs::functions::min(deletion,
                                   qs::functions::min(insertion, substitution));
      d[i][j] = min;
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
