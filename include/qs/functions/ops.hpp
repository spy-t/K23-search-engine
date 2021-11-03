#ifndef QS_OPS_HPP
#define QS_OPS_HPP

#include <iterator>
#include <type_traits>
#include <utility>

namespace qs {
namespace functions {

template <class S, class D> inline void copy(S start, S end, D destination) {
  while (start != end) {
    *destination = *start;
    destination++;
    start++;
  }
}

template <class T> void swap(T &a, T &b) noexcept {
  T tmp = std::move(a);
  a = std::move(b);
  b = std::move(tmp);
}

} // namespace functions
} // namespace qs
#endif // QS_OPS_HPP
