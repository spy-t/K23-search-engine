#ifndef PLAINLIB_OPS_HPP
#define PLAINLIB_OPS_HPP

#include <type_traits>
#include <utility>

namespace pl {
namespace functions {

template <class I, class D> inline void copy(I from, I to, D destination) {
  while (from != to) {
    *destination = *from;
    destination++;
    from++;
  }
}

template <class T> void swap(T &a, T &b) noexcept {
  T tmp = std::move(a);
  a = std::move(b);
  b = std::move(tmp);
}

} // namespace functions
} // namespace pl
#endif // PLAINLIB_OPS_HPP
