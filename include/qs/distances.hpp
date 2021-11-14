#ifndef QS_DISTANCES_HPP
#define QS_DISTANCES_HPP

#include <qs/functions/ops.hpp>
#include <qs/string.h>

namespace qs {

int hamming_distance(qs::string s1, qs::string s2);
int edit_distance(qs::string s1, qs::string s2);

} // namespace qs

#endif // QS_DISTANCES_HPP
