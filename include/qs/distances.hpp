#ifndef QS_DISTANCES_HPP
#define QS_DISTANCES_HPP

#include <qs/string.h>

namespace qs {

int edit_distance(qs::string s1, qs::string s2) {
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

// TODO add more distances

} // namespace qs

#endif // QS_DISTANCES_HPP
