#ifndef QS_DISTANCES_HPP
#define QS_DISTANCES_HPP

#include <limits>
#include <qs/functions/ops.hpp>
#include <qs/string_view.h>

namespace qs {

int hamming_distance(const qs::string_view &s1, const qs::string_view &s2,
                     int max = std::numeric_limits<int>::max());
int edit_distance(const qs::string_view &s1, const qs::string_view &s2,
                  int max = std::numeric_limits<int>::max());

} // namespace qs

#endif // QS_DISTANCES_HPP
