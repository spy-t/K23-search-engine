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
  void insert(qs::string &&key);

  void remove(const qs::string &key);

  bool contains(const qs::string &key);
  std::size_t get_size();

  struct iterator {
    using ht_iterator = hash_table<char>::iterator;
    friend class hash_set;

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const qs::string;
    using pointer = value_type *;
    using reference = value_type &;

  private:
    ht_iterator iter;

    explicit iterator(ht_iterator iter) : iter(iter) {}

  public:
    reference operator*() { return iter->get_key(); }
    pointer operator->() { return &this->iter->get_key(); }

    iterator &operator++() {
      ++iter;
      return *this;
    }

    iterator operator++(int) {
      iterator old = *this;
      ++iter;
      return old;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.iter == b.iter;
    }
    friend bool operator!=(const iterator &a, const iterator &b) {
      return !(a == b);
    }
  };

  iterator begin() { return iterator(table.begin()); }
  iterator end() { return iterator(table.end()); }
};
} // namespace qs

#endif // QS_HASH_SET_HPP
