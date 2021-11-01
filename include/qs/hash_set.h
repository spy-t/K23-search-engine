#ifndef QS_HASH_SET_HPP
#define QS_HASH_SET_HPP

#include <qs/hash_table.hpp>
#include <qs/optional.hpp>
#include <qs/string.h>

namespace qs {
class hash_set {
  hash_table<char> table;

public:
  void insert(const qs::string &key);

  void remove(const qs::string &key);

  bool contains(const qs::string &key);

  vector<qs::string> get_all();
};
} // namespace qs

#endif // QS_HASH_SET_HPP
