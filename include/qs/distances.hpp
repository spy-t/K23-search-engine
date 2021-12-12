#ifndef QS_DISTANCES_HPP
#define QS_DISTANCES_HPP

#include <qs/functions/ops.hpp>
#include <qs/string.h>
#include <limits>

namespace qs {

int hamming_distance(const qs::string &s1, const qs::string &s2, int max = std::numeric_limits<int>::max());
int edit_distance(const qs::string &s1, const qs::string &s2, int max = std::numeric_limits<int>::max());

} // namespace qs

#endif // QS_DISTANCES_HPP
