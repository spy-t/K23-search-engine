#ifndef QS_HASH_SET_HPP
#define QS_HASH_SET_HPP

#include <qs/hash_table.hpp>
#include <qs/optional.hpp>
#include <qs/string.h>

namespace qs {
template <class K, class Hash = std::hash<K>> class hash_set {
  hash_table<K, char> table;

public:
  void insert(const K &key) { this->table.insert(key, '\0'); }
  void insert(K &&key) { this->table.insert(std::move(key), '\0'); }

  void remove(const K &key) { this->table.remove(key); }

  bool contains(const K &key) {
    return this->table.lookup(key) != this->table.end();
  }
  std::size_t get_size() { return this->table.get_size(); }

  struct iterator {
    using ht_iterator = typename hash_table<K, char>::iterator;
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