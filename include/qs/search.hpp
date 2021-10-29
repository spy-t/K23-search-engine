#ifndef QS_SEARCH_HPP
#define QS_SEARCH_HPP

#include <qs/functions.hpp>

namespace qs {

template <class T> int binary_search(T data[], std::size_t size, T search_elem) {
  int left = 0;
  int right = int(size) - 1;
  int middle;

  while (left <= right) {
    middle = (left + right) / 2;
    if (qs::functions::deref(data[middle]) < qs::functions::deref(search_elem)) {
      left = middle + 1;
    } else if (qs::functions::deref(data[middle]) > qs::functions::deref(search_elem)) {
      right = middle - 1;
    } else {
      return middle;
    }
  }
  return -1;
}
}; // namespace qs

#endif // QS_SEARCH_HPP
