#ifndef QS_OPS_HPP
#define QS_OPS_HPP

#include <iterator>
#include <qs/core.h>
#include <type_traits>
#include <utility>

namespace qs::functions {

// Copies from an iterator-like S to an iterator-like D assuming that D is
// initialized storage using a copy assignment operation
template <class S, class D> inline void copy(S start, S end, D destination) {
  while (start != end) {
    *destination = *start;
    destination++;
    start++;
  }
}

// Copies from an iterator-like S to an iterator-like D assuming that D is
// uninitialized storage using placement new
template <class S, class D>
inline void copy_uninitialized(S start, S end, D destination) {
  using value_type = typename std::iterator_traits<D>::value_type;
  D iter = destination;

  try {
    while (start != end) {
      new (static_cast<void *>(std::addressof(*iter))) value_type(*start);
      start++;
      iter++;
    }
  } catch (...) {
    // Prevent a potential memory leak if a constructor throws
    for (; destination != iter; ++destination) {
      destination->~value_type();
    }
    throw;
  }
}

template <class T> void swap(T &a, T &b) noexcept {
  T tmp = std::move(a);
  a = std::move(b);
  b = std::move(tmp);
}

template <class T> static QS_FORCE_INLINE T &max(T &a, T &b) {
  return a > b ? a : b;
}
template <class T> static QS_FORCE_INLINE T &min(T &a, T &b) {
  return a < b ? a : b;
}

template <class T> static QS_FORCE_INLINE const T &max(const T &a, const T &b) {
  return a > b ? a : b;
}
template <class T> static QS_FORCE_INLINE const T &min(const T &a, const T &b) {
  return a < b ? a : b;
}

template <class I, class T> inline I find(I start, I end, const T &t) {
  while (start != end) {
    if (*start == t) {
      return start;
    }
    start++;
  }
  return end;
}

template <class I, class P>
QS_FORCE_INLINE I find_if(I start, I end, P predicate) {
  while (start != end) {
    if (predicate(*start)) {
      return start;
    }
    start++;
  }
  return end;
}
} // namespace qs::functions
#endif // QS_OPS_HPP
