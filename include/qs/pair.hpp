#ifndef QS_STRING_PAIR_H
#define QS_STRING_PAIR_H

#include <qs/string.h>

#include <utility>

namespace qs {
template <class V, class Hash = std::hash<V>> struct pair {
  V v1;
  V v2;
  pair(V &v1, V &v2) : v1(v1), v2(v2){};
  pair(V &&v1, V &&v2) : v1(std::move(v1)), v2(std::move(v2)){};

  bool operator==(const pair &other) const {
    return std::hash<pair<V>>{}(*this) == std::hash<pair<V>>{}(other);
  }
};

} // namespace qs
template <class V> struct std::hash<qs::pair<V>> {
  std::size_t operator()(const qs::pair<V> &pair) const noexcept {
    return std::hash<V>{}(pair.v1) ^ std::hash<V>{}(pair.v2);
  }
};

#endif // QS_STRING_PAIR_H
