#ifndef QS_OPS_HPP
#define QS_OPS_HPP

#include <type_traits>
#include <utility>

namespace qs {
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

template <class T>
std::enable_if_t<std::is_pointer_v<T>, std::remove_pointer_t<T> &> deref(T &t) {
  return *t;
}
template <class T> std::enable_if_t<!std::is_pointer_v<T>, T &> deref(T &t) {
  return t;
}

} // namespace functions
} // namespace qs
#endif // QS_OPS_HPP
