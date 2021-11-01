#ifndef QS_HASH_SET_HPP
#define QS_HASH_SET_HPP

#include <qs/hash_table.hpp>
#include <qs/optional.hpp>
#include <qs/string.h>

namespace qs {
template <typename V> class hash_set {
  hash_table<V> table;

public:
  hash_set() { table = hash_table<V>(); }
  ~hash_set() { delete table; }

  void insert(qs::string key) { table.insert(key, key); }

  void remove(qs::string key) { table.remove(key); }

  bool contains(qs::string key) {
    qs::optional<V> val = table.lookup(key);
    return !val.is_empty();
  }
};
} // namespace qs

#endif // QS_HASH_SET_HPP
